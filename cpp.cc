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
#include "preprocessor.hh"
#include "cmdline_except.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "semantic_except.hh"
#include "target_x86_64_gcc.hh"

using namespace klp::ccp;

static const char prog_name[] = "klp-ccp";

static const char optstr[] = ":hc:";

static const option longopts[] {
	{ "help", 0, nullptr, 'h' },
	{ "compiler", 1, nullptr, 'c' },
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
    << "\t\tTarget compiler specification." << std::endl
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


int main(int argc, char* argv[])
{
  bool do_help = false;
  char *o_compiler = nullptr;
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
  preprocessor pp{hr, *tgt};
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
    return 1;
  }

  const pp_tokens &tokens = pp.get_result().get_pp_tokens();
  while(true) {
    try {
      auto &tok = tokens[pp.read_next_token()];
      if (!pp.get_remarks().empty()) {
	std::cerr << pp.get_remarks();
	pp.get_remarks().clear();
      }

      if (tok.is_eof())
	break;

      std::cout << tok.stringify();

    } catch (const pp_except&) {
	std::cerr << pp.get_remarks();
	return 1;
    } catch (const parse_except&) {
      std::cerr << pp.get_remarks();
      return 2;
    } catch (const semantic_except&) {
      std::cerr << pp.get_remarks();
      return 3;
    }
  }

  return 0;
}
