/*
 * Copyright (C) 2026  SUSE Software Solutions Germany GmbH
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

#include <dejagnu.h>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include "raw_pp_directive.hh"
#include "code_remarks.hh"
#include "pp_except.hh"

using namespace klp::ccp;

typedef raw_pp_directive_kind kind;
typedef std::vector<std::tuple<std::size_t, std::size_t, kind>> lines_type;

// Scans the given contents from a temporary file. Returns false if
// scanning failed.
static bool scan(const std::string &contents, lines_type &lines)
{
  char name[] = "/tmp/test_raw_pp_directive.XXXXXX";
  const int fd = mkstemp(name);
  if (fd < 0)
    std::abort();
  close(fd);
  {
    std::ofstream o{name, std::ios::binary};
    o << contents;
  }

  bool ok = true;
  lines.clear();
  {
    pp_result::header_inclusion_root hir{name, false};
    code_remarks remarks;
    try {
      for (const auto &l : scan_raw_pp_logical_lines(hir, remarks))
	lines.emplace_back(l.first_line, l.end_line, l.directive_kind);
    } catch (const pp_except&) {
      ok = false;
    }
  }
  unlink(name);
  return ok;
}

static void check(TestState &s, const std::string &name,
		  const std::string &contents, const lines_type &expected)
{
  lines_type lines;
  if (scan(contents, lines) && lines == expected)
    s.pass(name);
  else
    s.fail(name);
}

int main()
{
  TestState s;

  check(s, "empty file", "", {});
  check(s, "plain lines", "a\n\nb\n",
	{{0, 1, kind::none}, {1, 2, kind::none}, {2, 3, kind::none}});
  check(s, "no final newline", "a\nb",
	{{0, 1, kind::none}, {1, 2, kind::none}});
  check(s, "continuation", "a \\\n  b\nc\n",
	{{0, 2, kind::none}, {2, 3, kind::none}});
  check(s, "multi-line comment", "a /* x\n y\n */ b\nc\n",
	{{0, 3, kind::none}, {3, 4, kind::none}});
  check(s, "comment only lines", "// x\n/* y\n */\n",
	{{0, 1, kind::none}, {1, 3, kind::none}});
  check(s, "conditional directives",
	"#if A\n#ifdef B\n#ifndef C\n#elif D\n#elifdef E\n#elifndef F\n"
	"#else\n#endif\n",
	{{0, 1, kind::if_}, {1, 2, kind::ifdef}, {2, 3, kind::ifndef},
	 {3, 4, kind::elif}, {4, 5, kind::elifdef}, {5, 6, kind::elifndef},
	 {6, 7, kind::else_}, {7, 8, kind::endif}});
  check(s, "other directives",
	"#error \"x\"\n#define X 1\n#include <y.h>\n#pragma once\n#\n",
	{{0, 1, kind::error}, {1, 2, kind::other}, {2, 3, kind::other},
	 {3, 4, kind::other}, {4, 5, kind::null}});
  check(s, "directive spelling",
	"  #  if A\n/* c */ # else\n%:endif\n#/* c */error\n",
	{{0, 1, kind::if_}, {1, 2, kind::else_}, {2, 3, kind::endif},
	 {3, 4, kind::error}});
  check(s, "directive continued",
	"#if 1 + /*\n  blah\n */ 1\n#\\\nendif\nA\n",
	{{0, 3, kind::if_}, {3, 5, kind::endif}, {5, 6, kind::none}});
  check(s, "not a directive", "a # if\n\"#if\"\n",
	{{0, 1, kind::none}, {1, 2, kind::none}});

  lines_type lines;
  if (!scan("a\n/* unterminated\n", lines))
    s.pass("unterminated comment");
  else
    s.fail("unterminated comment");

  return 0;
}
