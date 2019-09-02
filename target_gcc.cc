/*
 * Copyright (C) 2019  SUSE Software Solutions Germany GmbH
 *
 * This file is part of klp-ccp.
 *
 * klp-ccp is free software: you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * klp-ccp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstring>
#include "cmdline_except.hh"
#include "pp_except.hh"
#include "preprocessor.hh"
#include "target_gcc.hh"

using namespace klp::ccp;


static gcc_cmdline_parser::option gcc_opts_common[] = {
	#include "gcc_cmdline_opts_common.cc"
	{ nullptr }
};

static gcc_cmdline_parser::option gcc_opts_c_family[] = {
	#include "gcc_cmdline_opts_c_family.cc"
	{ nullptr }
};


target_gcc::target_gcc(const char * const version)
  : _gcc_version(_parse_version(version))
{}

target_gcc::~target_gcc() noexcept = default;

void target_gcc::parse_command_line
		(int argc, const char *argv[],
		 header_resolver &hr,
		 preprocessor &pp,
		 const std::function<void(const std::string&)> &report_warning)
{
  if (!argv) {
    // This only happens for the testsuite programs. Provide a
    // default set of #defines.
    pp.register_builtin_macro("__STDC__", "1");
    pp.register_builtin_macro("__STDC_HOSTED__", "1");
    pp.register_builtin_macro("__STDC_VERSION__", "199901L");
    _register_builtin_macros(pp);
    return;
  }

  gcc_cmdline_parser p{_gcc_version};
  p.register_table(gcc_opts_common);
  p.register_table(gcc_opts_c_family);
  const gcc_cmdline_parser::option * const arch_opts = this->_arch_get_opts();
  if (arch_opts)
    p.register_table(arch_opts);

  const char *_base_file = nullptr;
  std::vector<const char *> pre_includes;

  std::vector<const char *> include_dirs;
  std::vector<const char *> include_dirs_quoted;
  std::vector<const char *> include_dirs_system;
  std::vector<const char *> include_dirs_after;

  bool undef = false;

  struct macro_def_or_undef
  {
    bool undef;
    const char *arg;
  };
  std::vector<macro_def_or_undef> macro_defs_and_undefs;

  bool optimize = false;

  auto &&handle_opt =
    [&](const gcc_cmdline_parser::option * const o,
	const gcc_cmdline_parser::option * const table,
	const char *val, const bool negative) {
      if (!o) {
	if (_base_file) {
	  throw cmdline_except{
		  std::string{"more than one input file: '"} + _base_file +
		  "' and '" + val + "'"
		};
	}

	_base_file = val;
	return;
      }

      if (!std::strcmp(o->name, "include")) {
	pre_includes.push_back(val);
	return;
      }

      if (!std::strcmp(o->name, "I")) {
	if (!std::strcmp(val, "-")) {
	  include_dirs_quoted.insert
	    (include_dirs_quoted.end(),
	     std::make_move_iterator(include_dirs.begin()),
	     std::make_move_iterator(include_dirs.end()));
	  include_dirs.clear();
	  include_dirs.shrink_to_fit();
	} else {
	  include_dirs.push_back(val);
	}
	return;
      }

      if (!std::strcmp(o->name, "iquote")) {
	include_dirs_quoted.push_back(val);
	return;
      }

      if (!std::strcmp(o->name, "isystem")) {
	include_dirs_after.push_back(val);
	return;
      }

      if (!std::strcmp(o->name, "idirafter")) {
	include_dirs_after.push_back(val);
	return;
      }

      if (!std::strcmp(o->name, "undef")) {
	undef = true;
	return;
      }

      if (!std::strcmp(o->name, "D")) {
	macro_defs_and_undefs.emplace_back(macro_def_or_undef{false, val});
	return;
      }

      if (!std::strcmp(o->name, "U")) {
	macro_defs_and_undefs.emplace_back(macro_def_or_undef{true, val});
	return;
      }

      if (!std::strcmp(o->name, "O")) {
	if (val && !strcmp(val, "0"))
	  optimize = false;
	else
	  optimize = true;
	return;
      }
    };

  p(argc, argv, handle_opt);

  if (!_base_file) {
    throw cmdline_except{"no input file"};
  }

  for (const auto dir : include_dirs_quoted)
    hr.append_search_dir_quoted(dir);

  for (const auto dir : include_dirs)
    hr.append_search_dir(dir);

  for (const auto dir : include_dirs_system)
    hr.append_search_dir(dir);

  for (const auto dir : include_dirs_after)
    hr.append_search_dir(dir);

  const std::string base_file{_base_file};

  for (const auto i : pre_includes) {
    std::string resolved = hr.resolve(i, base_file, header_resolver::cwd);
    if (resolved.empty()) {
      throw cmdline_except{
	      std::string{"file '"} + i + "' not found"
	    };
    }

    pp.add_root_source(resolved, true);
  }

  pp.add_root_source(base_file, false);
  pp.set_base_file(base_file);

  pp.register_builtin_macro("__STDC__", "1");
  pp.register_builtin_macro("__STDC_HOSTED__", "1");
  pp.register_builtin_macro("__STDC_VERSION__", "199901L");

  if (!undef) {
    _register_builtin_macros(pp);
    if (optimize)
      pp.register_builtin_macro("__OPTIMIZE__", "1");
  }

  for (const auto &m : macro_defs_and_undefs) {
    if (!m.undef) {
      const auto &_report_warning =
	[&](const std::string &msg) {
	  report_warning(std::string{"predefined macro '"} +
			 m.arg + "': " + msg);
	};

      const char * const repl = std::strchr(m.arg, '=');
      if (!repl) {
	try {
	  pp.register_predefined_macro(m.arg, "1", _report_warning);
	} catch (const pp_except &e) {
	  throw cmdline_except{
		  std::string{"failed to parse predefined macro '"} +
		  m.arg + "': " + e.what()
		};
	}

      } else {
	const std::string signature{m.arg, repl};
	try {
	  pp.register_predefined_macro(signature, repl + 1, _report_warning);
	} catch (const pp_except &e) {
	  throw cmdline_except{
		  std::string{"failed to parse predefined macro '"} +
		  m.arg + "': " + e.what()
		};
	}
      }

    } else {
      const auto &_report_warning =
	[&](const std::string &msg) {
	  report_warning(std::string{"macro undef '"} +
			 m.arg + "': " + msg);
	};

      try {
	pp.register_predefined_macro_undef(m.arg, _report_warning);
      } catch (const pp_except &e) {
	throw cmdline_except{
		std::string{"failed to parse macro undef '"} +
		m.arg + "': " + e.what()
	      };
      }
    }
  }
}

gcc_cmdline_parser::gcc_version
target_gcc::_parse_version(const char * const version)
{
  const char * const pmajor = version;
  const char *pminor = std::strchr(pmajor, '.');
  const char *ppatchlevel = nullptr;

  if (pminor) {
    ++pminor;
    ppatchlevel = std::strchr(pminor, '.');
    if (ppatchlevel)
      ++ppatchlevel;
  }

  if (!ppatchlevel ||
      (pminor == pmajor + 1) ||
      (ppatchlevel == pminor + 1) ||
      (*ppatchlevel == '\0')) {
    throw cmdline_except {
		(std::string{"compiler version specifier \'"}
		 + version + "\' has invalid format")
	  };
  }

  std::size_t endpos;
  int major = std::stoi(std::string{pmajor, pminor - 1}, &endpos);
  if (*(pmajor + endpos) != '.')
    major = -1;

  int minor = std::stoi(std::string{pminor, ppatchlevel - 1}, &endpos);
  if (*(pminor + endpos) != '.')
    minor = -1;

  int patchlevel = std::stoi(std::string{ppatchlevel}, &endpos);
  if (*(ppatchlevel + endpos) != '\0')
    patchlevel = -1;

  if (major < 0 || minor < 0 || patchlevel < 0) {
    throw cmdline_except {
		(std::string{"compiler version specifier \'"}
		 + version + "\' has invalid format")
	  };
  }

  using gcc_version = gcc_cmdline_parser::gcc_version;
  const gcc_version parsed_version(major, minor, patchlevel);
  constexpr gcc_version MIN_SUPP_VER_48{4, 8, 0};
  constexpr gcc_version MAX_SUPP_VER_48{4, 8, 5};
  constexpr gcc_version MIN_SUPP_VER_49{4, 9, 0};
  constexpr gcc_version MAX_SUPP_VER_49{4, 9, 4};
  // It looks like from 5.1.0 onwards, the upstream patchlevel is
  // always set to zero and may be used freely by distributions.
  constexpr gcc_version MIN_SUPP_VER_5{5, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_5{
    5, 5, std::numeric_limits<unsigned int>::max()
  };
  constexpr gcc_version MIN_SUPP_VER_6{6, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_6{
    6, 5, std::numeric_limits<unsigned int>::max()
  };
  constexpr gcc_version MIN_SUPP_VER_7{7, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_7{
    7, 4, std::numeric_limits<unsigned int>::max()
  };
  constexpr gcc_version MIN_SUPP_VER_8{8, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_8{
    8, 3, std::numeric_limits<unsigned int>::max()
  };
  constexpr gcc_version MIN_SUPP_VER_9{9, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_9{
    9, 2, std::numeric_limits<unsigned int>::max()
  };

  auto &&in_range = [&](const gcc_version &f, const gcc_version &l) {
    return (f <= parsed_version && parsed_version <= l);
  };

  if (!in_range(MIN_SUPP_VER_48, MAX_SUPP_VER_48) &&
      !in_range(MIN_SUPP_VER_49, MAX_SUPP_VER_49) &&
      !in_range(MIN_SUPP_VER_5, MAX_SUPP_VER_5) &&
      !in_range(MIN_SUPP_VER_6, MAX_SUPP_VER_6) &&
      !in_range(MIN_SUPP_VER_7, MAX_SUPP_VER_7) &&
      !in_range(MIN_SUPP_VER_8, MAX_SUPP_VER_8) &&
      !in_range(MIN_SUPP_VER_9, MAX_SUPP_VER_9)) {
    throw cmdline_except {
		(std::string{"unrecognized compiler version \'"}
		 + version + "\'")
	  };
  }

  return parsed_version;
}

void target_gcc::_register_builtin_macros(preprocessor &pp) const
{
  const std::initializer_list<std::pair<const char *, const char*>>
    builtin_object_macros = {
	{ "__unix__", "1" },
	{ "__unix", "1" },
	{ "__linux__", "1" },
	{ "__linux", "1" },

	{ "__GCC_ATOMIC_TEST_AND_SET_TRUEVAL", "1" },
	{ "__ATOMIC_RELAXED", "0" },
	{ "__ATOMIC_CONSUME", "1" },
	{ "__ATOMIC_ACQUIRE", "2" },
	{ "__ATOMIC_RELEASE", "3" },
	{ "__ATOMIC_ACQ_REL", "4" },
	{ "__ATOMIC_SEQ_CST", "5" },
  };

  pp.register_builtin_macro("__GNUC__", std::to_string(_gcc_version.maj));
  pp.register_builtin_macro("__GNUC_MINOR__", std::to_string(_gcc_version.min));
  pp.register_builtin_macro("__GNUC_PATCHLEVEL__",
			    std::to_string(_gcc_version.patchlevel));

  for (const auto &bom : builtin_object_macros)
    pp.register_builtin_macro(bom.first, bom.second);

  this->_arch_register_builtin_macros(pp);
}
