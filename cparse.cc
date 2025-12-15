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
#include "gnuc_parser_driver.hh"
#include "header_resolver.hh"
#include "parse_except.hh"
#include "pp_except.hh"
#include "semantic_except.hh"
#include "target/cmdline_except.hh"
#include "target/gcc/x86_64/target_x86_64_gcc.hh"

using namespace klp::ccp;

static const option options[] = {
  { "include", true, nullptr, 'i' },
  { "help", true, nullptr, 'h' },
  {}
};

static const char optstring[] = "h";

static void print_help()
{
  std::cerr << "usage: "
	    << "cparse [-include <file>] [-h] <file>" << std::endl;
}

static void print_usage()
{
  std::cerr << "error: invalid command line" << std::endl;
}

int main(int argc, char* argv[])
{
  int r = 0;
  std::vector<const char*> pre_includes;

  while(true) {
    const int o = getopt_long(argc, argv, optstring, options, NULL);

    if (o == -1)
      break;

    switch (o) {
    case '?':
      print_usage();
      return 1;

    case 'h':
      print_help();
      return 0;

    case 'i':
      pre_includes.push_back(optarg);
      break;
    }
  }

  if (optind != argc - 1) {
    print_usage();
    return 1;
  }

  std::vector<const char*> gcc_argv;
  const char include_argv_str[] = "-include";
  for (const auto &pi : pre_includes) {
    gcc_argv.push_back(include_argv_str);
    gcc_argv.push_back(pi);
  }
  gcc_argv.push_back(argv[optind]);

  header_resolver hr;
  target_x86_64_gcc tgt{"4.8.5"};
  preprocessor p{hr, tgt};
  try {
    tgt.parse_command_line(gcc_argv.size(), &gcc_argv[0], hr, p,
			   [](const std::string&) {
			     assert(0);
			     __builtin_unreachable();
			   });
  } catch (const cmdline_except &e) {
    std::cerr << "internal compiler command line error: "
	      << e.what() << std::endl;
    return 1;
  }

  yy::gnuc_parser_driver pd{std::move(p), tgt};

  try {
    pd.parse();
  } catch (const pp_except&) {
    r = 2;
  } catch (const parse_except&) {
    r = 3;
  } catch (const semantic_except&) {
    r = 4;
  }

  if (!pd.get_remarks().empty())
    std::cerr << pd.get_remarks();
  if (r)
    return r;

  ast::ast_translation_unit ast(pd.grab_result());
  try {
    ast.resolve(tgt);
  } catch (const semantic_except&) {
    r = 5;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  try {
    ast.evaluate(tgt);
  } catch (const semantic_except&) {
    r = 6;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  return r;
}
