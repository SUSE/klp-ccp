#include <ostream>
#include "code_remark_pp.hh"
#include "pp_tokens.hh"

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
  if (range.end == range.begin + 1) {
    // Single token, print only its start location.
    const pp_token &tok = tokens[range.begin];
    const file_range &tok_fr = tok.get_file_range();
    const file_range fr{tok_fr.get_inclusion_node(), tok_fr.get_start_loc()};
    o << fr;

  } else {
    // Range spans more than one token. Handle the case where they're
    // from different input files.
    const pp_token &first_tok = tokens[range.begin];
    const file_range &first_tok_fr = first_tok.get_file_range();
    const pp_token &last_tok = tokens[range.end - 1];
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
