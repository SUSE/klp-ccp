#include <ostream>
#include "code_remark_pp.hh"
#include "pp_token.hh"
#include "raw_pp_token.hh"
#include "pp_tokens.hh"
#include "pp_result.hh"

using namespace klp::ccp;


code_remark_pp::code_remark_pp(const severity sev, const std::string &msg,
			       const pp_result &pp_result,
			       const pp_tokens_range &range)
  : _msg(msg), _sev(sev)
{
  const pp_tokens &tokens = pp_result.get_pp_tokens();

  // Initialize _fr and _fr_end to be used from operator<<().  How to
  // set them depends on whether the range spans more than one token
  // and whether these tokens are coming from source multiple files or
  // not.
  assert(range.end - range.begin >= 1);
  const raw_pp_tokens_range raw_range {
	tokens[range.begin].get_token_source().begin,
	tokens[range.end - 1].get_token_source().end,
  };

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

code_remark_pp::code_remark_pp(const severity sev, const std::string &msg,
			       const pp_result &pp_result,
			       const pp_token_index first,
			       const pp_token_index last)
  : code_remark_pp(sev, msg, pp_result, pp_tokens_range{first, last + 1})
{}

code_remark_pp::code_remark_pp(const severity sev, const std::string &msg,
			       const pp_result &pp_result,
			       const pp_token_index tok_index)
  : code_remark_pp(sev, msg, pp_result, tok_index, tok_index)
{}

std::ostream& klp::ccp::operator<<(std::ostream &o,
				   const code_remark_pp &remark)
{
  o << ((remark.get_severity() == code_remark_pp::severity::fatal)
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
