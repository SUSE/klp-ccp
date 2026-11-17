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

#include <cassert>
#include "raw_pp_directive.hh"
#include "pp_tokenizer.hh"
#include "pp_except.hh"
#include "code_remarks.hh"

using namespace klp::ccp;

bool klp::ccp::raw_pp_directive_opens_cond_incl(const raw_pp_directive_kind k)
  noexcept
{
  return (k == raw_pp_directive_kind::if_ ||
	  k == raw_pp_directive_kind::ifdef ||
	  k == raw_pp_directive_kind::ifndef);
}

bool
klp::ccp::raw_pp_directive_continues_cond_incl(const raw_pp_directive_kind k)
  noexcept
{
  return (k == raw_pp_directive_kind::elif ||
	  k == raw_pp_directive_kind::elifdef ||
	  k == raw_pp_directive_kind::elifndef ||
	  k == raw_pp_directive_kind::else_);
}

static raw_pp_directive_kind directive_kind_from_name(const std::string &name)
{
  static const struct
  {
    const char *name;
    raw_pp_directive_kind kind;
  } names[] = {
    { "if", raw_pp_directive_kind::if_ },
    { "ifdef", raw_pp_directive_kind::ifdef },
    { "ifndef", raw_pp_directive_kind::ifndef },
    { "elif", raw_pp_directive_kind::elif },
    { "elifdef", raw_pp_directive_kind::elifdef },
    { "elifndef", raw_pp_directive_kind::elifndef },
    { "else", raw_pp_directive_kind::else_ },
    { "endif", raw_pp_directive_kind::endif },
    { "error", raw_pp_directive_kind::error },
  };

  for (const auto &n : names) {
    if (name == n.name)
      return n.kind;
  }
  return raw_pp_directive_kind::other;
}

std::vector<raw_pp_logical_line>
klp::ccp::scan_raw_pp_logical_lines(pp_result::header_inclusion_node &file,
				    code_remarks &remarks)
{
  // The tokenizer takes care of backslash line continuations and
  // turns each comment into a single whitespace token, even if it
  // spans multiple lines. So the logical lines are exactly the token
  // sequences inbetween newline tokens. Their physical line ranges
  // are obtained from the tokens' offsets, which the file maps to
  // line numbers as the tokenizer reads it.
  //
  // A directive's '#' must be the first token on a logical line other
  // than whitespace and the empty tokens marking line continuations,
  // and the directive's name is the first identifier following it,
  // again skipping these.
  enum class state
  {
    // Nothing but whitespace seen on the current logical line so far.
    line_start,
    // The '#' starting a directive has been seen, but not its name.
    directive_start,
    // The rest of the logical line doesn't matter.
    rest,
  };

  pp_tokenizer tokenizer{file};
  std::vector<raw_pp_logical_line> lines;
  state s = state::line_start;
  bool in_line = false;
  range_in_file::loc_type line_begin = 0;
  raw_pp_directive_kind kind = raw_pp_directive_kind::none;

  const auto finish_line =
    [&](const range_in_file::loc_type end) {
      // The physical line containing the last character, i.e. the
      // newline character, if any, is the logical line's last one.
      assert(end > line_begin);
      const std::size_t first =
	static_cast<std::size_t>(file.offset_to_line_col(line_begin).first);
      const std::size_t last =
	static_cast<std::size_t>(file.offset_to_line_col(end - 1).first);
      assert(first >= 1 && last >= first);
      if (s == state::directive_start)
	kind = raw_pp_directive_kind::null;
      lines.push_back(raw_pp_logical_line{first - 1, last, kind});
      s = state::line_start;
      in_line = false;
      kind = raw_pp_directive_kind::none;
    };

  try {
    while (true) {
      const raw_pp_token tok = tokenizer.read_next_token();
      if (tok.is_eof()) {
	if (in_line && tok.get_range_in_file().begin > line_begin)
	  finish_line(tok.get_range_in_file().begin);
	break;
      }

      if (!in_line) {
	in_line = true;
	line_begin = tok.get_range_in_file().begin;
      }

      if (tok.is_newline()) {
	finish_line(tok.get_range_in_file().end);
	continue;
      } else if (tok.is_ws() || tok.is_empty()) {
	continue;
      }

      switch (s) {
      case state::line_start:
	if (tok.is_punctuator("#") || tok.is_punctuator("%:")) {
	  s = state::directive_start;
	} else {
	  s = state::rest;
	}
	break;

      case state::directive_start:
	kind = (tok.is_id()
		? directive_kind_from_name(tok.get_value())
		: raw_pp_directive_kind::other);
	s = state::rest;
	break;

      case state::rest:
	break;
      }
    }
  } catch (const pp_except&) {
    remarks += tokenizer.get_remarks();
    throw;
  }

  remarks += tokenizer.get_remarks();
  return lines;
}
