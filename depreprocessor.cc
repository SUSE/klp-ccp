/*
 * Copyright (C) 2019  SUSE Software Solutions Germany GmbH
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

#include "depreprocessor.hh"
#include "source_reader.hh"
#include "source_writer.hh"
#include "raw_pp_token.hh"
#include "callables_wrapper.hh"
#include "output_remarks.hh"

using namespace klp::ccp;

depreprocessor::_macro_undef_to_emit::
_macro_undef_to_emit(const pp_result::macro_undef &_original) noexcept
  : original(&_original)
{}

depreprocessor::_macro_undef_to_emit::
_macro_undef_to_emit(const std::string &_name)
  : original(nullptr), name(_name)
{}


bool depreprocessor::_macro_undef_to_emit::
operator<(const _macro_undef_to_emit& rhs) const noexcept
{
  if (this->original) {
    if (rhs.original) {
      if (!this->original->is_predefined()) {
	if (!rhs.original->is_predefined()) {
	  // Neither *this nor rhs are predefined.
	  return (this->original->get_directive_range() <
		  rhs.original->get_directive_range());
	} else {
	  // *this is not predefined, rhs is.
	  return false;
	}
      } else {
	if (!rhs.original->is_predefined()) {
	  // *this is predefined, rhs is not.
	  return true;
	} else {
	  // *this and rhs are predefined.
	  return (this->original->get_predefinition_pos() <
		  rhs.original->get_predefinition_pos());
	}
      }
    } else {
      // *this refers to an actual macro_undef from the input, rhs
      // doesn't.
      return true;
    }
  } else {
    if (rhs.original) {
      // *this doesn't refer to an actual macro_undef from the input,
      // rhs does.
      return false;
    } else {
      // Both, *this and rhs, don't refer to an actual macro_undef
      // from the input.  Compare by name as a last resort.
      return this->name < rhs.name;
    }
  }
}

bool depreprocessor::
_macro_undef_to_emit::operator<(const _macro_define_to_emit& rhs) const noexcept
{
  if (!this->original)
    return false;

  const pp_result::macro_undef &mu_lhs = *this->original;
  const pp_result::macro &md_rhs = rhs.m.get();
  if (!mu_lhs.is_predefined()) {
    if (!md_rhs.is_predefined())
      return mu_lhs.get_directive_range() < md_rhs.get_directive_range();
    else
      return false;
  } else {
    if (!md_rhs.is_predefined())
      return true;
    else
      return mu_lhs.get_predefinition_pos() < md_rhs.get_predefinition_pos();
  }
}


depreprocessor::_macro_define_to_emit::
_macro_define_to_emit(const pp_result::macro &_m) noexcept
  : m(_m)
{}

bool depreprocessor::_macro_define_to_emit::
operator<(const _macro_define_to_emit& rhs) const noexcept
{
  const pp_result::macro &m_lhs = this->m.get();
  const pp_result::macro &m_rhs = rhs.m.get();

  if (!m_lhs.is_predefined()) {
    if (!m_rhs.is_predefined()) {
      return (this->m.get().get_directive_range() <
	      rhs.m.get().get_directive_range());
    } else {
      return false;
    }
  } else {
    if (!m_rhs.is_predefined())
      return true;
    else
      return m_lhs.get_predefinition_pos() < m_rhs.get_predefinition_pos();
  }
}

bool depreprocessor::_macro_define_to_emit::
operator<(const _macro_undef_to_emit& rhs) const noexcept
{
  if (!rhs.original)
    return true;

  const pp_result::macro &md_lhs = this->m.get();
  const pp_result::macro_undef &mu_rhs = *rhs.original;
  if (!md_lhs.is_predefined()) {
    if (!mu_rhs.is_predefined())
      return md_lhs.get_directive_range() < mu_rhs.get_directive_range();
    else
      return false;
  } else {
    if (!mu_rhs.is_predefined())
      return true;
    else
      return md_lhs.get_predefinition_pos() < mu_rhs.get_predefinition_pos();
  }
}


source_reader& depreprocessor::_source_reader_cache::
get(const pp_result::header_inclusion_node &h)
{
  const auto id = h.get_id();
  if (_cache.size() <= id)
    _cache.resize(id + 1);

  if (!_cache[id])
    _cache[id] = h.create_source_reader();

  return *_cache[id];
}


depreprocessor::transformed_input_chunk::
transformed_input_chunk(const pp_tokens_range &bounding_r)
  : _bounding_r(bounding_r)
{}

void depreprocessor::transformed_input_chunk::
copy_subrange(const pp_tokens_range &r, const bool need_whitespace_before,
	      const pp_tokens &toks)
{
  if (r.begin == r.end)
    return;

  _ops_type::iterator it = _prepare_insert(r);

  // See if we can join the to be copied range to the one preceeding
  // it, if any. Two copy ops are joinable if they're separated only
  // by whitespace in the underlying pp_tokens sequence.
  // Any existing insert_ws ops will be skipped in the search for the
  // preceeding copy op below: it is assumed that all needed
  // whitespace will be found inbetween the two copied ranges joined
  // into one.
  auto &&tok_is_ws_or_newline =
    [](const pp_token &t) {
      return t.is_type_any_of<pp_token::type::ws,
			      pp_token::type::newline>();
    };

  auto &&is_insert_ws_op =
    [](const _op &o) {
      return (o.a == _op::action::insert_ws);
    };

  const auto rit_prec_op =
    std::find_if_not(std::reverse_iterator<_ops_type::iterator>{it},
		     _ops.rend(),
		     is_insert_ws_op);
  if (rit_prec_op != _ops.rend() && rit_prec_op->a == _op::action::copy
      && std::all_of(toks.begin() + rit_prec_op->r.end,
		     toks.begin() + r.begin,
		     tok_is_ws_or_newline)) {
    // Ok, found a preceeding copy op the new one can me merged into.
    // Remove any separating insert_ws op before.
    it = _ops.erase(rit_prec_op.base(), it);
    --it;
    it->r.end = r.end;

  } else {
    // Insert insert_ws op before if wanted and not already present.
    if (need_whitespace_before &&
	!(it != _ops.begin() && (it - 1)->a == _op::action::insert_ws &&
	  (it - 1)->r.end == r.begin)) {
      it = _ops.emplace(it, r.begin, _op::sticky_side::right);
    }

    it = _ops.emplace(it, r);
  }

  // Finally, try to merge any succeeding copy op to the one just
  // inserted.
  const auto it_succ_op =
    std::find_if_not(it + 1, _ops.end(), is_insert_ws_op);
  if (it_succ_op != _ops.end() && it_succ_op->a == _op::action::copy &&
      std::all_of(toks.begin() + r.end,
		  toks.begin() + it_succ_op->r.begin,
		  tok_is_ws_or_newline)) {
    // Merging is possible.
    it->r.end = it_succ_op->r.end;
    _ops.erase(it + 1, it_succ_op + 1);
  }
}

void depreprocessor::transformed_input_chunk::
purge_subrange(const pp_tokens_range &r, const bool need_whitespace_before)
{
  // This removes or adjusts any existing ops covered by the range.
  auto it = _prepare_insert(r);

  if (need_whitespace_before &&
      !(it != _ops.begin() && (it - 1)->a == _op::action::insert_ws &&
	(it - 1)->r.end == r.begin)) {
    _ops.emplace(it, r.begin, _op::sticky_side::left);
  }
}

void depreprocessor::transformed_input_chunk::
replace_token(const pp_token_index &index, pp_token &&repl_tok,
	      const bool add_pointer_deref)
{
  const pp_tokens_range r{index, index + 1};
  auto it = _prepare_insert(r);
  _ops.emplace(it, r, std::move(repl_tok), add_pointer_deref);
}

void depreprocessor::transformed_input_chunk::
insert_token(const pp_token_index &pos, pp_token &&new_tok,
	     const bool need_whitespace_before,
	     const bool need_whitespace_after)
{
  auto it = _prepare_insert(pp_tokens_range{pos, pos});

  const _op::sticky_side stickiness =
    (need_whitespace_before ? _op::sticky_side::left :
     (need_whitespace_after ? _op::sticky_side::right :
      _op::sticky_side::left));

  if (need_whitespace_before &&
      !(it != _ops.begin() && (it - 1)->a == _op::action::insert_ws &&
	(it - 1)->r.end == pos)) {
    it = _ops.emplace(it, pos, stickiness) + 1;
  }

  it = _ops.emplace(it, pos, std::move(new_tok), stickiness);

  // insert and insert_ws ops have zero length ranges associated with
  // them. When given a point range, _prepare_insert() always returns
  // an iterator pointing past the last such op being located before
  // or at this point. In particular, there can't be an insert_ws op
  // with ->r.begin == pos after the element just inserted.
  if (need_whitespace_after)
    _ops.emplace(it + 1, pos, stickiness);
}

depreprocessor::transformed_input_chunk
depreprocessor::transformed_input_chunk::
split_head_off(const pp_tokens_range &r)
{
  assert(r.is_contained_in(_bounding_r));

  auto ops_tail_begin = _prepare_insert(r);
  transformed_input_chunk head_tic{pp_tokens_range{_bounding_r.begin, r.begin}};
  head_tic._ops.insert(head_tic._ops.begin(),
		       std::make_move_iterator(_ops.begin()),
		       std::make_move_iterator(ops_tail_begin));
  _bounding_r.begin = r.end;
  _ops.erase(_ops.begin(), ops_tail_begin);
  return head_tic;
}

depreprocessor::transformed_input_chunk::_pos_in_chunk
depreprocessor::transformed_input_chunk::_begin_pos_in_chunk() const noexcept
{
  assert(!_ops.empty());
  const _op &first_op = _ops.front();
  if (first_op.a == _op::action::copy)
    return _pos_in_chunk{0, first_op.r.begin};
  else
    return _pos_in_chunk{0, 0};
}

depreprocessor::transformed_input_chunk::_pos_in_chunk
depreprocessor::transformed_input_chunk::_end_pos_in_chunk() const noexcept
{
  return _pos_in_chunk{_ops.size(), 0};
}

depreprocessor::transformed_input_chunk::_pos_in_chunk
depreprocessor::transformed_input_chunk::
_next_pos_in_chunk(const _pos_in_chunk &cur_pos)const noexcept
{
  assert(cur_pos.op < _ops.size());
  const _op &cur_op = _ops[cur_pos.op];
  switch (cur_op.a) {
  case _op::action::copy:
    if (cur_pos.tok + 1 < cur_op.r.end)
      return _pos_in_chunk{cur_pos.op, cur_pos.tok + 1};
    /* fall through */
  case _op::action::replace:
    /* fall through */
  case _op::action::insert:
    /* fall through */
  case _op::action::insert_ws:
    if (cur_pos.op + 1 < _ops.size() &&
	_ops[cur_pos.op + 1].a == _op::action::copy) {
      return _pos_in_chunk{
		cur_pos.op + 1,
		_ops[cur_pos.op + 1].r.begin
	     };
    } else {
      return _pos_in_chunk{cur_pos.op + 1, 0};
    }
  }
}

std::pair<depreprocessor::transformed_input_chunk::_ops_type::const_iterator,
	  depreprocessor::transformed_input_chunk::_ops_type::const_iterator>
depreprocessor::transformed_input_chunk::
_find_overlapping_ops_range(const raw_pp_tokens_range &r,
			    const pp_result &pp_result) const noexcept
{
  struct comp_op_range
  {
    comp_op_range(const class pp_result &_pp_result) noexcept
      : pp_result(_pp_result)
    {}

    bool operator()(const _op &op, const raw_pp_tokens_range &r)
    {
      return (op.get_range_raw(pp_result) < r);
    }

    bool operator()(const raw_pp_tokens_range &r, const _op &op)
    {
      return (r < op.get_range_raw(pp_result));
    }

    const class pp_result &pp_result;
  };

  return std::equal_range(_ops.begin(), _ops.end(), r,
			  comp_op_range{pp_result});
}

depreprocessor::transformed_input_chunk::_pos_in_chunk
depreprocessor::transformed_input_chunk::
_directive_range_to_pos_in_chunk(const raw_pp_tokens_range &directive_range,
				 const pp_result &pp_result)
  const noexcept
{
  // Preprocessor directives' raw_pp_tokens_ranges never intersect
  // with macro expansions and can always be associated with locations
  // inbetween preprocessed pp_tokens and thus, with some
  // _pos_in_chunk.
  const auto overlapping_ops = _find_overlapping_ops_range(directive_range,
							   pp_result);
  if (overlapping_ops.first == _ops.end()) {
    return _end_pos_in_chunk();
  } else if (overlapping_ops.first == overlapping_ops.second) {
    // The op at overlapping_ops.first comes strictly after
    // directive_range.
    return _pos_in_chunk {
	     static_cast<_ops_type::size_type>(overlapping_ops.first -
					       _ops.begin()),
	     (overlapping_ops.first->a == _op::action::copy ?
	      overlapping_ops.first->r.begin : 0)
	   };
  }

  // The op at overlapping_ops.first is located neither completely before
  // nor completely after directive_range. Hence it must span more
  // than one pp_token and this is possible only for copy ops. Find
  // the spot beween the two consecutive pp_tokens where the
  // directive_range is located.
  assert(overlapping_ops.second - overlapping_ops.first == 1);
  assert(overlapping_ops.first->a == _op::action::copy);
  const pp_tokens_range &op_range= overlapping_ops.first->r;
  assert(op_range.end - op_range.begin > 1);
  const pp_tokens &toks = pp_result.get_pp_tokens();
  auto it_first_tok_after
    = std::upper_bound(toks.begin() + op_range.begin,
		       toks.begin() + op_range.end,
		       directive_range,
		       [&](const raw_pp_tokens_range &r, const pp_token &tok) {
			 return r < tok.get_token_source();
		       });
  return _pos_in_chunk {
	   static_cast<_ops_type::size_type>(overlapping_ops.first -
					     _ops.begin()),
	   static_cast<pp_token_index>(it_first_tok_after - toks.begin())
	 };
}

depreprocessor::transformed_input_chunk::_ops_type::iterator
depreprocessor::transformed_input_chunk::
_prepare_insert(const pp_tokens_range &subrange)
{
  assert(subrange.begin >= this->_bounding_r.begin);
  assert(subrange.end <= this->_bounding_r.end);

  // The ops are sorted by their respective range. Search for
  // existing ops overlapping with the to be inserted op's
  // subrange and make room by shrinking/splitting/removing.
  struct comp_op_range
  {
    bool operator()(const _op &op, const pp_tokens_range &r) const noexcept
    { return op < r; }
    bool operator()(const pp_tokens_range &r, const _op &op) const noexcept
    { return op > r; }
  };
  auto it_overlap
    = std::equal_range(_ops.begin(), _ops.end(), subrange, comp_op_range{});
  if (it_overlap.first != it_overlap.second) {
    assert(!(*it_overlap.first < subrange));
    assert(subrange.begin < it_overlap.first->r.end);
    if (it_overlap.first->r.begin < subrange.begin) {
      if (subrange.end < it_overlap.first->r.end) {
	assert(it_overlap.first + 1 == it_overlap.second);
	auto new_tail_op = *it_overlap.first;
	assert(new_tail_op.a != _op::action::replace &&
	       new_tail_op.a != _op::action::insert);
	new_tail_op.r.begin = subrange.end;
	it_overlap.first->r.end = subrange.begin;
	it_overlap.first = _ops.insert(it_overlap.first + 1,
				       std::move(new_tail_op));
	it_overlap.second = it_overlap.first;

      } else {
	it_overlap.first->r.end = subrange.begin;
	++it_overlap.first;

      }
    }

    if (it_overlap.first != it_overlap.second) {
      const auto it_last = it_overlap.second - 1;
      assert(!(subrange < it_last->r));
      assert(it_last->r.begin < subrange.end);
      if (subrange.end < it_last->r.end) {
	it_last->r.begin = subrange.end;
	--it_overlap.second;
      }

      if (it_overlap.first != it_overlap.second) {
#ifndef NDEBUG
	for (auto it_erased = it_overlap.first;
	     it_erased != it_overlap.second;
	     ++it_erased) {
	  assert(it_erased->a != _op::action::replace &&
		 it_erased->a != _op::action::insert);
	}
#endif
	it_overlap.first = _ops.erase(it_overlap.first, it_overlap.second);
	it_overlap.second = it_overlap.first;
      }
    }
  }

  assert(it_overlap.first == it_overlap.second);
  auto it = it_overlap.first;
  assert(it == _ops.end() || subrange.end <= it->r.begin);
  while (it != _ops.end() && it->r.end == subrange.begin) {
    // Always order successive equal point ranges by their insertion
    // order.
    assert(subrange.begin == subrange.end);
    assert(it->r.begin == it->r.end);
    assert(it->r == subrange);
    ++it;
  }

  return it;
}

pp_tokens_range depreprocessor::transformed_input_chunk::_get_range()
  const noexcept
{
  assert(!_ops.empty());
  return pp_tokens_range{_ops.front().r.begin, _ops.back().r.end};
}

raw_pp_tokens_range depreprocessor::transformed_input_chunk::
_get_range_raw(const pp_result &pp_result) const noexcept
{
  assert(!_ops.empty());
  return raw_pp_tokens_range{
		_ops.front().get_range_raw(pp_result).begin,
		_ops.back().get_range_raw(pp_result).end
	 };
}

void depreprocessor::transformed_input_chunk::_trim()
{
  // Strip any leading or trailining insert_ws ops and collate
  // multiple consecutive ones.
  auto &&is_insert_ws_op =
    [](const _op &o) {
      return (o.a == _op::action::insert_ws);
    };

  if (_ops.empty())
    return;

  auto insert_ws_begin = std::find_if(_ops.begin(), _ops.end(),
				      is_insert_ws_op);
  if (insert_ws_begin == _ops.begin()) {
    // Leading insert_ws ops. Strip them all.
    auto insert_ws_end = std::find_if_not(insert_ws_begin + 1, _ops.end(),
					  is_insert_ws_op);
    insert_ws_end = _ops.erase(insert_ws_begin, insert_ws_end);
    insert_ws_begin = std::find_if(insert_ws_end, _ops.end(), is_insert_ws_op);
  }

  while (insert_ws_begin != _ops.end()) {
    auto insert_ws_end = std::find_if_not(insert_ws_begin + 1, _ops.end(),
					  is_insert_ws_op);

    if (insert_ws_end == _ops.end()) {
      // Trailing insert_ws ops. Strip them all.
      _ops.erase(insert_ws_begin, insert_ws_end);
      return;
    }

    // Multiple consecutive insert_ws ops. Strip all but the first.
    insert_ws_end = _ops.erase(insert_ws_begin + 1, insert_ws_end);
    insert_ws_begin = std::find_if(insert_ws_end, _ops.end(),  is_insert_ws_op);
  }
}

bool depreprocessor::transformed_input_chunk::
_find_macro_constraints(const pp_result &pp_result,
			bool next_raw_tok_is_opening_parenthesis)
{
  const raw_pp_tokens_range range_raw = _get_range_raw(pp_result);
  const auto mis = pp_result.find_overlapping_macro_invocations(range_raw);

  // Function-like macros get expanded by the preprocessor depending
  // on whether or not a referencing identifier token is followed by
  // an opening parenthesis. Thus, when determining the
  // ->is_func_like_allowed property of macro_nondef_constraints, we
  // will have to look ahead at the next non-whitespace token and will
  // have to check whether the macro invocation that lookahead token
  // originated from, if any, will be retained in the final output or
  // not. Process the chunk in reverse order.
  auto _it_mi = mis.second;
  for (auto _it_op = _ops.end(); _it_op != _ops.begin(); --_it_op) {
    const auto it_op = _it_op - 1;

    switch (it_op->a) {
    case _op::action::insert_ws:
      break;

    case _op::action::insert:
      if (it_op->new_tok.is_id()) {
	_macro_nondef_constraints_in_chunk.emplace_back
	  (_pos_in_chunk{(static_cast<_ops_type::size_type>
			  (it_op - _ops.begin())),
			 0},
	   pp_result::macro_nondef_constraint{
	     it_op->new_tok.get_value(),
	     !next_raw_tok_is_opening_parenthesis});
	next_raw_tok_is_opening_parenthesis = false;
      } else {
	next_raw_tok_is_opening_parenthesis =
	  it_op->new_tok.is_punctuator("(");
      }
      break;

    case _op::action::replace:
      if (it_op->new_tok.is_id()) {
	if (it_op->add_pointer_deref) {
	  // If a pointer dereference operation is to be added to the
	  // id token, it will be surrounded by (* ... ). Hence the
	  // token following the id is not an opening parenthesis.
	  next_raw_tok_is_opening_parenthesis = false;
	}
	_macro_nondef_constraints_in_chunk.emplace_back
	  (_pos_in_chunk{(static_cast<_ops_type::size_type>
			  (it_op - _ops.begin())),
			 0},
	   pp_result::macro_nondef_constraint{
	     it_op->new_tok.get_value(),
	     !next_raw_tok_is_opening_parenthesis});
	next_raw_tok_is_opening_parenthesis = it_op->add_pointer_deref;
      } else {
	if (it_op->add_pointer_deref) {
	  next_raw_tok_is_opening_parenthesis = true;

	} else {
	  next_raw_tok_is_opening_parenthesis =
	    it_op->new_tok.is_punctuator("(");
	}
      }
      break;

    case _op::action::copy:
      {
	const raw_pp_tokens_range cur_copy_range_raw
	  = it_op->get_range_raw(pp_result);
	auto directives =
	  pp_result.find_overlapping_directives(cur_copy_range_raw);
	pp_token_index _cur_tok_index = it_op->r.end;
	while (_cur_tok_index != it_op->r.begin) {
	  // Handle some pp_token which either doesn't originate from any
	  // macro expansion or which does, but from a macro invocation
	  // which can't get retained in the output.
	  auto handle_nonexpanded_token
	    = [&](const pp_token &tok, const pp_token_index pos) {
		if (tok.is_id()) {
		  const _pos_in_chunk pos_in_chunk{
			static_cast<_ops_type::size_type>(it_op - _ops.begin()),
			pos
		  };

		  _macro_nondef_constraints_in_chunk.emplace_back
		    (pos_in_chunk,
		     pp_result::macro_nondef_constraint{
		       tok.get_value(),
		       !next_raw_tok_is_opening_parenthesis
		     });

		  next_raw_tok_is_opening_parenthesis = false;

		} else if (tok.is_punctuator("(")) {
		  next_raw_tok_is_opening_parenthesis = true;

		} else if (!tok.is_type_any_of<pp_token::type::ws,
					       pp_token::type::newline>()) {
		  next_raw_tok_is_opening_parenthesis = false;
		}
	      };

	  const pp_token_index cur_tok_index = _cur_tok_index - 1;
	  const pp_tokens &toks = pp_result.get_pp_tokens();
	  const pp_token &cur_tok = toks[cur_tok_index];

	  // Ignore all macro invocations after the current token if
	  // they are located after the copy op alltogether or are
	  // part of some preprocessor directive within the copy op's
	  // range, which won't get emitted.
	  while (_it_mi != mis.first &&
		 (cur_tok.get_token_source() <
		  (_it_mi - 1)->get_source_range())) {
	    const raw_pp_tokens_range &mi_range_raw =
	      (_it_mi - 1)->get_source_range();

	    // Move the directives iterator to at or before the macro
	    // invocation.
	    while (directives.second != directives.first &&
		   (mi_range_raw <
		    (directives.second - 1)->get_source_range())) {
	      --directives.second;
	    }

	    if ((_cur_tok_index != it_op->r.end) &&
		(directives.second == directives.first ||
		 ((directives.second - 1)->get_source_range() <
		  mi_range_raw))) {
	      break;
	    } else {
	      --_it_mi;
	    }
	  }

	  if (_it_mi == mis.first ||
	      ((_it_mi - 1)->get_source_range() < cur_tok.get_token_source())) {
	    // The current pp_token is not from any macro invocation.
	    // Add macro_nondef constraints as appropriate.
	    handle_nonexpanded_token(cur_tok, cur_tok_index);
	    --_cur_tok_index;

	  } else {
	    // The current pp_token within the copy op's range comes
	    // from some macro expansion. See if the macro invocation
	    // can be retained.
	    const auto it_mi = _it_mi - 1;
	    const pp_tokens_range invocation_range =
	      (pp_result.raw_pp_tokens_range_to_nonraw
	       (it_mi->get_source_range()));

	    if (invocation_range.is_contained_in(it_op->r)) {
	      // This macro invocation is completely contained within some
	      // copied range and can be retained.
	      const _pos_in_chunk mi_pos{
			static_cast<_ops_type::size_type>(it_op - _ops.begin()),
			invocation_range.begin
	      };

	      for (const pp_result::macro &m : it_mi->get_used_macros())
		_used_macros_in_chunk.emplace_back(mi_pos, m);

	      for (const pp_result::macro_nondef_constraint &mnc
		     : it_mi->get_macro_nondef_constraints()) {
		_macro_nondef_constraints_in_chunk.emplace_back
		  (mi_pos, pp_result::macro_nondef_constraint{mnc});
	      }

	      // If the last token from the macro expansion is some id
	      // token, it could still combine with a subsequent
	      // opening parenthesis to form another, func-like macro
	      // invocation. The preprocessor would have added a
	      // macro_nondef_constraint already, but depending on the
	      // original context, that might be too lax and have
	      // ->func_like_allowed set.
	      if (next_raw_tok_is_opening_parenthesis) {
		auto non_ws_end = invocation_range.end;
		while (non_ws_end != invocation_range.begin &&
		       (toks[non_ws_end - 1]
			.is_type_any_of<pp_token::type::ws,
					pp_token::type::newline>())) {
		  --non_ws_end;
		}
		if (non_ws_end != invocation_range.begin &&
		    toks[non_ws_end - 1].is_id()) {
		  const std::string &id = toks[non_ws_end - 1].get_value();

		  // See if the recursive macro evaluation rules have
		  // prevented this identifier from going through
		  // macro expansion again.
		  bool id_used_as_macro = false;
		  for (const auto &um : it_mi->get_used_macros()) {
		    if (um.get_name() == id) {
		      id_used_as_macro = true;
		      break;
		    }
		  }

		  if (!id_used_as_macro) {
		    _macro_nondef_constraints_in_chunk.emplace_back
		      (mi_pos, pp_result::macro_nondef_constraint{id, false});
		  }
		}
	      }

	      --_it_mi;
	      _cur_tok_index = invocation_range.begin;
	      next_raw_tok_is_opening_parenthesis = false;

	    } else {
	      // This macro invocation can't get retained as is
	      // because it isn't covered completely by the copy op's
	      // range.

	      // Crop the macro invocation range to the copied range.
	      const pp_tokens_range intersecting_range{
		std::max(invocation_range.begin, it_op->r.begin),
		std::min(invocation_range.end, it_op->r.end)
	      };

	      // Add a record that the expanded tokens in this range
	      // shall be emitted verbatim.
	      _ranges_to_emit_expanded.push_back(intersecting_range);

	      // Treat the intersecting pp_tokens as if they didn't
	      // originate from any macro invocation at all, in
	      // particular, register macro_nondef_constraints as
	      // appropriate.
	      assert(intersecting_range.end == _cur_tok_index);
	      while (_cur_tok_index > intersecting_range.begin) {
		handle_nonexpanded_token(toks[_cur_tok_index - 1],
					 _cur_tok_index - 1);
		--_cur_tok_index;
	      }

	      // The invocation_range might also overlap with some
	      // preceeding copy operation. Give it a chance to
	      // record the intersection if that's the case.
	      if (invocation_range.begin >= it_op->r.begin)
		--_it_mi;
	    }
	  }
	}
      }
      break;
    }
  }

  // Bring the registered records into non-reversed order.
  std::reverse(_used_macros_in_chunk.begin(), _used_macros_in_chunk.end());
  std::reverse(_macro_nondef_constraints_in_chunk.begin(),
	       _macro_nondef_constraints_in_chunk.end());
  std::reverse(_ranges_to_emit_expanded.begin(),
	       _ranges_to_emit_expanded.end());

  // Finally remove redundancies in _used_macros_in_chunk
  // and _macro_nondef_constraints_in_chunk:
  // - Keep only the first of two _used_macros_in_chunk records for
  //   the same macro instance with no corresponding
  //   macro_nondef_constraint inbetween.
  // - Keep only one of the _macro_nondef_constraints_in_chunk records
  //   for each name at a single pos_in_chunk. Note that
  //   macro_nondef_constraints for the same name at different
  //   positions must not get merged, because the second one might not
  //   exist in the original code and the later stages could try to
  //   make use of a matching #define found inbetween, if any.
  auto it_mnc = _macro_nondef_constraints_in_chunk.begin();
  auto it_um = _used_macros_in_chunk.begin();
  while(it_mnc != _macro_nondef_constraints_in_chunk.end() ||
	it_um != _used_macros_in_chunk.end()) {
    while (it_mnc != _macro_nondef_constraints_in_chunk.end() &&
	   (it_um == _used_macros_in_chunk.end() ||
	    it_mnc->pos < it_um->pos)) {
      auto it_prev_mnc = it_mnc;
      while (it_prev_mnc != _macro_nondef_constraints_in_chunk.begin() &&
	     (it_prev_mnc - 1)->pos == it_mnc->pos &&
	     (it_prev_mnc - 1)->mnc.get_name() != it_mnc->mnc.get_name()) {
	--it_prev_mnc;
      }
      if (it_prev_mnc != _macro_nondef_constraints_in_chunk.begin() &&
	  (it_prev_mnc - 1)->pos == it_mnc->pos) {
	// Previous macro_nondef_constraint for the same name and
	// this position.
	--it_prev_mnc;
	if (!it_mnc->mnc.is_func_like_allowed())
	  std::swap(*it_prev_mnc, *it_mnc);
	it_mnc = _macro_nondef_constraints_in_chunk.erase(it_mnc);
      } else {
	++it_mnc;
      }
    }

    if (it_um != _used_macros_in_chunk.end()) {
      // Search for a previous usage of the same macro in this chunk.
      auto it_prev_um = it_um;
      while (it_prev_um != _used_macros_in_chunk.begin() &&
	     &(it_prev_um - 1)->m.get() != &it_um->m.get()) {
	--it_prev_um;
      }

      if (it_prev_um != _used_macros_in_chunk.begin()) {
	--it_prev_um;
	// See if there's a matching macro_nondef_constraint record
	// inbetween.
	bool matching_mnc_inbetween = false;
	for (auto it_mnc_inbetween = it_mnc;
	     it_mnc_inbetween != _macro_nondef_constraints_in_chunk.begin() &&
	     it_prev_um->pos < (it_mnc_inbetween - 1)->pos;
	     --it_mnc_inbetween) {
	  if ((it_mnc_inbetween - 1)->mnc.get_name() ==
	      it_um->m.get().get_name()) {
	    matching_mnc_inbetween = true;
	    break;
	  }
	}

	if (matching_mnc_inbetween)
	  it_um = _used_macros_in_chunk.erase(it_um);
	else
	  ++it_um;
      } else {
	++it_um;
      }
    }
  }

  _used_macros_in_chunk.shrink_to_fit();
  _macro_nondef_constraints_in_chunk.shrink_to_fit();
  _ranges_to_emit_expanded.shrink_to_fit();

  return next_raw_tok_is_opening_parenthesis;
}

void depreprocessor::transformed_input_chunk::
_add_macro_undef_to_emit(const _pos_in_chunk &pos, _macro_undef_to_emit &&mu)
{
  _macro_undef_to_emit_in_chunk mu_in_chunk{pos, std::move(mu)};
  const auto it = std::upper_bound(_macro_undefs_to_emit.begin(),
				   _macro_undefs_to_emit.end(),
				   mu_in_chunk);
  _macro_undefs_to_emit.insert(it, std::move(mu_in_chunk));
}

void depreprocessor::transformed_input_chunk::
_add_macro_define_to_emit(const _pos_in_chunk &pos, _macro_define_to_emit &&m)
{
  _macro_define_to_emit_in_chunk md_in_chunk{pos, std::move(m)};
  const auto it = std::upper_bound(_macro_defines_to_emit.begin(),
				   _macro_defines_to_emit.end(),
				   md_in_chunk);
  _macro_defines_to_emit.insert(it, std::move(md_in_chunk));
}

void depreprocessor::transformed_input_chunk::
_write(source_writer &writer, const pp_result &pp_result,
       _source_reader_cache &source_reader_cache,
       output_remarks &remarks) const
{
  // In what follows, iterate through the _ops and write out each one
  // as appropriate. Macro #defines and #undefs might have to get emitted
  // inbetween consecutive ops, or, in the case of copy ops, even at some
  // place inside those.
  bool skip_following_insert_ws = true;
  bool last_was_newline = true;

  _pos_in_chunk cur_pos = _begin_pos_in_chunk();
  auto it_mu = _macro_undefs_to_emit.cbegin();
  auto it_md = _macro_defines_to_emit.cbegin();
  auto write_mus_and_mds_at_cur_pos =
    [&]() {
      auto write_md =
	[&](const _macro_define_to_emit &md) {
	  if (!last_was_newline)
	    writer.append(source_writer::newline_tag{});

	  _write_define(writer, md.m.get(), pp_result, source_reader_cache,
			remarks);

	  // Always emit an empty line after the directive.
	  writer.append(source_writer::newline_tag{});

	  last_was_newline = true;
	  skip_following_insert_ws = true;
	};

      auto write_mu =
	[&](const _macro_undef_to_emit &mu) {
	  if (!last_was_newline)
	    writer.append(source_writer::newline_tag{});

	  _write_undef(writer, mu, pp_result, source_reader_cache);

	  last_was_newline = true;
	  skip_following_insert_ws = true;
	};

      // Write in directive order, except for undefs for which a
      // corresponding define is about to get emitted at cur_pos:
      // always put these immediately in front of the #define.
      const auto mus_begin = it_mu;
      const auto mus_end =
	std::find_if_not(mus_begin, _macro_undefs_to_emit.cend(),
			 [&](const _macro_undef_to_emit_in_chunk &mu) {
			   return mu.pos == cur_pos;
			 });
      const auto mds_begin = it_md;
      const auto mds_end =
	std::find_if_not(mds_begin, _macro_defines_to_emit.cend(),
			 [&](const _macro_define_to_emit_in_chunk &md) {
			   return md.pos == cur_pos;
			 });
      while (it_mu != mus_end || it_md != mds_end) {
	while (it_md != mds_end &&
	       (it_mu == mus_end || it_md->m < it_mu->mu)){
	  const auto it_matching_mu =
	    std::find_if(mus_begin, mus_end,
			 [&](const _macro_undef_to_emit_in_chunk &mu) {
			   return (it_md->m.m.get().get_name() ==
				   (mu.mu.original ?
				    mu.mu.original->get_name() :
				    mu.mu.name));
			 });
	  if (it_matching_mu != mus_end)
	    write_mu(it_matching_mu->mu);
	  write_md(it_md->m);
	  ++it_md;
	}

	while (it_mu != mus_end &&
	       (it_md == mds_end || it_mu->mu < it_md->m)) {
	  const std::string &name =  (it_mu->mu.original ?
				      it_mu->mu.original->get_name() :
				      it_mu->mu.name);
	  if (!std::any_of(mds_begin, mds_end,
			   [&](const _macro_define_to_emit_in_chunk &md) {
			     return md.m.m.get().get_name() == name;
			   })) {
	    write_mu(it_mu->mu);
	  }
	  ++it_mu;
	}
      }
    };

  // Avoid doing the source file lookup for every single copy op and
  // do it here once for the whole raw_pp_tokens_range spanned by this
  // chunk. Note that a given raw_pp_tokens_range can originate from
  // more than one source file, but
  // pp_result::const_intersecting_source_iterator will visit each
  // source file once for every maximal subregion in the queried range
  // originating from it.
  const raw_pp_tokens_range range_raw = _get_range_raw(pp_result);
  pp_result::const_intersecting_source_iterator it_source =
    pp_result.intersecting_sources_begin(range_raw);
  const pp_result::const_intersecting_source_iterator sources_end =
    pp_result.intersecting_sources_end(range_raw);
  pp_result::const_intersecting_source_iterator it_source_next = it_source;
  if (it_source_next != sources_end)
    ++it_source_next;

  auto get_cur_source_end =
    [&]() -> raw_pp_token_index {
      if (it_source_next == sources_end) {
	assert(it_source == sources_end ||
	       it_source->get_range().end >= range_raw.end);
	return range_raw.end;
      }

      const raw_pp_tokens_range &cur_range = it_source->get_range();
      const raw_pp_tokens_range &next_range = it_source_next->get_range();
      if (cur_range.end < next_range.end) {
	// *it_source is a descendant or (preceeding) sibling of it_source_next
	return std::min(cur_range.end, range_raw.end);
      } else {
	// *it_source_next is a descendant of *it_source.
	assert(next_range.begin < cur_range.end);
	return std::min(next_range.begin, range_raw.end);
      }
    };

  assert(it_source == sources_end ||
	 it_source->get_range().begin <= range_raw.begin);
  raw_pp_tokens_range cur_source_range{
	range_raw.begin,
	get_cur_source_end()
  };

  auto advance_source_to =
    [&](const pp_token_index raw_index) {
      assert(raw_index >= cur_source_range.begin);
      while (cur_source_range.end <= raw_index) {
	it_source = it_source_next;
	assert(it_source != sources_end);
	++it_source_next;
	cur_source_range.begin = cur_source_range.end;
	cur_source_range.end = get_cur_source_end();
      }
    };


  const _pos_in_chunk end_pos = _end_pos_in_chunk();
  auto it_range_to_emit_expanded = _ranges_to_emit_expanded.cbegin();
  const auto directives = pp_result.find_overlapping_directives(range_raw);
  auto it_directive = directives.first;
  while (cur_pos != end_pos) {
    const _op &op = _ops[cur_pos.op];
    switch (op.a) {
    case _op::action::copy:
      while(cur_pos.tok != op.r.end) {
	// Copy operations are special in that there might be some
	// #define/#undef directives to get emitted at an interior
	// point.  Moreover, they're partitioned into runs of tokens
	// to get emitted either raw, i.e. copied verbatim from the
	// source files, or expanded. find_batch_end() returns the
	// next stop where either #defines or #undefs are to be placed
	// or where there's a change from raw to expanded mode or vice
	// versa.
	auto find_next_stop =
	  [&]() -> pp_token_index {
	    pp_token_index next_stop = op.r.end;

	    if (it_mu != _macro_undefs_to_emit.cend() &&
		it_mu->pos.op == cur_pos.op) {
	      next_stop = std::min(it_mu->pos.tok, next_stop);
	    }
	    if (it_md != _macro_defines_to_emit.cend() &&
		it_md->pos.op == cur_pos.op) {
	      next_stop = std::min(it_md->pos.tok, next_stop);
	    }
	    if (it_range_to_emit_expanded != _ranges_to_emit_expanded.cend()) {
	      if (cur_pos.tok < it_range_to_emit_expanded->begin) {
		next_stop = std::min(it_range_to_emit_expanded->begin,
				     next_stop);
	      }
	    }
	    return next_stop;
	  };

	write_mus_and_mds_at_cur_pos();

	const pp_token_index next_stop = find_next_stop();
	if (it_range_to_emit_expanded == _ranges_to_emit_expanded.cend() ||
	    cur_pos.tok < it_range_to_emit_expanded->begin) {
	  // Copy the raw tokens, leave out any preprocessor
	  // directives in the source file. Note that the
	  // raw_pp_tokens_range might originate from more than one
	  // source file. So copy in batches, each spanning the
	  // region from the last one's end up to either the next
	  // directive or needed source file switch.
	  raw_pp_tokens_range batch_range_raw
	    = pp_result.pp_tokens_range_to_raw(pp_tokens_range{
							cur_pos.tok,
							next_stop
					       });

	  while (batch_range_raw.begin != batch_range_raw.end) {
	    while (it_directive != directives.second &&
		   it_directive->get_source_range() < batch_range_raw) {
	      ++it_directive;
	    }

	    raw_pp_token_index copy_end = batch_range_raw.end;
	    if (it_directive != directives.second) {
	      const raw_pp_tokens_range &directive_range
		= it_directive->get_source_range();
	      if (directive_range.begin == batch_range_raw.begin) {
		batch_range_raw.begin = directive_range.end;
		continue;
	      } else {
		copy_end = std::min(directive_range.begin, copy_end);
	      }
	    }

	    advance_source_to(batch_range_raw.begin);
	    copy_end = std::min(cur_source_range.end, copy_end);
	    assert(copy_end != batch_range_raw.begin);
	    const raw_pp_tokens &raw_toks = pp_result.get_raw_tokens();
	    writer.append
	      (source_reader_cache.get(*it_source),
	       range_in_file{
		      raw_toks[batch_range_raw.begin].get_range_in_file().begin,
		      raw_toks[copy_end - 1].get_range_in_file().end
	       });
	    batch_range_raw.begin = copy_end;

	    const raw_pp_token& last_tok = raw_toks[copy_end - 1];
	    last_was_newline = last_tok.is_newline();
	    skip_following_insert_ws = (last_tok.is_punctuator("(") ||
					last_tok.is_punctuator("[") ||
					last_tok.is_punctuator("{") ||
					last_tok.is_ws() ||
					last_was_newline);
	  }
	  cur_pos.tok = next_stop;

	} else {
	  // Copy preprocessed tokens.
	  const pp_tokens &toks = pp_result.get_pp_tokens();
	  while (cur_pos.tok != it_range_to_emit_expanded->end) {
	    const pp_token &tok = toks[cur_pos.tok];
	    writer.append(pp_token::stringify(tok.get_type(), tok.get_value()));
	    ++cur_pos.tok;

	    last_was_newline = tok.is_newline();
	    skip_following_insert_ws = (tok.is_punctuator("(") ||
					tok.is_punctuator("[") ||
					tok.is_punctuator("{") ||
					tok.is_ws() ||
					last_was_newline);
	  }
	  ++it_range_to_emit_expanded;
	}
      }
      break;

    case _op::action::replace:
      write_mus_and_mds_at_cur_pos();

      if (op.add_pointer_deref) {
	writer.append("(*" + op.new_tok.get_value() + ")");
	last_was_newline = false;
	skip_following_insert_ws = false;
      } else {
	writer.append(op.new_tok.get_value());
	last_was_newline = op.new_tok.is_newline();
	skip_following_insert_ws = (op.new_tok.is_punctuator("(") ||
				    op.new_tok.is_punctuator("[") ||
				    op.new_tok.is_punctuator("{") ||
				    op.new_tok.is_ws() ||
				    last_was_newline);
      }
      break;

    case _op::action::insert:
      write_mus_and_mds_at_cur_pos();
      writer.append(op.new_tok.get_value());
      last_was_newline = op.new_tok.is_newline();
      skip_following_insert_ws = (op.new_tok.is_punctuator("(") ||
				  op.new_tok.is_punctuator("[") ||
				  op.new_tok.is_punctuator("{") ||
				  op.new_tok.is_ws() ||
				  last_was_newline);
      break;

    case _op::action::insert_ws:
      {
	write_mus_and_mds_at_cur_pos();

	bool skip_insert_ws = skip_following_insert_ws;
	if (!skip_insert_ws) {
	  // Look ahaed to see if anything would render this extra
	  // whitespace redundant.
	  const _pos_in_chunk next_pos = _next_pos_in_chunk(cur_pos);
	  if (it_mu != _macro_undefs_to_emit.cend() && it_mu->pos == next_pos) {
	    skip_insert_ws = true;
	  } else if (it_md != _macro_defines_to_emit.cend() &&
		     it_md->pos == next_pos) {
	    skip_insert_ws = true;
	  } else if (next_pos.op == _ops.size()) {
	    // Can't happen, because leading and trailing insert_ws
	    // ops get trimmed, but whatever.
	    skip_insert_ws = true;
	  } else if (_ops[next_pos.op].a == _op::action::copy) {
	    const pp_token &next_tok = pp_result.get_pp_tokens()[next_pos.tok];
	    // See if the first token will get emitted expanded or
	    // not.
	    if (it_range_to_emit_expanded == _ranges_to_emit_expanded.cend() ||
		next_pos.tok <= it_range_to_emit_expanded->begin) {
	      const raw_pp_tokens_range tok_range_raw =
		next_tok.get_token_source();
	      const raw_pp_token &raw_next_tok =
		pp_result.get_raw_tokens()[tok_range_raw.begin];
	      skip_insert_ws = (raw_next_tok.is_punctuator(")") ||
				raw_next_tok.is_punctuator("]") ||
				raw_next_tok.is_punctuator("}") ||
				raw_next_tok.is_punctuator(";") ||
				raw_next_tok.is_ws() ||
				raw_next_tok.is_newline());
	    } else {
	      skip_insert_ws = (next_tok.is_punctuator(")") ||
				next_tok.is_punctuator("]") ||
				next_tok.is_punctuator("}") ||
				next_tok.is_punctuator(";") ||
				next_tok.is_ws() ||
				next_tok.is_newline());
	    }
	  }
	}

	if (!skip_insert_ws) {
	  assert(!last_was_newline);
	  writer.append(" ");
	}
      }
      break;
    }

    cur_pos = _next_pos_in_chunk(cur_pos);
  }

  // Finally, terminate the chunk by a newline, depending on
  // whether or not the last written charakter was a newline.
  if (!last_was_newline)
    writer.append(source_writer::newline_tag{});
}


depreprocessor::transformed_input_chunk::_op::
_op(const pp_tokens_range &copied_range)
  : a(action::copy), r(copied_range), stickiness(sticky_side::none),
    new_tok{pp_token::type::eof, std::string{}, raw_pp_tokens_range{}},
    add_pointer_deref(false)
{
  assert(copied_range.begin != copied_range.end);
}

depreprocessor::transformed_input_chunk::_op::
_op(const pp_tokens_range &replaced_range, pp_token &&repl_tok,
    const bool _add_pointer_deref)
  : a(action::replace), r(replaced_range), stickiness(sticky_side::none),
    new_tok(std::move(repl_tok)),add_pointer_deref(_add_pointer_deref)
{
  assert(replaced_range.begin + 1 == replaced_range.end);
}

depreprocessor::transformed_input_chunk::_op::_op(const pp_token_index pos,
						  pp_token &&_new_tok,
						  const sticky_side _stickiness)
  : a(action::insert), r(pp_tokens_range{pos, pos}), stickiness(_stickiness),
    new_tok(std::move(_new_tok)), add_pointer_deref(false)
{}

depreprocessor::transformed_input_chunk::_op::_op(const pp_token_index pos,
						  const sticky_side _stickiness)
  : a(action::insert_ws), r(pp_tokens_range{pos, pos}), stickiness(_stickiness),
    new_tok{pp_token::type::eof, std::string{}, raw_pp_tokens_range{}},
    add_pointer_deref(false)
{}

bool depreprocessor::transformed_input_chunk::_op::
operator<(const pp_tokens_range &rhs) const noexcept
{
  return (this->r < rhs);
}

bool depreprocessor::transformed_input_chunk::_op::
operator>(const pp_tokens_range &rhs) const noexcept
{
  return (rhs < this->r);
}

raw_pp_tokens_range depreprocessor::transformed_input_chunk::_op::
get_range_raw(const pp_result &pp_result)
  const noexcept
{
  switch (a) {
  case action::copy:
    /* fall through */
  case action::replace:
    /* fall through */
    return pp_result.pp_tokens_range_to_raw(r);

  case action::insert:
    /* fall through */
  case action::insert_ws:
    // Point pp_token_ranges get mapped mapped to raw_pp_tokens_ranges
    // spanning everything inbetween the preceeding and the subsequent
    // pp_token. Use the stickiness flag to determine which end of
    // that raw range to pick.
    {
      const pp_tokens &toks = pp_result.get_pp_tokens();
      assert(r.begin == r.end);
      if (r.begin && r.begin < toks.size() &&
	  toks[r.begin - 1].get_token_source().begin ==
	  toks[r.begin].get_token_source().begin) {
	// The preceeding and subsequent pp_tokens from the input
	// originate from the same macro invocation. Return its
	// associated raw_pp_tokens_range.
	return pp_result.pp_tokens_range_to_raw(r);
      }
      raw_pp_tokens_range raw_range = pp_result.pp_tokens_range_to_raw(r);
      switch (stickiness) {
      case sticky_side::left:
	return raw_pp_tokens_range{raw_range.begin, raw_range.begin};

      case sticky_side::right:
	return raw_pp_tokens_range{raw_range.end, raw_range.end};

      case sticky_side::none:
	assert(0);
	__builtin_unreachable();
      }
    }
  }
}


depreprocessor::transformed_input_chunk::_pos_in_chunk::
_pos_in_chunk(const _ops_type::size_type _op, const pp_token_index _tok)
  noexcept
  : op(_op), tok(_tok)
{}

bool depreprocessor::transformed_input_chunk::_pos_in_chunk::
operator==(const _pos_in_chunk &rhs) const noexcept
{
  return this->op == rhs.op && this->tok == rhs.tok;
}

bool depreprocessor::transformed_input_chunk::_pos_in_chunk::
operator<(const _pos_in_chunk &rhs) const noexcept
{
  return (this->op != rhs.op ? this->op < rhs.op : this->tok < rhs.tok);
}


depreprocessor::transformed_input_chunk::_used_macro_in_chunk::
_used_macro_in_chunk(const _pos_in_chunk &_pos, const pp_result::macro &_m)
  noexcept
  : pos(_pos), m(_m)
{}


depreprocessor::transformed_input_chunk::_macro_nondef_constraint_in_chunk::
_macro_nondef_constraint_in_chunk(const _pos_in_chunk &_pos,
				  pp_result::macro_nondef_constraint &&_mnc)
  : pos(_pos), mnc(std::move(_mnc))
{}

depreprocessor::transformed_input_chunk::_macro_undef_to_emit_in_chunk::
_macro_undef_to_emit_in_chunk(const _pos_in_chunk &_pos,
			      _macro_undef_to_emit &&_mu)
  : pos(_pos), mu(std::move(_mu))
{}

bool depreprocessor::transformed_input_chunk::_macro_undef_to_emit_in_chunk::
operator<(const _macro_undef_to_emit_in_chunk &rhs)
  const noexcept
{
  if (this->pos == rhs.pos)
    return this->mu < rhs.mu;

  return this->pos < rhs.pos;
}

depreprocessor::transformed_input_chunk::_macro_define_to_emit_in_chunk::
_macro_define_to_emit_in_chunk(const _pos_in_chunk &_pos,
			       _macro_define_to_emit &&_m)
  : pos(_pos), m(std::move(_m))
{}

bool depreprocessor::transformed_input_chunk::_macro_define_to_emit_in_chunk::
operator<(const _macro_define_to_emit_in_chunk &rhs) const noexcept
{
  if (this->pos == rhs.pos)
    return this->m < rhs.m;

  return this->pos < rhs.pos;
}


depreprocessor::_header_inclusion_chunk::
_header_inclusion_chunk(const pp_result::header_inclusion_child &child_node)
  : _k(_kind::k_child), _child_node(&child_node)
{}

depreprocessor::_header_inclusion_chunk::
_header_inclusion_chunk(const pp_result::header_inclusion_root &root_node)
  : _k(_kind::k_root), _root_node(&root_node)
{}

const pp_result::header_inclusion_node&
depreprocessor::_header_inclusion_chunk::get_inclusion_node()const noexcept
{
  switch (_k) {
  case _kind::k_child:
    return *_child_node;

  case _kind::k_root:
    return *_root_node;
  }
}

void depreprocessor::_header_inclusion_chunk::
find_macro_constraints(const pp_result &pp_result)
{
  // Process the #defines and #undefs within this header in file
  // order. Maintain a list of currently active macro definitions as
  // well as a list of all macro definitions ever seen. The latter one
  // will be used to resolve internal references to macros from within
  // this header itself. Similarly for all #undefs seen.
  const raw_pp_tokens_range header_range_raw =
    this->get_inclusion_node().get_range();
  const auto mds = pp_result.find_overlapping_macros(header_range_raw);
  const auto mus = pp_result.find_overlapping_macro_undefs(header_range_raw);
  std::multimap<std::string, std::reference_wrapper<const pp_result::macro> >
    all_defines;
  std::map<std::string, std::reference_wrapper<const pp_result::macro> >
    active_defines;
  // We only care about the first #undef in file order for a given
  // name each.
  std::map<std::string, std::reference_wrapper<const pp_result::macro_undef> >
    all_undefs;
  auto it_md = mds.first;
  auto it_mu = mus.first;
  while (it_md != mds.second || it_mu != mus.second) {
    while (it_mu != mus.second &&
	   (it_md == mds.second ||
	    it_mu->get_directive_range() < it_md->get_directive_range())) {
      active_defines.erase(it_mu->get_name());
      if (_macro_defines_wo_prior_undef.find(it_mu->get_name()) ==
	  _macro_defines_wo_prior_undef.end()) {
	_macro_undefs.insert(it_mu->get_name());
      }
      all_undefs.insert(std::make_pair(it_mu->get_name(), std::cref(*it_mu)));
      ++it_mu;
    }

    while (it_md != mds.second &&
	   (it_mu == mus.second ||
	    it_md->get_directive_range() < it_mu->get_directive_range())) {
      const auto it_all_defines = all_defines.find(it_md->get_name());
      if (it_all_defines == all_defines.end()) {
	if (_macro_undefs.find(it_md->get_name()) == _macro_undefs.end()) {
	  // First define for that name and no prior undef.
	  _macro_defines_wo_prior_undef.insert
	    (std::make_pair(it_md->get_name(), std::cref(*it_md)));
	}

	all_defines.insert(std::make_pair(it_md->get_name(),
					  std::cref(*it_md)));
      } else {
	// Make use of the hint.
	all_defines.insert(it_all_defines,
			   std::make_pair(it_md->get_name(),
					  std::cref(*it_md)));
      }

      auto it_active_define = active_defines.find(it_md->get_name());
      if (it_active_define == active_defines.end()) {
	active_defines.insert(std::make_pair(it_md->get_name(),
					     std::cref(*it_md)));
      } else {
	it_active_define = active_defines.erase(it_active_define);
	// Make use of the hint.
	active_defines.insert(it_active_define,
			      std::make_pair(it_md->get_name(),
					     std::cref(*it_md)));
      }
      ++it_md;
    }
  }

  _new_macro_defines.reserve(active_defines.size());
  for (const auto &ad : active_defines) {
    _new_macro_defines.push_back(ad.second);
  }
  active_defines.clear();

  // Now find the header's dependencies on externally defined macros
  // as well as constraints wrt. macro non-definedness.
  auto add_um =
    [&](const pp_result::macro &m) {
      // Avoid adding dependencies to #defines from within this header.
      const auto defs = all_defines.equal_range(m.get_name());
      for (auto it_md = defs.first; it_md != defs.second; ++it_md) {
	if (&it_md->second.get() == &m)
	  return;
      }
      _used_macros += m;
    };
  auto add_ums =
    [&](const pp_result::used_macros &ums) {
      for (const auto &m : ums)
	add_um(m);
    };
  auto add_mnc =
    [&](const pp_result::macro_nondef_constraint &mnc,
	const raw_pp_token_index pos) {
      // Avoid adding nondef constraints for macros #undefined in this
      // header prior to pos.
      const auto undefs = all_undefs.equal_range(mnc.get_name());
      auto it_mu = all_undefs.find(mnc.get_name());
      if (it_mu != all_undefs.end() &&
	  (it_mu->second.get().get_directive_range()
	   < raw_pp_tokens_range{pos, pos})) {
	return;
      }
      _macro_nondef_constraints += mnc;
    };
  auto add_mncs =
    [&](const pp_result::macro_nondef_constraints &mncs,
	const raw_pp_token_index pos) {
      for (const auto &mnc : mncs)
	add_mnc(mnc, pos);
    };

  // First handle the requirements of contained preprocessor directives.
  if (_k == _kind::k_child) {
    add_ums(_child_node->get_used_macros());
    add_mncs(_child_node->get_macro_nondef_constraints(),
	     _child_node->get_directive_range().begin);
  }

  this->get_inclusion_node().for_each_descendant_inclusion_node
    (wrap_callables<no_default_action>
     ([&](const pp_result::header_inclusion_child &h) {
	add_ums(h.get_used_macros());
	add_mncs(h.get_macro_nondef_constraints(),
		 h.get_directive_range().begin);
      },
      [&](const pp_result::conditional_inclusion_node &c) {
	add_ums(c.get_used_macros());
	add_mncs(c.get_macro_nondef_constraints(), c.get_directive_begin());
      }));

  // Now consider the individual pp_token instances emitted from this
  // header.
  const auto mis =
    pp_result.find_overlapping_macro_invocations(header_range_raw);
  auto it_mi = mis.first;
  const pp_tokens_range header_range =
    pp_result.raw_pp_tokens_range_to_nonraw(header_range_raw);
  const auto tok_begin = pp_result.get_pp_tokens().begin() + header_range.begin;
  const auto tok_end = pp_result.get_pp_tokens().begin() + header_range.end;
  auto it_tok = tok_begin;
  while (it_tok != tok_end) {
    if (it_mi != mis.second &&
	!(it_tok->get_token_source() < it_mi->get_source_range())) {
      // The current token to process does not come before the next
      // macro invocation. However, the macro invocation might still
      // be unrelated to any pp_token, namely if it's part of some
      // preprocessor directive.
      if (!(it_mi->get_source_range() < it_tok->get_token_source())) {
	add_ums(it_mi->get_used_macros());
	add_mncs(it_mi->get_macro_nondef_constraints(),
		 it_mi->get_source_range().begin);
	// Advance the current token iterator past the tokens from the
	// macro_invocation just processed.
	while (it_tok != tok_end &&
	       !(it_mi->get_source_range() < it_tok->get_token_source())) {
	  ++it_tok;
	}
      }
      ++it_mi;

    } else if (it_tok->is_id()) {
      // Look ahead to check whether the identifier token is not
      // followed by an opening parenthesis and thus, a function style
      // definition, even if present, would not alter the result. If
      // the next token is a parenthesis, but it itself or any
      // whitespace before it is the result of some macro invocation,
      // then it can be safely ignored.
      auto it_next_tok = it_tok + 1;
      while (it_next_tok != tok_end &&
	     it_next_tok->is_type_any_of<pp_token::type::ws,
					 pp_token::type::empty,
					 pp_token::type::newline>()) {
	++it_next_tok;
      }

      bool func_like_allowed = false;
      if (it_next_tok == tok_end ||
	  !it_next_tok->is_punctuator("(") ||
	  (it_mi != mis.second &&
	   !(it_next_tok->get_token_source() < it_mi->get_source_range()))) {
	func_like_allowed = true;
      }

      add_mnc(pp_result::macro_nondef_constraint{it_tok->get_value(),
						 func_like_allowed},
	      it_tok->get_token_source().begin);
      ++it_tok;

    } else {
      ++it_tok;

    }
  }
}

bool depreprocessor::_header_inclusion_chunk::
needs_undef_before_include(const pp_result::macro &m) const noexcept
{
  const auto it_mnc = _macro_nondef_constraints.find(m.get_name());
  if (it_mnc != _macro_nondef_constraints.end()) {
    if (!m.is_func_like() || !it_mnc->is_func_like_allowed())
      return true;
  }

  const auto it_define_wo_undef =
    _macro_defines_wo_prior_undef.find(m.get_name());
  if (it_define_wo_undef != _macro_defines_wo_prior_undef.end() &&
      it_define_wo_undef->second.get() != m) {
    // This header defines a macro w/o any prior undef in an
    // incompatible way.
    return true;
  }

  return false;
}

bool depreprocessor::_header_inclusion_chunk::
is_unmodified_by_include(const pp_result::macro &m) const noexcept
{
  if (_macro_undefs.find(m.get_name()) != _macro_undefs.end())
    return false;

  // This macro could still have been redefined in a compatible way
  // and this counts as a modification here.
  if (_macro_defines_wo_prior_undef.find(m.get_name()) !=
      _macro_defines_wo_prior_undef.end()) {
    return false;
  }

  return true;
}

void depreprocessor::_header_inclusion_chunk::
add_macro_undef_to_emit(_macro_undef_to_emit &&mu)
{
  const auto it = std::upper_bound(_macro_undefs_to_emit.begin(),
				   _macro_undefs_to_emit.end(),
				   mu);
  _macro_undefs_to_emit.insert(it, std::move(mu));
}

void depreprocessor::_header_inclusion_chunk::
add_macro_define_to_emit(_macro_define_to_emit &&m)
{
  const auto it = std::upper_bound(_macro_defines_to_emit.begin(),
				   _macro_defines_to_emit.end(),
				   m);
  _macro_defines_to_emit.insert(it, std::move(m));
}

bool depreprocessor::_header_inclusion_chunk::
has_macro_undefs_or_defines_to_emit() const noexcept
{
  return !_macro_undefs_to_emit.empty() || !_macro_defines_to_emit.empty();
}

void depreprocessor::_header_inclusion_chunk::
write(source_writer &writer, const pp_result &pp_result,
      _source_reader_cache &source_reader_cache,
      output_remarks &remarks) const
{
  // First, write the needed #defines + #undefs.
  depreprocessor::_write_defines_and_undefs(writer, _macro_defines_to_emit,
					    _macro_undefs_to_emit, pp_result,
					    source_reader_cache, remarks);

  // And write the #include directive.
  switch (_k) {
  case _kind::k_child:
    _write_directive(writer, this->_child_node->get_directive_range(),
		     pp_result, source_reader_cache);
    break;

  case _kind::k_root:
    writer.append("#include \"" + _root_node->get_filename() + "\"");
    writer.append(source_writer::newline_tag{});
    break;
  }
}


depreprocessor::_chunk::_chunk(transformed_input_chunk &&_tic,
			       const pp_result &pp_result)
  : k(kind::transformed_input), tic(std::move(_tic)),
    range_raw(tic._get_range_raw(pp_result))
{}

depreprocessor::_chunk::_chunk(_header_inclusion_chunk &&_hic)
  : k(kind::header_inclusion), hic(std::move(_hic)),
    range_raw(hic.get_inclusion_node().get_range())
{}
{}

depreprocessor::_chunk::_chunk(_chunk &&c)
  : k(c.k), range_raw(c.range_raw)
{
  switch (k) {
  case kind::transformed_input:
    new (&tic) transformed_input_chunk{std::move(c.tic)};
    break;

  case kind::header_inclusion:
    new (&hic) _header_inclusion_chunk{std::move(c.hic)};
    break;

  case kind::_dead:
    __builtin_unreachable();
    assert(0);
  }
}

depreprocessor::_chunk::~_chunk() noexcept
{
  switch(k) {
  case kind::transformed_input:
    tic.~transformed_input_chunk();
    break;

  case kind::header_inclusion:
    hic.~_header_inclusion_chunk();
    break;

  case kind::_dead:
    break;
  };
}

depreprocessor::_chunk& depreprocessor::_chunk::operator=(_chunk &&rhs)
{
  if (this == &rhs)
    return *this;

  switch (this->k) {
  case kind::transformed_input:
    tic.~transformed_input_chunk();
    break;

  case kind::header_inclusion:
    hic.~_header_inclusion_chunk();
    break;

  case kind::_dead:
    __builtin_unreachable();
    assert(0);
  };

  // The below can throw, avoid double free.
  this->k = kind::_dead;
  switch (rhs.k) {
  case kind::transformed_input:
    new (&this->tic) transformed_input_chunk{std::move(rhs.tic)};
    break;

  case kind::header_inclusion:
    new (&this->hic) _header_inclusion_chunk{std::move(rhs.hic)};
    break;

  case kind::_dead:
    __builtin_unreachable();
    assert(0);
  };

  this->k = rhs.k;
  this->range_raw = rhs.range_raw;

  return *this;
}


depreprocessor::_in_order_chunk::
_in_order_chunk(const _chunks_type::iterator _it_chunk) noexcept
  : it_chunk(_it_chunk), attributed_range_raw(0, it_chunk->range_raw.end)
{}


depreprocessor::_pos_in_output::
_pos_in_output(const _chunks_type::iterator &_chunk,
	       const transformed_input_chunk::_pos_in_chunk &_pos_in_chunk)
  noexcept
  : chunk(_chunk), pos_in_chunk(_pos_in_chunk)
{}

depreprocessor::_pos_in_output::
_pos_in_output(const _chunks_type::iterator &_chunk) noexcept
  : chunk(_chunk), pos_in_chunk(0, 0)
{
  // Used for header chunks and the end iterator.
}

bool depreprocessor::_pos_in_output::operator==(const _pos_in_output &rhs)
  const noexcept
{
  return (this->chunk == rhs.chunk &&
	  this->pos_in_chunk == rhs.pos_in_chunk);
}

bool depreprocessor::_pos_in_output::operator<(const _pos_in_output &rhs)
  const noexcept
{
  if (this->chunk == rhs.chunk)
    return this->pos_in_chunk < rhs.pos_in_chunk;

  return this->chunk < rhs.chunk;
}


depreprocessor::_macro_state::_macro_state(const pp_result::macro &_m,
					   const _pos_in_output &_last_usage)
  noexcept
  : m(_m), last_usage(_last_usage)
{}


depreprocessor::depreprocessor(const pp_result &pp_result,
			       output_remarks &remarks)
  : _pp_result(pp_result), _remarks(remarks)
{}

void depreprocessor::append(transformed_input_chunk &&tic)
{
  tic._trim();
  if (tic._ops.empty())
    return;
  _chunks.emplace_back(std::move(tic), _pp_result);
}

void depreprocessor::append(const pp_result::header_inclusion_child &include)
{
  _chunks.emplace_back(_header_inclusion_chunk{include});
}

void depreprocessor::append(const pp_result::header_inclusion_root &hir)
{
  _chunks.emplace_back(_header_inclusion_chunk{hir});
}

void depreprocessor::operator()(const std::string &outfile)
{
  _try_merge_chunks_pre();
  _compute_needed_macro_defs_and_undefs();

  source_writer writer{outfile};
  _source_reader_cache source_reader_cache;
  bool is_first_chunk = true;
  bool prev_was_header_inclusion = false;
  for (const auto &c : _chunks) {
    switch (c.k) {
    case _chunk::kind::transformed_input:
      // Separate transformed_input_chunks by an empty line from
      // whatever came before.
      if (!is_first_chunk)
	writer.append(source_writer::newline_tag{});

      c.tic._write(writer, _pp_result, source_reader_cache, _remarks);

      prev_was_header_inclusion = false;
      break;

    case _chunk::kind::header_inclusion:
      // Add an empty line if there will be some #defines or #undefs
      // before this header or if the previous chunk was not a
      // _header_inclusion_chunk.
      if (!is_first_chunk &&
	  (!prev_was_header_inclusion ||
	   c.hic.has_macro_undefs_or_defines_to_emit())) {
	writer.append(source_writer::newline_tag{});
      }
      c.hic.write(writer, _pp_result, source_reader_cache, _remarks);

      prev_was_header_inclusion = true;
      break;

    case _chunk::kind::_dead:
      assert(0);
      __builtin_unreachable();
    }

    is_first_chunk = false;
  }

  writer.flush();
}

void depreprocessor::_try_merge_chunks_pre()
{
  // In a first step, prune transformed_input_chunks with empty ops.
  for (auto it = _chunks.begin(); it != _chunks.end();) {
    if (it->k != _chunk::kind::transformed_input ||
	!it->tic._ops.empty()) {
      ++it;
    } else {
      it = _chunks.erase(it);
    }
  }

  // If two consecutive chunks' first resp. last operations are copy
  // ops and they're separated in the input by whitespace only, then
  // these can joined. However, separate chunks are usually associated
  // with different top-level declarations and thus, the boundaries
  // inbetween are preferred points for emitting macro #defines and
  // #undefs as needed -> in general, these chunks should be kept
  // separate. OTOH, joining consecutive chunks might make retaining
  // macro invocations crossing the former boundary possible. Join
  // chunks if this is indeed the case.
  auto next_seq_begin =
    [&](const _chunks_type::iterator &_it) {
      _chunks_type::iterator it;
      for (it = _it; it != _chunks.end(); ++it) {
	if (it->k == _chunk::kind::transformed_input) {
	  const auto &ops = it->tic._ops;
	  if (ops.back().a == transformed_input_chunk::_op::action::copy)
	    break;
	}
      }

      return it;
    };

  auto seq_begin = next_seq_begin(_chunks.begin());
  while (seq_begin != _chunks.end()) {
    auto seq_end = seq_begin + 1;
    while (seq_end != _chunks.end()) {
      if (seq_end->k != _chunk::kind::transformed_input)
	break;

      const auto &next_ops = seq_end->tic._ops;
      if (next_ops.front().a != transformed_input_chunk::_op::action::copy)
	break;

      const auto &last_ops = (seq_end - 1)->tic._ops;
      if (next_ops.front().r.begin < last_ops.back().r.end)
	break;

      // In order for the two chunks to be joinable, the last copy op
      // from the first and the first from from the subsequent one
      // must be separated in the input by whitespace only.
      const pp_tokens &toks = _pp_result.get_pp_tokens();
      if (!std::all_of(toks.begin() + last_ops.back().r.end,
		       toks.begin() + next_ops.front().r.begin,
		       [](const pp_token &tok) {
			 return tok.is_type_any_of<pp_token::type::ws,
						   pp_token::type::newline>();
		       })) {
	break;
      }

      ++seq_end;
      if (next_ops.size() > 1) {
	// The next_ops sequence does't consist of a single copy op
	// and the current run can't be extended beyond it.
	break;
      }
    }

    if (seq_end - seq_begin < 2) {
      seq_begin = next_seq_begin(seq_end);
      continue;
    }

    const pp_tokens_range seq_range = {
	seq_begin->tic._ops.back().r.begin,
	(seq_end - 1)->tic._ops.front().r.end
      };
    const raw_pp_tokens_range seq_range_raw
      = _pp_result.pp_tokens_range_to_raw(seq_range);

    const auto mis =
      _pp_result.find_overlapping_macro_invocations(seq_range_raw);
    for (auto it_mi = mis.first; it_mi != mis.second; ++it_mi) {
      const raw_pp_tokens_range mi_range_raw = it_mi->get_source_range();
      const pp_tokens_range mi_range =
	_pp_result.raw_pp_tokens_range_to_nonraw(mi_range_raw);
      if (!mi_range.is_contained_in(seq_range))
	continue;

      for (auto it = seq_begin + 1; it != seq_end; ++it) {
	const auto it_prev = it - 1;
	transformed_input_chunk &tic = it->tic;
	transformed_input_chunk &tic_prev = it_prev->tic;
	const pp_tokens_range gap_range = {
		tic_prev._ops.back().r.end,
		tic._ops.front().r.begin,
	};

	if (gap_range.is_contained_in(mi_range)) {
	  // Merge the chunks.
	  tic_prev._ops.back().r.end = tic._ops.front().r.end;
	  tic_prev._ops.insert(tic_prev._ops.end(),
			       tic._ops.begin() + 1, tic._ops.end());
	  const auto saved_seq_begin = seq_begin - _chunks.begin();
	  const auto saved_seq_end = seq_end - _chunks.begin();
	  it = _chunks.erase(it);
	  seq_begin = _chunks.begin() + saved_seq_begin;
	  seq_end = _chunks.begin() + saved_seq_end - 1; // Account for removal.
	  --it;
	}
      }
    }

    seq_begin = next_seq_begin(seq_end - 1);
  }

  _chunks.shrink_to_fit();
}

void depreprocessor::_compute_needed_macro_defs_and_undefs()
{
  // Iterate through the chunks in reverse order, determine their "in
  // order" property and find their macro constraints.
  bool next_raw_tok_is_opening_parenthesis = false;
  _in_order_chunks_type in_order_chunks;
  for (auto rit_chunk = _chunks.rbegin(); rit_chunk != _chunks.rend();
       ++rit_chunk) {
    auto check_chunk_in_order =
      [&](const _chunks_type::iterator &it) {
	// A chunk is defined to be "in order" iff all subsequent (in
	// output order) chunks cover input ranges strictly following
	// its own one.
	//
	// Each "in order" chunk is attributed a raw_pp_tokens_range:
	// it spans from the previous (in file order) "in order"
	// chunk's ->range_raw.end to its own ->range_raw.end.
	// Preprocessing directives like #defines and #undefs from
	// within this attributed range will preferrably get emitted
	// as part of this chunk in order to retain input order as
	// much as possible.
	//
	// Maintain in_order_chunks in reversed output order for the
	// moment, it will get reversed below.
	if (!in_order_chunks.empty()) {
	  const raw_pp_tokens_range &min_range_seen
	    = in_order_chunks.back().it_chunk->range_raw;
	  if (it->range_raw < min_range_seen) {
	    in_order_chunks.back().attributed_range_raw.begin
	      = it->range_raw.end;
	    in_order_chunks.emplace_back(it);
	  } else if (it->range_raw == min_range_seen) {
	    // Keep only the first (in output order) of consecutive
	    // chunks with equal ranges.
	    in_order_chunks.pop_back();
	    if (!in_order_chunks.empty()) {
	      in_order_chunks.back().attributed_range_raw.begin
		= it->range_raw.end;
	    }
	    in_order_chunks.emplace_back(it);
	  }
	} else {
	  in_order_chunks.emplace_back(it);
	}
      };

    check_chunk_in_order(rit_chunk.base() - 1);
    switch (rit_chunk->k) {
    case _chunk::kind::transformed_input:
      next_raw_tok_is_opening_parenthesis =
	(rit_chunk->tic._find_macro_constraints
	 (_pp_result, next_raw_tok_is_opening_parenthesis));
      break;

    case _chunk::kind::header_inclusion:
      rit_chunk->hic.find_macro_constraints(_pp_result);
      next_raw_tok_is_opening_parenthesis = false;
      break;

    case _chunk::kind::_dead:
      assert(0);
      __builtin_unreachable();
    }
  }

  std::reverse(in_order_chunks.begin(), in_order_chunks.end());
  in_order_chunks.shrink_to_fit();

  _macro_states_type macro_states;
  // Populate macro_states with the builtin macros so that no
  // dependencies will be added to those (if not #undefined
  // meanwhile).
  for (auto it_m = _pp_result.macros_begin();
       it_m != _pp_result.macros_end() && it_m->is_builtin(); ++it_m) {
    // Record a special value of _end_pos_in_output() for the builtin
    // macro's initial ->last_usage.  It will be updated upon actual
    // usage.
    macro_states.emplace(it_m->get_name(),
			 _macro_state {
			   *it_m,
			   _end_pos_in_output()
			 });
  }

  // Traverse all chunks in forward order now and form longest runs of
  // chunks with strictly increasing (or equal) ranges.
  auto cur_run_begin = _chunks.begin();
  raw_pp_tokens_range last_range_in_run = raw_pp_tokens_range{0, 0};
  for (auto it_chunk = _chunks.begin(); it_chunk != _chunks.end(); ++it_chunk) {
    if (!(last_range_in_run < it_chunk->range_raw ||
	  last_range_in_run == it_chunk->range_raw)) {
      cur_run_begin = it_chunk;
    }
    last_range_in_run = it_chunk->range_raw;

    switch (it_chunk->k) {
    case _chunk::kind::transformed_input:
      _compute_needed_macro_defs_and_undefs_tic(it_chunk,
						in_order_chunks,
						cur_run_begin,
						macro_states);
      break;

    case _chunk::kind::header_inclusion:
      _compute_needed_macro_defs_and_undefs_hic(it_chunk,
						in_order_chunks,
						cur_run_begin,
						macro_states);
      break;

    case _chunk::kind::_dead:
      assert(0);
      __builtin_unreachable();
    }
  }
}

void depreprocessor::_compute_needed_macro_defs_and_undefs_tic
			(const _chunks_type::iterator &it_chunk,
			 const _in_order_chunks_type &in_order_chunks,
			 const _chunks_type::iterator &cur_run_begin,
			 _macro_states_type &macro_states)
{
  transformed_input_chunk &tic = it_chunk->tic;
  auto it_um = tic._used_macros_in_chunk.cbegin();
  auto it_mnc = tic._macro_nondef_constraints_in_chunk.cbegin();
  const auto ums_end = tic._used_macros_in_chunk.cend();
  const auto mncs_end = tic._macro_nondef_constraints_in_chunk.cend();
  while (it_um != ums_end || it_mnc != mncs_end) {
    while (it_mnc != mncs_end &&
	   (it_um == ums_end || it_mnc->pos <= it_um->pos)) {
      const auto it_ms = macro_states.find(it_mnc->mnc.get_name());
      if (it_ms != macro_states.end() &&
	  !(it_ms->second.m.get().is_func_like() &&
	    it_mnc->mnc.is_func_like_allowed())) {
	// #undef after last usage is needed.
	_emit_undef(it_ms->second, _pos_in_output{it_chunk, it_mnc->pos},
		    in_order_chunks, cur_run_begin);
	macro_states.erase(it_ms);
      }
      ++it_mnc;
    }

    while (it_um != ums_end &&
	   (it_mnc == mncs_end || it_um->pos < it_mnc->pos)) {
      const auto it_ms = macro_states.find(it_um->m.get().get_name());
      if (it_ms != macro_states.end() &&
	  (&it_ms->second.m.get() == &it_um->m.get() ||
	   it_ms->second.m.get() == it_um->m.get())) {
	// Macro active, update its last usage.
	it_ms->second.last_usage = _pos_in_output{it_chunk, it_um->pos};

      } else if (it_ms == macro_states.end()) {
	// Macro not active at all.
	_emit_define(it_um->m.get(),
		     _pos_in_output{it_chunk, it_um->pos},
		     in_order_chunks, cur_run_begin,
		     nullptr);
	macro_states.emplace(it_um->m.get().get_name(),
			     _macro_state {
			       it_um->m.get(),
			       _pos_in_output{it_chunk, it_um->pos}
			     });

      } else {
	// Incompatible macro with same name is active.
	_emit_define(it_um->m.get(),
		     _pos_in_output{it_chunk, it_um->pos},
		     in_order_chunks, cur_run_begin, &it_ms->second);
	it_ms->second = _macro_state {
			  it_um->m.get(),
			  _pos_in_output{it_chunk, it_um->pos}
			};
      }

      ++it_um;
    }
  }
}

void depreprocessor::_compute_needed_macro_defs_and_undefs_hic
		(const _chunks_type::iterator &it_chunk,
		 const _in_order_chunks_type &in_order_chunks,
		 const _chunks_type::iterator &cur_run_begin,
		 _macro_states_type &macro_states)
{
  _header_inclusion_chunk &hic = it_chunk->hic;
  for (auto it_ms = macro_states.begin(); it_ms != macro_states.end();) {
    const pp_result::macro &m = it_ms->second.m.get();
    if (hic.needs_undef_before_include(m)) {
      _emit_undef(it_ms->second, _pos_in_output{it_chunk},
		  in_order_chunks, cur_run_begin);
      it_ms = macro_states.erase(it_ms);
    }  else {
      ++it_ms;
    }
  }

  for (const auto &um : hic.get_used_macros()) {
    const auto it_ms = macro_states.find(um.get_name());
    if (it_ms != macro_states.end() &&
	(&it_ms->second.m.get() == &um ||
	 it_ms->second.m.get() == um)) {
      // Macro active, update its last usage.
      it_ms->second.last_usage = _pos_in_output{it_chunk};

    } else if (it_ms == macro_states.end()) {
      // Macro not active at all.
      _emit_define(um,
		   _pos_in_output{it_chunk},
		   in_order_chunks, cur_run_begin,
		   nullptr);
      macro_states.emplace(um.get_name(),
			   _macro_state {
			     um,
			     _pos_in_output{it_chunk}
			   });

    } else {
      // Incompatible macro with same name is active.
      _emit_define(um,
		   _pos_in_output{it_chunk},
		   in_order_chunks, cur_run_begin, &it_ms->second);
      it_ms->second = _macro_state {
			  um,
			  _pos_in_output{it_chunk}
		      };
    }
  }

  // Finally consider the effect of the included header on
  // macro_states.
  for (auto it_ms = macro_states.begin(); it_ms != macro_states.end();) {
    if (hic.is_unmodified_by_include(it_ms->second.m.get())) {
      ++it_ms;
    } else {
      it_ms = macro_states.erase(it_ms);
    }
  }
  for (auto it_md = hic.new_macro_defines_begin();
       it_md != hic.new_macro_defines_end(); ++it_md) {
    macro_states.emplace(it_md->get_name(),
			 _macro_state {
			   *it_md,
			   _pos_in_output{it_chunk}
			 });
  }
}

bool depreprocessor::
_can_place_define_at(const pp_result::macro &m,
		     const _pos_in_output &placement_pos,
		     const _pos_in_output &um_pos,
		     const _macro_state * const conflicting_macro_state)
  noexcept
{
  if (!(placement_pos <= um_pos))
    return false;

  if (conflicting_macro_state &&
      conflicting_macro_state->last_usage != _end_pos_in_output() &&
      !(conflicting_macro_state->last_usage < placement_pos)) {
    return false;
  }

  // Check if there are any macro_nondef_constraints inbetween
  // placement_pos and um_pos
  _pos_in_output cur_pos = placement_pos;
  while (cur_pos != um_pos) {
    auto advance_to_next_chunk =
      [&]() {
	if (cur_pos.chunk == um_pos.chunk) {
	  assert(cur_pos.chunk->k == _chunk::kind::transformed_input);
	  cur_pos = um_pos;
	  return;
	}

	assert((cur_pos.chunk + 1) != _chunks.end());
	cur_pos = _chunk_pos_in_output(cur_pos.chunk + 1);
      };

    switch (cur_pos.chunk->k) {
    case _chunk::kind::transformed_input:
      {
	const transformed_input_chunk &tic = cur_pos.chunk->tic;
	const transformed_input_chunk::_pos_in_chunk end_pos_in_chunk
	  = (cur_pos.chunk != um_pos.chunk ?
	     tic._end_pos_in_chunk() : um_pos.pos_in_chunk);
	const auto &mncs_in_chunk = tic._macro_nondef_constraints_in_chunk;
	auto it_mnc =
	  std::lower_bound(mncs_in_chunk.begin(),
			   mncs_in_chunk.end(),
			   cur_pos.pos_in_chunk,
			   [&](const transformed_input_chunk::
					_macro_nondef_constraint_in_chunk &mnc,
			       const transformed_input_chunk::
					_pos_in_chunk &begin_pos) {
			     return mnc.pos < begin_pos;
			   });
	while (it_mnc != mncs_in_chunk.end() &&
	       it_mnc->pos < end_pos_in_chunk) {
	  if (it_mnc->mnc.get_name() == m.get_name())
	    return false;
	  ++it_mnc;
	}
      }
      break;

    case _chunk::kind::header_inclusion:
      {
	const _header_inclusion_chunk &hic = cur_pos.chunk->hic;
	if (hic.needs_undef_before_include(m) ||
	    !hic.is_unmodified_by_include(m)) {
	  return false;
	}
      }
      break;

    case _chunk::kind::_dead:
      assert(0);
      __builtin_unreachable();
    }

    advance_to_next_chunk();
  }

  return true;
}

void depreprocessor::_emit_undef(const _macro_state &macro_state,
				 const _pos_in_output &mnc_pos,
				 const _in_order_chunks_type &in_order_chunks,
				 const _chunks_type::iterator &cur_run_begin)
{
  // As an optimization, only consider those #undef directives from
  // the original input which are located before the
  // raw_pp_token_index corresponding to mnc_pos. Use the depending
  // chunk's raw range end as an approximation.
  const raw_pp_token_index search_range_end =  mnc_pos.chunk->range_raw.end;

  const std::string &name = macro_state.m.get().get_name();
  pp_result::const_macro_undef_iterator it_mu;
  _pos_in_output mu_pos = _end_pos_in_output();
  for (it_mu = _pp_result.macro_undefs_begin();
       it_mu != _pp_result.macro_undefs_end(); ++it_mu) {
    if (!it_mu->is_predefined() &&
	it_mu->get_directive_range().end > search_range_end) {
      it_mu = _pp_result.macro_undefs_end();
      break;
    }

    if (it_mu->get_name() != name)
      continue;

    // See if placing this original #undef directive at either its "in
    // order" position (preferred) or its associated position within
    // the current run will work.
    auto can_use_mu_at_pos =
      [&](const _pos_in_output &pos) {
	// Note that #undefs from within headers won't work: we would
	// either have seen it already and updated macro_states
	// accordingly or the header is currently being processed and the
	// #undef is needed for it itself. In the latter case,
	// _can_place_undef_at() would not forbid it, so do the
	// check here.
	return (pos != _end_pos_in_output() &&
		(pos.chunk->k != _chunk::kind::header_inclusion ||
		 it_mu->is_predefined() ||
		 !(it_mu->get_directive_range()
		   .is_contained_in(pos.chunk->range_raw))) &&
		_can_place_undef_at(name, pos, mnc_pos,
				    macro_state.last_usage));
      };

    // Try to place the #undef at its "in order" position first.
    mu_pos = _find_macro_undef_in_order_pos(*it_mu, in_order_chunks);
    if (can_use_mu_at_pos(mu_pos))
      break;

    // Now try to place the #undef at its position within the
    // current run.
    mu_pos = _find_macro_undef_in_run_pos(*it_mu,
					  cur_run_begin, mnc_pos.chunk + 1);
    if (can_use_mu_at_pos(mu_pos))
      break;
  }

  if (it_mu != _pp_result.macro_undefs_end()) {
    switch (mu_pos.chunk->k) {
    case _chunk::kind::transformed_input:
      mu_pos.chunk->tic._add_macro_undef_to_emit(mu_pos.pos_in_chunk,
						 _macro_undef_to_emit{*it_mu});
      break;

    case _chunk::kind::header_inclusion:
      mu_pos.chunk->hic.add_macro_undef_to_emit(_macro_undef_to_emit{*it_mu});
      break;

    case _chunk::kind::_dead:
      assert(0);
      __builtin_unreachable();
    }

  } else {
    // Try to place the undef before the chunk where it's needed.
    mu_pos = _chunk_pos_in_output(mnc_pos.chunk);
    if (!_can_place_undef_at(name, mu_pos, mnc_pos, macro_state.last_usage)) {
      // Ok, give up, place the #undef right in front of the mnc
      // position.
      assert(mnc_pos.chunk->k == _chunk::kind::transformed_input);
      mu_pos = mnc_pos;
    }

    switch (mu_pos.chunk->k) {
    case _chunk::kind::transformed_input:
      mu_pos.chunk->tic._add_macro_undef_to_emit(mu_pos.pos_in_chunk,
						 _macro_undef_to_emit{name});
      break;

    case _chunk::kind::header_inclusion:
      mu_pos.chunk->hic.add_macro_undef_to_emit(_macro_undef_to_emit{name});
      break;

    case _chunk::kind::_dead:
      assert(0);
      __builtin_unreachable();
    }
  }
}

void depreprocessor::
_emit_define(const pp_result::macro &m,
	     const _pos_in_output &um_pos,
	     const _in_order_chunks_type &in_order_chunks,
	     const _chunks_type::iterator &cur_run_begin,
	     const _macro_state * const conflicting_macro_state)
{
  auto can_use_md_at_pos =
    [&](const _pos_in_output &pos) {
      if (pos != _end_pos_in_output() &&
	  _can_place_define_at(m, pos, um_pos,
			       conflicting_macro_state)) {
	// The #define can't be from any header: if it were, it would
	// have been seen already and registered in macro_states.
	assert(pos.chunk->k != _chunk::kind::header_inclusion ||
	       m.is_predefined() ||
	       !(m.get_directive_range()
		 .is_contained_in(pos.chunk->range_raw)));
	return true;
      } else {
	return false;
      }
    };

  // Try to place the #define at its "in order" position first.
  _pos_in_output md_pos = _find_macro_define_in_order_pos(m, in_order_chunks);
  if (!can_use_md_at_pos(md_pos)) {
    // Now try to place the #define at its position within the
    // current run.
    md_pos = _find_macro_define_in_run_pos(m, cur_run_begin, um_pos.chunk + 1);
    if (!can_use_md_at_pos(md_pos)) {
      // Try to place the undef before the chunk where it's needed.
      md_pos = _chunk_pos_in_output(um_pos.chunk);
      if (!_can_place_define_at(m, md_pos, um_pos, conflicting_macro_state)) {
	// Ok, give up, place the #undef right in front of the mnc
	// position.
	assert(um_pos.chunk->k == _chunk::kind::transformed_input);
	md_pos = um_pos;
      }
    }
  }

  switch (md_pos.chunk->k) {
  case _chunk::kind::transformed_input:
    md_pos.chunk->tic._add_macro_define_to_emit(md_pos.pos_in_chunk,
						_macro_define_to_emit{m});
    break;

  case _chunk::kind::header_inclusion:
    md_pos.chunk->hic.add_macro_define_to_emit(_macro_define_to_emit{m});
    break;

  case _chunk::kind::_dead:
    assert(0);
    __builtin_unreachable();
  }

  if (conflicting_macro_state) {
    // There's an incompatible #define active at the place where the
    // #define has just been emitted. Place an #undef before it.
    _emit_undef(*conflicting_macro_state,
		md_pos, in_order_chunks, cur_run_begin);
  }
}

bool depreprocessor::_can_place_undef_at(const std::string &name,
					 const _pos_in_output &placement_pos,
					 const _pos_in_output &mnc_pos,
					 const _pos_in_output &last_macro_usage)
  noexcept
{
  if (!(placement_pos <= mnc_pos))
    return false;

  if (last_macro_usage != _end_pos_in_output() &&
      !(last_macro_usage < placement_pos)) {
    return false;
  }

  return true;
}

depreprocessor::_pos_in_output depreprocessor::
_find_macro_undef_in_order_pos(const pp_result::macro_undef &mu,
			       const _in_order_chunks_type &in_order_chunks)
  noexcept
{
  if (in_order_chunks.empty())
    return _end_pos_in_output();

  if (!mu.is_predefined()) {
    return _find_directive_in_order_pos(mu.get_directive_range(),
					in_order_chunks);
  } else {
    return _chunk_pos_in_output(in_order_chunks.front().it_chunk);
  }
}

depreprocessor::_pos_in_output depreprocessor::
_find_macro_define_in_order_pos(const pp_result::macro &m,
				const _in_order_chunks_type &in_order_chunks)
  noexcept
{
  if (in_order_chunks.empty())
    return _end_pos_in_output();

  if (!m.is_predefined()) {
    return _find_directive_in_order_pos(m.get_directive_range(),
					in_order_chunks);
  } else {
    return _chunk_pos_in_output(in_order_chunks.front().it_chunk);
  }
}

depreprocessor::_pos_in_output depreprocessor::
_find_directive_in_order_pos(const raw_pp_tokens_range &directive_range,
			     const _in_order_chunks_type &in_order_chunks)
  noexcept
{
  const auto it_ioc = std::lower_bound(in_order_chunks.begin(),
				       in_order_chunks.end(),
				       directive_range,
				       [](const _in_order_chunk &ioc,
					  const raw_pp_tokens_range &range) {
					 return (ioc.attributed_range_raw <
						 range);
				       });

  if (it_ioc == in_order_chunks.end())
    return _end_pos_in_output();

  // All the range from 0 to
  // in_order_chunks.back().attributed_range_raw.end is covered by the
  // in_order_chunks.
  assert(!(directive_range < it_ioc->attributed_range_raw));

  // Also, the attributed ranges begin and end at pp_token boundaries
  // each and do not cross preprocessing directives.
  assert(directive_range.is_contained_in(it_ioc->attributed_range_raw));

  switch (it_ioc->it_chunk->k) {
  case _chunk::kind::transformed_input:
    return _pos_in_output{
		it_ioc->it_chunk,
		(it_ioc->it_chunk->tic._directive_range_to_pos_in_chunk
		 (directive_range, _pp_result))
	   };

  case _chunk::kind::header_inclusion:
    return _pos_in_output{it_ioc->it_chunk};

  case _chunk::kind::_dead:
    assert(0);
    __builtin_unreachable();
  }
}

depreprocessor::_pos_in_output depreprocessor::
_find_macro_undef_in_run_pos(const pp_result::macro_undef &mu,
			     const _chunks_type::iterator &run_begin,
			     const _chunks_type::iterator &run_end) noexcept
{
  if (run_begin == run_end)
    return _end_pos_in_output();

  if (!mu.is_predefined()) {
    return _find_directive_in_run_pos(mu.get_directive_range(),
				      run_begin, run_end);
  } else {
    return _chunk_pos_in_output(run_begin);
  }
}

depreprocessor::_pos_in_output depreprocessor::
_find_macro_define_in_run_pos(const pp_result::macro &m,
			      const _chunks_type::iterator &run_begin,
			      const _chunks_type::iterator &run_end) noexcept
{
  if (run_begin == run_end)
    return _end_pos_in_output();

  if (!m.is_predefined()) {
    return _find_directive_in_run_pos(m.get_directive_range(),
				      run_begin, run_end);
  } else {
    return _chunk_pos_in_output(run_begin);
  }
}

depreprocessor::_pos_in_output depreprocessor::
_find_directive_in_run_pos(const raw_pp_tokens_range &directive_range,
			   const _chunks_type::iterator &run_begin,
			   const _chunks_type::iterator &run_end) noexcept
{
  const auto it_chunk_in_run
    = std::lower_bound(run_begin, run_end, directive_range,
		       [](const _chunk &c, const raw_pp_tokens_range &range) {
			 return c.range_raw < range;
		       });

  if (it_chunk_in_run == run_end)
    return _end_pos_in_output();

  // it_chunk_in_run gets attributed the range from
  // (it_chunk_in_run - 1)->range_raw.end (or 0 if no previous chunk
  // exists) to it_chunk_in_run->range_raw.end. By conststruction, we
  // have (it_chunk_in_run - 1)->range_raw.end <= directive_range.begin
  // and directive_range.begin < it_chunk_in_run->range_raw.end.
  // Directives don't cross chunks, and hence...
  assert(directive_range.end <= it_chunk_in_run->range_raw.end);

  switch (it_chunk_in_run->k) {
  case _chunk::kind::transformed_input:
    return _pos_in_output{
		it_chunk_in_run,
		(it_chunk_in_run->tic._directive_range_to_pos_in_chunk
		 (directive_range, _pp_result))
	   };

  case _chunk::kind::header_inclusion:
    return _pos_in_output{it_chunk_in_run};

  case _chunk::kind::_dead:
    assert(0);
    __builtin_unreachable();
  }
}

depreprocessor::_pos_in_output depreprocessor::
_chunk_pos_in_output(const _chunks_type::iterator &it_chunk) noexcept
{
  if (it_chunk == _chunks.end())
    return _end_pos_in_output();

  switch (it_chunk->k) {
  case _chunk::kind::transformed_input:
    return _pos_in_output{
	     it_chunk,
	     it_chunk->tic._begin_pos_in_chunk()
	   };

  case _chunk::kind::header_inclusion:
    return _pos_in_output{it_chunk};

  case _chunk::kind::_dead:
    assert(0);
    __builtin_unreachable();
  }
}

depreprocessor::_pos_in_output depreprocessor::_end_pos_in_output() noexcept
{
  return _pos_in_output{_chunks.end()};
}


void
depreprocessor::_write_directive(source_writer &writer,
				 const raw_pp_tokens_range &directive_range,
				 const pp_result &pp_result,
				 _source_reader_cache &source_reader_cache)
{
  auto it_source = pp_result.intersecting_sources_begin(directive_range);
  assert(it_source !=  pp_result.intersecting_sources_end(directive_range));
  source_reader &reader = source_reader_cache.get(*it_source);
  const raw_pp_tokens &raw_toks = pp_result.get_raw_tokens();
  writer.append
    (reader,
     range_in_file{
       raw_toks[directive_range.begin].get_range_in_file().begin,
       raw_toks[directive_range.end - 1].get_range_in_file().end
     });
}

void depreprocessor::_write_define(source_writer &writer,
				   const pp_result::macro &m,
				   const pp_result &pp_result,
				   _source_reader_cache &source_reader_cache,
				   output_remarks &remarks)
{
  if (!m.is_predefined()) {
    _write_directive(writer, m.get_directive_range(), pp_result,
		     source_reader_cache);
  } else {
    if (!m.is_builtin_special()) {
      std::string s = "#define " + m.get_name();
      if (m.is_func_like()) {
	s += '(';
	const auto &arg_names = m.get_arg_names();
	for (auto it_arg = arg_names.begin(); it_arg != arg_names.end();
	     ++it_arg) {
	  if (it_arg != arg_names.begin())
	    s += ", ";

	  if (it_arg != arg_names.end() - 1 || !m.is_variadic()) {
	    s += *it_arg;

	  } else {
	    if (*it_arg != "__VA_ARGS__")
	      s += *it_arg;
	    s += "...";
	  }
	}
	s += ')';
      }

      const auto &repl_toks = m.get_repl();
      if (!repl_toks.empty()) {
	s += ' ';
	for (const auto &tok : repl_toks)
	  s+= tok.get_value();
      }
      s += '\n';
      writer.append(s);
    } else {
      // Ugh, something bad happend: user code #undeffed some builtin
      // special macro like __FILE__, __LINE__, __COUNTER__
      // and due to reordering, we now need it again.
      // Report a warning and emit an #error in the output so that
      // the user can figure out what went wrong.
      output_remark remark{
	output_remark::severity::fatal,
	writer.get_cur_line(),
	("unable to emit define for required special macro \""
	 + m.get_name() + "\"")
      };
      remarks.add(std::move(remark));
      writer.append("#error \"required special macro " + m.get_name() +
		    " has been undefined before\"\n");
    }
  }
}

void depreprocessor::_write_undef(source_writer &writer,
				  const _macro_undef_to_emit &mu,
				  const pp_result &pp_result,
				  _source_reader_cache &source_reader_cache)
{
  if (mu.original && !mu.original->is_predefined()) {
    _write_directive(writer, mu.original->get_directive_range(), pp_result,
		     source_reader_cache);
  } else {
    const std::string &name = mu.original ? mu.original->get_name() : mu.name;
    writer.append("#undef " + name + "\n");
  }
}

void depreprocessor::
_write_defines_and_undefs(source_writer &writer,
			  const std::vector<_macro_define_to_emit> &mds,
			  const std::vector<_macro_undef_to_emit> &mus,
			  const pp_result &pp_result,
			  _source_reader_cache &source_reader_cache,
			  output_remarks &remarks)
{
  auto write_md =
    [&](const _macro_define_to_emit &md) {
    _write_define(writer, md.m.get(), pp_result, source_reader_cache,
		  remarks);

    // Always emit an empty line after the directive.
    writer.append(source_writer::newline_tag{});
  };

  auto write_mu =
    [&](const _macro_undef_to_emit &mu) {
    _write_undef(writer, mu, pp_result, source_reader_cache);
  };

  // Write in directive order, except for undefs for which a
  // corresponding define is about to get emitted at cur_pos:
  // always put these immediately in front of the #define.
  const auto mus_begin = mus.begin();
  const auto mus_end = mus.end();
  const auto mds_begin = mds.begin();
  const auto mds_end = mds.end();
  auto it_mu = mus_begin;
  auto it_md = mds_begin;
  while (it_mu != mus_end || it_md != mds_end) {
    while (it_md != mds_end &&
	   (it_mu == mus_end || *it_md < *it_mu)) {
      const auto it_matching_mu =
	std::find_if(mus_begin, mus_end,
		     [&](const _macro_undef_to_emit &mu) {
		       return (it_md->m.get().get_name() ==
			       (mu.original ?
				mu.original->get_name() :
				mu.name));
		     });
      if (it_matching_mu != mus_end)
	write_mu(*it_matching_mu);
      write_md(*it_md);
      ++it_md;
    }

    while (it_mu != mus_end &&
	   (it_md == mds_end || *it_mu < *it_md)) {
      const std::string &name =  (it_mu->original ?
				  it_mu->original->get_name() :
				  it_mu->name);
      if (!std::any_of(mds_begin, mds_end,
		       [&](const _macro_define_to_emit &md) {
			 return md.m.get().get_name() == name;
		       })) {
	write_mu(*it_mu);
      }
      ++it_mu;
    }
  }
}
