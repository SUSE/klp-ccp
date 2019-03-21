#include <stdexcept>
#include <set>
#include <utility>
#include <cassert>
#include <algorithm>
#include "macro.hh"
#include "pp_token.hh"
#include "pp_except.hh"

using namespace klp::ccp;

macro::macro(const std::string &name,
	     const bool func_like,
	     const bool variadic,
	     std::vector<std::string> &&arg_names,
	     raw_pp_tokens &&repl,
	     const file_range &file_range,
	     std::shared_ptr<const macro_undef> &&prev_macro_undef)
  : _name(name), _arg_names(std::move(arg_names)),
    _repl(std::move(repl)),
    _file_range(file_range),
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

std::shared_ptr<const macro>
macro::parse_macro_definition(const raw_pp_tokens::const_iterator begin,
			      raw_pp_tokens::const_iterator end,
			      std::shared_ptr<const macro_undef> &&macro_undef,
			      code_remarks &remarks)
{
  assert (begin != end);
  assert(begin->is_punctuator("#"));

  assert((end - 1)->is_newline() || (end - 1)->is_eof());
  // end is decremented by one such that we can dereference
  // it even for it == end from now on.
  --end;

  const file_range &range_start = begin->get_file_range();
  const file_range &range_end = end->get_file_range();

  auto skip_ws = [&end](const decltype(end) it) {
    if (it == end || !it->is_ws()) {
      return it;
    }
    // No need to loop here:
    // Macro definitions come in a single line and pp_tokenizer
    // won't emit more than one consecutive whitespace.
    assert(it + 1 == end || !(it + 1)->is_ws());
    return it + 1;
  };

  auto it = skip_ws(begin + 1);
  assert(it->is_id() && it->get_value() == "define");
  it = skip_ws(it + 1);

  if (!it->is_id()) {
    code_remark_raw remark(code_remark_raw::severity::fatal,
			   "no identifier following #define",
			   it->get_file_range());
    remarks.add(remark);
    throw pp_except(remark);
  }

  const std::string &name = it->get_value();
  ++it;

  bool func_like = false;
  bool variadic = false;
  std::vector<std::string> arg_names;
  std::set<std::string> unique_arg_names;
  if (it->is_punctuator("(")) {
    func_like = true;

    it = skip_ws(it + 1);
    while (it != end && !it->is_punctuator(")")) {
      if (variadic) {
	code_remark_raw remark(code_remark_raw::severity::fatal,
			       "garbage in macro argument list after ...",
			       it->get_file_range());
	remarks.add(remark);
	throw pp_except(remark);
      }

      if (!arg_names.empty() && !it->is_punctuator(",")) {
	code_remark_raw remark(code_remark_raw::severity::fatal,
			       "comma expected in macro argument list",
			       it->get_file_range());
	remarks.add(remark);
	throw pp_except(remark);
      } else if (it->is_punctuator(",")) {
	if (arg_names.empty()) {
	  code_remark_raw remark(code_remark_raw::severity::fatal,
				 "leading comma in macro argument list",
				 it->get_file_range());
	  remarks.add(remark);
	  throw pp_except(remark);
	}
	it = skip_ws(it + 1);
	if (it == end || it->is_punctuator(")")) {
	  code_remark_raw remark(code_remark_raw::severity::fatal,
				 "trailing comma in macro argument list",
				 it->get_file_range());
	  remarks.add(remark);
	  throw pp_except(remark);
	}
      }

      if (it->is_punctuator("...")) {
	variadic = true;
	arg_names.push_back("__VA_ARGS__");
	unique_arg_names.insert("__VA_ARGS__");
	it = skip_ws(it + 1);
      } else if (it->is_id()) {
	if (it->get_value() == "__VA_ARGS__") {
	  code_remark_raw remark
	    (code_remark_raw::severity::fatal,
	     "__VA_ARGS__ not allowed as a macro argument name",
	     it->get_file_range());
	  remarks.add(remark);
	  throw pp_except(remark);
	} else if (!unique_arg_names.insert(it->get_value()).second) {
	  code_remark_raw remark(code_remark_raw::severity::fatal,
				 "duplicate macro argument name",
				 it->get_file_range());
	  remarks.add(remark);
	  throw pp_except(remark);
	}

	arg_names.push_back(it->get_value());
	it = skip_ws(it + 1);
	if (it->is_punctuator("...")) {
	  variadic = true;
	  it = skip_ws(it + 1);
	}
      } else {
	code_remark_raw remark(code_remark_raw::severity::fatal,
			       "garbage in macro argument list",
			       it->get_file_range());
	remarks.add(remark);
	throw pp_except(remark);
      }
    }
    if (it == end) {
      code_remark_raw remark(code_remark_raw::severity::fatal,
			     "macro argument list not closed",
			     end->get_file_range());
      remarks.add(remark);
      throw pp_except(remark);
    }

    assert(it->is_punctuator(")"));
    ++it;
  }

  if (it != end && !it->is_ws()) {
    code_remark_raw remark(code_remark_raw::severity::warning,
			   "no whitespace before macro replacement list",
			   it->get_file_range());
    remarks.add(remark);
  }

  it = skip_ws(it);

  return create(name, func_like, variadic, std::move(arg_names),
		_normalize_repl(it, end, func_like, unique_arg_names,
				remarks),
		file_range(range_start, range_end),
		std::move(macro_undef));
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

raw_pp_tokens macro::_normalize_repl(const raw_pp_tokens::const_iterator begin,
				     const raw_pp_tokens::const_iterator end,
				     const bool func_like,
				     const std::set<std::string> &arg_names,
				     code_remarks &remarks)
{
  if (begin == end)
    return raw_pp_tokens();

  assert(!begin->is_ws() && !(end-1)->is_ws());

  if (begin->is_punctuator("##")) {
    code_remark_raw remark(code_remark_raw::severity::fatal,
			   "## at beginning of macro replacement list",
			   begin->get_file_range());
    remarks.add(remark);
    throw pp_except(remark);
  } else if ((end - 1)->is_punctuator("##")) {
    code_remark_raw remark(code_remark_raw::severity::fatal,
			   "## at end of macro replacement list",
			   (end - 1)->get_file_range());
    remarks.add(remark);
    throw pp_except(remark);
  }

  // Thanks to pp_tokenizer, we're already free of multiple
  // consecutive whitespace tokens.
  // Remove any whitespace separating either ## or # and their
  // operands. Collapse multiple ## operators into one.
  auto concat_or_stringify_op_pred = [func_like](const raw_pp_token &tok) {
    return (tok.is_punctuator("##") ||
	    (func_like && tok.is_punctuator("#")));
  };

  raw_pp_tokens result;
  auto it_seq_begin = begin;
  while (it_seq_begin != end) {
    auto it_seq_end = std::find_if(it_seq_begin, end,
				   concat_or_stringify_op_pred);
    if (it_seq_end == end) {
      result.insert(result.end(), it_seq_begin, it_seq_end);
    } else if (it_seq_end->is_punctuator("##")) {
      if ((it_seq_end - 1)->is_ws()) {
	assert((it_seq_end - 1) != it_seq_begin);
	result.insert(result.end(), it_seq_begin, it_seq_end - 1);
      } else {
	result.insert(result.end(), it_seq_begin, it_seq_end);
      }
      result.push_back(*it_seq_end);

      while (it_seq_end != end &&
	     (it_seq_end->is_ws() || it_seq_end->is_punctuator("##"))) {
	++it_seq_end;
      }
    } else {
      assert(func_like && it_seq_end->is_punctuator("#"));
      auto it_arg = it_seq_end + 1;
      if (it_arg != end && it_arg->is_ws())
	++it_arg;
      assert(it_arg == end || !it_arg->is_ws());
      if (it_arg == end || !it_arg->is_id() ||
	  !arg_names.count(it_arg->get_value())) {
	code_remark_raw remark(code_remark_raw::severity::fatal,
			       "# in func-like macro not followed by parameter",
			       (end - 1)->get_file_range());
	remarks.add(remark);
	throw pp_except(remark);
      }

      result.insert(result.end(), it_seq_begin, it_seq_end + 1);
      result.push_back(*it_arg);
      it_seq_end = it_arg + 1;
    }
    it_seq_begin = it_seq_end;
  }

  return result;
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
