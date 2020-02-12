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

enum opt_code_common
{
  opt_code_common_unused = 0,

  opt_code_common_O,
  opt_code_common_Ofast,
  opt_code_common_Og,
  opt_code_common_Os,
};

static gcc_cmdline_parser::option gcc_opt_table_common[] = {
	#include "gcc_cmdline_opts_common.cc"
	{ nullptr }
};

enum opt_code_c_family
{
  opt_code_c_family_unused = 0,

  opt_code_c_family_D,
  opt_code_c_family_I,
  opt_code_c_family_U,
  opt_code_c_family_idirafter,
  opt_code_c_family_include,
  opt_code_c_family_iquote,
  opt_code_c_family_isystem,
  opt_code_c_family_undef,
};

static gcc_cmdline_parser::option gcc_opt_table_c_family[] = {
	#include "gcc_cmdline_opts_c_family.cc"
	{ nullptr }
};


target_gcc::target_gcc(const char * const version)
  : _gcc_version(_parse_version(version)),
    _opts_common(), _opts_c_family()
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

  _init_options_struct();
  _c_lang_init_options_struct();
  _c_lang_init_options();
  _decode_options(argc, argv, report_warning);
  _process_options();

  for (const auto dir : _opts_c_family.include_dirs_quoted)
    hr.append_search_dir_quoted(dir);

  for (const auto dir : _opts_c_family.include_dirs)
    hr.append_search_dir(dir);

  for (const auto dir : _opts_c_family.include_dirs_system)
    hr.append_search_dir(dir);

  for (const auto dir : _opts_c_family.include_dirs_after)
    hr.append_search_dir(dir);

  for (const auto i : _opts_c_family.pre_includes) {
    std::string resolved = hr.resolve(i, _opts_common.base_file,
				      header_resolver::cwd);
    if (resolved.empty()) {
      throw cmdline_except{
	      std::string{"file '"} + i + "' not found"
	    };
    }

    pp.add_root_source(resolved, true);
  }

  pp.add_root_source(_opts_common.base_file, false);
  pp.set_base_file(_opts_common.base_file);

  pp.register_builtin_macro("__STDC__", "1");
  pp.register_builtin_macro("__STDC_HOSTED__", "1");
  pp.register_builtin_macro("__STDC_VERSION__", "199901L");

  if (!_opts_c_family.flag_undef) {
    _register_builtin_macros(pp);
    if (_opts_common.optimize)
      pp.register_builtin_macro("__OPTIMIZE__", "1");
    if (_opts_common.optimize_size)
      pp.register_builtin_macro("__OPTIMIZE_SIZE__", "1");
  }

  for (const auto &m : _opts_c_family.macro_defs_and_undefs) {
    if (!m.undef) {
      const auto &_report_warning =
	[&](const std::string &msg) {
	  report_warning(std::string{"predefined macro '"} +
			 m.arg + "': " + msg);
	};

      const std::string::size_type repl_pos = m.arg.find('=');
      if (repl_pos == std::string::npos) {
	try {
	  pp.register_predefined_macro(m.arg, "1", _report_warning);
	} catch (const pp_except &e) {
	  throw cmdline_except{
		  std::string{"failed to parse predefined macro '"} +
		  m.arg + "': " + e.what()
		};
	}

      } else {
	const std::string signature{m.arg.cbegin(),
				    m.arg.cbegin() + repl_pos};
	const std::string repl{m.arg.cbegin() + repl_pos + 1,
			       m.arg.cend()};
	try {
	  pp.register_predefined_macro(signature, repl, _report_warning);
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

void target_gcc::_init_options_struct() noexcept
{
  // This corresponds to GCC's language agnostic
  // init_options_struct().
  _opts_common.init_options_struct();
  _opts_c_family.init_options_struct();
  this->_arch_option_init_struct();
}

void target_gcc::_c_lang_init_options_struct() noexcept
{
  // This corresponds to GCC's c_common_init_options_struct()
  _opts_common.c_lang_init_options_struct();
  _opts_c_family.c_lang_init_options_struct();
}

void target_gcc::_c_lang_init_options() noexcept
{
  // Thist corresponds to GCC's c_common_init().
  _opts_common.c_lang_init_options_struct();
  _opts_c_family.c_lang_init_options();
}

void target_gcc::
_decode_options(int argc, const char *argv[],
		const std::function<void(const std::string&)> &report_warning)
{
  gcc_cmdline_parser p{_gcc_version};
  p.register_table(gcc_opt_table_common);
  p.register_table(gcc_opt_table_c_family);
  const gcc_cmdline_parser::option * const opt_table_arch =
    this->_arch_get_opt_table();
  if (opt_table_arch)
    p.register_table(opt_table_arch);

  const gcc_cmdline_parser::decoded_opts_type decoded_opts{p(argc, argv)};

  _default_options_optimization(decoded_opts);

  for (const auto &decoded_opt : decoded_opts)
    _handle_opt(decoded_opt.table, decoded_opt.o, decoded_opt.val,
		decoded_opt.negative, false, *opt_table_arch);

  if (_opts_common.base_file.empty()) {
    throw cmdline_except{"no input file"};
  }
  _finish_options();
}

void target_gcc::_default_options_optimization
	(const gcc_cmdline_parser::decoded_opts_type &decoded_opts)
{
  // This corresponds to GCC's default_options_optimization().
  // Pre-scan for the optimization options as these imply default
  // settings, c.f. GCC's default_options_optimization().
  for (const auto &decoded_opt : decoded_opts) {
    if (decoded_opt.table == gcc_opt_table_common) {
      switch (decoded_opt.o->code) {
      case opt_code_common_O:
	_opts_common.optimize_size = false;
	_opts_common.optimize_fast = false;
	_opts_common.optimize_debug = false;
	if (!decoded_opt.val) {
	  _opts_common.optimize = 1;
	} else {
	  int _optimize = -1;
	  std::size_t endpos;

	  try {
	    _optimize = std::stoi(std::string{decoded_opt.val}, &endpos);
	  } catch (...) {}

	  if (_optimize < 0 || decoded_opt.val[endpos] != '\0')
	    throw cmdline_except{"invalid argument to -O"};
	  else if (_optimize > 255)
	    _optimize = 255;

	  _opts_common.optimize = static_cast<unsigned int>(_optimize);
	}
	break;

      case opt_code_common_Ofast:
	_opts_common.optimize_size = false;
	_opts_common.optimize = 3;
	_opts_common.optimize_fast = true;
	_opts_common.optimize_debug = false;
	break;

      case opt_code_common_Og:
	_opts_common.optimize_size = false;
	_opts_common.optimize = 1;
	_opts_common.optimize_fast = false;
	_opts_common.optimize_debug = true;;
	break;

      case opt_code_common_Os:
	_opts_common.optimize_size = true;
	_opts_common.optimize = 2;
	_opts_common.optimize_fast = false;
	_opts_common.optimize_debug = false;
	break;
      }
    }
  }
}

void target_gcc::
_handle_opt(const gcc_cmdline_parser::option * const opt_table,
	    const gcc_cmdline_parser::option * const o,
	    const char *val, const bool negative,
	    const bool generated,
	    const gcc_cmdline_parser::option &opt_table_arch)
{
  if (!o || opt_table == gcc_opt_table_common) {
    _opts_common.handle_opt(o, val, negative, generated, _gcc_version);
  } else if (opt_table == gcc_opt_table_c_family) {
    _opts_c_family.handle_opt(o, val, negative, generated, _gcc_version);
  } else {
    assert(opt_table == &opt_table_arch);
    this->_arch_handle_opt(o, val, negative, generated);
  }
}

void target_gcc::_finish_options()
{
  // This corresponds to GCC's finish_options().
  _opts_common.finish_options();
  _opts_c_family.finish_options();
}

void target_gcc::_process_options()
{
  // This corresponds to GCC's process_options().
  _c_lang_post_options();
  this->_arch_option_override();
  _opts_common.process_options();
  _opts_c_family.process_options();
}

void target_gcc::_c_lang_post_options()
{
  // This corresponds to GCC's c_common_post_options().
  _opts_common.c_lang_post_options();
  _opts_c_family.c_lang_post_options();
}

target_gcc::opts_common::opts_common() noexcept
  : optimize(0), optimize_debug(false), optimize_fast(false),
    optimize_size(false)
{}

void target_gcc::opts_common::init_options_struct() noexcept
{
  // This gets called from target_gcc::_init_options_struct() which
  // corresponds to GCC's language agnostic init_options_struct().
}

void target_gcc::opts_common::c_lang_init_options_struct() noexcept
{
  // This gets called from target_gcc::_c_lang_init_options_struct()
  // which corresponds to GCC's c_common_init_options_struct().
}

void target_gcc::opts_common::c_lang_init_options() noexcept
{
  // This gets called from target_gcc::_c_lang_init_options()
  // which corresponds to GCC's c_common_init_options().
}

void target_gcc::opts_common::
handle_opt(const gcc_cmdline_parser::option * const o,
	   const char *val, const bool negative,
	   const bool generated,
	   const gcc_cmdline_parser::gcc_version &ver)
{
  if (!o) {
    if (!base_file.empty()) {
      throw cmdline_except{
	std::string{"more than one input file: '"} +
		    base_file + "' and '" + val + "'"
		   };
    }

    base_file = val;
    return;
  }

  switch (o->code) {
  case opt_code_common_unused:
    break;

  case opt_code_common_O:
    // Handled in pre-scan
    break;

  case opt_code_common_Ofast:
    // Handled in pre-scan
    break;

  case opt_code_common_Og:
    // Handled in pre-scan
    break;

  case opt_code_common_Os:
    // Handled in pre-scan
    break;
  }
}

void target_gcc::opts_common::finish_options() noexcept
{
  // This gets called from target_gcc::_finish_options() which
  // corresponds to GCC's finish_options().
}

void target_gcc::opts_common::c_lang_post_options() noexcept
{
  // This gets called from target_gcc::_c_lang_post_options() which
  // corresponds to GCC's c_common_post_options().
}

void target_gcc::opts_common::process_options()
{
  // This gets called from target_gcc::_process_options() which
  // corresponds to GCC's process_options().
}


target_gcc::opts_c_family::opts_c_family() noexcept
  : flag_undef(false)
{}

void target_gcc::opts_c_family::init_options_struct() noexcept
{
  // This gets called from target_gcc::_init_options_struct() which
  // corresponds to GCC's language agnostic init_options_struct().
}

void target_gcc::opts_c_family::c_lang_init_options_struct() noexcept
{
  // This gets called from target_gcc::_c_lang_init_options_struct()
  // which corresponds to GCC's c_common_init_options_struct().
}

void target_gcc::opts_c_family::c_lang_init_options() noexcept
{
  // This gets called from target_gcc::_c_lang_init_options() which
  // corresponds to GCC's c_common_init_options().
}

void target_gcc::
opts_c_family::handle_opt(const gcc_cmdline_parser::option * const o,
			  const char *val, const bool negative,
			  const bool generated,
			  const gcc_cmdline_parser::gcc_version &ver)
{
  switch (o->code) {
  case opt_code_c_family_unused:
    break;

  case opt_code_c_family_D:
    macro_defs_and_undefs.emplace_back(macro_def_or_undef{false, val});
    break;

  case opt_code_c_family_I:
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
    break;

  case opt_code_c_family_U:
    macro_defs_and_undefs.emplace_back(macro_def_or_undef{true, val});
    break;

  case opt_code_c_family_idirafter:
    include_dirs_after.push_back(val);
    break;

  case opt_code_c_family_include:
    pre_includes.push_back(val);
    break;

  case opt_code_c_family_iquote:
    include_dirs_quoted.push_back(val);
    break;

  case opt_code_c_family_isystem:
    include_dirs_after.push_back(val);
    break;

  case opt_code_c_family_undef:
    flag_undef = true;
    break;
  }
}

void target_gcc::opts_c_family::finish_options() noexcept
{
  // This gets called from target_gcc::_finish_options() which
  // corresponds to GCC's finish_options().
}

void target_gcc::opts_c_family::c_lang_post_options()
{
  // This gets called from target_gcc::_c_lang_post_options() which
  // corresponds to GCC's c_common_post_options().
}

void target_gcc::opts_c_family::process_options() noexcept
{
  // This gets called from target_gcc::_process_options() which
  // corresponds to GCC's process_options().
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
