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
				       pp_token::type::ws>;

  auto ws_pred =
    pp_token::is_type_any_of_predicate<pp_token::type::ws>;

  auto empty_pred =
    pp_token::is_type_any_of_predicate<pp_token::type::empty>;

  iterator it_seq_begin = std::find_if(_tokens.begin(), _tokens.end(),
					  ws_or_empty_pred);
  while(it_seq_begin != _tokens.end()) {
    iterator it_seq_end =
      std::find_if_not(it_seq_begin, _tokens.end(), ws_or_empty_pred);

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
    // There's never more than one whitespace.
    assert(it_ws == it_seq_end || it_ws + 1 == it_seq_end);
    if (it_seq_begin == _tokens.begin() && it_ws == it_seq_begin) {
      // Beginning of token sequence, and it's all whitespace w/o any
      // empties. Strip it.
      it_seq_end = _tokens.erase(it_ws, it_seq_end);

    } else if (it_seq_end == _tokens.end() && it_ws == it_seq_begin) {
      // End of token sequence, and it's all whitespace w/o any
      // empties. Strip it.
	_tokens.erase(it_ws, it_seq_end);
	return;
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

used_macro_undefs pp_tokens::get_used_macro_undefs() const
{
  used_macro_undefs result;

  for (auto tok : _tokens) {
    result += tok.used_macro_undefs();
  }

  return result;
}

std::string pp_tokens::stringify(const bool as_string) const
{
  std::string s;
  for (auto &&it = _tokens.begin(); it != _tokens.end(); ++it) {
    if (it + 1 == _tokens.end() && it->is_ws())
      break;
    s += it->stringify();
  }

  if (as_string)
    s = __escape_string(std::move(s));

  return s;
}
