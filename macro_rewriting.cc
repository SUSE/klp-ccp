/*
 * Copyright (C) 2024-2026  SUSE Software Solutions Germany GmbH
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

#include <utility>
#include <type_traits>
#include <functional>
#include <cassert>
#include <algorithm>
#include <list>
#include <limits>
#include <iterator>
#include <memory>
#include <numeric>
#include <map>
#include <set>
#include <cctype>

#include "macro_rewriting.hh"
#include "pp_result_impl.hh"
#include "source_reader.hh"
#include "raw_pp_token.hh"
#include "source_writer.hh"
#include "source_reader_cache.hh"

namespace klp
{
namespace ccp
{

bool tokens_need_separation(const pp_token::type prev_type,
				      const std::string &prev_value,
				      const pp_token::type next_type,
				      const std::string &next_value) noexcept
{
  const auto is_ws_like =
    [](const pp_token::type t) {
      return (t == pp_token::type::ws || t == pp_token::type::newline ||
	      t == pp_token::type::empty || t == pp_token::type::eof);
    };
  if (is_ws_like(prev_type) || is_ws_like(next_type))
    return false;

  const auto is_str_like =
    [](const pp_token::type t) {
      switch (t) {
      case pp_token::type::chr:
      case pp_token::type::wchr:
      case pp_token::type::uchr16:
      case pp_token::type::uchr32:
      case pp_token::type::str:
      case pp_token::type::wstr:
      case pp_token::type::ustr8:
      case pp_token::type::ustr16:
      case pp_token::type::ustr32:
	return true;
      default:
	return false;
      }
    };

  switch (prev_type) {
  case pp_token::type::pp_number:
    return (next_type == pp_token::type::pp_number ||
	    next_type == pp_token::type::id ||
	    (next_type == pp_token::type::punctuator &&
	     (next_value == "-" || next_value == "+" || next_value == "--" ||
	      next_value == "++" || next_value == "-=" || next_value == "+=" ||
	      next_value == ".")));

  case pp_token::type::id:
    return (next_type == pp_token::type::id ||
	    next_type == pp_token::type::pp_number ||
	    is_str_like(next_type));

  case pp_token::type::punctuator:
    if (next_type == pp_token::type::pp_number)
      return prev_value == ".";
    if (next_type != pp_token::type::punctuator)
      return false;
    {
      static const char * const puncts[] = {
	"++", "--", "->", "<<", ">>", "<=", ">=", "==", "!=", "&&", "||",
	"+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=",
	"##", "...", "::", "%:", "%:%:", "<:", ":>", "<%", "%>", "->*", ".*",
      };
      const std::string glued = prev_value + next_value;
      for (const char * const p : puncts) {
	const std::string ps{p};
	if (ps.size() >= glued.size() && ps.compare(0, glued.size(), glued) == 0)
	  return true;
      }
    }
    return false;

  default:
    if (is_str_like(prev_type))
      return false;
    return false;
  }
}


rewritten_macro_token::rewritten_macro_token(const pp_token &_tok)
  : tok{_tok}, rename_ref{invalid_rewritten_macro_definition_id},
    is_pp_operator{false}
{}

rewritten_macro_token::
rewritten_macro_token(const pp_token &_tok,
		      const rewritten_macro_definition_id _rename_ref)
  : tok{_tok}, rename_ref{_rename_ref}, is_pp_operator{false}
{}

rewritten_macro_token::
rewritten_macro_token(const pp_token &_tok, const pp_operator_tag&)
  : tok{_tok}, rename_ref{invalid_rewritten_macro_definition_id},
    is_pp_operator{true}
{
  assert(tok.is_punctuator("##"));
}

bool rewritten_macro_token::operator==(const rewritten_macro_token &rhs)
  const noexcept
{
  return (this->rename_ref == rhs.rename_ref &&
	  this->is_pp_operator == rhs.is_pp_operator &&
	  this->tok.get_type() == rhs.tok.get_type() &&
	  this->tok.get_value() == rhs.tok.get_value());
}


bool rewritten_macro_definition::_op::operator==(const _op &rhs) const noexcept
{
  if (this->a != rhs.a)
    return false;

  switch (this->a) {
  case action::copy:
    return this->r == rhs.r;

  case action::replace:
    return this->r == rhs.r && this->new_toks == rhs.new_toks;

  case action::insert:
    return this->new_toks == rhs.new_toks;

  case action::insert_ws:
    return true;
  }
  return false;
}

rewritten_macro_definition::builder::builder(const pp_result::macro &orig_m)
  noexcept
  : _orig_m{orig_m}
{}

void rewritten_macro_definition::builder::erase(const raw_pp_tokens_range &r)
{
  assert(r.begin < r.end);
  _ops.push_back(_op{_op::action::replace, r, sticky_side::none, {}});
}

void rewritten_macro_definition::builder::
replace(const raw_pp_tokens_range &r,
	std::vector<rewritten_macro_token> &&new_toks)
{
  assert(r.begin < r.end);
  _ops.push_back(_op{_op::action::replace, r, sticky_side::none,
		     std::move(new_toks)});
}

void rewritten_macro_definition::builder::
insert(const raw_pp_token_index pos, const sticky_side stickiness,
       std::vector<rewritten_macro_token> &&new_toks)
{
  if (new_toks.empty())
    return;
  _ops.push_back(_op{_op::action::insert, raw_pp_tokens_range{pos, pos},
		     stickiness, std::move(new_toks)});
}

void rewritten_macro_definition::builder::
insert_ws(const raw_pp_token_index pos, const sticky_side stickiness)
{
  _ops.push_back(_op{_op::action::insert_ws, raw_pp_tokens_range{pos, pos},
		     stickiness, {}});
}

rewritten_macro_definition
rewritten_macro_definition::builder::finalize(const pp_result &pp_result) &&
{
  const raw_pp_tokens &repl_toks = _orig_m.get_repl();
  std::vector<std::string> arg_names = _orig_m.get_arg_names();

  // Sort the edits by position. Insertions preceed any replacements
  // at the same position.
  std::stable_sort(_ops.begin(), _ops.end(),
		   [](const _op &a, const _op &b) {
		     if (a.r.begin != b.r.begin)
		       return a.r.begin < b.r.begin;
		     return (a.a != _op::action::replace &&
			     b.a == _op::action::replace);
		   });

  // Identifiers introduced into the replacement list must not clash
  // with the macro's parameter names, rename the parameters if needed.
  std::map<std::string, std::string> param_renames;
  if (_orig_m.is_func_like() && !arg_names.empty()) {
    std::set<std::string> introduced_ids;
    for (const auto &o : _ops) {
      for (const auto &t : o.new_toks) {
	if (t.rename_ref == invalid_rewritten_macro_definition_id &&
	    t.tok.is_id()) {
	  introduced_ids.insert(t.tok.get_value());
	}
      }
    }
    if (!introduced_ids.empty()) {
      std::set<std::string> taken_ids = introduced_ids;
      for (const auto &n : arg_names)
	taken_ids.insert(n);
      for (const auto &t : repl_toks) {
	if (t.is_id())
	  taken_ids.insert(t.get_value());
      }
      for (auto &n : arg_names) {
	if (!introduced_ids.count(n))
	  continue;
	std::string new_name = n;
	do {
	  new_name += '_';
	} while (taken_ids.count(new_name));
	taken_ids.insert(new_name);
	param_renames.emplace(n, new_name);
	n = new_name;
      }
    }
  }

  // Fill the gaps inbetween the sparse edits with copy ops.
  _ops_container_type ops;
  ops.reserve(2 * _ops.size() + 1);
  const auto &push_copy =
    [&](const raw_pp_tokens_range &r) {
      if (r.begin == r.end)
	return;
      if (param_renames.empty()) {
	ops.push_back(_op{_op::action::copy, r, sticky_side::none, {}});
	return;
      }
      raw_pp_token_index pos = r.begin;
      for (raw_pp_token_index i = r.begin; i != r.end; ++i) {
	if (!repl_toks[i].is_id())
	  continue;
	const auto it_rename = param_renames.find(repl_toks[i].get_value());
	if (it_rename == param_renames.end())
	  continue;
	if (pos != i) {
	  ops.push_back(_op{_op::action::copy, raw_pp_tokens_range{pos, i},
			    sticky_side::none, {}});
	}
	ops.push_back(_op{
			_op::action::replace, raw_pp_tokens_range{i, i + 1},
			sticky_side::none,
			{
			  rewritten_macro_token{
			    pp_token{pp_token::type::id, it_rename->second}
			  }
			}
		      });
	pos = i + 1;
      }
      if (pos != r.end) {
	ops.push_back(_op{_op::action::copy, raw_pp_tokens_range{pos, r.end},
			  sticky_side::none, {}});
      }
    };

  raw_pp_token_index pos = 0;
  for (auto &o : _ops) {
    assert(o.r.begin >= pos);
    assert(o.r.end <= repl_toks.size());
    push_copy(raw_pp_tokens_range{pos, o.r.begin});
    pos = o.r.begin;
    switch (o.a) {
    case _op::action::replace:
      pos = o.r.end;
      if (o.new_toks.empty()) {
	// Erasure, represented as a gap.
	break;
      }
      ops.push_back(std::move(o));
      break;

    case _op::action::insert:
      /* fall through */
    case _op::action::insert_ws:
      assert(o.r.begin == o.r.end);
      ops.push_back(std::move(o));
      break;

    case _op::action::copy:
      assert(0);
      __builtin_unreachable();
    }
  }
  push_copy(raw_pp_tokens_range{pos, repl_toks.size()});

  return rewritten_macro_definition{_orig_m, std::move(ops),
				    std::move(arg_names), pp_result};
}

rewritten_macro_definition::
rewritten_macro_definition(const pp_result::macro &orig_m,
			   _ops_container_type &&ops,
			   std::vector<std::string> &&arg_names,
			   const pp_result &pp_result,
			   const bool forwarding)
  : _ops{std::move(ops)}, _orig_m{orig_m}, _arg_names{std::move(arg_names)},
    _forwarding{forwarding}
{
  // Forwarding definitions are synthesized from scratch, there's no
  // whitespace from the original replacement list to normalize.
  if (!_forwarding)
    normalize_ws(pp_result);
}

rewritten_macro_definition
rewritten_macro_definition::make_forwarding(const pp_result::macro &orig_m,
					    const pp_result &pp_result)
{
  // The forwarding definition's replacement list is an invocation of
  // the original macro with all parameters passed through, i.e.
  //   #define <new name>(a, b) <orig name>(a, b)
  // for function-like and
  //   #define <new name> <orig name>
  // for object-like macros. A variadic parameter gets forwarded
  // through its name (__VA_ARGS__ for an unnamed one), pasted to the
  // preceding comma with the GNU ", ## __VA_ARGS__" extension if
  // there is any: an invocation leaving the variadic arguments out
  // must get forwarded with them left out as well, because the
  // original definition might rely on that very extension and an
  // explicitly empty argument wouldn't swallow the comma.
  std::vector<std::string> arg_names = orig_m.get_arg_names();

  // A parameter named like the macro itself would get substituted
  // for the forwarding invocation's name token, rename it.
  const auto it_self = std::find(arg_names.begin(), arg_names.end(),
				 orig_m.get_name());
  if (it_self != arg_names.end()) {
    std::string new_name = *it_self;
    do {
      new_name += '_';
    } while (std::find(arg_names.begin(), arg_names.end(), new_name) !=
	     arg_names.end());
    *it_self = std::move(new_name);
  }

  std::vector<rewritten_macro_token> toks;
  toks.emplace_back(pp_token{pp_token::type::id, orig_m.get_name()});
  if (orig_m.is_func_like()) {
    toks.emplace_back(pp_token{pp_token::type::punctuator, "("});
    for (auto it_arg = arg_names.begin(); it_arg != arg_names.end();
	 ++it_arg) {
      if (it_arg != arg_names.begin()) {
	toks.emplace_back(pp_token{pp_token::type::punctuator, ","});
	toks.emplace_back(pp_token{pp_token::type::ws, " "});
	if (orig_m.is_variadic() && it_arg + 1 == arg_names.end()) {
	  toks.emplace_back(pp_token{pp_token::type::punctuator, "##"},
			    rewritten_macro_token::pp_operator_tag{});
	}
      }
      toks.emplace_back(pp_token{pp_token::type::id, *it_arg});
    }
    toks.emplace_back(pp_token{pp_token::type::punctuator, ")"});
  }

  // Replace the whole original replacement list, if any.
  const raw_pp_tokens_range repl_range{0, orig_m.get_repl().size()};
  _ops_container_type ops;
  if (repl_range.begin != repl_range.end) {
    ops.push_back(_op{_op::action::replace, repl_range, sticky_side::none,
		      std::move(toks)});
  } else {
    ops.push_back(_op{_op::action::insert, repl_range, sticky_side::none,
		      std::move(toks)});
  }

  return rewritten_macro_definition{orig_m, std::move(ops),
				    std::move(arg_names), pp_result, true};
}

bool rewritten_macro_definition::
operator==(const rewritten_macro_definition &rhs) const noexcept
{
  return (&this->_orig_m == &rhs._orig_m &&
	  this->_forwarding == rhs._forwarding &&
	  this->_arg_names == rhs._arg_names &&
	  this->_ops == rhs._ops);
}

bool rewritten_macro_definition::is_identity() const noexcept
{
  if (_arg_names != _orig_m.get_arg_names())
    return false;

  raw_pp_token_index pos = 0;
  for (const auto &o : _ops) {
    switch (o.a) {
    case _op::action::insert_ws:
      continue;

    case _op::action::copy:
      if (o.r.begin != pos)
	return false;
      pos = o.r.end;
      break;

    case _op::action::replace:
      /* fall through */
    case _op::action::insert:
      return false;
    }
  }
  return pos == _orig_m.get_repl().size();
}

void rewritten_macro_definition::
write_repl(source_writer &writer,
	   const pp_result &pp_result,
	   source_reader_cache &source_reader_cache,
	   const std::function<const std::string&(rewritten_macro_definition_id)>
	     &resolve_name) const
{
  const raw_pp_tokens &repl_toks = _orig_m.get_repl();

  source_reader *sr = nullptr;
  if (!_orig_m.is_predefined()) {
    const raw_pp_tokens_range &directive_range = _orig_m.get_directive_range();
    auto it_source = pp_result.intersecting_sources_begin(directive_range);
    assert(it_source != pp_result.intersecting_sources_end(directive_range));
    sr = &source_reader_cache.get(*it_source);
  }

  // Make sure there's some whitespace separating the macro signature
  // from the replacement text.
  bool last_was_ws = false;
  if (_ops.empty()) {
    return;
  } else if (_ops.front().a == _op::action::copy) {
    const raw_pp_token &first_tok = repl_toks[_ops.front().r.begin];
    last_was_ws = first_tok.is_ws() || first_tok.is_empty();
  } else if (_ops.front().a == _op::action::insert_ws) {
    last_was_ws = true;
  }
  if (!last_was_ws) {
    writer.append(" ");
    last_was_ws = true;
  }

  pp_token::type last_type = pp_token::type::ws;
  std::string last_value;
  const auto separate_if_needed =
    [&](const pp_token::type next_type, const std::string &next_value) {
      if (!last_was_ws &&
	  tokens_need_separation(last_type, last_value, next_type, next_value)) {
	writer.append(" ");
      }
    };

  for (const auto &o : _ops) {
    switch (o.a) {
    case _op::action::copy:
      {
	if (o.r.begin == o.r.end)
	  break;
	const raw_pp_token &first_tok = repl_toks[o.r.begin];
	separate_if_needed(first_tok.get_type(), first_tok.get_value());
	if (sr) {
	  writer.append(*sr,
			range_in_file{
			  repl_toks[o.r.begin].get_range_in_file().begin,
			  repl_toks[o.r.end - 1].get_range_in_file().end
			});
	} else {
	  for (raw_pp_token_index i = o.r.begin; i != o.r.end; ++i)
	    writer.append(repl_toks[i].get_value());
	}
	const raw_pp_token &last_tok = repl_toks[o.r.end - 1];
	last_was_ws = (last_tok.is_ws() || last_tok.is_empty() ||
		       last_tok.is_newline());
	last_type = last_tok.get_type();
	last_value = last_tok.get_value();
      }
      break;

    case _op::action::replace:
      /* fall through */
    case _op::action::insert:
      for (const auto &t : o.new_toks) {
	if (t.tok.is_empty()) {
	  // Continuation line marker.
	  if (!last_was_ws)
	    writer.append(" ");
	  writer.append("\\\n");
	  last_was_ws = true;
	  continue;
	} else if (t.tok.is_ws() || t.tok.is_newline()) {
	  if (!last_was_ws)
	    writer.append(" ");
	  last_was_ws = true;
	  continue;
	}

	std::string text;
	if (t.rename_ref != invalid_rewritten_macro_definition_id) {
	  text = resolve_name(t.rename_ref);
	  separate_if_needed(pp_token::type::id, text);
	  last_type = pp_token::type::id;
	  last_value = text;
	} else {
	  text = t.tok.stringify();
	  separate_if_needed(t.tok.get_type(), t.tok.get_value());
	  last_type = t.tok.get_type();
	  last_value = t.tok.get_value();
	}
	writer.append(text);
	last_was_ws = false;
      }
      break;

    case _op::action::insert_ws:
      if (!last_was_ws)
	writer.append(" ");
      last_was_ws = true;
      break;
    }
  }
}


rewritten_macro_definition_id
rewritten_macro_definitions_pool::insert(rewritten_macro_definition &&d)
{
  assert(!_names_assigned);
  auto &ids = _by_macro[&d.get_orig_macro()];
  for (const auto id : ids) {
    if (*_defs[id] == d)
      return id;
  }

  const rewritten_macro_definition_id id = _defs.size();
  _defs.push_back(std::make_unique<rewritten_macro_definition>(std::move(d)));
  ids.push_back(id);
  return id;
}

void rewritten_macro_definitions_pool::truncate(const std::size_t size)
{
  assert(!_names_assigned);
  assert(size <= _defs.size());
  for (auto it = _by_macro.begin(); it != _by_macro.end();) {
    auto &ids = it->second;
    ids.erase(std::remove_if(ids.begin(), ids.end(),
			     [size](const rewritten_macro_definition_id id) {
			       return id >= size;
			     }),
	      ids.end());
    if (ids.empty())
      it = _by_macro.erase(it);
    else
      ++it;
  }
  _defs.resize(size);
}

const rewritten_macro_definition&
rewritten_macro_definitions_pool::
operator[](const rewritten_macro_definition_id id) const noexcept
{
  assert(id < _defs.size());
  return *_defs[id];
}

void rewritten_macro_definitions_pool::
assign_names(const std::vector<rewritten_macro_definition_id> &first_use_order,
	     const std::function<std::string(const pp_result::macro&,
					      std::size_t, bool)> &name_cb)
{
  assert(!_names_assigned);
  _names.resize(_defs.size());

  std::vector<rewritten_macro_definition_id> ordered;
  ordered.reserve(_defs.size());
  std::vector<bool> seen(_defs.size(), false);
  for (const auto id : first_use_order) {
    if (id < _defs.size() && !seen[id]) {
      seen[id] = true;
      ordered.push_back(id);
    }
  }
  for (rewritten_macro_definition_id id = 0; id < _defs.size(); ++id) {
    if (!seen[id])
      ordered.push_back(id);
  }

  // Group by the original macros' names rather than by the original
  // definitions: a name can have several definitions within a
  // translation unit (#undef + #define), and numbering their
  // rewritten definitions independently would make the policy's
  // names collide.
  std::map<std::string, std::vector<rewritten_macro_definition_id>> by_name;
  for (const auto id : ordered)
    by_name[_defs[id]->get_orig_macro().get_name()].push_back(id);
  for (const auto &e : by_name) {
    const bool multiple = e.second.size() > 1;
    std::size_t index = 0;
    for (const auto id : e.second) {
      _names[id] = name_cb(_defs[id]->get_orig_macro(), index, multiple);
      ++index;
    }
  }
  _names_assigned = true;
}

const std::string& rewritten_macro_definitions_pool::
get_name(const rewritten_macro_definition_id id) const noexcept
{
  assert(_names_assigned);
  assert(id < _names.size());
  return _names[id];
}

void rewritten_macro_definition::normalize_ws(const pp_result &pp_result)
{
  if (_ops.empty())
    return;

  const auto &orig_repl_toks = _orig_m.get_repl();

  const auto &is_ws = [](const raw_pp_token &raw_tok) {
    return raw_tok.is_ws();
  };
  const auto is_cont_line_marker = [](const raw_pp_token &raw_tok) {
    return raw_tok.is_empty();
  };
  const auto &not_is_ws = [&](const raw_pp_token &raw_tok) {
    return !is_ws(raw_tok);
  };
  const auto &not_is_cont_line_marker = [&](const raw_pp_token &raw_tok) {
    return !is_cont_line_marker(raw_tok);
  };

  const auto &is_ws_or_cont_line_marker = [&](const raw_pp_token &raw_tok) {
    return is_ws(raw_tok) || is_cont_line_marker(raw_tok);
  };

  const auto &not_is_ws_or_cont_line_marker = [&](const raw_pp_token &raw_tok) {
    return !is_ws_or_cont_line_marker(raw_tok);
  };

  const auto &is_copy_or_replace_op = [](const _op &o) {
    return o.a == _op::action::copy || o.a == _op::action::replace;
  };

  const auto &is_cont_marker_insert_op = [](const _op &o) {
    return (o.a == _op::action::insert && o.new_toks.size() == 1 &&
	    o.new_toks.front().tok.is_empty());
  };

  const auto &op_head_has_nontrivial_ws =
    [&](const _op &o) {
      if (o.a == _op::action::insert_ws) {
	return true;
      } else if (is_cont_marker_insert_op(o)) {
	// A newly added line continuation marker, which will always
	// get emitted alongside a single space.
	return true;
      } else if (o.a != _op::action::copy) {
	return false;
      }

      const auto copy_toks_fw_end = orig_repl_toks.cbegin() + o.r.end;
      const auto it_first_non_cont_line_marker_tok
	= std::find_if(orig_repl_toks.cbegin() + o.r.begin,
		       copy_toks_fw_end, not_is_cont_line_marker);
      return (it_first_non_cont_line_marker_tok != copy_toks_fw_end &&
	      it_first_non_cont_line_marker_tok->is_ws());
    };

  const auto &op_tail_has_nontrivial_ws =
    [&](const _op &o) {
      if (o.a == _op::action::insert_ws) {
	return true;
      } else if (is_cont_marker_insert_op(o)) {
	// A newly added line continuation marker, which will always
	// get emitted alongside a single space.
	return true;
      } else if (o.a != _op::action::copy) {
	return false;
      }

      const auto copy_toks_bw_end =
	std::make_reverse_iterator(orig_repl_toks.cbegin() + o.r.begin);
      const auto it_last_non_cont_line_marker_tok
	= std::find_if(std::make_reverse_iterator(orig_repl_toks.cbegin() +
						  o.r.end),
		       copy_toks_bw_end, not_is_cont_line_marker);
      return (it_last_non_cont_line_marker_tok != copy_toks_bw_end &&
	      it_last_non_cont_line_marker_tok->is_ws());
    };

  // Check if a macro replacement text contains any continuation lines,
  // but ignore any such sequence at its head.
  const auto trimmed_macro_repl_begin =
    std::find_if(orig_repl_toks.cbegin(),
		 orig_repl_toks.cend(),
		 not_is_ws_or_cont_line_marker);
  const bool macro_repl_contains_any_cont_line =
    std::any_of(trimmed_macro_repl_begin, orig_repl_toks.cend(),
		is_cont_line_marker);
  const bool macro_contains_any_cont_line =
    macro_repl_contains_any_cont_line |
    std::any_of(orig_repl_toks.cbegin(), trimmed_macro_repl_begin,
		is_cont_line_marker);

  // First (preprocessing) phase: prune continuation line markers from
  // copy ops.
  //
  // When searching for maximal runs of copy/replace ops contained in
  // a common input continuation line in the next phase, it is crucial
  // for the search algorithm that these don't contain any
  // continuation line markers at their all-whitespace heads or tails
  // (somewhere in the middle is fine, as that would contiguously join
  // two adjacent continuation lines and yield the desired formatting
  // result).  So strip sequences of (whitespace*, continuation line
  // marker) at the copy ops' heads and sequences of (continuation
  // line marker, whitespace*) at their tails.
  //
  // As a side-effect, this also ensures that there will be no
  // continuation line formatting whitespace from more than one
  // continuation line left at the copy ops' heads and tails,
  // which also simplifies things.
  for (auto it_op = _ops.begin(); it_op != _ops.end();) {
    if (it_op->a == _op::action::replace) {
      // One cannot have newlines in a macro replacement list.
      for (auto &t : it_op->new_toks) {
	if (t.tok.is_newline())
	  t.tok = pp_token{pp_token::type::ws, " "};
      }
      if (std::all_of(it_op->new_toks.begin(), it_op->new_toks.end(),
		      [](const rewritten_macro_token &t) {
			return t.tok.is_ws();
		      })) {
	it_op = _ops.erase(it_op);

	if (it_op != _ops.begin() && it_op != _ops.end() &&
	    !op_tail_has_nontrivial_ws(*(it_op - 1)) &&
	    !op_head_has_nontrivial_ws(*it_op)) {
	  // The removed newline might have been required for proper
	  // token separation in the output. Insert an insert_ws then
	  // instead.
	  it_op = _ops.insert(it_op,
			      _op{
				_op::action::insert_ws,
				  raw_pp_tokens_range{},
				  sticky_side::none,
				  {}
			      }) + 1;
	}
	continue;
      }

      // Strip leading and trailing whitespace or continuation line
      // markers in the range to replace (there should be none
      // though, but for the sake of definiteness...).
      const auto repl_toks_bw_end =
	std::make_reverse_iterator(orig_repl_toks.cbegin() + it_op->r.begin);
      auto it_last_non_ws_tok =
	std::find_if(std::make_reverse_iterator(orig_repl_toks.cbegin() +
						it_op->r.end),
		     repl_toks_bw_end, not_is_ws_or_cont_line_marker);
      it_op->r.end = it_last_non_ws_tok.base() - orig_repl_toks.cbegin();
      // Pruning at the head is only needed if there is anything left,
      // which, in the common case, it is not.
      if (it_op->r.length() > 1) {
	const auto repl_toks_fw_end = orig_repl_toks.cbegin() + it_op->r.end;
	auto it_first_non_ws_tok =
	  std::find_if(orig_repl_toks.cbegin() + it_op->r.begin,
		       repl_toks_fw_end, not_is_ws_or_cont_line_marker);
	it_op->r.begin = it_first_non_ws_tok - orig_repl_toks.cbegin();
      }

      ++it_op;
      continue;
    }

    // The rest in this loop is concerned only with copy ops: strip
    // continuation line markers from their heads and tails and drop
    // all-whitespace copy ops altogether. The latter is also needed
    // for replacement lists without any continuation lines, as the
    // later phases assume that all-whitespace copy ops don't exist,
    // c.f. the junctions inserted inbetween two replace ops
    // separated by whitespace in the original replacement list only.
    if (it_op->a != _op::action::copy) {
      ++it_op;
      continue;
    }

    // Remove leading (whitespace*, continuation line marker)
    // pairs. Note that this retains whitespace following the last
    // such continuation line marker.
    bool removed_some_leading_ws = false;
    const auto copy_toks_fw_end = orig_repl_toks.cbegin() + it_op->r.end;
    auto it_next_non_ws_tok =
      std::find_if(orig_repl_toks.cbegin() + it_op->r.begin,
		   copy_toks_fw_end, not_is_ws);
    while (it_next_non_ws_tok != copy_toks_fw_end &&
	   is_cont_line_marker(*it_next_non_ws_tok)){
      removed_some_leading_ws |=
	it_next_non_ws_tok != orig_repl_toks.cbegin() + it_op->r.begin;
      ++it_next_non_ws_tok;
      it_op->r.begin = it_next_non_ws_tok - orig_repl_toks.cbegin();
      it_next_non_ws_tok =
	std::find_if(it_next_non_ws_tok, copy_toks_fw_end, not_is_ws);
    }

    // If the copy op comprises only whitespace and continuation line
    // makers, then it will get dropped below.
    const bool is_all_ws = it_next_non_ws_tok == copy_toks_fw_end;
    removed_some_leading_ws |= is_all_ws && it_op->r.length() != 0;

    if (removed_some_leading_ws &&
	(is_all_ws ||
	 it_next_non_ws_tok == orig_repl_toks.cbegin() + it_op->r.begin) &&
	it_op != _ops.begin() && !op_tail_has_nontrivial_ws(*(it_op - 1))) {
      // All of the leading whitespace got removed. As some whitespace might
      // be needed for proper separation in the output, add an insert_ws op.
      // It might, or might not, get removed again in later phases.
      it_op = _ops.insert(it_op,
			  _op{
			    _op::action::insert_ws,
			      raw_pp_tokens_range{},
			      (!is_all_ws ?
			       sticky_side::right :
			       sticky_side::none),
			      {}
			  }) + 1;
    }

    if (is_all_ws) {
      it_op = _ops.erase(it_op);
      continue;
    }

    // Similarly, strip sequences of (continuation line marker, whitespace*)
    // from the tail.
    bool removed_some_trailing_ws = false;
    const auto copy_toks_bw_end =
      std::make_reverse_iterator(orig_repl_toks.cbegin() + it_op->r.begin);
    auto it_prev_non_ws_tok =
      std::find_if(std::make_reverse_iterator(orig_repl_toks.cbegin() +
					      it_op->r.end),
		   copy_toks_bw_end, not_is_ws);
    while (it_prev_non_ws_tok != copy_toks_bw_end &&
	   is_cont_line_marker(*it_prev_non_ws_tok)){
      removed_some_trailing_ws |=
	it_prev_non_ws_tok.base() != orig_repl_toks.cbegin() + it_op->r.end;
      ++it_prev_non_ws_tok;
      it_op->r.end = it_prev_non_ws_tok.base() - orig_repl_toks.cbegin();
      it_prev_non_ws_tok =
	std::find_if(it_prev_non_ws_tok, copy_toks_bw_end, not_is_ws);
    }
    // It's already known that the copy range does not only
    // comprise whitespace and continuation line markers.
    assert(it_prev_non_ws_tok != copy_toks_bw_end);
    if (removed_some_trailing_ws &&
	(it_prev_non_ws_tok.base() ==
	 orig_repl_toks.cbegin() + it_op->r.end) &&
	(it_op + 1) != _ops.end() && !op_head_has_nontrivial_ws(*(it_op + 1))) {
      // All of the trailing whitespace got removed. As some whitespace might
      // be needed for proper separation in the output, add an insert_ws op.
      // It might, or might not, get removed again in later phases.
      it_op = _ops.insert(it_op + 1,
			  _op{
			    _op::action::insert_ws,
			      raw_pp_tokens_range{},
			      sticky_side::left,
			      {}
			  }) - 1;
    }

    ++it_op;
  }

  // Second phase: retain continuation line formatting from
  // the original input:
  // - partition the ops into subsequences stemming from
  //   the same common continuation line each,
  // - insert leading continuation line formatting whitespace
  //   to such a subsequence's front.
  // - insert trailing line continuation line formatting
  //   whitespace + the continuation line marker to its back and
  // - trim any pre-existing continuation line formatting whitespace
  //   from the subsequence's inner ops heads and tails (the ops might
  //   be out of order).
  const auto &is_all_ws_or_cont_line_markers_copy_op =
    [&](const _op &o) {
      return (o.a == _op::action::copy &&
	      std::all_of(orig_repl_toks.cbegin() + o.r.begin,
			  orig_repl_toks.cbegin() + o.r.end,
			  is_ws_or_cont_line_marker));
    };
  const auto &not_is_all_ws_or_cont_line_markers_copy_op =
    [&](const _op &o) {
      return !is_all_ws_or_cont_line_markers_copy_op(o);
    };

  for (auto it_common_cont_line_ops_begin = _ops.begin();
       it_common_cont_line_ops_begin != _ops.end();) {
    // Copy ops comprising only whitespace and continuation line
    // markers don't exist after the first phase, except for the
    // continuation line formatting whitespace ops inserted by this
    // very phase for the preceding groups. Those are not content and
    // must not start a group of their own: the trailing one inserted
    // below is located right at the previous group's end iterator.
    if ((it_common_cont_line_ops_begin->a != _op::action::copy &&
	 it_common_cont_line_ops_begin->a != _op::action::replace) ||
	is_all_ws_or_cont_line_markers_copy_op
	(*it_common_cont_line_ops_begin)) {
      ++it_common_cont_line_ops_begin;
      continue;
    }

    // Search for a maximal sequence of (copy/replace) ops from the
    // same continuation line starting at
    // it_common_cont_line_ops_begin. Note that if the original macro
    // replacement text does not contain any continuation line
    // markers, then this range will cover all the copy/replace ops.
    auto common_cont_line_range = it_common_cont_line_ops_begin->r;
    auto it_common_cont_line_ops_end = it_common_cont_line_ops_begin + 1;
    for (auto it_op = it_common_cont_line_ops_end; it_op != _ops.end();
	 ++it_op) {
      if (it_op->a != _op::action::copy && it_op->a != _op::action::replace)
	continue;

      if (it_op->r.end < common_cont_line_range.begin &&
	  std::any_of(orig_repl_toks.cbegin() + it_op->r.begin,
		      orig_repl_toks.cbegin() + common_cont_line_range.end,
		      is_cont_line_marker)) {
	break;
      }

      if (common_cont_line_range.end < it_op->r.begin  &&
	  std::any_of(orig_repl_toks.cbegin() + common_cont_line_range.end,
		      orig_repl_toks.cbegin() + it_op->r.begin,
		      is_cont_line_marker)) {
	break;
      }

      // The current token *is* still contained within the current common
      // continuation line. Extend the range.
      common_cont_line_range.begin = std::min(common_cont_line_range.begin,
					      it_op->r.begin);
      common_cont_line_range.end = std::max(common_cont_line_range.end,
					    it_op->r.end);
      it_common_cont_line_ops_end = it_op + 1;
    }

    // In principle, the range could still contain two back-to-back
    // copies of the same continuation line, which should not get
    // collated into one in the output. Check that the ops are all
    // disjunct, break the continuation line if not. The search is
    // quadratic, but common_cont_line_range is not expected to
    // contain many ops.
    if (macro_repl_contains_any_cont_line) {
      bool cont_line_split = false;
      for (auto it_op = it_common_cont_line_ops_begin + 1;
	   !cont_line_split && it_op != it_common_cont_line_ops_end; ++it_op) {
	if (it_op->a != _op::action::copy &&
	    it_op->a != _op::action::replace) {
	  continue;
	}
	for (auto it_prev_op = it_common_cont_line_ops_begin;
	     it_prev_op != it_op; ++it_prev_op) {
	  if (it_prev_op->r.overlaps_with(it_op->r)) {
	    // Recheck more carefully after trimming leading and trailing ws.
	    auto trimmed_prev_op_r = it_prev_op->r;
	    auto trimmed_op_r = it_op->r;
	    if (it_prev_op->a == _op::action::copy) {
	      trimmed_prev_op_r =
		raw_pp_tokens_range{
		  (static_cast<pp_token_index>
		   (std::find_if
		    (orig_repl_toks.cbegin() + trimmed_prev_op_r.begin,
		     orig_repl_toks.cend(),
		     not_is_ws) - orig_repl_toks.cbegin())),
		  (static_cast<pp_token_index>
		   (std::find_if
		    ((std::make_reverse_iterator
		      (orig_repl_toks.cbegin() + trimmed_prev_op_r.end)),
		     orig_repl_toks.crend(),
		     not_is_ws).base() - orig_repl_toks.cbegin()))
	      };
	      assert(trimmed_prev_op_r.begin < trimmed_prev_op_r.end);
	    }
	    if (it_op->a == _op::action::copy) {
	      trimmed_op_r =
		raw_pp_tokens_range{
		  (static_cast<pp_token_index>
		   (std::find_if
		    (orig_repl_toks.cbegin() + trimmed_op_r.begin,
		     orig_repl_toks.cend(),
		     not_is_ws) - orig_repl_toks.cbegin())),
		  (static_cast<pp_token_index>
		   (std::find_if
		    ((std::make_reverse_iterator
		      (orig_repl_toks.cbegin() + trimmed_op_r.end)),
		     orig_repl_toks.crend(),
		     not_is_ws).base() - orig_repl_toks.cbegin()))
	      };
	      assert(trimmed_op_r.begin < trimmed_op_r.end);
	    }
	    if (trimmed_prev_op_r.overlaps_with(trimmed_op_r)) {
	      it_common_cont_line_ops_end =
		(std::find_if
		 (std::make_reverse_iterator(it_op),
		  std::make_reverse_iterator(it_common_cont_line_ops_begin),
		  [](const _op &o) {
		    return (o.a == _op::action::copy ||
			    o.a == _op::action::replace);
		  }))
		.base();
	      assert(it_common_cont_line_ops_begin !=
		     it_common_cont_line_ops_end);
	      cont_line_split = true;
	      break;
	    }
	  }
	}
      }

      if (cont_line_split) {
	// If the line got split because of overlapping ops, then the
	// common continuation line's range must get recomputed.
	common_cont_line_range = it_common_cont_line_ops_begin->r;
	for (auto it_op = it_common_cont_line_ops_begin + 1;
	     it_op != it_common_cont_line_ops_end; ++it_op) {
	  common_cont_line_range.begin = std::min(common_cont_line_range.begin,
						  it_op->r.begin);
	  common_cont_line_range.end = std::max(common_cont_line_range.end,
						it_op->r.end);
	}
      }
    }

    assert(it_common_cont_line_ops_begin != it_common_cont_line_ops_end);
    assert(it_common_cont_line_ops_begin->a == _op::action::copy ||
	   it_common_cont_line_ops_begin->a == _op::action::replace);
    assert((it_common_cont_line_ops_end - 1)->a == _op::action::copy ||
	   (it_common_cont_line_ops_end - 1)->a == _op::action::replace);

    auto n_common_cont_line_ops =
      it_common_cont_line_ops_end - it_common_cont_line_ops_begin;

    // In order to have well-defined bounds for the continuation line
    // formatting whitespace, even in corner cases like e.g. all ops
    // being replace ops replacing empty ranges, trim leading and
    // trailing whitespace from the common_cont_line_range.
    common_cont_line_range.end =
      ((std::find_if(std::make_reverse_iterator(orig_repl_toks.cbegin()
						+ common_cont_line_range.end),
		     std::make_reverse_iterator(orig_repl_toks.cbegin()
						+ common_cont_line_range.begin),
		     not_is_ws)
	.base())
       - orig_repl_toks.cbegin());
    common_cont_line_range.begin =
      ((std::find_if(orig_repl_toks.cbegin() + common_cont_line_range.begin,
		     orig_repl_toks.cbegin() + common_cont_line_range.end,
		     not_is_ws))
       - orig_repl_toks.cbegin());

    // Retain continuation line formatting whitespace at the front.
    // If the current continuation line is at the same line as the
    // original #define or there are no continuation line markers at all,
    // then this will be the whitespace separating the macro signature
    // from the replacement text.
    const auto it_leading_cont_line_ws_toks_begin =
      (std::find_if((std::make_reverse_iterator
		    (orig_repl_toks.cbegin() +
		     common_cont_line_range.begin)),
		   orig_repl_toks.crend(),
		   is_cont_line_marker)
       .base());
    const auto it_leading_cont_line_ws_toks_end =
      std::find_if(it_leading_cont_line_ws_toks_begin,
		   orig_repl_toks.cbegin() + common_cont_line_range.begin,
		   not_is_ws);
    const raw_pp_tokens_range leading_cont_line_ws_range = {
      static_cast<raw_pp_token_index>(it_leading_cont_line_ws_toks_begin -
				      orig_repl_toks.cbegin()),
      static_cast<raw_pp_token_index>(it_leading_cont_line_ws_toks_end -
				      orig_repl_toks.cbegin()),
    };

    // Prune any pre-existing whitespace included at the head op
    // before adding the one from the continuation line formatting.
    bool removed_leading_ws = false;
    if (it_common_cont_line_ops_begin->a == _op::action::copy) {
      while (it_common_cont_line_ops_begin->r.begin <
	     it_common_cont_line_ops_begin->r.end &&
	     orig_repl_toks[it_common_cont_line_ops_begin->r.begin].is_ws()) {
	removed_leading_ws = true;
	++it_common_cont_line_ops_begin->r.begin;
      }
    }

    if (leading_cont_line_ws_range.length() != 0) {
      // Prepend the leading whitespace from the continuation line formatting.
      // Try to extend the first copy op, if any, to the left. Otherwise
      // insert a new one.
      if (it_common_cont_line_ops_begin->a == _op::action::copy &&
	  (leading_cont_line_ws_range.end ==
	   it_common_cont_line_ops_begin->r.begin)) {
	it_common_cont_line_ops_begin->r.begin =
	  leading_cont_line_ws_range.begin;
      } else {
	it_common_cont_line_ops_begin
	  = _ops.insert(it_common_cont_line_ops_begin,
			_op{
			  _op::action::copy,
			  leading_cont_line_ws_range,
			  sticky_side::none,
			  {}
			}) + 1;
	it_common_cont_line_ops_end =
	  it_common_cont_line_ops_begin + n_common_cont_line_ops;
      }
    } else if (removed_leading_ws &&
	       it_common_cont_line_ops_begin != _ops.begin() &&
	       !(op_tail_has_nontrivial_ws
		 (*(it_common_cont_line_ops_begin - 1)))) {
      // Some whitespace got removed at the head and now there isn't
      // any anymore. As it might still be needed for proper
      // separation in the output, insert a insert_ws op at that
      // point.
      it_common_cont_line_ops_begin
	= _ops.insert(it_common_cont_line_ops_begin,
		      _op{
			_op::action::insert_ws,
			  raw_pp_tokens_range{},
			  sticky_side::right,
			  {}
		      }) + 1;
      it_common_cont_line_ops_end =
	it_common_cont_line_ops_begin + n_common_cont_line_ops;
    }

    // Now take care of continuation line formatting whitespace at the
    // back.
    auto it_common_cont_line_last_op = it_common_cont_line_ops_end - 1;
    // Independent of whether or not there is a subsequent continuation
    // line marker in the input, trim the last ops' trailing whitespace.
    bool removed_trailing_ws = false;
    if (it_common_cont_line_last_op->a == _op::action::copy) {
      while (it_common_cont_line_last_op->r.end >
	     it_common_cont_line_last_op->r.begin &&
	     orig_repl_toks[it_common_cont_line_last_op->r.end - 1].is_ws()) {
	removed_trailing_ws = true;
	-- it_common_cont_line_last_op->r.end;
      }
    }
    const auto it_cont_line_marker_tok =
      std::find_if(orig_repl_toks.cbegin() + common_cont_line_range.end,
		   orig_repl_toks.cend(), is_cont_line_marker);
    if (it_cont_line_marker_tok != orig_repl_toks.cend()) {
      const auto it_trailing_cont_line_ws_toks_begin =
	(std::find_if(std::make_reverse_iterator(it_cont_line_marker_tok),
		      std::make_reverse_iterator(orig_repl_toks.cbegin() +
						 common_cont_line_range.end),
		      not_is_ws)
	 .base());
      const auto it_trailing_cont_line_ws_toks_end =
	it_cont_line_marker_tok + 1;
      const raw_pp_tokens_range trailing_cont_line_ws_range = {
	static_cast<raw_pp_token_index>(it_trailing_cont_line_ws_toks_begin -
					orig_repl_toks.cbegin()),
	static_cast<raw_pp_token_index>(it_trailing_cont_line_ws_toks_end -
					orig_repl_toks.cbegin()),
      };

      // Append the trailing whitespace from the continuation line formatting.
      // Try to extend the last copy op, if any, to the right. Otherwise
      // insert a new one.
      if (it_common_cont_line_last_op->a == _op::action::copy &&
	  (it_common_cont_line_last_op->r.end ==
	   trailing_cont_line_ws_range.begin)) {
	it_common_cont_line_last_op->r.end =
	  trailing_cont_line_ws_range.end;
      } else {
	it_common_cont_line_ops_end
	  = _ops.insert(it_common_cont_line_ops_end,
			_op{
			  _op::action::copy,
			  trailing_cont_line_ws_range,
			  sticky_side::none,
			  {}
			});
	it_common_cont_line_ops_begin =
	  it_common_cont_line_ops_end - n_common_cont_line_ops;
      }

      // If some non-trivial ws got reinserted, flip removed_trailing_ws
      // to false.
      removed_trailing_ws &= trailing_cont_line_ws_range.length() == 1;

    } else if (std::find_if(it_common_cont_line_ops_end, _ops.end(),
			    is_copy_or_replace_op) !=
	       _ops.end()) {
      // There is no subsequent continuation line marker in the input,
      // but this continuation line will not be the last in the output
      // (meaning it got moved ahead of others).
      // Insert one, separated by a insert_ws.
      it_common_cont_line_ops_end
	= _ops.insert(it_common_cont_line_ops_end,
		      _op{
			_op::action::insert,
			  raw_pp_tokens_range{},
			  sticky_side::none,
			  {
			    rewritten_macro_token{
			      pp_token{pp_token::type::empty, std::string{}}
			    }
			  }
		      });
      // Clear removed_trailing_ws, as an inserted continuation line marker
      // will always get emitted alongside a whitespace character.
      removed_trailing_ws = false;
      it_common_cont_line_ops_begin =
	it_common_cont_line_ops_end - n_common_cont_line_ops;
    }

    if (removed_trailing_ws &&
	it_common_cont_line_ops_end != _ops.end() &&
	!(op_head_has_nontrivial_ws(*it_common_cont_line_ops_end))) {
      // Some whitespace got removed at the tail and now there isn't
      // any anymore. As it might still be needed for proper
      // separation in the output, insert a insert_ws op at that
      // point.
      it_common_cont_line_ops_end
	= _ops.insert(it_common_cont_line_ops_end,
		      _op{
			_op::action::insert_ws,
			  raw_pp_tokens_range{},
			  sticky_side::left,
			  {}
		      });
      it_common_cont_line_ops_begin =
	it_common_cont_line_ops_end - n_common_cont_line_ops;
    }

    // Remove pre-existing leading continuation line formatting
    // whitespace from inner ops.
    for (std::size_t i_op = 1; i_op < n_common_cont_line_ops; ++i_op) {
      auto it_op = it_common_cont_line_ops_begin + i_op;
      if (it_op->a != _op::action::copy)
	continue;
      if (!orig_repl_toks[it_op->r.begin].is_ws())
	continue;

      const auto it_prev_non_ws_tok =
	std::find_if(std::make_reverse_iterator(orig_repl_toks.cbegin() +
						it_op->r.begin),
		     orig_repl_toks.crend(), not_is_ws);
      if (it_prev_non_ws_tok != orig_repl_toks.crend() &&
	  !is_cont_line_marker(*it_prev_non_ws_tok)) {
	continue;
      }

      while (orig_repl_toks[it_op->r.begin].is_ws())
	++it_op->r.begin;

      if (it_op != _ops.begin() && !op_tail_has_nontrivial_ws(*(it_op - 1))) {
      // Some whitespace got removed and now there isn't any
      // anymore. As it might still be needed for proper separation in
      // the output, insert a insert_ws op at that point.
	it_op = _ops.insert(it_op,
			    _op{
			      _op::action::insert_ws,
				raw_pp_tokens_range{},
				sticky_side::right,
				{}
			    }) + 1;
	++i_op;
	++n_common_cont_line_ops;
	it_common_cont_line_ops_begin = it_op - i_op;
	it_common_cont_line_ops_end =
	  it_common_cont_line_ops_begin + n_common_cont_line_ops;
      }
    }

    // Remove pre-existing trailing continuation line formatting
    // whitespace from the inner ops.
    for (std::size_t i_op = 0; i_op + 1 < n_common_cont_line_ops; ++i_op) {
      auto it_op = it_common_cont_line_ops_begin + i_op;
      if (it_op->a != _op::action::copy)
	continue;
      if (!orig_repl_toks[it_op->r.end - 1].is_ws())
	continue;

      const auto it_next_non_ws_tok =
	std::find_if(orig_repl_toks.cbegin() + it_op->r.end,
		     orig_repl_toks.cend(), not_is_ws);
      if (it_next_non_ws_tok != orig_repl_toks.cend() &&
	  !is_cont_line_marker(*it_next_non_ws_tok)) {
	continue;
      }

      while (orig_repl_toks[it_op->r.end - 1].is_ws())
	--it_op->r.end;

      if (it_op + 1 != _ops.end() && !op_head_has_nontrivial_ws(*(it_op + 1))) {
      // Some whitespace got removed and now there isn't any anymore. As
      // it might still be needed for proper separation in the output,
      // insert a insert_ws op at that point.
	it_op = _ops.insert(it_op + 1,
			    _op{
			      _op::action::insert_ws,
				raw_pp_tokens_range{},
				sticky_side::left,
				{}
			    }) - 1;
	++n_common_cont_line_ops;
	it_common_cont_line_ops_begin = it_op - i_op;
	it_common_cont_line_ops_end =
	  it_common_cont_line_ops_begin + n_common_cont_line_ops;
      }
    }

    it_common_cont_line_ops_begin = it_common_cont_line_ops_end;
  }

  // Third phase: shuffle sequences of insert + insert_ws ops over
  // continuation line formatting whitespace as appropriate:
  // - if the stickiness is left, move to before preceeding trailing
  //   continuation line formatting whitespace + markers.
  // - if the stickiness is right, move to after subsequent leading
  //   continuation line formatting whitespace.
  //
  // To retain the relative order of insertion ops, there
  // will be two loops:
  // - one forward loop, which, after forcing the operations'
  //   stickinesses into a well-defined and globally consistent state,
  //   may move insertion ops towards the front and
  // - a backward loop moving insertion ops towards the back.
  for (auto it_op = _ops.begin(); it_op != _ops.end(); ++it_op) {
    if ((it_op->a != _op::action::insert || is_cont_marker_insert_op(*it_op)) &&
	it_op->a != _op::action::insert_ws) {
      continue;
    }

    const auto ops_bw_end = _ops.rend();
    auto it_prev_op =
      std::find_if(std::make_reverse_iterator(it_op), ops_bw_end,
		   [&](const _op &o) {
		     return (not_is_all_ws_or_cont_line_markers_copy_op(o) &&
			     !is_cont_marker_insert_op(o));
		   });
    // If the stickiness is unspecified, try to derive a default from
    // surrounding context first.
    if (it_op->stickiness == sticky_side::none) {
      // If the first preceeding op, which is not an all-whitespace copy
      // is an insertion op, take its stickiness.
      if (it_prev_op != ops_bw_end &&
	  (it_prev_op->a == _op::action::insert ||
	   it_prev_op->a == _op::action::insert_ws)) {
	// The previous insertion op has been visited already.
	assert(it_prev_op->stickiness != sticky_side::none);
	it_op->stickiness = it_prev_op->stickiness;
      } else {
	// Otherwise, if the first subsequent op, which is not an
	// all-whitespace copy and not an insertion op having no
	// stickiness either, is an insertion op, take its stickiness.
	const auto ops_fw_end = _ops.end();
	auto it_next_op =
	  std::find_if(it_op + 1, ops_fw_end,
		       [&](const _op &o) {
			 // Ignore insert + insert_ws which also
			 // have no stickiness specified.
			 if ((o.a == _op::action::insert ||
			      o.a == _op::action::insert_ws) &&
			     o.stickiness == sticky_side::none) {
			   return false;
			 }
			 return
			   (not_is_all_ws_or_cont_line_markers_copy_op(o) &&
			    !is_cont_marker_insert_op(o));
		       });
	if (it_next_op != ops_fw_end &&
	    (it_next_op->a == _op::action::insert ||
	     it_next_op->a == _op::action::insert_ws)) {
	  assert(it_next_op->stickiness != sticky_side::none);
	  it_op->stickiness = it_next_op->stickiness;
	} else {
	  // Otherwise, if the preceeding or subsequent op is a
	  // replace, indicating that some editing is going on on that
	  // side anyways, make the stickiness towards that.
	  if (it_prev_op != ops_bw_end &&
	      it_prev_op->a == _op::action::replace) {
	    it_op->stickiness = sticky_side::left;
	  } else if (it_next_op != ops_fw_end &&
		     it_next_op->a == _op::action::replace) {
	    it_op->stickiness = sticky_side::right;
	  } else {
	    // Otherwise, it's an insertion op with no stickiness and
	    // surrounded only by copy ops, if any. Choose a default
	    // stickiness.  If there even is anything preceeding
	    // (i.e. a non-all-whitespace copy op), pick
	    // left. Otherwise, if there's any subsequent
	    // non-all-whitespace copy op, pick right. Otherwise, pick
	    // left for definiteness.
	    if (it_prev_op != ops_bw_end) {
	      assert(it_prev_op->a == _op::action::copy);
	      it_op->stickiness = sticky_side::left;
	    } else if (it_next_op != ops_fw_end) {
	      assert(it_next_op->a == _op::action::copy);
	      it_op->stickiness = sticky_side::right;
	    } else {
	      it_op->stickiness = sticky_side::left;
	    }
	  }
	}
      }
    } else if (it_op->stickiness == sticky_side::right) {
      // It makes no sense to have insertion ops with right-side
      // stickiness preceeding insertion ops with a left-side
      // stickiness. If that's the case, flip the current op's
      // stickiness to left-sided.
      const auto ops_fw_end = _ops.end();
      auto it_next_op =
	std::find_if(it_op + 1, ops_fw_end,
		     [&](const _op &o) {
		       // Ignore insert + insert_ws which also
		       // have no stickiness specified.
		       // Furthermore, skip over inser +
		       // insert_ws having the same stickiness,
		       // i.e. right.
		       if ((o.a == _op::action::insert ||
			    o.a == _op::action::insert_ws) &&
			   (o.stickiness == sticky_side::none ||
			    o.stickiness == sticky_side::right)){
			 return false;
		       }
		       return (not_is_all_ws_or_cont_line_markers_copy_op(o) &&
			       !is_cont_marker_insert_op(o));
		     });
      if (it_next_op != ops_fw_end &&
	  (it_next_op->a == _op::action::insert ||
	   it_next_op->a == _op::action::insert_ws) &&
	  it_next_op->stickiness == sticky_side::left) {
	it_op->stickiness = sticky_side::left;
      }
    }
    assert(it_op->stickiness != sticky_side::none);

    // Now, after the stickiness has been made well-defined and
    // globally consistent, move insertion ops with left stickiness
    // across preceeding continuation line formatting whitespace.
    if (it_op->stickiness != sticky_side::left)
      continue;

    // All the ops between it_prev_op.base() and it_op are copy ops
    // spanning only whitespace and continuation line markers. For
    // preserving whitespace separation when moving the current
    // insertion op across that sequence below, see if they contain
    // any actual whitespace.
    bool crossed_any_ws =
      std::any_of(it_prev_op.base(), it_op,
		  [&](const _op &o) {
		    // Additionally inserted continuation line markers
		    // will always be accompanied by a whitespace in
		    // the output.
		    return
		      (is_cont_marker_insert_op(o) ||
		       std::any_of(orig_repl_toks.cbegin() + o.r.begin,
				   orig_repl_toks.cbegin() + o.r.end,
				   is_ws));
		  });
    if (it_prev_op != ops_bw_end &&
	it_prev_op->a == _op::action::copy &&
	is_cont_line_marker(orig_repl_toks[it_prev_op->r.end - 1])) {
      // The preceeding non-all-whitespace-copy op includes the
      // trailing whitespace part of a continuation line, if any (+
      // the marker). Split that up and move the current op in front
      // of that trailing whitespace part.
      raw_pp_tokens_range ws_range{
	--it_prev_op->r.end
      };
      assert(it_prev_op->r.end > it_prev_op->r.begin);
      while (orig_repl_toks[it_prev_op->r.end - 1].is_ws()) {
	--it_prev_op->r.end;
	assert(it_prev_op->r.end > it_prev_op->r.begin);
	--ws_range.begin;
      }
      crossed_any_ws |= ws_range.length() > 1;

      // Move the current op to its destination position
      // before invalidating iterators.
      std::rotate(it_prev_op.base(), it_op, it_op + 1);
      it_op = it_prev_op.base();

      if (crossed_any_ws &&
	  it_op->a != _op::action::insert_ws) {
	// There had been some whitespace in front of the moved
	// insertion it_op, but now won't be anymore. Whitespace might
	// still be needed for proper separation though. Insert an
	// insert_ws op.
	it_op = _ops.insert(it_op,
			    _op{
			      _op::action::insert_ws,
				raw_pp_tokens_range{},
				sticky_side::left,
				{}
			    }) + 1;
      }

      // And insert a copy op for the white space split off above
      // after that.
      it_op = _ops.insert(it_op + 1,
			  _op{
			    _op::action::copy,
			      ws_range,
			      sticky_side::none,
			      {}
			  }) - 1;
    } else if (it_prev_op.base() != it_op) {
      // There are some preceeding pure all-whitespace copy ops to move
      // the current op ahead of.
      std::rotate(it_prev_op.base(), it_op, it_op + 1);
      it_op = it_prev_op.base();

      if (crossed_any_ws &&
	  it_op->a != _op::action::insert_ws &&
	  it_prev_op != ops_bw_end &&
	  !op_tail_has_nontrivial_ws(*it_prev_op)) {
	// There had been some whitespace in front of the moved
	// insertion it_op, but now won't be anymore. Whitespace might
	// still be needed for proper separation though. Insert an
	// insert_ws op.
	it_op = _ops.insert(it_op,
			    _op{
			      _op::action::insert_ws,
				raw_pp_tokens_range{},
				sticky_side::left,
				{}
			    }) + 1;
      }
    }
  }
  // The second, backwards loop moving insertion ops with right
  // side stickiness across subsequent continuation line formatting
  // whitespace.
  for (auto it_op = _ops.rbegin(); it_op != _ops.rend(); ++it_op) {
    if (((it_op->a != _op::action::insert ||
	  is_cont_marker_insert_op(*it_op)) &&
	 it_op->a != _op::action::insert_ws) ||
	it_op->stickiness != sticky_side::right) {
      continue;
    }
    const auto ops_fw_end = _ops.end();
    auto it_next_op =
      std::find_if(it_op.base(), ops_fw_end,
		   [&](const _op &o) {
		     return (not_is_all_ws_or_cont_line_markers_copy_op(o) &&
			     !is_cont_marker_insert_op(o));
		   });
    // All the ops between it_op.base() and it_next_op are copy ops
    // spanning only whitespace and continuation line markers. For
    // preserving whitespace separation when moving the current
    // insertion op across that sequence below, see if they contain
    // any actual whitespace.
    bool crossed_any_ws =
      std::any_of(it_op.base(), it_next_op,
		  [&](const _op &o) {
		    assert(o.a == _op::action::copy);
		    return
		      (is_cont_marker_insert_op(o) ||
		       std::any_of(orig_repl_toks.cbegin() + o.r.begin,
				   orig_repl_toks.cbegin() + o.r.end,
				   is_ws));
		  });
    if (it_next_op != ops_fw_end &&
	it_next_op->a == _op::action::copy &&
	orig_repl_toks[it_next_op->r.begin].is_ws() &&
	(it_next_op->r.begin == 0 ||
	 is_cont_line_marker(orig_repl_toks[it_next_op->r.begin - 1]))) {
      // The subsequent non-all-whitespace-copy op includes the
      // leading formatting whitespace part of a continuation
      // line. Split that up and move the current op past that
      // whitespace part.
      raw_pp_tokens_range ws_range{
	it_next_op->r.begin++
      };
      while (orig_repl_toks[it_next_op->r.begin].is_ws()) {
	++it_next_op->r.begin;
	assert(it_next_op->r.begin < it_next_op->r.end);
	++ws_range.end;
      }
      crossed_any_ws = true;

      // Move the current op to its destination position
      // before invalidating iterators.
      std::rotate(it_op.base() - 1, it_op.base(), it_next_op);
      it_op = std::make_reverse_iterator(it_next_op);

      if (crossed_any_ws && it_op->a != _op::action::insert_ws) {
	// There had been some whitespace after the moved insertion
	// it_op, but now won't be anymore. Whitespace might still be
	// needed for proper separation though. Insert an insert_ws
	// op.
	it_op = (std::make_reverse_iterator
		 (_ops.insert(it_op.base(),
			      _op{
				_op::action::insert_ws,
				raw_pp_tokens_range{},
				sticky_side::right,
				{}
			      })));
      }

      // And insert a copy op for the white space split off above
      // after that (relative to reverse order).
      it_op = (std::make_reverse_iterator
	       (_ops.insert(it_op.base() - 1,
			    _op{
			      _op::action::copy,
			       ws_range,
			       sticky_side::none,
			       {}
			    })
		+ 1));


    } else if (it_next_op != it_op.base()) {
      // There are some subsequent pure all-whitespace copy ops to move
      // the current op past by.
      std::rotate(it_op.base() - 1, it_op.base(), it_next_op);
      it_op = std::make_reverse_iterator(it_next_op);

      if (crossed_any_ws &&
	  it_op->a != _op::action::insert_ws &&
	  it_next_op != ops_fw_end &&
	  !op_head_has_nontrivial_ws(*it_next_op)) {
	// There had been some whitespace after the moved insertion
	// it_op, but now won't be anymore. Whitespace might still be
	// needed for proper separation though. Insert an insert_ws
	// op.
	it_op = (std::make_reverse_iterator
		 (_ops.insert(it_op.base(),
			      _op{
				_op::action::insert_ws,
				raw_pp_tokens_range{},
				sticky_side::right,
				{}
			      })));
      }
    }
  }

  // Fourth phase:
  // - connect copy/replace ops separated only by whitespace in the input,
  // - trim certain remaining pre-existing whitespace at the head and
  //   tail of copy ops as appropriate and replace with insert_ws ops
  //   and
  // - prune superfluous insert_ws.
  _ops.erase(_ops.cbegin(),
	     std::find_if(_ops.cbegin(), _ops.cend(),
			  [](const _op &o) {
			    return o.a != _op::action::insert_ws;
			  }));
  for (auto it_op = _ops.begin(); it_op != _ops.end();) {
    auto it_next_op =
      std::find_if(it_op + 1, _ops.end(),
		   [](const _op &o) {
		     return o.a != _op::action::insert_ws;
		   });
    if (it_next_op == _ops.end()) {
      // At insert_ws tail.
      assert(it_op->a != _op::action::insert_ws);
      _ops.erase(it_op + 1, _ops.end());
      break;
    }

    // Try to join ops separated only by whitespace in the input.
    bool joined_ops = false;
    if ((it_op->a == _op::action::copy ||
	 it_op->a == _op::action::replace) &&
	(it_next_op->a == _op::action::copy ||
	 it_next_op->a == _op::action::replace) &&
	it_op->r.begin <= it_next_op->r.end) {
      // There might still be pre-existing, non-normalized whitespace
      // at the head/tail of copy ops (if not associated with
      // continuation line formatting whitespace from the input). Trim
      // the ranges before checking whether the two ops can get joined
      // to avoid false negatives just because of overlapping
      // whitespace.
      //
      // Recall that
      // - if the first op ends with leading contination line
      //   formatting whitespace (i.e. whitespace taken from the
      //   beginning of some input continuation line), then the next
      //   op's head would have already been trimmed in a previous
      //   phase, and that whitespace is located strictly before the
      //   next op.
      // - likewise, if the second op starts with trailing continuation
      //   line formatting whitespace (i.e. whitespace* + continuation
      //   line marker taken from some continuation line's tail),
      //   then the first op's tail would have already been trimmed
      //   and that whitespace is located strictly subsequent to
      //   the first op.
      // Thus, by trimming below only when potentially needed for
      // joining, i.e. when the first op's end is not located before
      // or at the second op's beginning, it is being made sure that
      // previously inserted continuation line formatting whitespace
      // will not get truncated away here again.
      pp_token_index op_no_ws_end = it_op->r.end;
      pp_token_index next_op_no_ws_begin = it_next_op->r.begin;
      if (it_op->r.end > it_next_op->r.begin) {
	if (it_op->a == _op::action::copy) {
	  op_no_ws_end =
	    ((std::find_if
	      (std::make_reverse_iterator(orig_repl_toks.cbegin() +
					  op_no_ws_end),
	       orig_repl_toks.crend(), not_is_ws))
	     .base()
	     - orig_repl_toks.cbegin());
	  assert(op_no_ws_end > it_op->r.begin);
	}
	if (it_next_op->a == _op::action::copy) {
	  next_op_no_ws_begin =
	    ((std::find_if
	      (orig_repl_toks.cbegin() + next_op_no_ws_begin,
	       orig_repl_toks.cend(), not_is_ws))
	     - orig_repl_toks.cbegin());
	  assert(next_op_no_ws_begin < it_next_op->r.end);
	}
      }

      if (op_no_ws_end <= next_op_no_ws_begin &&
	  std::all_of(orig_repl_toks.cbegin() + op_no_ws_end,
		      orig_repl_toks.cbegin() + next_op_no_ws_begin,
		      is_ws_or_cont_line_marker)) {
	// The two copy/replace ops are separated only by whitespace +
	// continuation line markers, if anything. Join them.
	// If both are copy ops, join them directly into a single one,
	// otherwise extend the one copy op to the other or even
	// insert a new one (if both are replace ops).
	if (it_op->a == _op::action::copy &&
	    it_next_op->a == _op::action::copy) {
	  // Do not respect any insert_ws inbetween and silently
	  // dismiss them all.  If the consecutive range did have
	  // proper separation in the input, then it will also have
	  // it when copied to the output verbatim.
	  it_op->r.end = it_next_op->r.end;
	  it_op = _ops.erase(it_op + 1, it_next_op + 1) - 1;
	  // Redo the loop with the current, joined op.
	  continue;
	} else {
	  // At least one is a replace op whose associated
	  // range would not have been trimmed above.
	  assert(op_no_ws_end == it_op->r.end ||
		 next_op_no_ws_begin == it_next_op->r.begin);
	  if (it_op + 1 != it_next_op) {
	    // There are some insert_ws inbetween the two ops.  If
	    // there would be some whitespace in the input range
	    // spanned by the junction, then none of those would be
	    // needed anymore. Otherwise retain one.
	    bool junction_has_ws = false;
	    if (std::any_of(orig_repl_toks.cbegin() + op_no_ws_end,
			    orig_repl_toks.cbegin() + next_op_no_ws_begin,
			    is_ws)) {
	      junction_has_ws = true;
	    } else if (it_op->a == _op::action::copy &&
		       op_no_ws_end == it_op->r.end &&
		       op_tail_has_nontrivial_ws(*it_op)) {
		junction_has_ws = true;
	    } else if (it_next_op->a == _op::action::copy &&
		       next_op_no_ws_begin == it_next_op->r.begin &&
		       op_head_has_nontrivial_ws(*it_next_op)) {
		junction_has_ws = true;
	    }
	    // Remove the separating insert_ws tokens. As exlained
	    // above, if !junction_has_ws, then retain one, otherwise
	    // purge them all.
	    std::rotate(it_op + 1 + !junction_has_ws,
			it_next_op, it_next_op + 1);
	    const auto insert_ws_end = it_next_op + 1;
	    it_next_op = it_op + 1 + !junction_has_ws;
	    it_op =
	      _ops.erase(it_next_op + 1, insert_ws_end) - 2 - !junction_has_ws;
	    it_next_op = it_op + 1 + !junction_has_ws;
	  }
	  if (it_op->a == _op::action::copy) {
	    it_op->r.end = it_next_op->r.begin;
	  } else if (it_next_op->a == _op::action::copy) {
	    it_next_op->r.begin = it_op->r.end;
	  } else if (it_op->r.end != it_next_op->r.begin) {
	    // Join the two replace ops by a copy op
	    // spanning the separating whitespace.
	    const bool separated_by_insert_ws = it_op + 1 != it_next_op;
	    const unsigned int insertion_offset =
	      (!separated_by_insert_ws ?
	       0 :
	       ((it_op + 1)->stickiness == sticky_side::left ? 1 : 0));
	    it_op = _ops.insert(it_op + 1 + insertion_offset,
				_op{
				  _op::action::copy,
				  raw_pp_tokens_range{
				    it_op->r.end,
				    it_next_op->r.begin,
				  },
				  sticky_side::none,
				  {}
				}) - 1 - insertion_offset;
	    // Deliberately set it_next_op to the newly inserted op.
	    it_next_op = it_op + 1 + insertion_offset;
	  }
	  joined_ops = true;
	}
      }
    }

    // If the current op is a copy ending with whitespace, which is
    // not continuation line formatting whitespace, and does not come
    // from extending the copy op to the beginning of the next replace
    // op above, then trim that and insert a insert_ws instead (if
    // none there already).
    //
    // Igoring junctions inserted inbetween two replace ops separated
    // only by whitespace + continuation line makers for a moment, the
    // trailing whitespace is continuation line formatting whitespace
    // if either
    // - the trailing sequence is headed by a continuation line marker
    //   or
    // - the op is an all-whitespace copy op.
    // To see this, note that the only possible all-whitespace copy ops
    // before the joining op just above are the leading continuation
    // line formatting whitespace sequences, and these could have been
    // joined only to the back of the trailing continuation line
    // formatting immediately preceeding it in the original input.
    //
    // In the case of junctions inserted between two replace ops in
    // the op-joining step above, it_op will eventually point to such
    // one in the subsequent loop iteration, with joined_ops being
    // false. However, these two conditions listed above happen to
    // apply to such junctions as well, and thus, are required and
    // sufficient for matching whitespace that should be retained.
    if (!joined_ops &&
	it_op->a == _op::action::copy
	&& orig_repl_toks[it_op->r.end - 1].is_ws()) {
      const auto copy_toks_bw_end =
	std::make_reverse_iterator(orig_repl_toks.cbegin() + it_op->r.begin);
      const auto it_last_non_ws_tok
	= std::find_if((std::make_reverse_iterator
			(orig_repl_toks.cbegin() + it_op->r.end - 1)),
			copy_toks_bw_end, not_is_ws);
      if (it_last_non_ws_tok != copy_toks_bw_end &&
	  !is_cont_line_marker(*it_last_non_ws_tok)) {
	it_op->r.end = it_last_non_ws_tok.base() - orig_repl_toks.cbegin();
	if (it_next_op == it_op + 1) {
	  it_op = _ops.insert(it_op + 1,
			      _op{
				_op::action::insert_ws,
				raw_pp_tokens_range{},
				sticky_side::left,
				{}
			      }) - 1;
	  it_next_op = it_op + 2;
	}
      }
    }
    // Likewise, if the next op is a copy op starting with whitespace,
    // which is not continuation line formatting whitespace, and does
    // not come from extending the copy op to the beginning of the
    // preceeding replace it_op above, then trim that and insert a
    // insert_ws instead (if none there).
    //
    // Note that by virtue of the loop logic,
    // - it_next_op cannot yet be the result of merging two adjacent
    //   copy ops into one above,
    // - and if its head has somehow been extened to align with a
    //   preceeding replace op (or the whole copy op even been
    //   inserted) in the joining logic above, then joined_ops would
    //   be set.
    //
    // Thus, unlike it's the case for when attempting to strip
    // whitespace at the tail of the preceeding it_op above, the case
    // that the leading whitespace here stems from extending the copy
    // it_next_op in the joining logic can be caught by testing
    // joined_ops.
    //
    // The leading whitespace is continuation line formatting
    // whitespace if either
    // - the preceeding character in the original input is a
    //   continuation line marker or
    // - if the sequence of leading whitespace is terminated by a
    //   continuation line marker (in which case the copy op comprises
    //   exactly that trailing continuation line formatting
    //   whitespace).
    if (!joined_ops &&
	it_next_op->a == _op::action::copy &&
	orig_repl_toks[it_next_op->r.begin].is_ws() &&
	(it_next_op->r.begin != 0 &&
	 !orig_repl_toks[it_next_op->r.begin - 1].is_empty())) {
      const auto copy_toks_fw_end = orig_repl_toks.cbegin() + it_next_op->r.end;
      const auto it_first_non_ws_tok
	= std::find_if(orig_repl_toks.cbegin() + it_next_op->r.begin + 1,
		       copy_toks_fw_end, not_is_ws);
      // All-ws copy ops are possible only as leading continuation
      // line formatting whitespace, and that's been ruled out by the
      // condition above.
      assert(it_first_non_ws_tok != copy_toks_fw_end);
      // Similarly, as it_next_op has not been yet attempted to get
      // merged with any subsequent op adjacent to it, a leading
      // whitespace sequence at the head terminated with a
      // continuation line marker is possible only if that copy op
      // contains exactly some continuation line's trailing formatting
      // whitespace (including the marker) and nothing else.
      assert(!is_cont_line_marker(*it_first_non_ws_tok) ||
	     it_first_non_ws_tok + 1 == copy_toks_fw_end);
      if (!is_cont_line_marker(*it_first_non_ws_tok)) {
	it_next_op->r.begin = it_first_non_ws_tok - orig_repl_toks.cbegin();
	if (it_next_op == it_op + 1) {
	  it_op = _ops.insert(it_op + 1,
			      _op{
				_op::action::insert_ws,
				raw_pp_tokens_range{},
				sticky_side::right,
				{}
			      }) - 1;
	  it_next_op = it_op + 2;
	}
      }
    }

    // Purge redundant insert_ws ops.
    if (it_op + 1 != it_next_op &&
	((it_op->a == _op::action::copy &&
	  (op_tail_has_nontrivial_ws(*it_op) ||
	   orig_repl_toks[it_op->r.end - 1].is_punctuator("(") ||
	   orig_repl_toks[it_op->r.end - 1].is_punctuator("[") ||
	   orig_repl_toks[it_op->r.end - 1].is_punctuator("{"))) ||
	 ((it_op->a == _op::action::replace ||
	   it_op->a == _op::action::insert) &&
	  !it_op->new_toks.empty() &&
	  (it_op->new_toks.back().tok.is_punctuator("(") ||
	   it_op->new_toks.back().tok.is_punctuator("[") ||
	   it_op->new_toks.back().tok.is_punctuator("{"))))) {
      // It's a copy op ending with whitespace or some op ending in a
      // character commonly not separated by subsequent
      // whitespace. Remove all subsequent insert_ws tokens.
      const auto insert_ws_end = it_next_op;
      it_op = _ops.erase(it_op + 1, insert_ws_end) - 1;
      it_next_op = it_op + 1;
    } else if (it_op->a == _op::action::insert_ws) {
      const auto insert_ws_end = it_next_op;
      if ((it_next_op->a == _op::action::copy &&
	   (op_head_has_nontrivial_ws(*it_next_op) ||
	    orig_repl_toks[it_next_op->r.begin].is_punctuator(")") ||
	    orig_repl_toks[it_next_op->r.begin].is_punctuator("]") ||
	    orig_repl_toks[it_next_op->r.begin].is_punctuator("}"))) ||
	  ((it_next_op->a == _op::action::replace ||
	    it_next_op->a == _op::action::insert) &&
	   !it_next_op->new_toks.empty() &&
	   (it_next_op->new_toks.front().tok.is_punctuator(")") ||
	    it_next_op->new_toks.front().tok.is_punctuator("]") ||
	    it_next_op->new_toks.front().tok.is_punctuator("}")))) {
	// There's a subsequent copy op starting with whitespace or
	// some op starting with a character commonly not separated by
	// preceeding whitespace. Remove the insert_ws op sequence
	// headed by the current op.
	it_op = _ops.erase(it_op, insert_ws_end);
	it_next_op = it_op;
      } else {
	// Otherwise, retain only the first in a sequence of insert_ws
	// ops.
	const auto insert_ws_end = it_next_op;
	it_op = _ops.erase(it_op + 1, insert_ws_end) - 1;
	it_next_op = it_op + 1;
      }
    }
    ++it_op;
  }

  // Final phase: finishing touch.
  // - Insert whitespace + a continuation line marker, if any, from
  //   before the replacement text.
  // - If the last op is copy or replace, and is associated with the
  //   last non-ws token in the original input, extend it all the way
  //   to the end to include trailing whitespace and continuation line
  //   markers. Otherwise trim any trailing whitespace from the last
  //   copy op.
  if (!_ops.empty()) {
    raw_pp_token_index leading_ws_toks_end_pos =
      (std::find_if(orig_repl_toks.cbegin(), orig_repl_toks.cend(),
		    not_is_ws)
       - orig_repl_toks.cbegin());
    // If there's only whitespace + a continuation line marker
    // at the first continuation line containing the #define,
    // then include the marker.
    if (leading_ws_toks_end_pos != orig_repl_toks.size() &&
	is_cont_line_marker(orig_repl_toks[leading_ws_toks_end_pos])) {
      ++leading_ws_toks_end_pos;
    } else if (!macro_contains_any_cont_line) {
      // The whole macro definition lives on a single line. The
      // leading whitespace had already been added when amending
      // leading continuation line formatting whitespace, if any.
      assert(!(_ops.front().a == _op::action::copy ||
	       _ops.front().a == _op::action::replace) ||
	     _ops.front().r.begin == 0);
      leading_ws_toks_end_pos = 0;
    }
    _op &first_op = _ops.front();
    if ((first_op.a == _op::action::copy ||
	 first_op.a == _op::action::replace) &&
	std::all_of(orig_repl_toks.cbegin() + leading_ws_toks_end_pos,
		    orig_repl_toks.cbegin() + first_op.r.begin,
		    is_ws_or_cont_line_marker)) {
      // The initial whitespace extends all the way to the
      // first copy/replace op's position. Include all of it.
      leading_ws_toks_end_pos = first_op.r.begin;
    }
    if (first_op.a == _op::action::copy &&
	leading_ws_toks_end_pos == first_op.r.begin) {
      first_op.r.begin = 0;
    } else if (leading_ws_toks_end_pos != 0) {
      _ops.insert
	(_ops.begin(),
	 _op{
	   _op::action::copy,
	   raw_pp_tokens_range{0, leading_ws_toks_end_pos},
	   sticky_side::none,
	   {}
	 });
    }
    assert(_ops.front().a == _op::action::copy);

    auto all_ws_or_cont_line_markers_ops_tail_begin =
      (std::find_if(_ops.rbegin(), _ops.rend(),
		    [&](const _op &o) {
		      return (not_is_all_ws_or_cont_line_markers_copy_op(o) &&
			      o.a != _op::action::insert_ws);
		    })
       .base());

    // _ops had not been empty even before adding the leading
    // whitespace, as per the initial if-condition, which means that
    // there's at least one non-ws-emitting op.
    assert(all_ws_or_cont_line_markers_ops_tail_begin != _ops.begin());
    _ops.erase(all_ws_or_cont_line_markers_ops_tail_begin, _ops.end());

    _op &last_op = _ops.back();
    if ((last_op.a == _op::action::copy || last_op.a == _op::action::replace) &&
	std::all_of(orig_repl_toks.cbegin() + last_op.r.end,
		    orig_repl_toks.cend(),
		    is_ws_or_cont_line_marker)) {
      // The trailing whitespace extends all the way to the
      // last copy/replace op's position. Include all of it.
      if (last_op.a == _op::action::copy) {
	last_op.r.end = orig_repl_toks.size();
      } else if (last_op.r.end != orig_repl_toks.size()) {
      _ops.push_back
	(_op{
	   _op::action::copy,
	   raw_pp_tokens_range{last_op.r.end, orig_repl_toks.size()},
	   sticky_side::none,
	   {}
	 });
      }
    } else if (last_op.a == _op::action::copy) {
      // Trim any trailing whitespace from the last copy op.
      last_op.r.end = ((std::find_if
			((std::make_reverse_iterator
			  (orig_repl_toks.cbegin() + last_op.r.end)),
			 (std::make_reverse_iterator
			  (orig_repl_toks.cbegin() + last_op.r.begin)),
			 not_is_ws_or_cont_line_marker)).base()
		       - orig_repl_toks.cbegin());
    }

  } else if (!orig_repl_toks.empty() &&
	     std::all_of(orig_repl_toks.cbegin(), orig_repl_toks.cend(),
			 is_ws_or_cont_line_marker)) {
    // The rewritten macro replacement text is all empty, and the
    // original input comprised only whitespace as well. Copy it as-is.
    _ops.push_back
      (_op{
	 _op::action::copy,
	 raw_pp_tokens_range{0, orig_repl_toks.size()},
	 sticky_side::none,
	 {}
       });
  }
}

using macro_invocation = pp_result::macro_invocation;
using expansion_forest = macro_invocation::expansion_forest;
using rescan_macro_invocation = expansion_forest::rescan_macro_invocation;

class rescan_subtree_rewrite_cursor;
class rescan_macro_invocation_edit_script;
class unexpanded_macro_id_out_tokens_origin_traces;

class rewritten_expansion_forest
{
public:

private:
  class _rewritten_rescan_macro_invocation;

  class _rewritten_forest_nodes
  {
  private:
    typedef std::vector<_rewritten_rescan_macro_invocation> _container_type;
    const expansion_forest &_original_forest;
    _container_type _nodes;

  public:
    _rewritten_forest_nodes(const expansion_forest &original_forest) noexcept;

    class size_type
    {
    public:
      constexpr bool operator==(const size_type rhs) const noexcept
      {
	return this->_n == rhs._n;
      }

      constexpr bool operator!=(const size_type rhs) const noexcept
      { return !(*this == rhs); }

      constexpr bool operator<(const size_type rhs) const noexcept
      { return this->_n < rhs._n; }

      static constexpr const size_type invalid() noexcept
      {
	return size_type{
	  std::numeric_limits<_container_type::size_type>::max()
	};
      }

    private:
      friend class rewritten_expansion_forest;

      size_type() = delete;

      explicit constexpr size_type(const _container_type::size_type n) noexcept
	: _n(n)
      {}

      _container_type::size_type _n;
    }; // class size_type

    const _rewritten_rescan_macro_invocation& operator[](const size_type n)
      const noexcept;

    _rewritten_rescan_macro_invocation& operator[](const size_type n)
      noexcept;

  private:
    friend class rewritten_expansion_forest;

    // Until _finalize_structure() has established the serialization
    // order, the structural information not representable by
    // position gets kept on the side.
    struct _construction_info
    {
      _construction_info() noexcept;

      size_type arg_owner;
      std::size_t arg_index;
      // For rescan children: position of the name token within the
      // rescan parent's output, used for ordering siblings.
      std::size_t pos_in_parent;
    };

    size_type _add_rewritten_rescan_macro_invocation
      (const expansion_forest::nodes_size_type original);

    size_type _add_arg_tree_root
      (const size_type owner, const std::size_t arg_index,
       const expansion_forest::nodes_size_type original_arg_tree_root);

    size_type _get_arg_tree_root(const size_type owner,
				 const std::size_t arg_index) const noexcept;

    void _link_rescan_child(const size_type parent, const size_type child,
			    const std::size_t pos_in_parent) noexcept;

    // Structural queries, deduced from the serialization order once
    // finalized, c.f. the original expansion forest's
    // _forest_nodes.
    size_type _arg_owner(const size_type arg_root) const noexcept;
    std::size_t _arg_index(const size_type arg_root) const noexcept;
    size_type _first_arg_of(const size_type node) const noexcept;
    size_type _next_arg(const size_type prev_arg_root) const noexcept;
    size_type _rescan_subtree_end(const size_type subtree_root) const noexcept;

    // The enclosing node: the rescan parent, or the owner for
    // argument tree roots.
    size_type _up(const size_type node) const noexcept;

    // Argument tree roots are dummy identity nodes owned by the
    // invocation whose argument they represent. They mirror the
    // original forest's argument tree roots.
    bool _is_arg_tree_root(const size_type node) const noexcept;

    // Find the rewritten node for the given original one located
    // below (or at) subtree_root, invalid if none. An original
    // invocation can have several rewritten counterparts, one per
    // rewrite context (e.g. one below an argument tree root with
    // edits pushed down into the argument and one lowered elsewhere).
    // The candidates are disambiguated by walking their ancestor
    // chains, i.e. rescan parents and argument tree owners, towards
    // subtree_root.
    size_type _find_in_subtree(const expansion_forest::nodes_size_type original,
			       const size_type subtree_root) const noexcept;

    // Mirror an original (sub)tree which remains in effect as is,
    // i.e. without any rewrite cursor having been run on it, below the
    // given rewritten node.
    void _clone_original_subtree
      (const expansion_forest::rescan_macro_invocation &original_root,
       const size_type rewritten_root);

    // Bring the nodes into the original expansion forest's order: DFS
    // pre order with respect to the rescan tree, with each node's
    // argument trees serialized right after it.
    void _finalize_structure();

    // All rewritten nodes for a given original one, one per rewrite
    // context.
    std::map<expansion_forest::nodes_size_type, std::vector<size_type>>
      _original_to_rewritten;

    // Construction scaffolding, dropped by _finalize_structure().
    std::vector<_construction_info> _ci;
    bool _finalized = false;
  }; // class _rewritten_forest_nodes

  struct _rewritten_rescan_macro_invocation
  {
    _rewritten_rescan_macro_invocation
      (expansion_forest::nodes_size_type _original) noexcept;

    expansion_forest::nodes_size_type _original;
    // The rescan tree parent, invalid for rescan tree roots (the
    // main tree's root and argument tree roots).
    _rewritten_forest_nodes::size_type _rescan_parent;
    rewritten_macro_definition_id _rewritten_m;
    // The invocation has been removed from the rewritten structure
    // altogether.
    bool _deleted;
    // Number of argument trees. As in the original expansion
    // forest, the owner relationship is implied by the serialization
    // order (argument trees follow their owner) and the owners'
    // argument counts, c.f. _rewritten_forest_nodes.
    std::size_t _nargs;
  }; // class _rewritten_rescan_macro_invocation

  class _rewritten_rescan_macro_invocation_ref
  {
  public:
    _rewritten_rescan_macro_invocation_ref
      (_rewritten_forest_nodes &forest,
       const _rewritten_forest_nodes::size_type node) noexcept;

    _rewritten_forest_nodes& get_forest() const noexcept
    { return _rewritten_forest.get(); }

    const _rewritten_forest_nodes::size_type get_node() const noexcept
    { return _node; }

    _rewritten_rescan_macro_invocation& deref() const noexcept;

  private:
    std::reference_wrapper<_rewritten_forest_nodes> _rewritten_forest;
    _rewritten_forest_nodes::size_type _node;
  }; // class _rewritten_rescan_macro_invocation_ref

public:
  typedef _rewritten_forest_nodes::size_type nodes_size_type;

  class rewritten_rescan_macro_invocation
  {
  public:
    explicit operator nodes_size_type() const noexcept;

    expansion_forest::nodes_size_type get_original() const noexcept;

    rewritten_macro_definition_id get_rewritten_macro() const noexcept;

    void set_rewritten_macro(const rewritten_macro_definition_id rewritten_m)
      noexcept;

    bool is_deleted() const noexcept;

    void set_deleted() noexcept;

    bool is_arg_tree_root() const noexcept;

    // The rescan parent, invalid for rescan tree roots.
    nodes_size_type get_rescan_parent() const noexcept;

    // For argument tree roots: the owning invocation.
    nodes_size_type get_arg_tree_owner() const noexcept;

    std::size_t get_arg_index() const noexcept;

  private:
    friend class rewritten_expansion_forest;
    friend class rescan_subtree_rewrite_cursor;
    friend class rescan_macro_invocation_rewriter;

    rewritten_rescan_macro_invocation
      (const _rewritten_rescan_macro_invocation_ref &ref) noexcept;

    // Make child_rrmi a rescan child, pos_in_parent being the
    // position of its name token within this node's output.
    void _add_rescan_child(const nodes_size_type child_rrmi,
			   const std::size_t pos_in_parent) noexcept;

    _rewritten_rescan_macro_invocation_ref _ref;
  }; // class rewritten_rescan_macro_invocation

  // One particular identifier token in the final (rewritten) output
  // which might resolve to some macro definition. Once the rewrite
  // has determined where in the rewritten structure the token gets
  // emitted from, that position, its "context", gets recorded: the
  // rewritten node from whose output (or argument source) it
  // originates. The context's chain of enclosing invocations (rescan
  // parents, argument tree owners) are the ones active during the
  // token's rescan. If any of them is still invoking a macro with
  // the token's name, i.e. hasn't been renamed to a rewritten macro
  // definition, the token is protected from expansion by the
  // recursive expansion rules. Otherwise a macro_nondef_constraint
  // must be enforced for the token's name.
  class unexpanded_macro_id_out_token
  {
  public:
    const std::string& get_id() const noexcept
    { return _id; }

    bool has_lparen() const noexcept
    { return _has_lparen; }

    // Record the context. An invalid node denotes the raw invocation
    // source, where no protection applies at all. The token's
    // position in the rewritten structure is unique, so repeated
    // invocations are expected to carry an equivalent context and
    // are ignored.
    void set_context(const nodes_size_type context) noexcept;

    bool is_constraint_active(const rewritten_expansion_forest &forest)
      const noexcept;

  private:
    friend class rescan_subtree_rewrite_cursor;

    unexpanded_macro_id_out_token(const std::string &id,
				  const bool has_lparen);
    std::string _id;
    bool _has_lparen;
    bool _finalized;
    nodes_size_type _context;
  };

  typedef std::vector<unexpanded_macro_id_out_token>
   unexpanded_macro_id_out_tokens;

  class counter_invocation
  {
  private:
    friend class rewritten_expansion_forest;

    counter_invocation
      (const expansion_forest::nodes_size_type _original_counter_rmi,
       const std::size_t original_relative_value_before) noexcept;

    expansion_forest::nodes_size_type _original_counter_rmi;
    std::size_t _original_relative_value_before;
  }; // class counter_invocation

  typedef std::vector<counter_invocation> counter_invocations;

  const counter_invocations::size_type
  lookup_original_counter_invocation
    (const expansion_forest::nodes_size_type counter_rmi) const noexcept;

  // forced_renames lists macros all of whose invocations within the
  // rewritten forest must get renamed, even if their definitions
  // don't need any modification. Those from forwarding_renames get
  // a forwarding definition in this case
  // (rewritten_macro_definition::make_forwarding()), those from
  // copy_renames an identical copy of the original definition. The
  // latter takes precedence in case a macro is listed in both.
  rewritten_expansion_forest
    (const pp_result &pp_result,
     const macro_invocation &mi,
     rewritten_macro_definitions_pool &pool,
     const std::map<std::string, bool> &macro_ids,
     const std::set<const pp_result::macro*> &copy_renames,
     const std::set<const pp_result::macro*> &forwarding_renames);

  bool is_copy_rename_forced(const pp_result::macro &m) const noexcept
  { return _copy_renames.count(&m) != 0; }

  bool is_forwarding_rename_forced(const pp_result::macro &m) const noexcept
  { return _forwarding_renames.count(&m) != 0; }

  bool is_rename_forced(const pp_result::macro &m) const noexcept
  { return is_copy_rename_forced(m) || is_forwarding_rename_forced(m); }

  bool has_forced_renames() const noexcept
  { return !_copy_renames.empty() || !_forwarding_renames.empty(); }

  // Whether the given subtree, including any nested argument trees,
  // contains an invocation of a macro whose rename is forced.
  bool subtree_has_forced_rename(const expansion_forest::rescan_subtree &st)
    const noexcept;

  // Whether the given subtree, including any nested argument trees,
  // contains an invocation of __COUNTER__.
  bool subtree_invokes_counter(const expansion_forest::rescan_subtree &st)
    const noexcept;

  const pp_result& get_pp_result() const noexcept
  { return _pp_result; }

  const macro_invocation& get_macro_invocation() const noexcept
  { return _mi; }

  const expansion_forest& get_original_forest() const noexcept
  { return _original_forest; }

  rewritten_macro_definitions_pool& get_pool() const noexcept
  { return _pool; }

  const std::map<std::string, bool>& get_macro_ids() const noexcept
  { return _macro_ids; }

  rewritten_rescan_macro_invocation
  get_rewritten_rescan_macro_invocation(const nodes_size_type rrmi) noexcept;

  // An original rescan_macro_invocation can get rewritten in more
  // than one context: once for its position in the main rescan tree
  // resp. in a (pushed down) argument's expansion tree and once more
  // for each lowered reference to that argument. Each context has
  // its own rewritten_rescan_macro_invocation.
  //
  // Whether an identifier token with the given name scanned in the
  // given context is protected from macro expansion by the recursive
  // expansion rules, i.e. whether any of the enclosing invocations
  // still invokes a macro with that name.
  bool is_protected_from_expansion(const nodes_size_type context,
				   const std::string &id) const noexcept;

  // Find the rewritten node for the given original one located below
  // (or at) subtree_root, invalid if none. Used for determining the
  // rewritten context node to attribute an unexpanded macro id
  // token's trace to: among an original node's rewritten
  // counterparts, the one which lives in the rewritten structure
  // below subtree_root.
  nodes_size_type
  find_rewritten_node_in_subtree(const expansion_forest::nodes_size_type original,
				 const nodes_size_type subtree_root)
    const noexcept;

  // Add an argument tree root node for the given owner's argument.
  rewritten_rescan_macro_invocation
  add_arg_tree_root(const nodes_size_type owner, const std::size_t arg_index,
		    const expansion_forest::rescan_macro_invocation
		      &original_arg_tree_root);

  // Lookup a previously added argument tree root node.
  nodes_size_type get_arg_tree_root(const nodes_size_type owner,
				    const std::size_t arg_index)
    const noexcept;

  // Mirror an original (sub)tree which remains in effect as is,
  // i.e. without any rewrite cursor having been run on it, below the
  // given rewritten node.
  void clone_original_subtree
    (const expansion_forest::rescan_macro_invocation &original_root,
     const nodes_size_type rewritten_root);

  // Bring the nodes into the original expansion forest's order: DFS
  // pre order with respect to the rescan tree, with each node's
  // argument trees serialized right after it. To be invoked once
  // the rewrite has completed.
  void finalize_structure();

  unexpanded_macro_id_out_token&
  get_unexpanded_macro_id_out_token
    (const unexpanded_macro_id_out_tokens::size_type i) noexcept;

  rewritten_rescan_macro_invocation _add_rewritten_rescan_macro_invocation
    (const expansion_forest::rescan_macro_invocation &original);

  // Collect the final result after all rewriting has completed.
  void collect_result
    (rewritten_macro_invocation &result,
     const rescan_macro_invocation_edit_script &src_edit_script,
     const unexpanded_macro_id_out_tokens_origin_traces &src_traces);

private:
  friend class rescan_subtree_rewrite_cursor;

  const pp_result &_pp_result;
  const macro_invocation &_mi;
  const expansion_forest &_original_forest;
  rewritten_macro_definitions_pool &_pool;
  const std::map<std::string, bool> &_macro_ids;
  const std::set<const pp_result::macro*> &_copy_renames;
  const std::set<const pp_result::macro*> &_forwarding_renames;
  _rewritten_forest_nodes _rewritten_forest;

  unexpanded_macro_id_out_tokens _unexpanded_macro_id_out_tokens;
  counter_invocations _counter_invocations;
}; // class rewritten_expansion_forest


const rewritten_expansion_forest::_rewritten_rescan_macro_invocation&
rewritten_expansion_forest::_rewritten_forest_nodes::
operator[](const size_type n) const noexcept
{
  assert(n._n < _nodes.size());
  return _nodes[n._n];
}

rewritten_expansion_forest::_rewritten_rescan_macro_invocation&
rewritten_expansion_forest::_rewritten_forest_nodes::
operator[](const size_type n) noexcept
{
  assert(n._n < _nodes.size());
  return _nodes[n._n];
}

rewritten_expansion_forest::_rewritten_forest_nodes::_construction_info::
_construction_info() noexcept
  : arg_owner{size_type::invalid()}, arg_index{0}, pos_in_parent{0}
{}

rewritten_expansion_forest::_rewritten_forest_nodes::
_rewritten_forest_nodes(const expansion_forest &original_forest) noexcept
  : _original_forest{original_forest}
{}

bool rewritten_expansion_forest::_rewritten_forest_nodes::
_is_arg_tree_root(const size_type node) const noexcept
{
  return _original_forest[(*this)[node]._original].is_arg_tree_root();
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_add_rewritten_rescan_macro_invocation
  (const expansion_forest::nodes_size_type original)
{
  assert(!_finalized);
  _nodes.emplace_back(original);
  _ci.emplace_back();
  const size_type rrmi{_nodes.size() - 1};
  _original_to_rewritten[original].push_back(rrmi);
  return rrmi;
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_add_arg_tree_root
  (const size_type owner, const std::size_t arg_index,
   const expansion_forest::nodes_size_type original_arg_tree_root)
{
  assert(!_finalized);
  assert(_original_forest[original_arg_tree_root].is_arg_tree_root());
  _nodes.emplace_back(original_arg_tree_root);
  _ci.emplace_back();
  const size_type rrmi{_nodes.size() - 1};
  _ci.back().arg_owner = owner;
  _ci.back().arg_index = arg_index;
  ++(*this)[owner]._nargs;
  return rrmi;
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_get_arg_tree_root(const size_type owner, const std::size_t arg_index)
  const noexcept
{
  if (!_finalized) {
    // Creation order does not reflect the structure yet, search the
    // scaffolding.
    for (_container_type::size_type i = 0; i < _nodes.size(); ++i) {
      if (_is_arg_tree_root(size_type{i}) &&
	  _ci[i].arg_owner == owner && _ci[i].arg_index == arg_index) {
	return size_type{i};
      }
    }
    return size_type::invalid();
  }

  if (arg_index >= (*this)[owner]._nargs)
    return size_type::invalid();
  size_type arg_root = _first_arg_of(owner);
  for (std::size_t i = 0; i < arg_index; ++i)
    arg_root = _next_arg(arg_root);
  return arg_root;
}

void rewritten_expansion_forest::_rewritten_forest_nodes::
_link_rescan_child(const size_type parent, const size_type child,
		   const std::size_t pos_in_parent) noexcept
{
  assert(!_finalized);
  (*this)[child]._rescan_parent = parent;
  _ci[child._n].pos_in_parent = pos_in_parent;
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_arg_owner(const size_type arg_root) const noexcept
{
  assert(_is_arg_tree_root(arg_root));
  if (!_finalized)
    return _ci[arg_root._n].arg_owner;

  // Search backwards for the node whose argument slots cover the
  // given argument tree root, accounting for any intermediate
  // nodes' argument trees.
  _container_type::size_type pending_arg_roots = 1;
  _container_type::const_iterator it = _nodes.cbegin() + arg_root._n;
  while (it != _nodes.cbegin()) {
    const _container_type::const_iterator nit = it - 1;

    if (nit->_nargs >= pending_arg_roots)
      break;

    pending_arg_roots -= nit->_nargs;
    pending_arg_roots +=
      _is_arg_tree_root(size_type{
	  static_cast<_container_type::size_type>(nit - _nodes.cbegin())
	});

    it = nit;
  }
  if (it == _nodes.cbegin())
    return size_type::invalid();

  return size_type{
    static_cast<_container_type::size_type>(it - 1 - _nodes.cbegin())
  };
}

std::size_t rewritten_expansion_forest::_rewritten_forest_nodes::
_arg_index(const size_type arg_root) const noexcept
{
  assert(_is_arg_tree_root(arg_root));
  if (!_finalized)
    return _ci[arg_root._n].arg_index;

  const size_type owner = _arg_owner(arg_root);
  assert(owner != size_type::invalid());
  std::size_t index = 0;
  for (size_type cur = _first_arg_of(owner); cur != arg_root;
       cur = _next_arg(cur)) {
    ++index;
    assert(index < (*this)[owner]._nargs);
  }
  return index;
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_first_arg_of(const size_type node) const noexcept
{
  assert(_finalized);
  assert((*this)[node]._nargs > 0);
  const size_type arg_root{node._n + 1};
  assert(_is_arg_tree_root(arg_root));
  return arg_root;
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_next_arg(const size_type prev_arg_root) const noexcept
{
  assert(_finalized);
  assert(_is_arg_tree_root(prev_arg_root));
  const size_type next_arg_root = _rescan_subtree_end(prev_arg_root);
  assert(next_arg_root._n != _nodes.size());
  assert(_is_arg_tree_root(next_arg_root));
  return next_arg_root;
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_rescan_subtree_end(const size_type subtree_root) const noexcept
{
  assert(_finalized);
  assert(subtree_root._n < _nodes.size());
  // Find the position right after the last node from the subtree
  // located at subtree_root or from any of the argument trees
  // (recursively) referenced therefrom. Per the DFS pre
  // serialization order, subtree_root's (rescan tree) next sibling,
  // if any, will have its parent located before subtree_root.
  _container_type::size_type skip_arg_roots = (*this)[subtree_root]._nargs;
  _container_type::const_iterator it;
  for (it = _nodes.cbegin() + subtree_root._n + 1; it != _nodes.cend(); ++it) {
    const bool is_arg_root =
      _is_arg_tree_root(size_type{
	  static_cast<_container_type::size_type>(it - _nodes.cbegin())
	});
    if (!skip_arg_roots &&
	(is_arg_root ||
	 it->_rescan_parent == size_type::invalid() ||
	 it->_rescan_parent._n < subtree_root._n)) {
      break;
    }
    skip_arg_roots -= is_arg_root;
    skip_arg_roots += it->_nargs;
  }
  return size_type{
    static_cast<_container_type::size_type>(it - _nodes.cbegin())
  };
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_up(const size_type node) const noexcept
{
  return (_is_arg_tree_root(node) ?
	  _arg_owner(node) : (*this)[node]._rescan_parent);
}

rewritten_expansion_forest::_rewritten_forest_nodes::size_type
rewritten_expansion_forest::_rewritten_forest_nodes::
_find_in_subtree(const expansion_forest::nodes_size_type original,
		 const size_type subtree_root) const noexcept
{
  const auto it = _original_to_rewritten.find(original);
  if (it == _original_to_rewritten.end())
    return size_type::invalid();
  for (const auto rrmi : it->second) {
    for (size_type n = rrmi; n != size_type::invalid(); n = _up(n)) {
      if (n == subtree_root)
	return rrmi;
    }
  }
  return size_type::invalid();
}

rewritten_expansion_forest::_rewritten_rescan_macro_invocation::
_rewritten_rescan_macro_invocation
  (expansion_forest::nodes_size_type original) noexcept
  : _original{original},
    _rescan_parent{_rewritten_forest_nodes::size_type::invalid()},
    _rewritten_m{invalid_rewritten_macro_definition_id}, _deleted{false},
    _nargs{0}
{}

rewritten_expansion_forest::_rewritten_rescan_macro_invocation_ref::
_rewritten_rescan_macro_invocation_ref
  (_rewritten_forest_nodes &forest,
   const _rewritten_forest_nodes::size_type node) noexcept
  : _rewritten_forest{forest}, _node{node}
{}

rewritten_expansion_forest::_rewritten_rescan_macro_invocation&
rewritten_expansion_forest::_rewritten_rescan_macro_invocation_ref::
deref() const noexcept
{
  return _rewritten_forest.get()[_node];
}

rewritten_expansion_forest::rewritten_rescan_macro_invocation::
rewritten_rescan_macro_invocation
  (const _rewritten_rescan_macro_invocation_ref &ref) noexcept
  : _ref{ref}
{}

rewritten_expansion_forest::rewritten_rescan_macro_invocation::
operator nodes_size_type() const noexcept
{
  return _ref.get_node();
}

expansion_forest::nodes_size_type
rewritten_expansion_forest::rewritten_rescan_macro_invocation::
get_original() const noexcept
{
  return _ref.deref()._original;
}

rewritten_macro_definition_id
rewritten_expansion_forest::rewritten_rescan_macro_invocation::
get_rewritten_macro() const noexcept
{
  return _ref.deref()._rewritten_m;
}

void rewritten_expansion_forest::rewritten_rescan_macro_invocation::
set_rewritten_macro(const rewritten_macro_definition_id rewritten_m) noexcept
{
  _ref.deref()._rewritten_m = rewritten_m;
}

bool rewritten_expansion_forest::rewritten_rescan_macro_invocation::
is_deleted() const noexcept
{
  return _ref.deref()._deleted;
}

void rewritten_expansion_forest::rewritten_rescan_macro_invocation::
set_deleted() noexcept
{
  _ref.deref()._deleted = true;
}

bool rewritten_expansion_forest::rewritten_rescan_macro_invocation::
is_arg_tree_root() const noexcept
{
  return _ref.get_forest()._is_arg_tree_root(_ref.get_node());
}

rewritten_expansion_forest::nodes_size_type
rewritten_expansion_forest::rewritten_rescan_macro_invocation::
get_rescan_parent() const noexcept
{
  return _ref.deref()._rescan_parent;
}

rewritten_expansion_forest::nodes_size_type
rewritten_expansion_forest::rewritten_rescan_macro_invocation::
get_arg_tree_owner() const noexcept
{
  return _ref.get_forest()._arg_owner(_ref.get_node());
}

std::size_t rewritten_expansion_forest::rewritten_rescan_macro_invocation::
get_arg_index() const noexcept
{
  return _ref.get_forest()._arg_index(_ref.get_node());
}

void rewritten_expansion_forest::rewritten_rescan_macro_invocation::
_add_rescan_child(const nodes_size_type child_rrmi,
		  const std::size_t pos_in_parent) noexcept
{
  _ref.get_forest()._link_rescan_child(_ref.get_node(), child_rrmi,
				      pos_in_parent);
}

rewritten_expansion_forest::unexpanded_macro_id_out_token::
unexpanded_macro_id_out_token(const std::string &id, const bool has_lparen)
  : _id{id}, _has_lparen{has_lparen}, _finalized{false},
    _context{nodes_size_type::invalid()}
{}

void rewritten_expansion_forest::unexpanded_macro_id_out_token::
set_context(const nodes_size_type context) noexcept
{
  if (_finalized)
    return;
  _context = context;
  _finalized = true;
}

bool rewritten_expansion_forest::unexpanded_macro_id_out_token::
is_constraint_active(const rewritten_expansion_forest &forest) const noexcept
{
  if (!_finalized) {
    // Never finalized, assume the worst.
    return true;
  }

  return !forest.is_protected_from_expansion(_context, _id);
}

rewritten_expansion_forest::counter_invocation::
counter_invocation
  (const expansion_forest::nodes_size_type original_counter_rmi,
   const std::size_t original_relative_value_before) noexcept
  : _original_counter_rmi{original_counter_rmi},
    _original_relative_value_before{original_relative_value_before}
{}

rewritten_expansion_forest::
rewritten_expansion_forest(const pp_result &pp_result,
			   const macro_invocation &mi,
			   rewritten_macro_definitions_pool &pool,
			   const std::map<std::string, bool> &macro_ids,
			   const std::set<const pp_result::macro*>
			     &copy_renames,
			   const std::set<const pp_result::macro*>
			     &forwarding_renames)
  : _pp_result{pp_result}, _mi{mi},
    _original_forest{mi.get_expansion_forest()},
    _pool{pool}, _macro_ids{macro_ids}, _copy_renames{copy_renames},
    _forwarding_renames{forwarding_renames},
    _rewritten_forest{_original_forest},
    _counter_invocations{}
{
  // The all-nodes iterator traverses the forest in DFS pre order with
  // respect to the rescan tree relationship and argument expansions
  // before their owning macro invocation's output get's rescanned,
  // which matches the order the __COUNTER__ references got invoked.
  std::size_t original_relative_counter_value = 0;
  for (auto it_original_rmi = _original_forest.all_nodes_begin();
       it_original_rmi != _original_forest.all_nodes_end();
       ++it_original_rmi) {
    if ((*it_original_rmi).is_arg_tree_root())
      continue;

    const auto &m = (*it_original_rmi).get_macro();
    if (m.is_builtin_special() && m.get_name() == "__COUNTER__") {
      _counter_invocations.push_back
	(counter_invocation{
	  static_cast<expansion_forest::nodes_size_type>(*it_original_rmi),
	  original_relative_counter_value++,
	});
    }
    // The original rescan_macro_invocation ids are assigned in that
    // exact same order (rescan DFS pre, macro argument expansions
    // before output rescan), so the vector collected above can also
    // directly be used for lookup by those original
    // rescan_macro_invocation ids with a binary search.
    assert(std::is_sorted
	   (_counter_invocations.cbegin(), _counter_invocations.cend(),
	    [](const auto &a, const auto &b) {
	      return a._original_counter_rmi < b._original_counter_rmi;
	    }));
  }
}

rewritten_expansion_forest::rewritten_rescan_macro_invocation
rewritten_expansion_forest::_add_rewritten_rescan_macro_invocation
  (const expansion_forest::rescan_macro_invocation &original)
{
  const auto original_node =
    static_cast<expansion_forest::nodes_size_type>(original);
  const auto rrmi =
    _rewritten_forest._add_rewritten_rescan_macro_invocation(original_node);
  return rewritten_rescan_macro_invocation{
    _rewritten_rescan_macro_invocation_ref{_rewritten_forest, rrmi}
  };
}

rewritten_expansion_forest::rewritten_rescan_macro_invocation
rewritten_expansion_forest::
get_rewritten_rescan_macro_invocation(const nodes_size_type rrmi) noexcept
{
  return rewritten_rescan_macro_invocation{
    _rewritten_rescan_macro_invocation_ref{_rewritten_forest, rrmi}
  };
}

bool rewritten_expansion_forest::
is_protected_from_expansion(const nodes_size_type context,
			    const std::string &id) const noexcept
{
  for (nodes_size_type n = context; n != nodes_size_type::invalid();
       n = _rewritten_forest._up(n)) {
    const auto &node = _rewritten_forest[n];
    if (!_rewritten_forest._is_arg_tree_root(n) &&
	node._rewritten_m == invalid_rewritten_macro_definition_id) {
      const pp_result::macro &m = _original_forest[node._original].get_macro();
      if (!m.is_builtin_special() && m.get_name() == id)
	return true;
    }
  }
  return false;
}

rewritten_expansion_forest::nodes_size_type
rewritten_expansion_forest::
find_rewritten_node_in_subtree(const expansion_forest::nodes_size_type original,
			       const nodes_size_type subtree_root)
  const noexcept
{
  return _rewritten_forest._find_in_subtree(original, subtree_root);
}

rewritten_expansion_forest::rewritten_rescan_macro_invocation
rewritten_expansion_forest::
add_arg_tree_root(const nodes_size_type owner, const std::size_t arg_index,
		  const expansion_forest::rescan_macro_invocation
		    &original_arg_tree_root)
{
  assert(original_arg_tree_root.is_arg_tree_root());
  const auto rrmi =
    _rewritten_forest._add_arg_tree_root
      (owner, arg_index,
       static_cast<expansion_forest::nodes_size_type>(original_arg_tree_root));
  return rewritten_rescan_macro_invocation{
    _rewritten_rescan_macro_invocation_ref{_rewritten_forest, rrmi}
  };
}

rewritten_expansion_forest::nodes_size_type
rewritten_expansion_forest::
get_arg_tree_root(const nodes_size_type owner, const std::size_t arg_index)
  const noexcept
{
  return _rewritten_forest._get_arg_tree_root(owner, arg_index);
}

void rewritten_expansion_forest::
clone_original_subtree
  (const expansion_forest::rescan_macro_invocation &original_root,
   const nodes_size_type rewritten_root)
{
  _rewritten_forest._clone_original_subtree(original_root, rewritten_root);
}

void rewritten_expansion_forest::_rewritten_forest_nodes::
_clone_original_subtree
  (const expansion_forest::rescan_macro_invocation &original_root,
   const size_type rewritten_root)
{
  // The original forest serializes a subtree, nested argument trees
  // included, as one contiguous block in DFS pre order with each
  // node's argument trees right after it. Hence a single pass over
  // that block suffices: a node's rescan parent resp. argument tree
  // owner has always been cloned already when the node is reached.
  std::map<expansion_forest::nodes_size_type, size_type> clones;
  clones.emplace
    (static_cast<expansion_forest::nodes_size_type>(original_root),
     rewritten_root);
  const auto st = static_cast<expansion_forest::rescan_subtree>(original_root);
  auto it = st.all_nodes_begin();
  const auto end = st.all_nodes_end();
  assert(it != end);
  for (++it; it != end; ++it) {
    const expansion_forest::rescan_macro_invocation node = *it;
    const auto node_id = static_cast<expansion_forest::nodes_size_type>(node);
    if (node.is_arg_tree_root()) {
      const auto owner = node.get_arg_tree_owner();
      const auto it_owner =
	clones.find(static_cast<expansion_forest::nodes_size_type>(owner));
      assert(it_owner != clones.end());
      // Determine the argument index.
      const std::size_t nargs = owner.get_nargs();
      std::size_t arg_index = nargs;
      for (std::size_t i = 0; i < nargs && arg_index == nargs; ++i) {
	if (static_cast<expansion_forest::nodes_size_type>
	    (static_cast<expansion_forest::rescan_macro_invocation>
	     (owner.get_arg_tree(i))) == node_id) {
	  arg_index = i;
	}
      }
      assert(arg_index != nargs);
      clones.emplace(node_id,
		     _add_arg_tree_root(it_owner->second, arg_index, node_id));
    } else {
      const auto it_parent =
	clones.find(static_cast<expansion_forest::nodes_size_type>
		    (node.get_rescan_parent()));
      assert(it_parent != clones.end());
      const auto clone = _add_rewritten_rescan_macro_invocation(node_id);
      clones.emplace(node_id, clone);
      _link_rescan_child(it_parent->second, clone, node.get_src_range().begin);
    }
  }
}

void rewritten_expansion_forest::finalize_structure()
{
  _rewritten_forest._finalize_structure();
}

void rewritten_expansion_forest::_rewritten_forest_nodes::_finalize_structure()
{
  assert(!_finalized);
  auto &nodes = _nodes;
  const auto n = nodes.size();
  assert(_ci.size() == n);

  // Children lists.
  std::vector<std::vector<_container_type::size_type>>
    rescan_children(n), arg_roots(n);
  std::vector<_container_type::size_type> roots;
  for (_container_type::size_type i = 0; i < n; ++i) {
    const auto &node = nodes[i];
    if (_is_arg_tree_root(size_type{i})) {
      if (_ci[i].arg_owner != size_type::invalid())
	arg_roots[_ci[i].arg_owner._n].push_back(i);
      else
	roots.push_back(i);
    } else if (node._rescan_parent != size_type::invalid()) {
      rescan_children[node._rescan_parent._n].push_back(i);
    } else {
      roots.push_back(i);
    }
  }
  const auto by_pos =
    [&](const _container_type::size_type a,
	const _container_type::size_type b) {
      if (_ci[a].pos_in_parent != _ci[b].pos_in_parent)
	return _ci[a].pos_in_parent < _ci[b].pos_in_parent;
      return a < b;
    };
  const auto by_arg_index =
    [&](const _container_type::size_type a,
	const _container_type::size_type b) {
      return _ci[a].arg_index < _ci[b].arg_index;
    };

  // DFS pre order, argument trees right after their owner.
  std::vector<_container_type::size_type> order;
  order.reserve(n);
  std::vector<bool> visited(n, false);
  std::function<void(_container_type::size_type)>
    visit = [&](const _container_type::size_type i) {
      if (visited[i])
	return;
      visited[i] = true;
      order.push_back(i);
      auto &ars = arg_roots[i];
      std::sort(ars.begin(), ars.end(), by_arg_index);
      for (const auto a : ars)
	visit(a);
      auto &rcs = rescan_children[i];
      std::sort(rcs.begin(), rcs.end(), by_pos);
      for (const auto c : rcs)
	visit(c);
    };
  for (const auto r : roots)
    visit(r);
  // Anything not reachable (shouldn't happen) is appended in
  // creation order.
  for (_container_type::size_type i = 0; i < n; ++i)
    visit(i);
  assert(order.size() == n);

  // Remap.
  std::vector<_container_type::size_type> new_index(n);
  for (_container_type::size_type i = 0; i < n; ++i)
    new_index[order[i]] = i;
  const auto remap =
    [&](size_type &id) {
      if (id != size_type::invalid())
	id = size_type{new_index[id._n]};
    };
  _container_type reordered;
  reordered.reserve(n);
  for (const auto old : order) {
    reordered.push_back(nodes[old]);
    remap(reordered.back()._rescan_parent);
  }
  nodes = std::move(reordered);
  for (auto &e : _original_to_rewritten) {
    for (auto &id : e.second)
      remap(id);
  }

#ifndef NDEBUG
  // The owner relationship is now implied by position: verify
  // against the scaffolding before dropping it.
  {
    std::vector<_construction_info> ci_reordered;
    ci_reordered.reserve(n);
    for (const auto old : order) {
      ci_reordered.push_back(_ci[old]);
      remap(ci_reordered.back().arg_owner);
    }
    _ci = std::move(ci_reordered);
    _finalized = true;
    for (_container_type::size_type i = 0; i < n; ++i) {
      if (!_is_arg_tree_root(size_type{i}))
	continue;
      assert(_arg_owner(size_type{i}) == _ci[i].arg_owner);
      assert(_ci[i].arg_owner == size_type::invalid() ||
	     _arg_index(size_type{i}) == _ci[i].arg_index);
    }
    // Parents and owners preceed their children.
    for (_container_type::size_type i = 0; i < n; ++i) {
      const auto up = _up(size_type{i});
      assert(up == size_type::invalid() || up._n < i);
    }
  }
#endif

  // Drop the scaffolding.
  _ci.clear();
  _ci.shrink_to_fit();
  _finalized = true;
}

bool rewritten_expansion_forest::
subtree_has_forced_rename(const expansion_forest::rescan_subtree &st)
  const noexcept
{
  for (auto it = st.dfs_pre_begin(); it != st.dfs_pre_end(); ++it) {
    const expansion_forest::rescan_macro_invocation node = *it;
    if (!node.is_arg_tree_root() && is_rename_forced(node.get_macro()))
      return true;
    const std::size_t nargs = node.get_nargs();
    for (std::size_t i = 0; i < nargs; ++i) {
      if (subtree_has_forced_rename(node.get_arg_tree(i)))
	return true;
    }
  }
  return false;
}

bool rewritten_expansion_forest::
subtree_invokes_counter(const expansion_forest::rescan_subtree &st)
  const noexcept
{
  for (auto it = st.all_nodes_begin(); it != st.all_nodes_end(); ++it) {
    const expansion_forest::rescan_macro_invocation node = *it;
    if (node.is_arg_tree_root())
      continue;
    const auto &m = node.get_macro();
    if (m.is_builtin_special() && m.get_name() == "__COUNTER__")
      return true;
  }
  return false;
}

rewritten_expansion_forest::unexpanded_macro_id_out_token&
rewritten_expansion_forest::
get_unexpanded_macro_id_out_token
  (const unexpanded_macro_id_out_tokens::size_type i) noexcept
{
  assert(i < _unexpanded_macro_id_out_tokens.size());
  return _unexpanded_macro_id_out_tokens[i];
}

const rewritten_expansion_forest::counter_invocations::size_type
rewritten_expansion_forest::lookup_original_counter_invocation
(const expansion_forest::nodes_size_type counter_rmi) const noexcept
{
  const auto it_ci =
    std::lower_bound(_counter_invocations.cbegin(),
		     _counter_invocations.cend(),
		     counter_rmi,
		     [](const auto &ci, const auto counter_rmi) {
		       return ci._original_counter_rmi < counter_rmi;
		     });
  assert(it_ci != _counter_invocations.cend() &&
	 it_ci->_original_counter_rmi == counter_rmi);
  return it_ci - _counter_invocations.cbegin();
}


using rescan_subtree = expansion_forest::rescan_subtree;
using rescan_output_segment = expansion_forest::rescan_output_segment;

class rescan_subtree_rewrite_cursor;

class rescan_macro_invocation_edit_script_op
{
public:
  enum class op
  {
    copy,
    insert_token,
    insert_ws,
    replace_token,
    rewrite_macro_id,
  };

  struct copy
  {
    rescan_macro_invocation::output::tokens_range range;
  };

  struct insert_token
  {
    insert_token(const pp_token &_token, const sticky_side _stickiness)
      noexcept;

    ~insert_token() noexcept = default;

    pp_token token;
    sticky_side stickiness;
  };

  struct insert_ws
  {
    sticky_side stickiness;
  };

  struct replace_token
  {
    std::size_t replaced_token_index;
    std::vector<pp_token> replacement;
  };

  struct rewrite_macro_id
  {
    std::size_t rewritten_token_index;
    rewritten_expansion_forest::nodes_size_type
      rewritten_rescan_macro_invocation;
  };

  class result_tokens
  {
  public:
    bool is_insert_ws() const noexcept;
    bool is_rewrite_macro_id() const noexcept;

    const pp_token* begin() const noexcept;
    const pp_token* end() const noexcept;

  private:
    friend class rescan_macro_invocation_edit_script_op;

    struct insert_ws_tag{};

    result_tokens(const insert_ws_tag&) noexcept;

    result_tokens(const rewritten_expansion_forest::nodes_size_type
		    &target_rewritten_rescan_macro_invocation)
      noexcept;

    result_tokens(const pp_token *begin, const pp_token *end) noexcept;

    rewritten_expansion_forest::nodes_size_type
      _target_rewritten_rescan_macro_invocation;

    const pp_token *_begin;
    const pp_token *_end;
  };

  rescan_macro_invocation_edit_script_op(const copy &copy) noexcept;

  rescan_macro_invocation_edit_script_op(insert_token &&insert_token) noexcept;

  rescan_macro_invocation_edit_script_op(const insert_ws &insert_ws) noexcept;

  rescan_macro_invocation_edit_script_op(replace_token &&replace_token)
    noexcept;

  rescan_macro_invocation_edit_script_op
    (const rescan_macro_invocation_edit_script_op &rhs);

  rescan_macro_invocation_edit_script_op
    (rescan_macro_invocation_edit_script_op &&rhs) noexcept;

  ~rescan_macro_invocation_edit_script_op() noexcept;

  rescan_macro_invocation_edit_script_op&
  operator=(const rescan_macro_invocation_edit_script_op &s);

  rescan_macro_invocation_edit_script_op&
  operator=(rescan_macro_invocation_edit_script_op &&s) noexcept;

  const std::size_t tokens_length() const noexcept;

  const op get_op() const noexcept
  { return _op; }

  result_tokens get_result_tokens(const rescan_macro_invocation &rewritten_rmi)
    const noexcept;

private:
  friend class rescan_subtree_rewrite_cursor;
  friend class rescan_macro_invocation_rewriter;
  friend class rewritten_expansion_forest;
  friend bool klp::ccp::rewrite_macro_invocation
    (const pp_result&, const pp_result::macro_invocation&,
     const std::vector<macro_invocation_output_edit_script_op>&, const bool,
     const std::map<std::string, bool>&,
     const std::set<const pp_result::macro*>&,
     rewritten_macro_definitions_pool&,
     rewritten_macro_invocation&, std::string&);

  rescan_macro_invocation_edit_script_op
    (const rewrite_macro_id &rewrite_macro_id) noexcept;

public:
  op _op;

  union {
    copy _copy;
    insert_token _insert_token;
    insert_ws _insert_ws;
    replace_token _replace_token;
    rewrite_macro_id _rewrite_macro_id;
  };
};

rescan_macro_invocation_edit_script_op::insert_token::
insert_token(const pp_token &_token, const sticky_side _stickiness) noexcept
  : token{_token}, stickiness{_stickiness}
{}

rescan_macro_invocation_edit_script_op::result_tokens::
result_tokens(const insert_ws_tag&) noexcept
  : _target_rewritten_rescan_macro_invocation{
      rewritten_expansion_forest::nodes_size_type::invalid()
    },
    _begin{nullptr}, _end{nullptr}
{}

rescan_macro_invocation_edit_script_op::result_tokens::
result_tokens(const rewritten_expansion_forest::nodes_size_type
		&target_rewritten_rescan_macro_invocation)
  noexcept
  : _target_rewritten_rescan_macro_invocation{
      target_rewritten_rescan_macro_invocation
    },
    _begin{nullptr}, _end{nullptr}
{}

rescan_macro_invocation_edit_script_op::result_tokens::
result_tokens(const pp_token *begin, const pp_token *end) noexcept
  : _target_rewritten_rescan_macro_invocation{
      rewritten_expansion_forest::nodes_size_type::invalid()
    },
    _begin{begin}, _end{end}
{}

bool
rescan_macro_invocation_edit_script_op::result_tokens::is_insert_ws()
  const noexcept
{
  return _begin == nullptr && !is_rewrite_macro_id();
}

bool rescan_macro_invocation_edit_script_op::result_tokens::
is_rewrite_macro_id() const noexcept
{
  return (_target_rewritten_rescan_macro_invocation !=
	  rewritten_expansion_forest::nodes_size_type::invalid());
}

const pp_token* rescan_macro_invocation_edit_script_op::result_tokens::
begin() const noexcept
{
  return _begin;
}

const pp_token* rescan_macro_invocation_edit_script_op::result_tokens::
end() const noexcept
{
  return _end;
}

rescan_macro_invocation_edit_script_op::result_tokens
rescan_macro_invocation_edit_script_op::
get_result_tokens(const rescan_macro_invocation &rewritten_rmi) const noexcept
{
  switch (_op) {
  case op::copy:
    {
      const auto &rmi_out_tokens =
	rewritten_rmi.get_output().get_result();
      return result_tokens{
	&*(rmi_out_tokens.begin() + _copy.range.begin),
	&*(rmi_out_tokens.begin() + _copy.range.end),
      };
    }
  case op::insert_token:
    return result_tokens{
      &_insert_token.token,
      &_insert_token.token + 1
    };
  case op::insert_ws:
    return result_tokens{result_tokens::insert_ws_tag{}};
  case op::replace_token:
    return result_tokens{
      &*_replace_token.replacement.begin(),
      &*_replace_token.replacement.end(),
    };
  case op::rewrite_macro_id:
    return result_tokens{
      _rewrite_macro_id.rewritten_rescan_macro_invocation
    };
  }
}

rescan_macro_invocation_edit_script_op::
rescan_macro_invocation_edit_script_op
  (const copy &copy) noexcept
  : _op{op::copy}, _copy{copy}
{}

rescan_macro_invocation_edit_script_op::
rescan_macro_invocation_edit_script_op(insert_token &&insert_token) noexcept
  : _op{op::insert_token}, _insert_token{std::move(insert_token)}
{}

rescan_macro_invocation_edit_script_op::
rescan_macro_invocation_edit_script_op(const insert_ws &insert_ws) noexcept
  : _op{op::insert_ws}, _insert_ws{insert_ws}
{}

rescan_macro_invocation_edit_script_op::
rescan_macro_invocation_edit_script_op(replace_token &&replace_token)
  noexcept
  : _op{op::replace_token}, _replace_token{std::move(replace_token)}
{}

rescan_macro_invocation_edit_script_op::
rescan_macro_invocation_edit_script_op(const rewrite_macro_id &rewrite_macro_id)
  noexcept
  : _op{op::rewrite_macro_id}, _rewrite_macro_id{rewrite_macro_id}
{}

rescan_macro_invocation_edit_script_op::
rescan_macro_invocation_edit_script_op
  (const rescan_macro_invocation_edit_script_op &rhs)
  : _op{rhs._op}
{
  switch (rhs._op) {
  case op::copy:
    new (&_copy) copy{rhs._copy};
    break;

  case op::insert_token:
    new (&_insert_token) insert_token{rhs._insert_token};
    break;

  case op::insert_ws:
    new (&_insert_ws) insert_ws{rhs._insert_ws};
    break;

  case op::replace_token:
    new (&_replace_token) replace_token{rhs._replace_token};
    break;

  case op::rewrite_macro_id:
    new (&_rewrite_macro_id) rewrite_macro_id{rhs._rewrite_macro_id};
    break;

  }
}

rescan_macro_invocation_edit_script_op::
rescan_macro_invocation_edit_script_op
  (rescan_macro_invocation_edit_script_op &&rhs) noexcept
  : _op{rhs._op}
{
  switch (rhs._op) {
  case op::copy:
    new (&_copy) copy{std::move(rhs._copy)};
    break;

  case op::insert_token:
    new (&_insert_token) insert_token{std::move(rhs._insert_token)};
    break;

  case op::insert_ws:
    new (&_insert_ws) insert_ws{std::move(rhs._insert_ws)};
    break;

  case op::replace_token:
    new (&_replace_token)
      replace_token{std::move(rhs._replace_token)};
    break;

  case op::rewrite_macro_id:
    new (&_rewrite_macro_id)
      rewrite_macro_id{std::move(rhs._rewrite_macro_id)};
    break;

  }
}

rescan_macro_invocation_edit_script_op::
~rescan_macro_invocation_edit_script_op() noexcept
{
  switch (_op) {
  case op::copy:
    _copy.~copy();
    break;

  case op::insert_token:
    _insert_token.~insert_token();
    break;

  case op::insert_ws:
    _insert_ws.~insert_ws();
    break;

  case op::replace_token:
    _replace_token.~replace_token();
    break;

  case op::rewrite_macro_id:
    _rewrite_macro_id.~rewrite_macro_id();
    break;

  }
}

rescan_macro_invocation_edit_script_op& rescan_macro_invocation_edit_script_op::
operator=(const rescan_macro_invocation_edit_script_op &rhs)
{
  switch (_op) {
  case op::copy:
    _copy.~copy();
    break;

  case op::insert_token:
    _insert_token.~insert_token();
    break;

  case op::insert_ws:
    _insert_ws.~insert_ws();
    break;

  case op::replace_token:
    _replace_token.~replace_token();
    break;

  case op::rewrite_macro_id:
    _rewrite_macro_id.~rewrite_macro_id();
    break;

  }

  // Make the destructor a nop in case the copy below throws.
  static_assert
    (std::is_trivially_destructible<copy>::value,
     "intermediate variant type not trivially destructible");
  _op = op::copy;

  switch (rhs._op) {
  case op::copy:
    new (&_copy) copy{rhs._copy};
    break;

  case op::insert_token:
    new (&_insert_token) insert_token{rhs._insert_token};
    break;

  case op::insert_ws:
    new (&_insert_ws) insert_ws{rhs._insert_ws};
    break;

  case op::replace_token:
    new (&_replace_token) replace_token{rhs._replace_token};
    break;

  case op::rewrite_macro_id:
    new (&_rewrite_macro_id) rewrite_macro_id{rhs._rewrite_macro_id};
    break;

  }

  _op = rhs._op;

  return *this;
}

rescan_macro_invocation_edit_script_op& rescan_macro_invocation_edit_script_op::
operator=(rescan_macro_invocation_edit_script_op &&rhs) noexcept
{
  switch (_op) {
  case op::copy:
    _copy.~copy();
    break;

  case op::insert_token:
    _insert_token.~insert_token();
    break;

  case op::insert_ws:
    _insert_ws.~insert_ws();
    break;

  case op::replace_token:
    _replace_token.~replace_token();
    break;

  case op::rewrite_macro_id:
    _rewrite_macro_id.~rewrite_macro_id();
    break;

  }


  _op = rhs._op;

  switch (rhs._op) {
  case op::copy:
    new (&_copy) copy{std::move(rhs._copy)};
    break;

  case op::insert_token:
    new (&_insert_token) insert_token{std::move(rhs._insert_token)};
    break;

  case op::insert_ws:
    new (&_insert_ws) insert_ws{std::move(rhs._insert_ws)};
    break;

  case op::replace_token:
    new (&_replace_token)
      replace_token{std::move(rhs._replace_token)};
    break;

  case op::rewrite_macro_id:
    new (&_rewrite_macro_id)
      rewrite_macro_id{std::move(rhs._rewrite_macro_id)};
    break;

  }

  return *this;
}

const std::size_t rescan_macro_invocation_edit_script_op::
tokens_length() const noexcept
{
  switch (_op) {
  case op::copy:
    return _copy.range.length();

  case op::insert_token:
    return 1;

  case op::insert_ws:
    return 1;

  case op::replace_token:
    return _replace_token.replacement.size();

  case op::rewrite_macro_id:
    return 1;

  }
}


class rescan_macro_invocation_edit_script
{
private:
  typedef std::vector<rescan_macro_invocation_edit_script_op> _container_type;

public:
  typedef _container_type::iterator ops_iterator;
  typedef _container_type::const_iterator const_ops_iterator;
  typedef _container_type::size_type size_type;

  template<typename ...ops_init_args_types>
  rescan_macro_invocation_edit_script(ops_init_args_types&&... ops_init_args);

  ops_iterator begin() noexcept
  { return _ops.begin(); }

  ops_iterator end() noexcept
  { return _ops.end(); }

  const_ops_iterator begin() const noexcept
  { return _ops.cbegin(); }

  const_ops_iterator end() const noexcept
  { return _ops.cend(); }

  const_ops_iterator cbegin() const noexcept
  { return _ops.cbegin(); }

  const_ops_iterator cend() const noexcept
  { return _ops.cend(); }

  _container_type::reverse_iterator rbegin() noexcept
  { return _ops.rbegin(); }

  _container_type::reverse_iterator rend() noexcept
  { return _ops.rend(); }

  _container_type::const_reverse_iterator crend() const noexcept
  { return _ops.crend(); }

  bool empty() const noexcept
  { return _ops.empty(); }

  size_type size() const noexcept
  { return _ops.size(); }

  const rescan_macro_invocation_edit_script_op&
  back() const noexcept;

  rescan_macro_invocation_edit_script_op&
  back() noexcept;

  void reserve(const size_type capacity)
  { _ops.reserve(capacity); }

  template<typename ...args_types>
  void emplace_back(args_types&&... args);

  template<typename ...args_types>
  void push_back(args_types&&... args);

  template<typename ...args_types>
  decltype
    (std::declval<_container_type>().insert(std::declval<args_types>()...))
  insert(args_types&&... args);

  template<typename ...args_types>
  decltype
    (std::declval<_container_type>().erase(std::declval<args_types>()...))
  erase(args_types&&... args);

  const std::size_t tokens_length() const noexcept;

private:
  _container_type _ops;
};

template<typename ...ops_init_args_types>
rescan_macro_invocation_edit_script::
rescan_macro_invocation_edit_script(ops_init_args_types&&... ops_init_args)
  : _ops{std::forward<ops_init_args_types>(ops_init_args)...}
{}

const rescan_macro_invocation_edit_script_op&
rescan_macro_invocation_edit_script::back() const noexcept
{
  assert(!_ops.empty());
  return _ops.back();
}

rescan_macro_invocation_edit_script_op&
rescan_macro_invocation_edit_script::back() noexcept
{
  assert(!_ops.empty());
  return _ops.back();
}

template<typename ...args_types>
void rescan_macro_invocation_edit_script::emplace_back(args_types&&... args)
{
  _ops.emplace_back(std::forward<args_types>(args)...);
}

template<typename ...args_types>
void rescan_macro_invocation_edit_script::push_back(args_types&&... args)
{
  _ops.push_back(std::forward<args_types>(args)...);
}

template<typename ...args_types>
decltype
  (std::declval<rescan_macro_invocation_edit_script::_container_type>()
   .insert(std::declval<args_types>()...))
rescan_macro_invocation_edit_script::insert(args_types&&... args)
{
  return _ops.insert(std::forward<args_types>(args)...);
}

template<typename ...args_types>
decltype
  (std::declval<rescan_macro_invocation_edit_script::_container_type>()
   .erase(std::declval<args_types>()...))
rescan_macro_invocation_edit_script::erase(args_types&&... args)
{
  return _ops.erase(std::forward<args_types>(args)...);
}

const std::size_t rescan_macro_invocation_edit_script::tokens_length()
  const noexcept
{
  return std::accumulate(_ops.cbegin(), _ops.cend(),
			 static_cast<std::size_t>(0),
			 [](const std::size_t acc, const auto &op) {
			   return acc + op.tokens_length();
			 });
}


class rescan_macro_invocation_edit_script_rescanned_ranges
{
public:
  using mi_edit_script_op_index =
    rescan_macro_invocation_edit_script::size_type;

  struct mi_edit_script_rescanned_range
  {
    mi_edit_script_op_index begin;
    mi_edit_script_op_index end;
  };

  rescan_macro_invocation_edit_script_rescanned_ranges() = default;

  // Ranges covering all of the first n_ops ops.
  static rescan_macro_invocation_edit_script_rescanned_ranges
  covering_all(const mi_edit_script_op_index n_ops);

  bool is_covered(const mi_edit_script_op_index index) const noexcept;

  bool empty() const noexcept
  { return _rescanned_ranges.empty(); }

  void append(const mi_edit_script_rescanned_range &range);

  rescan_macro_invocation_edit_script_rescanned_ranges
  split_off_head(const mi_edit_script_op_index head_edit_script_end);

  void join_with(rescan_macro_invocation_edit_script_rescanned_ranges &&tail,
		 const mi_edit_script_op_index head_edit_script_end);

  void erase_region(const mi_edit_script_op_index erased_edit_script_begin,
		    const mi_edit_script_op_index erased_edit_script_end);

private:
  using _mi_edit_script_rescanned_ranges_type =
    std::vector<mi_edit_script_rescanned_range>;

  rescan_macro_invocation_edit_script_rescanned_ranges
    (_mi_edit_script_rescanned_ranges_type &&rescanned_ranges) noexcept;

  _mi_edit_script_rescanned_ranges_type _rescanned_ranges;
};

rescan_macro_invocation_edit_script_rescanned_ranges::
rescan_macro_invocation_edit_script_rescanned_ranges
  (_mi_edit_script_rescanned_ranges_type &&rescanned_ranges) noexcept
  : _rescanned_ranges{std::move(rescanned_ranges)}
{}

rescan_macro_invocation_edit_script_rescanned_ranges
rescan_macro_invocation_edit_script_rescanned_ranges::
covering_all(const mi_edit_script_op_index n_ops)
{
  rescan_macro_invocation_edit_script_rescanned_ranges r;
  if (n_ops)
    r.append(mi_edit_script_rescanned_range{.begin = 0, .end = n_ops});
  return r;
}

bool rescan_macro_invocation_edit_script_rescanned_ranges::
is_covered(const mi_edit_script_op_index index) const noexcept
{
  if (_rescanned_ranges.empty())
    return false;

  // Avoid a lookup in some common cases if possible, e.g. index == 0
  // when invoked in the course of splitting up a copy op at the
  // (remaining) edit script head.
  if (index == 0 || index < _rescanned_ranges.front().end) {
    return index >= _rescanned_ranges.front().begin;
  } else if (index >= _rescanned_ranges.back().begin) {
    return index < _rescanned_ranges.back().end;
  } else {
    // Slow path.
    const auto it_range =
      std::lower_bound(_rescanned_ranges.begin(), _rescanned_ranges.end(),
		       index,
		       [&](const mi_edit_script_rescanned_range &r,
			   const mi_edit_script_op_index index) {
			 return r.end <= index;
		       });
    return (it_range != _rescanned_ranges.end() &&
	    it_range->begin <= index);
  }
}

void rescan_macro_invocation_edit_script_rescanned_ranges::
append(const mi_edit_script_rescanned_range &range)
{
  if (range.begin == range.end)
    return;
  assert(_rescanned_ranges.empty() ||
	 _rescanned_ranges.back().end <= range.begin);
  if (_rescanned_ranges.empty() ||
      _rescanned_ranges.back().end < range.begin) {
    _rescanned_ranges.push_back(range);
  } else {
    // Merge with the last range.
    _rescanned_ranges.back().end = range.end;
  }
}

rescan_macro_invocation_edit_script_rescanned_ranges
rescan_macro_invocation_edit_script_rescanned_ranges::
split_off_head(const mi_edit_script_op_index head_edit_script_end)
{
  const auto head_ranges_last
    = std::lower_bound(_rescanned_ranges.begin(), _rescanned_ranges.end(),
		       head_edit_script_end,
		       [&](const mi_edit_script_rescanned_range &r,
			   const mi_edit_script_op_index head_edit_script_end) {
			 return r.end <= head_edit_script_end;
		       });
  if (head_ranges_last == _rescanned_ranges.end()) {
    return rescan_macro_invocation_edit_script_rescanned_ranges{
      std::move(_rescanned_ranges)
    };
  }

  _mi_edit_script_rescanned_ranges_type head_ranges;
  head_ranges.reserve((head_ranges_last - _rescanned_ranges.cbegin()) +
		      (head_ranges_last->begin < head_edit_script_end));
  head_ranges.insert(head_ranges.cend(),
		     std::make_move_iterator(_rescanned_ranges.begin()),
		     std::make_move_iterator(head_ranges_last));
  if (head_ranges_last->begin < head_edit_script_end) {
    // head_edit_script_end is located in the interior of
    // head_ranges_last. Split the range up.
    head_ranges.push_back(*head_ranges_last);
    head_ranges.back().end = head_edit_script_end;
    head_ranges_last->begin = head_edit_script_end;
  }

  _rescanned_ranges.erase(_rescanned_ranges.begin(), head_ranges_last);

  // Update the remaining ranges' offsets to be relative to the
  // splitting point.
  for (auto &r : _rescanned_ranges) {
    assert(r.end > r.begin);
    assert(r.begin >= head_edit_script_end);
    r.begin -= head_edit_script_end;
    r.end -= head_edit_script_end;
  }

  return rescan_macro_invocation_edit_script_rescanned_ranges{
    std::move(head_ranges)
  };
}

void rescan_macro_invocation_edit_script_rescanned_ranges::
join_with(rescan_macro_invocation_edit_script_rescanned_ranges &&tail,
	  const mi_edit_script_op_index head_edit_script_end)
{
  if (tail._rescanned_ranges.empty())
    return;

  assert(std::all_of(_rescanned_ranges.cbegin(),
		     _rescanned_ranges.cend(),
		     [&](const mi_edit_script_rescanned_range &r) {
		       return (r.begin < r.end &&
			       r.end <= head_edit_script_end);
		     }));

  // Update the tail ranges' offsets to be relative to the
  // head range's beginning.
  for (auto &r : tail._rescanned_ranges) {
    r.begin += head_edit_script_end;
    r.end += head_edit_script_end;
  }

  // If the head's last and the tail's first range
  // meet at their boundaries, merge them.
  auto tail_begin = tail._rescanned_ranges.begin();
  if (!_rescanned_ranges.empty() &&
      _rescanned_ranges.back().end == tail_begin->begin) {
    _rescanned_ranges.back().end = tail_begin->end;
    ++tail_begin;
  }

  _rescanned_ranges.reserve(_rescanned_ranges.size() +
			    (tail._rescanned_ranges.end() - tail_begin));
  _rescanned_ranges.insert
    (_rescanned_ranges.end(),
     std::make_move_iterator(tail_begin),
     std::make_move_iterator(tail._rescanned_ranges.end()));
}

void rescan_macro_invocation_edit_script_rescanned_ranges::
erase_region(const mi_edit_script_op_index erased_edit_script_begin,
	     const mi_edit_script_op_index erased_edit_script_end)
{
  if (erased_edit_script_begin == erased_edit_script_end)
    return;

  auto erased_ranges_begin
    = (std::lower_bound
       (_rescanned_ranges.begin(), _rescanned_ranges.end(),
	erased_edit_script_begin,
	[&](const mi_edit_script_rescanned_range &r,
	    const mi_edit_script_op_index erased_edit_script_begin) {
	  return r.end <= erased_edit_script_begin;
	}));
  if (erased_ranges_begin == _rescanned_ranges.end())
    return;

  // Begin of tail ranges that will have to get their offsets adjusted
  // in order to account for the removed region.
  _mi_edit_script_rescanned_ranges_type::iterator tail_ranges_begin;
  if (erased_edit_script_end <= erased_ranges_begin->begin) {
    // The to be erased region does not overlap with any range entry.
    tail_ranges_begin = erased_ranges_begin;
  } else if (erased_edit_script_begin > erased_ranges_begin->begin &&
	     erased_edit_script_end < erased_ranges_begin->end) {
    // The to be erased region is contained properly within a
    // single range entry. Split that up.
    const mi_edit_script_op_index range_end = erased_ranges_begin->end;
    erased_ranges_begin->end = erased_edit_script_begin;
    tail_ranges_begin =
      _rescanned_ranges.insert(erased_ranges_begin + 1,
			       mi_edit_script_rescanned_range{
				 .begin = erased_edit_script_end,
				 .end = range_end
			       });
  } else {
    _mi_edit_script_rescanned_ranges_type::iterator erased_ranges_end;
    // Shortcut to avoid the upper_bound() search below if possible.
    if (erased_edit_script_end <= erased_ranges_begin->end) {
      // The invariants established by the upper_bound() below
      // holds in this case here as well ...
      // ... because the initial if-condition above would have
      // evaluated to true otherwise and ...
      assert(erased_edit_script_end > erased_ranges_begin->begin);
      erased_ranges_end = erased_ranges_begin + 1;
      // ... because the _rescanned_ranges are ordered.
      assert(erased_ranges_end == _rescanned_ranges.end() ||
	     erased_ranges_begin->end <= erased_ranges_end->begin);
      assert(erased_ranges_end == _rescanned_ranges.end() ||
	     erased_edit_script_end <= erased_ranges_end->begin);
    } else {
      erased_ranges_end
	= (std::upper_bound
	   (erased_ranges_begin + 1, _rescanned_ranges.end(),
	    erased_edit_script_end,
	    [&](const mi_edit_script_op_index erased_edit_script_end,
		const mi_edit_script_rescanned_range &r) {
	      return erased_edit_script_end <= r.begin;
	    }));
    }
    assert(erased_ranges_end != erased_ranges_begin);
    if (erased_ranges_begin->begin < erased_edit_script_begin) {
      // Holds as per the lower_bound() search.
      assert(erased_ranges_begin->end > erased_edit_script_begin);
      erased_ranges_begin->end = erased_edit_script_begin;
      ++erased_ranges_begin;
    }
    if (erased_edit_script_end < (erased_ranges_end - 1)->end) {
      // The case where the erased edit script region is contained
      // properly in a single range entry would have been handled
      // above.
      assert(erased_ranges_begin != erased_ranges_end);
      --erased_ranges_end;
      // Holds as per the upper_bound() search.
      assert(erased_edit_script_end > erased_ranges_end->begin);
      erased_ranges_end->begin = erased_edit_script_end;
    }

    tail_ranges_begin = _rescanned_ranges.erase(erased_ranges_begin,
						erased_ranges_end);
  }

  const auto offset = erased_edit_script_end - erased_edit_script_begin;
  for (auto it = tail_ranges_begin; it != _rescanned_ranges.end(); ++it) {
    assert (it->begin >= erased_edit_script_end);
    it->begin -= offset;
    assert (it->end >= erased_edit_script_end);
    it->end -= offset;
  }
}

struct unexpanded_macro_id_out_token_origin_trace
{
  // Relative to the to be rewritten output segment's beginning.
  std::size_t traced_out_token_index;
  rewritten_expansion_forest::unexpanded_macro_id_out_tokens::size_type
    unexpanded_macro_id_out_token;
};

class unexpanded_macro_id_out_tokens_origin_traces
{
private:
  typedef std::vector<unexpanded_macro_id_out_token_origin_trace>
    _container_type;

public:
  unexpanded_macro_id_out_tokens_origin_traces() noexcept;

  unexpanded_macro_id_out_tokens_origin_traces
    (unexpanded_macro_id_out_tokens_origin_traces&&) noexcept;

  unexpanded_macro_id_out_tokens_origin_traces&
  operator=(unexpanded_macro_id_out_tokens_origin_traces&&) noexcept;

  bool empty() const noexcept
  { return _traces.empty(); }

  using const_iterator = _container_type::const_iterator;

  const_iterator begin() const noexcept
  { return _traces.begin(); }

  const_iterator end() const noexcept
  { return _traces.end(); }

  void append(unexpanded_macro_id_out_token_origin_trace &&t);

  unexpanded_macro_id_out_tokens_origin_traces
  split_off_head(const std::size_t head_tokens_end);

  void merge_with(unexpanded_macro_id_out_tokens_origin_traces &&other);

  void join_with(unexpanded_macro_id_out_tokens_origin_traces &&tail,
		 const std::size_t head_tokens_end);

  void erase_region(const std::size_t erased_tokens_begin,
		    const std::size_t erased_tokens_end);

private:
  unexpanded_macro_id_out_tokens_origin_traces(_container_type &&traces)
    noexcept;

  _container_type _traces;
}; // class class unexpanded_macro_id_out_tokens_origin_traces

unexpanded_macro_id_out_tokens_origin_traces::
unexpanded_macro_id_out_tokens_origin_traces() noexcept = default;

unexpanded_macro_id_out_tokens_origin_traces::
unexpanded_macro_id_out_tokens_origin_traces
(unexpanded_macro_id_out_tokens_origin_traces&&) noexcept = default;

unexpanded_macro_id_out_tokens_origin_traces::
unexpanded_macro_id_out_tokens_origin_traces(_container_type &&traces)
  noexcept
  : _traces{std::move(traces)}
{}

unexpanded_macro_id_out_tokens_origin_traces&
unexpanded_macro_id_out_tokens_origin_traces::
operator=(unexpanded_macro_id_out_tokens_origin_traces&&) noexcept = default;

void unexpanded_macro_id_out_tokens_origin_traces::
append(unexpanded_macro_id_out_token_origin_trace &&t)
{
  assert(_traces.empty() ||
	 _traces.back().traced_out_token_index <= t.traced_out_token_index);
  _traces.push_back(std::move(t));
}

unexpanded_macro_id_out_tokens_origin_traces
unexpanded_macro_id_out_tokens_origin_traces::
split_off_head(const std::size_t head_tokens_end)
{
  const auto pivot =
    std::lower_bound(_traces.begin(), _traces.end(),
		     head_tokens_end,
		     [](const auto &t, const auto traced_out_token_index) {
		       return (t.traced_out_token_index <
			       traced_out_token_index);
		     });

  _container_type head_traces;
  head_traces.reserve(pivot - _traces.cbegin());
  head_traces.insert(head_traces.end(),
		     std::make_move_iterator(_traces.begin()),
		     std::make_move_iterator(pivot));
  _traces.erase(_traces.begin(), pivot);
  for (auto &t : _traces)
    t.traced_out_token_index -= head_tokens_end;

  return unexpanded_macro_id_out_tokens_origin_traces{std::move(head_traces)};
}

void unexpanded_macro_id_out_tokens_origin_traces::
merge_with(unexpanded_macro_id_out_tokens_origin_traces &&other)
{
  if (other._traces.empty()) {
    return;
  } else if (_traces.empty()) {
    _traces = std::move(other._traces);
    return;
  }

  _container_type traces;
  traces.reserve(_traces.size() + other._traces.size());
  std::merge(std::make_move_iterator(_traces.begin()),
	     std::make_move_iterator(_traces.end()),
	     std::make_move_iterator(other._traces.begin()),
	     std::make_move_iterator(other._traces.end()),
	     std::back_inserter(traces),
	     [](const auto &t0, const auto &t1) {
	       return t0.traced_out_token_index < t1.traced_out_token_index;
	     });
  _traces = std::move(traces);
}

void unexpanded_macro_id_out_tokens_origin_traces::
join_with(unexpanded_macro_id_out_tokens_origin_traces &&tail,
	  const std::size_t head_tokens_end)
{
  for (auto &t : tail._traces) {
    t.traced_out_token_index += head_tokens_end;
  }

  if (_traces.empty()) {
    _traces = std::move(tail._traces);
    return;
  }

  _traces.reserve(_traces.size() + tail._traces.size());
  _traces.insert(_traces.end(),
		 std::make_move_iterator(tail._traces.begin()),
		 std::make_move_iterator(tail._traces.end()));
}

void unexpanded_macro_id_out_tokens_origin_traces::
erase_region(const std::size_t erased_tokens_begin,
	     const std::size_t erased_tokens_end)
{
  const auto b =
    std::lower_bound(_traces.cbegin(), _traces.cend(),
		     erased_tokens_begin,
		     [](const auto &t, const auto traced_out_token_index) {
		       return (t.traced_out_token_index <
			       traced_out_token_index);
		     });
  if (b == _traces.cend())
    return;

  const auto e =
    std::lower_bound(b, _traces.cend(),
		     erased_tokens_end,
		     [](const auto &t, const auto traced_out_token_index) {
		       return (t.traced_out_token_index <
			       traced_out_token_index);
		     });
  auto it_t = _traces.erase(b, e);
  while (it_t != _traces.end()) {
    it_t->traced_out_token_index -= erased_tokens_end - erased_tokens_begin;
    ++it_t;
  }
}


class rescan_macro_invocation_rewriter
{
public:
  struct rewritten_rescan_macro_invocation_child
  {
    // Position of the child invocation relative to the rewritten
    // output segement's beginning.
    std::size_t out_token_index;
    rewritten_expansion_forest::nodes_size_type child_rrmi;
  };

  struct descendant_rescan_counter_invocation
  {
    // Position of the child macro invocation being a (direct or
    // indirect) ancestor of the associated __COUNTER__
    // invocation. Relative to the to be rewritten output segment's
    // beginning.
    std::size_t out_token_index;
    rewritten_expansion_forest::counter_invocations::size_type invocation;
  };

  class rewrite_progress
  {
  public:
    struct result
    {
      rescan_macro_invocation_edit_script src_edit_script;
      rescan_macro_invocation_edit_script_rescanned_ranges src_rescanned_ranges;
      unexpanded_macro_id_out_tokens_origin_traces
	unexpanded_macro_id_out_tokens_origin_traces;
      std::vector<rewritten_expansion_forest::nodes_size_type>
	rewritten_rescan_macro_invocation_children;
      std::vector<rewritten_expansion_forest::counter_invocations::size_type>
	rescan_subtree_counter_invocations;
    };

    rewrite_progress(std::unique_ptr<rescan_subtree_rewrite_cursor> &&cursor)
      noexcept;

    rewrite_progress(result &&r) noexcept;

    rewrite_progress(rewrite_progress &&rhs) noexcept;

    ~rewrite_progress() noexcept;

    std::unique_ptr<rescan_subtree_rewrite_cursor>
    grab_pending_arg_tree_rewrite() noexcept;

    result grab_result() noexcept;

  private:
    std::unique_ptr<rescan_subtree_rewrite_cursor> _cursor;
    result _result;
  }; // class rewrite_progress

public:
  rescan_macro_invocation_rewriter
    (rewritten_expansion_forest &rewritten_forest,
     rewritten_expansion_forest::rewritten_rescan_macro_invocation &&rrmi,
     rescan_macro_invocation_edit_script &&out_edit_script,
     rescan_macro_invocation_edit_script_rescanned_ranges out_rescanned_ranges,
     unexpanded_macro_id_out_tokens_origin_traces
       &&unexpanded_macro_id_out_tokens_origin_traces,
     std::vector<rewritten_rescan_macro_invocation_child>
       &&rewritten_rescan_macro_invocation_children,
     std::vector<descendant_rescan_counter_invocation>
       &&descendant_rescan_counter_invocations);

  ~rescan_macro_invocation_rewriter() noexcept;

  rewrite_progress continue_rewrite();

  rewritten_expansion_forest::rewritten_rescan_macro_invocation&
  get_rewritten_rescan_macro_invocation() noexcept;

private:
  struct _impl;

  std::unique_ptr<_impl> _i;
};

class rescan_subtree_rewrite_cursor
{
public:
  struct result
  {
    rescan_macro_invocation_edit_script src_edit_script;
    rescan_macro_invocation_edit_script_rescanned_ranges rescanned_src_ranges;
    unexpanded_macro_id_out_tokens_origin_traces
      unexpanded_macro_id_out_tokens_origin_traces;
    // The rewritten nodes of the subtree's top level invocations,
    // i.e. the rescan children of the (argument tree) root.
    std::vector<rewritten_expansion_forest::nodes_size_type>
      top_level_rewritten_rescan_macro_invocations;
    std::vector<rewritten_expansion_forest::counter_invocations::size_type>
       rescan_subtree_counter_invocations;
  };

  rescan_subtree_rewrite_cursor(const rescan_subtree_rewrite_cursor&) = delete;

  // create_traces: whether to collect new unexpanded macro id token
  // traces from the edit script's result. This is to be done only
  // for the top-level cursor processing the final output, cursors
  // for argument trees receive their traces from the outside.
  rescan_subtree_rewrite_cursor
    (rewritten_expansion_forest &rewritten_forest,
     rescan_subtree &&subtree,
     rescan_macro_invocation_edit_script &&edit_script,
     rescan_macro_invocation_edit_script_rescanned_ranges &&rescanned_ranges,
     unexpanded_macro_id_out_tokens_origin_traces
       &&followed_unexpanded_macro_id_out_tokens_origin_traces,
     result &result_dst,
     const std::map<std::string, bool> &macros_ids,
     const bool next_tok_is_lparen,
     const bool create_traces);

  std::unique_ptr<rescan_subtree_rewrite_cursor> continue_rewrite();

private:
  struct _rewrite_segment;

  using _rewrite_segments_type = std::list<_rewrite_segment>;

  struct _rewrite_segment
  {
    using rewritten_rescan_macro_invocation_child =
      rescan_macro_invocation_rewriter::rewritten_rescan_macro_invocation_child;

    using descendant_rescan_counter_invocation =
      rescan_macro_invocation_rewriter::descendant_rescan_counter_invocation;

    struct _rewrite_queue_node
    {
      _rewrite_queue_node() noexcept;

      _rewrite_segments_type::iterator next;
      _rewrite_segments_type::iterator *prev;
    };

    _rewrite_queue_node _queue_node;

    std::size_t subtree_depth;
    rescan_macro_invocation mi;
    rescan_macro_invocation_edit_script out_edit_script;
    rescan_macro_invocation_edit_script_rescanned_ranges rescanned_out_ranges;
    unexpanded_macro_id_out_tokens_origin_traces
      unexpanded_macro_id_out_tokens_origin_traces;
    std::vector<rewritten_rescan_macro_invocation_child>
      rewritten_rescan_macro_invocation_children;
    std::vector<descendant_rescan_counter_invocation>
      descendant_rescan_counter_invocations;
  };

  void _join_segment_into(const _rewrite_segments_type::iterator &it_dst,
			  const _rewrite_segments_type::iterator &it_src);

  _rewrite_segments_type::iterator
  _merge_segments_with_same_mi(const _rewrite_segments_type::iterator &it_head);

  void _propagate_rescan_macro_invocation_src_rewrite
    (const _rewrite_segments_type::iterator &it_rewrite_seg,
     const rewritten_expansion_forest::rewritten_rescan_macro_invocation& rrmi,
     rescan_macro_invocation_edit_script &&rmi_src_edit_script,
     rescan_macro_invocation_edit_script_rescanned_ranges
       &&rescanned_rmi_src_ranges,
     unexpanded_macro_id_out_tokens_origin_traces
       &&followed_unexpanded_macro_id_out_tokens_origin_traces,
     std::vector<rewritten_expansion_forest::counter_invocations::size_type>
       &&rescan_subtree_counter_invocations);

  void
  _link_rewrite_queue_node_after
    (_rewrite_segments_type::iterator &prev_next,
     const _rewrite_segments_type::iterator &node)
    noexcept;

  void _unlink_rewrite_queue_node(_rewrite_segment::_rewrite_queue_node &node)
    noexcept;

  void _prepare_rewrite_queue() noexcept;

  template<typename erase_ops_cb_type>
  static void
  _normalize_mi_edit_script(rescan_macro_invocation_edit_script &edit_script,
			    erase_ops_cb_type &&erase_ops_cb);

  std::reference_wrapper<rewritten_expansion_forest> _rewritten_forest;
  std::reference_wrapper<result> _result_dst;

  _rewrite_segments_type _rewrite_segments;
  _rewrite_segments_type::iterator _rewrite_queue_head;

  std::unique_ptr<rescan_macro_invocation_rewriter> _pending_segment_rewrite;
};

rescan_subtree_rewrite_cursor::_rewrite_segment::_rewrite_queue_node::
_rewrite_queue_node() noexcept
  : prev{nullptr}, next{}
{}

rescan_subtree_rewrite_cursor::rescan_subtree_rewrite_cursor
  (rewritten_expansion_forest &rewritten_forest,
   rescan_subtree &&subtree,
   rescan_macro_invocation_edit_script &&edit_script,
   rescan_macro_invocation_edit_script_rescanned_ranges &&rescanned_ranges,
   unexpanded_macro_id_out_tokens_origin_traces
     &&followed_unexpanded_macro_id_out_tokens_origin_traces,
   result &result_dst,
   const std::map<std::string, bool> &macro_ids,
   const bool next_tok_is_lparen,
   const bool create_traces)
    : _rewritten_forest{rewritten_forest},
      _result_dst{result_dst}, _pending_segment_rewrite{nullptr}
{
  using mi_edit_script_op = rescan_macro_invocation_edit_script_op;

  // First collect the final output segments' ranges for splitting and
  // binning subsequences of the edit script later below.
  struct final_out_segment
  {
    rescan_output_segment out_segment;
    std::size_t subtree_depth;
    rescan_macro_invocation::output::tokens_range final_out_range;
  };
  std::vector<final_out_segment> final_out_segments;
  std::size_t final_out_end = 0;
  // Argument trees have a dummy identity node at the root to
  // eventually bin the final output rewrite script to. For the
  // initial entry tree root this is not the case, increase the depth
  // by one to have a final virtual "source segment" to bin source
  // rewrites to then as well.
  assert(static_cast<rescan_macro_invocation>(subtree).is_rescan_tree_root());
  const unsigned int subtree_depth_offset =
    static_cast<rescan_macro_invocation>(subtree).is_arg_tree_root() ? 0 : 1;
  const auto subtree_final_out_end = subtree.final_out_end();
  for (auto it_final_out = subtree.final_out_begin();
       it_final_out != subtree_final_out_end; ++it_final_out) {
    const std::size_t final_out_segment_length
      = (*it_final_out).get_out_range_in_emitting_node().length();
    const rescan_macro_invocation::output::tokens_range final_out_range = {
      final_out_end,
      final_out_end + final_out_segment_length,
    };
    final_out_segments.emplace_back(final_out_segment{
      .out_segment = *it_final_out,
      .subtree_depth = it_final_out.depth() + subtree_depth_offset,
      .final_out_range = final_out_range,
    });

    final_out_end += final_out_segment_length;
  }

  const auto &lookup_final_out_segment =
    [&](std::vector<final_out_segment>::const_iterator it_hint,
	const std::size_t final_out_range_begin) {
      if (it_hint != final_out_segments.end()) {
	if (it_hint->final_out_range.begin <= final_out_range_begin) {
	  if (it_hint->final_out_range.end > final_out_range_begin)
	    return it_hint;
	} else {
	  it_hint = final_out_segments.begin();
	}
      } else {
	it_hint = final_out_segments.begin();
      }
      const auto it =
	std::lower_bound(it_hint, final_out_segments.cend(),
			 final_out_range_begin,
			 [](const final_out_segment &s,
			    const std::size_t final_out_range_begin) {
			   return (s.final_out_range.end <=
				   final_out_range_begin);
			 });
      return it;
    };

  const auto &find_common_final_out_ancestor =
    [subtree_depth_offset](const rescan_macro_invocation &final_out_segment0,
       const final_out_segment &final_out_segment1)
      -> std::pair<rescan_macro_invocation, std::size_t> {
      // Shortcut to avoid the quadratic search below in
      // the obvious case.
      if (final_out_segment0.is_rescan_tree_root())
	return std::make_pair(final_out_segment0, subtree_depth_offset);

      // A simple quadratic search -- this is most likely not
      // performance sensitive. If it turns out to be, there is a
      // method to achieve O(1) common ancestor retrieval with the
      // help of an auxiliary tree.
      std::size_t depth = final_out_segment1.subtree_depth;
      rescan_macro_invocation ancestor1 =
	final_out_segment1.out_segment.get_emitting_node();
      while (!ancestor1.is_rescan_tree_root()) {
	rescan_macro_invocation ancestor0 = final_out_segment0;
	while (!ancestor0.is_rescan_tree_root() &&
	       (static_cast<expansion_forest::nodes_size_type>(ancestor0) !=
		static_cast<expansion_forest::nodes_size_type>(ancestor1))) {
	  ancestor0 = ancestor0.get_rescan_parent();
	}
	if (!ancestor0.is_rescan_tree_root())
	  break;

	ancestor1 = ancestor1.get_rescan_parent();
	assert(depth != 0);
	--depth;
      }
      assert(!ancestor1.is_rescan_tree_root() ||
	     depth == subtree_depth_offset);
      return std::make_pair(ancestor1, depth);
    };

  const auto &is_insert_token =
    [](const mi_edit_script_op &op) {
      return op._op == mi_edit_script_op::op::insert_token;
    };

  const auto &is_insert_ws =
    [](const mi_edit_script_op &op) {
      return op._op == mi_edit_script_op::op::insert_ws;
    };

  const auto &is_insert_any =
    [&](const mi_edit_script_op &op) {
      return is_insert_token(op) || is_insert_ws(op);
    };


  const auto &is_insert_token_with_stickiness =
    [&](const mi_edit_script_op &op, const sticky_side stickiness) {
      return (is_insert_token(op) &&
	      op._insert_token.stickiness == stickiness);
    };

  const auto &is_insert_ws_with_stickiness =
    [&](const mi_edit_script_op &op, const sticky_side stickiness) {
      return (is_insert_ws(op) &&
	      op._insert_ws.stickiness == stickiness);
    };

  const auto &is_insert_any_with_stickiness =
    [&](const mi_edit_script_op &op, const sticky_side stickiness) {
      return (is_insert_token_with_stickiness(op, stickiness) ||
	      is_insert_ws_with_stickiness(op, stickiness));
    };

  const auto get_edit_script_op_range_begin =
    [](const mi_edit_script_op &op) {
      switch (op._op) {
      case mi_edit_script_op::op::copy:
	return op._copy.range.begin;
      case mi_edit_script_op::op::insert_token:
	assert(0);
	__builtin_unreachable();
      case mi_edit_script_op::op::insert_ws:
	assert(0);
	__builtin_unreachable();
      case mi_edit_script_op::op::replace_token:
	return op._replace_token.replaced_token_index;
      case mi_edit_script_op::op::rewrite_macro_id:
	return op._rewrite_macro_id.rewritten_token_index;
      }
    };

  // Normalize the edit script.
  // - No insertion ops with left stickiness at the head.
  // - Likewise, no insertion ops with right stickiness at the tail.
  // - No inversions, i.e. insertion ops with right stickiness
  //   preceding insertion ops with left stickiness in a sequence
  //   thereof.
  // - No insertions ops with a stickiness of none before any with a
  //   stickiness of left in a sequence thereof ("any left stickiness
  //   enforces left stickiness for every insertion before it").
  // - Likewise, no insertions ops with a stickiness of none after any
  //   with a stickiness of right in a sequence thereof ("any right
  //   stickiness enforces right stickiness for every insertion after
  //   it")
  rescan_macro_invocation_edit_script::size_type last_erase_begin_ops_pos = 0;
  std::size_t last_erase_begin_tokens_pos = 0;
  auto erase_edit_script_ops_cb =
    [&](const rescan_macro_invocation_edit_script &edit_script,
	const rescan_macro_invocation_edit_script::const_ops_iterator
	  &erase_begin,
	const rescan_macro_invocation_edit_script::const_ops_iterator
	  &erase_end) {
      rescanned_ranges.erase_region(erase_begin - edit_script.cbegin(),
				    erase_end - edit_script.cbegin());

      if (followed_unexpanded_macro_id_out_tokens_origin_traces.empty())
	return;

      const rescan_macro_invocation_edit_script::size_type
	cur_erase_begin_ops_pos = erase_begin - edit_script.cbegin();
      if (last_erase_begin_ops_pos > cur_erase_begin_ops_pos) {
	last_erase_begin_ops_pos = 0;
	last_erase_begin_tokens_pos = 0;
      }
      const auto cur_erase_begin_tokens_pos =
	std::accumulate(edit_script.cbegin() + last_erase_begin_ops_pos,
			erase_begin,
			last_erase_begin_tokens_pos,
			[](const auto pos, const auto &op) {
			  return pos + op.tokens_length();
			});
      const auto erased_tokens_length =
	std::accumulate(erase_begin, erase_end, static_cast<std::size_t>(0),
			[](const std::size_t length, const auto &op) {
			  return length + op.tokens_length();
			});

      followed_unexpanded_macro_id_out_tokens_origin_traces.erase_region
	(cur_erase_begin_tokens_pos,
	 cur_erase_begin_tokens_pos + erased_tokens_length);

      last_erase_begin_ops_pos = cur_erase_begin_ops_pos;
      last_erase_begin_tokens_pos = cur_erase_begin_tokens_pos;

    };
  _normalize_mi_edit_script(edit_script, erase_edit_script_ops_cb);

  // Insertion ops with a stickiness of none will get applied to the
  // common ancestor of the preceeding and succeeding final output
  // segments at that position inbetween. Note that as per the edit
  // script normalization, there are no insertion ops with a left
  // stickiness possible at the head, those with a stickiness of none
  // are to be inserted at the rescan root.
  rescan_macro_invocation preceding_final_out_segment =
    static_cast<rescan_macro_invocation>(subtree);

  assert(std::all_of
	 (edit_script.cbegin(), edit_script.cend(),
	  [&](const rescan_macro_invocation_edit_script_op &op) {
	    switch (op._op) {
	    case mi_edit_script_op::op::copy:
	      return (op._copy.range.begin <= op._copy.range.end &&
		      op._copy.range.end <= final_out_end);
	    case mi_edit_script_op::op::insert_token:
	      return true;
	    case mi_edit_script_op::op::insert_ws:
	      return true;
	    case mi_edit_script_op::op::replace_token:
	      return (op._replace_token.replaced_token_index <
		      final_out_end);
	    case mi_edit_script_op::op::rewrite_macro_id:
	      return (op._rewrite_macro_id.rewritten_token_index <
		      final_out_end);
	    }
	  }));

  std::vector<final_out_segment>::const_iterator it_final_out_segment =
    final_out_segments.begin();
  rescan_macro_invocation_edit_script::ops_iterator it_edit_script =
    edit_script.begin();
  while (it_edit_script != edit_script.end()) {
    auto it_next_non_insert_op =
      std::find_if_not(it_edit_script, edit_script.end(), is_insert_any);
    if (it_next_non_insert_op == edit_script.end()) {
      // Run of insertion ops at the tail. As per the normalization
      // and the fact that any insertion ops with left stickiness at
      // the head would have been consumed in a previous iteration,
      // there can only be insertion ops with a stickiness of none
      // pending between it_edit_script and it_next_non_insert_op.
      // Insert all those at the rescan subtree root node.
      assert(std::all_of(it_edit_script, it_next_non_insert_op,
			 std::bind(is_insert_any_with_stickiness,
				   std::placeholders::_1,
				   sticky_side::none)));
      rescan_macro_invocation_edit_script segment_edit_script{
	std::make_move_iterator(it_edit_script),
	std::make_move_iterator(edit_script.end()),
      };
      _rewrite_segments.push_back
	(_rewrite_segment{
	  ._queue_node = _rewrite_segment::_rewrite_queue_node{},
	  .subtree_depth = it_final_out_segment->subtree_depth,
	  .mi = static_cast<rescan_macro_invocation>(subtree),
	  .out_edit_script = std::move(segment_edit_script),
	  .rescanned_out_ranges = std::move(rescanned_ranges),
	  .unexpanded_macro_id_out_tokens_origin_traces =
	    std::move(followed_unexpanded_macro_id_out_tokens_origin_traces),
	 });

      // Not needed, but do it for good measure.
      it_edit_script = edit_script.end();
      break;
    }

    it_final_out_segment =
      (lookup_final_out_segment
       (it_final_out_segment,
	get_edit_script_op_range_begin(*it_next_non_insert_op)));
    assert(it_final_out_segment != final_out_segments.cend());

    // As per the normalization, and the fact that any insertion ops
    // with left stickiness at the head would have been consumed in a
    // previous iteration, there can only be insertion ops with a
    // stickiness of none followed by a run of insertion ops with a
    // stickiness of right at the tail pending between it_edit_script
    // and it_next_non_insert_op.
    // Move any such but the that trailing sequence of insertions with
    // a right stickiness, if any, to the common the common ancestor
    // of the preceding and the next final output segments.
    assert(!is_insert_any_with_stickiness(*it_edit_script, sticky_side::left));
    const auto insert_with_stickiness_none_end =
      (std::find_if_not(std::make_reverse_iterator(it_next_non_insert_op),
			std::make_reverse_iterator(it_edit_script),
			std::bind(is_insert_any_with_stickiness,
				  std::placeholders::_1,
				  sticky_side::right))
       .base());
    if (insert_with_stickiness_none_end != it_edit_script) {
      const auto common_ancestor
	= find_common_final_out_ancestor(preceding_final_out_segment,
					 *it_final_out_segment);
      rescan_macro_invocation_edit_script segment_edit_script{
	std::make_move_iterator(it_edit_script),
	std::make_move_iterator(insert_with_stickiness_none_end),
      };
      auto segment_rescanned_ranges =
	rescanned_ranges.split_off_head(insert_with_stickiness_none_end -
					it_edit_script);
      _rewrite_segments.push_back
	(_rewrite_segment{
	  ._queue_node = _rewrite_segment::_rewrite_queue_node{},
	  .subtree_depth = common_ancestor.second,
	  .mi = common_ancestor.first,
	  .out_edit_script = std::move(segment_edit_script),
	  .rescanned_out_ranges = std::move(segment_rescanned_ranges),
       });
      it_edit_script = insert_with_stickiness_none_end;
    }
    assert(!is_insert_any(*it_edit_script) ||
	   is_insert_any_with_stickiness(*it_edit_script, sticky_side::right));

    // Consume as much from the remaining edit_script head
    // as is overlapping with the final output segment found
    // for the first non-insert op.
    auto overlapping_edit_script_end = it_next_non_insert_op + 1;
    bool split_copy_at_tail = true;
    while (it_next_non_insert_op->_op != mi_edit_script_op::op::copy ||
	   (it_next_non_insert_op->_copy.range.end <=
	    it_final_out_segment->final_out_range.end)) {
      split_copy_at_tail = false;
      it_next_non_insert_op = std::find_if_not(overlapping_edit_script_end,
					       edit_script.end(),
					       is_insert_any);
      if (it_next_non_insert_op == edit_script.end())
	break;
      const auto op_range_begin =
	get_edit_script_op_range_begin(*it_next_non_insert_op);
      if (op_range_begin < it_final_out_segment->final_out_range.begin ||
	  op_range_begin >= it_final_out_segment->final_out_range.end) {
	break;
      }
      overlapping_edit_script_end = it_next_non_insert_op + 1;
      split_copy_at_tail = true;
    }
    // If the last op is fully contained in the current final output
    // segment, collect all subsequent insert ops with a stickiness of
    // left.
    if (!split_copy_at_tail) {
      overlapping_edit_script_end =
	std::find_if_not(overlapping_edit_script_end,
			 edit_script.end(),
			 std::bind(is_insert_any_with_stickiness,
				   std::placeholders::_1,
				   sticky_side::left));
    }

    rescan_macro_invocation_edit_script segment_edit_script{
      std::make_move_iterator(it_edit_script),
      std::make_move_iterator(overlapping_edit_script_end - split_copy_at_tail),
    };
    auto segment_rescanned_ranges =
      rescanned_ranges.split_off_head(overlapping_edit_script_end -
				      split_copy_at_tail -
				      it_edit_script);
    if (split_copy_at_tail) {
      assert((overlapping_edit_script_end - 1)->_op ==
	     mi_edit_script_op::op::copy);
      segment_edit_script.push_back(*(overlapping_edit_script_end - 1));
      segment_edit_script.back()._copy.range.end =
	it_final_out_segment->final_out_range.end;
      (overlapping_edit_script_end - 1)->_copy.range.begin =
	it_final_out_segment->final_out_range.end;
      it_edit_script = overlapping_edit_script_end - 1;
      // If the former copy op had been within some rescanned range,
      // then so do the two parts from the split-up.
      if (rescanned_ranges.is_covered(0)) {
	segment_rescanned_ranges.append
	  (rescan_macro_invocation_edit_script_rescanned_ranges::
	     mi_edit_script_rescanned_range{
	    .begin = segment_edit_script.size() - 1,
	    .end = segment_edit_script.size(),
	  });
      }
    } else {
      it_edit_script = overlapping_edit_script_end;
    }

    // Translate the offsets from relative to total final output to
    // relative to the segment's rescan_macro_invocation output.
    for (auto &op : segment_edit_script) {
      switch (op._op) {
      case mi_edit_script_op::op::copy:
	op._copy.range.begin -= it_final_out_segment->final_out_range.begin;
	op._copy.range.begin += (it_final_out_segment->out_segment
				 .get_out_range_in_emitting_node().begin);
	op._copy.range.end -= it_final_out_segment->final_out_range.begin;
	op._copy.range.end += (it_final_out_segment->out_segment
			       .get_out_range_in_emitting_node().begin);
	break;

      case mi_edit_script_op::op::insert_token:
	break;

      case mi_edit_script_op::op::insert_ws:
	break;

      case mi_edit_script_op::op::replace_token:
	op._replace_token.replaced_token_index -=
	  it_final_out_segment->final_out_range.begin;
	op._replace_token.replaced_token_index +=
	  (it_final_out_segment->out_segment
	   .get_out_range_in_emitting_node().begin);
	break;

      case mi_edit_script_op::op::rewrite_macro_id:
	op._rewrite_macro_id.rewritten_token_index -=
	  it_final_out_segment->final_out_range.begin;
	op._rewrite_macro_id.rewritten_token_index +=
	  (it_final_out_segment->out_segment
	   .get_out_range_in_emitting_node().begin);
	break;

      }
    }

    auto segment_unexpanded_macro_id_out_tokens_origin_traces =
      (followed_unexpanded_macro_id_out_tokens_origin_traces.split_off_head
       (segment_edit_script.tokens_length()));

    _rewrite_segments.push_back
      (_rewrite_segment{
	 ._queue_node = _rewrite_segment::_rewrite_queue_node{},
	 .subtree_depth = it_final_out_segment->subtree_depth,
	 .mi = it_final_out_segment->out_segment.get_emitting_node(),
	 .out_edit_script = std::move(segment_edit_script),
	 .rescanned_out_ranges = std::move(segment_rescanned_ranges),
	 .unexpanded_macro_id_out_tokens_origin_traces =
	   std::move(segment_unexpanded_macro_id_out_tokens_origin_traces),
       });

    preceding_final_out_segment =
      it_final_out_segment->out_segment.get_emitting_node();
    // Increment, will serve as a lookup hint for the next iteration.
    ++it_final_out_segment;

  }

  // Now that the edit script has been binned to the rescan tree's
  // final output segment, and the original output tokens to operate
  // on are accessible, collect the list of unexpanded id tokens
  // (possibly) resolving to some macro id definition from the
  // rewritten output.
  for (auto it_seg = _rewrite_segments.begin();
       create_traces && it_seg != _rewrite_segments.end(); ++it_seg) {
    unexpanded_macro_id_out_tokens_origin_traces
      segment_unexpanded_macro_id_out_tokens_origin_traces;
    std::size_t seg_out_token_index = 0;
    for (auto it_op = it_seg->out_edit_script.begin();
	 it_op != it_seg->out_edit_script.end(); ++it_op) {
      const auto op_result_tokens = it_op->get_result_tokens(it_seg->mi);
      if (op_result_tokens.is_insert_ws() ||
	  op_result_tokens.is_rewrite_macro_id()) {
	seg_out_token_index += 1;
	continue;
      }

      for (auto it_op_result_token = op_result_tokens.begin();
	   it_op_result_token != op_result_tokens.end();
	   ++it_op_result_token, ++seg_out_token_index) {
	if (!it_op_result_token->is_id())
	  continue;

	const auto matching_m = macro_ids.find(it_op_result_token->get_value());
	if (matching_m == macro_ids.end())
	  continue;

	const auto unexpanded_macro_id_out_token_index
	  = rewritten_forest._unexpanded_macro_id_out_tokens.size();
	rewritten_forest._unexpanded_macro_id_out_tokens.push_back
	  (rewritten_expansion_forest::unexpanded_macro_id_out_token{
	    matching_m->first,
	    false,
	  });
	segment_unexpanded_macro_id_out_tokens_origin_traces.append
	  (unexpanded_macro_id_out_token_origin_trace{
	    .traced_out_token_index = seg_out_token_index,
	    .unexpanded_macro_id_out_token =
	      unexpanded_macro_id_out_token_index,
	  });

	const bool any_matching_m_func_like = matching_m->second;
	if (!any_matching_m_func_like)
	  continue;

	// There is at least one func-like macro definition for the id
	// in question. See whether the next token is an opening
	// parenthesis. If not, the func-like definitions wouldn't
	// conflict.
	bool lookahead_next_tok_is_lparen = next_tok_is_lparen;
	auto lookahead_it_op = it_op;
	auto lookahead_it_seg = it_seg;
	auto lookahead_it_op_result_token = it_op_result_token;
	auto lookahead_op_result_tokens_end = op_result_tokens.end();
	while (true) {
	  ++lookahead_it_op_result_token;
	  while (lookahead_it_op_result_token ==
		 lookahead_op_result_tokens_end) {
	    ++lookahead_it_op;
	    while (lookahead_it_op == lookahead_it_seg->out_edit_script.end()) {
	      auto prev_seg_rmi = lookahead_it_seg->mi;
	      ++lookahead_it_seg;
	      if (lookahead_it_seg == _rewrite_segments.end())
		break;
	      // As per how macro expansion rescan works, only opening
	      // parenthesises from rescan ancestors can combine
	      // with id tokens.
	      bool is_rescan_ancestor = false;
	      for (auto prev_seg_rmi_ancestor =
		     prev_seg_rmi.get_rescan_parent();
		   !prev_seg_rmi_ancestor.is_nil_node();
		   prev_seg_rmi_ancestor =
		     prev_seg_rmi_ancestor.get_rescan_parent()) {
		if (prev_seg_rmi_ancestor == lookahead_it_seg->mi) {
		  is_rescan_ancestor = true;
		  break;
		}
	      }
	      if (!is_rescan_ancestor) {
		lookahead_next_tok_is_lparen = false;
		lookahead_it_seg = _rewrite_segments.end();
		break;
	      }
	      lookahead_it_op = lookahead_it_seg->out_edit_script.begin();
	    }
	    if (lookahead_it_seg == _rewrite_segments.end())
	      break;

	    const auto lookahead_op_result_tokens =
	      lookahead_it_op->get_result_tokens(lookahead_it_seg->mi);
	    if (lookahead_op_result_tokens.is_rewrite_macro_id()) {
	      lookahead_next_tok_is_lparen = false;
	      lookahead_it_seg = _rewrite_segments.end();
	      break;
	    } else if (lookahead_op_result_tokens.is_insert_ws()) {
	      lookahead_it_op_result_token = nullptr;
	      lookahead_op_result_tokens_end = nullptr;
	    } else {
	      lookahead_it_op_result_token = lookahead_op_result_tokens.begin();
	      lookahead_op_result_tokens_end = lookahead_op_result_tokens.end();
	    }
	  }
	  if (lookahead_it_seg == _rewrite_segments.end())
	    break;

	  if (lookahead_it_op_result_token->is_ws() ||
	      lookahead_it_op_result_token->is_empty()) {
	    continue;
	  }

	  lookahead_next_tok_is_lparen = lookahead_it_op_result_token->is_punctuator("(");
	  break;
	}
	rewritten_forest._unexpanded_macro_id_out_tokens.back()._has_lparen =
	  lookahead_next_tok_is_lparen;
      }
    }
    // Merge the newly found traces with the ones followed from the
    // outside (through
    // followed_unexpanded_macro_id_out_tokens_origin_traces) and
    // aleady binned to the segment above.
    it_seg->unexpanded_macro_id_out_tokens_origin_traces.merge_with
      (std::move(segment_unexpanded_macro_id_out_tokens_origin_traces));
  }

  // And finally, prepare the rewrite queue, ordered by the segments'
  // associated subtree depths.
  _prepare_rewrite_queue();
}

std::unique_ptr<rescan_subtree_rewrite_cursor>
rescan_subtree_rewrite_cursor::continue_rewrite()
{
  if (_rewrite_segments.empty()) {
    // Input edit script was empty, nothing to do.
    return nullptr;
  }

  // Rewrite segments in order of decreasing expansion tree depth
  // until there's only the identity root rescan_macro_invocation node
  // left.
  assert(!_rewrite_segments.empty());
  assert(_rewrite_queue_head != _rewrite_segments.end());
  while (_rewrite_queue_head->subtree_depth != 0) {
    if (!_pending_segment_rewrite) {
      // All of the segments associated with the same
      // rescan_macro_invocation must be processed together.
      _rewrite_queue_head = _merge_segments_with_same_mi(_rewrite_queue_head);
    }
    const auto queue_head = _rewrite_queue_head;

    if (!_pending_segment_rewrite) {
      auto rrmi =
	(_rewritten_forest.get()._add_rewritten_rescan_macro_invocation
	 (queue_head->mi));

      _pending_segment_rewrite.reset
	(new rescan_macro_invocation_rewriter{
	   _rewritten_forest,
	   std::move(rrmi),
	   std::move(queue_head->out_edit_script),
	   std::move(queue_head->rescanned_out_ranges),
	   std::move(queue_head->unexpanded_macro_id_out_tokens_origin_traces),
	   std::move(queue_head->rewritten_rescan_macro_invocation_children),
	   std::move(queue_head->descendant_rescan_counter_invocations),
	 });
    }

    auto progress = _pending_segment_rewrite->continue_rewrite();
    // If the rescan_macro_invocation_rewriter asks for the rewrite of
    // some argument tree, process that first before eventually
    // returning back to it.
    auto queue_arg_rewrite = progress.grab_pending_arg_tree_rewrite();
    if (queue_arg_rewrite)
      return queue_arg_rewrite;

    auto result_src_rewrite = progress.grab_result();
    auto rrmi =
      _pending_segment_rewrite->get_rewritten_rescan_macro_invocation();
    for (const auto rescan_child_rrmi :
	   result_src_rewrite.rewritten_rescan_macro_invocation_children) {
      // Order the rescan children by the position of their name
      // token within the parent's output.
      const auto original_child =
	_rewritten_forest.get().get_original_forest()
	[_rewritten_forest.get()
	 .get_rewritten_rescan_macro_invocation(rescan_child_rrmi)
	 .get_original()];
      rrmi._add_rescan_child(rescan_child_rrmi,
			     original_child.get_src_range().begin);
    }

    _propagate_rescan_macro_invocation_src_rewrite
      (queue_head,
       _pending_segment_rewrite->get_rewritten_rescan_macro_invocation(),
       std::move(result_src_rewrite.src_edit_script),
       std::move(result_src_rewrite.src_rescanned_ranges),
       (std::move
	 (result_src_rewrite.unexpanded_macro_id_out_tokens_origin_traces)),
       std::move(result_src_rewrite.rescan_subtree_counter_invocations));
    _pending_segment_rewrite = nullptr;
    if (_rewrite_segments.empty()) {
      // The segment just processed was the last one and its
      // rescan_macro_invocation got deleted as a whole, because all
      // of the subtree's non-empty output had been deleted and only
      // ops on placeholder empty tokens emitted by that invocation
      // remained. Nothing is left to rewrite, the result stays empty
      // just as for an empty input edit script.
      assert(_rewrite_queue_head == _rewrite_segments.end());
      return nullptr;
    }
    assert(_rewrite_queue_head != _rewrite_segments.end());
  }

  // Insertions binned to the rescan tree root by the constructor,
  // e.g. ones with a stickiness of none inbetween two tokens emitted
  // by the root or at the very head or tail, live in segments of
  // their own. Merge those into the final root segment now, in
  // output order.
  if (_rewrite_segments.size() > 1)
    _rewrite_queue_head = _merge_segments_with_same_mi(_rewrite_queue_head);

  // For the invocation's initial entry rescan tree, the final segment
  // at subtree depth zero (c.f. subtree_depth_offset in the
  // constructor) has no rescan_macro_invocation associated with
  // it. Otherwise, i.e. for argument expansion trees, the final
  // segment corresponds to the argument tree's identity source root
  // node. In either case, grab the combined rewrite script to apply
  // to the rescan tree's source as has been binned into that final
  // segment.
  assert(_rewrite_segments.size() == 1);
  assert(_rewrite_queue_head == _rewrite_segments.begin());
  assert(_rewrite_queue_head->mi.is_nil_node() ||
	 _rewrite_queue_head->mi.is_arg_tree_root());
  _result_dst.get().src_edit_script =
    std::move(_rewrite_segments.front().out_edit_script);
  _result_dst.get().rescanned_src_ranges =
    std::move(_rewrite_segments.front().rescanned_out_ranges);
  _result_dst.get().unexpanded_macro_id_out_tokens_origin_traces =
    (std::move
     (_rewrite_segments.front().unexpanded_macro_id_out_tokens_origin_traces));
  for (const auto &c :
	 _rewrite_segments.front().rewritten_rescan_macro_invocation_children) {
    _result_dst.get().top_level_rewritten_rescan_macro_invocations
      .push_back(c.child_rrmi);
  }
  std::vector<rewritten_expansion_forest::counter_invocations::size_type>
    rescan_subtree_counter_invocations;
  rescan_subtree_counter_invocations.reserve
    (_rewrite_segments.front().descendant_rescan_counter_invocations.size());
  for (const auto &i :
	 _rewrite_segments.front().descendant_rescan_counter_invocations) {
    rescan_subtree_counter_invocations.push_back(i.invocation);
  }
  _result_dst.get().rescan_subtree_counter_invocations =
    std::move(rescan_subtree_counter_invocations);
  _unlink_rewrite_queue_node(_rewrite_segments.begin()->_queue_node);
  assert(_rewrite_queue_head == _rewrite_segments.end());
  _rewrite_segments.erase(_rewrite_segments.begin());

  return nullptr;
}


void rescan_subtree_rewrite_cursor::
_propagate_rescan_macro_invocation_src_rewrite
  (const _rewrite_segments_type::iterator &it_rewrite_seg,
   const rewritten_expansion_forest::rewritten_rescan_macro_invocation& rrmi,
   rescan_macro_invocation_edit_script &&rmi_src_edit_script,
   rescan_macro_invocation_edit_script_rescanned_ranges
     &&rescanned_rmi_src_ranges,
   unexpanded_macro_id_out_tokens_origin_traces
     &&followed_unexpanded_macro_id_out_tokens_origin_traces,
   std::vector<rewritten_expansion_forest::counter_invocations::size_type>
     &&rescan_subtree_counter_invocations)
{
  using mi_edit_script_op = rescan_macro_invocation_edit_script_op;

  assert(it_rewrite_seg->subtree_depth != 0);
  if (rmi_src_edit_script.empty()) {
    _unlink_rewrite_queue_node(it_rewrite_seg->_queue_node);
    _rewrite_segments.erase(it_rewrite_seg);
    return;
  }

  // First collect the macro invocation's source ranges for splitting
  // and binning subsequences of the edit script later below.
  struct rmi_src_segment
  {
    rescan_output_segment out_segment;
    std::size_t subtree_depth;
    rescan_macro_invocation::output::tokens_range rmi_src_range;
  };
  std::vector<rmi_src_segment> rmi_src_segments;
  std::size_t rmi_src_tokens_end= 0;
  const auto rmi_src_end = it_rewrite_seg->mi.src_end(false);
  for (auto it_src = it_rewrite_seg->mi.src_begin(false);
       it_src != rmi_src_end; ++it_src) {
    const std::size_t src_segment_length
      = (*it_src).get_out_range_in_emitting_node().length();
    const rescan_macro_invocation::output::tokens_range rmi_src_range = {
      rmi_src_tokens_end,
      rmi_src_tokens_end + src_segment_length,
    };
    assert(it_rewrite_seg->subtree_depth >= it_src.rescan_tree_distance());
    // The subtree depth decreases strictly between subsequent src
    // segments, as per how macro expansion rescan works.
    assert(rmi_src_segments.empty() ||
	   (rmi_src_segments.back().subtree_depth >
	    it_rewrite_seg->subtree_depth - it_src.rescan_tree_distance()));
    rmi_src_segments.emplace_back(rmi_src_segment{
      .out_segment = *it_src,
      .subtree_depth =
	it_rewrite_seg->subtree_depth - it_src.rescan_tree_distance(),
      .rmi_src_range = rmi_src_range,
    });

    rmi_src_tokens_end += src_segment_length;
  }

  const auto &lookup_rmi_src_segment =
    [&](std::vector<rmi_src_segment>::const_iterator it_hint,
	const std::size_t rmi_src_range_begin) {
      if (it_hint != rmi_src_segments.end()) {
	if (it_hint->rmi_src_range.begin <= rmi_src_range_begin) {
	  if (it_hint->rmi_src_range.end > rmi_src_range_begin)
	    return it_hint;
	} else {
	  it_hint = rmi_src_segments.begin();
	}
      } else {
	it_hint = rmi_src_segments.begin();
      }
      const auto it =
	std::lower_bound(it_hint, rmi_src_segments.cend(),
			 rmi_src_range_begin,
			 [](const rmi_src_segment &s,
			    const std::size_t rmi_src_range_begin) {
			   return (s.rmi_src_range.end <=
				   rmi_src_range_begin);
			 });
      return it;
    };

    const auto &is_insert_token =
    [](const mi_edit_script_op &op) {
      return op._op == mi_edit_script_op::op::insert_token;
    };

  const auto &is_insert_ws =
    [](const mi_edit_script_op &op) {
      return op._op == mi_edit_script_op::op::insert_ws;
    };

  const auto &is_insert_any =
    [&](const mi_edit_script_op &op) {
      return is_insert_token(op) || is_insert_ws(op);
    };


  const auto &is_insert_token_with_stickiness =
    [&](const mi_edit_script_op &op, const sticky_side stickiness) {
      return (is_insert_token(op) &&
	      op._insert_token.stickiness == stickiness);
    };

  const auto &is_insert_ws_with_stickiness =
    [&](const mi_edit_script_op &op, const sticky_side stickiness) {
      return (is_insert_ws(op) &&
	      op._insert_ws.stickiness == stickiness);
    };

  const auto &is_insert_any_with_stickiness =
    [&](const mi_edit_script_op &op, const sticky_side stickiness) {
      return (is_insert_token_with_stickiness(op, stickiness) ||
	      is_insert_ws_with_stickiness(op, stickiness));
    };

  const auto get_edit_script_op_range_begin =
    [](const mi_edit_script_op &op) {
      switch (op._op) {
      case mi_edit_script_op::op::copy:
	return op._copy.range.begin;
      case mi_edit_script_op::op::insert_token:
	assert(0);
	__builtin_unreachable();
      case mi_edit_script_op::op::insert_ws:
	assert(0);
	__builtin_unreachable();
      case mi_edit_script_op::op::replace_token:
	return op._replace_token.replaced_token_index;
      case mi_edit_script_op::op::rewrite_macro_id:
	return op._rewrite_macro_id.rewritten_token_index;
      }
    };

  // Normalize the edit script.
  // - No insertion ops with left stickiness at the head.
  // - Likewise, no insertion ops with right stickiness at the tail.
  // - No inversions, i.e. insertion ops with right stickiness
  //   preceding insertion ops with left stickiness in a sequence
  //   thereof.
  // - No insertions ops with a stickiness of none before any with a
  //   stickiness of left in a sequence thereof ("any left stickiness
  //   enforces left stickiness for every insertion before it").
  // - Likewise, no insertions ops with a stickiness of none after any
  //   with a stickiness of right in a sequence thereof ("any right
  //   stickiness enforces right stickiness for every insertion after
  //   it")
  rescan_macro_invocation_edit_script::size_type last_erase_begin_ops_pos = 0;
  std::size_t last_erase_begin_tokens_pos = 0;
  auto erase_edit_script_ops_update_offsets_cb =
    [&](const rescan_macro_invocation_edit_script &edit_script,
	const rescan_macro_invocation_edit_script::const_ops_iterator
	  &erase_begin,
	const rescan_macro_invocation_edit_script::const_ops_iterator
	  &erase_end) {
      rescanned_rmi_src_ranges.erase_region(erase_begin - edit_script.cbegin(),
					    erase_end - edit_script.cbegin());

      if (followed_unexpanded_macro_id_out_tokens_origin_traces.empty())
	return;

      const rescan_macro_invocation_edit_script::size_type
	cur_erase_begin_ops_pos = erase_begin - edit_script.cbegin();
      if (last_erase_begin_ops_pos > cur_erase_begin_ops_pos) {
	last_erase_begin_ops_pos = 0;
	last_erase_begin_tokens_pos = 0;
      }
      const auto cur_erase_begin_tokens_pos =
	std::accumulate(edit_script.cbegin() + last_erase_begin_ops_pos,
			erase_begin,
			last_erase_begin_tokens_pos,
			[](const auto pos, const auto &op) {
			  return pos + op.tokens_length();
			});
      const auto erased_tokens_length =
	std::accumulate(erase_begin, erase_end, static_cast<std::size_t>(0),
			[](const std::size_t length, const auto &op) {
			  return length + op.tokens_length();
			});

      followed_unexpanded_macro_id_out_tokens_origin_traces.erase_region
	(cur_erase_begin_tokens_pos,
	 cur_erase_begin_tokens_pos + erased_tokens_length);

      last_erase_begin_ops_pos = cur_erase_begin_ops_pos;
      last_erase_begin_tokens_pos = cur_erase_begin_tokens_pos;
    };
  _normalize_mi_edit_script(rmi_src_edit_script,
			    erase_edit_script_ops_update_offsets_cb);

  assert(std::all_of
	 (rmi_src_edit_script.cbegin(), rmi_src_edit_script.cend(),
	  [&](const rescan_macro_invocation_edit_script_op &op) {
	    switch (op._op) {
	    case mi_edit_script_op::op::copy:
	      return (op._copy.range.begin <= op._copy.range.end &&
		      op._copy.range.end <= rmi_src_tokens_end);
	    case mi_edit_script_op::op::insert_token:
	      return true;
	    case mi_edit_script_op::op::insert_ws:
	      return true;
	    case mi_edit_script_op::op::replace_token:
	      return (op._replace_token.replaced_token_index <
		      rmi_src_tokens_end);
	    case mi_edit_script_op::op::rewrite_macro_id:
	      return (op._rewrite_macro_id.rewritten_token_index <
		      rmi_src_tokens_end);
	    }
	  }));

  // All the rewritten subtree's __COUNTER__ invocations get
  // associated with the position of the rewritten subtree root
  // invocation, i.e. position zero. Note that the offset will get
  // adjusted when joining a single rescan macro invocation's output
  // segments together below.
  std::vector<_rewrite_segment::descendant_rescan_counter_invocation>
    descendant_rescan_counter_invocations;
  descendant_rescan_counter_invocations.reserve
    (rescan_subtree_counter_invocations.size());
  for (const auto &counter_invocation : rescan_subtree_counter_invocations) {
    descendant_rescan_counter_invocations.push_back
      (_rewrite_segment::descendant_rescan_counter_invocation{
	.out_token_index = 0,
	.invocation = counter_invocation,
      });
  }

  _rewrite_segments_type::iterator inserted_rewrite_segments_end =
    it_rewrite_seg;
  std::vector<rmi_src_segment>::const_iterator it_rmi_src_segment =
    rmi_src_segments.begin();
  rescan_macro_invocation_edit_script::ops_iterator it_edit_script =
    rmi_src_edit_script.begin();
  while (it_edit_script != rmi_src_edit_script.end()) {
    auto it_last_inserted_rewrite_seg = _rewrite_segments.end();
    if (inserted_rewrite_segments_end != it_rewrite_seg) {
      it_last_inserted_rewrite_seg = inserted_rewrite_segments_end;
      --it_last_inserted_rewrite_seg;
    }

    auto it_next_non_insert_op =
      std::find_if_not(it_edit_script, rmi_src_edit_script.end(),
		       is_insert_any);
    if (it_next_non_insert_op == rmi_src_edit_script.end()) {
      // Run of insertion ops at the tail. As per the normalization
      // and the fact that any insertion ops with left stickiness at
      // the head would have been consumed in a previous iteration,
      // there can only be insertion ops with a stickiness of none
      // pending between it_edit_script and it_next_non_insert_op.
      // Insert all those at the rescan subtree root node.
      assert(std::all_of(it_edit_script, it_next_non_insert_op,
			 std::bind(is_insert_any_with_stickiness,
				   std::placeholders::_1,
				   sticky_side::none)));
      rescan_macro_invocation_edit_script segment_edit_script{
	std::make_move_iterator(it_edit_script),
	std::make_move_iterator(rmi_src_edit_script.end()),
      };
      if (it_last_inserted_rewrite_seg == _rewrite_segments.end()) {
	// The former _rewrite_segments entry has not been recycled
	// yet. Do it now.
	it_rewrite_seg->mi =
	  rmi_src_segments.back().out_segment.get_emitting_node();
	it_rewrite_seg->subtree_depth = rmi_src_segments.back().subtree_depth;
	it_rewrite_seg->out_edit_script =
	  rescan_macro_invocation_edit_script{
	    std::make_move_iterator(it_edit_script),
	    std::make_move_iterator(rmi_src_edit_script.end())
	  };
	it_rewrite_seg->rescanned_out_ranges =
	  std::move(rescanned_rmi_src_ranges);
	it_rewrite_seg->unexpanded_macro_id_out_tokens_origin_traces =
	  std::move(followed_unexpanded_macro_id_out_tokens_origin_traces);
	// The macro rescan invocation just rewritten will be a rescan
	// child of the to be rewritten macro invocation emitting the
	// former's first source segment.
	it_rewrite_seg->rewritten_rescan_macro_invocation_children =
	  std::vector
	    <_rewrite_segment::rewritten_rescan_macro_invocation_child>{
	    {
	      _rewrite_segment::rewritten_rescan_macro_invocation_child{
		.out_token_index = 0,
		.child_rrmi =
		  static_cast<rewritten_expansion_forest::nodes_size_type>(rrmi)
	      }
	    }
	  };
	it_rewrite_seg->descendant_rescan_counter_invocations =
	  std::move(descendant_rescan_counter_invocations);
	++inserted_rewrite_segments_end;
      } else if ((static_cast<expansion_forest::nodes_size_type>
		  (it_last_inserted_rewrite_seg->mi)) ==
		 (static_cast<expansion_forest::nodes_size_type>
		  (rmi_src_segments.back().out_segment.get_emitting_node()))) {
	// The most recently added entry had been for the macro
	// invocation's last src segment's emitting
	// rescan_macro_invocation. Extend it.
	it_last_inserted_rewrite_seg->rescanned_out_ranges.join_with
	  (std::move(rescanned_rmi_src_ranges),
	   it_last_inserted_rewrite_seg->out_edit_script.size());
	it_last_inserted_rewrite_seg
	  ->unexpanded_macro_id_out_tokens_origin_traces
	  .join_with
	    (std::move(followed_unexpanded_macro_id_out_tokens_origin_traces),
	     it_last_inserted_rewrite_seg->out_edit_script.tokens_length());
	it_last_inserted_rewrite_seg->out_edit_script.insert
	  (it_last_inserted_rewrite_seg->out_edit_script.end(),
	   std::make_move_iterator(it_edit_script),
	   std::make_move_iterator(rmi_src_edit_script.end()));
      } else {
	// Create a completely new entry.
	rescan_macro_invocation_edit_script segment_edit_script{
	  std::make_move_iterator(it_edit_script),
	  std::make_move_iterator(rmi_src_edit_script.end()),
	};
	const auto it_inserted_rewrite_seg =
	  (_rewrite_segments.insert
	   (inserted_rewrite_segments_end,
	    _rewrite_segment{
	     ._queue_node = _rewrite_segment::_rewrite_queue_node{},
	     .subtree_depth = rmi_src_segments.back().subtree_depth,
	     .mi = rmi_src_segments.back().out_segment.get_emitting_node(),
	     .out_edit_script = std::move(segment_edit_script),
	     .rescanned_out_ranges = std::move(rescanned_rmi_src_ranges),
	     .unexpanded_macro_id_out_tokens_origin_traces =
	       std::move(followed_unexpanded_macro_id_out_tokens_origin_traces),
	    }));
	_link_rewrite_queue_node_after
	  (it_last_inserted_rewrite_seg->_queue_node.next,
	   it_inserted_rewrite_seg);
      }

      // Not needed, but do it for good measure.
      it_edit_script = rmi_src_edit_script.end();
      break;
    }

    it_rmi_src_segment =
      (lookup_rmi_src_segment
       (it_rmi_src_segment,
	get_edit_script_op_range_begin(*it_next_non_insert_op)));

    // Consume as much from the remaining edit_script head as is
    // overlapping with the source segment found for the first
    // non-insert op.
    auto overlapping_edit_script_end = it_next_non_insert_op + 1;
    bool split_copy_at_tail = true;
    while (it_next_non_insert_op->_op != mi_edit_script_op::op::copy ||
	   (it_next_non_insert_op->_copy.range.end <=
	    it_rmi_src_segment->rmi_src_range.end)) {
      split_copy_at_tail = false;
      it_next_non_insert_op = std::find_if_not(overlapping_edit_script_end,
					       rmi_src_edit_script.end(),
					       is_insert_any);
      if (it_next_non_insert_op == rmi_src_edit_script.end())
	break;
      const auto op_range_begin =
	get_edit_script_op_range_begin(*it_next_non_insert_op);
      if (op_range_begin < it_rmi_src_segment->rmi_src_range.begin ||
	  op_range_begin >= it_rmi_src_segment->rmi_src_range.end) {
	break;
      }
      overlapping_edit_script_end = it_next_non_insert_op + 1;
      split_copy_at_tail = true;
    }
    // If the last op is fully contained in the current source output
    // segment, collect all subsequent insert ops with a stickiness of
    // left.
    if (!split_copy_at_tail) {
      overlapping_edit_script_end =
	std::find_if_not(overlapping_edit_script_end,
			 rmi_src_edit_script.end(),
			 std::bind(is_insert_any_with_stickiness,
				   std::placeholders::_1,
				   sticky_side::left));
    }

    auto segment_rescanned_ranges =
      rescanned_rmi_src_ranges.split_off_head(overlapping_edit_script_end -
					      split_copy_at_tail -
					      it_edit_script);
    // Because of the way rescan macro expansion works, a macro
    // invocation's source can only extend from the node emitting its
    // name token into successively shallower ones: tokens from any
    // deeper node's output would get consumed as (part of) an
    // argument and never close the argument list. As the edit
    // scripts are in source order, the source segments' depths are
    // thus non-increasing.
    assert(it_last_inserted_rewrite_seg == _rewrite_segments.end() ||
	   (it_last_inserted_rewrite_seg->subtree_depth >=
	    it_rmi_src_segment->subtree_depth));
    rescan_macro_invocation_edit_script segment_edit_script{
      std::make_move_iterator(it_edit_script),
      std::make_move_iterator(overlapping_edit_script_end -
			      split_copy_at_tail),
    };
    if (split_copy_at_tail) {
      assert((overlapping_edit_script_end - 1)->_op ==
	     mi_edit_script_op::op::copy);
      segment_edit_script.push_back(*(overlapping_edit_script_end - 1));
      segment_edit_script.back()._copy.range.end =
	it_rmi_src_segment->rmi_src_range.end;
      (overlapping_edit_script_end - 1)->_copy.range.begin =
	it_rmi_src_segment->rmi_src_range.end;
      if (rescanned_rmi_src_ranges.is_covered(0)) {
	segment_rescanned_ranges.append
	  (rescan_macro_invocation_edit_script_rescanned_ranges::
	   mi_edit_script_rescanned_range{
	    .begin = segment_edit_script.size() - 1,
	    .end = segment_edit_script.size(),
	  });
      }
    }

    // Translate the offsets from relative to total source input to
    // relative to the segment's emitting rescan_macro_invocation's
    // output.
    for (auto &op : segment_edit_script) {
      switch (op._op) {
      case mi_edit_script_op::op::copy:
	op._copy.range.begin -= it_rmi_src_segment->rmi_src_range.begin;
	op._copy.range.begin += (it_rmi_src_segment->out_segment
				 .get_out_range_in_emitting_node().begin);
	op._copy.range.end -= it_rmi_src_segment->rmi_src_range.begin;
	op._copy.range.end += (it_rmi_src_segment->out_segment
			       .get_out_range_in_emitting_node().begin);
	break;

      case mi_edit_script_op::op::insert_token:
	break;

      case mi_edit_script_op::op::insert_ws:
	break;

      case mi_edit_script_op::op::replace_token:
	op._replace_token.replaced_token_index -=
	  it_rmi_src_segment->rmi_src_range.begin;
	op._replace_token.replaced_token_index +=
	  (it_rmi_src_segment->out_segment
	   .get_out_range_in_emitting_node().begin);
	break;

      case mi_edit_script_op::op::rewrite_macro_id:
	op._rewrite_macro_id.rewritten_token_index -=
	  it_rmi_src_segment->rmi_src_range.begin;
	op._rewrite_macro_id.rewritten_token_index +=
	  (it_rmi_src_segment->out_segment
	   .get_out_range_in_emitting_node().begin);
	break;
      }
    }

    auto segment_unexpanded_macro_id_out_tokens_origin_traces =
      (followed_unexpanded_macro_id_out_tokens_origin_traces.split_off_head
       (segment_edit_script.tokens_length()));

    assert(it_last_inserted_rewrite_seg == _rewrite_segments.end() ||
	   ((it_last_inserted_rewrite_seg->subtree_depth ==
	     it_rmi_src_segment->subtree_depth) ==
	    ((static_cast<expansion_forest::nodes_size_type>
	      (it_last_inserted_rewrite_seg->mi)) ==
	     (static_cast<expansion_forest::nodes_size_type>
	      (it_rmi_src_segment->out_segment.get_emitting_node())))));

    if (it_last_inserted_rewrite_seg == _rewrite_segments.end()) {
      // The former _rewrite_segments entry has not been recycled
      // yet. Do it now.
      it_rewrite_seg->mi =
	it_rmi_src_segment->out_segment.get_emitting_node();
      it_rewrite_seg->subtree_depth = it_rmi_src_segment->subtree_depth;
      it_rewrite_seg->out_edit_script = std::move(segment_edit_script);
      it_rewrite_seg->rescanned_out_ranges =
	std::move(segment_rescanned_ranges);
      it_rewrite_seg->unexpanded_macro_id_out_tokens_origin_traces =
	std::move(segment_unexpanded_macro_id_out_tokens_origin_traces);
      // The macro rescan invocation just rewritten will be a rescan
      // child of the to be rewritten macro invocation emitting the
      // former's first source segment.
      it_rewrite_seg->rewritten_rescan_macro_invocation_children =
	std::vector
	  <_rewrite_segment::rewritten_rescan_macro_invocation_child>{
	  {
	    _rewrite_segment::rewritten_rescan_macro_invocation_child{
	      .out_token_index = 0,
	      .child_rrmi =
		static_cast<rewritten_expansion_forest::nodes_size_type>(rrmi)
	    }
	  }
	};
      it_rewrite_seg->descendant_rescan_counter_invocations =
	std::move(descendant_rescan_counter_invocations);
      ++inserted_rewrite_segments_end;
    } else if ((static_cast<expansion_forest::nodes_size_type>
		(it_last_inserted_rewrite_seg->mi)) ==
	       (static_cast<expansion_forest::nodes_size_type>
		(it_rmi_src_segment->out_segment.get_emitting_node()))) {
      // The most recently added entry had been for the macro
      // invocation's last src segment's emitting
      // rescan_macro_invocation. Extend it.
      it_last_inserted_rewrite_seg->rescanned_out_ranges.join_with
	(std::move(segment_rescanned_ranges),
	 it_last_inserted_rewrite_seg->out_edit_script.size());
      it_last_inserted_rewrite_seg
	->unexpanded_macro_id_out_tokens_origin_traces
	.join_with
	  (std::move(segment_unexpanded_macro_id_out_tokens_origin_traces),
	   it_last_inserted_rewrite_seg->out_edit_script.tokens_length());
      it_last_inserted_rewrite_seg->out_edit_script.insert
	(it_last_inserted_rewrite_seg->out_edit_script.end(),
	 std::make_move_iterator(segment_edit_script.begin()),
	 std::make_move_iterator(segment_edit_script.end()));
    } else {
      // Create a completely new entry.
      const auto it_inserted_rewrite_seg =
	(_rewrite_segments.insert
	 (inserted_rewrite_segments_end,
	  _rewrite_segment{
	   ._queue_node = _rewrite_segment::_rewrite_queue_node{},
	   .subtree_depth = it_rmi_src_segment->subtree_depth,
	   .mi = it_rmi_src_segment->out_segment.get_emitting_node(),
	   .out_edit_script = std::move(segment_edit_script),
	   .rescanned_out_ranges = std::move(segment_rescanned_ranges),
	   .unexpanded_macro_id_out_tokens_origin_traces =
	     std::move(segment_unexpanded_macro_id_out_tokens_origin_traces),
	  }));
      _link_rewrite_queue_node_after
	(it_last_inserted_rewrite_seg->_queue_node.next,
	 it_inserted_rewrite_seg);
    }
    it_edit_script = overlapping_edit_script_end - split_copy_at_tail;
  }

  // Regarding the rewrite queue, the (repurposed) it_rewrite_seg had
  // been at the rewrite queue head and the newly inserted segments
  // have been linked after it. Move all of them to their target
  // positions in the queue, which is ordered by descending
  // subtree_depth: insert each one before the first node with a
  // strictly smaller depth.
  for (auto it_seg = it_rewrite_seg; it_seg != inserted_rewrite_segments_end;
       ++it_seg) {
    _unlink_rewrite_queue_node(it_seg->_queue_node);
    _rewrite_segments_type::iterator *prev_next = &_rewrite_queue_head;
    while (*prev_next != _rewrite_segments.end() &&
	   (*prev_next)->subtree_depth >= it_seg->subtree_depth) {
      prev_next = &(*prev_next)->_queue_node.next;
    }
    _link_rewrite_queue_node_after(*prev_next, it_seg);
  }

  // Finally, join neigboring rewrite segments targetting the same
  // rescan_macro_invocation together.
  auto it_seg = it_rewrite_seg;
  if (it_seg != _rewrite_segments.begin())
    --it_seg;
  auto it_next_seg = it_seg;
  ++it_next_seg;
  auto joinable_segments_end = inserted_rewrite_segments_end;
  if (joinable_segments_end != _rewrite_segments.end())
    ++joinable_segments_end;
  while (it_next_seg != joinable_segments_end) {
    if (static_cast<expansion_forest::nodes_size_type>(it_seg->mi) ==
	static_cast<expansion_forest::nodes_size_type>(it_next_seg->mi)) {
      assert(it_seg->subtree_depth == it_next_seg->subtree_depth);
      _join_segment_into(it_seg, it_next_seg);
      _unlink_rewrite_queue_node(it_next_seg->_queue_node);
      it_next_seg = _rewrite_segments.erase(it_next_seg);
    } else {
      it_seg = it_next_seg;
      ++it_next_seg;
    }
  }
}

void rescan_subtree_rewrite_cursor::
_join_segment_into(const _rewrite_segments_type::iterator &it_dst,
		   const _rewrite_segments_type::iterator &it_src)
{
  using mi_edit_script_op = rescan_macro_invocation_edit_script_op;

  it_dst->out_edit_script.reserve(it_dst->out_edit_script.size() + 1 +
				  it_src->out_edit_script.size());
  // Output tokens inbetween the two segments, if any, are not covered
  // by either edit script and thus, are going to be deleted. Separate
  // the joined sequences by an insert_ws op in this case, unless one
  // of the boundary tokens is whitespace already. Note that
  // insertion ops don't cover any output range and are to be skipped
  // in the search for the boundary tokens.
  const auto op_range_begin =
    [](const mi_edit_script_op &op, std::size_t &begin) {
      switch (op._op) {
      case mi_edit_script_op::op::copy:
	begin = op._copy.range.begin;
	return true;
      case mi_edit_script_op::op::replace_token:
	begin = op._replace_token.replaced_token_index;
	return true;
      case mi_edit_script_op::op::rewrite_macro_id:
	begin = op._rewrite_macro_id.rewritten_token_index;
	return true;
      case mi_edit_script_op::op::insert_token:
	/* fall through */
      case mi_edit_script_op::op::insert_ws:
	/* fall through */
      }
      return false;
    };
  const auto op_range_end =
    [](const mi_edit_script_op &op, std::size_t &end) {
      switch (op._op) {
      case mi_edit_script_op::op::copy:
	end = op._copy.range.end;
	return true;
      case mi_edit_script_op::op::replace_token:
	end = op._replace_token.replaced_token_index + 1;
	return true;
      case mi_edit_script_op::op::rewrite_macro_id:
	end = op._rewrite_macro_id.rewritten_token_index + 1;
	return true;
      case mi_edit_script_op::op::insert_token:
	/* fall through */
      case mi_edit_script_op::op::insert_ws:
	/* fall through */
      }
      return false;
    };
  bool need_separator = false;
  {
    std::size_t dst_end = 0;
    bool have_dst_end = false;
    for (auto it = it_dst->out_edit_script.rbegin();
	 it != it_dst->out_edit_script.rend() && !have_dst_end; ++it) {
      have_dst_end = op_range_end(*it, dst_end);
    }
    std::size_t src_begin = 0;
    bool have_src_begin = false;
    for (auto it = it_src->out_edit_script.begin();
	 it != it_src->out_edit_script.end() && !have_src_begin; ++it) {
      have_src_begin = op_range_begin(*it, src_begin);
    }
    if (have_dst_end && have_src_begin) {
      assert(dst_end <= src_begin);
      if (dst_end < src_begin) {
	const pp_tokens &out_toks = it_dst->mi.get_output().get_result();
	need_separator = (!(dst_end && out_toks[dst_end - 1].is_ws()) &&
			  !out_toks[src_begin].is_ws());
      }
    }
  }
  if (need_separator) {
    it_dst->out_edit_script.insert
      (it_dst->out_edit_script.end(),
       mi_edit_script_op{
	 mi_edit_script_op::insert_ws{ .stickiness = sticky_side::none }
       });
  }
  it_dst->rescanned_out_ranges.join_with
    (std::move(it_src->rescanned_out_ranges),
     it_dst->out_edit_script.size());
  const std::size_t head_segment_tokens_end
    = it_dst->out_edit_script.tokens_length();
  it_dst->out_edit_script.insert
    (it_dst->out_edit_script.end(),
     std::make_move_iterator(it_src->out_edit_script.begin()),
     std::make_move_iterator(it_src->out_edit_script.end()));
  if (!it_src->unexpanded_macro_id_out_tokens_origin_traces.empty() ||
      !it_src->rewritten_rescan_macro_invocation_children.empty() ||
      !it_src->descendant_rescan_counter_invocations.empty()) {
    it_dst->unexpanded_macro_id_out_tokens_origin_traces
      .join_with
	((std::move
	  (it_src->unexpanded_macro_id_out_tokens_origin_traces)),
	 head_segment_tokens_end);

    for (auto &c :
	   it_src->rewritten_rescan_macro_invocation_children) {
      c.out_token_index += head_segment_tokens_end;
    }
    it_dst->rewritten_rescan_macro_invocation_children.insert
      (it_dst->rewritten_rescan_macro_invocation_children.end(),
       (std::make_move_iterator
	(it_src->rewritten_rescan_macro_invocation_children.begin())),
       (std::make_move_iterator
	(it_src->rewritten_rescan_macro_invocation_children.end())));

    for (auto &i : it_src->descendant_rescan_counter_invocations) {
      i.out_token_index += head_segment_tokens_end;
    }
    it_dst->descendant_rescan_counter_invocations.insert
      (it_dst->descendant_rescan_counter_invocations.end(),
       (std::make_move_iterator
	(it_src->descendant_rescan_counter_invocations.begin())),
       (std::make_move_iterator
	(it_src->descendant_rescan_counter_invocations.end())));
  }
}

rescan_subtree_rewrite_cursor::_rewrite_segments_type::iterator
rescan_subtree_rewrite_cursor::
_merge_segments_with_same_mi(const _rewrite_segments_type::iterator &it_head)
{
  // Merge all segments associated with the same
  // rescan_macro_invocation as it_head into the first one of these
  // in list (that is, output) order. The result will be placed at
  // the rewrite queue head.
  const auto head_mi =
    static_cast<expansion_forest::nodes_size_type>(it_head->mi);
  _rewrite_segments_type::iterator it_first = _rewrite_segments.end();
  for (auto it = _rewrite_segments.begin(); it != _rewrite_segments.end();) {
    auto it_next = std::next(it);
    if (static_cast<expansion_forest::nodes_size_type>(it->mi) == head_mi) {
      if (it_first == _rewrite_segments.end()) {
	it_first = it;
      } else {
	assert(it_first->subtree_depth == it->subtree_depth);
	_join_segment_into(it_first, it);
	_unlink_rewrite_queue_node(it->_queue_node);
	_rewrite_segments.erase(it);
      }
    }
    it = it_next;
  }
  assert(it_first != _rewrite_segments.end());

  if (it_first != it_head) {
    // it_head had been merged into it_first and erased. Move
    // it_first to the rewrite queue's head.
    _unlink_rewrite_queue_node(it_first->_queue_node);
    _link_rewrite_queue_node_after(_rewrite_queue_head, it_first);
  }
  assert(_rewrite_queue_head == it_first);
  return it_first;
}


void rescan_subtree_rewrite_cursor::
_link_rewrite_queue_node_after(_rewrite_segments_type::iterator &prev_next,
			       const _rewrite_segments_type::iterator &node)
  noexcept
{
  assert(node->_queue_node.prev == nullptr);
  if (prev_next != _rewrite_segments.end())
    prev_next->_queue_node.prev = &node->_queue_node.next;
  node->_queue_node.next = prev_next;
  prev_next = node;
  node->_queue_node.prev = &prev_next;
}

void rescan_subtree_rewrite_cursor::
_unlink_rewrite_queue_node(_rewrite_segment::_rewrite_queue_node &node) noexcept
{
  if (node.prev == nullptr)
    return;

  if (node.next != _rewrite_segments.end())
    node.next->_queue_node.prev = node.prev;
  *node.prev = node.next;
  node.prev = nullptr;
  node.next = _rewrite_segments.end();
}

void rescan_subtree_rewrite_cursor::_prepare_rewrite_queue() noexcept
{
  // Order the rewrite queue by descending order of subtree_depth (as
  // those with largest depth need to get processed first). The sort
  // must be stable: segments of equal depth are to be processed in
  // output order.
  //
  // The queue is a linked order on top of the _rewrite_segments list,
  // which itself retains the output order and thus cannot get sorted
  // in place. Sort a vector of iterators instead and link the queue
  // nodes accordingly afterwards.
  std::vector<_rewrite_segments_type::iterator> order;
  order.reserve(_rewrite_segments.size());
  for (auto it_seg = _rewrite_segments.begin();
       it_seg != _rewrite_segments.end(); ++it_seg) {
    order.push_back(it_seg);
  }
  std::stable_sort(order.begin(), order.end(),
		   [](const _rewrite_segments_type::iterator &a,
		      const _rewrite_segments_type::iterator &b) {
		     return a->subtree_depth > b->subtree_depth;
		   });

  // Install the list at _rewrite_queue_head and set all nodes' ->prev
  // pointers for O(1) unlinking.
  _rewrite_queue_head = _rewrite_segments.end();
  _rewrite_segments_type::iterator *prev_next = &_rewrite_queue_head;
  for (const auto &it_seg : order) {
    *prev_next = it_seg;
    it_seg->_queue_node.prev = prev_next;
    prev_next = &it_seg->_queue_node.next;
  }
  *prev_next = _rewrite_segments.end();
}

template<typename erase_ops_cb_type>
void rescan_subtree_rewrite_cursor::
_normalize_mi_edit_script (rescan_macro_invocation_edit_script &edit_script,
			   erase_ops_cb_type &&erase_ops_cb)
{
  using mi_edit_script_op = rescan_macro_invocation_edit_script_op;

  const auto &is_empty_copy =
    [](const mi_edit_script_op &op) {
      return (op._op == mi_edit_script_op::op::copy &&
	      op._copy.range.length() == 0);
    };

  const auto &is_insert_ws =
    [](const mi_edit_script_op &op) {
      return op._op == mi_edit_script_op::op::insert_ws;
    };

  const auto &is_empty_copy_or_insert_ws =
    [&](const mi_edit_script_op &op) {
      return is_insert_ws(op) || is_empty_copy(op);
    };

  // First prune empty copy ops, possibly interspersed with insert_ws.
  auto empty_copies_begin =
    std::find_if(edit_script.cbegin(), edit_script.cend(), is_empty_copy);
  while (empty_copies_begin != edit_script.cend()) {
    // This is the end of a run of empty copies and insert_ws ops.
    auto succeeding_insert_ws_end =
      std::find_if_not(empty_copies_begin + 1,
		       edit_script.cend(),
		       is_empty_copy_or_insert_ws);
    auto preceding_insert_ws_begin =
      (std::find_if_not(std::make_reverse_iterator(empty_copies_begin),
			edit_script.crend(),
			is_insert_ws)
       .base());
    // Retain an insert_ws either from the tail or head, if there had
    // been any. insert_ws stickiness will be handled at the end of
    // the function.
    if ((succeeding_insert_ws_end - 1)->_op ==
	mi_edit_script_op::op::insert_ws) {
      --succeeding_insert_ws_end;
    } else if (preceding_insert_ws_begin != empty_copies_begin) {
      // No insert_ws at the tail, retain one at the head.
      ++preceding_insert_ws_begin;
    }
    erase_ops_cb
      (static_cast<const rescan_macro_invocation_edit_script&>(edit_script),
       preceding_insert_ws_begin, succeeding_insert_ws_end);
    empty_copies_begin = edit_script.erase(preceding_insert_ws_begin,
					   succeeding_insert_ws_end);
    empty_copies_begin =
      std::find_if(empty_copies_begin, edit_script.cend(), is_empty_copy);
  }

  // Now make the stickinesses of insert_token ops consistent: in a
  // run of insert_token (possibly interspersed with insert_ws), all
  // ops with a left stickiness must precede any with a right
  // stickiness.
  const auto &is_insert_token =
    [](const mi_edit_script_op &op) {
      return op._op == mi_edit_script_op::op::insert_token;
    };

  const auto &is_insert_token_or_insert_ws =
    [&](const mi_edit_script_op &op) {
      return is_insert_token(op) || is_insert_ws(op);
    };

  const auto &is_insert_token_with_stickiness =
    [&](const mi_edit_script_op &op, const sticky_side stickiness) {
      return (is_insert_token(op) &&
	      op._insert_token.stickiness == stickiness);
    };

  const auto &is_insert_ws_with_stickiness =
    [&](const mi_edit_script_op &op, const sticky_side stickiness) {
      return (is_insert_ws(op) &&
	      op._insert_ws.stickiness == stickiness);
    };

  auto insert_tokens_begin =
    std::find_if(edit_script.begin(), edit_script.end(), is_insert_token);
  while (insert_tokens_begin != edit_script.cend()) {
    // This is the end of a run of insert_token and insert_ws ops.
    auto succeeding_insert_ws_end =
      std::find_if_not(insert_tokens_begin + 1,
		       edit_script.end(),
		       is_insert_token_or_insert_ws);
    auto insert_tokens_end =
      (std::find_if(std::make_reverse_iterator(succeeding_insert_ws_end),
		    std::make_reverse_iterator(insert_tokens_begin),
		    is_insert_token)
       .base());
    auto preceding_insert_ws_begin =
      (std::find_if_not(std::make_reverse_iterator(insert_tokens_begin),
			edit_script.rend(),
			is_insert_ws)
       .base());

    const auto &set_range_stickiness =
      [&](const rescan_macro_invocation_edit_script::ops_iterator &begin,
	  const rescan_macro_invocation_edit_script::ops_iterator &end,
	  const sticky_side stickiness) {
	for (auto it = begin; it != end; ++it) {
	  switch (it->_op) {
	  case mi_edit_script_op::op::insert_token:
	    it->_insert_token.stickiness = stickiness;
	    break;
	  case mi_edit_script_op::op::insert_ws:
	    it->_insert_ws.stickiness = stickiness;
	    break;
	  case mi_edit_script_op::op::copy:
	    break;
	  case mi_edit_script_op::op::replace_token:
	    break;
	  case mi_edit_script_op::op::rewrite_macro_id:
	    break;
	  }
	}
      };

    // Find the maximal range having inversions, i.e. where
    // some insert_token with right stickiness is followed
    // by some other with left stickiness.
    rescan_macro_invocation_edit_script::ops_iterator
      stickiness_inversions_begin;
    if (preceding_insert_ws_begin != edit_script.begin()) {
      stickiness_inversions_begin =
	std::find_if(insert_tokens_begin, insert_tokens_end,
		     std::bind(is_insert_token_with_stickiness,
			       std::placeholders::_1,
			       sticky_side::right));
    } else {
      // It makes no sense to have a run with left stickiness at the
      // edit_script head, because there isn't anything to stick to.
      stickiness_inversions_begin = preceding_insert_ws_begin;
    }
    rescan_macro_invocation_edit_script::ops_iterator stickiness_inversions_end;
    if (succeeding_insert_ws_end != edit_script.end()) {
      stickiness_inversions_end =
	(std::find_if(std::make_reverse_iterator(insert_tokens_end),
		      std::make_reverse_iterator(stickiness_inversions_begin),
		      std::bind(is_insert_token_with_stickiness,
				std::placeholders::_1,
				sticky_side::left))
	 .base());
    } else {
      // It makes no sense to have a run with right stickiness at the
      // edit_script tail, because there isn't anything to stick to.
      stickiness_inversions_end = succeeding_insert_ws_end;
    }

    // Anything from the beginning up to the last insert_token with
    // left stickiness before the inversions area, if any, is forced
    // to left stickiness.
    const auto left_stickiness_end =
      (std::find_if(std::make_reverse_iterator(stickiness_inversions_begin),
		    std::make_reverse_iterator(preceding_insert_ws_begin),
		    std::bind(is_insert_token_with_stickiness,
			      std::placeholders::_1,
			      sticky_side::left))
       .base());
    set_range_stickiness(preceding_insert_ws_begin, left_stickiness_end,
			 sticky_side::left);

    // Likewise, anything from the first insert_token with right
    // stickiness after the inversions area, if any, up to the end is
    // forced to right stickiness.
    const auto right_stickiness_begin =
      std::find_if(stickiness_inversions_end,
		   succeeding_insert_ws_end,
		   std::bind(is_insert_token_with_stickiness,
			     std::placeholders::_1,
			     sticky_side::right));
    set_range_stickiness(right_stickiness_begin, succeeding_insert_ws_end,
			 sticky_side::right);

    // Force anything betweem left_stickiness_end and
    // right_stickiness_begin, which includes the regions of
    // inversions, to sticky_side::none. Note that this also handles
    // insert_ws after an all-left or before an all-right insert_token
    // sequence, which is desired.
    set_range_stickiness(left_stickiness_end, right_stickiness_begin,
			 sticky_side::none);

    insert_tokens_begin =
      std::find_if(succeeding_insert_ws_end, edit_script.end(),
		   is_insert_token);
  }

  // Finally collapse runs of multiple insert_ws tokens.
  auto insert_ws_begin = std::find_if(edit_script.begin(),
				      edit_script.end(),
				      is_insert_ws);
  while (insert_ws_begin != edit_script.end()) {
    auto insert_ws_end = std::find_if_not(insert_ws_begin + 1,
					  edit_script.end(),
					  is_insert_ws);
    if (insert_ws_begin == edit_script.begin() ||
	insert_ws_end == edit_script.end()) {
      // Trim.
      erase_ops_cb
	(static_cast<const rescan_macro_invocation_edit_script&>(edit_script),
	 insert_ws_begin, insert_ws_end);
      insert_ws_begin = edit_script.erase(insert_ws_begin, insert_ws_end);
      insert_ws_begin = std::find_if(insert_ws_begin, edit_script.end(),
				     is_insert_ws);
      continue;
    }

    if (insert_ws_end->_op != mi_edit_script_op::op::insert_token &&
	((insert_ws_begin - 1)->_op != mi_edit_script_op::op::insert_token)) {
      // No neighboring insert_token op, force stickiness to none.
      insert_ws_begin->_insert_ws.stickiness = sticky_side::none;
    } else {
      // There's some neighboring insert_token op, so the insert_ws
      // stickiness has been set correctly as per the insert_token
      // handling above.
      if (insert_ws_end->_op == mi_edit_script_op::op::insert_token) {
	if (insert_ws_end->_insert_token.stickiness == sticky_side::left) {
	  assert(((insert_ws_begin - 1)->_op !=
		  mi_edit_script_op::op::insert_token) ||
		 ((insert_ws_begin - 1)->_insert_token.stickiness ==
		  sticky_side::left));
	  assert(std::all_of(insert_ws_begin, insert_ws_end,
			     std::bind(is_insert_ws_with_stickiness,
				       std::placeholders::_1,
				       sticky_side::left)));
	} else {
	  assert(((insert_ws_begin - 1)->_op !=
		  mi_edit_script_op::op::insert_token) ||
		 ((insert_ws_begin - 1)->_insert_token.stickiness ==
		  sticky_side::right) ||
		 std::all_of(insert_ws_begin, insert_ws_end,
			     std::bind(is_insert_ws_with_stickiness,
				       std::placeholders::_1,
				       sticky_side::none)));
	}
      }
      if ((insert_ws_begin - 1)->_op == mi_edit_script_op::op::insert_token) {
	if ((insert_ws_begin - 1)->_insert_token.stickiness ==
	    sticky_side::right) {
	  assert(insert_ws_end->_op != mi_edit_script_op::op::insert_token ||
		 (insert_ws_end->_insert_token.stickiness ==
		  sticky_side::right));
	  assert(std::all_of(insert_ws_begin, insert_ws_end,
			     std::bind(is_insert_ws_with_stickiness,
				       std::placeholders::_1,
				       sticky_side::right)));
	} else {
	  assert(insert_ws_end->_op != mi_edit_script_op::op::insert_token ||
		 (insert_ws_end->_insert_token.stickiness ==
		  sticky_side::left) ||
		 std::all_of(insert_ws_begin, insert_ws_end,
			     std::bind(is_insert_ws_with_stickiness,
				       std::placeholders::_1,
				       sticky_side::none)));
	}
      }
    }

    if (insert_ws_begin + 1 != insert_ws_end) {
      erase_ops_cb
	(static_cast<const rescan_macro_invocation_edit_script&>(edit_script),
	 insert_ws_begin + 1, insert_ws_end);
      insert_ws_end = edit_script.erase(insert_ws_begin + 1, insert_ws_end);
    }
    insert_ws_begin =
      std::find_if(insert_ws_end + 1, edit_script.end(), is_insert_ws);
  }
}



// Thrown internally when a rewrite turns out to be not representable.
struct macro_rewriting_failure
{
  std::string msg;
};

namespace
{
  using mi_edit_script_op = rescan_macro_invocation_edit_script_op;
  using mi_edit_script = rescan_macro_invocation_edit_script;
  using rrmi_index = rewritten_expansion_forest::nodes_size_type;
  using trace_index =
    rewritten_expansion_forest::unexpanded_macro_id_out_tokens::size_type;
  using original_node = expansion_forest::nodes_size_type;
  using out_fragment = rescan_macro_invocation::output::fragment;

  // Visitor extracting the referenced argument index from a concat
  // sequence component, if any.
  struct concat_component_arg_index
  {
    std::size_t operator()(const out_fragment::repl_tag&) const noexcept
    { return std::numeric_limits<std::size_t>::max(); }

    std::size_t operator()(const out_fragment::arg_tag&,
			   const out_fragment::arg_ref &a) const noexcept
    { return a.get_arg_index(); }

    std::size_t operator()(const out_fragment::stringification_tag&,
			   const out_fragment::stringification &s)
      const noexcept
    { return s.get_arg().get_arg_index(); }

    std::size_t operator()(const out_fragment::conditional_comma_tag&,
			   const bool) const noexcept
    { return std::numeric_limits<std::size_t>::max(); }
  };

  bool pp_tokens_equal(const pp_token &a, const pp_token &b) noexcept
  {
    return a.get_type() == b.get_type() && a.get_value() == b.get_value();
  }

  bool pp_token_seqs_equal(const std::vector<pp_token> &a,
			   const std::vector<pp_token> &b) noexcept
  {
    return std::equal(a.begin(), a.end(), b.begin(), b.end(), pp_tokens_equal);
  }

  pp_token normalize_src_token(const pp_token &tok)
  {
    // Newlines in macro arguments act as whitespace.
    if (tok.is_newline())
      return pp_token{pp_token::type::ws, " "};
    return tok;
  }

  // Emit the material preceding a wrapped identifier, c.f.
  // rescan_macro_invocation_rewriter::_impl::wrap_edit: the pre
  // tokens and, for a non-empty prefix, 'prefix ##'. The prefix is
  // the head of an identifier and thus, an identifier itself.
  void append_wrap_pre(const std::vector<pp_token> &pre,
		       const std::string &prefix,
		       std::vector<rewritten_macro_token> &dst)
  {
    for (const auto &t : pre)
      dst.emplace_back(normalize_src_token(t));
    if (!prefix.empty()) {
      dst.emplace_back(pp_token{pp_token::type::id, prefix});
      dst.emplace_back(pp_token{pp_token::type::punctuator, "##"},
		       rewritten_macro_token::pp_operator_tag{});
    }
  }

  // Emit the material following a wrapped identifier: for a non-empty
  // suffix '## suffix', then the post tokens. The suffix is the tail
  // of an identifier, which may well start with a digit.
  void append_wrap_post(const std::string &suffix,
			const std::vector<pp_token> &post,
			std::vector<rewritten_macro_token> &dst)
  {
    if (!suffix.empty()) {
      dst.emplace_back(pp_token{pp_token::type::punctuator, "##"},
		       rewritten_macro_token::pp_operator_tag{});
      const bool is_num =
	std::isdigit(static_cast<unsigned char>(suffix.front()));
      dst.emplace_back(pp_token{(is_num ?
				 pp_token::type::pp_number :
				 pp_token::type::id),
				suffix});
    }
    for (const auto &t : post)
      dst.emplace_back(normalize_src_token(t));
  }

  rewritten_macro_definition_id
  rewritten_m_of_rrmi(rewritten_expansion_forest &forest,
		      const rrmi_index rrmi)
  {
    const auto rewritten_m =
      forest.get_rewritten_rescan_macro_invocation(rrmi)
      .get_rewritten_macro();
    assert(rewritten_m != invalid_rewritten_macro_definition_id);
    return rewritten_m;
  }

  // Collect the source tokens of a rescan macro invocation (or
  // argument tree root) from wherever they were emitted.
  std::vector<pp_token>
  collect_src_tokens(const rewritten_expansion_forest &forest,
		     const rescan_macro_invocation &node,
		     const bool cross_arg_tree_roots)
  {
    std::vector<pp_token> toks;
    const auto &inv_src =
      forest.get_macro_invocation().get_expansion_forest()
      .get_invocation_source();
    const raw_pp_tokens &raw_toks = forest.get_pp_result().get_raw_tokens();
    const auto src_end = node.src_end(cross_arg_tree_roots);
    for (auto it = node.src_begin(cross_arg_tree_roots); it != src_end; ++it) {
      const rescan_output_segment seg = *it;
      const auto emitting = seg.get_emitting_node();
      const auto r = seg.get_out_range_in_emitting_node();
      if (emitting.is_nil_node()) {
	const auto segs = inv_src.overlapping_segments(r);
	for (auto it_s = segs.first; it_s != segs.second; ++it_s) {
	  const auto s = *it_s;
	  const std::size_t b = std::max(r.begin, s.invocation_range.begin);
	  const std::size_t e = std::min(r.end, s.invocation_range.end);
	  for (std::size_t pos = b; pos < e; ++pos) {
	    const raw_pp_token &rt =
	      raw_toks[s.raw_pp_tokens_begin + (pos - s.invocation_range.begin)];
	    toks.push_back(normalize_src_token(pp_token{rt.get_type(),
							rt.get_value()}));
	  }
	}
      } else {
	const pp_tokens &out = emitting.get_output().get_result();
	for (std::size_t pos = r.begin; pos < r.end; ++pos)
	  toks.push_back(normalize_src_token(out[pos]));
      }
    }
    return toks;
  }

  // Whether a token is a non-keyword identifier. Those are what the
  // live patch writer's edits target (renames, externalizations) and
  // thus, are the most valuable to keep in a macro invocation's
  // argument rather than in a rewritten definition: a rewritten
  // definition mentioning some specific identifier is unlikely to be
  // reusable for other invocations.
  bool is_nonkeyword_id(const pp_token &tok) noexcept
  {
    if (!tok.is_id())
      return false;
    static const char * const keywords[] = {
      "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic",
      "_Noreturn", "_Static_assert", "_Thread_local",
      "__alignof", "__alignof__", "__asm", "__asm__", "__attribute",
      "__attribute__", "__auto_type", "__builtin_choose_expr",
      "__builtin_offsetof", "__builtin_types_compatible_p",
      "__builtin_va_arg", "__complex__", "__const", "__const__",
      "__inline", "__inline__", "__label__", "__restrict", "__restrict__",
      "__signed__", "__typeof", "__typeof__", "__volatile", "__volatile__",
      "asm", "auto", "break", "case", "char", "const", "continue",
      "default", "do", "double", "else", "enum", "extern", "float", "for",
      "goto", "if", "inline", "int", "long", "register", "restrict",
      "return", "short", "signed", "sizeof", "static", "struct", "switch",
      "typedef", "typeof", "union", "unsigned", "void", "volatile",
      "while",
    };
    const std::string &v = tok.get_value();
    return !std::binary_search(std::begin(keywords), std::end(keywords), v,
			       [](const std::string &a, const std::string &b) {
				 return a < b;
			       });
  }

  // Whether an argument's expansion tree is free of any macro
  // invocations, i.e. whether the argument's expansion result equals
  // its source token by token.
  bool arg_tree_is_macro_free(const expansion_forest::rescan_subtree &arg_tree)
    noexcept
  {
    for (auto it = arg_tree.all_nodes_begin(); it != arg_tree.all_nodes_end();
	 ++it) {
      const rescan_macro_invocation node = *it;
      if (!node.is_arg_tree_root())
	return false;
    }
    return true;
  }

  // The "universality" of the tokens in a macro invocation argument's
  // expansion result: for each token, the number of replacement lists
  // it emerged from on its way to the argument's final output. Tokens
  // written directly in the argument's source have universality
  // zero. A token passed through some macro invocation as (part of)
  // its argument retains its universality, whereas a token emitted
  // from the replacement list of some invocation gets the
  // universality of that invocation's name token plus one. A
  // concatenation result's universality is the minimum of its
  // constituents'.
  //
  // Example: with '#define M(x) N(x) baz' and '#define N(x) qux x', the
  // argument 'M(bar)' expands to 'qux bar baz' with universalities 2,
  // 0 and 1.
  //
  // The rationale is that tokens of higher universality are more
  // likely to be the same across different invocations of the macro
  // whose argument is being examined: a rewritten definition which
  // retains those but leaves the specific, low universality tokens to
  // the argument has a higher chance of getting reused.
  class arg_out_universality
  {
  public:
    arg_out_universality(const expansion_forest::rescan_subtree &arg_tree);

    const std::vector<unsigned>& get() const noexcept
    { return _u; }

  private:
    // Universality of the token at position pos of node's output. For
    // argument tree roots, the "output" is the unexpanded argument
    // source.
    unsigned _univ_out(const rescan_macro_invocation &node,
		       const std::size_t pos);

    // Universality of a non-root invocation's name token.
    unsigned _univ_name(const rescan_macro_invocation &node);

    // Universality of the token at position pos within some argument
    // tree's final output.
    unsigned _univ_arg_tree_out(const expansion_forest::rescan_subtree &tree,
				const std::size_t pos);

    rescan_macro_invocation _main_root;
    std::map<std::pair<original_node, std::size_t>, unsigned> _out_memo;
    std::map<original_node, unsigned> _name_memo;
    std::vector<unsigned> _u;
  };

  arg_out_universality::
  arg_out_universality(const expansion_forest::rescan_subtree &arg_tree)
    : _main_root{static_cast<rescan_macro_invocation>(arg_tree)}
  {
    for (auto it = arg_tree.final_out_begin(); it != arg_tree.final_out_end();
	 ++it) {
      const rescan_output_segment seg = *it;
      const auto emitting = seg.get_emitting_node();
      const auto &r = seg.get_out_range_in_emitting_node();
      for (std::size_t q = r.begin; q < r.end; ++q)
	_u.push_back(_univ_out(emitting, q));
    }
  }

  unsigned arg_out_universality::_univ_out(const rescan_macro_invocation &node,
					    const std::size_t pos)
  {
    if (node.is_arg_tree_root()) {
      // The argument's own source tokens.
      if (static_cast<original_node>(node) ==
	  static_cast<original_node>(_main_root)) {
	return 0;
      }
      // A nested argument tree's source is located in the owner's
      // rescan parent's output.
      const auto owner = node.get_arg_tree_owner();
      return _univ_out(owner.get_rescan_parent(),
		       node.get_src_range().begin + pos);
    }

    const auto key = std::make_pair(static_cast<original_node>(node), pos);
    const auto it_memo = _out_memo.find(key);
    if (it_memo != _out_memo.end())
      return it_memo->second;

    const auto &out = node.get_output();
    const auto frags =
      out.overlapping_fragments(rescan_macro_invocation::output::tokens_range{
				  pos, pos + 1
				});
    assert(frags.first != frags.second);
    const out_fragment &f = *frags.first;
    unsigned u = 0;
    switch (f.get_op()) {
    case out_fragment::op::repl:
      /* fall through */
    case out_fragment::op::stringification:
      /* fall through */
    case out_fragment::op::conditional_comma:
      /* fall through */
    case out_fragment::op::builtin_special:
      // Emerged from this invocation's replacement list (or created
      // by it).
      u = _univ_name(node) + 1;
      break;

    case out_fragment::op::empty:
      // Placeholders never get weighed.
      u = 0;
      break;

    case out_fragment::op::arg:
      // Passed through from the (pre-expanded) argument.
      u = _univ_arg_tree_out
	(node.get_arg_tree(f.get_arg_ref_fragment().get_arg_index()),
	 pos - f.get_out_range().begin);
      break;

    case out_fragment::op::concat:
      {
	// Minimum over the constituents. Tokens originating from
	// arguments are the unexpanded ones here, located in this
	// invocation's rescan parent's output.
	const auto univ_unexpanded_arg_tok =
	  [&](const std::size_t arg_index, const std::size_t k) {
	    const auto arg_root =
	      static_cast<rescan_macro_invocation>(node.get_arg_tree(arg_index));
	    return _univ_out(arg_root, k);
	  };
	using cs_out_fragment = out_fragment::concat_sequence::output_fragment;
	const auto &cs = f.get_concat_sequence_fragment();
	const std::vector<cs_out_fragment> cs_frags = cs.get_output_fragments(node);
	const std::size_t rel = pos - f.get_out_range().begin;
	const cs_out_fragment *cf = nullptr;
	for (const auto &c : cs_frags) {
	  if (c.get_out_begin() <= rel)
	    cf = &c;
	  else
	    break;
	}
	assert(cf);
	switch (cf->get_kind()) {
	case cs_out_fragment::kind::concatenated_tokens:
	  {
	    u = std::numeric_limits<unsigned>::max();
	    const auto &ct = cf->get_concatenated_tokens_fragment();
	    for (const auto &o : ct.get_origins()) {
	      switch (o.get_origin()) {
	      case cs_out_fragment::concatenated_tokens::
		concatenated_token_origin::origin::repl:
		u = std::min(u, _univ_name(node) + 1);
		break;
	      case cs_out_fragment::concatenated_tokens::
		concatenated_token_origin::origin::arg:
		{
		  const auto &a = o.get_arg_origin();
		  u = std::min(u,
			       univ_unexpanded_arg_tok
			       (a.arg.get_arg_index(),
				a.concatenated_arg_token_index));
		}
		break;
	      }
	    }
	    if (u == std::numeric_limits<unsigned>::max())
	      u = _univ_name(node) + 1;
	  }
	  break;

	case cs_out_fragment::kind::arg_tokens:
	  {
	    const auto &at = cf->get_arg_tokens_fragment();
	    u = univ_unexpanded_arg_tok(at.arg.get_arg_index(),
					at.arg_tokens.begin +
					(rel - cf->get_out_begin()));
	  }
	  break;

	case cs_out_fragment::kind::stringification:
	  /* fall through */
	case cs_out_fragment::kind::conditional_comma:
	  u = _univ_name(node) + 1;
	  break;
	}
      }
      break;
    }

    _out_memo.emplace(key, u);
    return u;
  }

  unsigned arg_out_universality::_univ_name(const rescan_macro_invocation &node)
  {
    assert(!node.is_arg_tree_root());
    const auto key = static_cast<original_node>(node);
    const auto it_memo = _name_memo.find(key);
    if (it_memo != _name_memo.end())
      return it_memo->second;
    // The name token is located in the rescan parent's output at the
    // beginning of this invocation's source range.
    const unsigned u = _univ_out(node.get_rescan_parent(),
				 node.get_src_range().begin);
    _name_memo.emplace(key, u);
    return u;
  }

  unsigned arg_out_universality::
  _univ_arg_tree_out(const expansion_forest::rescan_subtree &tree,
		     const std::size_t pos)
  {
    auto it = tree.final_out_begin();
    const std::size_t rem = it.skip_out_distance(pos);
    assert(it != tree.final_out_end());
    const rescan_output_segment seg = *it;
    return _univ_out(seg.get_emitting_node(),
		     seg.get_out_range_in_emitting_node().begin + rem);
  }

  // Materialize the result of applying a source edit script to the
  // source tokens it refers to.
  std::vector<rewritten_macro_token>
  materialize_src_edit(rewritten_expansion_forest &forest,
		       const mi_edit_script &script,
		       const std::vector<pp_token> &src_toks)
  {
    std::vector<rewritten_macro_token> result;
    for (const auto &op : script) {
      switch (op._op) {
      case mi_edit_script_op::op::copy:
	assert(op._copy.range.end <= src_toks.size());
	for (std::size_t p = op._copy.range.begin; p < op._copy.range.end; ++p) {
	  // Placeholder empty tokens from some enclosing invocation's
	  // output don't contribute anything.
	  if (src_toks[p].is_empty())
	    continue;
	  result.emplace_back(src_toks[p]);
	}
	break;

      case mi_edit_script_op::op::insert_token:
	result.emplace_back(normalize_src_token(op._insert_token.token));
	break;

      case mi_edit_script_op::op::insert_ws:
	result.emplace_back(pp_token{pp_token::type::ws, " "});
	break;

      case mi_edit_script_op::op::replace_token:
	for (const auto &t : op._replace_token.replacement)
	  result.emplace_back(normalize_src_token(t));
	break;

      case mi_edit_script_op::op::rewrite_macro_id:
	assert(op._rewrite_macro_id.rewritten_token_index < src_toks.size());
	result.emplace_back
	  (src_toks[op._rewrite_macro_id.rewritten_token_index],
	   rewritten_m_of_rrmi(forest,
		       op._rewrite_macro_id.rewritten_rescan_macro_invocation));
	break;

      }
    }
    return result;
  }

  void finalize_trace(rewritten_expansion_forest &forest,
		      const trace_index t,
		      const rrmi_index context)
  {
    forest.get_unexpanded_macro_id_out_token(t).set_context(context);
  }
}


rescan_macro_invocation_rewriter::rewrite_progress::
rewrite_progress(std::unique_ptr<rescan_subtree_rewrite_cursor> &&cursor)
  noexcept
  : _cursor{std::move(cursor)}
{}

rescan_macro_invocation_rewriter::rewrite_progress::
rewrite_progress(result &&r) noexcept
  : _cursor{nullptr}, _result{std::move(r)}
{}

rescan_macro_invocation_rewriter::rewrite_progress::
rewrite_progress(rewrite_progress &&rhs) noexcept
  : _cursor{std::move(rhs._cursor)}, _result{std::move(rhs._result)}
{}

rescan_macro_invocation_rewriter::rewrite_progress::
~rewrite_progress() noexcept = default;

std::unique_ptr<rescan_subtree_rewrite_cursor>
rescan_macro_invocation_rewriter::rewrite_progress::
grab_pending_arg_tree_rewrite() noexcept
{
  return std::move(_cursor);
}

rescan_macro_invocation_rewriter::rewrite_progress::result
rescan_macro_invocation_rewriter::rewrite_progress::grab_result() noexcept
{
  return std::move(_result);
}


struct rescan_macro_invocation_rewriter::_impl
{
  // Edit applied to a single token of the rescan macro invocation's
  // output.
  struct token_edit
  {
    enum class kind
    {
      kept,
      deleted,
      replaced,
      rewrite_macro_id,
    };

    token_edit() noexcept
      : k{kind::deleted}, rrmi{rrmi_index::invalid()}
    {}

    bool operator==(const token_edit &rhs) const noexcept
    {
      if (k != rhs.k)
	return false;
      switch (k) {
      case kind::kept:
	/* fall through */
      case kind::deleted:
	return true;
      case kind::replaced:
	return pp_token_seqs_equal(replacement, rhs.replacement);
      case kind::rewrite_macro_id:
	return rrmi == rhs.rrmi;
      }
      return false;
    }

    bool operator!=(const token_edit &rhs) const noexcept
    { return !(*this == rhs); }

    kind k;
    std::vector<pp_token> replacement;
    rrmi_index rrmi;
    // Traces attached to the result tokens, as pairs of (index within
    // the produced tokens, trace index).
    std::vector<std::pair<std::size_t, trace_index>> traces;
  };

  // Insertion at some gap inbetween the rescan macro invocation's
  // output tokens.
  struct insertion
  {
    enum class kind
    {
      token,
      ws,
    };

    bool operator==(const insertion &rhs) const noexcept
    {
      return (k == rhs.k && stickiness == rhs.stickiness &&
	      pp_token_seqs_equal(toks, rhs.toks));
    }

    bool operator!=(const insertion &rhs) const noexcept
    { return !(*this == rhs); }

    kind k;
    std::vector<pp_token> toks;
    sticky_side stickiness;
    std::vector<std::pair<std::size_t, trace_index>> traces;
  };

  // One of the rescan macro invocation's output fragments and the
  // gap insertions attributed to its head and tail.
  struct frag_info
  {
    // The fragment's output range [b(), e()).
    std::size_t b() const noexcept
    { return f->get_out_range().begin; }

    std::size_t e() const noexcept
    { return f->get_out_range().end; }

    const out_fragment *f;
    std::vector<insertion> head_ins;
    std::vector<insertion> tail_ins;
  };

  // The edits applying to one particular (expanded) reference to a
  // macro argument, relative to the argument's expansion.
  struct arg_ref_edit
  {
    std::vector<token_edit> tok_edits;
    std::vector<std::vector<insertion>> gaps;
  };

  // An edit replacing an identifier token by a sequence of the form
  // 'pre w post', where w = prefix + id + suffix is the only
  // identifier therein, id being the original token's value. Such an
  // edit can be implemented without placing the identifier itself
  // into the replacement list: the pre and post tokens get wrapped
  // around the identifier's source and a non-empty prefix resp.
  // suffix gets pasted onto it by means of a '##' operator. This
  // applies to parameter references, c.f. the argument analysis in
  // analyze(), as well as to the results of '##' concatenations,
  // c.f. finalize().
  struct wrap_edit
  {
    bool valid = false;
    std::vector<pp_token> pre;
    std::string prefix;
    std::string suffix;
    std::vector<pp_token> post;
    std::vector<std::pair<std::size_t, trace_index>> traces;
  };
  static wrap_edit detect_wrap_edit(const pp_token &orig,
				    const token_edit &te);

  // Per macro parameter: the fragments referencing it and how each
  // expanded reference is to be served. See the argument analysis in
  // analyze() for the model: a contiguous range X of the argument's
  // expansion result is retained in the (rewritten) argument, the
  // references agreeing on the edits within X are served through the
  // parameter with any edited material outside of X placed next to
  // the parameter reference in the replacement list, all other
  // references get lowered or replaced verbatim.
  struct arg_info
  {
    static constexpr std::size_t none = std::numeric_limits<std::size_t>::max();

    // Additional material needed in the replacement list around the
    // parameter reference for a reference served through the
    // parameter: its edited expansion result outside of X and/or a
    // wrap or paste around X's boundary tokens (c.f. wrap_edit in
    // analyze()).
    struct served_ref_material
    {
      // _arg_rewrites indices of the lowering rewrites for the
      // portions left resp. right of X, if lowered.
      std::size_t left_lower = none;
      std::size_t right_lower = none;
      // Whether the respective portion is to be emitted verbatim
      // instead, because its position is not subject to rescan.
      bool left_verbatim = false;
      bool right_verbatim = false;
      // Tokens wrapped around the X's boundary tokens: wrap_pre
      // follows the left portion, wrap_post precedes the right
      // portion. A non-empty paste_prefix is to be pasted onto X's
      // first token by means of a '##' operator placed inbetween
      // wrap_pre and the parameter reference, a paste_suffix onto X's
      // last token likewise.
      std::vector<pp_token> wrap_pre;
      std::string paste_prefix;
      std::string paste_suffix;
      std::vector<pp_token> wrap_post;
      // Traces of the replacement tokens which got turned into the
      // wrap/paste material.
      std::vector<trace_index> wrap_traces;
      // Whether X's first resp. last token's edit is implemented
      // through the wrap.
      bool wrap_first = false;
      bool wrap_last = false;
    };

    std::vector<std::size_t> expanded_arg_refs;
    std::vector<std::size_t> operator_arg_refs;
    // Expanded references whose output got deleted completely, c.f.
    // analyze(). They're removed from the replacement list and not
    // part of expanded_arg_refs.
    std::vector<std::size_t> deleted_arg_refs;
    // Expanded references whose output is consumed as an argument by
    // a rescan child which never references the corresponding
    // parameter, c.f. analyze(). Their content doesn't matter: they
    // stay as plain parameter references if anything is served
    // through the parameter and are not part of expanded_arg_refs.
    std::vector<std::size_t> dont_care_arg_refs;
    // The edits at each expanded reference, relative to the
    // argument's expansion result (parallel to expanded_arg_refs).
    std::vector<arg_ref_edit> ref_edits;
    // The retained range X, relative to the argument's expansion
    // result, and whether it extends to the argument's head resp. tail
    // (modulo placeholder empty tokens).
    std::size_t x_begin = 0;
    std::size_t x_end = 0;
    bool x_head = true;
    bool x_tail = true;
    // The edits to push down into the argument's expansion tree: the
    // served references' common edits within X, deletions outside of
    // it.
    arg_ref_edit common;
    bool common_identity = true;
    // Whether any expanded reference is served through the parameter
    // (as opposed to getting lowered or replaced verbatim).
    bool any_ref_kept = false;
    // Index into _impl::_arg_rewrites of the rewrite pushing the
    // common edits (or a forced rename) down into the argument's
    // expansion tree, if any. Its result's source edit script replaces
    // the argument's source in the rewritten invocation. As the
    // argument's source changes then, any #/## references' output
    // would change as well and these need to get replaced verbatim.
    std::size_t _arg_rewrites_index = none;
    // Served references needing material next to the parameter
    // reference: frag index -> material.
    std::map<std::size_t, served_ref_material> served_material;
    // Lowered references: frag index -> arg_rewrites index
    std::map<std::size_t, std::size_t> lower_rewrites;
    // Fragments whose reference gets replaced by the (edited)
    // expansion result verbatim, because the reference's position is
    // not subject to rescan and lowering the argument's source would
    // never get it expanded.
    std::vector<std::size_t> verbatim_arg_refs;
  };

  // A queued rewrite of an argument's expansion tree, either for
  // pushing down the common edits or for lowering one reference's
  // edits, to be run by a separate rescan_subtree_rewrite_cursor.
  struct arg_rewrite
  {
    std::size_t arg_index;
    mi_edit_script script;
    unexpanded_macro_id_out_tokens_origin_traces traces;
    rescan_subtree_rewrite_cursor::result result;
    bool started = false;
  };

  struct mat_token
  {
    rewritten_macro_token t;
    std::vector<trace_index> traces;
  };

  _impl(rewritten_expansion_forest &forest,
	rewritten_expansion_forest::rewritten_rescan_macro_invocation &&rrmi,
	mi_edit_script &&out_edit_script,
	rescan_macro_invocation_edit_script_rescanned_ranges
	  &&out_rescanned_ranges,
	unexpanded_macro_id_out_tokens_origin_traces &&traces,
	std::vector<rewritten_rescan_macro_invocation_child> &&children,
	std::vector<descendant_rescan_counter_invocation> &&counters);

  rewrite_progress continue_rewrite();

  void analyze();
  arg_ref_edit extract_arg_ref_edit(const frag_info &fi) const;
  mi_edit_script arg_ref_edit_to_script
    (const arg_ref_edit &re,
     unexpanded_macro_id_out_tokens_origin_traces &out_traces) const;
  void emit_insertion(const insertion &ins, std::vector<mat_token> &dst) const;
  void emit_token_edit(const token_edit &te, const std::size_t p,
		       std::vector<mat_token> &dst) const;
  std::vector<mat_token> materialize(const frag_info &fi,
				     const bool include_head,
				     const bool include_tail) const;
  std::vector<mat_token> materialize_portion(const frag_info &fi,
					     const arg_ref_edit &re,
					     const std::size_t p_begin,
					     const std::size_t p_end,
					     const std::size_t g_begin,
					     const std::size_t g_end) const;
  std::vector<rewritten_macro_token>
  finalize_mat_tokens(std::vector<mat_token> &&mat,
		      const rrmi_index context) const;
  void check_representable(const std::vector<rewritten_macro_token> &toks)
    const;
  rewrite_progress::result finalize();

  rewritten_expansion_forest &_forest;
  rewritten_expansion_forest::rewritten_rescan_macro_invocation _rrmi;
  rescan_macro_invocation _rmi;
  mi_edit_script _out_edit_script;
  rescan_macro_invocation_edit_script_rescanned_ranges _out_rescanned_ranges;
  unexpanded_macro_id_out_tokens_origin_traces _traces;
  std::vector<rewritten_rescan_macro_invocation_child> _children;
  std::vector<descendant_rescan_counter_invocation> _counters;

  bool _analyzed = false;
  std::vector<token_edit> _tok_edits;
  std::vector<std::vector<insertion>> _gap_insertions;
  std::vector<frag_info> _frags;
  std::vector<arg_info> _args;
  std::vector<std::pair<raw_pp_token_index, insertion>> _def_level_insertions;
  std::vector<arg_rewrite> _arg_rewrites;
  std::size_t _next_arg_rewrite = 0;
  bool _whole_output_deleted = false;
};

rescan_macro_invocation_rewriter::_impl::
_impl(rewritten_expansion_forest &forest,
      rewritten_expansion_forest::rewritten_rescan_macro_invocation &&rrmi,
      mi_edit_script &&out_edit_script,
      rescan_macro_invocation_edit_script_rescanned_ranges
	&&out_rescanned_ranges,
      unexpanded_macro_id_out_tokens_origin_traces &&traces,
      std::vector<rewritten_rescan_macro_invocation_child> &&children,
      std::vector<descendant_rescan_counter_invocation> &&counters)
  : _forest{forest}, _rrmi{std::move(rrmi)},
    _rmi{forest.get_original_forest()[_rrmi.get_original()]},
    _out_edit_script{std::move(out_edit_script)},
    _out_rescanned_ranges{std::move(out_rescanned_ranges)},
    _traces{std::move(traces)},
    _children{std::move(children)},
    _counters{std::move(counters)}
{}

rescan_macro_invocation_rewriter::_impl::wrap_edit
rescan_macro_invocation_rewriter::_impl::
detect_wrap_edit(const pp_token &orig, const token_edit &te)
{
  wrap_edit w;
  if (te.k != token_edit::kind::replaced || !orig.is_id())
    return w;
  const std::size_t none = std::numeric_limits<std::size_t>::max();
  std::size_t id_pos = none;
  for (std::size_t k = 0; k < te.replacement.size(); ++k) {
    if (te.replacement[k].is_id()) {
      if (id_pos != none)
	return w;
      id_pos = k;
    }
  }
  if (id_pos == none)
    return w;
  const std::string &wv = te.replacement[id_pos].get_value();
  const std::string &id = orig.get_value();
  const auto found = wv.find(id);
  if (found == std::string::npos)
    return w;
  w.prefix = wv.substr(0, found);
  w.suffix = wv.substr(found + id.size());
  w.pre.assign(te.replacement.begin(), te.replacement.begin() + id_pos);
  w.post.assign(te.replacement.begin() + id_pos + 1, te.replacement.end());
  w.traces = te.traces;
  w.valid = true;
  return w;
}

void rescan_macro_invocation_rewriter::_impl::analyze()
{
  const auto &out = _rmi.get_output();
  const pp_tokens &out_toks = out.get_result();
  const std::size_t L = out_toks.size();
  assert(L == out.out_length());

  _tok_edits.assign(L, token_edit{});
  _gap_insertions.assign(L + 1, std::vector<insertion>{});

  // Distribute the edit script onto the per-token / per-gap
  // representation.
  auto it_trace = _traces.begin();
  const auto take_traces =
    [&](const std::size_t t_begin, const std::size_t n) {
      std::vector<std::pair<std::size_t, trace_index>> r;
      while (it_trace != _traces.end() &&
	     it_trace->traced_out_token_index < t_begin) {
	// Should not happen, but skip stale entries for robustness.
	++it_trace;
      }
      while (it_trace != _traces.end() &&
	     it_trace->traced_out_token_index < t_begin + n) {
	r.emplace_back(it_trace->traced_out_token_index - t_begin,
		       it_trace->unexpanded_macro_id_out_token);
	++it_trace;
      }
      return r;
    };

  std::size_t t = 0;
  std::size_t prev_end = 0;
  bool have_prev = false;
  std::vector<insertion> pending;
  const auto flush_pending =
    [&](const std::size_t next_begin, const bool at_end) {
      for (auto &ins : pending) {
	std::size_t gap;
	switch (ins.stickiness) {
	case sticky_side::left:
	  gap = have_prev ? prev_end : next_begin;
	  break;
	case sticky_side::right:
	  gap = !at_end ? next_begin : (have_prev ? prev_end : L);
	  break;
	case sticky_side::none:
	  /* fall through */
	default:
	  gap = have_prev ? prev_end : next_begin;
	  break;
	}
	assert(gap <= L);
	_gap_insertions[gap].push_back(std::move(ins));
      }
      pending.clear();
    };

  // The input script and its rescanned ranges are needed only for
  // this distribution, consume them.
  const mi_edit_script out_edit_script = std::move(_out_edit_script);
  _out_edit_script = mi_edit_script{};
  const auto out_rescanned_ranges = std::move(_out_rescanned_ranges);
  _out_rescanned_ranges = rescan_macro_invocation_edit_script_rescanned_ranges{};
  // Whether tokens placed at an output position would be subject to
  // macro expansion: either because the position is in this
  // invocation's own output or because it is within the source of
  // some rescan child's argument which the child references through
  // its parameter (and thus, gets pre-expanded).
  std::vector<bool> tok_rescanned(L, false);
  std::size_t op_index = 0;
  for (const auto &op : out_edit_script) {
    const bool op_rescanned = out_rescanned_ranges.is_covered(op_index++);
    switch (op._op) {
    case mi_edit_script_op::op::copy:
      {
	const auto &r = op._copy.range;
	assert(r.end <= L);
	assert(!have_prev || r.begin >= prev_end);
	flush_pending(r.begin, false);
	for (std::size_t p = r.begin; p < r.end; ++p) {
	  _tok_edits[p].k = token_edit::kind::kept;
	  _tok_edits[p].traces = take_traces(t + (p - r.begin), 1);
	  tok_rescanned[p] = op_rescanned;
	}
	t += r.length();
	prev_end = r.end;
	have_prev = true;
      }
      break;

    case mi_edit_script_op::op::replace_token:
      {
	const std::size_t p = op._replace_token.replaced_token_index;
	assert(p < L);
	assert(!have_prev || p >= prev_end);
	flush_pending(p, false);
	auto &te = _tok_edits[p];
	te.k = token_edit::kind::replaced;
	tok_rescanned[p] = op_rescanned;
	te.replacement = op._replace_token.replacement;
	te.traces = take_traces(t, te.replacement.size());
	t += te.replacement.size();
	prev_end = p + 1;
	have_prev = true;
      }
      break;

    case mi_edit_script_op::op::rewrite_macro_id:
      {
	const std::size_t p = op._rewrite_macro_id.rewritten_token_index;
	assert(p < L);
	assert(!have_prev || p >= prev_end);
	flush_pending(p, false);
	auto &te = _tok_edits[p];
	te.k = token_edit::kind::rewrite_macro_id;
	tok_rescanned[p] = op_rescanned;
	te.rrmi = op._rewrite_macro_id.rewritten_rescan_macro_invocation;
	t += 1;
	prev_end = p + 1;
	have_prev = true;
      }
      break;

    case mi_edit_script_op::op::insert_token:
      pending.push_back(insertion{
	  insertion::kind::token,
	  {op._insert_token.token},
	  op._insert_token.stickiness,
	  take_traces(t, 1)
	});
      t += 1;
      break;

    case mi_edit_script_op::op::insert_ws:
      pending.push_back(insertion{
	  insertion::kind::ws,
	  {},
	  op._insert_ws.stickiness,
	  {}
	});
      t += 1;
      break;

    }
  }
  flush_pending(L, true);

  // The invocation is to be dropped from the source if all of its
  // non-empty output tokens got deleted. An invocation expanding to
  // nothing but placeholder empty tokens has no such token, but is
  // in effect nevertheless: its placeholder has been reached by some
  // edit op, otherwise there wouldn't be any rewrite of it in the
  // first place.
  bool any_nonempty = false;
  _whole_output_deleted = true;
  for (std::size_t p = 0; p < L && _whole_output_deleted; ++p) {
    if (out_toks[p].is_empty())
      continue;
    any_nonempty = true;
    if (_tok_edits[p].k != token_edit::kind::deleted)
      _whole_output_deleted = false;
  }
  for (std::size_t g = 0; g <= L && _whole_output_deleted; ++g) {
    if (!_gap_insertions[g].empty())
      _whole_output_deleted = false;
  }
  if (!any_nonempty)
    _whole_output_deleted = false;

  // Collect the output fragments.
  std::vector<std::size_t> frag_of_tok(L, std::numeric_limits<std::size_t>::max());
  std::size_t next_frag_begin = 0;
  for (auto it_frag = out.fragments_begin(); it_frag != out.fragments_end();
       ++it_frag) {
    const auto &r = it_frag->get_out_range();
    assert(r.begin == next_frag_begin);
    assert(r.end <= L);
    _frags.push_back(frag_info{&*it_frag, {}, {}});
    for (std::size_t p = r.begin; p < r.end; ++p)
      frag_of_tok[p] = _frags.size() - 1;
    next_frag_begin = r.end;
  }
  assert(next_frag_begin == L);

  const auto repl_begin_of_frag =
    [&](const std::size_t fi) -> raw_pp_token_index {
      const auto &f = *_frags[fi].f;
      assert(f.get_op() != out_fragment::op::empty);
      assert(f.get_op() != out_fragment::op::builtin_special);
      return f.get_repl().begin();
    };

  const auto repl_end_of_frag =
    [&](const std::size_t fi) -> raw_pp_token_index {
      const auto &f = *_frags[fi].f;
      return f.get_repl().begin() + f.get_repl().length();
    };

  // Attribute the gap insertions to fragments resp. the definition
  // level.
  //
  // An insertion inbetween two fragments can either go to the
  // definition level or get moved into an adjacent argument
  // reference's head or tail. The latter is preferable, because it
  // leaves the definition unchanged and thus, more likely to be
  // reusable for other invocations. However, pushing an insertion
  // into the argument affects every reference of the parameter, so
  // this is feasible only if all references have identical insertions
  // at that side. Decisions are made per parameter side: either all
  // of its references take their insertions or none does. If both
  // fragments adjacent to some gap are feasible argument references,
  // the insertion's stickiness decides.
  if (!_rmi.get_macro().is_builtin_special()) {
    const std::size_t none = std::numeric_limits<std::size_t>::max();

    // Canonicalize: placeholder empty tokens are transparent, move
    // insertions over them to the right so that each gap with
    // insertions is located immediately before a non-empty token (or
    // at the very end). Process the gaps from right to left to retain
    // the relative order of insertions moved to the same gap.
    for (std::size_t g = L; g > 0;) {
      --g;
      if (_gap_insertions[g].empty() || !out_toks[g].is_empty())
	continue;
      std::size_t cg = g;
      while (cg < L && out_toks[cg].is_empty())
	++cg;
      auto &dst = _gap_insertions[cg];
      dst.insert(dst.begin(),
		 std::make_move_iterator(_gap_insertions[g].begin()),
		 std::make_move_iterator(_gap_insertions[g].end()));
      _gap_insertions[g].clear();
    }

    // Placeholder empty tokens and whitespace (e.g. from the
    // replacement list inbetween two parameter references) are
    // transparent with respect to adjacency: the gaps inbetween two
    // non-transparent tokens form a single region, whose insertions
    // get attributed as a whole to either the fragment of the token
    // to the left, the one to the right or the definition level.
    const auto is_transparent =
      [&](const std::size_t p) {
	return out_toks[p].is_type_any_of<pp_token::type::empty,
					  pp_token::type::ws,
					  pp_token::type::newline>();
      };
    // The position of the first non-transparent token >= p, or L.
    const auto next_nt =
      [&](std::size_t p) {
	while (p < L && is_transparent(p))
	  ++p;
	return p;
      };
    // The position of the last non-transparent token < p, or none.
    const auto prev_nt =
      [&](std::size_t p) {
	while (p > 0) {
	  --p;
	  if (!is_transparent(p))
	    return p;
	}
	return none;
      };
    const auto arg_index_of_frag =
      [&](const std::size_t fi) {
	if (fi == none || _frags[fi].f->get_op() != out_fragment::op::arg)
	  return none;
	return _frags[fi].f->get_arg_ref_fragment().get_arg_index();
      };

    // A region is identified by the position of the non-transparent
    // token following it (L for the tail region) and spans the gaps
    // (prev_nt(rf_pos), rf_pos].
    struct region
    {
      std::size_t lf_pos;
      std::size_t rf_pos;
      std::size_t lf;
      std::size_t rf;
      std::vector<insertion> inss;
    };
    std::map<std::size_t, region> regions;
    for (std::size_t g = 0; g <= L; ++g) {
      if (_gap_insertions[g].empty())
	continue;
      const std::size_t rf_pos = next_nt(g);
      const std::size_t lf_pos = prev_nt(g);
      const std::size_t lf = lf_pos != none ? frag_of_tok[lf_pos] : none;
      const std::size_t rf = rf_pos != L ? frag_of_tok[rf_pos] : none;
      if (lf != none && lf == rf) {
	// Interior gap, leave as is.
	continue;
      }
      auto it = regions.find(rf_pos);
      if (it == regions.end())
	it = regions.emplace(rf_pos, region{lf_pos, rf_pos, lf, rf, {}}).first;
      auto &inss = _gap_insertions[g];
      it->second.inss.insert(it->second.inss.end(),
			     std::make_move_iterator(inss.begin()),
			     std::make_move_iterator(inss.end()));
      inss.clear();
    }

    // The regions at a fragment's head resp. tail, if any.
    const auto head_region_of =
      [&](const std::size_t fi) -> const region* {
	const std::size_t p = next_nt(_frags[fi].b());
	if (p >= _frags[fi].e())
	  return nullptr;
	const auto it = regions.find(p);
	return it != regions.end() ? &it->second : nullptr;
      };
    const auto tail_region_of =
      [&](const std::size_t fi) -> const region* {
	const std::size_t p = prev_nt(_frags[fi].e());
	if (p == none || p < _frags[fi].b())
	  return nullptr;
	const auto it = regions.find(next_nt(p + 1));
	return it != regions.end() ? &it->second : nullptr;
      };

    // Feasibility per parameter side: all references of the parameter
    // (ignoring those without any non-transparent tokens, which never
    // are adjacent to a region) have the same insertions in the region
    // at their head resp. tail. Pushing is pointless if there aren't
    // any insertions.
    const std::size_t nargs = _rmi.get_nargs();
    std::vector<bool> head_pushed(nargs, false);
    std::vector<bool> tail_pushed(nargs, false);
    const std::vector<insertion> no_inss;
    for (std::size_t i = 0; i < nargs; ++i) {
      bool head_feasible = true;
      bool tail_feasible = true;
      bool any_head = false;
      bool any_tail = false;
      const std::vector<insertion> *head_ref = nullptr;
      const std::vector<insertion> *tail_ref = nullptr;
      for (std::size_t fi = 0; fi < _frags.size(); ++fi) {
	if (arg_index_of_frag(fi) != i)
	  continue;
	if (next_nt(_frags[fi].b()) >= _frags[fi].e())
	  continue;
	const region * const hr = head_region_of(fi);
	const region * const tr = tail_region_of(fi);
	const auto &h = hr ? hr->inss : no_inss;
	const auto &t = tr ? tr->inss : no_inss;
	if (!head_ref) {
	  head_ref = &h;
	  tail_ref = &t;
	} else {
	  if (*head_ref != h)
	    head_feasible = false;
	  if (*tail_ref != t)
	    tail_feasible = false;
	}
	any_head |= !h.empty();
	any_tail |= !t.empty();
      }
      head_pushed[i] = head_feasible && any_head;
      tail_pushed[i] = tail_feasible && any_tail;
    }

    // Resolve the competition at regions with two feasible candidates
    // by the stickiness of the region's first insertion. The loser's
    // side doesn't take any insertions at all, which can only reduce
    // the competition elsewhere, so a single pass suffices.
    for (const auto &r : regions) {
      const std::size_t ti = arg_index_of_frag(r.second.lf);
      const std::size_t hj = arg_index_of_frag(r.second.rf);
      const bool tail_cand = ti != none && tail_pushed[ti];
      const bool head_cand = hj != none && head_pushed[hj];
      if (tail_cand && head_cand) {
	if (r.second.inss.front().stickiness == sticky_side::left)
	  head_pushed[hj] = false;
	else
	  tail_pushed[ti] = false;
      }
    }

    // Finally distribute the insertions.
    for (auto &r : regions) {
      const std::size_t lf = r.second.lf;
      const std::size_t rf = r.second.rf;
      const std::size_t ti = arg_index_of_frag(lf);
      const std::size_t hj = arg_index_of_frag(rf);
      auto &inss = r.second.inss;
      if (ti != none && tail_pushed[ti]) {
	auto &dst = _frags[lf].tail_ins;
	dst.insert(dst.end(), std::make_move_iterator(inss.begin()),
		   std::make_move_iterator(inss.end()));
      } else if (hj != none && head_pushed[hj]) {
	auto &dst = _frags[rf].head_ins;
	dst.insert(dst.end(), std::make_move_iterator(inss.begin()),
		   std::make_move_iterator(inss.end()));
      } else {
	// Definition level insertions, placed according to their
	// stickiness.
	for (auto &ins : inss) {
	  raw_pp_token_index raw_pos;
	  if (ins.stickiness == sticky_side::left && lf != none)
	    raw_pos = repl_end_of_frag(lf);
	  else if (rf != none)
	    raw_pos = repl_begin_of_frag(rf);
	  else if (lf != none)
	    raw_pos = repl_end_of_frag(lf);
	  else
	    raw_pos = _rmi.get_macro().get_repl().size();
	  _def_level_insertions.emplace_back(raw_pos, std::move(ins));
	}
      }
    }
  }

  // Analyze the argument references.
  const std::size_t nargs = _rmi.get_nargs();
  _args.resize(nargs);
  for (std::size_t fi = 0; fi < _frags.size(); ++fi) {
    const auto &f = *_frags[fi].f;
    switch (f.get_op()) {
    case out_fragment::op::arg:
      {
	const auto i = f.get_arg_ref_fragment().get_arg_index();
	assert(i < nargs);
	_args[i].expanded_arg_refs.push_back(fi);
      }
      break;

    case out_fragment::op::stringification:
      {
	const auto i = f.get_stringification_fragment().get_arg().get_arg_index();
	assert(i < nargs);
	_args[i].operator_arg_refs.push_back(fi);
      }
      break;

    case out_fragment::op::concat:
      {
	const auto &cs = f.get_concat_sequence_fragment();
	for (std::size_t c = 0; c < cs.size(); ++c) {
	  const std::size_t i = cs[c](concat_component_arg_index{});
	  if (i == std::numeric_limits<std::size_t>::max())
	    continue;
	  assert(i < nargs);
	  auto &operator_arg_refs = _args[i].operator_arg_refs;
	  if (std::find(operator_arg_refs.begin(), operator_arg_refs.end(), fi) ==
	      operator_arg_refs.end()) {
	    operator_arg_refs.push_back(fi);
	  }
	}
      }
      break;

    default:
      break;
    }
  }

  // Resolve the edits at the expanded references of each parameter.
  //
  // The original pre-expands each argument exactly once and the
  // result gets substituted for every reference of the parameter in
  // the replacement list. If the references are to be edited
  // differently, this cannot be reproduced by rewriting the argument
  // alone. The model applied here is to retain a contiguous range X
  // of the argument's expansion result in the (rewritten) argument:
  // references agreeing on the edits within X are "served" through
  // the parameter, with their edited expansion result outside of X
  // placed into the replacement list to the left resp. right of the
  // parameter reference (as lowered, rewritten argument source where
  // the position is subject to rescan, verbatim otherwise). All other
  // references get lowered or replaced verbatim as a whole.
  //
  // In addition, a reference whose edit at X's first (last) token
  // replaces that identifier by a sequence like '( * klpe_id )' can be
  // served by wrapping the parameter reference, '( * klpe_ ## x )',
  // with the prefix (suffix) rename implemented through a '##'
  // paste. As '##' operands don't get pre-expanded, pastes are
  // restricted to arguments not invoking any macros.
  //
  // The goal is to maximize the probability that the rewritten
  // definition is reusable for other invocations of the same macro,
  // without looking at any. Material from the argument's expansion
  // result ending up in the replacement list, be it due to served
  // references' outside portions or due to lowered/verbatim
  // references, is what makes the definition specific to this
  // invocation. Hence candidate ranges X are ranked by the cost of
  // that material first, counted lexicographically with non-keyword
  // identifiers of any universality (c.f. arg_out_universality) before
  // all other tokens and lower universality before higher within each
  // group. Ties are broken in favor of more served references, then
  // in favor of not having to change the definition at all, then not
  // having to rewrite the argument, then larger X.
  //
  // X may well cut through the expansion of some macro invoked from
  // the argument: the argument's rewrite would then rewrite that
  // invocation to drop the part outside of X and the lowered outside
  // portions of the served references would get their own rewritten
  // variants retaining that part.
  for (std::size_t i = 0; i < nargs; ++i) {
    auto &ai = _args[i];
    if (ai.expanded_arg_refs.empty())
      continue;

    // A reference whose output got deleted completely, with no
    // insertions attributed to it, is removed from the replacement
    // list: the parameter reference is what made the deleted tokens
    // appear in the first place. This keeps the argument out of the
    // analysis for that reference, rather than having it emptied,
    // which would rewrite its whole expansion tree into definitions
    // expanding to nothing. If no expanded reference remains, the
    // argument isn't expanded at all anymore and stays as is in the
    // invocation's source. The only observable difference is the
    // dropped pre-expansion, which matters for __COUNTER__ only,
    // where dropping is permitted. Placeholder empty tokens don't
    // count if there's any real token: the edit script's copy ranges
    // tend to extend over trailing ones. An output consisting of
    // placeholders only is deleted iff none of them is covered.
    {
      std::vector<std::size_t> remaining;
      for (const auto fi : ai.expanded_arg_refs) {
	arg_ref_edit re = extract_arg_ref_edit(_frags[fi]);
	bool any_nonempty = false;
	bool all_nonempty_deleted = true;
	bool all_deleted = true;
	for (std::size_t p = 0; p < re.tok_edits.size(); ++p) {
	  const bool del = re.tok_edits[p].k == token_edit::kind::deleted;
	  if (!out_toks[_frags[fi].b() + p].is_empty()) {
	    any_nonempty = true;
	    all_nonempty_deleted &= del;
	  }
	  all_deleted &= del;
	}
	const bool deleted =
	  (!re.tok_edits.empty() &&
	   (any_nonempty ? all_nonempty_deleted : all_deleted) &&
	   std::all_of(re.gaps.begin(), re.gaps.end(),
		       [](const std::vector<insertion> &g) {
			 return g.empty();
		       }));
	if (deleted) {
	  ai.deleted_arg_refs.push_back(fi);
	} else {
	  remaining.push_back(fi);
	  ai.ref_edits.push_back(std::move(re));
	}
      }
      ai.expanded_arg_refs = std::move(remaining);
    }
    if (ai.expanded_arg_refs.empty())
      continue;

    // "Don't care" references: a reference whose output tokens are
    // consumed as (part of) an argument by some rescan child which
    // never references the corresponding parameter, neither plainly
    // nor as an operand to # or ##, e.g. by a macro expanding to
    // nothing. Those tokens never make it to the output, so the
    // reference doesn't carry any edit, which would make it disagree
    // with the edited references and force a rewrite of the
    // definition. Whatever the argument's content, the child ignores
    // it: leave such references out of the analysis and keep them as
    // plain parameter references, so that edits can get pushed down
    // into the argument unimpeded. If nothing is served through the
    // parameter at all, the argument isn't expanded anymore and
    // these references get placed verbatim like the others. Note
    // that this is decided from the child's definition only: the
    // reference's tokens must be consumed directly by a child of
    // this invocation.
    {
      std::vector<std::pair<std::size_t, std::size_t>> unref_arg_ranges;
      const auto st = static_cast<expansion_forest::rescan_subtree>(_rmi);
      for (auto it = st.dfs_pre_begin(); it != st.dfs_pre_end(); ++it) {
	const rescan_macro_invocation c = *it;
	if (c.is_arg_tree_root() ||
	    (static_cast<original_node>(c) ==
	     static_cast<original_node>(_rmi)) ||
	    (static_cast<original_node>(c.get_rescan_parent()) !=
	     static_cast<original_node>(_rmi))) {
	  continue;
	}
	const pp_result::macro &cm = c.get_macro();
	if (!cm.is_func_like() || cm.is_builtin_special())
	  continue;
	const auto &names = cm.get_arg_names();
	const auto &crepl = cm.get_repl();
	const std::size_t c_nargs = std::min(c.get_nargs(), names.size());
	for (std::size_t j = 0; j < c_nargs; ++j) {
	  const bool referenced =
	    std::any_of(crepl.begin(), crepl.end(),
			[&](const raw_pp_token &t) {
			  return t.is_id() && t.get_value() == names[j];
			});
	  if (referenced)
	    continue;
	  const auto &r =
	    static_cast<rescan_macro_invocation>(c.get_arg_tree(j))
	    .get_src_range();
	  unref_arg_ranges.emplace_back(r.begin, r.end);
	}
      }
      if (!unref_arg_ranges.empty()) {
	std::vector<std::size_t> remaining;
	std::vector<arg_ref_edit> remaining_edits;
	for (std::size_t k = 0; k < ai.expanded_arg_refs.size(); ++k) {
	  const std::size_t fi = ai.expanded_arg_refs[k];
	  arg_ref_edit &re = ai.ref_edits[k];
	  const bool untouched =
	    (std::all_of(re.tok_edits.begin(), re.tok_edits.end(),
			 [](const token_edit &te) {
			   return te.k == token_edit::kind::kept;
			 }) &&
	     std::all_of(re.gaps.begin(), re.gaps.end(),
			 [](const std::vector<insertion> &g) {
			   return g.empty();
			 }));
	  // The preprocessor trims placeholder empty tokens from the
	  // arguments' sources: a reference consisting of placeholders
	  // only, i.e. an argument expanding to nothing, is consumed
	  // by an adjacent argument range.
	  const bool only_placeholders =
	    std::all_of(out_toks.begin() + _frags[fi].b(),
			out_toks.begin() + _frags[fi].e(),
			[](const pp_token &t) { return t.is_empty(); });
	  const bool consumed =
	    std::any_of(unref_arg_ranges.begin(), unref_arg_ranges.end(),
			[&](const std::pair<std::size_t, std::size_t> &r) {
			  if (only_placeholders) {
			    return (r.first <= _frags[fi].e() &&
				    _frags[fi].b() <= r.second);
			  }
			  return (r.first <= _frags[fi].b() &&
				  _frags[fi].e() <= r.second);
			});
	  if (untouched && consumed) {
	    ai.dont_care_arg_refs.push_back(fi);
	  } else {
	    remaining.push_back(fi);
	    remaining_edits.push_back(std::move(re));
	  }
	}
	ai.expanded_arg_refs = std::move(remaining);
	ai.ref_edits = std::move(remaining_edits);
      }
    }
    if (ai.expanded_arg_refs.empty()) {
      // Only "don't care" references remain: the argument is
      // referenced as is.
      ai.any_ref_kept = true;
      continue;
    }

    const std::size_t none = arg_info::none;
    const std::size_t R = ai.expanded_arg_refs.size();
    const std::size_t alen = ai.ref_edits.front().tok_edits.size();
    for (const auto &re : ai.ref_edits)
      assert(re.tok_edits.size() == alen);
    // All references are copies of the same expansion result, use the
    // first one's tokens for examining them.
    const std::size_t fb0 = _frags[ai.expanded_arg_refs.front()].b();
    const auto arg_out_tok =
      [&](const std::size_t p) -> const pp_token& {
	return out_toks[fb0 + p];
      };

    const auto arg_tree = _rmi.get_arg_tree(i);
    const std::vector<unsigned> univ = arg_out_universality{arg_tree}.get();
    assert(univ.size() == alen);
    const bool macro_free = arg_tree_is_macro_free(arg_tree);
    unsigned nlevels = 1;
    for (const unsigned u : univ)
      nlevels = std::max(nlevels, u + 1);

    // Placeholder empty tokens are transparent: a range X starting
    // before the first resp. ending after the last non-empty token
    // includes the argument's head resp. tail, i.e. the head/tail
    // insertions at gap 0 resp. gap alen. Note that the attribution
    // of insertions above has moved any insertions at gaps adjacent to
    // empty tokens over to the next non-empty token.
    std::size_t first_nonempty = alen;
    std::size_t last_nonempty_end = 0;
    for (std::size_t p = 0; p < alen; ++p) {
      if (arg_out_tok(p).is_empty())
	continue;
      first_nonempty = std::min(first_nonempty, p);
      last_nonempty_end = p + 1;
    }
    const auto at_head =
      [&](const std::size_t b) { return b <= first_nonempty; };
    const auto at_tail =
      [&](const std::size_t e) { return e >= last_nonempty_end; };

    // The cost vector index of material at position p: non-keyword
    // identifiers first, then by universality.
    const auto cost_index =
      [&](const std::size_t p) {
	return (is_nonkeyword_id(arg_out_tok(p)) ? 0 : nlevels) + univ[p];
      };

    // Whether reference r's edit leaves anything at position p.
    const auto has_material_at =
      [&](const std::size_t r, const std::size_t p) {
	const auto &te = ai.ref_edits[r].tok_edits[p];
	switch (te.k) {
	case token_edit::kind::deleted:
	  return false;
	case token_edit::kind::kept:
	  // Placeholders and whitespace don't make a definition
	  // specific to the invocation.
	  return !arg_out_tok(p).is_type_any_of<pp_token::type::empty,
						 pp_token::type::ws,
						 pp_token::type::newline>();
	case token_edit::kind::replaced:
	  return !te.replacement.empty();
	case token_edit::kind::rewrite_macro_id:
	  return true;
	}
	return false;
      };

    // Whether reference r's edit leaves anything within the positions
    // [p_begin, p_end) and the gaps [g_begin, g_end].
    const auto portion_has_material =
      [&](const std::size_t r,
	  const std::size_t p_begin, const std::size_t p_end,
	  const std::size_t g_begin, const std::size_t g_end) {
	for (std::size_t p = p_begin; p < p_end; ++p) {
	  if (has_material_at(r, p))
	    return true;
	}
	for (std::size_t g = g_begin; g <= g_end; ++g) {
	  if (!ai.ref_edits[r].gaps[g].empty())
	    return true;
	}
	return false;
      };

    // Lowering the argument's (rewritten) source into the replacement
    // list is possible only where it would still get macro
    // expanded. Otherwise, e.g. if the reference ends up as an operand
    // to # or ## in some rescan child, the (edited) expansion result
    // is to be placed there verbatim. Placeholder empty tokens don't
    // count: they're not part of any rescan child's argument.
    const auto ref_rescanned =
      [&](const std::size_t r) {
	const std::size_t fb = _frags[ai.expanded_arg_refs[r]].b();
	const std::size_t fe = _frags[ai.expanded_arg_refs[r]].e();
	for (std::size_t p = fb; p < fe; ++p) {
	  if (!out_toks[p].is_empty() && !tok_rescanned[p])
	    return false;
	}
	return true;
      };

    // A reference's edit at one of X's boundary tokens which can be
    // implemented by wrapping the parameter reference, c.f. wrap_edit.
    const auto detect_wrap =
      [&](const std::size_t r, const std::size_t p,
	  const bool allow_pre_side, const bool allow_post_side) {
	wrap_edit w = detect_wrap_edit(arg_out_tok(p),
				       ai.ref_edits[r].tok_edits[p]);
	if (!w.valid)
	  return w;
	// Pasting works on the unexpanded argument, which must thus
	// equal the expanded one.
	if ((!w.prefix.empty() || !w.suffix.empty()) && !macro_free)
	  w.valid = false;
	else if (!allow_pre_side && (!w.pre.empty() || !w.prefix.empty()))
	  w.valid = false;
	else if (!allow_post_side && (!w.post.empty() || !w.suffix.empty()))
	  w.valid = false;
	return w;
      };

    // Reference r's edits restricted to X = [b, e), with wrapped
    // boundary tokens normalized to "kept". Gaps strictly inside X are
    // included, the ones at the boundaries only if they coincide with
    // the argument's head resp. tail.
    const auto restrict_edit =
      [&](const std::size_t r, const std::size_t b, const std::size_t e,
	  const bool wrap_first, const bool wrap_last) {
	arg_ref_edit k;
	const auto &re = ai.ref_edits[r];
	k.tok_edits.assign(re.tok_edits.begin() + b, re.tok_edits.begin() + e);
	k.gaps.resize(e - b + 1);
	for (std::size_t g = b + 1; g < e; ++g)
	  k.gaps[g - b] = re.gaps[g];
	if (at_head(b))
	  k.gaps[0] = re.gaps[0];
	if (at_tail(e))
	  k.gaps[e - b] = re.gaps[alen];
	if (wrap_first) {
	  k.tok_edits.front() = token_edit{};
	  k.tok_edits.front().k = token_edit::kind::kept;
	}
	if (wrap_last) {
	  k.tok_edits.back() = token_edit{};
	  k.tok_edits.back().k = token_edit::kind::kept;
	}
	return k;
      };

    struct class_member
    {
      std::size_t r;
      bool wrap_first;
      bool wrap_last;
    };
    struct candidate
    {
      std::size_t b = 0;
      std::size_t e = 0;
      arg_ref_edit rep;
      std::vector<class_member> members;
      std::vector<unsigned> cost;
      std::size_t n_served = 0;
      bool def_change = false;
      bool identity = false;
    };
    // Whether a is to be preferred over b.
    const auto better =
      [](const candidate &a, const candidate &b) {
	if (a.cost != b.cost) {
	  return std::lexicographical_compare(a.cost.begin(), a.cost.end(),
					      b.cost.begin(), b.cost.end());
	}
	if (a.n_served != b.n_served)
	  return a.n_served > b.n_served;
	if (a.def_change != b.def_change)
	  return !a.def_change;
	if (a.identity != b.identity)
	  return a.identity;
	if (a.e - a.b != b.e - b.b)
	  return a.e - a.b > b.e - b.b;
	return a.b < b.b;
      };

    // Search for the best candidate X. Trying every range of the
    // expansion result with a from-scratch evaluation each would be
    // cubic in the argument's length, which is prohibitive for
    // arguments expanding to thousands of tokens. Instead, note that
    // the ranking is monotone under extension of X across
    // "uninteresting" positions: if X gets extended over a token and
    // the thereby enclosed gap at which all references agree, the
    // served class stays the same, the served references' material
    // moved into X stops counting towards the cost, their outside
    // portions shrink and X grows. The only way such an extension
    // can hurt is through the identity tie-break, namely if the
    // enclosed token or gap carries a non-identity edit while X's
    // previous boundary token did not. Thus, the best X has its
    // boundaries at the argument's head or tail, adjacent to a
    // position at which the references disagree, adjacent to a
    // non-empty gap or at a transition between identity and
    // non-identity edits. Only those boundaries get enumerated.
    //
    // Furthermore, with per-position agreement classes and prefix
    // sums of the material counts, partitioning the references for a
    // range and evaluating a class take time linear in the number of
    // conflicts within X rather than in its length.

    // Agreement classes per token position and per gap: two
    // references share a class iff their edits there compare equal.
    const auto classify =
      [&](const std::size_t n, auto &&get) {
	std::vector<std::vector<std::size_t>> cls
	  (n, std::vector<std::size_t>(R, 0));
	std::vector<std::size_t> ncls(n, 0);
	for (std::size_t x = 0; x < n; ++x) {
	  for (std::size_t r = 0; r < R; ++r) {
	    std::size_t r2 = 0;
	    for (; r2 < r; ++r2) {
	      if (get(r2, x) == get(r, x))
		break;
	    }
	    cls[x][r] = (r2 < r ? cls[x][r2] : ncls[x]++);
	  }
	}
	return std::make_pair(std::move(cls), std::move(ncls));
      };
    auto tok_cls =
      classify(alen,
	       [&](const std::size_t r, const std::size_t p)
	       -> const token_edit& {
		 return ai.ref_edits[r].tok_edits[p];
	       });
    // Placeholder empty tokens are transparent: whether the edit
    // script reached one of them or not doesn't make the references
    // disagree.
    for (std::size_t p = 0; p < alen; ++p) {
      if (arg_out_tok(p).is_empty()) {
	std::fill(tok_cls.first[p].begin(), tok_cls.first[p].end(), 0);
	tok_cls.second[p] = 1;
      }
    }
    const auto gap_cls =
      classify(alen + 1,
	       [&](const std::size_t r, const std::size_t g)
	       -> const std::vector<insertion>& {
		 return ai.ref_edits[r].gaps[g];
	       });
    const auto tok_conflict =
      [&](const std::size_t p) { return tok_cls.second[p] > 1; };
    const auto gap_conflict =
      [&](const std::size_t g) { return gap_cls.second[g] > 1; };
    const auto edit_kind =
      [&](const std::size_t r, const std::size_t p) {
	return ai.ref_edits[r].tok_edits[p].k;
      };
    // The class of a plain "kept" edit at p, as substituted for
    // wrapped boundary tokens. A fresh one if no reference has it.
    std::vector<std::size_t> kept_class(alen);
    for (std::size_t p = 0; p < alen; ++p) {
      kept_class[p] = tok_cls.second[p];
      for (std::size_t r = 0; r < R; ++r) {
	if (edit_kind(r, p) == token_edit::kind::kept) {
	  kept_class[p] = tok_cls.first[p][r];
	  break;
	}
      }
    }

    // Prefix sums per reference: material per cost index and in
    // total over the token positions [0, p), non-empty gaps among
    // [0, g) and non-"kept" edits at non-placeholder positions within
    // [0, p).
    const std::size_t ncost = 2 * nlevels;
    std::vector<std::vector<unsigned>> mat_prefix
      (R, std::vector<unsigned>(ncost * (alen + 1), 0));
    std::vector<std::vector<unsigned>> anymat_prefix
      (R, std::vector<unsigned>(alen + 1, 0));
    std::vector<std::vector<unsigned>> gap_prefix
      (R, std::vector<unsigned>(alen + 2, 0));
    std::vector<std::vector<unsigned>> nonkept_prefix
      (R, std::vector<unsigned>(alen + 1, 0));
    for (std::size_t r = 0; r < R; ++r) {
      for (std::size_t p = 0; p < alen; ++p) {
	for (std::size_t k = 0; k < ncost; ++k) {
	  mat_prefix[r][k * (alen + 1) + p + 1] =
	    mat_prefix[r][k * (alen + 1) + p];
	}
	const bool mat = has_material_at(r, p);
	if (mat)
	  ++mat_prefix[r][cost_index(p) * (alen + 1) + p + 1];
	anymat_prefix[r][p + 1] = anymat_prefix[r][p] + mat;
	nonkept_prefix[r][p + 1] =
	  (nonkept_prefix[r][p] +
	   (edit_kind(r, p) != token_edit::kind::kept &&
	    !arg_out_tok(p).is_empty()));
      }
      for (std::size_t g = 0; g <= alen; ++g) {
	gap_prefix[r][g + 1] =
	  gap_prefix[r][g] + !ai.ref_edits[r].gaps[g].empty();
      }
    }
    // Material of reference r at cost index k within [p_begin, p_end).
    const auto mat_in =
      [&](const std::size_t r, const std::size_t k,
	  const std::size_t p_begin, const std::size_t p_end) {
	return (mat_prefix[r][k * (alen + 1) + p_end] -
		mat_prefix[r][k * (alen + 1) + p_begin]);
      };
    const auto anymat_in =
      [&](const std::size_t r,
	  const std::size_t p_begin, const std::size_t p_end) {
	return anymat_prefix[r][p_end] - anymat_prefix[r][p_begin];
      };
    // Non-empty gaps of reference r among [g_begin, g_end].
    const auto gaps_in =
      [&](const std::size_t r,
	  const std::size_t g_begin, const std::size_t g_end) {
	return gap_prefix[r][g_end + 1] - gap_prefix[r][g_begin];
      };
    const auto nonkept_in =
      [&](const std::size_t r,
	  const std::size_t p_begin, const std::size_t p_end) {
	return nonkept_prefix[r][p_end] - nonkept_prefix[r][p_begin];
      };

    // The candidate boundaries, c.f. above. Canonical boundaries
    // don't point at placeholder empty tokens, c.f. the
    // canonicalization in the enumeration below.
    std::vector<std::size_t> bounds;
    const auto add_bound =
      [&](std::size_t x) {
	x = std::min(x, alen);
	while (x < alen && arg_out_tok(x).is_empty())
	  ++x;
	bounds.push_back(x);
      };
    add_bound(0);
    add_bound(alen);
    // At which positions all references agree on a non-identity edit.
    std::vector<bool> nonid(alen, false);
    for (std::size_t p = 0; p < alen; ++p) {
      if (tok_conflict(p)) {
	add_bound(p);
	add_bound(p + 1);
      } else {
	nonid[p] = (edit_kind(0, p) != token_edit::kind::kept &&
		    !arg_out_tok(p).is_empty());
      }
    }
    for (std::size_t g = 0; g <= alen; ++g) {
      if (gap_conflict(g) || !ai.ref_edits[0].gaps[g].empty()) {
	add_bound(g);
	// The head and tail gaps enter the comparison with the
	// boundaries of X at the argument's head resp. tail only. The
	// candidate boundaries adjacent to those matter then.
	if (g == 0)
	  add_bound(first_nonempty + 1);
	if (g == alen && last_nonempty_end != 0)
	  add_bound(last_nonempty_end - 1);
      }
    }
    for (std::size_t p = 1; p < alen; ++p) {
      if (nonid[p] != nonid[p - 1])
	add_bound(p);
    }
    std::sort(bounds.begin(), bounds.end());
    bounds.erase(std::unique(bounds.begin(), bounds.end()), bounds.end());

    // The conflict positions, for forming the agreement classes.
    std::vector<std::size_t> conflict_toks;
    std::vector<std::size_t> conflict_gaps;
    for (std::size_t p = 0; p < alen; ++p) {
      if (tok_conflict(p))
	conflict_toks.push_back(p);
    }
    for (std::size_t g = 0; g <= alen; ++g) {
      if (gap_conflict(g))
	conflict_gaps.push_back(g);
    }

    // The signature of reference r's edits restricted to X = [b, e),
    // with wrapped boundary tokens normalized to "kept": equal
    // signatures mean equal restricted edits. Besides the conflict
    // positions, the boundary tokens are always included so that
    // wrapped and literal variants get told apart.
    const auto signature =
      [&](const std::size_t r, const std::size_t b, const std::size_t e,
	  const bool wrap_first, const bool wrap_last) {
	std::vector<std::size_t> sig;
	const auto tok_sig =
	  [&](const std::size_t p) {
	    if ((wrap_first && p == b) || (wrap_last && p + 1 == e))
	      return kept_class[p];
	    return tok_cls.first[p][r];
	  };
	if (b < e) {
	  sig.push_back(tok_sig(b));
	  if (e > b + 1)
	    sig.push_back(tok_sig(e - 1));
	}
	for (auto it = std::lower_bound(conflict_toks.begin(),
					conflict_toks.end(), b + 1);
	     it != conflict_toks.end() && *it + 1 < e; ++it) {
	  sig.push_back(tok_sig(*it));
	}
	if (at_head(b))
	  sig.push_back(gap_cls.first[0][r]);
	for (auto it = std::lower_bound(conflict_gaps.begin(),
					conflict_gaps.end(), b + 1);
	     it != conflict_gaps.end() && *it < e; ++it) {
	  sig.push_back(gap_cls.first[*it][r]);
	}
	if (at_tail(e))
	  sig.push_back(gap_cls.first[alen][r]);
	return sig;
      };

    // Evaluate a candidate's cost etc. from its members in terms of
    // the prefix sums.
    const auto evaluate =
      [&](candidate &c) {
	std::vector<bool> served(R, false);
	for (const auto &m : c.members)
	  served[m.r] = true;
	c.cost.assign(ncost, 0);
	c.n_served = c.members.size();
	c.def_change = c.n_served < R;
	for (std::size_t r = 0; r < R; ++r) {
	  for (std::size_t k = 0; k < ncost; ++k) {
	    c.cost[k] += mat_in(r, k, 0, alen);
	    if (served[r])
	      c.cost[k] -= mat_in(r, k, c.b, c.e);
	  }
	  if (!served[r])
	    continue;
	  if (!at_head(c.b) &&
	      (anymat_in(r, 0, c.b) || gaps_in(r, 0, c.b)))
	    c.def_change = true;
	  if (!at_tail(c.e) &&
	      (anymat_in(r, c.e, alen) || gaps_in(r, c.e, alen)))
	    c.def_change = true;
	}
	for (const auto &m : c.members) {
	  if (m.wrap_first || m.wrap_last)
	    c.def_change = true;
	}
	// The representative edit is the first member's restricted
	// one: identity iff all of its edits within X are "kept",
	// wrapped boundary tokens included, and all of the gaps
	// considered are empty.
	c.identity = false;
	if (at_head(c.b) && at_tail(c.e) && c.n_served) {
	  const class_member &m = c.members.front();
	  unsigned nonkept = nonkept_in(m.r, c.b, c.e);
	  if (m.wrap_first && edit_kind(m.r, c.b) != token_edit::kind::kept)
	    --nonkept;
	  if (m.wrap_last && !(m.wrap_first && c.e == c.b + 1) &&
	      edit_kind(m.r, c.e - 1) != token_edit::kind::kept) {
	    --nonkept;
	  }
	  const bool inner_gaps =
	    c.e > c.b + 1 && gaps_in(m.r, c.b + 1, c.e - 1);
	  c.identity = (!nonkept && !inner_gaps &&
			ai.ref_edits[m.r].gaps[0].empty() &&
			ai.ref_edits[m.r].gaps[alen].empty());
	}
      };

    candidate best;
    bool have_best = false;
    const auto consider =
      [&](candidate &&c) {
	evaluate(c);
	if (!have_best || better(c, best)) {
	  best = std::move(c);
	  have_best = true;
	}
      };

    // The candidate serving no reference at all: everything gets
    // lowered or replaced verbatim.
    consider(candidate{});

    for (const std::size_t b : bounds) {
      for (const std::size_t e : bounds) {
	if (e < b)
	  continue;
	// The empty X is for arguments without any real token only:
	// nothing to retain, but the references can still be served.
	if (b == e && first_nonempty != alen)
	  continue;

	// Partition the references into agreement classes. A
	// reference with a wrappable boundary edit contributes both,
	// its literal edits and the wrapped variants.
	std::vector<std::pair<std::vector<std::size_t>, candidate>> classes;
	for (std::size_t r = 0; r < R; ++r) {
	  const auto &re = ai.ref_edits[r];
	  std::vector<class_member> variants;
	  variants.push_back(class_member{r, false, false});
	  if (b < e) {
	    // A wrap at X's first token must not be preceded by
	    // insertions within the rewritten argument, likewise for
	    // the last one.
	    const bool first_ok = (!at_head(b) || re.gaps[0].empty());
	    const bool last_ok = (!at_tail(e) || re.gaps[alen].empty());
	    const wrap_edit wf =
	      (first_ok ?
	       detect_wrap(r, b, true, e == b + 1) : wrap_edit{});
	    const wrap_edit wl =
	      (last_ok && e > b + 1 ?
	       detect_wrap(r, e - 1, false, true) : wrap_edit{});
	    if (wf.valid)
	      variants.push_back(class_member{r, true, e == b + 1});
	    if (wl.valid)
	      variants.push_back(class_member{r, false, true});
	    if (wf.valid && wl.valid)
	      variants.push_back(class_member{r, true, true});
	  }
	  for (const auto &v : variants) {
	    auto sig = signature(r, b, e, v.wrap_first, v.wrap_last);
	    bool found = false;
	    for (auto &c : classes) {
	      if (c.first != sig)
		continue;
	      found = true;
	      if (std::none_of(c.second.members.begin(),
			       c.second.members.end(),
			       [&](const class_member &m) {
				 return m.r == r;
			       })) {
		c.second.members.push_back(v);
	      }
	      break;
	    }
	    if (!found) {
	      candidate c;
	      c.b = b;
	      c.e = e;
	      c.members.push_back(v);
	      classes.emplace_back(std::move(sig), std::move(c));
	    }
	  }
	}
	for (auto &c : classes)
	  consider(std::move(c.second));
      }
    }
    assert(have_best);
    // Materialize the winner's representative edit.
    if (best.n_served) {
      const class_member &m = best.members.front();
      best.rep = restrict_edit(m.r, best.b, best.e,
			       m.wrap_first, m.wrap_last);
    }

    const std::size_t b = best.b;
    const std::size_t e = best.e;
    ai.x_begin = b;
    ai.x_end = e;
    ai.x_head = at_head(b);
    ai.x_tail = at_tail(e);

    // The original pre-expands the argument exactly once, no matter
    // how often the parameter is referenced. If that expansion
    // invokes __COUNTER__, the rewritten invocation must not invoke
    // it more often than that, so lowering it into the replacement
    // list, in whole or in part, is never an option. For robustness,
    // don't let any reference be served through the parameter then
    // unless all of them can be served without any additional
    // material, but replace all of them verbatim: the argument
    // doesn't get expanded at all and no live __COUNTER__ value can
    // disagree with the frozen ones. Whether the __COUNTER__
    // invocation would survive the argument's rewrite is not known
    // before that has been carried out, so assume it does.
    if (best.def_change &&
	_forest.subtree_invokes_counter(_rmi.get_arg_tree(i))) {
      ai.verbatim_arg_refs = ai.expanded_arg_refs;
      ai.verbatim_arg_refs.insert(ai.verbatim_arg_refs.end(),
				  ai.dont_care_arg_refs.begin(),
				  ai.dont_care_arg_refs.end());
      ai.dont_care_arg_refs.clear();
      continue;
    }

    // The edits to push down into the argument: the served
    // references' common edits within X and deletions outside of
    // it. Gaps outside of X belong to the outside portions. Note
    // that this deletes the placeholder empty tokens outside of X as
    // well, so that invocations expanding to nothing there are not
    // retained in the rewritten argument. Inside X, the placeholders
    // keep whatever the edits say, which, as the edit script's ops
    // get binned by the tokens they cover, means that an invocation
    // expanding to nothing survives iff it's part of a covered range.
    ai.common.tok_edits.assign(alen, token_edit{});
    ai.common.gaps.assign(alen + 1, std::vector<insertion>{});
    if (best.n_served) {
      for (std::size_t p = b; p < e; ++p)
	ai.common.tok_edits[p] = best.rep.tok_edits[p - b];
      for (std::size_t g = b; g <= e; ++g) {
	if ((g == b && !ai.x_head) || (g == e && !ai.x_tail))
	  continue;
	ai.common.gaps[g] = best.rep.gaps[g - b];
      }
    } else {
      // Nothing is served, the argument stays as is in the source
      // (and doesn't get expanded).
      for (auto &te : ai.common.tok_edits)
	te.k = token_edit::kind::kept;
    }
    // Identity modulo placeholders: those don't warrant a rewrite of
    // the argument on their own.
    ai.common_identity = true;
    for (std::size_t p = 0; p < alen && ai.common_identity; ++p) {
      if (!arg_out_tok(p).is_empty() &&
	  ai.common.tok_edits[p].k != token_edit::kind::kept) {
	ai.common_identity = false;
      }
    }
    for (std::size_t g = 0; g <= alen && ai.common_identity; ++g) {
      if (!ai.common.gaps[g].empty())
	ai.common_identity = false;
    }
    ai.any_ref_kept = best.n_served != 0;
    if (!ai.any_ref_kept) {
      // The parameter is unreferenced in the rewritten definition
      // and the argument doesn't get expanded anymore: place the
      // "don't care" references verbatim like the others.
      ai.verbatim_arg_refs.insert(ai.verbatim_arg_refs.end(),
				  ai.dont_care_arg_refs.begin(),
				  ai.dont_care_arg_refs.end());
      ai.dont_care_arg_refs.clear();
    }

    // Even if there's nothing to push down into the argument, its
    // expansion tree needs to get processed if it contains any
    // invocation of a macro whose rename is forced.
    const bool force_arg_rewrite =
      (ai.common_identity && _forest.has_forced_renames() &&
       _forest.subtree_has_forced_rename(_rmi.get_arg_tree(i)));

    if (!ai.common_identity || force_arg_rewrite) {
      arg_rewrite ar{i, {}, {}, {}};
      ar.script = arg_ref_edit_to_script(ai.common, ar.traces);
      ai._arg_rewrites_index = _arg_rewrites.size();
      _arg_rewrites.push_back(std::move(ar));
    }

    // Queue a lowering rewrite of reference r's edits restricted to
    // the positions [p_begin, p_end) and gaps [g_begin, g_end],
    // everything else deleted.
    const auto queue_lowering =
      [&](const std::size_t r,
	  const std::size_t p_begin, const std::size_t p_end,
	  const std::size_t g_begin, const std::size_t g_end) {
	arg_ref_edit part = ai.ref_edits[r];
	for (std::size_t p = 0; p < alen; ++p) {
	  if (p < p_begin || p >= p_end)
	    part.tok_edits[p] = token_edit{};
	}
	for (std::size_t g = 0; g <= alen; ++g) {
	  if (g < g_begin || g > g_end)
	    part.gaps[g].clear();
	}
	arg_rewrite ar{i, {}, {}, {}};
	ar.script = arg_ref_edit_to_script(part, ar.traces);
	_arg_rewrites.push_back(std::move(ar));
	return _arg_rewrites.size() - 1;
      };

    std::vector<const class_member*> member_of(R, nullptr);
    for (const auto &m : best.members)
      member_of[m.r] = &m;
    for (std::size_t r = 0; r < R; ++r) {
      const std::size_t fi = ai.expanded_arg_refs[r];
      const bool rescanned = ref_rescanned(r);
      if (!member_of[r]) {
	// Not served through the parameter: lower the reference's
	// rewritten argument source into the replacement list, or
	// place the edited expansion result there verbatim.
	if (!rescanned) {
	  ai.verbatim_arg_refs.push_back(fi);
	} else {
	  ai.lower_rewrites.emplace(fi, queue_lowering(r, 0, alen, 0, alen));
	}
	continue;
      }

      // Served: collect the material needed next to the parameter
      // reference, if any.
      const class_member &m = *member_of[r];
      const bool left_mat =
	!ai.x_head && portion_has_material(r, 0, b, 0, b);
      const bool right_mat =
	!ai.x_tail && portion_has_material(r, e, alen, e, alen);
      if (!left_mat && !right_mat && !m.wrap_first && !m.wrap_last)
	continue;

      arg_info::served_ref_material mat;
      if (left_mat) {
	if (rescanned)
	  mat.left_lower = queue_lowering(r, 0, b, 0, b);
	else
	  mat.left_verbatim = true;
      }
      if (right_mat) {
	if (rescanned)
	  mat.right_lower = queue_lowering(r, e, alen, e, alen);
	else
	  mat.right_verbatim = true;
      }
      if (m.wrap_first) {
	const wrap_edit w = detect_wrap(r, b, true, e == b + 1);
	assert(w.valid);
	mat.wrap_first = true;
	mat.wrap_pre = w.pre;
	mat.paste_prefix = w.prefix;
	if (e == b + 1) {
	  // Single token X, the wrap covers both sides.
	  mat.wrap_last = true;
	  mat.paste_suffix = w.suffix;
	  mat.wrap_post = w.post;
	}
	for (const auto &tr : w.traces)
	  mat.wrap_traces.push_back(tr.second);
      }
      if (m.wrap_last && e > b + 1) {
	const wrap_edit w = detect_wrap(r, e - 1, false, true);
	assert(w.valid);
	mat.wrap_last = true;
	mat.paste_suffix = w.suffix;
	mat.wrap_post = w.post;
	for (const auto &tr : w.traces)
	  mat.wrap_traces.push_back(tr.second);
      }
      ai.served_material.emplace(fi, std::move(mat));
    }
  }
}

rescan_macro_invocation_rewriter::_impl::arg_ref_edit
rescan_macro_invocation_rewriter::_impl::extract_arg_ref_edit(const frag_info &fi)
  const
{
  arg_ref_edit re;
  const std::size_t alen = fi.e() - fi.b();
  re.tok_edits.assign(_tok_edits.begin() + fi.b(), _tok_edits.begin() + fi.e());
  re.gaps.resize(alen + 1);
  re.gaps.front() = fi.head_ins;
  for (std::size_t g = fi.b() + 1; g < fi.e(); ++g)
    re.gaps[g - fi.b()] = _gap_insertions[g];
  // Note: if alen == 0, head and tail insertions coincide at gap 0.
  auto &tail = re.gaps.back();
  tail.insert(tail.end(), fi.tail_ins.begin(), fi.tail_ins.end());
  return re;
}

mi_edit_script rescan_macro_invocation_rewriter::_impl::
arg_ref_edit_to_script(const arg_ref_edit &re,
		   unexpanded_macro_id_out_tokens_origin_traces &out_traces)
  const
{
  mi_edit_script script;
  std::size_t t = 0;
  const std::size_t alen = re.tok_edits.size();

  const auto add_traces =
    [&](const std::vector<std::pair<std::size_t, trace_index>> &trs,
	const std::size_t t_begin) {
      for (const auto &tr : trs) {
	out_traces.append(unexpanded_macro_id_out_token_origin_trace{
	    .traced_out_token_index = t_begin + tr.first,
	    .unexpanded_macro_id_out_token = tr.second,
	  });
      }
    };

  const auto emit_insertion =
    [&](const insertion &ins) {
      switch (ins.k) {
      case insertion::kind::ws:
	script.push_back(mi_edit_script_op{
	    mi_edit_script_op::insert_ws{ .stickiness = ins.stickiness }
	  });
	t += 1;
	break;

      case insertion::kind::token:
	for (std::size_t k = 0; k < ins.toks.size(); ++k) {
	  script.push_back(mi_edit_script_op{
	      mi_edit_script_op::insert_token{ins.toks[k], ins.stickiness}
	    });
	}
	add_traces(ins.traces, t);
	t += ins.toks.size();
	break;
      }
    };

  for (std::size_t p = 0; p <= alen; ++p) {
    for (const auto &ins : re.gaps[p])
      emit_insertion(ins);
    if (p == alen)
      break;

    const auto &te = re.tok_edits[p];
    switch (te.k) {
    case token_edit::kind::kept:
      if (!script.empty() &&
	  script.back()._op == mi_edit_script_op::op::copy &&
	  script.back()._copy.range.end == p) {
	++script.back()._copy.range.end;
      } else {
	script.push_back(mi_edit_script_op{
	    mi_edit_script_op::copy{
	      rescan_macro_invocation::output::tokens_range{p, p + 1}
	    }
	  });
      }
      add_traces(te.traces, t);
      t += 1;
      break;

    case token_edit::kind::deleted:
      break;

    case token_edit::kind::replaced:
      script.push_back(mi_edit_script_op{
	  mi_edit_script_op::replace_token{p, te.replacement}
	});
      add_traces(te.traces, t);
      t += te.replacement.size();
      break;

    case token_edit::kind::rewrite_macro_id:
      script.push_back(mi_edit_script_op{
	  mi_edit_script_op::rewrite_macro_id{p, te.rrmi}
	});
      t += 1;
      break;
    }
  }

  return script;
}

void rescan_macro_invocation_rewriter::_impl::
emit_insertion(const insertion &ins, std::vector<mat_token> &dst) const
{
  switch (ins.k) {
  case insertion::kind::ws:
    dst.push_back(mat_token{
	rewritten_macro_token{pp_token{pp_token::type::ws, " "}},
	{}
      });
    break;

  case insertion::kind::token:
    for (std::size_t k = 0; k < ins.toks.size(); ++k) {
      mat_token mt{rewritten_macro_token{normalize_src_token(ins.toks[k])}, {}};
      for (const auto &tr : ins.traces) {
	if (tr.first == k)
	  mt.traces.push_back(tr.second);
      }
      dst.push_back(std::move(mt));
    }
    break;
  }
}

void rescan_macro_invocation_rewriter::_impl::
emit_token_edit(const token_edit &te, const std::size_t p,
		std::vector<mat_token> &dst) const
{
  const pp_tokens &out_toks = _rmi.get_output().get_result();
  switch (te.k) {
  case token_edit::kind::kept:
    {
      // Placeholder empty tokens don't contribute anything.
      if (out_toks[p].is_empty())
	break;
      mat_token mt{rewritten_macro_token{normalize_src_token(out_toks[p])}, {}};
      for (const auto &tr : te.traces)
	mt.traces.push_back(tr.second);
      dst.push_back(std::move(mt));
    }
    break;

  case token_edit::kind::deleted:
    break;

  case token_edit::kind::replaced:
    for (std::size_t k = 0; k < te.replacement.size(); ++k) {
      mat_token mt{
	rewritten_macro_token{normalize_src_token(te.replacement[k])},
	{}
      };
      for (const auto &tr : te.traces) {
	if (tr.first == k)
	  mt.traces.push_back(tr.second);
      }
      dst.push_back(std::move(mt));
    }
    break;

  case token_edit::kind::rewrite_macro_id:
    dst.push_back(mat_token{
	rewritten_macro_token{out_toks[p], rewritten_m_of_rrmi(_forest, te.rrmi)},
	{}
      });
    break;
  }
}

std::vector<rescan_macro_invocation_rewriter::_impl::mat_token>
rescan_macro_invocation_rewriter::_impl::
materialize(const frag_info &fi, const bool include_head,
	    const bool include_tail) const
{
  std::vector<mat_token> mat;
  if (include_head) {
    for (const auto &ins : fi.head_ins)
      emit_insertion(ins, mat);
  }
  for (std::size_t p = fi.b(); p < fi.e(); ++p) {
    if (p != fi.b()) {
      for (const auto &ins : _gap_insertions[p])
	emit_insertion(ins, mat);
    }
    emit_token_edit(_tok_edits[p], p, mat);
  }
  if (include_tail) {
    for (const auto &ins : fi.tail_ins)
      emit_insertion(ins, mat);
  }
  return mat;
}

// Materialize a portion of an expanded argument reference's edited
// result: the positions [p_begin, p_end) of the argument's expansion
// result and the gaps [g_begin, g_end], relative to the fragment fi.
std::vector<rescan_macro_invocation_rewriter::_impl::mat_token>
rescan_macro_invocation_rewriter::_impl::
materialize_portion(const frag_info &fi, const arg_ref_edit &re,
		    const std::size_t p_begin, const std::size_t p_end,
		    const std::size_t g_begin, const std::size_t g_end) const
{
  std::vector<mat_token> mat;
  for (std::size_t p = p_begin; p <= p_end; ++p) {
    if (g_begin <= p && p <= g_end) {
      for (const auto &ins : re.gaps[p])
	emit_insertion(ins, mat);
    }
    if (p == p_end)
      break;
    emit_token_edit(re.tok_edits[p], fi.b() + p, mat);
  }
  return mat;
}

std::vector<rewritten_macro_token>
rescan_macro_invocation_rewriter::_impl::
finalize_mat_tokens(std::vector<mat_token> &&mat,
		    const rrmi_index context) const
{
  std::vector<rewritten_macro_token> toks;
  toks.reserve(mat.size());
  for (auto &mt : mat) {
    for (const auto t : mt.traces)
      finalize_trace(_forest, t, context);
    toks.push_back(std::move(mt.t));
  }
  return toks;
}

void rescan_macro_invocation_rewriter::_impl::
check_representable(const std::vector<rewritten_macro_token> &toks) const
{
  for (const auto &t : toks) {
    if (t.rename_ref != invalid_rewritten_macro_definition_id)
      continue;
    // '##' operators synthesized for pasting onto a parameter are
    // fine, c.f. rewritten_macro_token::is_pp_operator.
    if (t.is_pp_operator)
      continue;
    if (t.tok.is_punctuator("#") || t.tok.is_punctuator("##")) {
      throw macro_rewriting_failure{
	("cannot place '" + t.tok.get_value() +
	 "' token into rewritten definition of macro \"" +
	 _rmi.get_macro().get_name() + "\"")
      };
    } else if (t.tok.is_id() && t.tok.get_value() == "__VA_ARGS__" &&
	       _rmi.get_macro().is_variadic()) {
      throw macro_rewriting_failure{
	("cannot place '__VA_ARGS__' token into rewritten definition"
	 " of variadic macro \"" + _rmi.get_macro().get_name() + "\"")
      };
    }
  }
}

rescan_macro_invocation_rewriter::rewrite_progress::result
rescan_macro_invocation_rewriter::_impl::finalize()
{
  rewrite_progress::result res;
  for (const auto &c : _children)
    res.rewritten_rescan_macro_invocation_children.push_back(c.child_rrmi);
  for (const auto &c : _counters)
    res.rescan_subtree_counter_invocations.push_back(c.invocation);

  const pp_result::macro &m = _rmi.get_macro();
  const pp_tokens &out_toks = _rmi.get_output().get_result();
  const std::size_t L = out_toks.size();

  if (_whole_output_deleted) {
    // Delete the whole invocation from the source.
    _rrmi.set_deleted();
    return res;
  }

  const auto src_len = _rmi.get_src_range().length();

  // The invocation's name token is subject to rescan in the
  // enclosing context, like the arguments referenced from the
  // replacement list through their parameter.
  const auto mark_name_rescanned =
    [&]() {
      assert(res.src_edit_script.size() == 1);
      res.src_rescanned_ranges.append
	(rescan_macro_invocation_edit_script_rescanned_ranges::
	 mi_edit_script_rescanned_range{ .begin = 0, .end = 1 });
    };

  if (m.is_builtin_special()) {
    // Builtin special macros always expand to a single token. The
    // invocation's source consists of the name token only, unless the
    // macro takes arguments like __has_attribute() does.
    assert(L == 1);
    assert(src_len >= 1);
    bool identity = _tok_edits[0].k == token_edit::kind::kept;
    for (const auto &g : _gap_insertions) {
      if (!g.empty())
	identity = false;
    }
    if (identity) {
      res.src_edit_script.push_back(mi_edit_script_op{
	  mi_edit_script_op::copy{
	    rescan_macro_invocation::output::tokens_range{0, src_len}
	  }
	});
      // The arguments, if any, are retained as they are: mirror
      // their expansion trees in the rewritten forest.
      const std::size_t nargs = _rmi.get_nargs();
      for (std::size_t i = 0; i < nargs; ++i) {
	const auto orig_arg_root =
	  static_cast<rescan_macro_invocation>(_rmi.get_arg_tree(i));
	const auto arg_root =
	  _forest.add_arg_tree_root(static_cast<rrmi_index>(_rrmi), i,
				   orig_arg_root);
	_forest.clone_original_subtree(orig_arg_root,
				      static_cast<rrmi_index>(arg_root));
      }
      for (const auto &tr : _tok_edits[0].traces)
	finalize_trace(_forest, tr.second, static_cast<rrmi_index>(_rrmi));
      mark_name_rescanned();
      return res;
    }

    // The builtin's output token itself needs to change: replace the
    // builtin macro invocation by the desired tokens verbatim.
    std::vector<mat_token> mat;
    for (const auto &ins : _gap_insertions[0])
      emit_insertion(ins, mat);
    emit_token_edit(_tok_edits[0], 0, mat);
    for (const auto &ins : _gap_insertions[1])
      emit_insertion(ins, mat);
    // Only inserted or replacement tokens from the edit script
    // can end up here, never a renamed macro invocation's name
    // token: those are always rewrites of existing tokens.
    std::vector<pp_token> replacement;
    for (auto &mt : mat) {
      for (const auto t : mt.traces)
	finalize_trace(_forest, t, static_cast<rrmi_index>(_rrmi));
      assert(mt.t.rename_ref == invalid_rewritten_macro_definition_id);
      replacement.push_back(mt.t.tok);
    }
    res.src_edit_script.push_back(mi_edit_script_op{
	mi_edit_script_op::replace_token{0, std::move(replacement)}
      });
    mark_name_rescanned();
    return res;
  }

  // Mirror the argument trees in the rewritten _forest: one argument
  // tree root per argument, with
  // - the rewritten top level invocations from the argument tree
  //   cursor's result below it if any edits had been pushed down
  //   (or the rewrite had been forced),
  // - a clone of the original argument tree if the argument is in
  //   effect as is for any reference,
  // - and nothing otherwise (the argument is not referenced through
  //   the parameter anymore).
  // Lowered references' top level invocations get attached to the
  // rescan child consuming them as an argument, or to this invocation
  // otherwise.
  const std::size_t nargs = _rmi.get_nargs();
  // The rewritten argument tree roots and, per lowered reference
  // (keyed by fragment index), the rewritten node the lowered
  // content got attached to. These serve as contexts for finalizing
  // the _traces below.
  std::vector<rrmi_index> arg_root_rrmis;
  std::map<std::size_t, rrmi_index> lowered_contexts;
  {
    const auto link_top_level =
      [&](const rrmi_index parent_rrmi,
	  const std::vector<rrmi_index> &top_level,
	  const std::size_t *const pos_override) {
	auto parent = _forest.get_rewritten_rescan_macro_invocation(parent_rrmi);
	for (const auto child_rrmi : top_level) {
	  const std::size_t pos =
	    (pos_override ?
	     *pos_override :
	     (_forest.get_original_forest()
	      [_forest.get_rewritten_rescan_macro_invocation(child_rrmi)
	       .get_original()].get_src_range().begin));
	  parent._add_rescan_child(child_rrmi, pos);
	}
      };

    for (std::size_t i = 0; i < nargs; ++i) {
      const auto orig_arg_root =
	static_cast<rescan_macro_invocation>(_rmi.get_arg_tree(i));
      const auto arg_root =
	_forest.add_arg_tree_root(static_cast<rrmi_index>(_rrmi), i,
				 orig_arg_root);
      arg_root_rrmis.push_back(static_cast<rrmi_index>(arg_root));
      const auto &ai = _args[i];
      if (ai._arg_rewrites_index != std::numeric_limits<std::size_t>::max()) {
	link_top_level(static_cast<rrmi_index>(arg_root),
		       (_arg_rewrites[ai._arg_rewrites_index].result
			.top_level_rewritten_rescan_macro_invocations),
		       nullptr);
      } else if (ai.any_ref_kept ||
		 (ai.expanded_arg_refs.empty() &&
		  ai.deleted_arg_refs.empty())) {
	// The argument is referenced as is, or has never been
	// expanded in the first place. Otherwise, i.e. if all its
	// expanded references got removed, it isn't expanded anymore
	// and its expansion tree is not in effect.
	_forest.clone_original_subtree(orig_arg_root,
				      static_cast<rrmi_index>(arg_root));
      }

      // Attach the lowered content of a reference (the whole
      // reference or a served reference's outside portions) to the
      // rescan child consuming the reference's range as (part of) one
      // of its arguments, or to this invocation otherwise.
      const auto attach_lowered =
	[&](const std::size_t fi, const arg_rewrite &ar) {
	  const auto &finfo = _frags[fi];
	  rrmi_index parent_rrmi = static_cast<rrmi_index>(_rrmi);
	  std::size_t pos = finfo.b();
	  const std::size_t *pos_override = &pos;
	  const auto it_lc = lowered_contexts.find(fi);
	  if (it_lc != lowered_contexts.end()) {
	    parent_rrmi = it_lc->second;
	    if (parent_rrmi != static_cast<rrmi_index>(_rrmi))
	      pos_override = nullptr;
	  } else {
	    const auto st = static_cast<expansion_forest::rescan_subtree>(_rmi);
	    bool found = false;
	    for (auto it = st.dfs_pre_begin(); it != st.dfs_pre_end() && !found;
		 ++it) {
	      const rescan_macro_invocation c = *it;
	      if (c.is_arg_tree_root() ||
		  (static_cast<original_node>(c) ==
		   static_cast<original_node>(_rmi)) ||
		  (static_cast<original_node>(c.get_rescan_parent()) !=
		   static_cast<original_node>(_rmi))) {
		continue;
	      }
	      const std::size_t c_nargs = c.get_nargs();
	      for (std::size_t j = 0; j < c_nargs && !found; ++j) {
		const auto &r =
		  static_cast<rescan_macro_invocation>(c.get_arg_tree(j))
		  .get_src_range();
		if (r.begin <= finfo.b() && finfo.e() <= r.end) {
		  for (const auto &ch : _children) {
		    if (_forest.get_rewritten_rescan_macro_invocation(ch.child_rrmi)
			.get_original() == static_cast<original_node>(c)) {
		      const auto root = _forest.get_arg_tree_root(ch.child_rrmi, j);
		      if (root != rrmi_index::invalid()) {
			parent_rrmi = root;
			pos_override = nullptr;
			found = true;
		      }
		      break;
		    }
		  }
		}
	      }
	    }
	    lowered_contexts.emplace(fi, parent_rrmi);
	  }
	  link_top_level(parent_rrmi,
			 ar.result.top_level_rewritten_rescan_macro_invocations,
			 pos_override);
	};

      for (const auto &lr : ai.lower_rewrites)
	attach_lowered(lr.first, _arg_rewrites[lr.second]);
      for (const auto &sm : ai.served_material) {
	if (sm.second.left_lower != arg_info::none)
	  attach_lowered(sm.first, _arg_rewrites[sm.second.left_lower]);
	if (sm.second.right_lower != arg_info::none)
	  attach_lowered(sm.first, _arg_rewrites[sm.second.right_lower]);
      }
    }
  }


  // Assemble the definition level edits.
  rewritten_macro_definition::builder rewritten_m_builder{m};
  std::set<std::size_t> verbatim_frags;
  for (const auto &ai : _args) {
    // The argument's source gets modified iff something has been
    // pushed down into it, in which case any #x / ## references'
    // output would change.
    if (ai._arg_rewrites_index == std::numeric_limits<std::size_t>::max())
      continue;
    for (const auto fi : ai.operator_arg_refs)
      verbatim_frags.insert(fi);
  }

  const auto push_insertion_edit =
    [&](const raw_pp_token_index raw_pos, const insertion &ins) {
      std::vector<mat_token> mat;
      emit_insertion(ins, mat);
      auto toks = finalize_mat_tokens(std::move(mat),
				      static_cast<rrmi_index>(_rrmi));
      if (ins.k == insertion::kind::ws) {
	rewritten_m_builder.insert_ws(raw_pos, ins.stickiness);
      } else {
	check_representable(toks);
	rewritten_m_builder.insert(raw_pos, ins.stickiness, std::move(toks));
      }
    };

  // Definition level insertions are placed before resp. after the
  // fragment they're attached to. The builder orders edits at the
  // same position by insertion order, so emit those preceding their
  // fragment before any fragment's own material and the others after.
  for (const auto &dli : _def_level_insertions) {
    if (dli.second.stickiness != sticky_side::left)
      push_insertion_edit(dli.first, dli.second);
  }

  for (std::size_t fi = 0; fi < _frags.size(); ++fi) {
    const auto &finfo = _frags[fi];
    const auto &f = *finfo.f;
    const auto &repl = f.get_repl();
    switch (f.get_op()) {
    case out_fragment::op::repl:
      {
	assert(repl.length() == finfo.e() - finfo.b());
	for (std::size_t p = finfo.b(); p < finfo.e(); ++p) {
	  const raw_pp_token_index raw = repl.begin() + (p - finfo.b());
	  const auto &te = _tok_edits[p];
	  switch (te.k) {
	  case token_edit::kind::kept:
	    for (const auto &tr : te.traces)
	      finalize_trace(_forest, tr.second, static_cast<rrmi_index>(_rrmi));
	    break;

	  case token_edit::kind::deleted:
	    rewritten_m_builder.erase(raw_pp_tokens_range{raw, raw + 1});
	    break;

	  case token_edit::kind::replaced:
	    /* fall through */
	  case token_edit::kind::rewrite_macro_id:
	    {
	      std::vector<mat_token> mat;
	      emit_token_edit(te, p, mat);
	      auto toks = finalize_mat_tokens(std::move(mat),
					      static_cast<rrmi_index>(_rrmi));
	      check_representable(toks);
	      rewritten_m_builder.replace(raw_pp_tokens_range{raw, raw + 1},
				  std::move(toks));
	    }
	    break;
	  }

	  if (p != finfo.b()) {
	    for (const auto &ins : _gap_insertions[p])
	      push_insertion_edit(raw, ins);
	  }
	}
	for (const auto &ins : finfo.head_ins)
	  push_insertion_edit(repl.begin(), ins);
	for (const auto &ins : finfo.tail_ins)
	  push_insertion_edit(repl.begin() + repl.length(), ins);
      }
      break;

    case out_fragment::op::arg:
      {
	const std::size_t i = f.get_arg_ref_fragment().get_arg_index();
	const auto &ai = _args[i];
	if (std::find(ai.deleted_arg_refs.begin(), ai.deleted_arg_refs.end(),
		      fi) != ai.deleted_arg_refs.end()) {
	  // The reference's output got deleted completely, remove the
	  // reference itself, c.f. analyze().
	  rewritten_m_builder.erase(raw_pp_tokens_range{repl.begin(),
							repl.begin() + 1});
	  break;
	}
	const auto it_lower = ai.lower_rewrites.find(fi);
	const auto it_mat = ai.served_material.find(fi);

	// Materialize the lowered (rewritten) argument source of a
	// completed lowering rewrite and finalize its traces in the
	// context the lowered content got attached to.
	const auto lower_portion =
	  [&](const arg_rewrite &ar) {
	    assert(ar.started);
	    const auto arg_root =
	      static_cast<rescan_macro_invocation>(_rmi.get_arg_tree(i));
	    const auto src_toks = collect_src_tokens(_forest, arg_root, true);
	    auto toks = materialize_src_edit(_forest, ar.result.src_edit_script,
					     src_toks);
	    const auto it_lc = lowered_contexts.find(fi);
	    assert(it_lc != lowered_contexts.end());
	    for (const auto &tr :
		   ar.result.unexpanded_macro_id_out_tokens_origin_traces) {
	      finalize_trace(_forest, tr.unexpanded_macro_id_out_token,
			     it_lc->second);
	    }
	    return toks;
	  };

	if (std::find(ai.verbatim_arg_refs.begin(), ai.verbatim_arg_refs.end(), fi) !=
	    ai.verbatim_arg_refs.end()) {
	  // Place the reference's (edited) expansion result into the
	  // replacement list verbatim, c.f. analyze().
	  auto toks = finalize_mat_tokens(materialize(finfo, true, true),
					  static_cast<rrmi_index>(_rrmi));
	  check_representable(toks);
	  rewritten_m_builder.replace(raw_pp_tokens_range{repl.begin(),
							   repl.begin() + 1},
				      std::move(toks));
	} else if (it_lower != ai.lower_rewrites.end()) {
	  // Lower the (separately rewritten) argument into the
	  // replacement list in place of the reference.
	  auto toks = lower_portion(_arg_rewrites[it_lower->second]);
	  check_representable(toks);
	  rewritten_m_builder.replace(raw_pp_tokens_range{repl.begin(), repl.begin() + 1},
			      std::move(toks));
	} else {
	  // The argument is referenced through the parameter, either
	  // as is or with the common edits pushed down into it, with
	  // the range X of the expansion result retained in the
	  // argument. The reference's edited material outside of X, if
	  // any, gets placed next to the parameter reference, together
	  // with any wrap/paste tokens. A "don't care" reference,
	  // c.f. analyze(), stays as is, whatever X: the consuming
	  // rescan child ignores its content.
	  const bool dont_care =
	    (std::find(ai.dont_care_arg_refs.begin(),
		       ai.dont_care_arg_refs.end(), fi) !=
	     ai.dont_care_arg_refs.end());
	  const std::size_t alen = finfo.e() - finfo.b();
	  const std::size_t b = dont_care ? 0 : ai.x_begin;
	  const std::size_t e = dont_care ? alen : ai.x_end;
	  if (it_mat != ai.served_material.end()) {
	    assert(!dont_care);
	    const std::size_t r =
	      (std::find(ai.expanded_arg_refs.begin(),
			 ai.expanded_arg_refs.end(), fi) -
	       ai.expanded_arg_refs.begin());
	    assert(r < ai.ref_edits.size());
	    const arg_ref_edit &re = ai.ref_edits[r];
	    const auto &mat = it_mat->second;
	    std::vector<rewritten_macro_token> pre_toks;
	    std::vector<rewritten_macro_token> post_toks;
	    if (mat.left_lower != arg_info::none) {
	      pre_toks = lower_portion(_arg_rewrites[mat.left_lower]);
	    } else if (mat.left_verbatim) {
	      pre_toks = finalize_mat_tokens(materialize_portion(finfo, re, 0, b,
								0, b),
					     static_cast<rrmi_index>(_rrmi));
	    }
	    append_wrap_pre(mat.wrap_pre, mat.paste_prefix, pre_toks);
	    append_wrap_post(mat.paste_suffix, mat.wrap_post, post_toks);
	    if (mat.right_lower != arg_info::none) {
	      auto toks = lower_portion(_arg_rewrites[mat.right_lower]);
	      post_toks.insert(post_toks.end(),
			       std::make_move_iterator(toks.begin()),
			       std::make_move_iterator(toks.end()));
	    } else if (mat.right_verbatim) {
	      auto toks = finalize_mat_tokens(materialize_portion(finfo, re,
								 e, alen,
								 e, alen),
					      static_cast<rrmi_index>(_rrmi));
	      post_toks.insert(post_toks.end(),
			       std::make_move_iterator(toks.begin()),
			       std::make_move_iterator(toks.end()));
	    }
	    // The wrapped tokens' replacements appear in this
	    // invocation's output.
	    for (const auto t : mat.wrap_traces)
	      finalize_trace(_forest, t, static_cast<rrmi_index>(_rrmi));
	    check_representable(pre_toks);
	    check_representable(post_toks);
	    rewritten_m_builder.insert(repl.begin(), sticky_side::right,
				       std::move(pre_toks));
	    rewritten_m_builder.insert(repl.begin() + 1, sticky_side::left,
				       std::move(post_toks));
	  }

	  // Each reference is a separate copy of the argument's
	  // tokens in the output, with the recursing origins given by
	  // the emitting node within the argument's expansion tree
	  // (and its ancestors, including this invocation). Finalize
	  // the _traces of all the tokens within X attributed to this
	  // reference accordingly. Note that for pushed down edits,
	  // the first reference's _traces also travel with the
	  // argument tree cursor and get finalized there as well, with
	  // an equivalent context.
	  const auto arg_tree = _rmi.get_arg_tree(i);
	  const rrmi_index arg_root_rrmi = arg_root_rrmis[i];
	  // The rewritten node in this argument's context for an
	  // emitting node from the original argument tree.
	  const auto context_for =
	    [&](const rescan_macro_invocation &emitting) {
	      if (emitting.is_arg_tree_root())
		return arg_root_rrmi;
	      const auto r =
		_forest.find_rewritten_node_in_subtree
		  (static_cast<original_node>(emitting), arg_root_rrmi);
	      return r != rrmi_index::invalid() ? r : arg_root_rrmi;
	    };
	  const auto finalize_ins_traces =
	    [&](const std::vector<insertion> &inss, const rrmi_index context) {
	      for (const auto &ins : inss) {
		for (const auto &tr : ins.traces)
		  finalize_trace(_forest, tr.second, context);
	      }
	    };
	  const auto wrapped_at =
	    [&](const std::size_t rel) {
	      if (it_mat == ai.served_material.end())
		return false;
	      return ((rel == b && it_mat->second.wrap_first) ||
		      (rel + 1 == e && it_mat->second.wrap_last));
	    };
	  if (ai.x_head)
	    finalize_ins_traces(finfo.head_ins, arg_root_rrmi);
	  auto it_seg = arg_tree.final_out_begin();
	  const auto seg_end = arg_tree.final_out_end();
	  std::size_t seg_begin = 0;
	  std::size_t seg_length =
	    (it_seg != seg_end ?
	     (*it_seg).get_out_range_in_emitting_node().length() : 0);
	  for (std::size_t p = finfo.b() + b; p < finfo.b() + e; ++p) {
	    const std::size_t rel = p - finfo.b();
	    const auto &te = _tok_edits[p];
	    const bool tok_traces = !te.traces.empty() && !wrapped_at(rel);
	    const bool gap_traces =
	      (rel != b &&
	       std::any_of(_gap_insertions[p].begin(), _gap_insertions[p].end(),
			   [](const insertion &ins) {
			     return !ins.traces.empty();
			   }));
	    if (!tok_traces && !gap_traces)
	      continue;
	    while (it_seg != seg_end && rel >= seg_begin + seg_length) {
	      seg_begin += seg_length;
	      ++it_seg;
	      seg_length =
		(it_seg != seg_end ?
		 (*it_seg).get_out_range_in_emitting_node().length() : 0);
	    }
	    const rrmi_index context =
	      (it_seg != seg_end ?
	       context_for((*it_seg).get_emitting_node()) : arg_root_rrmi);
	    if (gap_traces)
	      finalize_ins_traces(_gap_insertions[p], context);
	    if (tok_traces) {
	      for (const auto &tr : te.traces)
		finalize_trace(_forest, tr.second, context);
	    }
	  }
	  if (ai.x_tail)
	    finalize_ins_traces(finfo.tail_ins, arg_root_rrmi);
	}
      }
      break;

    case out_fragment::op::stringification:
      /* fall through */
    case out_fragment::op::concat:
      /* fall through */
    case out_fragment::op::conditional_comma:
      {
	for (const auto &ins : finfo.head_ins)
	  push_insertion_edit(repl.begin(), ins);
	bool changed = verbatim_frags.count(fi) != 0;
	for (std::size_t p = finfo.b(); p < finfo.e() && !changed; ++p) {
	  // A placeholder empty result, e.g. from a conditional
	  // comma's removal, is no token to be edited.
	  if (_tok_edits[p].k != token_edit::kind::kept &&
	      !out_toks[p].is_empty()) {
	    changed = true;
	  }
	  if (p != finfo.b() && !_gap_insertions[p].empty())
	    changed = true;
	}
	bool wrapped = false;
	if (changed && f.get_op() == out_fragment::op::concat &&
	    finfo.e() == finfo.b() + 1 && !verbatim_frags.count(fi)) {
	  // A wrap edit on a concatenation's single identifier result
	  // can be implemented on the operands, keeping the definition
	  // independent of the particular arguments: the pre tokens
	  // and 'prefix ##' go in front of the '##' sequence, '##
	  // suffix' and the post tokens after it. The '##' operator is
	  // left-associative and the pastes operate on the same
	  // unexpanded operands as the original ones, so 'prefix ## x
	  // ## y ## suffix' yields exactly the edited token. For the
	  // result to be an identifier, its first constituent must be
	  // one as well, thus the prefix paste is valid.
	  const wrap_edit w = detect_wrap_edit(out_toks[finfo.b()],
					       _tok_edits[finfo.b()]);
	  if (w.valid) {
	    std::vector<rewritten_macro_token> pre_toks;
	    std::vector<rewritten_macro_token> post_toks;
	    append_wrap_pre(w.pre, w.prefix, pre_toks);
	    append_wrap_post(w.suffix, w.post, post_toks);
	    for (const auto &tr : w.traces)
	      finalize_trace(_forest, tr.second, static_cast<rrmi_index>(_rrmi));
	    check_representable(pre_toks);
	    check_representable(post_toks);
	    if (!pre_toks.empty()) {
	      rewritten_m_builder.insert(repl.begin(), sticky_side::right,
					 std::move(pre_toks));
	    }
	    if (!post_toks.empty()) {
	      rewritten_m_builder.insert(repl.begin() + repl.length(),
					 sticky_side::left,
					 std::move(post_toks));
	    }
	    wrapped = true;
	  }
	}
	if (changed && !wrapped) {
	  auto toks = finalize_mat_tokens(materialize(finfo, false, false),
					  static_cast<rrmi_index>(_rrmi));
	  check_representable(toks);
	  rewritten_m_builder.replace(raw_pp_tokens_range{repl.begin(),
						  repl.begin() + repl.length()},
			      std::move(toks));
	} else if (!changed) {
	  for (std::size_t p = finfo.b(); p < finfo.e(); ++p) {
	    for (const auto &tr : _tok_edits[p].traces)
	      finalize_trace(_forest, tr.second, static_cast<rrmi_index>(_rrmi));
	  }
	}
	for (const auto &ins : finfo.tail_ins)
	  push_insertion_edit(repl.begin() + repl.length(), ins);
      }
      break;

    case out_fragment::op::empty:
      break;

    case out_fragment::op::builtin_special:
      assert(0);
      __builtin_unreachable();
    }
  }

  for (const auto &dli : _def_level_insertions) {
    if (dli.second.stickiness == sticky_side::left)
      push_insertion_edit(dli.first, dli.second);
  }

  // A forced rename yields a rewritten definition even if the edits
  // didn't require one: an identical copy if the macro must not be
  // referenced by its original name (conflicts with macro nondef
  // constraints) and a definition forwarding to the original macro
  // otherwise.
  const bool force_copy_rename = _forest.is_copy_rename_forced(m);
  const bool force_forwarding_rename = _forest.is_forwarding_rename_forced(m);
  // Note that an empty builder is taken as the definite sign of
  // identity: is_identity() can give false negatives after
  // whitespace normalization, e.g. with continuation lines.
  bool need_forwarding = false;
  if (!rewritten_m_builder.empty() || force_copy_rename) {
    rewritten_macro_definition d =
      std::move(rewritten_m_builder).finalize(_forest.get_pp_result());
    if (force_copy_rename || !d.is_identity()) {
      const auto rewritten_m = _forest.get_pool().insert(std::move(d));
      _rrmi.set_rewritten_macro(rewritten_m);
    } else {
      need_forwarding = force_forwarding_rename;
    }
  } else {
    need_forwarding = force_forwarding_rename;
  }
  if (need_forwarding) {
    const auto rewritten_m =
      _forest.get_pool().insert
	(rewritten_macro_definition::make_forwarding
	   (m, _forest.get_pp_result()));
    _rrmi.set_rewritten_macro(rewritten_m);
  }
  const bool renamed =
    _rrmi.get_rewritten_macro() != invalid_rewritten_macro_definition_id;

  // Assemble the source edit script. The rescanned ranges mark the
  // name token as well as those argument sources which are
  // referenced from the replacement list through their parameter
  // (and not only as operands to # or ##) and thus, are subject to
  // macro expansion. In order to delimit these at the op level,
  // adjacent copy ops don't get merged across their boundaries.
  const std::size_t src_base = _rmi.get_src_range().begin;
  bool merge_barrier = false;
  const auto push_copy =
    [&](const std::size_t b, const std::size_t e) {
      if (b == e)
	return;
      if (!merge_barrier &&
	  !res.src_edit_script.empty() &&
	  res.src_edit_script.back()._op == mi_edit_script_op::op::copy &&
	  res.src_edit_script.back()._copy.range.end == b) {
	res.src_edit_script.back()._copy.range.end = e;
      } else {
	res.src_edit_script.push_back(mi_edit_script_op{
	    mi_edit_script_op::copy{
	      rescan_macro_invocation::output::tokens_range{b, e}
	    }
	  });
      }
      merge_barrier = false;
    };

  if (renamed) {
    res.src_edit_script.push_back(mi_edit_script_op{
	mi_edit_script_op::rewrite_macro_id{
	  0, static_cast<rrmi_index>(_rrmi)
	}
      });
  } else {
    push_copy(0, 1);
  }
  mark_name_rescanned();
  merge_barrier = true;
  std::size_t pos = 1;
  for (std::size_t i = 0; i < nargs; ++i) {
    const auto arg_root =
      static_cast<rescan_macro_invocation>(_rmi.get_arg_tree(i));
    assert(arg_root.get_src_range().begin >= src_base + pos);
    const std::size_t off = arg_root.get_src_range().begin - src_base;
    const std::size_t alen = arg_root.out_length();
    assert(off + alen <= src_len);
    push_copy(pos, off);
    const auto &ai = _args[i];
    // The argument is pre-expanded iff the rewritten replacement
    // list still references it through the parameter, i.e. unless
    // all references got lowered or replaced verbatim.
    const bool arg_rescanned = ai.any_ref_kept;
    merge_barrier = arg_rescanned;
    const std::size_t arg_ops_begin = res.src_edit_script.size();
    if (ai._arg_rewrites_index != std::numeric_limits<std::size_t>::max()) {
      auto &ar = _arg_rewrites[ai._arg_rewrites_index];
      assert(ar.started);
      for (auto &op : ar.result.src_edit_script) {
	switch (op._op) {
	case mi_edit_script_op::op::copy:
	  push_copy(op._copy.range.begin + off, op._copy.range.end + off);
	  continue;

	case mi_edit_script_op::op::replace_token:
	  op._replace_token.replaced_token_index += off;
	  break;

	case mi_edit_script_op::op::rewrite_macro_id:
	  op._rewrite_macro_id.rewritten_token_index += off;
	  break;

	case mi_edit_script_op::op::insert_token:
	  /* fall through */
	case mi_edit_script_op::op::insert_ws:
	  /* fall through */
	}
	res.src_edit_script.push_back(std::move(op));
      }
      // Tokens remaining in the argument's source are rescanned in
      // the argument's context.
      for (const auto &tr :
	     ar.result.unexpanded_macro_id_out_tokens_origin_traces) {
	finalize_trace(_forest, tr.unexpanded_macro_id_out_token,
		       arg_root_rrmis[i]);
      }
    } else {
      push_copy(off, off + alen);
    }
    if (arg_rescanned) {
      const std::size_t arg_ops_end = res.src_edit_script.size();
      if (arg_ops_end != arg_ops_begin) {
	res.src_rescanned_ranges.append
	  (rescan_macro_invocation_edit_script_rescanned_ranges::
	   mi_edit_script_rescanned_range{
	     .begin = arg_ops_begin,
	     .end = arg_ops_end,
	   });
      }
      merge_barrier = true;
    }
    pos = off + alen;
  }
  push_copy(pos, src_len);

  return res;
}

rescan_macro_invocation_rewriter::rewrite_progress
rescan_macro_invocation_rewriter::_impl::continue_rewrite()
{
  if (!_analyzed) {
    analyze();
    _analyzed = true;
  }

  while (_next_arg_rewrite < _arg_rewrites.size()) {
    auto &ar = _arg_rewrites[_next_arg_rewrite];
    if (!ar.started) {
      ar.started = true;
      auto arg_tree = _rmi.get_arg_tree(ar.arg_index);
      // The argument's expansion result gets rescanned as a whole.
      auto rescanned_ranges =
	(rescan_macro_invocation_edit_script_rescanned_ranges::covering_all
	 (ar.script.size()));
      std::unique_ptr<rescan_subtree_rewrite_cursor> cursor{
	new rescan_subtree_rewrite_cursor{
	  _forest,
	  std::move(arg_tree),
	  std::move(ar.script),
	  std::move(rescanned_ranges),
	  std::move(ar.traces),
	  ar.result,
	  _forest.get_macro_ids(),
	  false,
	  false
	}
      };
      return rewrite_progress{std::move(cursor)};
    }
    // The cursor handed out previously has completed.
    ++_next_arg_rewrite;
  }

  return rewrite_progress{finalize()};
}


rescan_macro_invocation_rewriter::rescan_macro_invocation_rewriter
  (rewritten_expansion_forest &rewritten_forest,
   rewritten_expansion_forest::rewritten_rescan_macro_invocation &&rrmi,
   rescan_macro_invocation_edit_script &&out_edit_script,
   rescan_macro_invocation_edit_script_rescanned_ranges out_rescanned_ranges,
   unexpanded_macro_id_out_tokens_origin_traces
     &&unexpanded_macro_id_out_tokens_origin_traces,
   std::vector<rewritten_rescan_macro_invocation_child>
     &&rewritten_rescan_macro_invocation_children,
   std::vector<descendant_rescan_counter_invocation>
     &&descendant_rescan_counter_invocations)
  : _i{std::make_unique<_impl>
       (rewritten_forest, std::move(rrmi), std::move(out_edit_script),
	std::move(out_rescanned_ranges),
	std::move(unexpanded_macro_id_out_tokens_origin_traces),
	std::move(rewritten_rescan_macro_invocation_children),
	std::move(descendant_rescan_counter_invocations))}
{}

rescan_macro_invocation_rewriter::~rescan_macro_invocation_rewriter() noexcept
  = default;

rescan_macro_invocation_rewriter::rewrite_progress
rescan_macro_invocation_rewriter::continue_rewrite()
{
  return _i->continue_rewrite();
}

rewritten_expansion_forest::rewritten_rescan_macro_invocation&
rescan_macro_invocation_rewriter::get_rewritten_rescan_macro_invocation()
  noexcept
{
  return _i->_rrmi;
}


void rewritten_macro_invocation::
add_src_edit_script_op(macro_invocation_src_edit_script_op &&op)
{
  using kind = macro_invocation_src_edit_script_op::kind;
  if (op.k == kind::literal &&
      !_src_edit_script.empty() &&
      _src_edit_script.back().k == kind::literal) {
    auto &toks = _src_edit_script.back().toks;
    toks.insert(toks.end(),
		std::make_move_iterator(op.toks.begin()),
		std::make_move_iterator(op.toks.end()));
    return;
  }
  _src_edit_script.push_back(std::move(op));
}

void rewritten_macro_invocation::add_used_macro(const used_macro &um)
{
  const bool dup =
    std::any_of(_used_macros.begin(), _used_macros.end(),
		[&](const used_macro &o) {
		  return o.m == um.m && o.rewritten == um.rewritten;
		});
  if (!dup)
    _used_macros.push_back(um);
}

void rewritten_macro_invocation::
add_macro_nondef_constraint(pp_result::macro_nondef_constraint &&mnc)
{
  _macro_nondef_constraints.push_back(std::move(mnc));
}

void rewritten_expansion_forest::
collect_result(rewritten_macro_invocation &result,
	       const rescan_macro_invocation_edit_script &src_edit_script,
	       const unexpanded_macro_id_out_tokens_origin_traces &src_traces)
{
  using src_item = macro_invocation_src_edit_script_op;

  // Tokens remaining in the raw invocation source are not subject to
  // any recursive expansion protection.
  for (const auto &tr : src_traces) {
    get_unexpanded_macro_id_out_token(tr.unexpanded_macro_id_out_token)
      .set_context(nodes_size_type::invalid());
  }

  const auto &inv_src = _original_forest.get_invocation_source();
  const auto total_range = inv_src.get_total_invocation_range();
  const raw_pp_tokens &raw_toks = _pp_result.get_raw_tokens();
  const auto src_token_at =
    [&](const std::size_t pos) -> pp_token {
      const auto segs = inv_src.overlapping_segments
	(rescan_macro_invocation::output::tokens_range{pos, pos + 1});
      assert(segs.first != segs.second);
      const auto s = *segs.first;
      const raw_pp_token &rt =
	raw_toks[s.raw_pp_tokens_begin + (pos - s.invocation_range.begin)];
      return pp_token{rt.get_type(), rt.get_value()};
    };

  const auto push_literal =
    [&](rewritten_macro_token &&t) {
      result.add_src_edit_script_op(src_item{
	  src_item::kind::literal,
	  rescan_macro_invocation::output::tokens_range{},
	  {std::move(t)}
	});
    };

  for (const auto &op : src_edit_script) {
    switch (op._op) {
    case rescan_macro_invocation_edit_script_op::op::copy:
      assert(op._copy.range.end <= total_range.length());
      result.add_src_edit_script_op(src_item{
	  src_item::kind::copy,
	  rescan_macro_invocation::output::tokens_range{
	    op._copy.range.begin + total_range.begin,
	    op._copy.range.end + total_range.begin
	  },
	  {}
	});
      break;

    case rescan_macro_invocation_edit_script_op::op::insert_token:
      push_literal(rewritten_macro_token{op._insert_token.token});
      break;

    case rescan_macro_invocation_edit_script_op::op::insert_ws:
      result.add_src_edit_script_op(src_item{
	  src_item::kind::ws,
	  rescan_macro_invocation::output::tokens_range{},
	  {}
	});
      break;

    case rescan_macro_invocation_edit_script_op::op::replace_token:
      for (const auto &t : op._replace_token.replacement)
	push_literal(rewritten_macro_token{t});
      break;

    case rescan_macro_invocation_edit_script_op::op::rewrite_macro_id:
      push_literal(rewritten_macro_token{
	  src_token_at(op._rewrite_macro_id.rewritten_token_index +
		       total_range.begin),
	  rewritten_m_of_rrmi(*this,
		      op._rewrite_macro_id.rewritten_rescan_macro_invocation)
	});
      break;

    }
  }

  // Used macros: every rewritten node not removed from the structure
  // uses either its (renamed) rewritten definition or the original
  // one. Original invocations never visited by any rewrite are not
  // in effect: each of their output tokens surviving the edit would
  // have been covered by some edit op and thus, would have made the
  // rewrite visit them. Their source has been dropped from the
  // enclosing rewritten definition, or, in case of macro argument
  // references placed verbatim, their expansion result has been
  // placed literally.
  const auto &nodes = _rewritten_forest._nodes;
  const auto rewritten_node_deleted =
    [&](_rewritten_forest_nodes::_container_type::size_type i) {
      for (;;) {
	if (nodes[i]._deleted)
	  return true;
	const nodes_size_type up = _rewritten_forest._up(nodes_size_type{i});
	if (up == nodes_size_type::invalid())
	  return false;
	i = up._n;
      }
    };
  for (_rewritten_forest_nodes::_container_type::size_type i = 0;
       i < nodes.size(); ++i) {
    const auto &node = nodes[i];
    if (_rewritten_forest._is_arg_tree_root(nodes_size_type{i}) ||
	rewritten_node_deleted(i)) {
      continue;
    }
    if (node._rewritten_m != invalid_rewritten_macro_definition_id) {
      result.add_used_macro(rewritten_macro_invocation::used_macro{
	  nullptr, node._rewritten_m
	});
      // A forwarding definition invokes the original macro by its
      // name when getting expanded.
      const rewritten_macro_definition &d = _pool[node._rewritten_m];
      if (d.is_forwarding()) {
	result.add_used_macro(rewritten_macro_invocation::used_macro{
	    &d.get_orig_macro(), invalid_rewritten_macro_definition_id
	  });
      }
    } else {
      result.add_used_macro(rewritten_macro_invocation::used_macro{
	  &_original_forest[node._original].get_macro(),
	  invalid_rewritten_macro_definition_id
	});
    }
  }

  // Macro nondef constraints from the unexpanded macro id tokens.
  for (const auto &t : _unexpanded_macro_id_out_tokens) {
    if (!t.is_constraint_active(*this))
      continue;
    result.add_macro_nondef_constraint
      (pp_result::macro_nondef_constraint{t.get_id(), !t.has_lparen()});
  }

  // With everything resolved, bring the nodes into their final order.
  finalize_structure();
}


bool rewrite_macro_invocation
	(const pp_result &pp_result,
	 const pp_result::macro_invocation &mi,
	 const std::vector<macro_invocation_output_edit_script_op> &out_edits,
	 const bool next_tok_is_lparen,
	 const std::map<std::string, bool> &macro_ids,
	 const std::set<const pp_result::macro*> &forced_renames,
	 rewritten_macro_definitions_pool &pool,
	 rewritten_macro_invocation &result,
	 std::string &error_msg)
{
  try {
    // A rewrite might render some identifier token in the output
    // unprotected from macro expansion due to all of its enclosing
    // invocations of the same-named macro getting renamed, resulting
    // in a macro_nondef_constraint for that name. If the same macro
    // is still invoked by its original name elsewhere in the
    // rewritten forest, that's a conflict: the constraint would
    // demand an #undef right at the point of use. Resolve such
    // conflicts by forcing a rename of all that macro's invocations
    // to identical copies and redoing the rewrite. Note that this
    // includes macros from forced_renames whose forwarding
    // definitions would reference the original name.
    std::set<const pp_result::macro*> copy_renames;
    while (true) {
      const std::size_t pool_size_before = pool.size();
      rewritten_macro_invocation attempt;
      {
	rewritten_expansion_forest forest{pp_result, mi, pool, macro_ids,
					  copy_renames, forced_renames};

	rescan_macro_invocation_edit_script script;
	for (const auto &e : out_edits) {
	  switch (e.k) {
	  case macro_invocation_output_edit_script_op::kind::copy:
	    if (e.r.begin == e.r.end)
	      break;
	    script.push_back(rescan_macro_invocation_edit_script_op{
		rescan_macro_invocation_edit_script_op::copy{
		  rescan_macro_invocation::output::tokens_range{e.r.begin,
								e.r.end}
		}
	      });
	    break;

	  case macro_invocation_output_edit_script_op::kind::replace:
	    assert(e.r.begin + 1 == e.r.end);
	    script.push_back(rescan_macro_invocation_edit_script_op{
		rescan_macro_invocation_edit_script_op::replace_token{
		  e.r.begin, e.new_toks
		}
	      });
	    break;

	  case macro_invocation_output_edit_script_op::kind::insert:
	    for (const auto &t : e.new_toks) {
	      script.push_back(rescan_macro_invocation_edit_script_op{
		  rescan_macro_invocation_edit_script_op::insert_token{
		    t, e.stickiness
		  }
		});
	    }
	    break;

	  case macro_invocation_output_edit_script_op::kind::insert_ws:
	    script.push_back(rescan_macro_invocation_edit_script_op{
		rescan_macro_invocation_edit_script_op::insert_ws{
		  .stickiness = e.stickiness
		}
	      });
	    break;
	  }
	}

	rescan_subtree_rewrite_cursor::result top_result;
	std::vector<std::unique_ptr<rescan_subtree_rewrite_cursor>> stack;
	auto main_tree = mi.get_expansion_forest().get_main_tree();
	// The final output tokens all are the result of rescans.
	auto rescanned_ranges =
	  (rescan_macro_invocation_edit_script_rescanned_ranges::covering_all
	   (script.size()));
	stack.emplace_back
	  (new rescan_subtree_rewrite_cursor{
	     forest,
	     std::move(main_tree),
	     std::move(script),
	     std::move(rescanned_ranges),
	     unexpanded_macro_id_out_tokens_origin_traces{},
	     top_result,
	     macro_ids,
	     next_tok_is_lparen,
	     true
	   });
	while (!stack.empty()) {
	  auto next = stack.back()->continue_rewrite();
	  if (next)
	    stack.push_back(std::move(next));
	  else
	    stack.pop_back();
	}

	forest.collect_result
	  (attempt, top_result.src_edit_script,
	   top_result.unexpanded_macro_id_out_tokens_origin_traces);
      }

      // Check for conflicts between the macro nondef constraints and
      // the macros still used by their original names.
      std::set<const pp_result::macro*> conflicting;
      for (const auto &um : attempt.get_used_macros()) {
	if (!um.m)
	  continue;
	for (const auto &mnc : attempt.get_macro_nondef_constraints()) {
	  if (mnc.get_name() == um.m->get_name() &&
	      (!um.m->is_func_like() || !mnc.is_func_like_allowed())) {
	    conflicting.insert(um.m);
	    break;
	  }
	}
      }
      if (conflicting.empty()) {
	result = std::move(attempt);
	return true;
      }

      if (std::includes(copy_renames.begin(), copy_renames.end(),
			conflicting.begin(), conflicting.end())) {
	// Should not happen, renaming all of a macro's invocations
	// removes it from the set of used macros.
	throw macro_rewriting_failure{
	  ("unresolvable macro nondef constraint conflict on \"" +
	   (*conflicting.begin())->get_name() + "\"")
	};
      }
      copy_renames.insert(conflicting.begin(), conflicting.end());
      pool.truncate(pool_size_before);
    }

  } catch (const macro_rewriting_failure &f) {
    error_msg = f.msg;
    return false;
  }
}

} // namespace ccp
} // namespace klp
