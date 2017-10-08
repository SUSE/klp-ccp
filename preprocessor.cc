#include <cassert>
#include <functional>
#include "preprocessor.hh"
#include "pp_except.hh"
#include "macro_undef.hh"

using namespace suse::cp;
using namespace suse::cp::_preprocessor_impl;

preprocessor::preprocessor(const std::string &filename,
			   const header_resolver &header_resolver)
  : _header_resolver(header_resolver),
    _root_expansion_state(), __counter__(0),
    _maybe_pp_directive(true), _line_empty(true)
{
  _tokenizers.emplace(filename);
}

pp_token preprocessor::read_next_token()
{
  // Basically, this is just a wrapper around _expand()
  // which removes or adds whitespace as needed.
  //
  // There are two cases for _pending_tokens: either it
  // consists of a single ws token, in which case it's
  // not clear yet whether it should get emitted or
  // it consists of empties + ws + something else
  // in which case all of these but the last should get emitted.
  assert(_pending_tokens.size() != 1 || !_pending_tokens.front().is_empty());
  if (_pending_tokens.size() > 1 ||
      (_pending_tokens.size() == 1 &&
       (_pending_tokens.front().is_eof() ||
	_pending_tokens.front().is_newline()))) {
    auto tok = _pending_tokens.front();
    if (tok.is_newline())
      _line_empty = true;
    _pending_tokens.pop();
    return tok;
  }

 read_next:
  auto next_tok = _expand(_root_expansion_state,
			std::bind(&preprocessor::_read_next_plain_token, this));
  next_tok.expansion_history().clear();

  if (_pending_tokens.size() == 1 && _pending_tokens.front().is_ws()) {
    if (next_tok.is_newline() || next_tok.is_eof()) {
      _pending_tokens.pop();
      if (_line_empty && next_tok.is_newline())
	goto read_next;
      _line_empty = true;
      return next_tok;
    } else if (next_tok.is_empty()) {
      return next_tok;
    }
    assert(!next_tok.is_ws());
    _line_empty = false;
    std::swap(next_tok, _pending_tokens.front());
    return next_tok;
  }

  assert(_pending_tokens.empty() ||
	 (_pending_tokens.size() == 1 && !_pending_tokens.front().is_ws() &&
	  !_pending_tokens.front().is_newline() &&
	  !_pending_tokens.front().is_empty() &&
	  !_pending_tokens.front().is_eof()));
  // There are certain token combinations where GNU cpp emits an extra
  // space in order to make preprocessing idempotent:
  // pp_number {-, +, --, ++, -=, +=, ., id, pp_number}
  // . pp_number
  // id {id, pp_number)
  // + {+, ++, =}, - {-, --, =, >}, ! =, * =, / =, % =, ^ =,
  // < {<, =, <=, <<=, :, %}, << =, > {>, =, >=, >>=}, >> =,
  // & {=, &}, | {=, |},
  // % {=, >, :},  %: %:, : >,
  // : :, . ., # # if any of these is coming from macro expansion
  //                or they're separated by any empties
  pp_token prev_tok(pp_token::type::eof, std::string(), file_range());
  if (_pending_tokens.empty()) {
    if (next_tok.is_ws()) {
      _pending_tokens.push(std::move(next_tok));
      goto read_next;
    } else if (next_tok.is_newline()) {
      if (_line_empty)
	goto read_next;
      _line_empty = true;
      return next_tok;
    } else if (next_tok.is_empty() || next_tok.is_eof()) {
      return next_tok;
    } else {
      prev_tok = std::move(next_tok);
      next_tok = _expand(_root_expansion_state,
			std::bind(&preprocessor::_read_next_plain_token, this));
    }
  } else {
    assert(_pending_tokens.size() == 1);
    prev_tok = std::move(_pending_tokens.front());
    _pending_tokens.pop();
  }

  assert (!prev_tok.is_empty() && !prev_tok.is_newline() &&
	  !prev_tok.is_ws() && !prev_tok.is_eof());
  _line_empty = false;
  while (next_tok.is_empty()) {
    _pending_tokens.push(std::move(next_tok));
    next_tok = _expand(_root_expansion_state,
		       std::bind(&preprocessor::_read_next_plain_token, this));
  }

  if (next_tok.is_ws() || next_tok.is_newline() || next_tok.is_eof()) {
    _pending_tokens.push(std::move(next_tok));
    return prev_tok;
  }

  if ((prev_tok.is_type_any_of<pp_token::type::pp_number>() &&
       (next_tok.is_punctuator("-") || next_tok.is_punctuator("+") ||
	next_tok.is_punctuator("--") || next_tok.is_punctuator("++") ||
	next_tok.is_punctuator("-=") || next_tok.is_punctuator("+=") ||
	next_tok.is_punctuator(".") || next_tok.is_id() ||
	next_tok.is_type_any_of<pp_token::type::pp_number>())) ||
      (prev_tok.is_punctuator(".") &&
       next_tok.is_type_any_of<pp_token::type::pp_number>()) ||
      (prev_tok.is_id() &&
       (next_tok.is_id() ||
	next_tok.is_type_any_of<pp_token::type::pp_number>())) ||
      (prev_tok.is_type_any_of<pp_token::type::punctuator>() &&
       next_tok.is_type_any_of<pp_token::type::punctuator>() &&
       ((prev_tok.is_punctuator("+") &&
	 (next_tok.is_punctuator("+") || next_tok.is_punctuator("++") ||
	  next_tok.is_punctuator("="))) ||
	(prev_tok.is_punctuator("-") &&
	 (next_tok.is_punctuator("-") || next_tok.is_punctuator("--") ||
	  next_tok.is_punctuator("=") || next_tok.is_punctuator(">"))) ||
	(prev_tok.is_punctuator("!") && next_tok.is_punctuator("=")) ||
	(prev_tok.is_punctuator("*") && next_tok.is_punctuator("=")) ||
	(prev_tok.is_punctuator("/") && next_tok.is_punctuator("=")) ||
	(prev_tok.is_punctuator("%") && next_tok.is_punctuator("=")) ||
	(prev_tok.is_punctuator("^") && next_tok.is_punctuator("=")) ||
	(prev_tok.is_punctuator("<") &&
	 (next_tok.is_punctuator("<") || next_tok.is_punctuator("=") ||
	  next_tok.is_punctuator("<=") || next_tok.is_punctuator("<<=") ||
	  next_tok.is_punctuator(":") || next_tok.is_punctuator("%"))) ||
	(prev_tok.is_punctuator("<<") && next_tok.is_punctuator("=")) ||
	(prev_tok.is_punctuator(">") &&
	 (next_tok.is_punctuator(">") || next_tok.is_punctuator("=") ||
	  next_tok.is_punctuator(">=") || next_tok.is_punctuator(">>="))) ||
	(prev_tok.is_punctuator(">>") && next_tok.is_punctuator("=")) ||
	(prev_tok.is_punctuator("&") &&
	 (next_tok.is_punctuator("=") || next_tok.is_punctuator("&"))) ||
	(prev_tok.is_punctuator("|") &&
	 (next_tok.is_punctuator("=") || next_tok.is_punctuator("|"))) ||
	(prev_tok.is_punctuator("%") &&
	 (next_tok.is_punctuator("=") || next_tok.is_punctuator(">") ||
	  next_tok.is_punctuator(":"))) ||
	(prev_tok.is_punctuator("%:") && next_tok.is_punctuator("%:")) ||
	(prev_tok.is_punctuator(":") && next_tok.is_punctuator(">")) ||
	(((prev_tok.is_punctuator(":") && next_tok.is_punctuator(":")) ||
	  (prev_tok.is_punctuator(".") && next_tok.is_punctuator(".")) ||
	  (prev_tok.is_punctuator("#") && next_tok.is_punctuator("#"))) &&
	 (!prev_tok.used_macros().empty() || !next_tok.used_macros().empty() ||
	  !_pending_tokens.empty()))))) {
    _pending_tokens.emplace(pp_token::type::ws, " ",
			file_range(next_tok.get_file_range().get_filename(),
				   next_tok.get_file_range().get_start_loc()));
  }

  _pending_tokens.push(std::move(next_tok));
  return prev_tok;
}

template<typename T>
void preprocessor::_grab_remarks_from(T &from)
{
  _remarks += from.get_remarks();
  from.get_remarks().clear();
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
    ++it_tok;
    if (it_tok->is_ws())
      ++it_tok;

    if (!it_tok->is_id()) {
      code_remark remark(code_remark::severity::fatal,
			 "identifier expected after #define",
			 it_tok->get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    auto it_m_undef = _macro_undefs.find(it_tok->get_value());
    std::shared_ptr<const macro_undef> m_undef;
    if (it_m_undef != _macro_undefs.end()) {
      m_undef = std::move(it_m_undef->second);
      _macro_undefs.erase(it_m_undef);
    }

    std::shared_ptr<const macro> m =
      macro::parse_macro_definition(directive_toks.cbegin(),
				    directive_toks.cend(),
				    std::move(m_undef),
				    _remarks);

    auto it_existing = _macros.find(m->get_name());
    if (it_existing != _macros.end() && *it_existing->second != *m) {
      code_remark remark(code_remark::severity::fatal,
			 "macro redefined in an incompatible way",
			 m->get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    _macros.insert(std::make_pair(m->get_name(), std::move(m)));
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

    auto it_tok_id = it_tok;
    ++it_tok;
    if (!it_tok->is_newline() && !it_tok->is_eof()) {
      code_remark remark(code_remark::severity::fatal,
			 "garbage after #undef",
			 it_tok->get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    auto it_m = _macros.find(it_tok_id->get_value());
    if (it_m != _macros.end()) {
      const file_range &range_start = directive_toks.begin()->get_file_range();
      const file_range &range_end = directive_toks.rbegin()->get_file_range();

      std::shared_ptr<const macro_undef> mu =
	macro_undef::create(std::move(it_m->second),
			    file_range(range_start, range_end));
      _macro_undefs.insert(std::make_pair(it_tok_id->get_value(),
					  std::move(mu)));
      _macros.erase(it_m);
    }
  }
}

pp_token
preprocessor::_expand(_preprocessor_impl::_expansion_state &state,
		      const std::function<pp_token()> &token_reader)
{
  auto read_tok = [&]() {
    while (!state.macro_instances.empty()) {
      try {
	auto tok = state.macro_instances.back().read_next_token();
	_grab_remarks_from(state.macro_instances.back());
	if (!tok.is_eof()) {
	  return tok;
	} else {
	  state.macro_instances.pop_back();
	}
      } catch (const pp_except&) {
	_grab_remarks_from(state.macro_instances.back());
	throw;
      }
    }

    return token_reader();
  };

 read_next:
  pp_token tok = (state.pending_tokens.empty()
		  ? read_tok()
		  : std::move(state.pending_tokens.front()));
  if (!state.pending_tokens.empty())
    state.pending_tokens.pop();

  if (tok.is_eof())
    return tok;

  // Deal with possible macro invocation
  if (tok.is_id()) {
    // Check for the predefined macros first.
    if (tok.get_value() == "__FILE__") {
      state.last_ws = false;
      return pp_token(pp_token::type::str, tok.get_file_range().get_filename(),
		      tok.get_file_range(), used_macros(),
		      std::move(tok.used_macros()), tok.used_macro_undefs());
    } else if (tok.get_value() == "__LINE__") {
      auto line = tok.get_file_range().get_start_loc().line();
      state.last_ws = false;
      return pp_token(pp_token::type::pp_number, std::to_string(line),
		      tok.get_file_range(), used_macros(),
		      std::move(tok.used_macros()), tok.used_macro_undefs());
    } else if (tok.get_value() == "__INCLUDE_LEVEL__") {
      state.last_ws = false;
      return pp_token(pp_token::type::pp_number,
		      std::to_string(_tokenizers.size() - 1),
		      tok.get_file_range(), used_macros(),
		      std::move(tok.used_macros()), tok.used_macro_undefs());
    } else if (tok.get_value() == "__COUNTER__") {
      state.last_ws = false;
      return pp_token(pp_token::type::pp_number, std::to_string(__counter__++),
		      tok.get_file_range(), used_macros(),
		      std::move(tok.used_macros()), tok.used_macro_undefs());
    }

    auto m = _macros.find(tok.get_value());
    if (m != _macros.end() && !tok.expansion_history().count(m->second)) {
      if (!m->second->is_func_like()) {
	state.macro_instances.push_back(
		_handle_object_macro_invocation(m->second, std::move(tok)));
	goto read_next;
      } else {
	pp_token next_tok = read_tok();
	bool last_ws = false;
	while (true) {
	  if (next_tok.is_eof()) {
	    state.pending_tokens.push(std::move(next_tok));
	    state.last_ws = false;
	    return tok;
	  } else if (next_tok.is_ws()) {
	    if (!last_ws) {
	      state.pending_tokens.push(std::move(next_tok));
	      last_ws = true;
	    }
	  } else if (next_tok.is_newline()) {
	      state.pending_tokens.push(std::move(next_tok));
	      last_ws = false;
	  } else if (next_tok.is_empty()) {
	    state.pending_tokens.push(std::move(next_tok));
	  } else {
	    break;
	  }
	  next_tok = read_tok();
	};

	// Not a macro invocation?
	if (!next_tok.is_punctuator("(")) {
	  state.last_ws = false;
	  state.pending_tokens.push(std::move(next_tok));
	  return tok;
	}

	used_macros base_um(std::move(tok.used_macros()));
	used_macro_undefs base_umu(std::move(tok.used_macro_undefs()));
	while (!state.pending_tokens.empty()) {
	  base_um += std::move(state.pending_tokens.front().used_macros());
	  base_umu +=
	    std::move(state.pending_tokens.front().used_macro_undefs());
	  state.pending_tokens.pop();
	}

	base_um += next_tok.used_macros();
	base_umu += next_tok.used_macro_undefs();
	state.macro_instances.push_back(_handle_func_macro_invocation(
						m->second, std::move(base_um),
						std::move(base_umu),
						tok.get_file_range(),
						read_tok));
	}
	goto read_next;
    } else if (m == _macros.end()) {
      auto it_m_undef = _macro_undefs.find(tok.get_value());
      if (it_m_undef != _macro_undefs.end())
	tok.used_macro_undefs() += it_m_undef->second;
    }
  }

  if (tok.is_ws()) {
      if (state.last_ws)
	goto read_next;
      state.last_ws = true;
  } else if (!tok.is_empty()) {
    state.last_ws = false;
  }

  return tok;
}

macro::instance
preprocessor::_handle_object_macro_invocation(
				const std::shared_ptr<const macro> &macro,
				pp_token &&id_tok)
{
  return macro::instance(macro, std::move(id_tok.used_macros()),
			 std::move(id_tok.used_macro_undefs()),
			 std::vector<pp_tokens>(), std::vector<pp_tokens>(),
			 id_tok.get_file_range());
}

macro::instance
preprocessor::_handle_func_macro_invocation(
	const std::shared_ptr<const macro> &macro,
	used_macros &&used_macros_base,
	used_macro_undefs &&used_macro_undefs_base,
	const file_range &id_tok_file_range,
	const std::function<pp_token()> &token_reader)
{
  std::vector<pp_tokens> args;
  std::vector<pp_tokens> exp_args;
  file_range invocation_range;

  if (macro->non_va_arg_count() || macro->is_variadic()) {
    size_t i_arg = 0;
    const size_t n_args =  macro->non_va_arg_count() + macro->is_variadic();
    args.reserve(n_args);
    exp_args.reserve(n_args);
    while (i_arg < n_args) {
      std::tuple<pp_tokens, pp_tokens, pp_token> cur_arg
	= _create_macro_arg(token_reader, macro->shall_expand_arg(i_arg),
			    i_arg == macro->non_va_arg_count(),
			    used_macros_base, used_macro_undefs_base);
      ++i_arg;
      args.push_back(std::move(std::get<0>(cur_arg)));
      exp_args.push_back(std::move(std::get<1>(cur_arg)));

      assert(std::get<2>(cur_arg).get_type() == pp_token::type::punctuator);
      assert(std::get<2>(cur_arg).get_value().size() == 1);
      const char arg_delim = std::get<2>(cur_arg).get_value()[0];
      if (i_arg < (!macro->is_variadic() ? n_args : n_args - 1)) {
	if (arg_delim != ',') {
	  assert(arg_delim == ')');
	  code_remark remark(code_remark::severity::fatal,
			     "too few parameters in macro invocation",
			     std::get<2>(cur_arg).get_file_range());
	  _remarks.add(remark);
	  throw pp_except(remark);
	}
      } else if (i_arg == n_args - 1) {
	// A variadic argument might simply have been left out.
	assert(macro->is_variadic());
	if (arg_delim == ')') {
	  // Add empty dummy argument.
	  args.emplace_back();
	  exp_args.emplace_back();
	  ++i_arg;
	}
      } else if (i_arg == n_args) {
	if (arg_delim != ')') {
	  assert(arg_delim == ',');
	  code_remark remark(code_remark::severity::fatal,
			     "too many parameters in macro invocation",
			     std::get<2>(cur_arg).get_file_range());
	  _remarks.add(remark);
	  throw pp_except(remark);
	}
      }

      if (i_arg == n_args) {
	assert(arg_delim == ')');
	invocation_range = file_range(id_tok_file_range,
				      std::get<2>(cur_arg).get_file_range());
      }
    }
  } else {
    // Slurp in the tokens between the () and make sure they're all empties.
    std::tuple<pp_tokens, pp_tokens, pp_token> dummy_arg
      = _create_macro_arg(token_reader, false, false,
			  used_macros_base, used_macro_undefs_base);
    assert(std::get<2>(dummy_arg).get_type() == pp_token::type::punctuator);
    bool non_empty_found = false;
    for (auto tok : std::get<0>(dummy_arg)) {
      if (!tok.is_empty() && !tok.is_ws()) {
	non_empty_found = true;
	break;
      }
    }
    if (non_empty_found || std::get<2>(dummy_arg).get_value() != ")") {
      code_remark remark(code_remark::severity::fatal,
			 "too many parameters in macro invocation",
			 std::get<2>(dummy_arg).get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }
    invocation_range = file_range(id_tok_file_range,
				  std::get<2>(dummy_arg).get_file_range());
  }

  return macro::instance(macro, std::move(used_macros_base),
			 std::move(used_macro_undefs_base),
			 std::move(args), std::move(exp_args),
			 invocation_range);
}

std::tuple<pp_tokens, pp_tokens, pp_token>
preprocessor::_create_macro_arg(const std::function<pp_token()> &token_reader,
				const bool expand, const bool variadic,
				used_macros &used_macros_base,
				used_macro_undefs &used_macro_undefs_base)
{
  pp_tokens arg;
  pp_tokens exp_arg;
  std::size_t lparens = 0;
  pp_token end_delim(pp_token::type::eof, std::string(), file_range());

  bool last_ws = false;
  auto read_arg_tok = [&]() {
  skip_ws_or_empty:
    auto tok = token_reader();

    if (tok.is_eof()) {
      code_remark remark(code_remark::severity::fatal,
			 "no closing right parenthesis in macro invocation",
			 tok.get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    used_macros_base += tok.used_macros();
    tok.used_macros().clear();
    used_macro_undefs_base += tok.used_macro_undefs();
    tok.used_macro_undefs().clear();

    if (tok.is_empty())
      goto skip_ws_or_empty;

    if (!lparens &&
	((!variadic && tok.is_punctuator(",")) || tok.is_punctuator(")"))) {
      end_delim = std::move(tok);
      return pp_token(pp_token::type::eof, std::string(),
		      end_delim.get_file_range());
    }

    if (tok.is_punctuator("("))
      ++lparens;
    else if (tok.is_punctuator(")"))
      --lparens;
    else if (tok.is_newline()) {
      tok = pp_token(pp_token::type::ws, " ", tok.get_file_range());
    } else if (tok.is_ws() && tok.get_value().size() > 1) {
      tok.set_type_and_value(pp_token::type::ws, " ");
    }

    if (tok.is_ws()) {
      if (last_ws)
	goto skip_ws_or_empty;
      last_ws = true;
    } else if (!tok.is_empty()) {
      last_ws = false;
    }

    arg.push_back(tok);
    return tok;
  };

  if (!expand) {
    while (read_arg_tok().get_type() != pp_token::type::eof) {}
  } else {
    _expansion_state state;
    while(true) {
      auto tok = _expand(state, read_arg_tok);
      if (tok.is_eof())
	break;
      exp_arg.push_back(std::move(tok));
    }
  }

  return std::tuple<pp_tokens, pp_tokens, pp_token>
		(std::move(arg), std::move(exp_arg), std::move(end_delim));
}
_expansion_state::_expansion_state()
  : last_ws(false)
{}
