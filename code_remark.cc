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

#include <ostream>
#include "code_remark.hh"
#include "pp_token.hh"
#include "raw_pp_token.hh"
#include "pp_tokens.hh"
#include "pp_result.hh"

using namespace klp::ccp;


code_remark::code_remark(const severity sev, const std::string &msg,
			 const pp_result &pp_result,
			 const pp_tokens_range &range)
  : code_remark(sev, msg, pp_result, pp_result.pp_tokens_range_to_raw(range))
{}

code_remark::code_remark(const severity sev, const std::string &msg,
			 const pp_result &pp_result,
			 const pp_token_index first,
			 const pp_token_index last)
  : code_remark(sev, msg, pp_result, pp_tokens_range{first, last + 1})
{}

code_remark::code_remark(const severity sev, const std::string &msg,
			       const pp_result &pp_result,
			       const pp_token_index tok_index)
  : code_remark(sev, msg, pp_result, tok_index, tok_index)
{}

code_remark::code_remark(const severity sev, const std::string &msg,
			 const pp_result &pp_result,
			 const raw_pp_tokens_range &raw_range)
  : _msg(msg), _sev(sev)
{
  auto is_it = pp_result.intersecting_sources_begin(raw_range);
  const auto is_end
    = pp_result.intersecting_sources_end(raw_range);
  _first_file = &*is_it;
  _last_file = &*is_it;
  for (++is_it; is_it != is_end; ++is_it)
    _last_file = &*is_it;

  const raw_pp_tokens &raw_tokens = pp_result.get_raw_tokens();
  if (raw_range.end == raw_range.begin + 1) {
    // Single token, print only its start location.
    const raw_pp_token &tok = raw_tokens[raw_range.begin];
    const range_in_file &tok_rif = tok.get_range_in_file();
    _begin_loc = _end_loc = tok_rif.begin;

  } else {
    // Raw range spans more than one token.
    const raw_pp_token &first_tok = raw_tokens[raw_range.begin];
    const range_in_file &first_tok_rif = first_tok.get_range_in_file();
    const raw_pp_token &last_tok = raw_tokens[raw_range.end - 1];
    const range_in_file &last_tok_rif = last_tok.get_range_in_file();

    _begin_loc = first_tok_rif.begin;
    _end_loc = last_tok_rif.end;
  }
}

code_remark::code_remark(const severity sev, const std::string &msg,
			 const pp_result::header_inclusion_node &file,
			 const range_in_file &range_in_file)
  : _first_file(&file), _last_file(&file),
    _begin_loc(range_in_file.begin), _end_loc(range_in_file.end),
    _msg(msg), _sev(sev)
{}

std::ostream& klp::ccp::operator<<(std::ostream &o, const code_remark &remark)
{
  o << ((remark.get_severity() == code_remark::severity::fatal)
	? "error" : "warning");
  o << ": ";

  const std::pair<std::streamoff, std::streamoff> begin_line_col
    = remark._first_file->offset_to_line_col(remark._begin_loc);
  if (remark._first_file == remark._last_file) {
    o << remark._first_file->get_filename();
    if (remark._begin_loc == remark._end_loc) {
      o << ": line " << begin_line_col.first
	<< ", column " << begin_line_col.second;
    } else {
      const std::pair<std::streamoff, std::streamoff> end_line_col
	= remark._first_file->offset_to_line_col(remark._end_loc);
      if (begin_line_col.first == end_line_col.first) {
	assert(begin_line_col.second != end_line_col.second);
	o << ": line " << begin_line_col.first
	  << ", columns " << begin_line_col.second
	  << '-' << end_line_col.second;
      } else {
      o << ": line " << begin_line_col.first
	<< ", column " << begin_line_col.second
	<< " to line " << end_line_col.first
	<< ", column " << end_line_col.second;
      }
    }

  } else {
    const std::pair<std::streamoff, std::streamoff> end_line_col
      = remark._last_file->offset_to_line_col(remark._end_loc);
    o << remark._first_file->get_filename()
      << ": line " << begin_line_col.first
      << ", column " << begin_line_col.second
      << " - "
      << remark._last_file->get_filename()
      << ": line " << end_line_col.first
      << ", column " << end_line_col.second;
  }

  o << ": " << remark._msg << std::endl;
  return o;
}
