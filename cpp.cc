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

static const char optstr[] = ":hc:a";

static const option longopts[] {
	{ "help", 0, nullptr, 'h' },
	{ "compiler", 1, nullptr, 'c' },
	{ "annotate", 1, nullptr, 'a' },
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
    << " -a, --annotate"
    << "\t\t\t\t\tEmit source location annotations." << std::endl
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
  bool annotate = false;
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

    case 'a':
      annotate = true;
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

  const auto &ppr = pp.get_result();
  const pp_tokens &tokens = ppr.get_pp_tokens();
  while(true) {
    try {
      const auto &tok = tokens[pp.read_next_token()];
      if (!pp.get_remarks().empty()) {
	std::cerr << pp.get_remarks();
	pp.get_remarks().clear();
      }

      if (tok.is_eof())
	break;

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

  const auto &raw_tokens = ppr.get_raw_tokens();
  const auto &emit_src_annotation =
    [&](const pp_result::header_inclusion_node &h,
	const raw_pp_token_index index_raw) {
      const auto &r = h.get_range();
      assert((index_raw == r.begin && r.begin == r.end)
	     || index_raw < raw_tokens.size());
      assert((raw_pp_tokens_range{index_raw,
				 r.begin == r.end ? index_raw : index_raw + 1}
	      .is_contained_in(r)));
      const auto offset_in_file =
	r.begin != r.end ? raw_tokens[index_raw].get_range_in_file().begin : 0;
      const auto line = h.offset_to_line_col(offset_in_file).first;

      if (!annotate)
	return;

      std::cout << "#line " << line << " \"" << h.get_filename() << '"'
		<< std::endl;
    };

  pp_token_index tok_index = 0;
  raw_pp_token_index raw_tok_index = 0;
  auto emit_tokens_until =
    [&](const raw_pp_token_index end) {
      for (; tok_index < tokens.size(); ++tok_index) {
	const auto &tok = tokens[tok_index];
	if (tok.get_token_source().begin >= end)
	  break;
	std::cout << tok.stringify();
      }
    };

  auto it_src =
    ppr.intersecting_sources_begin(raw_pp_tokens_range{0, raw_tokens.size()});
  auto it_src_end =
    ppr.intersecting_sources_end(raw_pp_tokens_range{0, raw_tokens.size()});
  if (it_src == it_src_end)
    return 0;

  while (true) {
    const pp_result::header_inclusion_node &h = *it_src;
    ++it_src;

    const auto &cur_range = h.get_range();
    if (it_src == it_src_end) {
      emit_src_annotation(h, raw_tok_index);
      emit_tokens_until(cur_range.end);
      break;

    }

    const auto &next_range = it_src->get_range();
    if (next_range.is_contained_in(cur_range)) {
      emit_src_annotation(h, raw_tok_index);
      emit_tokens_until(next_range.begin);
      raw_tok_index = next_range.begin;
    } else {
      assert(cur_range.is_contained_in(next_range) ||
	     cur_range.end == next_range.begin);
      emit_src_annotation(h, raw_tok_index);
      emit_tokens_until(cur_range.end);
      raw_tok_index = cur_range.end;
    }
  }

  return 0;
}
