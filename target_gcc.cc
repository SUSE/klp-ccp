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

  opt_code_common_fassociative_math,
  opt_code_common_fexcess_precision,
  opt_code_common_ffinite_math_only,
  opt_code_common_fmath_errno,
  opt_code_common_freciprocal_math,
  opt_code_common_fsignaling_nans,
  opt_code_common_fsigned_zeros,
  opt_code_common_ftrapping_math,

  opt_code_common_funsafe_math_optimizations,
  opt_code_common_ffast_math,

  opt_code_common_ffp_contract,
  opt_code_common_fpermitted_flt_eval_methods,
  opt_code_common_fsingle_precision_constant,

  opt_code_common_fabi_version,
  opt_code_common_fleading_underscore,
  opt_code_common_finline,
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
  opt_code_c_family_fgnu89_inline,

  opt_code_c_family_ansi,

  opt_code_c_family_std_c90,
  opt_code_c_family_std_iso9899_199409,
  opt_code_c_family_std_c99,
  opt_code_c_family_std_c11,
  opt_code_c_family_std_c17,
  opt_code_c_family_std_c2x,

  opt_code_c_family_std_gnu90,
  opt_code_c_family_std_gnu99,
  opt_code_c_family_std_gnu11,
  opt_code_c_family_std_gnu17,
  opt_code_c_family_std_gnu2x,

  opt_code_c_family_std_cxx98,
  opt_code_c_family_std_cxx11,
  opt_code_c_family_std_cxx14,
  opt_code_c_family_std_cxx17,
  opt_code_c_family_std_cxx1y,
  opt_code_c_family_std_cxx1z,
  opt_code_c_family_std_cxx2a,

  opt_code_c_family_std_gnuxx98,
  opt_code_c_family_std_gnuxx11,
  opt_code_c_family_std_gnuxx14,
  opt_code_c_family_std_gnuxx17,
  opt_code_c_family_std_gnuxx1y,
  opt_code_c_family_std_gnuxx1z,
  opt_code_c_family_std_gnuxx2a,
};

static gcc_cmdline_parser::option gcc_opt_table_c_family[] = {
	#include "gcc_cmdline_opts_c_family.cc"
	{ nullptr }
};


target_gcc::target_gcc(const char * const version)
  : _gcc_version(_parse_version(version)),
    _opts_common(_gcc_version), _opts_c_family()
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
  _opts_c_family.c_lang_init_options(_gcc_version);
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

  _default_options_optimization(decoded_opts, p, *opt_table_arch);

  for (const auto &decoded_opt : decoded_opts)
    _handle_opt(decoded_opt.table, decoded_opt.o, decoded_opt.val,
		decoded_opt.negative, false, *opt_table_arch);

  if (_opts_common.base_file.empty()) {
    throw cmdline_except{"no input file"};
  }
  _finish_options();
}

void target_gcc::_default_options_optimization
	(const gcc_cmdline_parser::decoded_opts_type &decoded_opts,
	 const gcc_cmdline_parser &p,
	 const gcc_cmdline_parser::option &opt_table_arch)
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

  static const default_option default_options_table[] = {
    {
      default_option::opt_levels_1_plus, "fcombine-stack-adjustments",
      nullptr, false
    },
    { default_option::opt_levels_1_plus, "fcompare-elim", nullptr, false },
    { default_option::opt_levels_1_plus, "fcprop-registers", nullptr, false },
    { default_option::opt_levels_1_plus, "fdefer-pop", nullptr, false },
    { default_option::opt_levels_1_plus, "fforward-propagate", nullptr, false },
    {
      default_option::opt_levels_1_plus, "fguess-branch-probability",
      nullptr, false
    },
    { default_option::opt_levels_1_plus, "fipa-profile", nullptr, false },
    { default_option::opt_levels_1_plus, "fipa-pure-const", nullptr, false },
    { default_option::opt_levels_1_plus, "fipa-reference", nullptr, false },
    {
      default_option::opt_levels_1_plus, "fipa-reference-addressable",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    { default_option::opt_levels_1_plus, "fmerge-constants", nullptr, false },
    {
      default_option::opt_levels_1_plus, "fomit-frame-pointer", nullptr, false,
      .min_gcc_version = {8, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "freorder-blocks", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_1_plus, "freorder-blocks", nullptr, false,
      .min_gcc_version = {6, 1, 0},
    },
    { default_option::opt_levels_1_plus, "fshrink-wrap", nullptr, false },
    { default_option::opt_levels_1_plus, "fsplit-wide-types", nullptr, false },
    {
      default_option::opt_levels_2_plus, "ftree-builtin-call-dce",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_1_plus, "ftree-builtin-call-dce",
      nullptr, false,
      .min_gcc_version = {6, 1, 0},
    },
    { default_option::opt_levels_1_plus, "ftree-ccp", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-ch", nullptr, false },
    {
      default_option::opt_levels_1_plus, "ftree-coalesce-vars", nullptr, false,
      .min_gcc_version = {6, 1, 0},
    },
    { default_option::opt_levels_1_plus, "ftree-copy-prop", nullptr, false },
    {
      default_option::opt_levels_1_plus, "ftree-copy-rename", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
    },
    { default_option::opt_levels_1_plus, "ftree-dce", nullptr, false },
    {
      default_option::opt_levels_1_plus, "ftree-dominator-opts", nullptr, false
    },
    { default_option::opt_levels_1_plus, "ftree-dse", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-fre", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-sink", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-slsr", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-ter", nullptr, false },
    {
      default_option::opt_levels_1_plus_not_debug, "fbranch-count-reg",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_1_plus, "fif-conversion",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 2},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fif-conversion",
      nullptr, false,
      .min_gcc_version = {4, 8, 3},
      .max_gcc_version = {4, 8, 5},
    },
    {
      default_option::opt_levels_1_plus, "fif-conversion",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
      .max_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fif-conversion",
      nullptr, false,
      .min_gcc_version = {4, 9, 1},
    },
    {
      default_option::opt_levels_1_plus, "fif-conversion2",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 2},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fif-conversion2",
      nullptr, false,
      .min_gcc_version = {4, 8, 3},
      .max_gcc_version = {4, 8, 5},
    },
    {
      default_option::opt_levels_1_plus, "fif-conversion2",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
      .max_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fif-conversion2",
      nullptr, false,
      .min_gcc_version = {4, 9, 1},
    },
    {
      default_option::opt_levels_1_plus_not_debug,
      "finline-functions-called-once", nullptr, false
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fmove-loop-invariants",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fssa-phiopt",
      nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    {
      default_option::opt_levels_1_plus, "ftree-bit-ccp",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 9, 4},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "ftree-bit-ccp",
      nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "ftree-sra",
      nullptr, false
    },
    {
      default_option::opt_levels_1_plus_not_debug, "ftree-pta", nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    { default_option::opt_levels_2_plus, "fcaller-saves", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fcode-hoisting", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    { default_option::opt_levels_2_plus, "fcrossjumping", nullptr, false },
    { default_option::opt_levels_2_plus, "fcse-follow-jumps", nullptr, false },
    { default_option::opt_levels_2_plus, "fdevirtualize", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fdevirtualize-speculatively",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_2_plus, "fexpensive-optimizations",
      nullptr, false
    },
    { default_option::opt_levels_2_plus, "fgcse", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fhoist-adjacent-loads",
      nullptr, false
    },
    { default_option::opt_levels_2_plus, "findirect-inlining", nullptr, false },
    {
      default_option::opt_levels_2_plus, "finline-small-functions",
      nullptr, false
    },
    {
      default_option::opt_levels_2_plus, "fipa-bit-cp", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    { default_option::opt_levels_2_plus, "fipa-cp", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fipa-cp-alignment", nullptr, false,
      .min_gcc_version = {5, 1, 0},
      .max_gcc_version = {6, 5, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus, "fipa-icf", nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "fipa-ra", nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    { default_option::opt_levels_2_plus, "fipa-sra", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fipa-vrp", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "fisolate-erroneous-paths-dereference",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_2_plus, "flra-remat", nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "foptimize-sibling_calls",
      nullptr, false
    },
    { default_option::opt_levels_2_plus, "fpartial-inlining", nullptr, false },
    { default_option::opt_levels_2_plus, "fpeephole2", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fregmove", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 5},
    },
    { default_option::opt_levels_2_plus, "freorder-functions", nullptr, false },
    {
      default_option::opt_levels_2_plus, "frerun-cse-after-loop", nullptr, false
    },
    { default_option::opt_levels_2_plus, "fschedule-insns2", nullptr, false },
    { default_option::opt_levels_2_plus, "fstrict-aliasing", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fstrict-overflow", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus, "fstore-merging", nullptr, false,
      .min_gcc_version = {7, 1, std::numeric_limits<unsigned int>::max()},
    },
    { default_option::opt_levels_2_plus, "fthread-jumps", nullptr, false },
    { default_option::opt_levels_2_plus, "ftree-pre", nullptr, false },
    {
      default_option::opt_levels_2_plus, "ftree-switch-conversion",
      nullptr, false
    },
    { default_option::opt_levels_2_plus, "ftree-tail-merge", nullptr, false },
    { default_option::opt_levels_2_plus, "ftree-vrp", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fvect-cost-model", "cheap", false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_2_plus, "falign-functions",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "falign-functions",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "falign-jumps",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "falign-jumps",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "falign-labels",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "falign-labels",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "falign-loops",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "falign-loops",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "foptimize-strlen",
      nullptr, false
    },
    {
      default_option::opt_levels_2_plus_speed_only, "freorder-blocks-algorithm",
      "stc", false,
      .min_gcc_version = {6, 1, 0},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "fschedule-insns",
      nullptr, false
    },
    {
      default_option::opt_levels_3_plus_and_size, "finline-functions",
      nullptr, false
    },
    { default_option::opt_levels_3_plus, "fgcse-after-reload", nullptr, false },
    { default_option::opt_levels_3_plus, "fipa-cp-clone", nullptr, false },
    {
      default_option::opt_levels_3_plus, "floop-interchange", nullptr, false,
      .min_gcc_version = {8, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "floop-unroll-and-jam", nullptr, false,
      .min_gcc_version = {8, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "fpeel-loops", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "fpredictive-commoning", nullptr, false
    },
    {
      default_option::opt_levels_3_plus, "fsplit-loops", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "fsplit-paths", nullptr, false,
      .min_gcc_version = {6, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "ftree-loop-distribute-patterns",
      nullptr, false
    },
    {
      default_option::opt_levels_3_plus, "ftree-loop-distribution",
      nullptr, false,
      .min_gcc_version = {8, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "ftree-loop-vectorize", nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    { default_option::opt_levels_3_plus, "ftree-partial-pre", nullptr, false },
    {
      default_option::opt_levels_3_plus, "ftree-slp-vectorize", nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_3_plus, "ftree-vectorize", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 5},
    },
    { default_option::opt_levels_3_plus, "funswitch-loops", nullptr, false },
    {
      default_option::opt_levels_3_plus, "fvect-cost-model", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 5},
    },
    {
      default_option::opt_levels_3_plus, "fvect-cost-model", "dynamic", false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_3_plus, "fversion-loops-for-strides",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    { default_option::opt_levels_fast, "ffast-math", nullptr, false },

    { default_option::opt_levels_none }
  };

  _maybe_default_options(*default_options_table, p, opt_table_arch);
  _maybe_default_options(this->_arch_get_option_optimization_table(), p,
			 opt_table_arch);
}

void target_gcc::
_maybe_default_options(const struct default_option &table,
		       const gcc_cmdline_parser &p,
		       const gcc_cmdline_parser::option &opt_table_arch)
{
  for (const default_option *default_opt = &table;
       default_opt->levels != default_option::opt_levels_none; ++default_opt) {
    if (!(default_opt->min_gcc_version <= _gcc_version &&
	  _gcc_version <= default_opt->max_gcc_version)) {
      continue;
    }

    const gcc_cmdline_parser::option *o;
    const gcc_cmdline_parser::option *t;
    std::tie(o, t) = p.find_option(default_opt->name);
    assert(o);

    const unsigned int level = _opts_common.optimize;
    const bool size = _opts_common.optimize_size;
    const bool fast = _opts_common.optimize_fast;
    const bool debug = _opts_common.optimize_debug;

    bool enabled = false;
    switch (default_opt->levels) {
    case default_option::opt_levels_none:
      assert(0);
      __builtin_unreachable();
      break;

    case default_option::opt_levels_all:
      enabled = true;
      break;

    case default_option::opt_levels_0_only:
      enabled = (level == 0);
      break;

    case default_option::opt_levels_1_plus:
      enabled = (level >= 1);
      break;

    case default_option::opt_levels_1_plus_speed_only:
      enabled = (level >= 1 && !size && !debug);
      break;

    case default_option::opt_levels_1_plus_not_debug:
      enabled = (level >= 1 && !debug);
      break;

    case default_option::opt_levels_2_plus:
      enabled = (level >= 2);
      break;

    case default_option::opt_levels_2_plus_speed_only:
      enabled = (level >= 2 && !size && !debug);
      break;

    case default_option::opt_levels_3_plus:
      enabled = (level >= 3);
      break;

    case default_option::opt_levels_3_plus_and_size:
      enabled = (level >=3 || size);
      break;

    case default_option::opt_levels_size:
      enabled = size;
      break;

    case default_option::opt_levels_fast:
      enabled = fast;
      break;
    }

    if (enabled) {
      assert (!default_opt->negative || !o->reject_negative);
      _handle_opt(t, o, default_opt->val, default_opt->negative,
		  true, opt_table_arch);
    } else if (!default_opt->val && !o->reject_negative) {
      _handle_opt(t, o, nullptr, !default_opt->negative, true,
		  opt_table_arch);
    } else {
      // For our purposes default_opt->val should imply
      // o->reject_negative.
      assert(o->reject_negative);
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
  _opts_common.process_options(_gcc_version);
  _opts_c_family.process_options();
}

void target_gcc::_c_lang_post_options()
{
  // This corresponds to GCC's c_common_post_options().
  _opts_common.c_lang_post_options(_gcc_version, _opts_c_family);
  _opts_c_family.c_lang_post_options();
}

target_gcc::opts_common::opts_common(const gcc_cmdline_parser::gcc_version &ver)
  noexcept
  : optimize(0), optimize_debug(false), optimize_fast(false),
    optimize_size(false), flag_associative_math(false),
    flag_excess_precision(excess_precision::ep_default),
    flag_finite_math_only(false), flag_errno_math(true),
    flag_reciprocal_math(false), flag_signaling_nans(false),
    flag_signed_zeros(true), flag_trapping_math(true),
    flag_unsafe_math_optimizations(false),
    flag_fp_contract_mode(fp_contract_mode::fcm_fast),
    flag_fp_contract_mode_set(false),
    flag_permitted_flt_eval_methods(permitted_flt_eval_methods::pfem_default),
    flag_permitted_flt_eval_methods_set(false),
    flag_single_precision_constant(false),
    flag_abi_version(0), flag_leading_underscore(-1),
    flag_no_inline(false)
{
  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (!(gcc_version{5, 1, 0} <= ver)) {
    // GCC 4.8.x and 4.9.x used to initialize flag_abi_version to 2.
    flag_abi_version = 2;
  }
}

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

  case opt_code_common_fassociative_math:
    flag_associative_math = !negative;
    break;

  case opt_code_common_fexcess_precision:
    assert(val);
    if (!std::strcmp(val, "standard")) {
      flag_excess_precision = excess_precision::ep_standard;
    } else if (!std::strcmp(val, "fast")) {
      flag_excess_precision = excess_precision::ep_fast;
    } else {
      throw cmdline_except{"invalid parameter for -fexcess-precision"};
    }
    break;

  case opt_code_common_ffinite_math_only:
    flag_finite_math_only = !negative;
    break;

  case opt_code_common_fmath_errno:
    flag_errno_math = !negative;
    break;

  case opt_code_common_freciprocal_math:
    flag_reciprocal_math = !negative;
    break;

  case opt_code_common_fsignaling_nans:
    flag_signaling_nans = !negative;
    break;

  case opt_code_common_fsigned_zeros:
    flag_signed_zeros = !negative;
    break;

  case opt_code_common_ftrapping_math:
    flag_trapping_math = !negative;
    break;

  case opt_code_common_funsafe_math_optimizations:
    flag_unsafe_math_optimizations = !negative;
    _set_unsafe_math_optimizations_flags(!negative);
    break;

  case opt_code_common_ffast_math:
    _set_fast_math_flags(!negative);
    break;

  case opt_code_common_ffp_contract:
    if (!std::strcmp(val, "off") || !std::strcmp(val, "on")) {
      flag_fp_contract_mode = fp_contract_mode::fcm_off;
    } else if (!std::strcmp(val, "fast")) {
      flag_fp_contract_mode = fp_contract_mode::fcm_fast;
    } else {
      throw cmdline_except{"invalid value for -ffp-contract"};
    }
    if (!generated)
      flag_fp_contract_mode_set = true;
    break;

  case opt_code_common_fpermitted_flt_eval_methods:
    if (!std::strcmp(val, "c11")) {
      flag_permitted_flt_eval_methods = permitted_flt_eval_methods::pfem_c11;
    } else if (!std::strcmp(val, "ts-18661-3")) {
      flag_permitted_flt_eval_methods =
	permitted_flt_eval_methods::pfem_ts_18661;
    } else {
      throw cmdline_except{"invalid value for -fpermitted-flt-eval-methods"};
    }
    if (!generated)
      flag_permitted_flt_eval_methods_set = true;
    break;

  case opt_code_common_fsingle_precision_constant:
    flag_single_precision_constant = !negative;
    break;

  case opt_code_common_fabi_version:
    {
      std::size_t endpos;
      int _abi_version = -1;

      try {
	_abi_version = std::stoi(std::string{val}, &endpos);
      } catch (...) {}

      if (_abi_version < 0 || val[endpos] != '\0')
	throw cmdline_except{"invalid argument to -fabi-version"};

      flag_abi_version = static_cast<unsigned int>(_abi_version);
    }
    break;

  case opt_code_common_fleading_underscore:
    flag_leading_underscore = !negative;
    break;

  case opt_code_common_finline:
    flag_no_inline = negative;
    break;
  }
}

void target_gcc::opts_common::finish_options() noexcept
{
  // This gets called from target_gcc::_finish_options() which
  // corresponds to GCC's finish_options().

  if (optimize == 0)
    flag_no_inline = true;
}

void target_gcc::opts_common::
c_lang_post_options(const gcc_cmdline_parser::gcc_version &ver,
		    const opts_c_family &c_opts)
  noexcept
{
  // This gets called from target_gcc::_c_lang_post_options() which
  // corresponds to GCC's c_common_post_options().
  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (flag_excess_precision == excess_precision::ep_default) {
    if (c_opts.is_iso())
      flag_excess_precision = excess_precision::ep_standard;
    else
      flag_excess_precision = excess_precision::ep_fast;
  }

  if (gcc_version{4, 9, 0} <= ver) {
    if (c_opts.is_iso() &&
	!flag_fp_contract_mode_set &&
	!flag_unsafe_math_optimizations) {
      flag_fp_contract_mode = fp_contract_mode::fcm_off;
    }
  }

  // This code exists as of GCC 7.1.0. Before that, the
  // flag_permitted_flt_eval_methods variable hadn't existed.
  if (gcc_version{7, 1, 0} <= ver) {
    if (!c_opts.is_iso() &&
	!flag_permitted_flt_eval_methods_set) {
      flag_permitted_flt_eval_methods =
	permitted_flt_eval_methods::pfem_ts_18661;
    } else {
      flag_permitted_flt_eval_methods = permitted_flt_eval_methods::pfem_c11;
    }
  }

  if (gcc_version{8, 2, 0} <= ver) {
    const unsigned int latest_abi_version = 13;

    if (!flag_abi_version || flag_abi_version > latest_abi_version)
      flag_abi_version = latest_abi_version;

  } else if (gcc_version{8, 1, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 12;

  } else if (gcc_version{7, 1, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 11;

  } else if (gcc_version{6, 1, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 10;

  } else if (gcc_version{5, 2, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 9;

  } else if (gcc_version{5, 1, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 8;
  } else {
    // For GCC versions <= 4.9, leave a flag_abi_version == 0 as is.
    // Note that for these, flag_abi_version is initialized with '2',
    // hence the user would have to set a zero value
    // explicitly. Preprocessor defines are special cased on
    // flag_abi_version == 0 for these GCC versions.
  }
}

void target_gcc::opts_common::
process_options(const gcc_cmdline_parser::gcc_version &ver)
{
  // This gets called from target_gcc::_process_options() which
  // corresponds to GCC's process_options().
  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (flag_abi_version == 1 && gcc_version{5, 1, 0} <= ver)
    throw cmdline_except{"-fabi_version=1 only supported for gcc < 5.1.0"};

  if (flag_signaling_nans)
    flag_trapping_math = true;

  if (flag_associative_math && (flag_trapping_math || flag_signed_zeros))
    flag_associative_math = false;
}

bool target_gcc::opts_common::fast_math_flags_set_p() const noexcept
{
  return (!flag_trapping_math &&
	  flag_unsafe_math_optimizations &&
	  flag_finite_math_only &&
	  !flag_signed_zeros &&
	  !flag_errno_math &&
	  flag_excess_precision == excess_precision::ep_fast);
}

void
target_gcc::opts_common::_set_unsafe_math_optimizations_flags(const bool set)
  noexcept
{
  flag_trapping_math = !set;
  flag_signed_zeros = !set;
  flag_associative_math = !set;
  flag_reciprocal_math = !set;
}

void target_gcc::opts_common::_set_fast_math_flags(const bool set) noexcept
{
  flag_unsafe_math_optimizations = set;
  _set_unsafe_math_optimizations_flags(set);

  flag_finite_math_only = set;
  flag_errno_math = !set;

  if (set) {
    flag_excess_precision = excess_precision::ep_fast;
    flag_signaling_nans = false;
  }
}


target_gcc::opts_c_family::opts_c_family() noexcept
  : flag_undef(false), c_std(c_lang_kind::clk_gnuc89),
    flag_gnu89_inline(-1)
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

void target_gcc::opts_c_family::
c_lang_init_options(const gcc_cmdline_parser::gcc_version &ver)
  noexcept
{
  // This gets called from target_gcc::_c_lang_init_options() which
  // corresponds to GCC's c_common_init_options().
  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (gcc_version{8, 1, 0} <= ver)
    _set_std_c17(false);
  else if (gcc_version{5, 1, 0} <= ver)
    _set_std_c11(false);
  else
    _set_std_c89(false, false);
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

  case opt_code_c_family_fgnu89_inline:
    flag_undef = !negative;
    break;

  case opt_code_c_family_ansi:
    _set_std_c89(false, true);
    break;

  case opt_code_c_family_std_c90:
    _set_std_c89(false, true);
    break;

  case opt_code_c_family_std_iso9899_199409:
    _set_std_c89(true, true);
    break;

  case opt_code_c_family_std_c99:
    _set_std_c99(true);
    break;

  case opt_code_c_family_std_c11:
    _set_std_c11(true);
    break;

  case opt_code_c_family_std_c17:
    _set_std_c17(true);
    break;

  case opt_code_c_family_std_c2x:
    _set_std_c2x(true);
    break;

  case opt_code_c_family_std_gnu90:
    _set_std_c89(false, false);
    break;

  case opt_code_c_family_std_gnu99:
    _set_std_c99(false);
    break;

  case opt_code_c_family_std_gnu11:
    _set_std_c11(false);
    break;

  case opt_code_c_family_std_gnu17:
    _set_std_c17(false);
    break;

  case opt_code_c_family_std_gnu2x:
    _set_std_c2x(false);
    break;

  case opt_code_c_family_std_cxx98:
    /* fall through */
  case opt_code_c_family_std_cxx11:
    /* fall through */
  case opt_code_c_family_std_cxx14:
    /* fall through */
  case opt_code_c_family_std_cxx17:
    /* fall through */
  case opt_code_c_family_std_cxx1y:
    /* fall through */
  case opt_code_c_family_std_cxx1z:
    /* fall through */
  case opt_code_c_family_std_cxx2a:
    /* fall through */
  case opt_code_c_family_std_gnuxx98:
    /* fall through */
  case opt_code_c_family_std_gnuxx11:
    /* fall through */
  case opt_code_c_family_std_gnuxx14:
    /* fall through */
  case opt_code_c_family_std_gnuxx17:
    /* fall through */
  case opt_code_c_family_std_gnuxx1y:
    /* fall through */
  case opt_code_c_family_std_gnuxx1z:
    /* fall through */
  case opt_code_c_family_std_gnuxx2a:
    throw cmdline_except{"C++ standards not supported"};
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
  if (flag_gnu89_inline == -1) {
    flag_gnu89_inline = !this->is_std_geq_c99();
  } else if (!flag_gnu89_inline && !this->is_std_geq_c99()) {
    throw cmdline_except{
      "-fno-gnu89-inline is only supported in GNU99 or C99 mode"
    };
  }
}

void target_gcc::opts_c_family::process_options() noexcept
{
  // This gets called from target_gcc::_process_options() which
  // corresponds to GCC's process_options().
}

bool target_gcc::opts_c_family::is_iso() const noexcept
{
  switch (c_std) {
  case c_lang_kind::clk_stdc89:
    /* fall through */
  case c_lang_kind::clk_stdc94:
    /* fall through */
  case c_lang_kind::clk_stdc99:
    /* fall through */
  case c_lang_kind::clk_stdc11:
    /* fall through */
  case c_lang_kind::clk_stdc17:
    /* fall through */
  case c_lang_kind::clk_stdc2x:
    return true;

  case c_lang_kind::clk_gnuc89:
    /* fall through */
  case c_lang_kind::clk_gnuc99:
    /* fall through */
  case c_lang_kind::clk_gnuc11:
    /* fall through */
  case c_lang_kind::clk_gnuc17:
    /* fall through */
  case c_lang_kind::clk_gnuc2x:
    return false;
  }
}

bool target_gcc::opts_c_family::is_std_geq_c99() const noexcept
{
  switch (c_std) {
  case c_lang_kind::clk_stdc89:
    /* fall through */
  case c_lang_kind::clk_gnuc89:
    /* fall through */
    return false;

  case c_lang_kind::clk_stdc94:
    /* fall through */
  case c_lang_kind::clk_stdc99:
    /* fall through */
  case c_lang_kind::clk_stdc11:
    /* fall through */
  case c_lang_kind::clk_stdc17:
    /* fall through */
  case c_lang_kind::clk_stdc2x:
    /* fall through */
  case c_lang_kind::clk_gnuc99:
    /* fall through */
  case c_lang_kind::clk_gnuc11:
    /* fall through */
  case c_lang_kind::clk_gnuc17:
    /* fall through */
  case c_lang_kind::clk_gnuc2x:
    return true;
  }
}

void target_gcc::opts_c_family::_set_std_c89(const bool c94, const bool iso)
  noexcept
{
  if (c94) {
    c_std = c_lang_kind::clk_stdc94;
    return;
  }

  c_std = iso ? c_lang_kind::clk_stdc89 : c_lang_kind::clk_gnuc89;
}

void target_gcc::opts_c_family::_set_std_c99(const bool iso) noexcept
{
  c_std = iso ? c_lang_kind::clk_stdc99 : c_lang_kind::clk_gnuc99;
}

void target_gcc::opts_c_family::_set_std_c11(const bool iso) noexcept
{
  c_std = iso ? c_lang_kind::clk_stdc11 : c_lang_kind::clk_gnuc11;
}

void target_gcc::opts_c_family::_set_std_c17(const bool iso) noexcept
{
  c_std = iso ? c_lang_kind::clk_stdc17 : c_lang_kind::clk_gnuc17;
}

void target_gcc::opts_c_family::_set_std_c2x(const bool iso) noexcept
{
  c_std = iso ? c_lang_kind::clk_stdc2x : c_lang_kind::clk_gnuc2x;
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
