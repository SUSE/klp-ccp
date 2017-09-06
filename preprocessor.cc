#include <cassert>
#include <functional>
#include "preprocessor.hh"
#include "pp_except.hh"

using namespace suse::cp;
using namespace suse::cp::_preprocessor_impl;

preprocessor::preprocessor(const std::string &filename,
			   const header_resolver &header_resolver)
  : _header_resolver(header_resolver),
    _root_expansion_state(),
    _maybe_pp_directive(true), _line_empty(true)
{
  _tokenizers.emplace(filename);
}

pp_token preprocessor::read_next_token()
{
  return _expand(_root_expansion_state,
		 std::bind(&preprocessor::_read_next_plain_token, this));
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

pp_token
preprocessor::_expand(_preprocessor_impl::_expansion_state &state,
		      const std::function<pp_token()> &token_reader)
{
  if (!state.pending_empties.empty()) {
    pp_token tok = std::move(state.pending_empties.front());
    state.pending_empties.erase(state.pending_empties.begin());
    return tok;
  } else if (state.pending_ws) {
    assert(!state.last_ws);
    state.last_ws = true;
    state.pending_ws = false;
    return pp_token(pp_token::type::ws, " ", std::move(state.pending_ws_range));
  } else if (state.pending_eof) {
    return pp_token(pp_token::type::eof, std::string(),
		    std::move(state.pending_eof_range));
  }

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
  pp_token tok = read_tok();
  if (tok.is_eof())
    return tok;

  // Deal with possible macro invocation
  if (tok.is_id()) {
    auto m = _macros.find(tok.get_value());
    if (m != _macros.end() && !tok.used_macros().count(m->second)) {
      if (!m->second->is_func_like()) {
	state.macro_instances.push_back(
		_handle_object_macro_invocation(m->second, std::move(tok)));
	goto read_next;
      } else {
	pp_token next_tok = read_tok();
	while (true) {
	  if (next_tok.is_eof()) {
	    state.pending_eof = true;
	    state.pending_eof_range = next_tok.get_file_range();
	    return tok;
	  } else if (next_tok.is_ws()) {
	    if (!state.pending_ws) {
	      state.pending_ws = true;
	      state.pending_ws_range = next_tok.get_file_range();
	    }
	  } else if (next_tok.is_empty()) {
	    state.pending_empties.push_back(std::move(next_tok));
	  } else {
	    break;
	  }
	  next_tok = read_tok();
	};

	// Not a macro invocation?
	if (!next_tok.is_punctuator("(")) {
	  state.last_ws = false;
	  return tok;
	}

	state.pending_empties.clear();
	state.pending_ws = false;
	assert(!state.pending_eof);

	used_macros base_um(std::move(tok.used_macros()));
	while (!state.pending_empties.empty()) {
	  base_um += std::move(state.pending_empties.front().used_macros());
	  state.pending_empties.erase(state.pending_empties.begin());
	}
	base_um += next_tok.used_macros();
	state.macro_instances.push_back(_handle_func_macro_invocation(
						m->second, std::move(base_um),
						tok.get_file_range(),
						read_tok));
	}
	goto read_next;
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
			 std::vector<pp_tokens>(), std::vector<pp_tokens>(),
			 id_tok.get_file_range());
}

macro::instance
preprocessor::_handle_func_macro_invocation(
	const std::shared_ptr<const macro> &macro,
	used_macros &&used_macros_base,
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
			    i_arg == macro->non_va_arg_count());
      ++i_arg;
      args.push_back(std::move(std::get<0>(cur_arg)));
      exp_args.push_back(std::move(std::get<1>(cur_arg)));

      used_macros_base += std::move(std::get<2>(cur_arg).used_macros());
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
      = _create_macro_arg(token_reader, false, false);
    assert(std::get<2>(dummy_arg).get_type() == pp_token::type::punctuator);
    used_macros_base += std::move(std::get<2>(dummy_arg).used_macros());
    bool non_empty_found = false;
    for (auto tok : std::get<0>(dummy_arg)) {
      if (tok.is_empty()) {
	used_macros_base += tok.used_macros();
      } else if (!tok.is_ws()) {
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
			 std::move(args), std::move(exp_args),
			 invocation_range);
}

std::tuple<pp_tokens, pp_tokens, pp_token>
preprocessor::_create_macro_arg(const std::function<pp_token()> &token_reader,
				const bool expand, const bool variadic)
{
  pp_tokens arg;
  pp_tokens exp_arg;
  std::size_t lparens = 0;
  pp_token end_delim(pp_token::type::eof, std::string(), file_range());

  bool last_ws = false;
  auto read_arg_tok = [&]() {
  skip_ws:
    auto tok = token_reader();

    if (tok.is_eof()) {
      code_remark remark(code_remark::severity::fatal,
			 "no closing right parenthesis in macro invocation",
			 tok.get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }

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
      assert(tok.used_macros().empty());
      tok = pp_token(pp_token::type::ws, " ", tok.get_file_range());
    } else if (tok.is_ws() && tok.get_value().size() > 1) {
      tok.set_type_and_value(pp_token::type::ws, " ");
    }

    if (tok.is_ws()) {
      if (last_ws)
	goto skip_ws;
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
  : pending_ws(false), pending_eof(false), last_ws(false)
{}
