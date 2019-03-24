#include <ostream>
#include "code_remark_pp.hh"
#include "pp_tokens.hh"
#include "pp_tracking.hh"

using namespace klp::ccp;


code_remark_pp::code_remark_pp(const severity sev, const std::string &msg,
			       const pp_tokens &tokens,
			       const pp_tokens_range &range)
  : _msg(msg), _tokens(tokens), _range(range), _sev(sev)
{}

code_remark_pp::code_remark_pp(const severity sev, const std::string &msg,
			       const pp_tokens &tokens,
			       const pp_token_index first,
			       const pp_token_index last)
  : code_remark_pp(sev, msg, tokens, pp_tokens_range{first, last + 1})
{}

code_remark_pp::code_remark_pp(const severity sev, const std::string &msg,
			       const pp_tokens &tokens,
			       const pp_token_index tok_index)
  : code_remark_pp(sev, msg, tokens, tok_index, tok_index)
{}

std::ostream& klp::ccp::operator<<(std::ostream &o,
				   const code_remark_pp &remark)
{
  o << ((remark.get_severity() == code_remark_pp::severity::fatal)
	? "error" : "warning");
  o << ": ";

  const pp_tokens &tokens = remark._tokens;
  const pp_tokens_range &range = remark._range;
  assert(range.end - range.begin >= 1);
  const raw_pp_tokens_range raw_range {
	tokens[remark._range.begin].get_token_source().begin,
	tokens[remark._range.end - 1].get_token_source().end,
  };
  const raw_pp_tokens &raw_tokens = tokens.get_pp_tracking().get_raw_tokens();
  if (raw_range.end == raw_range.begin + 1) {
    // Single token, print only its start location.
    const raw_pp_token &tok = raw_tokens[raw_range.begin];
    const file_range &tok_fr = tok.get_file_range();
    const file_range fr{tok_fr.get_inclusion_node(), tok_fr.get_start_loc()};
    o << fr;

  } else {
    // Raw_Range spans more than one token. Handle the case where they're
    // from different input files.
    const raw_pp_token &first_tok = raw_tokens[raw_range.begin];
    const file_range &first_tok_fr = first_tok.get_file_range();
    const raw_pp_token &last_tok = raw_tokens[raw_range.end - 1];
    const file_range &last_tok_fr = last_tok.get_file_range();

    if (&first_tok_fr.get_inclusion_node() ==
	&last_tok_fr.get_inclusion_node()) {
      const file_range fr{first_tok_fr.get_inclusion_node(),
			  first_tok_fr.get_start_loc(),
			  last_tok_fr.get_end_loc()};
      o << fr;

    } else {
      const file_range fr_begin{first_tok_fr.get_inclusion_node(),
				first_tok_fr.get_start_loc()};
      const file_range fr_end{last_tok_fr.get_inclusion_node(),
			      last_tok_fr.get_start_loc()};
      o << fr_begin << " - " << fr_end;

    }
  }

  o << ": " << remark._msg << std::endl;
  return o;
}
