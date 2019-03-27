#include <stdexcept>
#include <set>
#include <utility>
#include <cassert>
#include <algorithm>
#include "macro.hh"
#include "pp_token.hh"
#include "raw_pp_token.hh"
#include "pp_except.hh"

using namespace klp::ccp;

macro::macro(const std::string &name,
	     const bool func_like,
	     const bool variadic,
	     std::vector<std::string> &&arg_names,
	     raw_pp_tokens &&repl,
	     const raw_pp_tokens_range &directive_range,
	     std::shared_ptr<const macro_undef> &&prev_macro_undef)
  : _name(name), _arg_names(std::move(arg_names)),
    _repl(std::move(repl)),
    _directive_range(directive_range),
    _prev_macro_undef(prev_macro_undef),
    _func_like(func_like),
    _variadic(variadic)
{
  // Due to the restriction that a ## concatenated token must again
  // yield a valid preprocessing token, macro evaluation can fail and
  // thus yield an error. Hence macro arguments must not get macro
  // expanded if not needed. An argument needs to get expanded if it
  // appears in the replacement list, ignoring operands of ## and
  // #. Determine those.
  std::set<std::string> do_expand;
  bool in_concat = false;
  for (auto it = _repl.cbegin(); it != _repl.cend();) {
    if (it->is_id() &&
	(std::find(_arg_names.cbegin(), _arg_names.cend(), it->get_value())
	 != _arg_names.cend())) {
      if (_is_concat_op(it + 1)) {
	in_concat = true;
	it += 2;
      } else if (in_concat) {
	in_concat = false;
	it = _next_non_ws_repl(it + 1);
      } else {
	do_expand.insert(it->get_value());
	it = _next_non_ws_repl(it + 1);
      }
    } else {
      it = _skip_stringification_or_single(it);
      if (_is_concat_op(it)) {
	in_concat = true;
	++it;
      } else {
	in_concat = false;
      }
    }
  }

  _do_expand_args.resize(_arg_names.size(), false);
  for (size_t i = 0; i < _arg_names.size(); ++i) {
    if (do_expand.count(_arg_names[i]))
      _do_expand_args[i] = true;
  }
}

bool macro::operator==(const macro &rhs) const noexcept
{
  return (_name == rhs._name &&
	  _func_like == rhs._func_like &&
	  _arg_names == rhs._arg_names &&
	  _variadic == rhs._variadic &&
	  _repl == rhs._repl);
}

const std::vector<std::string>& macro::get_arg_names() const noexcept
{
  return _arg_names;
}

size_t macro::non_va_arg_count() const noexcept
{
  assert(_func_like);
  if (_variadic)
    return _do_expand_args.size() - 1;

  return _do_expand_args.size();
}

bool macro::shall_expand_arg(const size_t pos) const noexcept
{
  assert(_func_like);
  assert(pos <= _do_expand_args.size());
  return _do_expand_args[pos];
}


raw_pp_tokens::const_iterator
macro::_next_non_ws_repl(const raw_pp_tokens::const_iterator it)
  const noexcept
{
  if (it == _repl.end() || !it->is_ws()) {
    return it;
  }
  // No need to loop here: we won't have more than one
  // consecutive whitespace in a macro replacement list
  assert(it + 1 == _repl.end() || !(it + 1)->is_ws());
  return it + 1;
}

raw_pp_tokens::const_iterator
macro::_skip_stringification_or_single(const raw_pp_tokens::const_iterator &it)
  const noexcept
{
  assert(it != _repl.end());

  if (_func_like && it->is_punctuator("#")) {
    assert(it + 1 != _repl.end());
    assert((it + 1)->is_id());
    return it + 2;
  }
  return it + 1;
}

bool macro::_is_concat_op(const raw_pp_tokens::const_iterator &it)
  const noexcept
{
  return (it != _repl.end() && it->is_punctuator("##"));
}
