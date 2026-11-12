/*
 * Copyright (C) 2017-2022, 2024-2026  SUSE Software Solutions Germany GmbH
 *
 * This file is part of klp-ccp.
 *
 * klp-ccp is free software: you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * klp-ccp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <functional>
#include "preprocessor.hh"
#include "target/target.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "semantic_except.hh"
#include "path.hh"
#include "pp_expr_parser_driver.hh"
#include "callables_wrapper.hh"
#include "type_set.hh"
#include "pp_result_impl.hh"

using namespace klp::ccp;

struct preprocessor::_scanned_macro_invocation
{
  std::vector<std::size_t> args_offsets;
  std::vector<std::vector<pp_token>> args;

  std::size_t n_tokens;
};


struct preprocessor::_macro_expansion_stack_entry
{
private:
  typedef std::vector<pp_token> _pp_tokens_type;

public:
  _macro_expansion_stack_entry
		(const _macro_expansion_forest::_forest_nodes::size_type _node,
		 const pp_result::macro &_m,
		 std::vector<std::size_t> &&_args_offsets,
		 std::vector<_pp_tokens_type> &&args);

  _macro_expansion_stack_entry
		(const _macro_expansion_forest::_forest_nodes::size_type _node,
		 const pp_result::macro &_m);

  _macro_expansion_stack_entry
		(const _macro_expansion_forest::_forest_nodes::size_type _node,
		 const _pp_tokens_type &arg_tokens);

  const pp_result::macro *m;
  _macro_expansion_forest::_forest_nodes::size_type node;

  std::vector<std::size_t> args_offsets;
  std::vector<_pp_tokens_type> args;
  std::vector<_pp_tokens_type> exp_args;
  std::vector<_pp_tokens_type>::size_type next_arg;

  const pp_token *out_tokens;
  pp_tokens::size_type n_out_tokens;
  pp_tokens::size_type next_out_token;
};


struct preprocessor::_cond_incl_state
{
  _cond_incl_state(pp_result::conditional_inclusion_node &_incl_node);

  std::reference_wrapper<pp_result::conditional_inclusion_node> incl_node;
  pp_result::used_macros um;
  pp_result::macro_nondef_constraints mnc;
  pp_result::conditional_inclusion_node::directive_ranges_type
	directive_ranges;
  bool branch_active;
  pp_result::conditional_inclusion_node::directive_ranges_type::size_type
	taken_branch;
  bool has_else_branch;
};


preprocessor::preprocessor(const header_resolver &header_resolver,
			   const target &tgt)
  : _header_resolver(header_resolver), _tgt(tgt),
    _pp_result(new pp_result{}),
    _cur_header_inclusion_root(_pp_result->get_header_inclusion_roots()
			       .begin()),
    _cond_incl_nesting(0), __counter__(0),
    _m_defined_op(_pp_result->_add_macro("defined", {"identifier"},
				 pp_result::macro::builtin_special_tag{})),
    _maybe_pp_directive(true),
    _eof(false), _eof_index(std::numeric_limits<raw_pp_token_index>::max()),
    _peeked_begin(_none_peeked)
{
  // Populate the special builtin macros.
  for (const auto &name : { "__FILE__",
			    "__BASE_FILE__",
			    "__LINE__",
			    "__INCLUDE_LEVEL__",
			    "__COUNTER__" }) {
    const pp_result::macro &m =
      _pp_result->_add_macro(name, pp_result::macro::builtin_special_tag{});
    _macros.insert(std::make_pair(m.get_name(), std::ref(m)));
  }
}

preprocessor::preprocessor(preprocessor &&pp) = default;

preprocessor::~preprocessor() noexcept = default;

void preprocessor::add_root_source(const std::string &filename,
				   const bool is_preinclude)
{
  pp_result::header_inclusion_roots &roots =
    _pp_result->get_header_inclusion_roots();
  assert(_cur_header_inclusion_root == roots.begin());
  _pp_result->get_header_inclusion_roots().add(filename, is_preinclude);
  _cur_header_inclusion_root = roots.begin();
}

void preprocessor::set_base_file(const std::string &filename)
{
  assert(_base_file.empty());
  assert(_tokenizers.empty());
  _base_file = filename;
}

void preprocessor::register_builtin_macro(const std::string &name,
					  const std::string &repl)
{
  raw_pp_tokens repl_toks = pp_string_tokenizer::tokenize_builtin(repl);

  _check_macro_repl(repl_toks.begin(), repl_toks.end(),
		    false, std::vector<std::string>{},
		    [](const std::string&, const raw_pp_tokens::const_iterator&)
				-> pp_except {
		      assert(0);
		      __builtin_unreachable();
		    });

  const pp_result::macro &m =
    _pp_result->_add_macro(name, false, false, std::vector<std::string>{},
			   std::move(repl_toks),
			   pp_result::macro::builtin_tag{});
  if (!_macros.insert(std::make_pair(m.get_name(), std::ref(m))).second)
    assert(0);
}

void preprocessor::
register_builtin_macro(const std::string &name,
		       const std::initializer_list<const char *> &arg_list,
		       const bool variadic,
		       const std::string &repl)
{
  std::vector<std::string> _arg_list;
  _arg_list.reserve(arg_list.size());
  for (const auto a : arg_list)
    _arg_list.emplace_back(a);

  raw_pp_tokens repl_toks = pp_string_tokenizer::tokenize_builtin(repl);

  _check_macro_repl(repl_toks.begin(), repl_toks.end(),
		    true, _arg_list,
		    [](const std::string&, const raw_pp_tokens::const_iterator&)
				-> pp_except {
		      assert(0);
		      __builtin_unreachable();
		    });

  const pp_result::macro &m =
    _pp_result->_add_macro(name, true, variadic, std::move(_arg_list),
			   std::move(repl_toks),
			   pp_result::macro::builtin_tag{});
  if (!_macros.insert(std::make_pair(m.get_name(), std::ref(m))).second)
    assert(0);
}

void preprocessor::
register_builtin_special_macro(const std::string &name,
			       const std::initializer_list<const char *>
				 &arg_list,
			       builtin_special_macro_handler &&handler)
{
  std::vector<std::string> _arg_list;
  _arg_list.reserve(arg_list.size());
  for (const auto a : arg_list)
    _arg_list.emplace_back(a);

  const pp_result::macro &m =
    _pp_result->_add_macro(name, std::move(_arg_list),
			   pp_result::macro::builtin_special_tag{});
  if (!_macros.insert(std::make_pair(m.get_name(), std::ref(m))).second)
    assert(0);
  if (!_builtin_special_macro_handlers.insert
	(std::make_pair(m.get_name(), std::move(handler))).second) {
    assert(0);
  }
}

void preprocessor::register_predefined_macro
	(const std::string &signature,
	 const std::string &repl,
	 const std::function<void(const std::string)> &report_warning)
{
  const auto &report_fatal =
    [](const std::string &s) {
      return pp_except{s};
    };

  pp_string_tokenizer signature_tokenizer{
	signature, report_warning, report_fatal
  };
  raw_pp_tokens signature_toks;
  while (true) {
    raw_pp_token tok = signature_tokenizer.read_next_token();
    if (tok.is_eof())
      break;

    signature_toks.push_back(std::move(tok));
  }

  pp_string_tokenizer repl_tokenizer{
	repl, report_warning, report_fatal
  };
  raw_pp_tokens repl_toks;
  while (true) {
    raw_pp_token tok = repl_tokenizer.read_next_token();
    if (tok.is_eof())
      break;

    repl_toks.push_back(std::move(tok));
  }

  const auto &skip_ws
    = [](const raw_pp_tokens::const_iterator &it,
	 const raw_pp_tokens::const_iterator &end) {
	  return std::find_if(it, end,
			      [](const raw_pp_token &raw_tok) {
				return !raw_tok.is_ws() && !raw_tok.is_empty();
			      });
	 };

  raw_pp_tokens::const_iterator signature_end;
  std::string name;
  bool func_like;
  std::vector<std::string> arg_names;
  bool variadic;
  std::tie(signature_end, name, func_like, arg_names, variadic)
    = _parse_macro_signature(skip_ws(signature_toks.cbegin(),
				     signature_toks.cend()),
			     signature_toks.cend(),
			     [&](const std::string &msg,
				 const raw_pp_tokens::const_iterator&) {
			       return report_fatal(msg);
			     });
  if (skip_ws(signature_end, signature_toks.cend()) !=
      signature_toks.cend()) {
    throw report_fatal("garbage after macro signature");
  }

  _check_macro_repl(repl_toks.cbegin(), repl_toks.cend(),
		    func_like, arg_names,
		    [&](const std::string &msg,
			const raw_pp_tokens::const_iterator&) {
		      return report_fatal(msg);
		    });

  const pp_result::macro &m =
    _pp_result->_add_macro(name, func_like, variadic, std::move(arg_names),
			   std::move(repl_toks),
			   pp_result::macro::predefined_user_tag{});

  const auto it_existing = _macros.find(name);
  if (it_existing != _macros.end()) {
    if (it_existing->second.get().is_builtin_special()) {
      // GCC accepts redefinitions of its builtin macros from the
      // command line with a warning and lets the user's definition
      // take precedence.
      report_warning("\"" + name + "\" redefined");
      it_existing->second = std::cref(m);
      return;
    } else if (it_existing->second.get() != m) {
      throw report_fatal("macro redefined in an incompatible way");
    }
  }

  _macros.insert(std::make_pair(m.get_name(), std::ref(m)));
}

void preprocessor::register_predefined_macro_undef
	(const std::string &name,
	 const std::function<void(const std::string)> &report_warning)
{
  const auto &report_fatal =
    [](const std::string &s) {
      return pp_except{s};
    };

  pp_string_tokenizer name_tokenizer{
	name, report_warning, report_fatal
  };
  raw_pp_tokens name_toks;
  while (true) {
    raw_pp_token tok = name_tokenizer.read_next_token();
    if (tok.is_eof())
      break;

    name_toks.push_back(std::move(tok));
  }

  const auto &skip_ws
    = [](const raw_pp_tokens::const_iterator &it,
	 const raw_pp_tokens::const_iterator &end) {
	 return std::find_if(it, end,
			[](const raw_pp_token &raw_tok) {
			  return !raw_tok.is_ws() && !raw_tok.is_empty();
			});
      };

  const auto it_name = skip_ws(name_toks.cbegin(), name_toks.cend());
  if (it_name == name_toks.cend() ||
      !it_name->is_id() ||
      skip_ws(it_name + 1, name_toks.cend()) != name_toks.end()) {
    throw report_fatal("invalid macro name");
  }

  _pp_result->_add_macro_undef(it_name->get_value(),
			       pp_result::macro_undef::predefined_user_tag{});
  _macros.erase(it_name->get_value());
}

template<typename T>
void preprocessor::_grab_remarks_from(T &from)
{
  _remarks += from.get_remarks();
  from.get_remarks().clear();
}


pp_token_index preprocessor::_emit_pp_token(pp_result &r, pp_token &&tok)
{
  r._append_token(std::move(tok));
  const pp_token_index tok_index = r._get_last_pp_index();
  return tok_index;
}

const std::pair<raw_pp_token_index, bool> preprocessor::
_read_next_plain_token(const _read_ctl ctl)
{
 again:
  // Has the final eof token been seen?
  if (_eof) {
    return std::make_pair(_eof_index, true);
  }

  if (_tokenizers.empty()) {
    // First time _read_next_plain_token() gets called.
    // Setup the tokenizer for the first root file.
    assert(_cur_header_inclusion_root ==
	   _pp_result->get_header_inclusion_roots().begin());
    assert(!_pp_result->get_header_inclusion_roots().empty());
    _pp_result->_enter_root(*_cur_header_inclusion_root, 0);
    _tokenizers.emplace(*_cur_header_inclusion_root);
    _inclusions.emplace(std::ref(*_cur_header_inclusion_root));
  }

  if (ctl == _read_ctl::peek && _peeked_begin == _none_peeked) {
    // First in a series of peek requests. Setup everything so that
    // the peeked tokens can later get replayed in regular,
    // non-peeking mode.
    _peeked_begin = _pp_result->get_raw_tokens().size();
    _peeked_maybe_pp_directive = _maybe_pp_directive;
  }

  if (_peeked_begin != _none_peeked && ctl != _read_ctl::peek &&
      _peeked_begin == _pp_result->get_raw_tokens().size()) {
    // All previously peeked tokens have been replayed and the peek
    // series had been terminated by an EOF from the tokenizer, which
    // doesn't get recorded. Read it again below, this time for real.
    _peeked_begin = _none_peeked;
  }

  auto next_tok = [&]() -> const std::pair<raw_pp_token_index, bool> {
    if (_peeked_begin == _none_peeked || ctl == _read_ctl::peek) {
      try {
	auto tok = _tokenizers.top().read_next_token();

	_grab_remarks_from(_tokenizers.top());

	if (tok.is_eof()) {
	  if (ctl != _read_ctl::peek)
	    _handle_eof_from_tokenizer(tok);

	  return std::make_pair(_pp_result->get_raw_tokens().size(), true);
	}

	_pp_result->_append_token(std::move(tok));
	return std::make_pair(_pp_result->get_raw_tokens().size() - 1, false);

      } catch (const pp_except&) {
	_grab_remarks_from(_tokenizers.top());
	throw;
      }

    } else {
      const raw_pp_token_index peeked_tok = _peeked_begin;
      if (++_peeked_begin == _pp_result->get_raw_tokens().size())
	_peeked_begin = _none_peeked;
      _maybe_pp_directive = _peeked_maybe_pp_directive;
      return std::make_pair(peeked_tok, false);

    }
  }();

  if (next_tok.second) {
    // It's an EOF.
    _maybe_pp_directive = true;

    if (!_eof && ctl != _read_ctl::normal)
      return next_tok;

    goto again;
  }

  const raw_pp_token_index raw_tok_index = next_tok.first;
  const auto &raw_tok = _pp_result->get_raw_tokens()[raw_tok_index];

  const bool is_pp_directive =
    _maybe_pp_directive && raw_tok.is_punctuator("#");
  if (raw_tok.is_newline()) {
    _maybe_pp_directive = true;

  } else if (is_pp_directive) {
    // In peek mode, crossing preprocessor directives is not allowed.
    if (ctl == _read_ctl::peek) {
      return std::make_pair(raw_tok_index, true);
    }

    _handle_pp_directive(ctl);
    goto again;

  } else if (!raw_tok.is_ws() && !raw_tok.is_empty()) {
    _maybe_pp_directive = false;

  }

  if (ctl != _read_ctl::peek) {
    // If in the course of replaying some previously peeked tokens,
    // _peeked_maybe_pp_directive must be updated. Otherwise there's
    // no harm in doing so.
    _peeked_maybe_pp_directive = _maybe_pp_directive;
  }

  if (_cond_incl_inactive())
    goto again;

  return std::make_pair(raw_tok_index, false);
}

void preprocessor::_handle_eof_from_tokenizer(const raw_pp_token &eof_tok)
{
  // If the topmost inclusion tree node is an (unfinished) conditional
  // inclusion, that's an error.
  if (_cur_incl_node_is_cond()) {
    _fixup_inclusion_node_ranges();
    const raw_pp_token_index eof_index = _pp_result->get_raw_tokens().size();
    const raw_pp_tokens_range eof_range{eof_index, eof_index};
    const auto it_src
      =  _pp_result->intersecting_sources_begin(eof_range);
    assert(it_src != _pp_result->intersecting_sources_end(eof_range));
    code_remark remark(code_remark::severity::fatal,
		       "missing #endif at end of file",
		       *it_src, eof_tok.get_range_in_file());
    _remarks.add(remark);
    throw pp_except(remark);
  }

  _tokenizers.pop();
  const raw_pp_token_index cur_raw_pos =
    (!_pp_result->get_raw_tokens().empty() ?
     _pp_result->_get_last_raw_index() + 1 :
     0);
  _inclusions.top().get()._set_range_end(cur_raw_pos);
  _inclusions.pop();

  if (_tokenizers.empty()) {
    if (++_cur_header_inclusion_root ==
	_pp_result->get_header_inclusion_roots().end()) {
      _eof = true;
      _eof_index = _pp_result->get_raw_tokens().size();
      return;
    }

    _pp_result->_enter_root(*_cur_header_inclusion_root, cur_raw_pos);
    _tokenizers.emplace(*_cur_header_inclusion_root);
    _inclusions.emplace(std::ref(*_cur_header_inclusion_root));
  }
}

void preprocessor::_fixup_inclusion_node_ranges() noexcept
{
  // Fixup the all pending inclusion nodes' range information such
  // that those intersecting a given query range can be found.
  if (_cur_header_inclusion_root ==
      _pp_result->get_header_inclusion_roots().end()) {
    return;
  }
  const raw_pp_token_index cur_raw_end = _pp_result->_raw_tokens.size();
  for (auto it_root = _cur_header_inclusion_root + 1;
       it_root != _pp_result->get_header_inclusion_roots().end();
       ++it_root) {
    it_root->_range.begin = cur_raw_end;
    it_root->_range.end = cur_raw_end;
  }

  for (pp_result::inclusion_node *n = &_inclusions.top().get(); n;
       n = n->_parent) {
    n->_range.end = cur_raw_end;
  }
}

const pp_result::header_inclusion_node& preprocessor::
_raw_tok_it_to_source(const raw_pp_tokens::const_iterator &it)
{
  const raw_pp_token_index index = it - _pp_result->get_raw_tokens().begin();
  return get_pending_token_source(index);
}

const pp_result::header_inclusion_node&
preprocessor::_raw_pp_tokens_range_to_source(const raw_pp_tokens_range &r)
{
  const pp_result::header_inclusion_node &s
    = get_pending_token_source(r.begin);
  assert(r.begin == r.end || &s == &get_pending_token_source(r.end - 1));
  return s;
}

range_in_file preprocessor::
_raw_pp_tokens_range_to_range_in_file(const raw_pp_tokens_range &r)
  const noexcept
{
    if (r.begin == r.end + 1) {
    // A single token, emit "point" range for its start.
    const raw_pp_token &tok = _pp_result->get_raw_tokens()[r.begin];
    const range_in_file &tok_rif = tok.get_range_in_file();
    return range_in_file{tok_rif.begin};
  } else if (r.begin == r.end) {
    // An empty range, used for EOFs. Emit a "point" range for the
    // preceeding token's end.
    assert(r.begin > 0);
    assert(r.begin <= _pp_result->get_raw_tokens().size());
    const raw_pp_token &tok = _pp_result->get_raw_tokens()[r.begin - 1];
    const range_in_file &tok_rif = tok.get_range_in_file();
    return range_in_file{tok_rif.end};
  } else {
    // A real range. It must come from a single header.
    assert(r.end <= _pp_result->get_raw_tokens().size());
    const raw_pp_token &first_tok = _pp_result->get_raw_tokens()[r.begin];
    const raw_pp_token &last_tok = _pp_result->get_raw_tokens()[r.end - 1];
    const range_in_file &first_tok_rif = first_tok.get_range_in_file();
    const range_in_file &last_tok_rif = last_tok.get_range_in_file();
    return range_in_file{first_tok_rif.begin, last_tok_rif.end};
  }
}

void preprocessor::_handle_pp_directive(const _read_ctl ctl)
{
  const raw_pp_token_index raw_begin = _pp_result->_get_last_raw_index();
  raw_pp_token_index raw_end = raw_begin + 1;
  assert(_pp_result->get_raw_tokens()[raw_begin].is_punctuator("#"));
  bool endif_possible = true;
  bool is_endif = false;
  while (true) {
    assert(!_tokenizers.empty());
    auto tok = _tokenizers.top().read_next_token();
    _grab_remarks_from(_tokenizers.top());

    if (tok.is_eof()) {
      _handle_eof_from_tokenizer(tok);
      break;
    }

    _pp_result->_append_token(tok);
    raw_end = _pp_result->_get_last_raw_index() + 1;

    if (tok.is_newline()) {
      break;

    } else if (endif_possible && tok.get_type() == pp_token::type::id &&
	       tok.get_value() == "endif") {
      // Be careful not to read until the end of line or end of file
      // for #endif directives: _handle_eof_from_tokenizer() will
      // throw an exception if there's some conditional inclusion
      // pending.
      is_endif = true;
      endif_possible = false;

      if (!_cur_incl_node_is_cond()) {
	code_remark remark
	  (code_remark::severity::fatal,
	   "#endif without #if",
	   get_pending_token_source(_pp_result->_get_last_raw_index()),
	   tok.get_range_in_file());
	_remarks.add(remark);
	throw pp_except(remark);
      }

      do {
	tok = _tokenizers.top().read_next_token();
	_grab_remarks_from(_tokenizers.top());
	_pp_result->_append_token(tok);
	raw_end = _pp_result->_get_last_raw_index() + 1;
      } while (tok.is_ws() || tok.is_empty());

      if (tok.is_newline()) {
	_pp_result->_append_token(tok);
	raw_end = _pp_result->_get_last_raw_index() + 1;
      } else if (!tok.is_eof()) {
	code_remark remark
	  (code_remark::severity::warning,
	   "garbage after #endif",
	   get_pending_token_source(_pp_result->_get_last_raw_index()),
	   tok.get_range_in_file());
	_remarks.add(remark);
      }

      if (_cond_incl_nesting == _cond_incl_states.size()) {
	_cond_incl_states.top().directive_ranges.emplace_back(raw_begin,
							      raw_end);
	_pop_cond_incl(raw_end);
      }
      --_cond_incl_nesting;

      // Handling EOF is safe now after the conditional inclusion has
      // been popped.
      if (tok.is_eof())
	_handle_eof_from_tokenizer(tok);

      break;

    } else if (!tok.is_ws() && !tok.is_empty()) {
      endif_possible = false;
    }
  }

  _pp_result->_add_directive(raw_pp_tokens_range{raw_begin, raw_end});

  if (is_endif)
    return;

  auto it_tok = _pp_result->get_raw_tokens().begin() + raw_begin + 1;
  if (it_tok == _pp_result->get_raw_tokens().begin() + raw_end) {
    // Null directive terminated by EOF.
    return;
  }

  it_tok = std::find_if(it_tok, _pp_result->get_raw_tokens().begin() + raw_end,
			[](const raw_pp_token &raw_tok) {
			  return !raw_tok.is_ws() && !raw_tok.is_empty();
			});

  if (it_tok->is_newline() ||
      it_tok == _pp_result->get_raw_tokens().begin() + raw_end) {
    // Null directive
    return;
  }

  assert(!it_tok->is_ws() && !it_tok->is_empty());
  if (!it_tok->is_id()) {
    code_remark remark(code_remark::severity::fatal,
		       "identifier expected in preprocessor directive",
		       _raw_tok_it_to_source(it_tok),
		       it_tok->get_range_in_file());
    _remarks.add(remark);
    throw pp_except(remark);
  }


  // First, process conditional inclusion directives: those
  // must be looked at even if within a non-taken branch of
  // another conditional inclusion.
  if (it_tok->get_value() == "if") {
    ++_cond_incl_nesting;
    if (_cond_incl_inactive())
      return;

    _push_cond_incl(raw_begin);
    _cond_incl_states.top().directive_ranges.emplace_back(raw_begin, raw_end);

    if (_eval_conditional_inclusion(raw_pp_tokens_range{raw_begin, raw_end}))
      _enter_cond_incl();

  } else if (it_tok->get_value() == "ifdef") {
    ++_cond_incl_nesting;
    if (_cond_incl_inactive())
      return;

    ++it_tok;
    while (it_tok->is_ws() || it_tok->is_empty())
      ++it_tok;

    if (!it_tok->is_id()) {
      code_remark remark(code_remark::severity::fatal,
			 "identifier expected after #ifdef",
			 _raw_tok_it_to_source(it_tok),
			 it_tok->get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    _push_cond_incl(raw_begin);
    _cond_incl_states.top().directive_ranges.emplace_back(raw_begin, raw_end);

    auto it_m = _macros.find(it_tok->get_value());
    if (it_m != _macros.end()) {
      _cond_incl_states.top().um += it_m->second;
      _enter_cond_incl();
    } else {
      _cond_incl_states.top().mnc +=
	pp_result::macro_nondef_constraint{it_tok->get_value()};
    }

  } else if (it_tok->get_value() == "ifndef") {
    ++_cond_incl_nesting;
    if (_cond_incl_inactive())
      return;

    ++it_tok;
    while (it_tok->is_ws() || it_tok->is_empty())
      ++it_tok;

    if (!it_tok->is_id()) {
      code_remark remark(code_remark::severity::fatal,
			 "identifier expected after #ifndef",
			 _raw_tok_it_to_source(it_tok),
			 it_tok->get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    _push_cond_incl(raw_begin);
    _cond_incl_states.top().directive_ranges.emplace_back(raw_begin, raw_end);

    auto it_m = _macros.find(it_tok->get_value());
    if (it_m != _macros.end()) {
      _cond_incl_states.top().um += it_m->second;
    } else {
      _cond_incl_states.top().mnc +=
	pp_result::macro_nondef_constraint{it_tok->get_value()};
      _enter_cond_incl();
    }

  } else if (it_tok->get_value() == "elif") {
    if (!_cur_incl_node_is_cond()) {
      code_remark remark(code_remark::severity::fatal,
			 "#elif without #if",
			 _raw_tok_it_to_source(it_tok),
			 it_tok->get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    if (_cond_incl_nesting > _cond_incl_states.size())
      return;

    _cond_incl_states.top().directive_ranges.emplace_back(raw_begin, raw_end);

    if (!_cond_incl_states.top().taken_branch) {
      if (_eval_conditional_inclusion(raw_pp_tokens_range{raw_begin, raw_end}))
	_enter_cond_incl();
    } else {
      _cond_incl_states.top().branch_active = false;
    }

  } else if (it_tok->get_value() == "else") {
    if (!_cur_incl_node_is_cond()) {
      code_remark remark(code_remark::severity::fatal,
			 "#else without #if",
			 _raw_tok_it_to_source(it_tok),
			 it_tok->get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    if (_cond_incl_nesting > _cond_incl_states.size())
      return;

    _cond_incl_states.top().directive_ranges.emplace_back(raw_begin, raw_end);

    if (!_cond_incl_states.top().taken_branch) {
      _enter_cond_incl();
    } else {
      _cond_incl_states.top().branch_active = false;
    }
    _cond_incl_states.top().has_else_branch = true;
  }

  // Look at the other directives only if they're not within a
  // non-taken conditional inclusion branch.
  if (_cond_incl_inactive())
    return;

  if (it_tok->get_value() == "define") {
    ++it_tok;
    while (it_tok->is_ws() || it_tok->is_empty())
      ++it_tok;

    if (!it_tok->is_id()) {
      code_remark remark(code_remark::severity::fatal,
			 "identifier expected after #define",
			 _raw_tok_it_to_source(it_tok),
			 it_tok->get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    const pp_result::macro &m =
      _handle_macro_definition(raw_pp_tokens_range{raw_begin, raw_end});

    auto it_existing = _macros.find(m.get_name());
    if (it_existing != _macros.end() && it_existing->second.get() != m) {
      code_remark remark(code_remark::severity::fatal,
		"macro redefined in an incompatible way",
		_raw_pp_tokens_range_to_source(m.get_directive_range()),
		_raw_pp_tokens_range_to_range_in_file(m.get_directive_range()));
      _remarks.add(remark);
      throw pp_except(remark);
    }

    _macros.insert(std::make_pair(m.get_name(), std::ref(m)));
  } else if (it_tok->get_value() == "undef") {
    ++it_tok;
    while (it_tok->is_ws() || it_tok->is_empty())
      ++it_tok;

    if (!it_tok->is_id()) {
      code_remark remark(code_remark::severity::fatal,
			 "identifier expected after #undef",
			 _raw_tok_it_to_source(it_tok),
			 it_tok->get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    auto it_tok_id = it_tok;
    ++it_tok;
    if (!it_tok->is_newline() && !it_tok->is_eof()) {
      code_remark remark(code_remark::severity::fatal,
			 "garbage after #undef",
			 _raw_tok_it_to_source(it_tok),
			 it_tok->get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    _pp_result->_add_macro_undef(it_tok_id->get_value(),
				 raw_pp_tokens_range{raw_begin, raw_end});

    auto it_m = _macros.find(it_tok_id->get_value());
    if (it_m != _macros.end()) {
      _macros.erase(it_m);
    }

  } else if (it_tok->get_value() == "include") {
    if (ctl == _read_ctl::no_file_crossing) {
      code_remark remark(code_remark::severity::fatal,
			 "#include directive in macro invocation",
			 _raw_tok_it_to_source(it_tok),
			 it_tok->get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }

    _handle_include(raw_pp_tokens_range{raw_begin, raw_end});

  }
}

pp_token_index preprocessor::read_next_token()
{
 from_expansion:
  if (!_expanded_tokens.empty()) {
    auto tok = std::move(_expanded_tokens.front());
    _expanded_tokens.pop();
    return _emit_pp_token(*_pp_result, std::move(tok));
  }

  const auto raw_tok = _read_next_plain_token(_read_ctl::normal);
  if (!raw_tok.second) {
    const auto &raw_toks = _pp_result->get_raw_tokens();
    if (raw_toks[raw_tok.first].is_id()) {
      auto &&expansion_result =
	_try_expand_raw(raw_tok.first,
			std::bind(&preprocessor::_read_next_plain_token,
				  this, std::placeholders::_1),
			false);

      if (expansion_result.first) {
	_pp_result->_pp_tokens_origins.append
	  (_pp_result->_macro_invocations.size() - 1,
	   _pp_result->_macro_invocations,
	   pp_result::_pp_tokens_origins::from_macro_expansion_tag{});
	for (auto &&t : expansion_result.second)
	  _expanded_tokens.emplace(std::move(t));
	goto from_expansion;
      }
    }

    _pp_result->_pp_tokens_origins.append
      (raw_tok.first, _pp_result->_macro_invocations,
       pp_result::_pp_tokens_origins::from_raw_tag{});

    return _emit_pp_token(*_pp_result,
			  pp_token{
			    raw_toks[raw_tok.first].get_type(),
			    raw_toks[raw_tok.first].get_value(),
			  });

  } else {
    return _emit_pp_token(*_pp_result,
			  pp_token{
			    pp_token::type::eof,
			    std::string{},
			  });
  }
}

std::pair<bool, std::vector<pp_token>>
preprocessor::_try_expand_raw
  (const raw_pp_token_index id_tok_index,
   const std::function<std::pair<raw_pp_token_index, bool>(const _read_ctl)>
     &raw_token_reader,
   const bool from_cond_incl_cond)
{
  typedef std::vector<pp_token> _pp_tokens_type;

  const auto &raw_tokens = _pp_result->get_raw_tokens();
  assert(raw_tokens[id_tok_index].get_type() == pp_token::type::id);
  const pp_result::macro *m = nullptr;
  auto it_m = _macros.find(raw_tokens[id_tok_index].get_value());
  if (it_m != _macros.end()) {
    m = &it_m->second.get();
  } else if (from_cond_incl_cond &&
	     raw_tokens[id_tok_index].get_value() == "defined") {
    m = &_m_defined_op;
  } else {
    return std::make_pair(false, _pp_tokens_type{});
  }

  using expansion_forest_nodes = _macro_expansion_forest::_forest_nodes;

  _macro_expansion_forest::invocation_source initial_invocation_source;
  _macro_expansion_forest::invocation_source *invocation_source =
    &initial_invocation_source;

  auto read_raw_tok = [&](const _read_ctl read_ctl,
			  const raw_pp_tokens_range&) {
    const auto next_raw_tok = raw_token_reader(read_ctl);
    const raw_pp_token_index raw_tok_index = next_raw_tok.first;
    if (next_raw_tok.second) {
      return (std::make_pair
	      (pp_token{
		 pp_token::type::eof,
		 std::string{},
	       },
	       raw_pp_tokens_range{raw_tok_index, raw_tok_index}));
    }

    if (read_ctl != _read_ctl::peek)
      invocation_source->_append_raw_pp_token(raw_tok_index);

    const auto &raw_tok = raw_tokens[raw_tok_index];
    return (std::make_pair
	    (pp_token{
	      raw_tok.get_type(),
	      raw_tok.get_value(),
	     },
	     raw_pp_tokens_range{raw_tok_index}));
  };

  pp_result::macro_invocation *mi;
  std::vector<_macro_expansion_stack_entry> expansion_stack;
  if (m->is_func_like()) {
    if (m != &_m_defined_op) {
      // The invocation_range is unused by read_raw_tok, but be
      // consistent for maintainability.
      raw_pp_tokens_range invocation_range{id_tok_index};
      auto next_tok = read_raw_tok(_read_ctl::peek, invocation_range);
      invocation_range.end = std::max(invocation_range.end,
				      next_tok.second.end);
      while (next_tok.first.is_ws() || next_tok.first.is_newline() ||
	     next_tok.first.is_empty()) {
	next_tok = read_raw_tok(_read_ctl::peek, invocation_range);
	invocation_range.end = std::max(invocation_range.end,
					next_tok.second.end);
      }

      if (!next_tok.first.is_punctuator("("))
	return std::make_pair(false, _pp_tokens_type{});
    }

    initial_invocation_source._append_raw_pp_token(id_tok_index);
    _scanned_macro_invocation smi =
      _read_func_macro_invocation
	(*m,
	 invocation_source->get_total_spanned_source_range(),
	 read_raw_tok);

    mi =
      &_pp_result->_add_macro_invocation(*m,
					 std::move(initial_invocation_source));
    invocation_source = &mi->_expansion_forest._invocation_src;
    expansion_stack.emplace_back(expansion_forest_nodes::size_type{0},
				 *m, std::move(smi.args_offsets),
				 std::move(smi.args));

  } else {
    initial_invocation_source._append_raw_pp_token(id_tok_index);
    mi =
      &_pp_result->_add_macro_invocation(*m,
					 std::move(initial_invocation_source));
    invocation_source = &mi->_expansion_forest._invocation_src;
    expansion_stack.emplace_back(expansion_forest_nodes::size_type{0},
				 *m);

  }

  expansion_forest_nodes &forest_nodes = mi->_expansion_forest._forest;

  std::stack<_pp_tokens_type> result_stack;
  result_stack.emplace();

  while (true) {
    _macro_expansion_stack_entry &t = expansion_stack.back();
    assert(t.next_arg == t.args.size() || !t.out_tokens);
    if (!t.out_tokens && t.m) {
      // The macro at the top of the expansion stack hasn't been expanded yet.
      // As long as there are macro arguments pending expansion, handle those,
      // one at a time.
      assert(!t.args.size() || t.m);
      if (t.m == &_m_defined_op) {
	// Don't expand the arguent to the defined() operator.
	t.next_arg = t.args.size();
      }
      bool entered_arg_expansion = false;
      while (t.next_arg != t.args.size()) {
	// Add a dummy ID macro expansion spanning the argument's
	// unexpanded tokens and expand it recursively, if needed.
	// The source range is relative to the owner parent's output,
	// that is, it is relative to the same anchor as for the owner.
	const auto arg_src_range_begin
	  = forest_nodes[t.node]._src_range.begin + t.args_offsets[t.next_arg];
	const expansion_forest_nodes::size_type arg_root_node =
	  forest_nodes.add_macro_arg_tree
	  (t.node,
	   _macro_expansion_forest::_rescan_macro_invocation{
	     _macro_expansion_forest::rescan_macro_invocation::output::
	       tokens_range{
	       arg_src_range_begin,
	       arg_src_range_begin + t.args[t.next_arg].size()
	     }
	   });
	if (t.m->shall_expand_arg(t.next_arg)) {
	  expansion_stack.emplace_back(arg_root_node, t.args[t.next_arg]);
	  result_stack.emplace();
	  entered_arg_expansion = true;
	  break;
	}
	++t.next_arg;
      }
      if (entered_arg_expansion)
	continue;

      // All arguments have been expanded as needed, but not the macro
      // itself yet. Do it now.
      if (!t.out_tokens) {
	forest_nodes[t.node]._output._result =
	  _expand_one(forest_nodes[t.node], *mi,
		      std::move(t.args), std::move(t.exp_args),
		      mi->get_source_range());
	t.out_tokens = &forest_nodes[t.node]._output._result[0];
	t.n_out_tokens = forest_nodes[t.node]._output._result.size();
	assert(t.n_out_tokens == forest_nodes[t.node].out_length());
      }

    } else if (t.next_out_token == t.n_out_tokens) {
      // All output tokens from the top expansion stack entry have been
      // processed. Pop it.
      expansion_stack.pop_back();
      if (expansion_stack.empty())
	break;

      _macro_expansion_stack_entry &new_top = expansion_stack.back();
      if (new_top.next_arg != new_top.args.size()) {
	// The popped entry had been associated with some macro
	// argument expansion, move the result to the corresponding
	// slot.
	new_top.exp_args[new_top.next_arg++] = std::move(result_stack.top());
	result_stack.pop();
	assert(!expansion_stack.back().out_tokens);
      }

      continue;
    }

    // Iterate over the macro expansion's output tokens and invoke
    // recursive expansions as appropriate.
    while (t.next_out_token != t.n_out_tokens) {
      const auto cur_out_token_index = t.next_out_token++;
      auto &&cur_out_token = t.out_tokens[cur_out_token_index];

      if (!cur_out_token.is_id()) {
	result_stack.top().emplace_back(std::move(cur_out_token));
	continue;
      }

      // With recursive macro expansion, the tail output tokens from
      // the current expansion can combine with some output tokens
      // from an expansion lower down on the expansion stack, or raw
      // input tokens from the source file even, to form another
      // func-like macro invocation. This must be considered when
      // collecting the token sequence making up a func-like
      // invocation.
      bool peeking = false;
      std::reverse_iterator <
	  std::vector<_macro_expansion_stack_entry>::iterator
	> it_read_stack_entry = expansion_stack.rbegin();
      std::vector<pp_token>::size_type peeking_token_index;
      auto &&read_tok =
	[&](const _read_ctl ctl, const raw_pp_tokens_range &invocation_range) {
	  if (ctl == _read_ctl::peek) {
	    if (!peeking) {
	      it_read_stack_entry = expansion_stack.rbegin();
	      peeking_token_index = it_read_stack_entry->next_out_token;
	      peeking = true;
	    }

	    while (peeking_token_index == it_read_stack_entry->n_out_tokens) {
	      const auto it_next = it_read_stack_entry + 1;
	      if (it_next == expansion_stack.rend()) {
		return read_raw_tok(ctl, invocation_range);
	      } else if (it_next->next_arg != it_next->args.size()) {
		// Don't cross over a macro argument expansion's root
		// entry.
		return (std::make_pair
			(pp_token{pp_token::type::eof, std::string{}},
			 invocation_range));
	      }
	      it_read_stack_entry = it_next;
	      peeking_token_index = it_read_stack_entry->next_out_token;
	    }

	    return (std::make_pair
		    (it_read_stack_entry->out_tokens[peeking_token_index++],
		     invocation_range));
	  } else {
	    if (peeking) {
	      it_read_stack_entry = expansion_stack.rbegin();
	      peeking = false;
	    }

	    while (it_read_stack_entry->next_out_token ==
		   it_read_stack_entry->n_out_tokens) {
	      const auto it_next = it_read_stack_entry + 1;
	      if (it_next == expansion_stack.rend()) {
		return read_raw_tok(ctl, invocation_range);
	      } else if (it_next->next_arg != it_next->args.size()) {
		// Don't cross over a macro argument expansion's root
		// entry.
		return
		  std::make_pair(pp_token{pp_token::type::eof, std::string{}},
				 invocation_range);
	      }
	      it_read_stack_entry = it_next;
	    }

	    return (std::make_pair
		    ((it_read_stack_entry->out_tokens
		      [it_read_stack_entry->next_out_token++]),
		     invocation_range));;
	  }
	};

      m = nullptr;
      it_m = _macros.find(cur_out_token.get_value());
      if (it_m != _macros.end()) {
	if (_macro_is_in_id_tok_expansion_history
	    (it_m->second.get(), mi->_expansion_forest, t.node,
	     cur_out_token_index)) {
	  result_stack.top().emplace_back(std::move(cur_out_token));
	  continue;
	}

	m = &it_m->second.get();

      } else if (from_cond_incl_cond &&
		 cur_out_token.get_value() == "defined") {
	m = &_m_defined_op;

      } else {
	// The current token came from some macro expansion, but
	// doesn't resolve to a macro definition itself. Register a
	// macro_nondef_constraint instance at the
	// macro_invocation. Look ahead to check whether the
	// identifier token is not followed by an opening parenthesis
	// and thus, a function style definition, even if present,
	// would not alter the result.
	auto next_tok =
	  read_tok(_read_ctl::peek,
		   invocation_source->get_total_spanned_source_range());
	while (next_tok.first.is_ws() || next_tok.first.is_newline() ||
	       next_tok.first.is_empty()) {
	  next_tok =
	    read_tok(_read_ctl::peek,
		     invocation_source->get_total_spanned_source_range());
	}

	mi->_macro_nondef_constraints +=
	  pp_result::macro_nondef_constraint{
	    cur_out_token.get_value(),
	    !next_tok.first.is_punctuator("(")
	  };

	result_stack.top().emplace_back(std::move(cur_out_token));
	continue;

      }

      if (m->is_func_like()) {
	if (m != &_m_defined_op) {
	  auto next_tok =
	    read_tok(_read_ctl::peek,
		     invocation_source->get_total_spanned_source_range());
	  while (next_tok.first.is_ws() || next_tok.first.is_newline()
		 || next_tok.first.is_empty()) {
	    next_tok =
	      read_tok(_read_ctl::peek,
		       invocation_source->get_total_spanned_source_range());
	  }

	  if (!next_tok.first.is_punctuator("(")) {
	    // This macro is defined as a function style macro, but
	    // won't get expanded, because it isn't followed by an
	    // opening '('. Register a macro_nondef_constraint for
	    // object style macros only.
	    mi->_macro_nondef_constraints +=
	      pp_result::macro_nondef_constraint{cur_out_token.get_value(),
						 true};

	    result_stack.top().emplace_back(std::move(cur_out_token));
	    continue;
	  }
	}

	_scanned_macro_invocation smi =
	  _read_func_macro_invocation
	    (*m,
	     invocation_source->get_total_spanned_source_range(),
	     read_tok);
	const expansion_forest_nodes::size_type node =
	  forest_nodes.add_rescan_invocation
	  (_macro_expansion_forest::_rescan_macro_invocation{
	     t.node, *m,
	     _macro_expansion_forest::rescan_macro_invocation::output::
	       tokens_range{
	       cur_out_token_index, cur_out_token_index + smi.n_tokens
	     }
	   });
	expansion_stack.emplace_back(node, *m, std::move(smi.args_offsets),
				     std::move(smi.args));
	break;

      } else {
	// Object-like macro invocation.
	const expansion_forest_nodes::size_type node =
	  forest_nodes.add_rescan_invocation
	  (_macro_expansion_forest::_rescan_macro_invocation{
	     t.node, *m,
	     _macro_expansion_forest::rescan_macro_invocation::output::
	       tokens_range{
	       cur_out_token_index, cur_out_token_index + 1
	     }
	   });
	expansion_stack.emplace_back(node, *m);
	break;

      }
    }
  }

  assert(result_stack.size() == 1);
  return std::make_pair(true, std::move(result_stack.top()));
}

bool preprocessor::_macro_is_in_id_tok_expansion_history
(const pp_result::macro &m,
 const _macro_expansion_forest &forest,
 _macro_expansion_forest::nodes_size_type emitting_node_index,
 std::size_t id_tok_index) const noexcept
{
  using rescan_macro_invocation =
    _macro_expansion_forest::rescan_macro_invocation;
  using rescan_output_segment =
    _macro_expansion_forest::rescan_output_segment;
  using rescan_out_tokens_range =
    rescan_macro_invocation::output::tokens_range;
  using rescan_out_fragment =
    rescan_macro_invocation::output::fragment;

  assert(emitting_node_index !=
	 _macro_expansion_forest::nodes_size_type::invalid());
  auto emitting_node = forest[emitting_node_index];
  while (!emitting_node.is_nil_node()) {
    if (!emitting_node.is_arg_tree_root()) {
      if (&emitting_node.get_macro() == &m)
	return true;

      const auto seg =
	static_cast<rescan_output_segment>(emitting_node)
	.crop(rescan_out_tokens_range{id_tok_index, id_tok_index + 1});
      const auto &src_frag = *seg.fragments().first;
      src_frag.fragment
	(wrap_callables<no_default_action>
	 ([&](const rescan_out_fragment::repl_tag&) {
	   const auto &src = *emitting_node.src_begin(false);
	   id_tok_index = src.get_out_range_in_emitting_node().begin;
	   emitting_node = src.get_emitting_node();
	  },
	  [&](const rescan_out_fragment::arg_tag&,
	      const rescan_out_fragment::arg_ref &arg) {
	    const auto arg_expansion_tree =
	      emitting_node.get_arg_tree(arg.get_arg_index());
	    auto it_arg_out_seg = arg_expansion_tree.final_out_begin();
	    id_tok_index =
	      it_arg_out_seg.skip_out_distance
		(src_frag.range_in_fragment.begin);
	    const auto &arg_out_seg = *it_arg_out_seg;
	    id_tok_index += arg_out_seg.get_out_range_in_emitting_node().begin;
	    emitting_node = arg_out_seg.get_emitting_node();
	  },
	  [](const rescan_out_fragment::stringification_tag&,
	     const rescan_out_fragment::stringification&) {
	    assert(0);
	    __builtin_unreachable();
	  },
	  [&](const rescan_out_fragment::concat_tag&,
	      const rescan_out_fragment::concat_sequence&) {
	    const auto &src = *emitting_node.src_begin(false);
	    id_tok_index = src.get_out_range_in_emitting_node().begin;
	    emitting_node = src.get_emitting_node();
	  },
	  [](const rescan_out_fragment::conditional_comma_tag&, bool) {
	    assert(0);
	    __builtin_unreachable();
	  },
	  [](const rescan_out_fragment::empty_tag&) {
	    assert(0);
	    __builtin_unreachable();
	  },
	  [](const rescan_out_fragment::builtin_special_tag&) {
	    assert(0);
	    __builtin_unreachable();
	  }));

    } else {
      auto it_arg_src_seg = emitting_node.src_begin(true);
      id_tok_index = it_arg_src_seg.skip_src_distance(id_tok_index);
      const auto &arg_src_seg = *it_arg_src_seg;
      id_tok_index += arg_src_seg.get_out_range_in_emitting_node().begin;
      emitting_node = arg_src_seg.get_emitting_node();
    }
  }

  return false;
}


preprocessor::_scanned_macro_invocation preprocessor::
_read_func_macro_invocation
	(const pp_result::macro &m,
	 raw_pp_tokens_range invocation_range,
	 const std::function<std::pair<pp_token, raw_pp_tokens_range>
			       (const _read_ctl, const raw_pp_tokens_range&)>
	   &token_reader)
{
  if (&m == &_m_defined_op)
    return _read_defined_operator_invocation(invocation_range, token_reader);

  const std::size_t non_va_arg_count = m.non_va_arg_count();
  const bool is_variadic = m.is_variadic();
  _scanned_macro_invocation result{};

  result.n_tokens = 1; // For the identifier token

  while (true) {
    result.n_tokens++;

    const auto &_tok =
      token_reader(_read_ctl::no_file_crossing, invocation_range);
    const pp_token &tok = _tok.first;
    const raw_pp_tokens_range &tok_src_raw_range = _tok.second;
    invocation_range.end = std::max(invocation_range.end,
				    tok_src_raw_range.end);

    if (tok.is_punctuator("("))
      break;
    assert(tok.is_ws() || tok.is_newline() || tok.is_empty());
  }

  if (non_va_arg_count || is_variadic) {
    const size_t n_args = non_va_arg_count + is_variadic;
    result.args.reserve(n_args);
    char last_delim = 0;
    size_t i_arg;
    for (i_arg = 0; i_arg < non_va_arg_count && last_delim != ')'; ++i_arg) {
      auto cur_arg = _read_macro_arg(invocation_range, token_reader, false);
      result.args.push_back(std::move(std::get<0>(cur_arg)));
      assert(std::get<1>(cur_arg).get_type() == pp_token::type::punctuator);
      assert(std::get<1>(cur_arg).get_value().size() == 1);
      last_delim = std::get<1>(cur_arg).get_value()[0];
      assert(last_delim == ',' || last_delim == ')');
      result.args_offsets.emplace_back(result.n_tokens + std::get<2>(cur_arg));
      result.n_tokens += std::get<2>(cur_arg) + std::get<3>(cur_arg);
      invocation_range.end = std::max(invocation_range.end,
				      std::get<4>(cur_arg));
    }

    if (last_delim == ')' && i_arg < non_va_arg_count) {
      code_remark remark
	(code_remark::severity::fatal,
	 "too few parameters in macro invocation",
	 _raw_pp_tokens_range_to_source(invocation_range),
	 _raw_pp_tokens_range_to_range_in_file(invocation_range));
      _remarks.add(remark);
      throw pp_except(remark);

    } else if (last_delim == ',' && !is_variadic) {
      code_remark remark
	(code_remark::severity::fatal,
	 "too many parameters in macro invocation",
	 _raw_pp_tokens_range_to_source(invocation_range),
	 _raw_pp_tokens_range_to_range_in_file(invocation_range));
      _remarks.add(remark);
      throw pp_except(remark);

    }

    if (is_variadic) {
      if (last_delim != ')') {
	auto cur_arg = _read_macro_arg(invocation_range, token_reader, true);
	result.args.push_back(std::move(std::get<0>(cur_arg)));
	assert(std::get<1>(cur_arg).get_type() == pp_token::type::punctuator);
	assert(std::get<1>(cur_arg).get_value().size() == 1);
	last_delim = std::get<1>(cur_arg).get_value()[0];
	assert(last_delim == ')');
	result.args_offsets.emplace_back(result.n_tokens +
					 std::get<2>(cur_arg));
	result.n_tokens += std::get<2>(cur_arg) + std::get<3>(cur_arg);
	invocation_range.end = std::max(invocation_range.end,
					std::get<4>(cur_arg));

      } else {
	// The variadic argument might simply has been left out, i.e.
	// it's empty.
	result.args.emplace_back();
	result.args_offsets.emplace_back(result.n_tokens);

      }
    }
  } else {
    // Slurp in the tokens between the () and make sure they're all empties.
    auto dummy_arg = _read_macro_arg(invocation_range, token_reader, false);
    invocation_range.end = std::max(invocation_range.end,
				    std::get<4>(dummy_arg));
    assert(std::get<1>(dummy_arg).get_type() == pp_token::type::punctuator);
    bool non_empty_found = false;
    for (auto tok : std::get<0>(dummy_arg)) {
      if (!tok.is_empty() && !tok.is_ws()) {
	non_empty_found = true;
	break;
      }
    }
    if (non_empty_found || std::get<1>(dummy_arg).get_value() != ")") {
      code_remark remark
	(code_remark::severity::fatal,
	 "too many parameters in macro invocation",
	 _raw_pp_tokens_range_to_source(invocation_range),
	 _raw_pp_tokens_range_to_range_in_file(invocation_range));
      _remarks.add(remark);
      throw pp_except(remark);
    }

    result.n_tokens += std::get<2>(dummy_arg) + std::get<3>(dummy_arg);

  }

  return result;
}

std::tuple<std::vector<pp_token>, pp_token,
	   std::size_t, std::size_t, raw_pp_token_index>
preprocessor::
_read_macro_arg(raw_pp_tokens_range invocation_range,
		const
		  std::function<std::pair<pp_token, raw_pp_tokens_range>
				(const _read_ctl, const raw_pp_tokens_range&)>
		  &token_reader,
		const bool variadic)
{
  std::vector<pp_token> arg;
  std::size_t offset = 0;
  std::size_t n_read = 0;
  std::size_t lparens = 0;
  pp_token end_delim{pp_token::type::eof, std::string()};
  bool last_was_ws = false;
  bool in_trailing_ws = false;

  auto &&is_end_delim_tok =
    [&](const pp_token &tok) {
      return (!lparens &&
	      ((!variadic && tok.is_punctuator(",")) ||
	       tok.is_punctuator(")")));
    };

  while (true) {
    auto &&_tok = token_reader(_read_ctl::no_file_crossing, invocation_range);
    pp_token &&tok = std::move(_tok.first);
    const raw_pp_tokens_range &tok_src_raw_range = _tok.second;
    invocation_range.end = std::max(invocation_range.end,
				    tok_src_raw_range.end);
    n_read++;

    if (tok.is_eof()) {
      code_remark remark
	(code_remark::severity::fatal,
	 "no closing right parenthesis in macro invocation",
	 _raw_pp_tokens_range_to_source(tok_src_raw_range),
	 _raw_pp_tokens_range_to_range_in_file(tok_src_raw_range));
      _remarks.add(remark);
      throw pp_except(remark);

    } else if (tok.is_empty() || tok.is_newline() || tok.is_ws()) {
      if (arg.empty()) {
	offset++;
	continue; // Skip whitespace at the beginning.
      }

      // For the first in a sequence of whitespace tokens, Peek ahead
      // to check whether there's some subsequent non-whitespace
      // token. Otherwise strip trailing whitespace.
      bool is_trailing_ws = in_trailing_ws;
      if (!in_trailing_ws && !last_was_ws) {
	is_trailing_ws = true;
	while (is_trailing_ws) {
	  const auto &peeked_tok =
	    token_reader(_read_ctl::peek, invocation_range).first;

	  if (peeked_tok.is_eof() || is_end_delim_tok(peeked_tok)) {
	    break;
	  } else if (!peeked_tok.is_empty() && !peeked_tok.is_newline() &&
		     !peeked_tok.is_ws()) {
	    is_trailing_ws = false;
	  }
	}
      }

      last_was_ws = true;
      if (is_trailing_ws) {
	in_trailing_ws = true;
	continue;
      }

      // Turn newlines into whitespace
      if (tok.is_newline())
	tok.set_type_and_value(pp_token::type::ws, " ");

    } else if (is_end_delim_tok(tok)) {
      // End of macro argument. Skip queued whitespace.
      end_delim = std::move(tok);
      break;

    } else if (tok.is_punctuator("(")) {
      lparens++;
      last_was_ws = false;

    } else if (tok.is_punctuator(")")) {
      lparens--;
      last_was_ws = false;

    } else {
      last_was_ws = false;

    }

    arg.push_back(std::move(tok));
  }

  return std::make_tuple(std::move(arg), std::move(end_delim),
			 offset, n_read - offset, invocation_range.end);
}

preprocessor::_scanned_macro_invocation
preprocessor::_read_defined_operator_invocation
		(raw_pp_tokens_range invocation_range,
		 const
		   std::function<std::pair<pp_token, raw_pp_tokens_range>
				 (const _read_ctl, const raw_pp_tokens_range&)>
		   &token_reader)
{
  _scanned_macro_invocation result{};
  bool with_parens = false;

  result.n_tokens = 1; // For the "defined" identifier token
  result.args.reserve(1);

  auto &&read_tok_skip_ws =
    [&]() {
      while (true) {
	auto &&_tok =
	  token_reader(_read_ctl::no_file_crossing, invocation_range);
	pp_token &&tok = std::move(_tok.first);
	invocation_range = _tok.second;
	result.n_tokens++;
	if (!(tok.is_ws() || tok.is_newline() || tok.is_empty()))
	  return std::move(tok);
      }
    };

  pp_token tok = read_tok_skip_ws();
  if (tok.is_punctuator("(")) {
    with_parens = true;
    tok = read_tok_skip_ws();
  }

  if (tok.is_id()) {
    result.args.emplace_back(std::vector<pp_token>{std::move(tok)});
    result.args_offsets.emplace_back(result.n_tokens - 1);
  } else {
    code_remark remark(code_remark::severity::fatal,
		       "invalid argument to \"defined\" operator",
		       _raw_pp_tokens_range_to_source(invocation_range),
		       _raw_pp_tokens_range_to_range_in_file(invocation_range));
    _remarks.add(remark);
    throw pp_except(remark);
  }

  if (with_parens) {
    tok = read_tok_skip_ws();

    if (!tok.is_punctuator(")")) {
      code_remark remark
		(code_remark::severity::fatal,
		 "no closing parenthesis to \"defined\" operator",
		 _raw_pp_tokens_range_to_source(invocation_range),
		 _raw_pp_tokens_range_to_range_in_file(invocation_range));
    _remarks.add(remark);
    throw pp_except(remark);
    }
  }

  return result;
}

std::vector<pp_token> preprocessor::
_expand_one(_macro_expansion_forest::_rescan_macro_invocation &rmi,
	    pp_result::macro_invocation &mi,
	    std::vector<std::vector<pp_token>> &&args,
	    std::vector<std::vector<pp_token>> &&exp_args,
	    const raw_pp_tokens_range &invocation_range)
{
  const pp_result::macro &m = rmi.get_macro();

  if (m.is_builtin_special()) {
    return _expand_one_builtin_special(rmi, std::move(args),
				       std::move(exp_args), invocation_range);
  }

  // This is the core of macro expansion.
  //
  // The macro expansion will insert empty dummy tokens at various
  // locations to allow the higher levels,
  // preprocessor::read_next_token() in particular, to separate
  // certain productions from surrounding context. These locations are
  // - before and after the whole macro expansion
  // - before and after a concatentation chain
  // - before and after the emission of an expanded macro parameter.
  // This is needed such that
  //    #define __concat(a,b) :a ## b:
  //    :__concat(::a,b::):
  // can evaluate to
  //    : ::ab:: :
  // and so on.
  //
  // For the non-expanded argument values, which are used in
  // concatenations and stringifications, the associated pp_tokens
  // sequence must not have any leading or trailing whitespace or
  // empties.
  //
  const auto &repl_toks = m.get_repl();

  auto &&is_stringification =
    [&](const raw_pp_tokens::size_type repl_pos) {
      return m.is_func_like() && repl_toks[repl_pos].is_punctuator("#");
    };

  auto &&skip_ws =
    [&](raw_pp_tokens::size_type repl_pos) {
      while (repl_pos != repl_toks.size() &&
	     (repl_toks[repl_pos].is_ws() || repl_toks[repl_pos].is_empty())) {
	++repl_pos;
      }
      return repl_pos;
    };

  auto &&advance_repl_pos =
    [&](const raw_pp_tokens::size_type repl_pos) {
      assert(repl_pos != repl_toks.size());
      if (is_stringification(repl_pos)) {
	assert(skip_ws(repl_pos + 1) != repl_toks.size());
	assert(repl_toks[skip_ws(repl_pos + 1)].is_id());
	return skip_ws(repl_pos + 1) + 1;
      }

      return repl_pos + 1;
    };

  auto &&is_vaargs =
    [&](const raw_pp_tokens::size_type repl_pos) {
      return (m.is_variadic() && repl_toks[repl_pos].is_id() &&
	      repl_toks[repl_pos].get_value() == m.get_arg_names().back());
    };

  std::map<std::string, std::vector<std::vector<pp_token>>::size_type>
    args_name_map;
  for (std::vector<std::vector<pp_token>>::size_type i_arg = 0;
       i_arg != m.get_arg_names().size(); ++i_arg) {
    assert(m.shall_expand_arg(i_arg) || exp_args[i_arg].empty());
    args_name_map.insert(std::make_pair(m.get_arg_names()[i_arg], i_arg));
  }

  const std::size_t no_arg_index = std::numeric_limits<std::size_t>::max();
  auto &&resolve_arg =
    [&](const raw_pp_tokens::size_type arg_id_tok_pos) {
      assert(arg_id_tok_pos != repl_toks.size());
      assert(repl_toks[arg_id_tok_pos].is_id());

      if (!m.is_func_like())
	return no_arg_index;

      const auto &name = repl_toks[arg_id_tok_pos].get_value();
      auto it_arg_mapping = args_name_map.find(name);
      if (it_arg_mapping == args_name_map.end())
	return no_arg_index;

      return it_arg_mapping->second;
    };

  auto &&stringify =
    [&](const raw_pp_tokens::size_type arg_index) {
      assert(arg_index != no_arg_index);
      return _stringify(args[arg_index]);
    };

  std::vector<pp_token> result;
  using out_fragment =
    _macro_expansion_forest::rescan_macro_invocation::output::fragment;
  auto &out_fragments = rmi._output;
  // Start the macro expansion with an empty token.
  result.emplace_back(pp_token::type::empty, std::string{});
  out_fragments._add_fragment(out_fragment::empty_tag{});
  bool last_was_ws_or_empty = true;
  for (raw_pp_tokens::size_type repl_pos = skip_ws(0), next_repl_pos;
       repl_pos != repl_toks.size(); repl_pos = next_repl_pos) {
    next_repl_pos = advance_repl_pos(repl_pos);
    auto next_no_ws_repl_pos = skip_ws(next_repl_pos);
    if (next_no_ws_repl_pos != repl_toks.size() &&
	repl_toks[next_no_ws_repl_pos].is_punctuator("##")) {
      auto next_concat_tok_repl_pos = skip_ws(next_no_ws_repl_pos + 1);
      assert(next_concat_tok_repl_pos != repl_toks.size());

      // Handle the GNU extension that in a sequence of
      //  , ## __VA_ARGS__
      // the comma gets removed if __VA_ARGS__ is empty.
      if (repl_toks[repl_pos].is_punctuator(",") &&
	  is_vaargs(next_concat_tok_repl_pos)) {
	const auto lookahead_repl_pos = skip_ws(next_concat_tok_repl_pos + 1);

	if (lookahead_repl_pos == repl_toks.size() ||
	    !repl_toks[lookahead_repl_pos].is_punctuator("##")) {
	  if (!args.back().empty()) {
	    result.emplace_back(repl_toks[repl_pos].get_type(),
				repl_toks[repl_pos].get_value());
	    out_fragments._add_fragment
	      (repl_pos, next_no_ws_repl_pos + 1, false,
	       out_fragment::conditional_comma_tag{});
	    last_was_ws_or_empty = false;
	  } else {
	    out_fragments._add_fragment(repl_pos, next_no_ws_repl_pos + 1, true,
					out_fragment::conditional_comma_tag{});
	  }

	  next_repl_pos = next_concat_tok_repl_pos; // Continue at __VA_ARGS__
	  continue;
	}
      }

      // Otherwise it's a concatenation.
      // Emit an empty token, if needed.
      if (!last_was_ws_or_empty) {
	result.emplace_back(pp_token::type::empty, std::string{});
	out_fragments._add_fragment(out_fragment::empty_tag{});
	last_was_ws_or_empty = true;
      }

      pp_token concat_result{pp_token::type::empty, std::string{}};
      auto concat_frag = out_fragments._add_fragment
	(repl_pos, out_fragment::concat_tag{});
      assert(!repl_toks[repl_pos].is_ws() && !repl_toks[repl_pos].is_empty());
      if (is_stringification(repl_pos)) {
	const auto arg_id_tok_pos = skip_ws(repl_pos + 1);
	const auto arg_index = resolve_arg(arg_id_tok_pos);
	concat_result.set_type_and_value(pp_token::type::str,
					 stringify(arg_index));
	concat_frag.add_component(repl_pos, arg_id_tok_pos + 1,
				  arg_index,
				  out_fragment::stringification_tag{});

      } else {
	auto arg_index = no_arg_index;
	if (repl_toks[repl_pos].is_id())
	  arg_index = resolve_arg(repl_pos);

	if (arg_index != no_arg_index) {
	  const std::vector<pp_token> &arg = args[arg_index];
	  if (!arg.empty()) {
	    if (arg.size() > 1) {
	      // Only the argument's last token participates in
	      // concatenation, the rest gets emitted verbatim.
	      for (auto it_arg_tok = arg.begin();
		   it_arg_tok + 1 != arg.end(); ++it_arg_tok) {
		assert(!it_arg_tok->is_newline());
		result.emplace_back(it_arg_tok->get_type(),
				    it_arg_tok->get_value());
	      }
	    }

	    assert(!arg.back().is_empty() && !arg.back().is_ws() &&
		   !arg.back().is_newline());
	    concat_result.set_type_and_value(arg.back().get_type(),
					     arg.back().get_value());
	    concat_frag.add_component(repl_pos, arg_index, arg.size(),
				      out_fragment::arg_tag{});
	  }
	} else {
	  concat_result.set_type_and_value(repl_toks[repl_pos].get_type(),
					   repl_toks[repl_pos].get_value());
	  concat_frag.add_component(repl_pos, out_fragment::repl_tag{});
	}
      }

      while (next_no_ws_repl_pos != repl_toks.size() &&
	     repl_toks[next_no_ws_repl_pos].is_punctuator("##")) {
	repl_pos = next_concat_tok_repl_pos;
	assert(repl_pos != repl_toks.size());
	assert(!repl_toks[repl_pos].is_ws() && !repl_toks[repl_pos].is_empty());

	if (repl_toks[repl_pos].is_punctuator("##")) {
	  // Multile consecutive ## operators, collate.
	  next_repl_pos = next_no_ws_repl_pos = repl_pos;
	  next_concat_tok_repl_pos = skip_ws(next_no_ws_repl_pos + 1);
	  continue;
	}

	next_repl_pos = advance_repl_pos(repl_pos);
	next_no_ws_repl_pos = skip_ws(next_repl_pos);
	next_concat_tok_repl_pos = (next_no_ws_repl_pos != repl_toks.size() ?
				    skip_ws(next_no_ws_repl_pos + 1) :
				    next_no_ws_repl_pos);
	assert(next_no_ws_repl_pos == repl_toks.size() ||
	       !repl_toks[next_no_ws_repl_pos].is_punctuator("##") ||
	       next_concat_tok_repl_pos != repl_toks.size());

	// Handle the GNU extension that in a sequence of
	//  ... ## , ## __VA_ARGS__,
	// the comma gets removed if __VA_ARGS__ is empty.
	if (repl_toks[repl_pos].is_punctuator(",") &&
	    next_no_ws_repl_pos != repl_toks.size() &&
	    repl_toks[next_no_ws_repl_pos].is_punctuator("##") &&
	    is_vaargs(next_concat_tok_repl_pos)) {
	  const auto lookahead_repl_pos = skip_ws(next_concat_tok_repl_pos + 1);

	  if (lookahead_repl_pos == repl_toks.size() ||
	      !repl_toks[lookahead_repl_pos].is_punctuator("##")) {
	    if (!args.back().empty()) {
	      concat_result = _concat(std::move(concat_result),
				      pp_token{
					repl_toks[repl_pos].get_type(),
					repl_toks[repl_pos].get_value(),
				      },
				      invocation_range);
	      concat_frag.add_component
		(repl_pos, next_no_ws_repl_pos + 1, false,
		 out_fragment::conditional_comma_tag{});
	    } else {
	      concat_frag.add_component(repl_pos, next_no_ws_repl_pos + 1, true,
					out_fragment::conditional_comma_tag{});
	    }

	    next_repl_pos = next_concat_tok_repl_pos; // Continue at __VA_ARGS__
	    break;
	  }
	}

	if (is_stringification(repl_pos)) {
	  const auto arg_id_tok_pos = skip_ws(repl_pos + 1);
	  const auto arg_index = resolve_arg(arg_id_tok_pos);
	  concat_result = _concat(std::move(concat_result),
				  pp_token{
					pp_token::type::str,
					stringify(arg_index),
				  },
				  invocation_range);
	  concat_frag.add_component(repl_pos, arg_id_tok_pos + 1,
				    arg_index,
				    out_fragment::stringification_tag{});

	} else {
	  auto arg_index = no_arg_index;
	  if (repl_toks[repl_pos].is_id())
	    arg_index = resolve_arg(repl_pos);

	  if (arg_index != no_arg_index) {
	    const std::vector<pp_token> &arg = args[arg_index];
	    if (arg.empty())
	      continue;
	    assert(!arg.front().is_empty() && !arg.front().is_ws() &&
		   !arg.front().is_newline());
	    concat_result = _concat(std::move(concat_result),
				    pp_token{
					arg.front().get_type(),
					arg.front().get_value()
				    },
				    invocation_range);
	    concat_frag.add_component(repl_pos, arg_index, arg.size(),
				      out_fragment::arg_tag{});

	    if (arg.size() == 1)
	      continue;

	    // Only the argument's first token participates in the
	    // pending concatenation. Any intermediate surplus tokens
	    // are emitted verbatim, the last one might start a
	    // subsequent concatenation operation, if any.
	    result.emplace_back(std::move(concat_result));
	    for (auto it_arg_tok = arg.begin() + 1;
		 it_arg_tok + 1 != arg.end(); ++it_arg_tok) {
	      assert(!it_arg_tok->is_newline());
	      result.emplace_back(it_arg_tok->get_type(),
				  it_arg_tok->get_value());
	    }

	    assert(!arg.back().is_empty() && !arg.back().is_ws() &&
		   !arg.back().is_newline());
	    concat_result.set_type_and_value(arg.back().get_type(),
					     arg.back().get_value());

	  } else {
	    concat_result = _concat(std::move(concat_result),
				    pp_token{
					repl_toks[repl_pos].get_type(),
					repl_toks[repl_pos].get_value(),
				    },
				    invocation_range);
	    concat_frag.add_component(repl_pos, out_fragment::repl_tag{});

	  }
	}
      }

      result.emplace_back(std::move(concat_result));
      result.emplace_back(pp_token::type::empty, std::string{});
      out_fragments._add_fragment(out_fragment::empty_tag{});
      last_was_ws_or_empty = true;

    } else if (is_stringification(repl_pos)) {
      const auto arg_id_tok_pos = skip_ws(repl_pos + 1);
      const auto arg_index = resolve_arg(arg_id_tok_pos);
      result.emplace_back(pp_token::type::str, stringify(arg_index));
      out_fragments._add_fragment(repl_pos, arg_id_tok_pos + 1, arg_index,
				  out_fragment::stringification_tag{});
      last_was_ws_or_empty = false;

    } else {
      auto arg_index = no_arg_index;
      if (repl_toks[repl_pos].is_id())
	arg_index = resolve_arg(repl_pos);

      if (arg_index != no_arg_index) {
	if (!last_was_ws_or_empty) {
	  result.emplace_back(pp_token::type::empty, std::string{});
	  out_fragments._add_fragment(out_fragment::empty_tag{});
	}

	const std::vector<pp_token> &arg = exp_args[arg_index];
	for (const auto &arg_tok : arg) {
	  assert(!arg_tok.is_newline());
	  result.emplace_back(arg_tok.get_type(), arg_tok.get_value());
	}
	out_fragments._add_fragment(repl_pos, arg_index, arg.size(),
				    out_fragment::arg_tag{});

	if (!arg.empty() && !arg.back().is_empty()) {
	  result.emplace_back(pp_token::type::empty, std::string{});
	  out_fragments._add_fragment(out_fragment::empty_tag{});
	}
	last_was_ws_or_empty = true;

      } else {
	result.emplace_back(repl_toks[repl_pos].get_type(),
			    repl_toks[repl_pos].get_value());
	out_fragments._add_fragment(repl_pos, out_fragment::repl_tag{});
	last_was_ws_or_empty = repl_toks[repl_pos].is_ws();

      }
    }
  }

  // Terminate the macro expansion with an empty token.
  if (!last_was_ws_or_empty) {
    result.emplace_back(pp_token::type::empty, std::string{});
    out_fragments._add_fragment(out_fragment::empty_tag{});
  }

  mi._used_macros += m;

  return result;
}

std::vector<pp_token> preprocessor::
_expand_one_builtin_special
			(_macro_expansion_forest::_rescan_macro_invocation &rmi,
			 std::vector<std::vector<pp_token>> &&args,
			 std::vector<std::vector<pp_token>> &&exp_args,
			 const raw_pp_tokens_range &invocation_range)
{
  const pp_result::macro &m = rmi.get_macro();
  std::vector<pp_token> result;
  using out_fragment =
    _macro_expansion_forest::rescan_macro_invocation::output::fragment;
  auto &out_fragments = rmi._output;

  if (&m == &_m_defined_op) {
    assert(args.size() == 1 && args[0].size() == 1 && args[0][0].is_id());
    const std::string &id = args[0][0].get_value();
    bool is_defined = false;
    auto it_m = _macros.find(id);
    if (it_m != _macros.end()) {
      assert(!_cond_incl_states.empty());
      _cond_incl_states.top().um += it_m->second;
      is_defined = true;
    } else {
      assert(!_cond_incl_states.empty());
      _cond_incl_states.top().mnc += pp_result::macro_nondef_constraint{id};
    }

    result.emplace_back(pp_token::type::pp_number,
			is_defined ? "1" : "0");
    out_fragments._add_fragment(1, out_fragment::builtin_special_tag{});

  } else if (m.get_name() == "__FILE__") {
    const auto &source =
      _raw_pp_tokens_range_to_source(invocation_range);

    result.emplace_back(pp_token::type::str, source.get_filename());
    out_fragments._add_fragment(1, out_fragment::builtin_special_tag{});

  } else if (m.get_name() == "__BASE_FILE__") {
    result.emplace_back(pp_token::type::str, _base_file);
    out_fragments._add_fragment(1, out_fragment::builtin_special_tag{});

  } else if (m.get_name() == "__LINE__") {
      const auto &source =
	_raw_pp_tokens_range_to_source(invocation_range);
      const auto &range_in_file =
	_raw_pp_tokens_range_to_range_in_file(invocation_range);
      const auto line = source.offset_to_line_col(range_in_file.begin).first;

      result.emplace_back(pp_token::type::pp_number, std::to_string(line));
      out_fragments._add_fragment(1, out_fragment::builtin_special_tag{});

  } else if (m.get_name() == "__INCLUDE_LEVEL__") {
    result.emplace_back(pp_token::type::pp_number,
			std::to_string(_tokenizers.size() - 1));
    out_fragments._add_fragment(1, out_fragment::builtin_special_tag{});

  } else if (m.get_name() == "__COUNTER__") {
    result.emplace_back(pp_token::type::pp_number,
			std::to_string(__counter__++));
    out_fragments._add_fragment(1, out_fragment::builtin_special_tag{});

  } else {
    // A builtin special macro registered through
    // register_builtin_special_macro(), e.g. __has_attribute(): the
    // target computes the value from the expanded arguments.
    const auto it_handler = _builtin_special_macro_handlers.find(m.get_name());
    assert(it_handler != _builtin_special_macro_handlers.end());
    assert(exp_args.size() == m.get_arg_names().size());
    const auto report_fatal =
      [&](const std::string &msg) {
	code_remark remark
	  (code_remark::severity::fatal, msg,
	   _raw_pp_tokens_range_to_source(invocation_range),
	   _raw_pp_tokens_range_to_range_in_file(invocation_range));
	_remarks.add(remark);
	return pp_except(remark);
      };
    result.push_back(it_handler->second(exp_args, report_fatal));
    out_fragments._add_fragment(1, out_fragment::builtin_special_tag{});
  }

  return result;
}

std::string preprocessor::
_stringify(const std::vector<pp_token> &arg_toks)
{
  assert(arg_toks.empty() ||
	 !(arg_toks.front().is_ws() || arg_toks.front().is_empty() ||
	   arg_toks.back().is_ws() || arg_toks.back().is_empty()));

  // Concatenate the stringified argument tokens
  std::string s;
  bool last_was_ws = false;
  for (auto &&it_tok = arg_toks.begin(); it_tok != arg_toks.end(); ++it_tok) {
    assert(!it_tok->is_newline());
    if (it_tok->is_empty()) {
      continue;
    } else if (it_tok->is_ws()) {
      // Multiple consecutive whitespace tokens get transformed into a single
      // whitespace character.
      if (!last_was_ws) {
	s += ' ';
	last_was_ws = true;
      }
    } else {
      s += it_tok->stringify();
      last_was_ws = false;
    }
  }

  // And escape '\' and '"' characters
  for (auto &&it = s.begin(); it != s.end();) {
    if (*it == '"' || *it == '\\') {
      it = s.insert(it, '\\');
      it += 2;
    } else {
      ++it;
    }
  }

  return s;
}

pp_token preprocessor::
_concat(pp_token &&tok1, pp_token &&tok2,
	const raw_pp_tokens_range &invocation_range)
{
  assert(tok1.get_type() != pp_token::type::ws);
  assert(tok2.get_type() != pp_token::type::ws);
  assert(tok1.get_type() != pp_token::type::newline);
  assert(tok2.get_type() != pp_token::type::newline);
  assert(tok1.get_type() != pp_token::type::eof);
  assert(tok2.get_type() != pp_token::type::eof);
  assert(tok1.get_type() != pp_token::type::qstr);
  assert(tok2.get_type() != pp_token::type::qstr);
  assert(tok1.get_type() != pp_token::type::hstr);
  assert(tok2.get_type() != pp_token::type::hstr);

  if (tok1.get_type() == pp_token::type::empty) {
    return std::move(tok2);
  } else if (tok2.get_type() == pp_token::type::empty) {
    return std::move(tok1);
  }

  if (tok1.get_type() == pp_token::type::id &&
      tok2.get_type() == pp_token::type::pp_number) {
    if (!std::all_of(tok2.get_value().cbegin(), tok2.get_value().cend(),
		     [](const char c) {
		       return (c == '_' || ('a' <= c && c <= 'z') ||
			       ('A' <= c && c <= 'Z') ||
			       ('0' <= c && c <= '9'));
		     })) {
      code_remark remark
	(code_remark::severity::fatal,
	 "can't concatenate " + tok1.get_value() + " and " + tok2.get_value(),
	 _raw_pp_tokens_range_to_source(invocation_range),
	 _raw_pp_tokens_range_to_range_in_file(invocation_range));
      _remarks.add(remark);
      throw pp_except(remark);
    }

    return pp_token{pp_token::type::id,
		    tok1.get_value() + tok2.get_value()};

  } else if (tok1.get_type() == pp_token::type::pp_number &&
	     tok2.get_type() == pp_token::type::id) {
    return pp_token{pp_token::type::pp_number,
		    tok1.get_value() + tok2.get_value()};

  } else if (tok1.get_type() == pp_token::type::pp_number &&
	     tok2.get_type() == pp_token::type::punctuator &&
	     (tok2.get_value() == "-" || tok2.get_value() == "+")) {
    return pp_token{pp_token::type::pp_number,
		    tok1.get_value() + tok2.get_value()};
  }

  if (tok1.get_type() != tok2.get_type()) {
    code_remark remark
      (code_remark::severity::fatal,
       "can't concatenate tokens of different type",
       _raw_pp_tokens_range_to_source(invocation_range),
       _raw_pp_tokens_range_to_range_in_file(invocation_range));
    _remarks.add(remark);
    throw pp_except(remark);
  } else if(tok1.get_type() == pp_token::type::chr ||
	    tok1.get_type() == pp_token::type::wchr ||
	    tok1.get_type() == pp_token::type::uchr16 ||
	    tok1.get_type() == pp_token::type::uchr32) {
    code_remark remark
      (code_remark::severity::fatal,
       "can't concatenate character constants",
       _raw_pp_tokens_range_to_source(invocation_range),
       _raw_pp_tokens_range_to_range_in_file(invocation_range));
    _remarks.add(remark);
    throw pp_except(remark);
  } else if (tok1.get_type() == pp_token::type::str ||
	     tok1.get_type() == pp_token::type::wstr ||
	     tok1.get_type() == pp_token::type::ustr8 ||
	     tok1.get_type() == pp_token::type::ustr16 ||
	     tok1.get_type() == pp_token::type::ustr32 ||
	     tok1.get_type() == pp_token::type::qstr ||
	     tok1.get_type() == pp_token::type::hstr) {
    code_remark remark
      (code_remark::severity::fatal,
       "can't concatenate string literals",
       _raw_pp_tokens_range_to_source(invocation_range),
       _raw_pp_tokens_range_to_range_in_file(invocation_range));
    _remarks.add(remark);
    throw pp_except(remark);
  } else if (tok1.get_type() == pp_token::type::non_ws_char) {
    code_remark remark
      (code_remark::severity::fatal,
       "can't concatenate " + tok1.get_value() + " and " + tok2.get_value(),
       _raw_pp_tokens_range_to_source(invocation_range),
       _raw_pp_tokens_range_to_range_in_file(invocation_range));
    _remarks.add(remark);
    throw pp_except(remark);
  }

  std::string value = tok1.get_value() + tok2.get_value();
  if (tok1.get_type() == pp_token::type::punctuator) {
    if (value != "->" && value != "++" && value != "--" && value != "<<" &&
	value != ">>" && value != "<=" && value != ">=" && value != "==" &&
	value != "!=" && value != "&&" && value != "||" && value != "*=" &&
	value != "/=" && value != "%=" && value != "+=" && value != "-=" &&
	value != "<<=" && value != ">>=" && value != "&=" &&
	value != "^=" && value != "|=" && value != "##" && value != "<:" &&
	value != ":>" && value != "<%" && value != "%>" && value != "%:" &&
	value != "%:%:") {
      code_remark remark
	(code_remark::severity::fatal,
	 "can't concatenate " + tok1.get_value() + " and " + tok2.get_value(),
	 _raw_pp_tokens_range_to_source(invocation_range),
	 _raw_pp_tokens_range_to_range_in_file(invocation_range));
      _remarks.add(remark);
      throw pp_except(remark);
    }
  }

  return pp_token{tok1.get_type(), std::move(value)};
}

void preprocessor::_handle_include(const raw_pp_tokens_range &directive_range)
{
  const auto &raw_toks = _pp_result->get_raw_tokens();
  auto raw_tok_index = directive_range.begin;

  assert(raw_toks[raw_tok_index].is_punctuator("#"));
  ++raw_tok_index;

  if (raw_toks[raw_tok_index].is_ws())
    ++raw_tok_index;

  assert(raw_toks[raw_tok_index].is_id());
  assert(raw_toks[raw_tok_index].get_value() == "include");
  ++raw_tok_index;

  if (raw_toks[raw_tok_index].is_ws())
    ++raw_tok_index;

  assert(raw_tok_index != directive_range.end);

  auto &&raw_tok_index_to_range_in_file =
    [&](const raw_pp_token_index raw_tok_index) {
      // Shrink range to start location
      return range_in_file{raw_toks[raw_tok_index].get_range_in_file().begin};
    };
  std::string unresolved;
  bool is_qstr;
  pp_result::used_macros um;
  pp_result::macro_nondef_constraints mnc;
  if (raw_toks[raw_tok_index].is_type_any_of<pp_token::type::qstr,
					     pp_token::type::hstr>()) {
    unresolved = raw_toks[raw_tok_index].get_value();
    is_qstr = raw_toks[raw_tok_index].get_type() == pp_token::type::qstr;
    ++raw_tok_index;
    if (raw_toks[raw_tok_index].is_ws())
      ++raw_tok_index;

    if (!raw_toks[raw_tok_index].is_newline() &&
	!raw_toks[raw_tok_index].is_eof()) {
      code_remark remark(code_remark::severity::fatal,
			 "garbage after #include",
			 get_pending_token_source(raw_tok_index),
			 raw_tok_index_to_range_in_file(raw_tok_index));
      _remarks.add(remark);
      throw pp_except(remark);
    }
  } else {
    // Run a macro expansion. Temporarily append the expanded pp_token
    // instances to _pp_result and drop them afterwards again.
    auto peeked_raw_tok_index = raw_toks.size();
    auto read_tok = [&](const _read_ctl ctl) {
      if (peeked_raw_tok_index != raw_toks.size() && ctl != _read_ctl::peek) {
	const raw_pp_token_index next_raw_tok_index = peeked_raw_tok_index;
	if (++peeked_raw_tok_index == raw_tok_index)
	  peeked_raw_tok_index = raw_toks.size();

	return std::make_pair(next_raw_tok_index, false);
      }

      if (raw_toks[raw_tok_index].is_newline() ||
	  raw_toks[raw_tok_index].is_eof()) {
	return std::make_pair(raw_tok_index, true);
      }

      if (ctl == _read_ctl::peek && peeked_raw_tok_index == raw_toks.size())
	peeked_raw_tok_index = raw_tok_index;

      const raw_pp_token_index next_tok_index = raw_tok_index;
      ++raw_tok_index;
      return std::make_pair(next_tok_index, false);
    };

    const pp_tokens::size_type temp_tokens_tail_begin
      = _pp_result->get_pp_tokens().size();
    while (true) {
      const auto raw_tok = read_tok(_read_ctl::normal);
      if (!raw_tok.second) {
	if (raw_toks[raw_tok.first].is_id()) {
	  auto &&expansion_result =
	    _try_expand_raw(raw_tok.first, read_tok, false);
	  if (expansion_result.first) {
	    _pp_result->_pp_tokens_origins.append
	      (_pp_result->_macro_invocations.size() - 1,
	       _pp_result->_macro_invocations,
	       pp_result::_pp_tokens_origins::from_macro_expansion_tag{});
	    for (auto &&t : expansion_result.second)
	      _emit_pp_token(*_pp_result, std::move(t));
	    continue;
	  }
	}

	_pp_result->_pp_tokens_origins.append
	  (raw_tok.first, _pp_result->_macro_invocations,
	   pp_result::_pp_tokens_origins::from_raw_tag{});

	_emit_pp_token(*_pp_result,
		       pp_token{
			 raw_toks[raw_tok.first].get_type(),
			 raw_toks[raw_tok.first].get_value(),
		       });

      } else {
	_emit_pp_token(*_pp_result,
		       pp_token{pp_token::type::eof,std::string{}});
	break;
      }
    }

    auto skip = [&] (pp_tokens::const_iterator it) {
	while(it->is_type_any_of<pp_token::type::ws,
				 pp_token::type::empty>()) {
	assert(!it->is_newline());
	++it;
      }
      return it;
    };

    auto it_tok = _pp_result->get_pp_tokens().cbegin() + temp_tokens_tail_begin;
    it_tok = skip(it_tok);

    if (it_tok->is_eof()) {
      code_remark remark(code_remark::severity::fatal,
			 "macro expansion at #include evaluated to nothing",
			 get_pending_token_source(directive_range.begin),
			 raw_tok_index_to_range_in_file(directive_range.begin));
      _remarks.add(remark);
      throw pp_except(remark);
    }

    if (it_tok->is_punctuator("<")) {
      is_qstr = false;

      ++it_tok;
      for (auto it_next = skip(it_tok + 1); !it_next->is_eof();
	   it_tok = it_next, it_next = skip(it_next + 1)) {
	unresolved += it_tok->stringify();
      }

      assert(!it_tok->is_eof());
      if (!it_tok->is_punctuator(">")) {
	code_remark remark
	  (code_remark::severity::fatal,
	   "macro expansion at #include does not end with '>'",
	   get_pending_token_source(directive_range.begin),
	   raw_tok_index_to_range_in_file(directive_range.begin));
	_remarks.add(remark);
	throw pp_except(remark);
      } else if (unresolved.empty()) {
	code_remark remark
	  (code_remark::severity::fatal,
	   "macro expansion at #include gives \"<>\"",
	   get_pending_token_source(directive_range.begin),
	   raw_tok_index_to_range_in_file(directive_range.begin));
	_remarks.add(remark);
	throw pp_except(remark);
      }
    } else if (it_tok->get_type() == pp_token::type::str) {
      is_qstr = true;
      unresolved = it_tok->get_value();

      it_tok = skip(it_tok + 1);
      if (!it_tok->is_eof()) {
	code_remark remark
	  (code_remark::severity::fatal,
	   "macro expansion at #include yields garbage at end",
	   get_pending_token_source(directive_range.begin),
	   raw_tok_index_to_range_in_file(directive_range.begin));
	_remarks.add(remark);
	throw pp_except(remark);
      }
    } else {
      code_remark remark(code_remark::severity::fatal,
			 "macro expansion at #include doesn't conform",
			 get_pending_token_source(directive_range.begin),
			 raw_tok_index_to_range_in_file(directive_range.begin));
      _remarks.add(remark);
      throw pp_except(remark);
    }

    std::tie(um, mnc) =
      _pp_result->_drop_pp_tokens_tail(temp_tokens_tail_begin);
  }

  std::string resolved
    = (is_qstr ?
       _header_resolver.resolve(unresolved,
				(_tokenizers.top().get_header_inclusion_node()
				 .get_filename())) :
       _header_resolver.resolve(unresolved));

  if (resolved.empty()) {
    code_remark remark(code_remark::severity::fatal,
		       "could not find header from #include",
		       get_pending_token_source(directive_range.begin),
		       raw_tok_index_to_range_in_file(directive_range.begin));
    _remarks.add(remark);
    throw pp_except(remark);
  }

  pp_result::header_inclusion_child &new_header_inclusion_node
    = _inclusions.top().get()._add_header_inclusion(resolved,
						    directive_range,
						    std::move(um),
						    std::move(mnc),
						    *_pp_result);
  _tokenizers.emplace(new_header_inclusion_node);
  _inclusions.emplace(std::ref(new_header_inclusion_node));
}

void preprocessor::_push_cond_incl(const raw_pp_token_index range_begin)
{
  pp_result::conditional_inclusion_node &node
    = _inclusions.top().get()._add_conditional_inclusion(range_begin);
  _cond_incl_states.emplace(std::ref(node));
  _inclusions.emplace(std::ref(node));
}

void preprocessor::_pop_cond_incl(const raw_pp_token_index range_end)
{
  _cond_incl_state &cond_incl_state = _cond_incl_states.top();
  cond_incl_state.incl_node.get()._finalize
    (range_end,
     std::move(cond_incl_state.um), std::move(cond_incl_state.mnc),
     std::move(cond_incl_state.directive_ranges),
     cond_incl_state.taken_branch,
     cond_incl_state.has_else_branch);
  assert(&cond_incl_state.incl_node.get() == &_inclusions.top().get());
  _inclusions.pop();
  _cond_incl_states.pop();
}

bool preprocessor::_cur_incl_node_is_cond() const noexcept
{
  if (_cond_incl_states.empty())
    return false;

  return &_inclusions.top().get() == &_cond_incl_states.top().incl_node.get();
}

void preprocessor::_enter_cond_incl()
{
  _cond_incl_state &cond_incl_state = _cond_incl_states.top();
  cond_incl_state.branch_active = true;
  cond_incl_state.taken_branch = cond_incl_state.directive_ranges.size();;
}

bool preprocessor::_cond_incl_inactive() const noexcept
{
  return !_cond_incl_states.empty() && !_cond_incl_states.top().branch_active;
}

bool preprocessor::
_eval_conditional_inclusion(const raw_pp_tokens_range &directive_range)
{
  const auto &raw_toks = _pp_result->get_raw_tokens();
  auto raw_tok_index = directive_range.begin + 1;
  if (raw_toks[raw_tok_index].is_ws())
    ++raw_tok_index;
  assert(raw_toks[raw_tok_index].get_type() == pp_token::type::id &&
	 (raw_toks[raw_tok_index].get_value() == "if" ||
	  raw_toks[raw_tok_index].get_value() == "elif"));
  ++raw_tok_index;

  // Temporarily append the expanded pp_token instances from the
  // condition to _pp_result. They'll get purged again when we're done
  // with them.
  _fixup_inclusion_node_ranges();
  auto peeked_raw_tok_index = raw_toks.size();
  auto read_tok =
    [&](const _read_ctl ctl) {
      if (peeked_raw_tok_index != raw_toks.size() && ctl != _read_ctl::peek) {
	const raw_pp_token_index next_raw_tok_index = peeked_raw_tok_index;
	if (++peeked_raw_tok_index == raw_tok_index)
	  peeked_raw_tok_index = raw_toks.size();

	return std::make_pair(next_raw_tok_index, false);
      }

      if (raw_toks[raw_tok_index].is_newline() ||
	  raw_toks[raw_tok_index].is_eof()) {
	return std::make_pair(raw_tok_index, true);
      }

      if (ctl == _read_ctl::peek && peeked_raw_tok_index == raw_toks.size())
	peeked_raw_tok_index = raw_tok_index;

      const raw_pp_token_index next_tok_index = raw_tok_index;
      ++raw_tok_index;
      return std::make_pair(next_tok_index, false);
    };

  const pp_tokens::size_type temp_tokens_tail_begin
    = _pp_result->get_pp_tokens().size();
  while (true) {
      const auto raw_tok = read_tok(_read_ctl::normal);
      if (!raw_tok.second) {
	if (raw_toks[raw_tok.first].is_id()) {
	  auto &&expansion_result =
	    _try_expand_raw(raw_tok.first, read_tok, true);
	  if (expansion_result.first) {
	    _pp_result->_pp_tokens_origins.append
	      (_pp_result->_macro_invocations.size() - 1,
	       _pp_result->_macro_invocations,
	       pp_result::_pp_tokens_origins::from_macro_expansion_tag{});
	    for (auto &&t : expansion_result.second)
	      _emit_pp_token(*_pp_result, std::move(t));
	    continue;
	  }
	}

	_pp_result->_pp_tokens_origins.append
	  (raw_tok.first, _pp_result->_macro_invocations,
	   pp_result::_pp_tokens_origins::from_raw_tag{});

	_emit_pp_token(*_pp_result,
		       pp_token{
			 raw_toks[raw_tok.first].get_type(),
			 raw_toks[raw_tok.first].get_value(),
		       });

      } else {
	_emit_pp_token(*_pp_result,
		       pp_token{pp_token::type::eof, std::string{}});
	break;
      }
  }

  pp_tokens::size_type read_token_index = temp_tokens_tail_begin;
  auto &&exp_read_tok = [&]() { return read_token_index++; };
  yy::pp_expr_parser_driver pd(exp_read_tok, *_pp_result);
  try {
    pd.parse();
  } catch (const pp_except&) {
    _remarks += pd.get_remarks();
    pd.get_remarks().clear();
    throw;
  } catch (const parse_except&) {
    _remarks += pd.get_remarks();
    pd.get_remarks().clear();
    throw;
  }

  ast::ast_pp_expr a = pd.grab_result();
  bool result;
  try {
    result = a.evaluate(_tgt);
  } catch (const semantic_except&) {
    _remarks += a.get_remarks();
    a.get_remarks().clear();
    throw;
  }
  _remarks += a.get_remarks();
  a.get_remarks().clear();

  auto um_mnc = _pp_result->_drop_pp_tokens_tail(temp_tokens_tail_begin);
  _cond_incl_states.top().um += std::move(um_mnc.first);
  _cond_incl_states.top().mnc += std::move(um_mnc.second);

  return result;
}

const pp_result::macro& preprocessor::
_handle_macro_definition(const raw_pp_tokens_range &directive_range)
{
  const raw_pp_tokens::const_iterator begin =
    _pp_result->get_raw_tokens().begin() + directive_range.begin;
  raw_pp_tokens::const_iterator end =
    _pp_result->get_raw_tokens().begin() + directive_range.end - 1;
  assert (begin != end);
  assert(end != _pp_result->get_raw_tokens().end());
  assert(begin->is_punctuator("#"));
  assert(end->is_newline() || end->is_eof());

  auto skip_ws = [&end](const decltype(end) it) {
    return std::find_if(it, end,
			[](const raw_pp_token &raw_tok) {
			  return !raw_tok.is_ws() && !raw_tok.is_empty();
			});
  };

  auto it = skip_ws(begin + 1);
  assert(it->is_id() && it->get_value() == "define");
  it = skip_ws(it + 1);

  auto report_fatal =
    [&](const std::string &msg, const raw_pp_tokens::const_iterator &tok) {
      code_remark remark(code_remark::severity::fatal, msg,
			 _raw_tok_it_to_source(tok),
			 tok->get_range_in_file());
      _remarks.add(remark);
      return pp_except{remark};
    };

  std::string name;
  bool func_like;
  std::vector<std::string> arg_names;
  bool variadic;
  std::tie(it, name, func_like, arg_names, variadic)
    = _parse_macro_signature(it, end, report_fatal);

  it = it = std::find_if(it, end,
			 [](const raw_pp_token &raw_tok) {
			   return !raw_tok.is_empty();
			 });
  if (it != end && !it->is_ws()) {
    code_remark remark(code_remark::severity::warning,
		       "no whitespace before macro replacement list",
		       _raw_tok_it_to_source(it),
		       it->get_range_in_file());
    _remarks.add(remark);
  }

  raw_pp_tokens repl_toks{it, end};
  _check_macro_repl(repl_toks.cbegin(), repl_toks.cend(), func_like,
		    arg_names, report_fatal);
  const raw_pp_tokens_range repl_range{
    static_cast<pp_token_index>(it - _pp_result->get_raw_tokens().begin()),
    static_cast<pp_token_index>(end - _pp_result->get_raw_tokens().begin())
  };

  return _pp_result->_add_macro(name, func_like, variadic,
				std::move(arg_names), std::move(repl_toks),
				repl_range, directive_range);
}

std::tuple<raw_pp_tokens::const_iterator,
	   std::string,
	   bool,
	   std::vector<std::string>,
	   bool>
preprocessor::_parse_macro_signature
			(const raw_pp_tokens::const_iterator begin,
			 const raw_pp_tokens::const_iterator end,
			 const _report_fatal_at_raw_tok_type &report_fatal)
{
  auto skip_ws = [&end](const decltype(end) it) {
    return std::find_if(it, end,
			[](const raw_pp_token &raw_tok) {
			  return !raw_tok.is_ws() && !raw_tok.is_empty();
			});
  };

  auto it = skip_ws(begin);
  if (it == end || !it->is_id()) {
    throw report_fatal("no identifier in macro definition", begin);
  }

  const std::string &name = it->get_value();
  ++it;

  bool func_like = false;
  bool variadic = false;
  std::vector<std::string> arg_names;
  if (it != end && it->is_punctuator("(")) {
    it = skip_ws(it + 1);
    func_like = true;

    while (it != end && !it->is_punctuator(")")) {
      if (variadic)
	throw report_fatal("garbage in macro argument list after ...", it);

      if (!arg_names.empty() && !it->is_punctuator(",")) {
	throw report_fatal("comma expected in macro argument list", it);
      } else if (it->is_punctuator(",")) {
	if (arg_names.empty())
	  throw report_fatal("leading comma in macro argument list", it);
	it = skip_ws(it + 1);
	if (it == end || it->is_punctuator(")"))
	  throw report_fatal("trailing comma in macro argument list", it);
      }

      if (it->is_punctuator("...")) {
	variadic = true;
	arg_names.push_back("__VA_ARGS__");
	it = skip_ws(it + 1);
      } else if (it->is_id()) {
	if (it->get_value() == "__VA_ARGS__") {
	  throw report_fatal("__VA_ARGS__ not allowed as a macro argument name",
			     it);
	} else if (std::find(arg_names.cbegin(), arg_names.cend(),
			     it->get_value())
		   != arg_names.cend()) {
	  throw report_fatal("duplicate macro argument name", it);
	}

	arg_names.push_back(it->get_value());
	it = skip_ws(it + 1);
	if (it != end && it->is_punctuator("...")) {
	  variadic = true;
	  it = skip_ws(it + 1);
	}
      } else {
	throw report_fatal("garbage in macro argument list", it);
      }
    }
    if (it == end)
      throw report_fatal("macro argument list not closed", it - 1);

    assert(it->is_punctuator(")"));
    ++it;
  }

  return std::make_tuple(it, name, func_like, std::move(arg_names), variadic);
}

void preprocessor::
_check_macro_repl(raw_pp_tokens::const_iterator begin,
		  raw_pp_tokens::const_iterator end,
		  const bool func_like,
		  const std::vector<std::string> &arg_names,
		  const _report_fatal_at_raw_tok_type &report_fatal)
{
  auto &&is_ws_pred =
    [](const raw_pp_token &tok) { return tok.is_ws() || tok.is_empty(); };

  auto &&not_is_ws_pred =
    [&](const raw_pp_token &tok) { return !is_ws_pred(tok); };

  begin = std::find_if(begin, end, not_is_ws_pred);
  end = (std::find_if(std::make_reverse_iterator(end),
		     std::make_reverse_iterator(begin),
		     not_is_ws_pred)
	 .base());
  if (begin == end)
    return;

  assert(!begin->is_ws() && !begin->is_empty() &&
	 !(end-1)->is_ws() && !(end-1)->is_empty());

  if (begin->is_punctuator("##")) {
    throw report_fatal("## at beginning of macro replacement list", begin);
  } else if ((end - 1)->is_punctuator("##")) {
    throw report_fatal("## at end of macro replacement list", end - 1);
  }

  if (!func_like)
    return;

  auto &&is_stringify_op_pred =
    [](const raw_pp_token &tok) { return tok.is_punctuator("#"); };

  auto it = begin;
  while (it != end) {
    it = std::find_if(it, end, is_stringify_op_pred);
    if (it == end)
      break;

    it = std::find_if_not(it + 1, end, is_ws_pred);
    if (it == end || !it->is_id() ||
	(std::find(arg_names.cbegin(), arg_names.cend(), it->get_value())
	 == arg_names.cend())) {
      throw report_fatal("# in func-like macro not followed by parameter",
			 it != end ? it : end - 1);
    }
    ++it;
  }
}


preprocessor::_macro_expansion_stack_entry::_macro_expansion_stack_entry
		(const _macro_expansion_forest::_forest_nodes::size_type _node,
		 const pp_result::macro &_m,
		 std::vector<std::size_t> &&_args_offsets,
		 std::vector<_pp_tokens_type> &&_args)
  : m(&_m), node(_node),
    args_offsets(std::move(_args_offsets)), args(std::move(_args)), next_arg(0),
    out_tokens(nullptr), n_out_tokens(0), next_out_token(0)
{
  exp_args.resize(args.size());
}

preprocessor::_macro_expansion_stack_entry::_macro_expansion_stack_entry
		(const _macro_expansion_forest::_forest_nodes::size_type _node,
		 const pp_result::macro &_m)
  : m(&_m), node(_node), next_arg(0),
    out_tokens(nullptr), n_out_tokens(0), next_out_token(0)
{}

preprocessor::_macro_expansion_stack_entry::_macro_expansion_stack_entry
		(const _macro_expansion_forest::_forest_nodes::size_type _node,
		 const _pp_tokens_type &arg_tokens)
  : m(nullptr), node(_node), next_arg(0),
    out_tokens(arg_tokens.data()), n_out_tokens(arg_tokens.size()),
    next_out_token(0)
{}


const pp_result::header_inclusion_node&
preprocessor::get_pending_token_source(const raw_pp_token_index tok)
{
  _fixup_inclusion_node_ranges();
  if (tok == _pp_result->get_raw_tokens().size()) {
    // Virtual EOF token.
    assert(!_pp_result->get_header_inclusion_roots().empty());
    return *(_pp_result->get_header_inclusion_roots().end() - 1);
  }
  return _pp_result->get_raw_token_source(tok);
}

preprocessor::_cond_incl_state::
_cond_incl_state(pp_result::conditional_inclusion_node &_incl_node)
  : incl_node(_incl_node), branch_active(false), taken_branch(0),
    has_else_branch(false)
{}
