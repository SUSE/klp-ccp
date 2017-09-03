#include <cassert>
#include <functional>
#include "preprocessor.hh"
#include "pp_except.hh"

using namespace suse::cp;

preprocessor::preprocessor(const std::string &filename,
			   const header_resolver &header_resolver)
  : _header_resolver(header_resolver),
    _maybe_pp_directive(true), _line_empty(true)
{
  _tokenizers.emplace(filename);
}

pp_token preprocessor::read_next_token()
{
 again:
  auto tok = _read_next_token();

  if (tok.is_eof()) {
    _pending_ws.reset();
    return tok;
  }

  if (tok.is_newline()) {
    if (_line_empty) {
      _pending_ws.reset();
      goto again;
    } else {
      _line_empty = true;
      return tok;
    }
  }

  if (tok.is_ws()) {
    if (!_pending_ws) {
      // One or more macros evaluating to nothing might get invoked
      // hereafter. Stash the whitespace for now.
      _pending_ws.reset(new pp_token(std::move(tok)));
    }
    goto again;
  }

  // Deal with possible macro invocation
  if (tok.is_id() &&
      // Check that we'd not recurse into any already active macro.
      std::all_of(_macro_instances.cbegin(), _macro_instances.cend(),
		  [&tok](const macro::instance &i) {
		    return i.get_macro().get_name() != tok.get_value();
		  })) {
    auto macro = _macros.find(tok.get_value());
    if (macro != _macros.end()) {
      if (!macro->second->is_func_like()) {
	_macro_instances.push_back(
		_handle_object_macro_invocation(macro->second, std::move(tok)));
	goto again;
      } else {
	std::unique_ptr<pp_token> sep_ws_tok;
	auto next_tok = _read_next_token();
	if (next_tok.is_ws()) {
	  sep_ws_tok.reset(new pp_token(std::move(next_tok)));
	  next_tok = _read_next_token();
	}
	if (next_tok.is_punctuator("(")) {
	  _macro_instances.push_back(
		_handle_func_macro_invocation(
			macro->second,
			std::move(tok), std::move(next_tok),
			std::bind(&preprocessor::_read_next_token, this)));
	  goto again;
	}

	_line_empty = false;
	assert(_stashed_tokens.empty());
	if (_pending_ws) {
	  _stashed_tokens.push(std::move(tok));
	  if (sep_ws_tok)
	    _stashed_tokens.push(std::move(*sep_ws_tok));
	  _stashed_tokens.push(std::move(next_tok));
	  return _yield_pending_ws();
	}
	if (sep_ws_tok)
	  _stashed_tokens.push(std::move(*sep_ws_tok));
	_stashed_tokens.push(std::move(next_tok));
	return tok;
      }
    }
  }

  if (tok.is_empty())
    return tok;

  _line_empty = false;
  if (_pending_ws) {
    assert(_stashed_tokens.empty());
    _stashed_tokens.push(std::move(tok));
    return _yield_pending_ws();
  }

  return tok;
}

template<typename T>
void preprocessor::_grab_remarks_from(T &from)
{
  _remarks += from.get_remarks();
  from.get_remarks().clear();
}

pp_token preprocessor::_yield_pending_ws()
{
  assert(_pending_ws);
  auto tok = std::move(*_pending_ws);
  _pending_ws.reset();
  return tok;
}

pp_token preprocessor::_read_next_plain_token()
{
 again:
  assert(!_tokenizers.empty());
  try {
    auto tok = _tokenizers.top().read_next_token();
    _grab_remarks_from(_tokenizers.top());

    if (tok.is_eof()) {
      _tokenizers.pop();
      _maybe_pp_directive = true;
    } else if (tok.is_newline()) {
      _maybe_pp_directive = true;
    } else if (_maybe_pp_directive && tok.is_punctuator("#")) {
      _maybe_pp_directive = false;
      _handle_pp_directive(std::move(tok));
      _maybe_pp_directive = true;
      goto again;
    } else if (!tok.is_ws()) {
      _maybe_pp_directive = false;
    }

    return tok;
  } catch (const pp_except&) {
    _grab_remarks_from(_tokenizers.top());
    throw;
  }
}

pp_token preprocessor::_read_next_token()
{
  if (!_stashed_tokens.empty()) {
    auto tok = _stashed_tokens.front();
    _stashed_tokens.pop();
    return tok;
  } else if (!_macro_instances.empty()) {
    do {
      try {
	auto tok = _macro_instances.back().read_next_token();
	_grab_remarks_from(_macro_instances.back());
	if(!tok.is_eof())
	  return tok;
	else
	  _macro_instances.pop_back();
      } catch (const pp_except&) {
	_grab_remarks_from(_macro_instances.back());
	throw;
      }
    } while(!_macro_instances.empty());
  }

  return _read_next_plain_token();
}

void
preprocessor::_handle_pp_directive(pp_token &&sharp_tok)
{
  pp_tokens directive_toks{1U, std::move(sharp_tok)};

  while (true) {
    auto tok = _read_next_plain_token();
    directive_toks.push_back(std::move(tok));

    if (tok.is_newline() || tok.is_eof())
      break;
  }

  auto it_tok = directive_toks.cbegin() + 1;
  assert(it_tok != directive_toks.cend());

  if (it_tok->is_ws())
    ++it_tok;

  if (it_tok->is_newline() || it_tok->is_eof()) {
    // Null directive
    return;
  }

  assert(!it_tok->is_ws());
  if (!it_tok->is_id()) {
    code_remark remark(code_remark::severity::fatal,
		       "identifier expected in preprocessor directive",
		       it_tok->get_file_range());
    _remarks.add(remark);
    throw pp_except(remark);
  }

  if (it_tok->get_value() == "define") {
    std::shared_ptr<const macro> m =
      macro::parse_macro_definition(directive_toks.cbegin(),
				    directive_toks.cend(), _remarks);

    auto it_existing = _macros.find(m->get_name());
    if (it_existing != _macros.end() && *it_existing->second != *m) {
      code_remark remark(code_remark::severity::fatal,
			 "macro redefined in an incompatible way",
			 m->get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    _macros.insert(std::make_pair(m->get_name(), m));
  } else if (it_tok->get_value() == "undef") {
    ++it_tok;
    if (it_tok->is_ws())
      ++it_tok;

    if (!it_tok->is_id()) {
      code_remark remark(code_remark::severity::fatal,
			 "identifier expected after #undef",
			 it_tok->get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    _macros.erase(it_tok->get_value());

    ++it_tok;
    if (!it_tok->is_newline() && !it_tok->is_eof()) {
      code_remark remark(code_remark::severity::fatal,
			 "garbage after #undef",
			 it_tok->get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }
  }
}

macro::instance
preprocessor::_handle_object_macro_invocation(
				const std::shared_ptr<const macro> &macro,
				pp_token &&id_tok)
{
  return macro::instance(macro, used_macros(),
			 std::vector<pp_tokens>(), std::vector<pp_tokens>(),
			 id_tok.get_file_range());
}

template <typename token_reader_type>
macro::instance
preprocessor::_handle_func_macro_invocation(
	const std::shared_ptr<const macro> &macro,
	pp_token &&id_tok, pp_token &&lparen_tok,
	token_reader_type &&token_reader)
{
  used_macros base_um{id_tok.used_macros()};
  base_um += lparen_tok.used_macros();

  std::vector<pp_tokens> args;
  bool in_variadic_arg = !macro->non_va_arg_count() && macro->is_variadic();
  pp_tokens cur_arg;
  std::size_t n_lparens = 1;
  bool arg_empty = true;
  std::unique_ptr<pp_token> pending_ws;
  file_range arg_start_range = lparen_tok.get_file_range();
  while(n_lparens) {
    auto tok = token_reader();
    if (tok.is_eof()) {
      code_remark remark(code_remark::severity::fatal,
			 "preliminary end of macro parameter list",
			 tok.get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    if (tok.is_newline())
      tok.set_type_and_value(pp_token::type::ws, " ");

    if (tok.is_ws()) {
      if (!arg_empty && !pending_ws)
	pending_ws.reset(new pp_token(std::move(tok)));
      continue;
    } else if (tok.is_empty()) {
      cur_arg.push_back(std::move(tok));
      continue;
    }

    if (tok.is_punctuator(")")) {
      --n_lparens;

      // Handle func-like invocations with zero parameters explicitly.
      if (!n_lparens && !macro->non_va_arg_count() && !macro->is_variadic()) {
	base_um += tok.used_macros();
	break;
      }
    } else if (tok.is_punctuator("(")) {
      ++n_lparens;
    }

    if (!n_lparens ||
	(!in_variadic_arg && n_lparens == 1 && tok.is_punctuator(","))) {
      base_um += tok.used_macros();

      if (args.size() == macro->non_va_arg_count()) {
	if (!macro->is_variadic()) {
	  code_remark remark(code_remark::severity::fatal,
			     "too many macro parameters",
			     file_range(arg_start_range, tok.get_file_range()));
	  _remarks.add(remark);
	  throw pp_except(remark);
	}

	in_variadic_arg = true;
      }

      args.push_back(std::move(cur_arg));
      arg_start_range = tok.get_file_range();
      arg_empty = true;
      pending_ws.reset();
      continue;
    }

    arg_empty = false;
    if (pending_ws) {
      cur_arg.push_back(std::move(*pending_ws));
      pending_ws.reset();
    }

    cur_arg.push_back(std::move(tok));
  }

  if (args.size() != macro->non_va_arg_count() + macro->is_variadic()) {
	  code_remark remark(code_remark::severity::fatal,
			     "too few macro parameters", arg_start_range);
	  _remarks.add(remark);
	  throw pp_except(remark);
  }

  file_range invocation_range(id_tok.get_file_range(), arg_start_range);
  std::vector<pp_tokens> exp_args;
  exp_args.reserve(args.size());
  for (std::size_t i = 0; i < args.size(); ++i) {
    if (macro->shall_expand_arg(i)) {
      exp_args.push_back(_expand_macro_arg(args[i]));
    } else {
      exp_args.emplace_back();
    }
  }

  return macro::instance(macro, std::move(base_um),
			 std::move(args), std::move(exp_args),
			 invocation_range);
}

pp_tokens
preprocessor::_expand_macro_arg(const pp_tokens &arg)
{
  // This is very similar to read_next_token() in functionality and
  // structure, except that we don't get our to be expanded tokens
  // from the token stream, but from a the arg buffer.
  std::vector<macro::instance> macro_instances;
  auto it_arg = arg.begin();

  auto token_reader = [&]() {
    if (!macro_instances.empty()) {
      do {
	try {
	  auto tok = macro_instances.back().read_next_token();
	  _grab_remarks_from(macro_instances.back());
	  if(!tok.is_eof())
	    return tok;
	  else
	    macro_instances.pop_back();
	} catch (const pp_except&) {
	  _grab_remarks_from(macro_instances.back());
	  throw;
	}
      } while(!macro_instances.empty());
    }

    if (it_arg == arg.end())
      return pp_token(pp_token::type::eof, std::string(), ::file_range());

    return *it_arg++;
  };

  pp_tokens exp_arg;
  std::unique_ptr<pp_token> pending_ws;
  while (true) {
    auto tok = token_reader();
    if (tok.is_eof()) {
      pending_ws.reset();
      break;
    }

    if (tok.is_ws()) {
      if (!pending_ws) {
	// One or more macros evaluating to nothing might get invoked
	// hereafter. Stash the whitespace for now.
	pending_ws.reset(new pp_token(std::move(tok)));
      }
      continue;
    }

    // Deal with possible macro invocation
    if (tok.is_id() &&
	// Check that we'd not recurse into any already active macro.
	std::all_of(macro_instances.cbegin(), macro_instances.cend(),
		    [&tok](const macro::instance &i) {
		      return (i.get_macro().get_name() != tok.get_value());
		    })) {
      auto m = _macros.find(tok.get_value());
      if (m != _macros.end()) {
	if (!m->second->is_func_like()) {
	  macro_instances.push_back(
		_handle_object_macro_invocation(m->second, std::move(tok)));
	  continue;
	} else {
	  std::unique_ptr<pp_token> sep_ws_tok;
	  auto next_tok = token_reader();
	  if (next_tok.is_eof()) {
	    exp_arg.push_back(std::move(tok));
	    break;
	  }
	  if (next_tok.is_ws()) {
	    sep_ws_tok.reset(new pp_token(std::move(next_tok)));
	    next_tok = token_reader();
	    if (next_tok.is_eof()) {
	      exp_arg.push_back(std::move(tok));
	      exp_arg.push_back(std::move(*sep_ws_tok));
	      break;
	    }
	  }
	  if (next_tok.is_punctuator("(")) {
	    macro_instances.push_back(
				_handle_func_macro_invocation(
				m->second, std::move(tok), std::move(next_tok),
				token_reader));
	    continue;
	  }

	  if (pending_ws) {
	    exp_arg.push_back(std::move(*pending_ws));
	    pending_ws.reset();
	  }
	  exp_arg.push_back(std::move(tok));
	  if (sep_ws_tok) {
	    assert(!next_tok.is_ws());
	    exp_arg.push_back(std::move(*sep_ws_tok));
	  } else if (next_tok.is_ws()) {
	    pending_ws.reset(new pp_token(std::move(next_tok)));
	  } else {
	    exp_arg.push_back(std::move(next_tok));
	  }
	  continue;
	}
      }
    }

    if (!tok.is_empty() && pending_ws) {
      exp_arg.push_back(std::move(*pending_ws));
      pending_ws.reset();
    }
    exp_arg.push_back(std::move(tok));
  }

  return exp_arg;
}
