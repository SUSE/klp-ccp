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

#include <unistd.h>
#include <getopt.h>
#include <cstring>
#include <iostream>
#include <vector>
#include "target_x86_64_gcc.hh"
#include "cmdline_except.hh"
#include "header_resolver.hh"
#include "preprocessor.hh"
#include "gnuc_parser_driver.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "semantic_except.hh"
#include "lp_except.hh"
#include "lp_creation_python_policy.hh"
#include "output_remarks.hh"
#include "create_lp.hh"

using namespace klp::ccp;

static const char prog_name[] = "klp-ccp";

static const char optstr[] = ":hc:o:i:P:";

static const option longopts[] {
	{ "help", 0, nullptr, 'h' },
	{ "compiler", 1, nullptr, 'c' },
	{ "outfile", 1, nullptr, 'o' },
	{ "patched-functions", 1, nullptr, 'i' },
	{ "python-policy", 1, nullptr, 'P' },
	{ nullptr, 0, nullptr, 0 }
};

static void show_usage(const char * const prog)
{
  std::cerr << "See '" << prog << " --help' for usage." << std::endl;
}

static void show_help(const char * const prog)
{
  std::cout
    << "Usage: " << prog
    << " OPTIONS"
    << " -- ORIGINAL COMPILER COMMAND LINE"
    << std::endl;

  std::cout << "Options:" << std::endl;
  std::cout
    << " -c, --compiler=ARCH-COMPILER-VERSION"
    << "\t\tTarget compiler specification."
    << std::endl;
  std::cout
    << " -o, --outfile=FILE"
    << "\t\t\t\tOutput file."
    << std::endl;
  std::cout
    << " -i, --patched-functions=LIST"
    << "\t\t\tComma separated list of patched" << std::endl
    << "\t\t\t\t\t\tfunctions."
    << std::endl;
  std::cout
    << " -P, --python-policy=MOD"
    << "\tFully qualified python class name implementing" << std::endl
    << "\t\t\t\t\t\tlivepatch creation policies." << std::endl
    << std::endl;

  std::cout
    << "ORIGINAL COMPILER COMMAND LINE"
    << "\tdenotes the compiler invocation from the live" << std::endl
    << "\t\t\t\tpatching target's original compilation, without" << std::endl
    << "\t\t\t\tthe program name."
    << std::endl << std::endl;
  std::cout
    << "For further information, please see"
    << " <https://github.com/suse/klp-ccp>."
    << std::endl;
}

static void show_opt_duplicate(const char * const prog,
			       const char * const o)
{
  std::cerr << "command line error: "
	    << " option '" << o << "' specified more than once"
	    << std::endl;
  show_usage(prog);
}

static void show_opt_missing(const char * const prog,
			       const char * const o)
{
  std::cerr << "command line error: "
	    << " option '" << o << "' is required"
	    << std::endl;
  show_usage(prog);
}


static void split_patched_funs(const char *arg, std::vector<std::string> &funs)
{
  while (true) {
    const char *delim = std::strchr(arg, ',');
    if (!delim) {
      delim = arg + std::strlen(arg);
    }

    const auto& f = std::use_facet<std::ctype<char>>(std::locale());
    while (arg != delim &&
	   f.is(std::ctype_base::blank, *arg)) {
      ++arg;
    }

    const char *cur_end = delim;
    while (cur_end != arg &&
	   f.is(std::ctype_base::blank, *(cur_end - 1))) {
      --cur_end;
    }

    if (arg != cur_end)
      funs.emplace_back(arg, cur_end);

    if (*delim == '\0')
      break;

    arg = delim + 1;
  }
}

int main(int argc, char *argv[], char *envp[])
{
  bool do_help = false;
  char *o_compiler = nullptr;
  const char *o_outfile = nullptr;
  std::vector<std::string> patched_funs;
  const char *o_python_policy_fq_cls = nullptr;

  int o;
  int longindex = -1;
  while (optind < argc && std::strcmp(argv[optind], "--") &&
	 (o = getopt_long(argc, argv, optstr, longopts, &longindex)) != -1) {
    switch (o) {
    case 'h':
      do_help = true;
      break;

    case 'c':
      if (o_compiler) {
	show_opt_duplicate(prog_name, "--compiler");
	return 1;
      }
      o_compiler = optarg;
      break;

    case 'o':
      if (o_outfile) {
	show_opt_duplicate(prog_name, "--outfile");
	return 1;
      }
      o_outfile = optarg;
      break;

    case 'i':
      split_patched_funs(optarg, patched_funs);
      break;

    case 'P':
      if (o_python_policy_fq_cls) {
	show_opt_duplicate(prog_name, "--python-policy");
	return 1;
      }
      o_python_policy_fq_cls = optarg;
      break;

    case '?':
      std::cerr << "command line error: invalid option '";
      if (optopt)
	std::cerr << '-' << static_cast<char>(optopt);
      else
	std::cerr << argv[optind - 1];
      std::cerr << '\'' << std::endl;
      show_usage(prog_name);
      return 1;

    case ':':
      std::cerr << "command line error: missing parameter for option '";
      if (optopt)
	std::cerr << '-' << static_cast<char>(optopt);
      else
	std::cerr << longopts[longindex].name;
      std::cerr << '\'' << std::endl;
      show_usage(prog_name);
      return 1;
    }
  }

  if (do_help) {
    show_help(prog_name);
    return 0;
  }

  if (optind != argc && !std::strcmp(argv[optind], "--"))
    ++optind;

  if (optind == argc) {
    std::cerr << "command line error: missing compiler invocation" << std::endl;
    show_usage(prog_name);
    return 1;
  }

  if (!o_compiler) {
    show_opt_missing(prog_name, "--compiler");
    return 1;
  }

  char *o_compiler_version = std::strrchr(o_compiler, '-');
  if (!o_compiler_version || !std::strlen(o_compiler_version + 1)) {
    std::cerr
      << "command line error: compiler version specification missing in '"
      << o_compiler << '\'' << std::endl;
    show_usage(prog_name);
    return 1;
  }
  *o_compiler_version = '\0';
  ++o_compiler_version;


  if (!o_outfile) {
    show_opt_missing(prog_name, "--outfile");
    return 1;
  }

  if (!o_python_policy_fq_cls) {
    show_opt_missing(prog_name, "--python-policy");
    return 1;
  }
  const char * o_python_policy_cls =
    std::strrchr(o_python_policy_fq_cls, '.');
  if (!o_python_policy_cls) {
    std::cerr << "command line error: "
	      << "expected fully qualified \"MOD.CLS\" for "
	      << "--python-policy"
	      << std::endl;
    return 1;
  }
  const std::string o_python_policy_mod =
    std::string{o_python_policy_fq_cls, o_python_policy_cls};
  ++o_python_policy_cls;

  std::unique_ptr<target> tgt;
  if (!strcmp(o_compiler, "x86_64-gcc")) {
    try {
      tgt.reset(new target_x86_64_gcc{o_compiler_version});
    } catch (const cmdline_except &e) {
      std::cerr << "command line error: " << e.what() << std::endl;
      show_usage(prog_name);
      return 1;
    }
  } else {
    std::cerr << "command line error: "
	      << "unrecognized compiler specification '"
	      << o_compiler << '\'' << std::endl;
    show_usage(prog_name);
    return 1;
  }

  header_resolver hr;
  preprocessor pp(hr, *tgt);
  try {
    tgt->parse_command_line(argc - optind,
			    const_cast<const char **>(&argv[optind]),
			    hr, pp,
			    [&](const std::string &warning) {
			      std::cerr << "compiler command line warning: "
					<< warning << std::endl;
			    });
  } catch (const cmdline_except &e) {
    std::cerr << "compiler command line error: "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }


  int r = 0;
  yy::gnuc_parser_driver pd{std::move(pp), *tgt};
  try {
    pd.parse();

  } catch (const pp_except &) {
    r = 2;
  } catch (const parse_except&) {
    r = 2;
  } catch (const semantic_except&) {
    r = 2;
  }

  if (!pd.get_remarks().empty())
    std::cerr << pd.get_remarks();
  if (r)
    return r;


  ast::ast_translation_unit ast(pd.grab_result());
  try {
    ast.resolve(*tgt);
  } catch (const semantic_except&) {
    r = 2;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  try {
    ast.evaluate(*tgt);
  } catch (const semantic_except&) {
    r = 2;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  code_remarks iremarks;
  output_remarks oremarks;
  try {
    lp_creation_python_policy pol{
      ast.get_pp_result(),
      std::move(patched_funs),
      o_python_policy_mod.c_str(),
      o_python_policy_cls
    };

    create_lp(o_outfile, ast, pol, iremarks, oremarks);

  } catch (const lp_except&) {
    r = 3;

  } catch (const std::system_error &e) {
    std::cerr << "error: " << e.what() << std::endl;
    r = 4;

  } catch (const python_except &e) {
    std::cerr << "error: python policy failure: " << e.what() << std::endl;
    handle_python_except();
    r = 5;

  }

  // Teardown python only now after exceptions had a chance to print
  // their message. It's bad code architecture, but the API for
  // embedding Python is what it is, i.e. AFAICT there's no way to
  // obtain a formatted error message and print it ourselves.
  lp_creation_python_policy::teardown_python();

  if (!iremarks.empty())
    std::cerr << iremarks;
  iremarks.clear();
  if (r)
    return r;

  if (!oremarks.empty())
    std::cerr << oremarks;
  oremarks.clear();

  return r;
}
