#include <algorithm>
#include <cassert>
#include "pp_tokens.hh"
#include "pp_token.hh"

using namespace suse::cp;

static std::string __escape_string(std::string &&s)
{
  for (auto &&it = s.begin(); it != s.end();) {
    if (*it == '"' || *it == '\\') {
      it = s.insert(it, '\\');
      it += 2;
    } else {
      ++it;
    }
  }

  return std::move(s);
}

void pp_tokens::normalize_ws()
{
  auto ws_or_empty_pred =
    pp_token::is_type_any_of_predicate<pp_token::type::empty,
				       pp_token::type::ws,
				       pp_token::type::newline>;

  auto ws_pred =
    pp_token::is_type_any_of_predicate<pp_token::type::ws,
				       pp_token::type::newline>;

  auto empty_pred =
    pp_token::is_type_any_of_predicate<pp_token::type::empty>;

  iterator it_seq_begin = std::find_if(_tokens.begin(), _tokens.end(),
					  ws_or_empty_pred);
  while(it_seq_begin != _tokens.end()) {
    iterator it_seq_end =
      std::find_if_not(it_seq_begin + 1, _tokens.end(), ws_or_empty_pred);

    // Move any whitespace or newline to after the empty tokens while
    // preserving the order of the empties.
    auto it_ws = std::find_if(it_seq_begin, it_seq_end, ws_pred);
    auto it_pred = std::find_if(it_ws, it_seq_end, empty_pred);
    while (it_pred != it_seq_end) {
      assert(ws_pred(*it_ws));
      assert(empty_pred(*it_pred));
      std::swap(*it_ws++, *it_pred++);
      if (it_pred != it_seq_end && !empty_pred(*it_pred)) {
	it_pred = std::find_if(it_pred + 1, it_seq_end, empty_pred);
      }
    }

    // Now, it_ws points right to the beginning of the trailing
    // whitespace sequence.
    assert(it_ws == it_seq_end || ws_pred(*it_ws));
    if (it_seq_begin == _tokens.begin() || it_seq_end == _tokens.end()) {
      // Beginning or end of token sequence, purge the whitespace.
      _tokens.erase(it_ws, it_seq_end);
      return;
    } else if (it_ws != it_seq_end) {
      // There is some whitespace and it's not at the beginning or end.
      // Keep a single representative.
      assert(std::all_of(it_ws, it_seq_end,
			 [&it_ws](const pp_token &tok) {
			   return (tok.used_macros().empty() &&
				   (tok.get_file_range() ==
				    it_ws->get_file_range()));
			 }));
      it_ws->set_type_and_value(pp_token::type::ws, " ");
      it_seq_end = _tokens.erase(it_ws + 1, it_seq_end);
    }

    it_seq_begin = std::find_if(it_seq_end, _tokens.end(), ws_or_empty_pred);
  }
}

used_macros pp_tokens::get_used_macros() const
{
  used_macros result;

  for (auto tok : _tokens) {
    result += tok.used_macros();
  }

  return result;
}

std::string pp_tokens::stringify(const bool as_string) const
{
  std::string s;
  for (auto &&it = _tokens.begin(); it != _tokens.end(); ++it)
    s += it->stringify();

  if (as_string)
    s = __escape_string(std::move(s));

  return s;
}
