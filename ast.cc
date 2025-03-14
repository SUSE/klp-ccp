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

#include <cassert>
#include "ast_impl.hh"
#include "ast_processor.hh"
#include "semantic_except.hh"
#include "pp_token.hh"
#include "raw_pp_token.hh"

using namespace klp::ccp::ast;

template<typename T>
static T* mv_p(T* &&p)
{
	T *_p = nullptr;
	std::swap(p, _p);
	return _p;
}

_ast_entity::_ast_entity(const pp_tokens_range &tr) noexcept
  : _parent(nullptr), _tokens_range(tr)
{}

_ast_entity::_ast_entity(const _ast_entity &ae) noexcept
  : _parent(nullptr), _tokens_range(ae._tokens_range)
{}

_ast_entity::~_ast_entity() noexcept = default;

void _ast_entity::_extend_tokens_range(const pp_tokens_range &tr) noexcept
{
  assert(_tokens_range.end <= tr.begin);
  _tokens_range.end = tr.end;
}


_typed::~_typed() noexcept = default;



expr_list::expr_list(expr* &&e)
  : ast_entity(e->get_tokens_range())
{
  auto _e = std::ref(*mv_p(std::move(e)));
  try {
    _exprs.push_back(_e);
  } catch(...) {
    delete &_e.get();
    throw;
  }
  _e.get()._set_parent(*this);
}

expr_list::expr_list(expr_list &&el)
  : ast_entity(el), _exprs(std::move(el._exprs))
{
  for (auto e : _exprs)
    e.get()._reset_parent(*this, el);
}

expr_list::~expr_list() noexcept
{
  for (auto e : _exprs)
    delete &e.get();
}

void expr_list::extend(expr* &&e)
{
  auto _e = std::ref(*mv_p(std::move(e)));
  try {
    _exprs.push_back(_e);
  } catch(...) {
    delete &_e.get();
    throw;
  }
  _e.get()._set_parent(*this);
  _extend_tokens_range(_e.get().get_tokens_range());
}

expr& expr_list::operator[](const std::size_t i) noexcept
{
  assert(i < size());
  return _exprs[i];
}

const expr& expr_list::operator[](const std::size_t i) const noexcept
{
  assert(i < size());
  return _exprs[i];
}

_ast_entity* expr_list::_get_child(const size_t i) const noexcept
{
  if (i >= _exprs.size())
    return nullptr;

  return &_exprs[i].get();
}

void expr_list::_process(processor<void> &p)
{
  p(*this);
}

void expr_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr::expr(const pp_tokens_range &tr) noexcept
  : typed_ast_entity(tr), _is_lvalue(false)
{}

expr::~expr() noexcept = default;

static klp::ccp::pp_tokens_range
tr_from_aes(const _ast_entity &ae1,
	    const _ast_entity &ae2) noexcept
{
  return klp::ccp::pp_tokens_range{
    ae1.get_tokens_range().begin,
    ae2.get_tokens_range().end
  };
}

bool expr::is_constexpr() const noexcept
{
  return (static_cast<bool>(_value) &&
	  ((_value->get_value_kind() !=
	    constexpr_value::value_kind::vk_address) ||
	   !_value->get_address_value().is_dereferenced()));
}

bool expr::is_dereferenced_const_addr() const noexcept
{
  return (static_cast<bool>(_value) &&
	  (_value->get_value_kind() ==
	   constexpr_value::value_kind::vk_address) &&
	  _value->get_address_value().is_dereferenced());
}

const constexpr_value&
expr::get_constexpr_value() const noexcept
{
  assert(static_cast<bool>(_value));
  return *_value;
}

const expr& expr::skip_parens_down() const noexcept
{
  return *this;
}

const _ast_entity* expr::skip_parens_up() const noexcept
{
  return this;
}

const _ast_entity* expr::skip_parens_up() noexcept
{
  return this;
}


const _ast_entity* expr::get_non_parens_parent() const noexcept
{
  const _ast_entity *p = this->get_parent();

  if (!p)
    return p;

  p->process<void, type_set<expr_parenthesized> >
    (wrap_callables<default_action_nop>
     ([&](const expr_parenthesized &ep) {
       p = ep.skip_parens_up();
      }));

  return p;
}

_ast_entity* expr::get_non_parens_parent() noexcept
{
  _ast_entity *p = this->get_parent();

  if (!p)
    return p;

  p->process<void, type_set<expr_parenthesized> >
    (wrap_callables<default_action_nop>
     ([&](expr_parenthesized &ep) {
       p = ep.skip_parens_up();
      }));

  return p;
}

bool expr::is_lvalue() const noexcept
{
  // The expr should have been evaluated.
  assert(this->get_type());
  return _is_lvalue;
}

void expr::_set_value(std::unique_ptr<constexpr_value> &&cv)
{
  assert(!_value);
  _value = std::move(cv);
}

void expr::_set_lvalue(const bool is_lvalue) noexcept
{
  _is_lvalue = is_lvalue;
}


expr_comma::expr_comma(expr* &&l, expr* &&r) noexcept
  : expr(tr_from_aes(*l, *r)),
    _left(*mv_p(std::move(l))), _right(*mv_p(std::move(r)))
{
  _left._set_parent(*this);
  _right._set_parent(*this);
}

expr_comma::~expr_comma() noexcept
{
  delete &_left;
  delete &_right;
}

_ast_entity* expr_comma::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_left;

  case 1:
    return &_right;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_comma::_process(processor<void> &p)
{
  p(*this);
}

void expr_comma::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_comma::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_comma::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_assignment::expr_assignment(const assign_op op, expr* &&lhs, expr* &&rhs)
  noexcept
  : expr(tr_from_aes(*lhs, *rhs)), _op(op),
    _lhs(*mv_p(std::move(lhs))), _rhs(*mv_p(std::move(rhs)))
{
  _lhs._set_parent(*this);
  _rhs._set_parent(*this);
}

expr_assignment::~expr_assignment() noexcept
{
  delete &_lhs;
  delete &_rhs;
}

_ast_entity* expr_assignment::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_lhs;

  case 1:
    return &_rhs;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_assignment::_process(processor<void> &p)
{
  p(*this);
}

void expr_assignment::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_assignment::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_assignment::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_conditional::expr_conditional(expr* &&cond,
				   expr* &&expr_true, expr* &&expr_false)
  noexcept
  : expr(tr_from_aes(*cond, *expr_false)),
    _cond(*mv_p(std::move(cond))), _expr_true(mv_p(std::move(expr_true))),
    _expr_false(*mv_p(std::move(expr_false)))
{
  _cond._set_parent(*this);
  _expr_true->_set_parent(*this);
  _expr_false._set_parent(*this);
}

expr_conditional::expr_conditional(expr* &&cond, expr* &&expr_false) noexcept
  : expr(tr_from_aes(*cond, *expr_false)),
    _cond(*mv_p(std::move(cond))), _expr_true(nullptr),
    _expr_false(*mv_p(std::move(expr_false)))
{
  _cond._set_parent(*this);
  _expr_false._set_parent(*this);
}

expr_conditional::~expr_conditional() noexcept
{
  delete &_cond;
  delete _expr_true;
  delete &_expr_false;
}

_ast_entity* expr_conditional::_get_child(const size_t i) const noexcept
{
  if (!i) {
    return &_cond;
  } else if (i == 1) {
    if (_expr_true)
      return _expr_true;
    else
      return &_expr_false;
  } else if (i == 2 && _expr_true) {
    return &_expr_false;
  }

  return nullptr;
}

void expr_conditional::_process(processor<void> &p)
{
  p(*this);
}

void expr_conditional::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_conditional::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_conditional::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_binop::expr_binop(const binary_op op, expr* &&l, expr* &&r) noexcept
  : expr(tr_from_aes(*l, *r)), _op(op),
    _left(*mv_p(std::move(l))), _right(*mv_p(std::move(r)))
{
  _left._set_parent(*this);
  _right._set_parent(*this);
}

expr_binop::~expr_binop() noexcept
{
  delete &_left;
  delete &_right;
}

_ast_entity* expr_binop::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_left;

  case 1:
    return &_right;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_binop::_process(processor<void> &p)
{
  p(*this);
}

void expr_binop::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_binop::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_binop::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_cast::expr_cast(const pp_tokens_range &tr, type_name* &&tn, expr* &&e)
  noexcept
  : expr(tr), _tn(*mv_p(std::move(tn))), _e(*mv_p(std::move(e)))
{
  _tn._set_parent(*this);
  _e._set_parent(*this);
}

expr_cast::~expr_cast() noexcept
{
  delete &_tn;
  delete &_e;
}

_ast_entity* expr_cast::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_tn;

  case 1:
    return &_e;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_cast::_process(processor<void> &p)
{
  p(*this);
}

void expr_cast::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_cast::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_cast::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_label_addr::expr_label_addr(const pp_tokens_range &tr,
				 const pp_token_index label_tok) noexcept
  : expr(tr), _label_tok(label_tok), _resolved(nullptr)
{}

expr_label_addr::~expr_label_addr() noexcept = default;

void expr_label_addr::set_resolved(const stmt_labeled &resolved) noexcept
{
  assert(!_resolved);
  _resolved = &resolved;
}

_ast_entity* expr_label_addr::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void expr_label_addr::_process(processor<void> &p)
{
  p(*this);
}

void expr_label_addr::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_label_addr::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_label_addr::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_unop_pre::expr_unop_pre(const pp_tokens_range &tr,
			     const unary_op_pre op, expr* &&e) noexcept
  : expr(tr), _op(op), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

expr_unop_pre::~expr_unop_pre() noexcept
{
  delete &_e;
}

_ast_entity* expr_unop_pre::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_e;

  return nullptr;
}

void expr_unop_pre::_process(processor<void> &p)
{
  p(*this);
}

void expr_unop_pre::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_unop_pre::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_unop_pre::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_sizeof_expr::expr_sizeof_expr(const pp_tokens_range &tr, expr* &&e)
  noexcept
  : expr(tr), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

expr_sizeof_expr::~expr_sizeof_expr() noexcept
{
  delete &_e;
}

_ast_entity* expr_sizeof_expr::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_e;

  return nullptr;
}

void expr_sizeof_expr::_process(processor<void> &p)
{
  p(*this);
}

void expr_sizeof_expr::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_sizeof_expr::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_sizeof_expr::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_sizeof_type_name::expr_sizeof_type_name(const pp_tokens_range &tr,
					     type_name* &&tn) noexcept
  : expr(tr), _tn(*mv_p(std::move(tn)))
{
  _tn._set_parent(*this);
}

expr_sizeof_type_name::~expr_sizeof_type_name() noexcept
{
  delete &_tn;
}

_ast_entity* expr_sizeof_type_name::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_tn;

  return nullptr;
}

void expr_sizeof_type_name::_process(processor<void> &p)
{
  p(*this);
}

void expr_sizeof_type_name::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_sizeof_type_name::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_sizeof_type_name::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_alignof_expr::expr_alignof_expr(const pp_tokens_range &tr, expr* &&e)
  noexcept
  : expr(tr), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

expr_alignof_expr::~expr_alignof_expr() noexcept
{
  delete &_e;
}

_ast_entity* expr_alignof_expr::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_e;

  return nullptr;
}

void expr_alignof_expr::_process(processor<void> &p)
{
  p(*this);
}

void expr_alignof_expr::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_alignof_expr::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_alignof_expr::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_alignof_type_name::expr_alignof_type_name(const pp_tokens_range &tr,
					     type_name* &&tn) noexcept
  : expr(tr), _tn(*mv_p(std::move(tn)))
{
  _tn._set_parent(*this);
}

expr_alignof_type_name::~expr_alignof_type_name() noexcept
{
  delete &_tn;
}

_ast_entity* expr_alignof_type_name::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_tn;

  return nullptr;
}

void expr_alignof_type_name::_process(processor<void> &p)
{
  p(*this);
}

void expr_alignof_type_name::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_alignof_type_name::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_alignof_type_name::_process(const_processor<bool> &p) const
{
  return p(*this);
}


generic_association::generic_association(const pp_tokens_range &tr,
					 type_name* &&tn, expr *&&e) noexcept
  : ast_entity(tr), _tn(mv_p(std::move(tn))), _e(*mv_p(std::move(e)))
{
  _tn->_set_parent(*this);
  _e._set_parent(*this);
}

generic_association::generic_association(const pp_tokens_range &tr, expr *&&e)
  noexcept
  : ast_entity(tr), _tn(nullptr), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

generic_association::~generic_association() noexcept
{
  if (_tn)
    delete _tn;

  delete &_e;
}

_ast_entity* generic_association::_get_child(const size_t i) const noexcept
{
  if (!_tn) {
    if (!i)
      return &_e;
    else
      return nullptr;
  }

  switch (i) {
  case 0:
    return _tn;

  case 1:
    return &_e;

  default:
    return nullptr;
  }
}

void generic_association::_process(processor<void> &p)
{
  p(*this);
}

void generic_association::_process(const_processor<void> &p) const
{
  p(*this);
}

bool generic_association::_process(processor<bool> &p)
{
  return p(*this);
}

bool generic_association::_process(const_processor<bool> &p) const
{
  return p(*this);
}


generic_association_list::generic_association_list(generic_association *&&ga)
  : ast_entity(ga->get_tokens_range())
{
  auto _ga = std::ref(*mv_p(std::move(ga)));
  try {
    _gas.push_back(_ga);
  } catch(...) {
    delete &_ga.get();
    throw;
  }
  _ga.get()._set_parent(*this);
}

generic_association_list::~generic_association_list() noexcept
{
  for (auto ga : _gas)
    delete &ga.get();
}

void generic_association_list::extend(generic_association *&&ga)
{
  auto _ga = std::ref(*mv_p(std::move(ga)));
  try {
    _gas.push_back(_ga);
  } catch(...) {
    delete &_ga.get();
    throw;
  }
  _ga.get()._set_parent(*this);
  _extend_tokens_range(_ga.get().get_tokens_range());
}

_ast_entity* generic_association_list::_get_child(const size_t i) const noexcept
{
  if (i >= _gas.size())
    return 0;

  return &_gas[i].get();
}

void generic_association_list::_process(processor<void> &p)
{
  p(*this);
}

void generic_association_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool generic_association_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool generic_association_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_generic::expr_generic(const pp_tokens_range &tr,
			   expr *&&ctrl_e,
			   generic_association_list *&&gal) noexcept
  : expr(tr), _ctrl_e(*mv_p(std::move(ctrl_e))), _gal(*mv_p(std::move(gal))),
    _match(nullptr)
{
  _ctrl_e._set_parent(*this);
  _gal._set_parent(*this);
}

expr_generic::~expr_generic() noexcept
{
  delete &_ctrl_e;
  delete &_gal;
}

const expr& expr_generic::get_match() const noexcept
{
  assert(_match);
  return *_match;
}

_ast_entity* expr_generic::_get_child(const size_t i) const noexcept
{
  switch (i) {
  case 0:
    return &_ctrl_e;

  case 1:
    return &_gal;

  default:
    return nullptr;
  }
}

void expr_generic::_process(processor<void> &p)
{
  p(*this);
}

void expr_generic::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_generic::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_generic::_process(const_processor<bool> &p) const
{
  return p(*this);
}



expr_builtin_choose_expr::expr_builtin_choose_expr(const pp_tokens_range &tr,
						   expr *&&cond,
						   expr *&&expr_true,
						   expr *&&expr_false) noexcept
  : expr(tr), _cond(*mv_p(std::move(cond))),
    _expr_true(*mv_p(std::move(expr_true))),
    _expr_false(*mv_p(std::move(expr_false)))
{
  _cond._set_parent(*this);
  _expr_true._set_parent(*this);
  _expr_false._set_parent(*this);
}

expr_builtin_choose_expr::~expr_builtin_choose_expr() noexcept
{
  delete &_cond;
  delete &_expr_true;
  delete &_expr_false;
}

_ast_entity* expr_builtin_choose_expr::_get_child(const size_t i) const noexcept
{
  switch (i) {
  case 0:
    return &_cond;

  case 1:
    return &_expr_true;

  case 2:
    return &_expr_false;

  default:
    return nullptr;
  }
}

void expr_builtin_choose_expr::_process(processor<void> &p)
{
  p(*this);
}

void expr_builtin_choose_expr::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_builtin_choose_expr::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_builtin_choose_expr::_process(const_processor<bool> &p) const
{
  return p(*this);
}


offset_member_designator::member::
member(const pp_token_index _member_tok, const bool _ptr_base) noexcept
  : member_tok(_member_tok), ptr_base(_ptr_base)
{}

offset_member_designator::component::component(const member &m) noexcept
  : _k(kind::k_member), _m(m)
{}

offset_member_designator::component::component(expr &index_expr) noexcept
  : _k(kind::k_array_subscript), _index_expr(&index_expr)
{}

offset_member_designator::component::component(component &&c) noexcept
  : _k(c._k)
{
  switch(_k) {
  case kind::k_member:
    _m = c._m;
    break;

  case kind::k_array_subscript:
    _index_expr = c._index_expr;
    c._index_expr = nullptr;
    break;
  }
}

offset_member_designator::component::~component() noexcept
{
  switch (_k) {
  case kind::k_member:
    break;

  case kind::k_array_subscript:
    if (_index_expr)
      _index_expr->~expr();
    break;
  };
}

const offset_member_designator::member&
offset_member_designator::component::get_member() const noexcept
{
  assert(_k == kind::k_member);
  return _m;
}

expr& offset_member_designator::component::get_index_expr() noexcept
{
  assert(_k == kind::k_array_subscript);
  return *_index_expr;
}

const expr& offset_member_designator::component::get_index_expr() const noexcept
{
  assert(_k == kind::k_array_subscript);
  return *_index_expr;
}

offset_member_designator::
offset_member_designator(const pp_token_index member_tok)
  : ast_entity(pp_tokens_range{member_tok, member_tok + 1})
{
  _components.emplace_back(member{member_tok, false});
}

offset_member_designator::~offset_member_designator() noexcept = default;

void offset_member_designator::extend(const pp_token_index member_tok,
				      const bool ptr_base)
{
  _components.emplace_back(member{member_tok, ptr_base});
  _extend_tokens_range(pp_tokens_range{member_tok, member_tok + 1});
}

void offset_member_designator::extend(expr* &&index_expr)
{
  expr * const _index_expr = mv_p(std::move(index_expr));
  try {
    _components.emplace_back(*_index_expr);
  } catch (...) {
    delete &_index_expr;
    throw;
  }
  _index_expr->_set_parent(*this);
  // Account for the trailing ']'.
  const pp_tokens_range &tr = _index_expr->get_tokens_range();
  _extend_tokens_range(pp_tokens_range{tr.begin, tr.end + 1});
}

_ast_entity* offset_member_designator::_get_child(const size_t i) const noexcept
{
  size_t j = 0;
  for (auto &c : _components) {
    if (c.get_kind() == component::kind::k_array_subscript) {
      if (j == i)
	return const_cast<expr *>(&c.get_index_expr());
      ++j;
    }
  }

  return nullptr;
}

void offset_member_designator::_process(processor<void> &p)
{
  p(*this);
}

void offset_member_designator::_process(const_processor<void> &p) const
{
  p(*this);
}

bool offset_member_designator::_process(processor<bool> &p)
{
  return p(*this);
}

bool offset_member_designator::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_builtin_offsetof::
expr_builtin_offsetof(const pp_tokens_range &tr, type_name* &&tn,
		      offset_member_designator* &&member_designator) noexcept
  : expr(tr), _tn(*mv_p(std::move(tn))),
    _member_designator(*mv_p(std::move(member_designator)))
{
  _tn._set_parent(*this);
  _member_designator._set_parent(*this);
}

expr_builtin_offsetof::~expr_builtin_offsetof() noexcept
{
  delete &_tn;
  delete &_member_designator;
}

_ast_entity* expr_builtin_offsetof::_get_child(const size_t i) const noexcept
{
    switch (i)
  {
  case 0:
    return &_tn;

  case 1:
    return &_member_designator;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_builtin_offsetof::_process(processor<void> &p)
{
  p(*this);
}

void expr_builtin_offsetof::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_builtin_offsetof::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_builtin_offsetof::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_builtin_types_compatible_p::
expr_builtin_types_compatible_p(const pp_tokens_range &tr,
				type_name* &&tn1, type_name* &&tn2) noexcept
  : expr(tr), _tn1(*mv_p(std::move(tn1))), _tn2(*mv_p(std::move(tn2)))
{
  _tn1._set_parent(*this);
  _tn2._set_parent(*this);
}
expr_builtin_types_compatible_p::~expr_builtin_types_compatible_p() noexcept
{
  delete &_tn1;
  delete &_tn2;
}

_ast_entity* expr_builtin_types_compatible_p::_get_child(const size_t i)
  const noexcept
{
  switch (i)
  {
  case 0:
    return &_tn1;

  case 1:
    return &_tn2;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_builtin_types_compatible_p::_process(processor<void> &p)
{
  p(*this);
}

void expr_builtin_types_compatible_p::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_builtin_types_compatible_p::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_builtin_types_compatible_p::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_builtin_va_arg::expr_builtin_va_arg(const pp_tokens_range &tr,
					 expr* &&e, type_name* &&tn) noexcept
  : expr(tr), _e(*mv_p(std::move(e))), _tn(*mv_p(std::move(tn)))
{
  _e._set_parent(*this);
  _tn._set_parent(*this);
}

expr_builtin_va_arg::~expr_builtin_va_arg() noexcept
{
  delete &_e;
  delete &_tn;
}

_ast_entity* expr_builtin_va_arg::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_e;

  case 1:
    return &_tn;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_builtin_va_arg::_process(processor<void> &p)
{
  p(*this);
}

void expr_builtin_va_arg::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_builtin_va_arg::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_builtin_va_arg::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_array_subscript::expr_array_subscript(const pp_tokens_range &tr,
					   expr* &&base, expr* &&index)
  noexcept
  : expr(tr), _base(*mv_p(std::move(base))), _index(*mv_p(std::move(index)))
{
  _base._set_parent(*this);
  _index._set_parent(*this);
}

expr_array_subscript::~expr_array_subscript() noexcept
{
  delete &_base;
  delete &_index;
}

_ast_entity* expr_array_subscript::_get_child(const size_t i) const noexcept
{
    switch (i)
  {
  case 0:
    return &_base;

  case 1:
    return &_index;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_array_subscript::_process(processor<void> &p)
{
  p(*this);
}

void expr_array_subscript::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_array_subscript::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_array_subscript::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_func_invocation::expr_func_invocation(const pp_tokens_range &tr,
		     expr* &&func, expr_list* &&args) noexcept
  : expr(tr), _func(*mv_p(std::move(func))), _args(mv_p(std::move(args)))
{
  _func._set_parent(*this);
  _args->_set_parent(*this);
}

expr_func_invocation::expr_func_invocation(const pp_tokens_range &tr,
					   expr* &&func) noexcept
  : expr(tr), _func(*mv_p(std::move(func))), _args(nullptr)
{
  _func._set_parent(*this);
}

expr_func_invocation::~expr_func_invocation() noexcept
{
  delete &_func;
  delete _args;
}

_ast_entity* expr_func_invocation::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_func;

  else if (i == 1)
    return _args;

  else
    return nullptr;
}

void expr_func_invocation::_process(processor<void> &p)
{
  p(*this);
}

void expr_func_invocation::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_func_invocation::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_func_invocation::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_member_deref::expr_member_deref(const pp_tokens_range &tr,
				     const member_deref_type deref_type,
				     expr* &&base,
				     const pp_token_index member_tok)
  noexcept
  : expr(tr), _deref_type(deref_type), _base(*mv_p(std::move(base))),
    _member_tok(member_tok)
{
  _base._set_parent(*this);
}

expr_member_deref::~expr_member_deref() noexcept
{
  delete &_base;
}

_ast_entity* expr_member_deref::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_base;

  return nullptr;
}

void expr_member_deref::_process(processor<void> &p)
{
  p(*this);
}

void expr_member_deref::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_member_deref::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_member_deref::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_unop_post::expr_unop_post(const pp_tokens_range &tr,
			       const unary_op_post op, expr* &&e) noexcept
  : expr(tr), _op(op), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

expr_unop_post::~expr_unop_post() noexcept
{
  delete &_e;
}

_ast_entity* expr_unop_post::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_e;

  return nullptr;
}

void expr_unop_post::_process(processor<void> &p)
{
  p(*this);
}

void expr_unop_post::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_unop_post::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_unop_post::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_compound_literal::expr_compound_literal(const pp_tokens_range &tr,
					     type_name* &&tn,
					     initializer_list* &&il) noexcept
  : expr(tr), _tn(*mv_p(std::move(tn))), _il(mv_p(std::move(il)))
{
  _tn._set_parent(*this);
  if (_il)
    _il->_set_parent(*this);
}

expr_compound_literal::~expr_compound_literal() noexcept
{
  delete &_tn;
  delete _il;
}

_ast_entity* expr_compound_literal::_get_child(const size_t i) const noexcept
{
    switch (i)
  {
  case 0:
    return &_tn;

  case 1:
    return _il;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void expr_compound_literal::_process(processor<void> &p)
{
  p(*this);
}

void expr_compound_literal::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_compound_literal::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_compound_literal::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_statement::expr_statement(const pp_tokens_range &tr,
			       stmt_compound* &&s) noexcept
  : expr(tr), _s(*mv_p(std::move(s)))
{
  _s._set_parent(*this);
}

expr_statement::~expr_statement() noexcept
{
  delete &_s;
}

_ast_entity* expr_statement::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_s;

  return nullptr;
}

void expr_statement::_process(processor<void> &p)
{
  p(*this);
}

void expr_statement::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_statement::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_statement::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_id::resolved::resolved() noexcept
  : _kind(resolved_kind::none)
{}

expr_id::resolved::resolved(const builtin_func::factory &builtin_func_fac)
  noexcept
  : _kind(resolved_kind::builtin_func), _builtin_func_fac(&builtin_func_fac)
{}

expr_id::resolved::resolved(const builtin_var::factory builtin_var_fac)
  noexcept
  : _kind(resolved_kind::builtin_var), _builtin_var_fac(builtin_var_fac)
{}

expr_id::resolved::resolved(init_declarator &id) noexcept
  : _kind(resolved_kind::init_declarator), _id(&id)
{}

expr_id::resolved::resolved(parameter_declaration_declarator &pdd) noexcept
  : _kind(resolved_kind::parameter_declaration_declarator), _pdd(&pdd)
{}

expr_id::resolved::resolved(function_definition &fd) noexcept
  : _kind(resolved_kind::function_definition), _fd(&fd)
{}

expr_id::resolved::resolved(stmt_labeled &sl) noexcept
  : _kind(resolved_kind::stmt_labeled), _sl(&sl)
{}

expr_id::resolved::resolved(enumerator &e) noexcept
  : _kind(resolved_kind::enumerator), _e(&e)
{}

expr_id::resolved::resolved(identifier_list &pil) noexcept
  : _kind(resolved_kind::in_param_id_list), _pil(&pil)
{}

const klp::ccp::builtin_func::factory&
expr_id::resolved::get_builtin_func_factory() const noexcept
{
  assert(_kind == resolved_kind::builtin_func);
  return *_builtin_func_fac;
}

klp::ccp::builtin_var::factory expr_id::resolved::get_builtin_var_factory()
  const noexcept
{
  assert(_kind == resolved_kind::builtin_var);
  return _builtin_var_fac;
}

init_declarator& expr_id::resolved::get_init_declarator() const noexcept
{
  assert(_kind == resolved_kind::init_declarator);
  return *_id;
}

parameter_declaration_declarator&
expr_id::resolved::get_parameter_declaration_declarator() const noexcept
{
  assert(_kind == resolved_kind::parameter_declaration_declarator);
  return *_pdd;
}

function_definition& expr_id::resolved::get_function_definition()
const noexcept
{
  assert(_kind == resolved_kind::function_definition);
  return *_fd;
}

stmt_labeled& expr_id::resolved::get_stmt_labeled() const noexcept
{
  assert(_kind == resolved_kind::stmt_labeled);
  return *_sl;
}

enumerator& expr_id::resolved::get_enumerator() const noexcept
{
  assert(_kind == resolved_kind::enumerator);
  return *_e;
}

identifier_list& expr_id::resolved::get_param_id_list()
  const noexcept
{
  assert(_kind == resolved_kind::in_param_id_list);
  return *_pil;
}


expr_id::expr_id(const pp_token_index id_tok) noexcept
  : expr(pp_tokens_range{id_tok, id_tok + 1}), _id_tok(id_tok)
{}

expr_id::~expr_id() noexcept = default;

void expr_id::set_resolved(const resolved &r) noexcept
{
  assert(_resolved.get_kind() == resolved::resolved_kind::none);
  _resolved = r;
}

const expr_id::resolved& expr_id::get_resolved() const noexcept
{
  assert(_resolved.get_kind() != resolved::resolved_kind::none);
  return _resolved;
}

_ast_entity* expr_id::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void expr_id::_process(processor<void> &p)
{
  p(*this);
}

void expr_id::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_id::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_id::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_constant::expr_constant(const pp_token_index const_tok) noexcept
  : expr(pp_tokens_range{const_tok, const_tok + 1}), _const_tok(const_tok)
{}

expr_constant::~expr_constant() noexcept = default;

_ast_entity* expr_constant::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void expr_constant::_process(processor<void> &p)
{
  p(*this);
}

void expr_constant::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_constant::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_constant::_process(const_processor<bool> &p) const
{
  return p(*this);
}


string_literal::string_literal(const pp_token_index s)
  : ast_entity(pp_tokens_range{s, s+1}), _strings(1, s)
{}

string_literal::~string_literal() noexcept = default;

void string_literal::extend(const pp_token_index s)
{
  _strings.push_back(s);
  _extend_tokens_range(pp_tokens_range{s, s+1});
}

_ast_entity* string_literal::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void string_literal::_process(processor<void> &p)
{
  p(*this);
}

void string_literal::_process(const_processor<void> &p) const
{
  p(*this);
}

bool string_literal::_process(processor<bool> &p)
{
  return p(*this);
}

bool string_literal::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_string_literal::expr_string_literal(string_literal* &&sl) noexcept
  : expr(sl->get_tokens_range()), _sl(*mv_p(std::move(sl)))
{
  _sl._set_parent(*this);
}

expr_string_literal::~expr_string_literal() noexcept
{
  delete &_sl;
}

_ast_entity* expr_string_literal::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_sl;

  return nullptr;
}

void expr_string_literal::_process(processor<void> &p)
{
  p(*this);
}

void expr_string_literal::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_string_literal::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_string_literal::_process(const_processor<bool> &p) const
{
  return p(*this);
}


expr_parenthesized::expr_parenthesized(const pp_tokens_range &tr,
					      expr* &&e) noexcept
  : expr(tr), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

expr_parenthesized::~expr_parenthesized() noexcept
{
  delete &_e;
}

const expr& expr_parenthesized::skip_parens_down() const noexcept
{
  return _e;
}

const _ast_entity* expr_parenthesized::skip_parens_up() const noexcept
{
  const _ast_entity *cur = this->get_parent();
  const _ast_entity *result = nullptr;

  while(cur &&
	cur->process<bool, type_set<expr_parenthesized, _ast_entity> >
	(wrap_callables<default_action_unreachable<bool, type_set<> >::type>
	 ([&](const expr_parenthesized &ep) {
	    cur = ep.get_parent();
	    return true;
	  },
	  [&](const _ast_entity &ae) {
	    result = &ae;
	    return false;
	  })))
    {}

  return result;
}

_ast_entity* expr_parenthesized::skip_parens_up() noexcept
{
  _ast_entity *cur = this->get_parent();
  _ast_entity *result = nullptr;

  while(cur &&
	cur->process<bool, type_set<expr_parenthesized, _ast_entity> >
	(wrap_callables<default_action_unreachable<bool, type_set<> >::type>
	 ([&](expr_parenthesized &ep) {
	    cur = ep.get_parent();
	    return true;
	  },
	  [&](_ast_entity &ae) {
	    result = &ae;
	    return false;
	  })))
    {}

  return result;
}

_ast_entity* expr_parenthesized::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_e;

  return nullptr;
}

void expr_parenthesized::_process(processor<void> &p)
{
  p(*this);
}

void expr_parenthesized::_process(const_processor<void> &p) const
{
  p(*this);
}

bool expr_parenthesized::_process(processor<bool> &p)
{
  return p(*this);
}

bool expr_parenthesized::_process(const_processor<bool> &p) const
{
  return p(*this);
}


attribute::attribute(const pp_token_index name_tok) noexcept
  : ast_entity(pp_tokens_range{name_tok, name_tok + 1}),
    _name_tok(name_tok), _params(nullptr)
{}

attribute::attribute(const pp_tokens_range &tr, const pp_token_index name_tok,
		     expr_list* &&params) noexcept
  : ast_entity(tr), _name_tok(name_tok), _params(mv_p(std::move(params)))
{
  if (_params)
    _params->_set_parent(*this);
}

attribute::~attribute() noexcept
{
  delete _params;
}

_ast_entity* attribute::_get_child(const size_t i) const noexcept
{
  if (!i)
    return _params;

  return nullptr;
}

void attribute::_process(processor<void> &p)
{
  p(*this);
}

void attribute::_process(const_processor<void> &p) const
{
  p(*this);
}

bool attribute::_process(processor<bool> &p)
{
  return p(*this);
}

bool attribute::_process(const_processor<bool> &p) const
{
  return p(*this);
}


attribute_list::attribute_list(const pp_tokens_range &tr, attribute* &&a)
  : ast_entity(tr)
{
  attribute * const _a = mv_p(std::move(a));
  try {
    _attributes.push_back(_a);
  } catch(...) {
    delete _a;
    throw;
  }
  if (_a)
    _a->_set_parent(*this);
}

attribute_list::~attribute_list() noexcept
{
  for (auto a : _attributes)
    delete a;
}

void attribute_list::extend(const pp_tokens_range &tr, attribute* &&a)
{
  attribute * const _a = mv_p(std::move(a));
  try {
    _attributes.push_back(_a);
  } catch(...) {
    delete _a;
    throw;
  }
  if (_a)
    _a->_set_parent(*this);
  _extend_tokens_range(tr);
}

_ast_entity* attribute_list::_get_child(const size_t i) const noexcept
{
  size_t _i = 0;

  for (auto a : _attributes) {
    if (a && _i++ == i)
      return a;
  }

  return nullptr;
}

void attribute_list::_process(processor<void> &p)
{
  p(*this);
}

void attribute_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool attribute_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool attribute_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


attribute_specifier::attribute_specifier(const pp_tokens_range &tr,
					 attribute_list* &&al) noexcept
  : ast_entity(tr), _al(*mv_p(std::move(al)))
{
  _al._set_parent(*this);
}

attribute_specifier::~attribute_specifier() noexcept = default;

_ast_entity* attribute_specifier::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_al;

  return nullptr;
}

void attribute_specifier::_process(processor<void> &p)
{
  p(*this);
}

void attribute_specifier::_process(const_processor<void> &p) const
{
  p(*this);
}

bool attribute_specifier::_process(processor<bool> &p)
{
  return p(*this);
}

bool attribute_specifier::_process(const_processor<bool> &p) const
{
  return p(*this);
}


attribute_specifier_list::attribute_specifier_list(attribute_specifier* &&as)
  : ast_entity(as->get_tokens_range())
{
  auto _as = std::ref(*mv_p(std::move(as)));
  try {
    _ass.push_back(_as);
  } catch (...) {
    delete &_as.get();
    throw;
  }
  _as.get()._set_parent(*this);
}

attribute_specifier_list::~attribute_specifier_list() noexcept
{
  for (auto as : _ass)
    delete &as.get();
}

void attribute_specifier_list::extend(attribute_specifier* &&as)
{
  auto _as = std::ref(*mv_p(std::move(as)));
  try {
    _ass.push_back(_as);
  } catch (...) {
    delete &_as.get();
    throw;
  }
  _extend_tokens_range(_as.get().get_tokens_range());
  _as.get()._set_parent(*this);
}

_ast_entity* attribute_specifier_list::_get_child(const size_t i) const noexcept
{
  if (i >= _ass.size())
    return nullptr;

  return &_ass[i].get();
}

void attribute_specifier_list::_process(processor<void> &p)
{
  p(*this);
}

void attribute_specifier_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool attribute_specifier_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool attribute_specifier_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


pointer::pointer(const pp_tokens_range &tr, type_qualifier_list* &&tql)
  : ast_entity(tr)
{
  type_qualifier_list * const _tql = mv_p(std::move(tql));
  try {
    _tqls.push_back(_tql);
  } catch (...) {
    delete _tql;
    throw;
  }
  if (_tql)
    _tql->_set_parent(*this);
}

pointer::~pointer() noexcept
{
  for (auto tql : _tqls)
    delete tql;
}

void pointer::extend(const pp_tokens_range &tr, type_qualifier_list* &&tql)
{
  type_qualifier_list * const _tql = mv_p(std::move(tql));
  try {
    _tqls.push_back(_tql);
  } catch (...) {
    delete _tql;
    throw;
  }
  _extend_tokens_range(tr);
  if (_tql)
    _tql->_set_parent(*this);
}

_ast_entity* pointer::_get_child(const size_t i) const noexcept
{
  std::size_t _i = 0;

  for (auto tq : _tqls) {
    if (tq && _i++ == i)
      return tq;
  }

  return nullptr;
}

void pointer::_process(processor<void> &p)
{
  p(*this);
}

void pointer::_process(const_processor<void> &p) const
{
  p(*this);
}

bool pointer::_process(processor<bool> &p)
{
  return p(*this);
}

bool pointer::_process(const_processor<bool> &p) const
{
  return p(*this);
}


direct_abstract_declarator::
direct_abstract_declarator(const pp_tokens_range &tr) noexcept
  : typed_ast_entity(tr)
{}

direct_abstract_declarator::~direct_abstract_declarator() noexcept = default;

std::shared_ptr<const klp::ccp::types::addressable_type>
direct_abstract_declarator::_get_enclosing_type() const noexcept
{
  std::shared_ptr<const types::addressable_type> pt;

  process_parent
    (wrap_callables<no_default_action>
     ([&pt](const direct_abstract_declarator &dad) {
	pt = dad.get_type();
      },
      [&pt](const abstract_declarator &ad) {
	pt = ad.get_type();
      }));
  return pt;
}


direct_abstract_declarator_parenthesized::
direct_abstract_declarator_parenthesized(const pp_tokens_range &tr,
					 attribute_specifier_list* &&asl,
					 abstract_declarator* &&ad) noexcept
  : direct_abstract_declarator(tr), _asl(mv_p(std::move(asl))),
    _ad(*mv_p(std::move(ad)))
{
  if (_asl)
    _asl->_set_parent(*this);
  _ad._set_parent(*this);
}

direct_abstract_declarator_parenthesized::
~direct_abstract_declarator_parenthesized() noexcept
{
  delete _asl;
  delete &_ad;
}

std::shared_ptr<const klp::ccp::types::addressable_type>
direct_abstract_declarator_parenthesized::
get_innermost_type() const noexcept
{
  return _ad.get_innermost_type();
}

_ast_entity*
direct_abstract_declarator_parenthesized::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_asl)
      return _asl;
    else
      return &_ad;
  } else if (i == 1 && _asl) {
    return &_ad;
  }

  return nullptr;
}

void direct_abstract_declarator_parenthesized::_process(processor<void> &p)
{
  p(*this);
}

void direct_abstract_declarator_parenthesized::_process(const_processor<void> &p) const
{
  p(*this);
}

bool direct_abstract_declarator_parenthesized::_process(processor<bool> &p)
{
  return p(*this);
}

bool direct_abstract_declarator_parenthesized::_process(const_processor<bool> &p) const
{
  return p(*this);
}


direct_abstract_declarator_array::
direct_abstract_declarator_array(const pp_tokens_range &tr,
				 direct_abstract_declarator* &&dad,
				 type_qualifier_list* &&tql,
				 expr* &&size,
				 const bool is_static) noexcept
  : direct_abstract_declarator(tr), _dad(mv_p(std::move(dad))),
    _tql(mv_p(std::move(tql))), _size(mv_p(std::move(size))),
    _static(is_static), _vla_unspec_size(false)
{
  assert(!_static || _size);
  if (_dad)
    _dad->_set_parent(*this);
  if (_tql)
    _tql->_set_parent(*this);
  if (_size)
    _size->_set_parent(*this);
}

direct_abstract_declarator_array::
direct_abstract_declarator_array(const pp_tokens_range &tr,
				 direct_abstract_declarator* &&dad,
				 const vla_unspec_size_tag&) noexcept
  : direct_abstract_declarator(tr), _dad(mv_p(std::move(dad))),
    _tql(nullptr), _size(nullptr), _static(false), _vla_unspec_size(true)
{
  if (_dad)
    _dad->_set_parent(*this);
}

direct_abstract_declarator_array::
~direct_abstract_declarator_array() noexcept
{
  delete _dad;
  delete _tql;
  delete _size;
}

std::shared_ptr<const klp::ccp::types::addressable_type>
direct_abstract_declarator_array::get_innermost_type() const noexcept
{
  if (_dad)
    return _dad->get_innermost_type();
  return get_type();
}

_ast_entity* direct_abstract_declarator_array::_get_child(const size_t i)
  const noexcept
{
  std::size_t _i = i;

  if (_dad) {
    if (!_i)
      return _dad;

    --_i;
  }

  if (_tql) {
    if (!_i)
      return _tql;

    --_i;
  }

  if (_size) {
    if (!_i)
      return _size;

    --_i;
  }

  return nullptr;
}

void direct_abstract_declarator_array::_process(processor<void> &p)
{
  p(*this);
}

void direct_abstract_declarator_array::_process(const_processor<void> &p) const
{
  p(*this);
}

bool direct_abstract_declarator_array::_process(processor<bool> &p)
{
  return p(*this);
}

bool direct_abstract_declarator_array::_process(const_processor<bool> &p) const
{
  return p(*this);
}


direct_abstract_declarator_func::
direct_abstract_declarator_func(const pp_tokens_range &tr,
			       direct_abstract_declarator* &&dad,
			       parameter_declaration_list* &&ptl) noexcept
  : direct_abstract_declarator(tr),
    _dad(mv_p(std::move(dad))), _ptl(mv_p(std::move(ptl)))
{
  if (_dad)
    _dad->_set_parent(*this);
  if (_ptl)
    _ptl->_set_parent(*this);
}

direct_abstract_declarator_func::~direct_abstract_declarator_func() noexcept
{
  delete _dad;
  delete _ptl;
}

std::shared_ptr<const klp::ccp::types::addressable_type>
direct_abstract_declarator_func::get_innermost_type() const noexcept
{
  if (_dad)
    return _dad->get_innermost_type();
  return get_type();
}

_ast_entity* direct_abstract_declarator_func::_get_child(const size_t i)
  const noexcept
{
  if (!i) {
    if (_dad)
      return _dad;
    else
      return _ptl;
  } else if (i == 1 && _dad) {
    return _ptl;
  }

  return nullptr;
}

void direct_abstract_declarator_func::_process(processor<void> &p)
{
  p(*this);
}

void direct_abstract_declarator_func::_process(const_processor<void> &p) const
{
  p(*this);
}

bool direct_abstract_declarator_func::_process(processor<bool> &p)
{
  return p(*this);
}

bool direct_abstract_declarator_func::_process(const_processor<bool> &p) const
{
  return p(*this);
}


abstract_declarator::abstract_declarator(const pp_tokens_range &tr,
					 pointer* &&pt,
					 direct_abstract_declarator* &&dad)
noexcept
  : typed_ast_entity(tr), _pt(mv_p(std::move(pt))), _dad(mv_p(std::move(dad)))
{
  if (_pt)
    _pt->_set_parent(*this);
  if (_dad)
    _dad->_set_parent(*this);
}

abstract_declarator::~abstract_declarator() noexcept
{
  delete _pt;
  delete _dad;
}

std::shared_ptr<const klp::ccp::types::addressable_type>
abstract_declarator::
get_innermost_type() const noexcept
{
  if (_dad)
    return _dad->get_innermost_type();

  return get_type();
}

_ast_entity* abstract_declarator::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_pt)
      return _pt;
    else
      return _dad;
  } else if (i == 1 && _pt) {
    return _dad;
  }

  return nullptr;
}

void abstract_declarator::_process(processor<void> &p)
{
  p(*this);
}

void abstract_declarator::_process(const_processor<void> &p) const
{
  p(*this);
}

bool abstract_declarator::_process(processor<bool> &p)
{
  return p(*this);
}

bool abstract_declarator::_process(const_processor<bool> &p) const
{
  return p(*this);
}

std::shared_ptr<const klp::ccp::types::addressable_type>
abstract_declarator::_get_enclosing_type() const noexcept
{
  std::shared_ptr<const types::addressable_type> pt;

  process_parent
    (wrap_callables<no_default_action>
     ([&pt](const direct_abstract_declarator_parenthesized &dadp) {
	pt = dadp.get_type();
	assert(pt);
      },
      [&pt](const type_name &tn) {
	pt = tn.get_specifier_qualifier_list().get_type();
	assert(pt);
      },
      [&pt](const parameter_declaration_abstract &pda) {
	pt = pda.get_declaration_specifiers().get_type();
	assert(pt);
      }));

  return pt;
}


type_name::type_name(const pp_tokens_range &tr, specifier_qualifier_list* &&sql,
		     abstract_declarator* &&ad) noexcept
  : typed_ast_entity(tr), _sql(*mv_p(std::move(sql))), _ad(mv_p(std::move(ad)))
{
  _sql._set_parent(*this);
  if (_ad)
    _ad->_set_parent(*this);
}

type_name::~type_name() noexcept
{
  delete &_sql;
  delete _ad;
}

_ast_entity* type_name::_get_child(const size_t i) const noexcept
{
  if (!i) {
    return &_sql;
  } else if (i == 1) {
    return _ad;
  }

  return nullptr;
}

void type_name::_process(processor<void> &p)
{
  p(*this);
}

void type_name::_process(const_processor<void> &p) const
{
  p(*this);
}

bool type_name::_process(processor<bool> &p)
{
  return p(*this);
}

bool type_name::_process(const_processor<bool> &p) const
{
  return p(*this);
}


direct_declarator::direct_declarator(const pp_tokens_range &tr) noexcept
  : typed_ast_entity(tr)
{}

direct_declarator::~direct_declarator() noexcept = default;

std::shared_ptr<const klp::ccp::types::addressable_type>
direct_declarator::_get_enclosing_type() const noexcept
{
  std::shared_ptr<const types::addressable_type> pt;

  process_parent
    (wrap_callables<no_default_action>
     ([&pt](const direct_declarator& dd) {
	pt = dd.get_type();
      },
      [&pt](const declarator& d) {
	// If in an __auto_type declaration, the type will not be
	// available until after the initializer has been evaluated.
	if (!d.is_evaluated())
	  return;

	pt = d.get_type();
      }));
  return pt;
}


direct_declarator_id::direct_declarator_id(const pp_token_index id_tok) noexcept
  : direct_declarator(pp_tokens_range{id_tok, id_tok + 1}), _id_tok(id_tok)
{}

direct_declarator_id::~direct_declarator_id() noexcept = default;

const direct_declarator_id& direct_declarator_id::get_direct_declarator_id()
  const noexcept
{
  return *this;
}

direct_declarator_id& direct_declarator_id::get_direct_declarator_id() noexcept
{
  return *this;
}

const direct_declarator_func* direct_declarator_id::is_function() const noexcept
{
  const direct_declarator_func *ddf = nullptr;
  for_each_ancestor<type_set<direct_declarator_func,
			     direct_declarator_array,
			     struct_declarator,
			     init_declarator,
			     parameter_declaration_declarator,
			     function_definition> >
    (wrap_callables<no_default_action>
     ([](const declarator &d) {
	if (d.get_pointer()) {
	  // Will be a "pointer to ... function returning ..." at best.
	  return false;
	}
	return true;
      },
      [](const direct_declarator_parenthesized&) {
	return true;
      },
      [&](const direct_declarator_func &_ddf) {
	ddf = &_ddf;
      },
      [](const direct_declarator_array&) {},
      [](const struct_declarator&) {},
      [](const init_declarator&) {},
      [](const parameter_declaration_declarator&) {},
      [](const function_definition&) {}));

  return ddf;
}

void direct_declarator_id::
complete_type(std::shared_ptr<const types::array_type> &&at)
{
  _reset_type(std::move(at));
}

_ast_entity* direct_declarator_id::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void direct_declarator_id::_process(processor<void> &p)
{
  p(*this);
}

void direct_declarator_id::_process(const_processor<void> &p) const
{
  p(*this);
}

bool direct_declarator_id::_process(processor<bool> &p)
{
  return p(*this);
}

bool direct_declarator_id::_process(const_processor<bool> &p) const
{
  return p(*this);
}


direct_declarator_parenthesized::
direct_declarator_parenthesized(const pp_tokens_range &tr,
				declarator* &&d,
				attribute_specifier_list* &&asl)
noexcept
  : direct_declarator(tr), _d(*mv_p(std::move(d))),
    _asl(mv_p(std::move(asl)))
{
  _d._set_parent(*this);
  if (_asl)
    _asl->_set_parent(*this);
}

direct_declarator_parenthesized::~direct_declarator_parenthesized() noexcept
{
  delete &_d;
  delete _asl;
}

const direct_declarator_id& direct_declarator_parenthesized::
get_direct_declarator_id() const noexcept
{
  return _d.get_direct_declarator_id();
}

direct_declarator_id& direct_declarator_parenthesized::
get_direct_declarator_id() noexcept
{
  return _d.get_direct_declarator_id();
}

_ast_entity* direct_declarator_parenthesized::_get_child(const size_t i)
  const noexcept
{
  if (!i) {
    return &_d;
  } else if (i == 1) {
    return _asl;
  }

  return nullptr;
}

void direct_declarator_parenthesized::_process(processor<void> &p)
{
  p(*this);
}

void direct_declarator_parenthesized::_process(const_processor<void> &p) const
{
  p(*this);
}

bool direct_declarator_parenthesized::_process(processor<bool> &p)
{
  return p(*this);
}

bool direct_declarator_parenthesized::_process(const_processor<bool> &p) const
{
  return p(*this);
}


direct_declarator_array::direct_declarator_array(const pp_tokens_range &tr,
				direct_declarator* &&dd,
				type_qualifier_list* &&tql,
				expr* &&size,
				const bool is_static) noexcept
  : direct_declarator(tr), _dd(*mv_p(std::move(dd))),
    _tql(mv_p(std::move(tql))), _size(mv_p(std::move(size))),
    _static(is_static), _vla_unspec_size(false)
{
  assert(!_static || _size);
  _dd._set_parent(*this);
  if (_tql)
    _tql->_set_parent(*this);
  if (_size)
    _size->_set_parent(*this);
}

direct_declarator_array::direct_declarator_array(const pp_tokens_range &tr,
				direct_declarator* &&dd,
				type_qualifier_list* &&tql,
				const vla_unspec_size_tag&) noexcept
  : direct_declarator(tr), _dd(*mv_p(std::move(dd))),
    _tql(mv_p(std::move(tql))), _size(nullptr),
    _static(false), _vla_unspec_size(true)
{
  _dd._set_parent(*this);
  if (_tql)
    _tql->_set_parent(*this);
}

direct_declarator_array::~direct_declarator_array() noexcept
{
  delete &_dd;
  delete _tql;
  delete _size;
}

const direct_declarator_id& direct_declarator_array::get_direct_declarator_id()
  const noexcept
{
  return _dd.get_direct_declarator_id();
}

direct_declarator_id& direct_declarator_array::get_direct_declarator_id()
  noexcept
{
  return _dd.get_direct_declarator_id();
}

_ast_entity* direct_declarator_array::_get_child(const size_t i) const noexcept
{
  if (!i) {
    return &_dd;
  } else if (i == 1) {
    if (_tql)
      return _tql;
    else
      return _size;
  } else if (i == 2 && _tql) {
    return _size;
  }

  return nullptr;
}

void direct_declarator_array::_process(processor<void> &p)
{
  p(*this);
}

void direct_declarator_array::_process(const_processor<void> &p) const
{
  p(*this);
}

bool direct_declarator_array::_process(processor<bool> &p)
{
  return p(*this);
}

bool direct_declarator_array::_process(const_processor<bool> &p) const
{
  return p(*this);
}


direct_declarator_func::direct_declarator_func(const pp_tokens_range &tr,
					     direct_declarator* &&dd,
					     parameter_declaration_list* &&ptl)
noexcept
  : direct_declarator(tr), _dd(*mv_p(std::move(dd))),
    _ptl(mv_p(std::move(ptl))), _il(nullptr)
{
  _dd._set_parent(*this);
  assert(_ptl);
  _ptl->_set_parent(*this);
}

direct_declarator_func::direct_declarator_func(const pp_tokens_range &tr,
					       direct_declarator* &&dd,
					       identifier_list* &&il)
noexcept
  : direct_declarator(tr), _dd(*mv_p(std::move(dd))),
    _ptl(nullptr), _il(mv_p(std::move(il)))
{
  _dd._set_parent(*this);
  if (_il)
  _il->_set_parent(*this);
}

direct_declarator_func::~direct_declarator_func() noexcept
{
  delete &_dd;
  delete _ptl;
  delete _il;
}

const direct_declarator_id& direct_declarator_func::get_direct_declarator_id()
  const noexcept
{
  return _dd.get_direct_declarator_id();
}

direct_declarator_id& direct_declarator_func::get_direct_declarator_id()
  noexcept
{
  return _dd.get_direct_declarator_id();
}

_ast_entity* direct_declarator_func::_get_child(const size_t i) const noexcept
{
  if (!i) {
    return &_dd;
  } else if (i == 1) {
    if (_ptl)
      return _ptl;
    else
      return _il;
  } else if (i == 2 && _ptl) {
    return _il;
  }

  return nullptr;
}

void direct_declarator_func::_process(processor<void> &p)
{
  p(*this);
}

void direct_declarator_func::_process(const_processor<void> &p) const
{
  p(*this);
}

bool direct_declarator_func::_process(processor<bool> &p)
{
  return p(*this);
}

bool direct_declarator_func::_process(const_processor<bool> &p) const
{
  return p(*this);
}


declarator::declarator(const pp_tokens_range &tr, pointer* &&pt,
		       direct_declarator* &&dd) noexcept
  : typed_ast_entity(tr), _pt(mv_p(std::move(pt))),
    _dd(*mv_p(std::move(dd))), _cached_ddid(_dd.get_direct_declarator_id())
{
  _dd._set_parent(*this);
  if (_pt)
    _pt->_set_parent(*this);
}

declarator::~declarator() noexcept
{
  delete _pt;
  delete &_dd;
}

klp::ccp::pp_token_index declarator::get_id_tok() const noexcept
{
  return this->get_direct_declarator_id().get_id_tok();
}

std::shared_ptr<const klp::ccp::types::addressable_type>
declarator::get_innermost_type() const noexcept
{
  return get_direct_declarator_id().get_type();
}

_ast_entity* declarator::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_pt)
      return _pt;
    else
      return &_dd;
  } else if (i == 1 && _pt) {
    return &_dd;
  }

  return nullptr;
}

void declarator::_process(processor<void> &p)
{
  p(*this);
}

void declarator::_process(const_processor<void> &p) const
{
  p(*this);
}

bool declarator::_process(processor<bool> &p)
{
  return p(*this);
}

bool declarator::_process(const_processor<bool> &p) const
{
  return p(*this);
}

std::shared_ptr<const klp::ccp::types::addressable_type>
declarator::_get_enclosing_type() const noexcept
{
  std::shared_ptr<const types::addressable_type> pt;

  process_parent
    (wrap_callables<no_default_action>
     ([&pt](const direct_declarator_parenthesized &ddp) {
	pt = ddp.get_type();
	assert(pt);
      },
      [&pt](const struct_declarator &sd) {
	const specifier_qualifier_list *sql =
	  (sd.get_unique_parent<struct_declarator_list>()
	   .get_unique_parent<struct_declaration_c99>()
	   .get_specifier_qualifier_list());
	pt = sql->get_type();
	assert(pt);
      },
      [&pt](const init_declarator &id) {
	const declaration &d = id.get_containing_declaration();
	const declaration_specifiers &ds = d.get_declaration_specifiers();
	assert(ds.is_evaluated() || ds.is_auto_type());
	if (!ds.is_evaluated())
	  return;
	pt = d.get_declaration_specifiers().get_type();
	assert(pt);
      },
      [&pt](const parameter_declaration_declarator &pdd) {
	pt = pdd.get_declaration_specifiers().get_type();
	assert(pt);
      },
      [&pt](const function_definition &fd) {
	pt = fd.get_declaration_specifiers().get_type();
	assert(pt);
      }));

  return pt;
}

void declarator::
_set_deduced_auto_type(std::shared_ptr<const types::addressable_type> &&t)
  noexcept
{
  _set_type(std::move(t));
}


storage_class_specifier::
storage_class_specifier(const pp_tokens_range &tr,
			const storage_class sc) noexcept
  : ast_entity(tr), _sc(sc)
{}

storage_class_specifier::~storage_class_specifier() noexcept = default;

_ast_entity* storage_class_specifier::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void storage_class_specifier::_process(processor<void> &p)
{
  p(*this);
}

void storage_class_specifier::_process(const_processor<void> &p) const
{
  p(*this);
}

bool storage_class_specifier::_process(processor<bool> &p)
{
  return p(*this);
}

bool storage_class_specifier::_process(const_processor<bool> &p) const
{
  return p(*this);
}


type_qualifier::type_qualifier(const pp_tokens_range &tr,
			       const types::qualifier q) noexcept
  : ast_entity(tr), _q(q)
{}

type_qualifier::~type_qualifier() noexcept = default;

_ast_entity* type_qualifier::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void type_qualifier::_process(processor<void> &p)
{
  p(*this);
}

void type_qualifier::_process(const_processor<void> &p) const
{
  p(*this);
}

bool type_qualifier::_process(processor<bool> &p)
{
  return p(*this);
}

bool type_qualifier::_process(const_processor<bool> &p) const
{
  return p(*this);
}


type_qualifier_list::type_qualifier_list(type_qualifier* &&tq)
  : ast_entity(tq->get_tokens_range())
{
  auto _tq = std::ref(*mv_p(std::move(tq)));
  try {
    _tqs.push_back(_tq);
  } catch(...) {
    delete &_tq.get();
    throw;
  }
  _tq.get()._set_parent(*this);
}

type_qualifier_list::type_qualifier_list(attribute_specifier_list* &&asl)
  : ast_entity(asl->get_tokens_range())
{
  auto _asl = std::ref(*mv_p(std::move(asl)));
  try {
    _asls.push_back(_asl);
  } catch(...) {
    delete &_asl.get();
    throw;
  }
  _asl.get()._set_parent(*this);
}


type_qualifier_list::~type_qualifier_list() noexcept
{
  for (auto tq : _tqs)
    delete &tq.get();

  for (auto asl : _asls)
    delete &asl.get();
}

void type_qualifier_list::extend(type_qualifier* &&tq)
{
  auto _tq = std::ref(*mv_p(std::move(tq)));
  try {
    _tqs.push_back(_tq);
  } catch(...) {
    delete &_tq.get();
    throw;
  }
  _extend_tokens_range(_tq.get().get_tokens_range());
  _tq.get()._set_parent(*this);
}

void type_qualifier_list::extend(attribute_specifier_list* &&asl)
{
  auto _asl = std::ref(*mv_p(std::move(asl)));
  try {
    _asls.push_back(_asl);
  } catch(...) {
    delete &_asl.get();
    throw;
  }
  _extend_tokens_range(_asl.get().get_tokens_range());
  _asl.get()._set_parent(*this);
}

const klp::ccp::types::qualifiers
type_qualifier_list::get_qualifiers() const
{
  types::qualifiers qs;
  for (auto q : _tqs)
    qs.add(q.get().get_kind());

  return qs;
}

_ast_entity* type_qualifier_list::_get_child(const size_t i) const noexcept
{
  if (i < _tqs.size()) {
    return &_tqs[i].get();
  } else if (i < _tqs.size() + _asls.size()) {
    return &_asls[i - _tqs.size()].get();
  }

  return nullptr;
}

void type_qualifier_list::_process(processor<void> &p)
{
  p(*this);
}

void type_qualifier_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool type_qualifier_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool type_qualifier_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


type_specifier::type_specifier(const pp_tokens_range &tr) noexcept
  : ast_entity(tr)
{}

type_specifier::~type_specifier() noexcept = default;


type_specifier_pod::type_specifier_pod(const pp_tokens_range &tr,
				       const pod_spec &pod_spec) noexcept
  : type_specifier(tr), _pod_spec(pod_spec)
{}

type_specifier_pod::~type_specifier_pod() noexcept = default;

_ast_entity* type_specifier_pod::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void type_specifier_pod::_process(processor<void> &p)
{
  p(*this);
}

void type_specifier_pod::_process(const_processor<void> &p) const
{
  p(*this);
}

bool type_specifier_pod::_process(processor<bool> &p)
{
  return p(*this);
}

bool type_specifier_pod::_process(const_processor<bool> &p) const
{
  return p(*this);
}


type_specifier_ext_int::
type_specifier_ext_int(const pp_tokens_range &tr,
		       const types::ext_int_type::kind &k) noexcept
  : type_specifier(tr), _k(k)
{}

type_specifier_ext_int::~type_specifier_ext_int() noexcept = default;

_ast_entity* type_specifier_ext_int::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void type_specifier_ext_int::_process(processor<void> &p)
{
  p(*this);
}

void type_specifier_ext_int::_process(const_processor<void> &p) const
{
  p(*this);
}

bool type_specifier_ext_int::_process(processor<bool> &p)
{
  return p(*this);
}

bool type_specifier_ext_int::_process(const_processor<bool> &p) const
{
  return p(*this);
}


type_specifier_ext_float::
type_specifier_ext_float(const pp_tokens_range &tr,
			 const types::ext_float_type::kind &k) noexcept
  : type_specifier(tr), _k(k)
{}

type_specifier_ext_float::~type_specifier_ext_float() noexcept = default;

_ast_entity* type_specifier_ext_float::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void type_specifier_ext_float::_process(processor<void> &p)
{
  p(*this);
}

void type_specifier_ext_float::_process(const_processor<void> &p) const
{
  p(*this);
}

bool type_specifier_ext_float::_process(processor<bool> &p)
{
  return p(*this);
}

bool type_specifier_ext_float::_process(const_processor<bool> &p) const
{
  return p(*this);
}


type_specifier_tdid::resolved::resolved() noexcept
  : _kind(resolved_kind::none)
{}

type_specifier_tdid::resolved::
resolved(const builtin_typedef::factory &fac) noexcept
  : _kind(resolved_kind::builtin_typedef), _builtin_typedef_fac(&fac)
{}

type_specifier_tdid::resolved::
resolved(const init_declarator &id) noexcept
  : _kind(resolved_kind::init_declarator), _id(&id)
{}

const klp::ccp::builtin_typedef::factory& type_specifier_tdid::resolved::
get_builtin_typedef_factory() const noexcept
{
  assert(_kind == resolved_kind::builtin_typedef);
  return *_builtin_typedef_fac;
}

const init_declarator& type_specifier_tdid::resolved::
get_init_declarator() const noexcept
{
  assert(_kind == resolved_kind::init_declarator);
  return *_id;
}

type_specifier_tdid::type_specifier_tdid(const pp_token_index tdid_tok) noexcept
  : type_specifier(pp_tokens_range{tdid_tok, tdid_tok + 1}),
    _tdid_tok(tdid_tok)
{}

type_specifier_tdid::~type_specifier_tdid() noexcept = default;

void type_specifier_tdid::set_resolved(const resolved &r) noexcept
{
  assert(_resolved.get_kind() == resolved::resolved_kind::none);
  _resolved = r;
}

const type_specifier_tdid::resolved& type_specifier_tdid::get_resolved()
  const noexcept
{
  assert(_resolved.get_kind() != resolved::resolved_kind::none);
  return _resolved;
}

_ast_entity* type_specifier_tdid::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void type_specifier_tdid::_process(processor<void> &p)
{
  p(*this);
}

void type_specifier_tdid::_process(const_processor<void> &p) const
{
  p(*this);
}

bool type_specifier_tdid::_process(processor<bool> &p)
{
  return p(*this);
}

bool type_specifier_tdid::_process(const_processor<bool> &p) const
{
  return p(*this);
}


type_specifier_auto_type::
type_specifier_auto_type(const pp_token_index auto_type_tok) noexcept
  : type_specifier(pp_tokens_range{auto_type_tok, auto_type_tok + 1})
{}

type_specifier_auto_type::~type_specifier_auto_type() noexcept = default;

_ast_entity* type_specifier_auto_type::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void type_specifier_auto_type::_process(processor<void> &p)
{
  p(*this);
}
void type_specifier_auto_type::_process(const_processor<void> &p) const
{
  p(*this);
}

bool type_specifier_auto_type::_process(processor<bool> &p)
{
  return p(*this);
}
bool type_specifier_auto_type::_process(const_processor<bool> &p) const
{
  return p(*this);
}


struct_declaration::struct_declaration(const pp_tokens_range &tr) noexcept
  : ast_entity{tr}
{}

struct_declaration::~struct_declaration() noexcept = default;


struct_declaration_decl::
struct_declaration_decl(const pp_tokens_range &tr,
			specifier_qualifier_list* &&sql) noexcept
  : struct_declaration{tr}, _sql{mv_p(std::move(sql))}
{}

struct_declaration_decl::~struct_declaration_decl() noexcept
{
  delete _sql;
}


struct_declarator::struct_declarator(const pp_tokens_range &tr,
				     declarator* &&d, expr* &&width,
				     attribute_specifier_list* &&asl_after)
  noexcept
  : ast_entity(tr), _d(mv_p(std::move(d))),
    _width(mv_p(std::move(width))),
    _asl_before(nullptr), _asl_after(mv_p(std::move(asl_after))),
    _is_last(false)
{
  if (_d)
    _d->_set_parent(*this);
  if (_width)
    _width->_set_parent(*this);
  if (_asl_after)
    _asl_after->_set_parent(*this);
}

struct_declarator::~struct_declarator() noexcept
{
  delete _d;
  delete _width;
  delete _asl_before;
  delete _asl_after;
}

void struct_declarator::set_asl_before(attribute_specifier_list* &&asl_before)
  noexcept
{
  assert(!_asl_before);
  _asl_before = mv_p(std::move(asl_before));
  if (_asl_before)
    _asl_before->_set_parent(*this);
}

void struct_declarator::mark_last_member() noexcept
{
  _is_last = true;
}

_ast_entity* struct_declarator::_get_child(const size_t i) const noexcept
{
  size_t _i = i;

  if (_d) {
    if (!_i)
      return _d;
    --_i;
  }

  if (_width) {
    if (!_i)
      return _width;
    --_i;
  }

  if (_asl_before) {
    if (!_i)
      return _asl_before;
    --_i;
  }

  if (_asl_after) {
    if (!_i)
      return _asl_after;
    --_i;
  }

  return nullptr;
}

void struct_declarator::_process(processor<void> &p)
{
  p(*this);
}

void struct_declarator::_process(const_processor<void> &p) const
{
  p(*this);
}

bool struct_declarator::_process(processor<bool> &p)
{
  return p(*this);
}

bool struct_declarator::_process(const_processor<bool> &p) const
{
  return p(*this);
}


struct_declarator_list::struct_declarator_list(struct_declarator* &&sd)
  : ast_entity(sd->get_tokens_range())
{
  auto _sd = std::ref(*mv_p(std::move(sd)));
  try {
    _sds.push_back(_sd);
  } catch (...) {
    delete &_sd.get();
    throw;
  }
  _sd.get()._set_parent(*this);
}

struct_declarator_list::~struct_declarator_list() noexcept
{
  for (auto sd : _sds)
    delete &sd.get();
}

void struct_declarator_list::extend(struct_declarator* &&sd)
{
  auto _sd = std::ref(*mv_p(std::move(sd)));
  try {
    _sds.push_back(_sd);
  } catch (...) {
    delete &_sd.get();
    throw;
  }
  _extend_tokens_range(_sd.get().get_tokens_range());
  _sd.get()._set_parent(*this);
}

bool struct_declarator_list::empty() const noexcept
{
  return _sds.empty();
}

void struct_declarator_list::mark_last_member() noexcept
{
  if (_sds.empty())
    return;

  _sds.back().get().mark_last_member();
}

_ast_entity* struct_declarator_list::_get_child(const size_t i) const noexcept
{
  if (i >= _sds.size())
    return nullptr;

  return &_sds[i].get();
}

void struct_declarator_list::_process(processor<void> &p)
{
  p(*this);
}

void struct_declarator_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool struct_declarator_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool struct_declarator_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


struct_declaration_c99::struct_declaration_c99(const pp_tokens_range &tr,
					       specifier_qualifier_list* &&sql,
					       struct_declarator_list * &&sdl)
  noexcept
  : struct_declaration_decl(tr, std::move(sql)), _sdl(*mv_p(std::move(sdl)))
{
  if (get_specifier_qualifier_list())
    get_specifier_qualifier_list()->_set_parent(*this);
  _sdl._set_parent(*this);
}

struct_declaration_c99::~struct_declaration_c99() noexcept
{
  delete &_sdl;
}

void struct_declaration_c99::mark_last_member() noexcept
{
  _sdl.mark_last_member();
}

_ast_entity* struct_declaration_c99::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_sql)
      return _sql;
    else
      return &_sdl;
  } else if (i == 1 && _sql) {
    return &_sdl;
  }

  return nullptr;
}

void struct_declaration_c99::_process(processor<void> &p)
{
  p(*this);
}

void struct_declaration_c99::_process(const_processor<void> &p) const
{
  p(*this);
}

bool struct_declaration_c99::_process(processor<bool> &p)
{
  return p(*this);
}

bool struct_declaration_c99::_process(const_processor<bool> &p) const
{
  return p(*this);
}


unnamed_struct_or_union::
unnamed_struct_or_union(const pp_tokens_range &tr,
			const types::struct_or_union_kind souk,
			struct_declaration_list* &&sdl,
			attribute_specifier_list* &&asl_before,
			attribute_specifier_list* &&asl_after) noexcept
  : ast_entity(tr), _souk(souk), _sdl(mv_p(std::move(sdl))),
    _asl_before(mv_p(std::move(asl_before))),
    _asl_after(mv_p(std::move(asl_after)))
{
  if (_sdl)
    _sdl->_set_parent(*this);
  if (_asl_before)
    _asl_before->_set_parent(*this);
  if (_asl_after)
    _asl_after->_set_parent(*this);
}

unnamed_struct_or_union::~unnamed_struct_or_union() noexcept
{
  delete _sdl;
  delete _asl_before;
  delete _asl_after;
}

const klp::ccp::types::struct_or_union_content&
unnamed_struct_or_union::get_content() const noexcept
{
  assert(_content);
  return *_content;
}

void unnamed_struct_or_union::
set_content(std::unique_ptr<const types::struct_or_union_content> &&c) noexcept
{
  assert(!_content);
  _content = std::move(c);
}

_ast_entity* unnamed_struct_or_union::_get_child(const size_t i) const noexcept
{
  std::size_t _i = i;

  if (_sdl) {
    if (!_i)
      return _sdl;

    --_i;
  }

  if (_asl_before) {
    if (!_i)
      return _asl_before;

    --_i;
  }

  if (_asl_after) {
    if (!_i)
      return _asl_after;

    --_i;
  }

  return nullptr;
}

void unnamed_struct_or_union::_process(processor<void> &p)
{
  p(*this);
}

void unnamed_struct_or_union::_process(const_processor<void> &p) const
{
  p(*this);
}

bool unnamed_struct_or_union::_process(processor<bool> &p)
{
  return p(*this);
}

bool unnamed_struct_or_union::_process(const_processor<bool> &p) const
{
  return p(*this);
}


struct_declaration_unnamed_sou::
struct_declaration_unnamed_sou(const pp_tokens_range &tr,
			       specifier_qualifier_list* &&sql,
			       unnamed_struct_or_union* &&unnamed_sou)
  noexcept
  : struct_declaration_decl(tr, std::move(sql)),
    _unnamed_sou(*mv_p(std::move(unnamed_sou)))
{
  if (get_specifier_qualifier_list())
    get_specifier_qualifier_list()->_set_parent(*this);
  _unnamed_sou._set_parent(*this);
}

struct_declaration_unnamed_sou::~struct_declaration_unnamed_sou() noexcept
{
  delete &_unnamed_sou;
}

void struct_declaration_unnamed_sou::mark_last_member() noexcept
{
  // The ->is_last() information is only relevant for array members of
  // unspecified length. Do nothing.
}

_ast_entity* struct_declaration_unnamed_sou::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_sql)
      return _sql;
    else
      return &_unnamed_sou;
  } else if (i == 1 && _sql) {
    return &_unnamed_sou;
  }

  return nullptr;
}

void struct_declaration_unnamed_sou::_process(processor<void> &p)
{
  p(*this);
}

void struct_declaration_unnamed_sou::_process(const_processor<void> &p) const
{
  p(*this);
}

bool struct_declaration_unnamed_sou::_process(processor<bool> &p)
{
  return p(*this);
}

bool struct_declaration_unnamed_sou::_process(const_processor<bool> &p) const
{
  return p(*this);
}


struct_declaration_static_assert::
struct_declaration_static_assert(static_assertion* &&sa) noexcept
  : struct_declaration{sa->get_tokens_range()}, _sa{*mv_p(std::move(sa))}
{
  _sa._set_parent(*this);
}

struct_declaration_static_assert::~struct_declaration_static_assert() noexcept
{
  delete &_sa;
}

void struct_declaration_static_assert::mark_last_member() noexcept
{
  // The ->is_last() information is only relevant for array members of
  // unspecified length. Do nothing.
}

_ast_entity* struct_declaration_static_assert::
_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_sa;

  return nullptr;
}


void struct_declaration_static_assert::
_process(processor<void> &p)
{
  p(*this);
}

void struct_declaration_static_assert::
_process(const_processor<void> &p) const
{
  p(*this);
}

bool struct_declaration_static_assert::
_process(processor<bool> &p)
{
  return p(*this);
}

bool struct_declaration_static_assert::
_process(const_processor<bool> &p) const
{
  return p(*this);
}


struct_declaration_list::struct_declaration_list(struct_declaration* &&sd)
  : ast_entity(sd->get_tokens_range())
{
  auto _sd = std::ref(*mv_p(std::move(sd)));
  try {
    _sds.push_back(_sd);
  } catch (...) {
    delete &_sd.get();
    throw;
  }
  _sd.get()._set_parent(*this);
}

struct_declaration_list::~struct_declaration_list() noexcept
{
  for (auto sd : _sds)
    delete &sd.get();
}

void struct_declaration_list::extend(struct_declaration* &&sd)
{
  auto _sd = std::ref(*mv_p(std::move(sd)));
  try {
    _sds.push_back(_sd);
  } catch (...) {
    delete &_sd.get();
    throw;
  }
  _sd.get()._set_parent(*this);
}

void struct_declaration_list::mark_last_member() noexcept
{
  if (_sds.empty())
    return;

  _sds.back().get().mark_last_member();
}

_ast_entity* struct_declaration_list::_get_child(const size_t i) const noexcept
{
  if (i >= _sds.size())
    return nullptr;

  return &_sds[i].get();
}

void struct_declaration_list::_process(processor<void> &p)
{
  p(*this);
}

void struct_declaration_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool struct_declaration_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool struct_declaration_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


sou_decl_link::sou_decl_link() noexcept
  : _target_kind(target_kind::unlinked)
{}

sou_decl_link::sou_decl_link(struct_or_union_ref &sour) noexcept
  : _target_kind(target_kind::ref), _sour(&sour)
{}

sou_decl_link::sou_decl_link(struct_or_union_def &soud) noexcept
  : _target_kind(target_kind::def), _soud(&soud)
{}

bool sou_decl_link::operator==(const sou_decl_link &rhs) const noexcept
{
  if (_target_kind != rhs._target_kind)
    return false;

  switch (_target_kind) {
  case target_kind::ref:
    return _sour == rhs._sour;

  case target_kind::def:
    return _soud == rhs._soud;

  case target_kind::unlinked:
    assert(0);
    __builtin_unreachable();
  };
}

struct_or_union_ref& sou_decl_link::get_target_sou_ref() const noexcept
{
  assert(_target_kind == target_kind::ref);
  return *_sour;
}

struct_or_union_def& sou_decl_link::get_target_sou_def() const noexcept
{
  assert(_target_kind == target_kind::def);
  return *_soud;
}

const sou_decl_list_node& sou_decl_link::get_target_decl_list_node()
  const noexcept
{
  switch (_target_kind) {
  case target_kind::ref:
    return _sour->get_decl_list_node();

  case target_kind::def:
    return _soud->get_decl_list_node();

  case target_kind::unlinked:
    assert(0);
    __builtin_unreachable();
  };
}


sou_decl_list_node::sou_decl_list_node(struct_or_union_ref &self) noexcept
  : _next(self), _prev(&_next), _first_in_list(true)
{}

sou_decl_list_node::sou_decl_list_node(struct_or_union_def &self) noexcept
  : _next(self), _prev(&_next), _first_in_list(true)
{}

void sou_decl_list_node::link_to(struct_or_union_ref &target) noexcept
{
  // Any struct_or_union_ref on this list is a declaration by itself
  // and thus, must not have a link to a declaration set.
  // Our _decl_list_node should point to ourselves. The assertion
  // thus checks that the containing struct_or_union_ref, if any,
  // hasn't been linked to any declaration.
  assert(_next.get_target_kind() != sou_decl_link::target_kind::ref ||
	 (_next.get_target_sou_ref().get_link_to_decl().get_target_kind() ==
	  sou_decl_link::target_kind::unlinked));
  assert(target.get_link_to_decl().get_target_kind() ==
	 sou_decl_link::target_kind::unlinked);
  __link_to(target);
}

void sou_decl_list_node::link_to(struct_or_union_def &target) noexcept
{
  __link_to(target);
}

template<typename target_type>
void sou_decl_list_node::__link_to(target_type &target) noexcept
{
  assert(_prev == &_next);

  sou_decl_list_node &target_node = target.get_decl_list_node();
  _prev = target_node._prev;
  target_node._prev = &_next;
  *_prev = _next; // links previous node to this one
  _next = sou_decl_link{target};
  _first_in_list = false;
}

const sou_id sou_decl_list_node::get_id() const noexcept
{
  return sou_id(&this->_find_first_decl_node());
}

struct_or_union_def* sou_decl_list_node::find_definition() const noexcept
{
  // As an optimization, start the search at the resolved declaration
  // node itself.
  const sou_decl_list_node &start =
    _find_decl_node()._prev->get_target_decl_list_node();
  const sou_decl_list_node *cur = &start;
  do {
    switch(cur->_next.get_target_kind()) {
    case sou_decl_link::target_kind::ref:
      cur = &cur->_next.get_target_sou_ref().get_decl_list_node();
      break;

    case sou_decl_link::target_kind::def:
      return &cur->_next.get_target_sou_def();
      break;

    case sou_decl_link::target_kind::unlinked:
      assert(0);
      __builtin_unreachable();
    }
  } while (cur != &start);

  return nullptr;
}

bool sou_decl_list_node::is_visible_from(const sou_decl_list_node &from)
  const noexcept
{
  assert(&from._find_decl_node() == &from);
  for (const sou_decl_list_node *cur = &from;
       cur;
       cur = (!cur->is_first() ?
	      &cur->get_next().get_target_decl_list_node() :
	      nullptr)) {
    if (cur == this)
      return true;
  }

  return false;
}

const sou_decl_list_node &sou_decl_list_node::_find_decl_node()
  const noexcept
{
  if (_prev != &_next) {
    // This node is on some list of redeclarations and thus, a
    // declaration by itself.
    return *this;
  } else if (_prev->get_target_kind() == sou_decl_link::target_kind::def) {
    // This node is embedded in a definition (which is always a declaration).
    return *this;
  }

  // Get the struct_or_union_ref this sou_decl_list_node is embedded
  // into and resolve either to the declaration link target or, if
  // none has been set, to self.
  const struct_or_union_ref &sour = _prev->get_target_sou_ref();
  switch (sour.get_link_to_decl().get_target_kind()) {
  case sou_decl_link::target_kind::unlinked:
    return *this;

  case sou_decl_link::target_kind::def:
    return sour.get_link_to_decl().get_target_sou_def().get_decl_list_node();

  case sou_decl_link::target_kind::ref:
    return sour.get_link_to_decl().get_target_sou_ref().get_decl_list_node();
  }
}

const sou_decl_list_node& sou_decl_list_node::_find_first_decl_node()
  const noexcept
{
  const sou_decl_list_node &start = _find_decl_node();
  const sou_decl_list_node *cur = &start;
  while (!cur->_first_in_list) {
    cur = &cur->_next.get_target_decl_list_node();
    assert(cur != &start);
  }

  return *cur;
}


struct_or_union_def::struct_or_union_def(const pp_tokens_range &tr,
					 const types::struct_or_union_kind souk,
					 const pp_token_index id_tok,
					 struct_declaration_list* &&sdl,
					 attribute_specifier_list* &&asl_before,
					 attribute_specifier_list* &&asl_after)
  noexcept
  : struct_or_union_def(tr, souk, id_tok, std::move(sdl),
			std::move(asl_before), std::move(asl_after), true)
{}

struct_or_union_def::struct_or_union_def(const pp_tokens_range &tr,
					 const types::struct_or_union_kind souk,
					 struct_declaration_list* &&sdl,
					 attribute_specifier_list* &&asl_before,
					 attribute_specifier_list* &&asl_after)
  noexcept
  : struct_or_union_def(tr, souk, 0, std::move(sdl),
			std::move(asl_before), std::move(asl_after), false)
{}

struct_or_union_def::struct_or_union_def(const pp_tokens_range &tr,
					 const types::struct_or_union_kind souk,
					 const pp_token_index id_tok,
					 struct_declaration_list* &&sdl,
					 attribute_specifier_list* &&asl_before,
					 attribute_specifier_list* &&asl_after,
					 const bool id_tok_valid) noexcept

  : type_specifier(tr), _souk(souk), _id_tok(id_tok),
    _sdl(mv_p(std::move(sdl))), _asl_before(mv_p(std::move(asl_before))),
    _asl_after(mv_p(std::move(asl_after))),
    _decl_list_node(*this), _id_tok_valid(id_tok_valid)
{
  if (_sdl)
    _sdl->_set_parent(*this);
  if (_asl_before)
    _asl_before->_set_parent(*this);
  if (_asl_after)
    _asl_after->_set_parent(*this);
}

struct_or_union_def::~struct_or_union_def() noexcept
{
  delete _sdl;
  delete _asl_before;
  delete _asl_after;
}

const klp::ccp::types::struct_or_union_content*
struct_or_union_def::get_content() const noexcept
{
  return _content.get();
}

void struct_or_union_def::
set_content(std::unique_ptr<const types::struct_or_union_content> &&c) noexcept
{
  assert(!_content);
  _content = std::move(c);
}

_ast_entity* struct_or_union_def::_get_child(const size_t i) const noexcept
{
  std::size_t _i = i;

  if (_sdl) {
    if (!_i)
      return _sdl;

    --_i;
  }

  if (_asl_before) {
    if (!_i)
      return _asl_before;

    --_i;
  }

  if (_asl_after) {
    if (!_i)
      return _asl_after;

    --_i;
  }

  return nullptr;
}

void struct_or_union_def::_process(processor<void> &p)
{
  p(*this);
}

void struct_or_union_def::_process(const_processor<void> &p) const
{
  p(*this);
}

bool struct_or_union_def::_process(processor<bool> &p)
{
  return p(*this);
}

bool struct_or_union_def::_process(const_processor<bool> &p) const
{
  return p(*this);
}


struct_or_union_ref::struct_or_union_ref(const pp_tokens_range &tr,
					 const types::struct_or_union_kind souk,
					 const pp_token_index id_tok,
					 attribute_specifier_list* &&asl)
  noexcept
  : type_specifier(tr), _souk(souk), _id_tok(id_tok),
    _asl(mv_p(std::move(asl))), _decl_list_node(*this)
{
  if (_asl)
    _asl->_set_parent(*this);
}

struct_or_union_ref::~struct_or_union_ref() noexcept
{
  delete _asl;
}

void struct_or_union_ref::link_to_declaration(const sou_decl_link &target)
  noexcept
{
  assert(_link_to_decl.get_target_kind() ==
	 sou_decl_link::target_kind::unlinked);
  assert(&_decl_list_node.get_next().get_target_sou_ref() == this);
  _link_to_decl = target;
}

bool struct_or_union_ref::is_standalone_decl() const noexcept
{
  bool is_standalone = false;
  auto &&standalone_decl_checker
    = (wrap_callables<no_default_action>
       ([](const specifier_qualifier_list&) {
	  // Move on searching upwards the tree.
	  return true;
	},
	[&is_standalone](const struct_declaration_c99 &sd) {
	  is_standalone = sd.get_struct_declarator_list().empty();
	},
	[&is_standalone](const struct_declaration_unnamed_sou&) {
	  // struct_declaration_unnamed_sou can be a parent of a
	  // specifier_qualifier_list. A specifier_qualifier_list can
	  // be a parent of our struct_or_union_ref.
	  // However, this can't both be true for the *same*
	  // specifier_qualifier_list instance.
	  assert(0);
	  __builtin_unreachable();
	},
	[&is_standalone](const type_name&) {
	  is_standalone = false;
	},
	[&is_standalone](const declaration &d) {
	  is_standalone = !d.get_init_declarator_list();
	},
	[&is_standalone](const parameter_declaration_declarator&) {
	  is_standalone = false;
	},
	[&is_standalone](const parameter_declaration_abstract&) {
	  is_standalone = false;
	},
	[&is_standalone](const function_definition&) {
	  is_standalone = false;
	}));
  this->for_each_ancestor<type_set<struct_declaration_c99,
				   struct_declaration_unnamed_sou,
				   type_name, declaration,
				   parameter_declaration_declarator,
				   parameter_declaration_abstract,
				   function_definition> >
    (standalone_decl_checker);
  return is_standalone;
}

_ast_entity* struct_or_union_ref::_get_child(const size_t i) const noexcept
{
  if (!i)
    return _asl;

  return nullptr;
}

void struct_or_union_ref::_process(processor<void> &p)
{
  p(*this);
}

void struct_or_union_ref::_process(const_processor<void> &p) const
{
  p(*this);
}

bool struct_or_union_ref::_process(processor<bool> &p)
{
  return p(*this);
}

bool struct_or_union_ref::_process(const_processor<bool> &p) const
{
  return p(*this);
}


enumerator::enumerator(const pp_tokens_range &tr, const pp_token_index id_tok,
		       expr* &&value) noexcept
  : ast_entity(tr), _id_tok(id_tok), _value(mv_p(std::move(value)))
{
  if (_value)
    _value->_set_parent(*this);
}

enumerator::~enumerator() noexcept
{
  delete _value;
}

const klp::ccp::types::enum_content::member& enumerator::to_member(const ast &a)
  const noexcept
{
  const std::string& name = a.get_pp_tokens()[_id_tok].get_value();
  const enumerator_list &el = get_unique_parent<enumerator_list>();
  const types::enum_content::member *m = el.get_content().lookup(name);
  assert(m);
  return *m;
}

_ast_entity* enumerator::_get_child(const size_t i) const noexcept
{
  if (!i)
    return _value;

  return nullptr;
}

void enumerator::_process(processor<void> &p)
{
  p(*this);
}

void enumerator::_process(const_processor<void> &p) const
{
  p(*this);
}

bool enumerator::_process(processor<bool> &p)
{
  return p(*this);
}

bool enumerator::_process(const_processor<bool> &p) const
{
  return p(*this);
}


enumerator_list::enumerator_list(enumerator* &&e)
  : ast_entity(e->get_tokens_range())
{
  auto _e = std::ref(*mv_p(std::move(e)));
  try {
    _es.push_back(_e);
  } catch(...) {
    delete &_e.get();
    throw;
  }
  _e.get()._set_parent(*this);
}

enumerator_list::~enumerator_list() noexcept
{
  for (auto e : _es)
    delete &e.get();
}

void enumerator_list::extend(enumerator* &&e)
{
  auto _e = std::ref(*mv_p(std::move(e)));
  try {
    _es.push_back(_e);
  } catch(...) {
    delete &_e.get();
    throw;
  }
  _extend_tokens_range(_e.get().get_tokens_range());
  _e.get()._set_parent(*this);
}

_ast_entity* enumerator_list::_get_child(const size_t i) const noexcept
{
  if (i >= _es.size())
    return nullptr;

  return &_es[i].get();
}

void enumerator_list::_process(processor<void> &p)
{
  p(*this);
}

void enumerator_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool enumerator_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool enumerator_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


enum_decl_link::enum_decl_link() noexcept
  : _target_kind(target_kind::unlinked)
{}

enum_decl_link::enum_decl_link(enum_ref &er) noexcept
  : _target_kind(target_kind::ref), _er(&er)
{}

enum_decl_link::enum_decl_link (enum_def &ed) noexcept
  : _target_kind(target_kind::def), _ed(&ed)
{}

bool enum_decl_link::operator==(const enum_decl_link &rhs) const noexcept
{
  if (_target_kind != rhs._target_kind)
    return false;

  switch (_target_kind) {
  case target_kind::ref:
    return _er == rhs._er;

  case target_kind::def:
    return _ed == rhs._ed;

  case target_kind::unlinked:
    assert(0);
    __builtin_unreachable();
  };
}

enum_ref& enum_decl_link::get_target_enum_ref() const noexcept
{
  assert(_target_kind == target_kind::ref);
  return *_er;
}

enum_def& enum_decl_link::get_target_enum_def() const noexcept
{
  assert(_target_kind == target_kind::def);
  return *_ed;
}

const enum_decl_list_node& enum_decl_link::get_target_decl_list_node()
  const noexcept
{
  switch (_target_kind) {
  case target_kind::ref:
    return _er->get_decl_list_node();

  case target_kind::def:
    return _ed->get_decl_list_node();

  case target_kind::unlinked:
    assert(0);
    __builtin_unreachable();
  };
}


enum_decl_list_node::enum_decl_list_node(enum_ref &self) noexcept
  : _next(self), _prev(&_next), _first_in_list(true)
{}

enum_decl_list_node::enum_decl_list_node(enum_def &self) noexcept
  : _next(self), _prev(&_next), _first_in_list(true)
{}

void enum_decl_list_node::link_to(enum_ref &target) noexcept
{
  // Any enum_ref on this list is a declaration by itself and thus,
  // must not have a link to a declaration set. Our _decl_list_node
  // should point to ourselves. The assertion thus checks that the
  // containing enum_ref, if any, hasn't been linked to any
  // declaration.
  assert(_next.get_target_kind() != enum_decl_link::target_kind::ref ||
	 (_next.get_target_enum_ref().get_link_to_decl().get_target_kind() ==
	  enum_decl_link::target_kind::unlinked));
  assert(target.get_link_to_decl().get_target_kind() ==
	 enum_decl_link::target_kind::unlinked);
  __link_to(target);
}

void enum_decl_list_node::link_to(enum_def &target) noexcept
{
  __link_to(target);
}

template <typename target_type>
void enum_decl_list_node::__link_to(target_type &target) noexcept
{
  assert(_prev == &_next);

  enum_decl_list_node &target_node = target.get_decl_list_node();
  _prev = target_node._prev;
  target_node._prev = &_next;
  *_prev = _next; // links previous node to this one
  _next = enum_decl_link{target};
  _first_in_list = false;
}

const enum_id enum_decl_list_node::get_id() const noexcept
{
  return enum_id(&this->_find_first_decl_node());
}

enum_def* enum_decl_list_node::find_definition() const noexcept
{
  // As an optimization, start the search at the resolved declaration
  // node itself.
  const enum_decl_list_node &start =
    _find_decl_node()._prev->get_target_decl_list_node();
  const enum_decl_list_node *cur = &start;
  do {
    switch(cur->_next.get_target_kind()) {
    case enum_decl_link::target_kind::ref:
      cur = &cur->_next.get_target_enum_ref().get_decl_list_node();
      break;

    case enum_decl_link::target_kind::def:
      return &cur->_next.get_target_enum_def();
      break;

    case enum_decl_link::target_kind::unlinked:
      assert(0);
      __builtin_unreachable();
    }
  } while (cur != &start);

  return nullptr;
}

bool enum_decl_list_node::is_visible_from(const enum_decl_list_node &from)
  const noexcept
{
  assert(&from._find_decl_node() == &from);
  for (const enum_decl_list_node *cur = &from;
       cur;
       cur = (!cur->is_first() ?
	      &cur->get_next().get_target_decl_list_node() :
	      nullptr)) {
    if (cur == this)
      return true;
  }

  return false;
}

const enum_decl_list_node& enum_decl_list_node::_find_decl_node()
  const noexcept
{
  if (_prev != &_next) {
    // This node is on some list of redeclarations and thus, a
    // declaration by itself.
    return *this;
  } else if (_prev->get_target_kind() == enum_decl_link::target_kind::def) {
    // This node is embedded in a definition (which is always a declaration).
    return *this;
  }

  // Get the enum_ref this sou_decl_list_node is embedded
  // into and resolve either to the declaration link target or, if
  // none has been set, to self.
  const enum_ref &er = _prev->get_target_enum_ref();
  switch (er.get_link_to_decl().get_target_kind()) {
  case enum_decl_link::target_kind::unlinked:
    return *this;

  case enum_decl_link::target_kind::def:
    return er.get_link_to_decl().get_target_enum_def().get_decl_list_node();

  case enum_decl_link::target_kind::ref:
    return er.get_link_to_decl().get_target_enum_ref().get_decl_list_node();
  }
}

const enum_decl_list_node& enum_decl_list_node::_find_first_decl_node()
  const noexcept
{
  const enum_decl_list_node &start = _find_decl_node();
  const enum_decl_list_node *cur = &start;
  while (!cur->_first_in_list) {
    cur = &cur->_next.get_target_decl_list_node();
    assert(cur != &start);
  }

  return *cur;
}


enum_def::enum_def(const pp_tokens_range &tr,
		   const pp_token_index id_tok,
		   enumerator_list* &&el,
		   attribute_specifier_list* &&asl_before,
		   attribute_specifier_list* &&asl_after) noexcept
  : enum_def(tr, id_tok, std::move(el), std::move(asl_before),
	     std::move(asl_after), true)
{}

enum_def::enum_def(const pp_tokens_range &tr,
		   enumerator_list* &&el,
		   attribute_specifier_list* &&asl_before,
		   attribute_specifier_list* &&asl_after) noexcept
  : enum_def(tr, 0, std::move(el), std::move(asl_before),
	     std::move(asl_after), false)
{}

enum_def::enum_def(const pp_tokens_range &tr,
		   const pp_token_index id_tok,
		   enumerator_list* &&el,
		   attribute_specifier_list* &&asl_before,
		   attribute_specifier_list* &&asl_after,
		   const bool id_tok_valid) noexcept
  : type_specifier(tr), _id_tok(id_tok),
    _el(*mv_p(std::move(el))), _asl_before(mv_p(std::move(asl_before))),
    _asl_after(mv_p(std::move(asl_after))),
    _decl_list_node(*this), _id_tok_valid(id_tok_valid)
{
  _el._set_parent(*this);
  if (_asl_before)
    _asl_before->_set_parent(*this);
  if (_asl_after)
    _asl_after->_set_parent(*this);
}

enum_def::~enum_def() noexcept
{
  delete &_el;
  delete _asl_before;
  delete _asl_after;
}

_ast_entity* enum_def::_get_child(const size_t i) const noexcept
{
  if (!i) {
    return &_el;
  } else if (i == 1) {
    if (_asl_before)
      return _asl_before;
    else
      return _asl_after;
  } else if (i == 2 && _asl_before) {
    return _asl_after;
  }

  return nullptr;
}

void enum_def::_process(processor<void> &p)
{
  p(*this);
}

void enum_def::_process(const_processor<void> &p) const
{
  p(*this);
}

bool enum_def::_process(processor<bool> &p)
{
  return p(*this);
}

bool enum_def::_process(const_processor<bool> &p) const
{
  return p(*this);
}


enum_ref::enum_ref(const pp_tokens_range &tr,
		   const pp_token_index id_tok,
		   attribute_specifier_list* &&asl) noexcept
  : type_specifier(tr), _id_tok(id_tok), _asl(mv_p(std::move(asl))),
    _decl_list_node(*this)
{
  if (_asl)
    _asl->_set_parent(*this);
}

enum_ref::~enum_ref() noexcept
{
  delete _asl;
}

void enum_ref::link_to_declaration(const enum_decl_link &target) noexcept
{
  assert(_link_to_decl.get_target_kind() ==
	 enum_decl_link::target_kind::unlinked);
  assert(&_decl_list_node.get_next().get_target_enum_ref() == this);
  _link_to_decl = target;
}

bool enum_ref::is_standalone_decl() const noexcept
{
  bool is_standalone = false;
  auto &&standalone_decl_checker
    = (wrap_callables<no_default_action>
       ([](const specifier_qualifier_list&) {
	  // Move on searching upwards the tree.
	  return true;
	},
	[&is_standalone](const struct_declaration_c99 &sd) {
	  is_standalone = sd.get_struct_declarator_list().empty();
	},
	[&is_standalone](const struct_declaration_unnamed_sou&) {
	  // struct_declaration_unnamed_sou can be a parent of a
	  // specifier_qualifier_list. A specifier_qualifier_list can
	  // be a parent of our struct_or_union_ref.
	  // However, this can't both be true for the *same*
	  // specifier_qualifier_list instance.
	  assert(0);
	  __builtin_unreachable();
	},
	[&is_standalone](const type_name&) {
	  is_standalone = false;
	},
	[&is_standalone](const declaration &d) {
	  is_standalone = !d.get_init_declarator_list();
	},
	[&is_standalone](const parameter_declaration_declarator&) {
	  is_standalone = false;
	},
	[&is_standalone](const parameter_declaration_abstract&) {
	  is_standalone = false;
	},
	[&is_standalone](const function_definition&) {
	  is_standalone = false;
	}));
  this->for_each_ancestor<type_set<struct_declaration_c99,
				   struct_declaration_unnamed_sou,
				   type_name, declaration,
				   parameter_declaration_declarator,
				   parameter_declaration_abstract,
				   function_definition> >
    (standalone_decl_checker);
  return is_standalone;
}

_ast_entity* enum_ref::_get_child(const size_t i) const noexcept
{
  if (!i)
    return _asl;

  return nullptr;
}

void enum_ref::_process(processor<void> &p)
{
  p(*this);
}

void enum_ref::_process(const_processor<void> &p) const
{
  p(*this);
}

bool enum_ref::_process(processor<bool> &p)
{
  return p(*this);
}

bool enum_ref::_process(const_processor<bool> &p) const
{
  return p(*this);
}


typeof_expr::typeof_expr(const pp_tokens_range &tr,
			 expr* &&e) noexcept
  : type_specifier(tr), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

typeof_expr::~typeof_expr() noexcept
{
  delete &_e;
}

_ast_entity* typeof_expr::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_e;

  return nullptr;
}

void typeof_expr::_process(processor<void> &p)
{
  p(*this);
}

void typeof_expr::_process(const_processor<void> &p) const
{
  p(*this);
}

bool typeof_expr::_process(processor<bool> &p)
{
  return p(*this);
}

bool typeof_expr::_process(const_processor<bool> &p) const
{
  return p(*this);
}


typeof_type_name::typeof_type_name(const pp_tokens_range &tr,
				   type_name* &&tn) noexcept
  : type_specifier(tr), _tn(*mv_p(std::move(tn)))
{
  _tn._set_parent(*this);
}

typeof_type_name::~typeof_type_name() noexcept
{
  delete &_tn;
}

_ast_entity* typeof_type_name::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_tn;

  return nullptr;
}

void typeof_type_name::_process(processor<void> &p)
{
  p(*this);
}

void typeof_type_name::_process(const_processor<void> &p) const
{
  p(*this);
}

bool typeof_type_name::_process(processor<bool> &p)
{
  return p(*this);
}

bool typeof_type_name::_process(const_processor<bool> &p) const
{
  return p(*this);
}


function_specifier::function_specifier(const pp_token_index spec_tok) noexcept
  : ast_entity(pp_tokens_range{spec_tok, spec_tok + 1}),
    _spec_tok(spec_tok)
{}

function_specifier::~function_specifier() noexcept = default;

_ast_entity* function_specifier::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void function_specifier::_process(processor<void> &p)
{
  p(*this);
}

void function_specifier::_process(const_processor<void> &p) const
{
  p(*this);
}

bool function_specifier::_process(processor<bool> &p)
{
  return p(*this);
}

bool function_specifier::_process(const_processor<bool> &p) const
{
  return p(*this);
}


specifier_qualifier_list::specifier_qualifier_list(type_specifier* &&ts)
  : typed_ast_entity(ts->get_tokens_range())
{
  auto _ts = std::ref(*mv_p(std::move(ts)));
  try {
    _tss.push_back(_ts);
  } catch(...) {
    delete &_ts.get();
    throw;
  }
  _ts.get()._set_parent(*this);
}

specifier_qualifier_list::specifier_qualifier_list(type_qualifier* &&tq)
  : typed_ast_entity::typed_ast_entity(tq->get_tokens_range())
{
  auto _tq = std::ref(*mv_p(std::move(tq)));
  try {
    _tqs.push_back(_tq);
  } catch(...) {
    delete &_tq.get();
    throw;
  }
  _tq.get()._set_parent(*this);
}

specifier_qualifier_list::
specifier_qualifier_list(attribute_specifier_list* &&asl)
  : typed_ast_entity(asl->get_tokens_range())
{
  auto _asl = std::ref(*mv_p(std::move(asl)));
  try {
    _asls.push_back(_asl);
  } catch(...) {
    delete &_asl.get();
    throw;
  }
  _asl.get()._set_parent(*this);
}

specifier_qualifier_list::specifier_qualifier_list(const pp_tokens_range &tr)
  noexcept
  : typed_ast_entity(tr)
{}

specifier_qualifier_list::~specifier_qualifier_list() noexcept
{
  for (auto ts: _tss)
    delete &ts.get();

  for (auto tq : _tqs)
    delete &tq.get();

  for (auto asl : _asls)
    delete &asl.get();
}

void specifier_qualifier_list::extend(type_specifier* &&ts)
{
  auto _ts = std::ref(*mv_p(std::move(ts)));
  try {
    _tss.push_back(_ts);
  } catch(...) {
    delete &_ts.get();
    throw;
  }
  _extend_tokens_range(_ts.get().get_tokens_range());
  _ts.get()._set_parent(*this);
}

void specifier_qualifier_list::extend(type_qualifier* &&tq)
{
  auto _tq = std::ref(*mv_p(std::move(tq)));
  try {
    _tqs.push_back(_tq);
  } catch(...) {
    delete &_tq.get();
    throw;
  }
  _extend_tokens_range(_tq.get().get_tokens_range());
  _tq.get()._set_parent(*this);
}

void specifier_qualifier_list::extend(attribute_specifier_list* &&asl)
{
  auto _asl = std::ref(*mv_p(std::move(asl)));
  try {
    _asls.push_back(_asl);
  } catch(...) {
    delete &_asl.get();
    throw;
  }
  _extend_tokens_range(_asl.get().get_tokens_range());
  _asl.get()._set_parent(*this);
}


void specifier_qualifier_list::extend(specifier_qualifier_list* &&sql)
{
  if (!sql)
    return;

  auto _sql = mv_p(std::move(sql));
  try {
    if (!_sql->_tss.empty())
      _tss.reserve(_tss.size() + _sql->_tss.size());

    if (!_sql->_tqs.empty())
      _tqs.reserve(_tqs.size() + _sql->_tqs.size());

    if (!_sql->_asls.empty())
      _asls.reserve(_asls.size() + _sql->_asls.size());
  } catch (...) {
    delete _sql;
    throw;
  }

  for (auto &ts : _sql->_tss) {
    _tss.push_back(ts);
    ts.get()._reset_parent(*this, *_sql);
  }
  _sql->_tss.clear();

  for (auto &tq : _sql->_tqs) {
    _tqs.push_back(tq);
    tq.get()._reset_parent(*this, *_sql);
  }
  _sql->_tqs.clear();

  for (auto &asl : _sql->_asls) {
    _asls.push_back(asl);
    asl.get()._reset_parent(*this, *_sql);
  }
  _sql->_asls.clear();

  _extend_tokens_range(_sql->get_tokens_range());
  delete _sql;
}

const klp::ccp::types::qualifiers
specifier_qualifier_list::get_qualifiers() const
{
  types::qualifiers qs;
  for (auto q : _tqs)
    qs.add(q.get().get_kind());

  return qs;
}

bool specifier_qualifier_list::is_signed_explicit() const noexcept
{
  // Check whether the 'signed' keyword is among the
  // type specifiers. Needed for bitfields.
  for (auto ts : _tss) {
    const type_specifier_pod *ts_pod =
      dynamic_cast<const type_specifier_pod*>(&ts.get());
    if (ts_pod && ts_pod->get_pod_spec() == pod_spec::ps_signed)
      return true;
  }
  return false;
}

bool specifier_qualifier_list::is_auto_type() const noexcept
{
  if (_tss.size() == 1) {
    const type_specifier &ts = _tss[0].get();
    return typeid(ts) == typeid(type_specifier_auto_type);
  }
  return false;
}

std::size_t specifier_qualifier_list::_n_children() const noexcept
{
  return _tss.size() + _tqs.size() + _asls.size();
}

_ast_entity* specifier_qualifier_list::_get_child(const size_t i) const noexcept
{
  if (i < _tss.size()) {
    return &_tss[i].get();
  } else if (i < _tss.size() + _tqs.size()) {
    return &_tqs[i - _tss.size()].get();
  } else if (i < _tss.size() + _tqs.size() + _asls.size()) {
    return &_asls[i - _tss.size() - _tqs.size()].get();
  }

  return nullptr;
}

void specifier_qualifier_list::_process(processor<void> &p)
{
  p(*this);
}

void specifier_qualifier_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool specifier_qualifier_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool specifier_qualifier_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


declaration_specifiers::declaration_specifiers(const pp_tokens_range &tr)
  : specifier_qualifier_list(tr)
{}

declaration_specifiers::declaration_specifiers(storage_class_specifier* &&scs)
  : specifier_qualifier_list(scs->get_tokens_range())
{
  auto _scs = std::ref(*mv_p(std::move(scs)));
  try {
    _scss.push_back(_scs);
  } catch(...) {
    delete &_scs.get();
    throw;
  }
  _scs.get()._set_parent(static_cast<specifier_qualifier_list&>(*this));
}

declaration_specifiers::declaration_specifiers(function_specifier* &&fs)
  : specifier_qualifier_list(fs->get_tokens_range())
{
  auto _fs = std::ref(*mv_p(std::move(fs)));
  try {
    _fss.push_back(_fs);
  } catch(...) {
    delete &_fs.get();
    throw;
  }
  _fs.get()._set_parent(static_cast<specifier_qualifier_list&>(*this));
}

declaration_specifiers::declaration_specifiers(type_specifier* &&ts)
  : specifier_qualifier_list(std::move(ts))
{}

declaration_specifiers::declaration_specifiers(type_qualifier* &&tq)
  : specifier_qualifier_list(std::move(tq))
{}

declaration_specifiers::declaration_specifiers(attribute_specifier_list* &&asl)
  : specifier_qualifier_list(std::move(asl))
{}

declaration_specifiers::~declaration_specifiers() noexcept
{
  for (auto scs: _scss)
    delete &scs.get();

  for (auto fs : _fss)
    delete &fs.get();
}

void declaration_specifiers::extend(storage_class_specifier* &&scs)
{
  auto _scs = std::ref(*mv_p(std::move(scs)));
  try {
    _scss.push_back(_scs);
  } catch(...) {
    delete &_scs.get();
    throw;
  }
  _extend_tokens_range(_scs.get().get_tokens_range());
  _scs.get()._set_parent(static_cast<specifier_qualifier_list&>(*this));
}

void declaration_specifiers::extend(function_specifier* &&fs)
{
  auto _fs = std::ref(*mv_p(std::move(fs)));
  try {
    _fss.push_back(_fs);
  } catch(...) {
    delete &_fs.get();
    throw;
  }
  _extend_tokens_range(_fs.get().get_tokens_range());
  _fs.get()._set_parent(static_cast<specifier_qualifier_list&>(*this));
}

void declaration_specifiers::extend(type_specifier* &&ts)
{
  specifier_qualifier_list::extend(std::move(ts));
}

void declaration_specifiers::extend(type_qualifier* &&tq)
{
  specifier_qualifier_list::extend(std::move(tq));
}

void declaration_specifiers::extend(attribute_specifier_list* &&asl)
{
  specifier_qualifier_list::extend(std::move(asl));
}

void declaration_specifiers::extend(declaration_specifiers* &&ds)
{
  if (!ds)
    return;

  auto _ds = mv_p(std::move(ds));
  try {
    if (!_ds->_scss.empty())
      _scss.reserve(_scss.size() + _ds->_scss.size());

    if (!_ds->_fss.empty())
      _fss.reserve(_fss.size() + _ds->_fss.size());
  } catch (...) {
    delete _ds;
    throw;
  }

  for (auto &scs : _ds->_scss) {
    _scss.push_back(scs);
    scs.get()._reset_parent(static_cast<specifier_qualifier_list&>(*this),
			    static_cast<specifier_qualifier_list&>(*_ds));
  }
  _ds->_scss.clear();

  for (auto &fs : _ds->_fss) {
    _fss.push_back(fs);
    fs.get()._reset_parent(static_cast<specifier_qualifier_list&>(*this),
			   static_cast<specifier_qualifier_list&>(*_ds));
  }
  _ds->_fss.clear();


  specifier_qualifier_list::extend(std::move(_ds));
}


storage_class declaration_specifiers::get_storage_class(ast &ast) const
{
  storage_class sc = storage_class::sc_none;
  for (auto scs : _scss) {
    if (sc == storage_class::sc_none) {
      sc = scs.get().get_storage_class();
    } else if (sc != scs.get().get_storage_class()) {
      code_remark remark(code_remark::severity::fatal,
			 "conflicting storage class specifiers",
			 ast.get_pp_result(),
			 scs.get().get_tokens_range());
      ast.get_remarks().add(remark);
      throw semantic_except(remark);
    }
  }

  return sc;
}

bool declaration_specifiers::is_inline() const noexcept
{
  return !_fss.empty();
}

_ast_entity* declaration_specifiers::_get_child(const size_t i) const noexcept
{
  const size_t _sql_n_children = specifier_qualifier_list::_n_children();

  if (i < _sql_n_children) {
    return specifier_qualifier_list::_get_child(i);
  } else if (i < _sql_n_children + _scss.size()) {
    return &_scss[i - _sql_n_children].get();
  } else if (i < _sql_n_children + _scss.size() + _fss.size()) {
    return &_fss[i - _sql_n_children - _scss.size()].get();
  }

  return nullptr;
}


initializer::initializer(const pp_tokens_range &tr) noexcept
  : ast_entity(tr), _d(nullptr)
{}

initializer::~initializer() noexcept
{
  delete _d;
}

void initializer::_set_designation(designation* &&d) noexcept
{
  assert(!_d);
  _d = mv_p(std::move(d));
}


initializer_expr::initializer_expr(expr* &&e) noexcept
  : initializer(e->get_tokens_range()), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

initializer_expr::~initializer_expr() noexcept
{
  delete &_e;
}

void initializer_expr::set_designation(designation* &&d) noexcept
{
  if (d)
    d->_set_parent(*this);
  _set_designation(std::move(d));
}


_ast_entity* initializer_expr::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_d)
      return _d;
    else
      return &_e;
  } else if (i == 1 && _d) {
    return &_e;
  }

  return nullptr;
}

void initializer_expr::_process(processor<void> &p)
{
  p(*this);
}

void initializer_expr::_process(const_processor<void> &p) const
{
  p(*this);
}

bool initializer_expr::_process(processor<bool> &p)
{
  return p(*this);
}

bool initializer_expr::_process(const_processor<bool> &p) const
{
  return p(*this);
}


initializer_init_list::initializer_init_list(const pp_tokens_range &tr,
					     initializer_list* &&il) noexcept
  : initializer(tr), _il(mv_p(std::move(il)))
{
  if (_il)
    _il->_set_parent(*this);
}

initializer_init_list::~initializer_init_list() noexcept
{
  delete _il;
}

void initializer_init_list::set_designation(designation* &&d) noexcept
{
  if (d)
    d->_set_parent(*this);
  _set_designation(std::move(d));
}

std::size_t initializer_init_list::get_size() const noexcept
{
  return _il ? _il->get_size() : 0;
}

_ast_entity* initializer_init_list::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_d)
      return _d;
    else
      return _il;
  } else if (i == 1 && _d) {
    return _il;
  }

  return nullptr;
}

void initializer_init_list::_process(processor<void> &p)
{
  p(*this);
}

void initializer_init_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool initializer_init_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool initializer_init_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


designator::designator(const pp_tokens_range &tr) noexcept
  : ast_entity(tr)
{}

designator::~designator() noexcept = default;


designator_member::designator_member(const pp_tokens_range &tr,
				     const pp_token_index member_tok) noexcept
  : designator(tr), _member_tok(member_tok)
{}

designator_member::~designator_member() noexcept = default;

_ast_entity* designator_member::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void designator_member::_process(processor<void> &p)
{
  p(*this);
}

void designator_member::_process(const_processor<void> &p) const
{
  p(*this);
}

bool designator_member::_process(processor<bool> &p)
{
  return p(*this);
}

bool designator_member::_process(const_processor<bool> &p) const
{
  return p(*this);
}


designator_array::designator_array(const pp_tokens_range &tr,
				   expr* &&index_first, expr* &&index_last)
  noexcept
  : designator(tr), _index_first(*mv_p(std::move(index_first))),
    _index_last(mv_p(std::move(index_last)))
{
  _index_first._set_parent(*this);
  if (_index_last)
    _index_last->_set_parent(*this);
}

designator_array::~designator_array() noexcept
{
  delete &_index_first;
  delete _index_last;
}

_ast_entity* designator_array::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_index_first;

  case 1:
    return _index_last;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void designator_array::_process(processor<void> &p)
{
  p(*this);
}

void designator_array::_process(const_processor<void> &p) const
{
  p(*this);
}

bool designator_array::_process(processor<bool> &p)
{
  return p(*this);
}

bool designator_array::_process(const_processor<bool> &p) const
{
  return p(*this);
}


designator_list::designator_list(designator* &&d)
  : ast_entity(d->get_tokens_range())
{
  auto _d = std::ref(*mv_p(std::move(d)));
  try {
    _ds.push_back(_d);
  } catch(...) {
    delete &_d.get();
    throw;
  }
  _d.get()._set_parent(*this);
}

designator_list::~designator_list() noexcept
{
  for (auto d : _ds)
    delete &d.get();
}

void designator_list::extend(designator* &&d)
{
  auto _d = std::ref(*mv_p(std::move(d)));
  try {
    _ds.push_back(_d);
  } catch(...) {
    delete &_d.get();
    throw;
  }
  _extend_tokens_range(_d.get().get_tokens_range());
  _d.get()._set_parent(*this);
}

_ast_entity* designator_list::_get_child(const size_t i) const noexcept
{
  if (i >= _ds.size())
    return nullptr;

  return &_ds[i].get();
}

void designator_list::_process(processor<void> &p)
{
  p(*this);
}

void designator_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool designator_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool designator_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


designation::designation(const pp_tokens_range &tr,
			 designator_list* &&dl) noexcept
  : ast_entity(tr), _dl(*mv_p(std::move(dl)))
{
  _dl._set_parent(*this);
}

designation::~designation() noexcept
{
  delete &_dl;
}

_ast_entity* designation::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_dl;

  return nullptr;
}

void designation::_process(processor<void> &p)
{
  p(*this);
}

void designation::_process(const_processor<void> &p) const
{
  p(*this);
}

bool designation::_process(processor<bool> &p)
{
  return p(*this);
}

bool designation::_process(const_processor<bool> &p) const
{
  return p(*this);
}


initializer_list::initializer_list(initializer* &&i)
  : ast_entity(i->get_tokens_range())
{
  auto _i = std::ref(*mv_p(std::move(i)));
  try {
    _is.push_back(_i);
  } catch(...) {
    delete &_i.get();
    throw;
  }
  _i.get()._set_parent(*this);
}

initializer_list::~initializer_list() noexcept
{
  for (auto i : _is)
    delete &i.get();
}

void initializer_list::extend(initializer* &&i)
{
  auto _i = std::ref(*mv_p(std::move(i)));
  try {
    _is.push_back(_i);
  } catch(...) {
    delete &_i.get();
    throw;
  }
  _extend_tokens_range(_i.get().get_tokens_range());
  _i.get()._set_parent(*this);
}

_ast_entity* initializer_list::_get_child(const size_t i) const noexcept
{
  if (i >= _is.size())
    return nullptr;

  return &_is[i].get();
}

void initializer_list::_process(processor<void> &p)
{
  p(*this);
}

void initializer_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool initializer_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool initializer_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


asm_label::asm_label(const pp_tokens_range &tr,
		     string_literal* &&label) noexcept
  : ast_entity(tr), _label(*mv_p(std::move(label)))
{
  _label._set_parent(*this);
}

asm_label::~asm_label() noexcept
{
  delete &_label;
}

_ast_entity* asm_label::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_label;

  return nullptr;
}

void asm_label::_process(processor<void> &p)
{
  p(*this);
}

void asm_label::_process(const_processor<void> &p) const
{
  p(*this);
}

bool asm_label::_process(processor<bool> &p)
{
  return p(*this);
}

bool asm_label::_process(const_processor<bool> &p) const
{
  return p(*this);
}


linkage::linkage() noexcept
  : _linkage_kind(linkage_kind::none),
    _target_kind(link_target_kind::unlinked),
    _target_is_visible(false), _first_in_chain(true),
    _last_in_file_scope_chain(false),
    _type_modified_through_linkage(false)
{}

void linkage::set_linkage_kind(const linkage_kind kind) noexcept
{
  assert(_linkage_kind == linkage_kind::none || _linkage_kind == kind);
  _linkage_kind = kind;
}

void linkage::link(init_declarator &source, init_declarator &target,
		   const linkage_kind kind, const bool target_is_visible,
		   const bool update_last_at_file_scope) noexcept
{
  linkage &l = source.get_linkage();
  assert(l._target_kind == link_target_kind::unlinked);
  l._first_in_chain = false;
  l._link_to(target, kind, target_is_visible);

  if (update_last_at_file_scope) {
    linkage &first_in_chain = l._set_last_in_file_scope_chain();
    first_in_chain._link_to(source, kind, false);
  }
}

void linkage::link(init_declarator &source, function_definition &target,
		   const linkage_kind kind, const bool target_is_visible,
		   const bool update_last_at_file_scope) noexcept
{
  linkage &l = source.get_linkage();
  assert(l._target_kind == link_target_kind::unlinked);
  l._first_in_chain = false;
  l._link_to(target, kind, target_is_visible);

  if (update_last_at_file_scope) {
    linkage &first_in_chain = l._set_last_in_file_scope_chain();
    first_in_chain._link_to(source, kind, false);
  }
}

void linkage::link(function_definition &source, init_declarator &target,
		   const linkage_kind kind, const bool target_is_visible,
		   const bool update_last_at_file_scope) noexcept
{
  linkage &l = source.get_linkage();
  assert(l._target_kind == link_target_kind::unlinked);
  l._first_in_chain = false;
  l._link_to(target, kind, target_is_visible);

  if (update_last_at_file_scope) {
    linkage &first_in_chain = l._set_last_in_file_scope_chain();
    first_in_chain._link_to(source, kind, false);
  }
}

void linkage::link(function_definition &source, function_definition &target,
		   const linkage_kind kind, const bool target_is_visible,
		   const bool update_last_at_file_scope) noexcept
{
  linkage &l = source.get_linkage();
  assert(l._target_kind == link_target_kind::unlinked);
  l._first_in_chain = false;
  l._link_to(target, kind, target_is_visible);

  if (update_last_at_file_scope) {
    linkage &first_in_chain = l._set_last_in_file_scope_chain();
    first_in_chain._link_to(source, kind, false);
  }
}

void linkage::set_first_at_file_scope(init_declarator &first) noexcept
{
  linkage &first_in_chain = first.get_linkage()._set_last_in_file_scope_chain();
  assert(first_in_chain._target_kind == link_target_kind::unlinked);
  first_in_chain._link_to(first, first_in_chain._linkage_kind, false);
}

void linkage::set_first_at_file_scope(function_definition &first) noexcept
{
  linkage &first_in_chain = first.get_linkage()._set_last_in_file_scope_chain();
  assert(first_in_chain._target_kind == link_target_kind::unlinked);
  first_in_chain._link_to(first, first_in_chain._linkage_kind, false);
}

bool linkage::is_linked_to(const init_declarator &id) const noexcept
{
  return _is_linked_to(id.get_linkage());
}

bool linkage::is_linked_to(const function_definition &fd) const noexcept
{
  return _is_linked_to(fd.get_linkage());
}

const init_declarator& linkage::get_target_init_declarator() const noexcept
{
  assert(_target_kind == link_target_kind::init_decl);
  return *_target_id;
}

const function_definition& linkage::get_target_function_definition()
  const noexcept
{
  assert(_target_kind == link_target_kind::function_def);
  return *_target_fd;
}

linkage::linkage_id linkage::get_id() const noexcept
{
  const linkage *cur = this;
  while (!cur->_first_in_chain) {
    switch (cur->_target_kind) {
    case link_target_kind::init_decl:
      cur = &cur->_target_id->get_linkage();
      break;

    case link_target_kind::function_def:
      cur = &cur->_target_fd->get_linkage();
      break;

    case link_target_kind::unlinked:
      assert(0);
      __builtin_unreachable();
    }
  }

  return linkage_id(*cur);
}

const linkage& linkage::find_last_in_file_scope_chain() const noexcept
{
  // This actual returns the first linkage in a file scope chain,
  // because that one refers back to the last init_declarator
  // resp. function_definition.
  const linkage *cur = this;
  while (!cur->_first_in_chain) {
    switch (cur->_target_kind) {
    case link_target_kind::init_decl:
      cur = &cur->_target_id->get_linkage();
      break;

    case link_target_kind::function_def:
      cur = &cur->_target_fd->get_linkage();
      break;

    case link_target_kind::unlinked:
      assert(0);
      __builtin_unreachable();
    }
  }

  assert(cur->_target_kind != link_target_kind::unlinked);
  return *cur;
}

void linkage::_link_to(init_declarator &target, const linkage_kind kind,
		       const bool target_is_visible) noexcept
{
  assert(target.get_linkage().get_linkage_kind() == kind);
  set_linkage_kind(kind);

  assert(_target_kind == link_target_kind::unlinked || _first_in_chain);
  assert(!_first_in_chain || !target_is_visible);
  _target_kind = link_target_kind::init_decl;
  _target_id = &target;

  _target_is_visible = target_is_visible;
}

void linkage::_link_to(function_definition &target,
		       const linkage_kind kind,
		       const bool target_is_visible) noexcept
{
  assert(target.get_linkage().get_linkage_kind() == kind);
  set_linkage_kind(kind);

  assert(_target_kind == link_target_kind::unlinked || _first_in_chain);
  assert(!_first_in_chain || !target_is_visible);
  _target_kind = link_target_kind::function_def;
  _target_fd = &target;

  _target_is_visible = target_is_visible;
}

linkage& linkage::_set_last_in_file_scope_chain() noexcept
{
  _last_in_file_scope_chain = true;

  // Find the first linkage in the file-scope chain and return it
  // such that the caller can link it back to the init_declarator or
  // function_definition enclosing *this;
  linkage *cur = this;
  while (!cur->_first_in_chain) {
    switch (cur->_target_kind) {
    case link_target_kind::init_decl:
      cur = &cur->_target_id->get_linkage();
      break;

    case link_target_kind::function_def:
      cur = &cur->_target_fd->get_linkage();
      break;

    case link_target_kind::unlinked:
      assert(0);
      __builtin_unreachable();
    }

    cur->_last_in_file_scope_chain = false;
  }

  return *cur;
}

bool linkage::_is_linked_to(const linkage &target) const noexcept
{
  return this->get_id() == target.get_id();
}


init_declarator::init_declarator(const pp_tokens_range &tr,
				 declarator* &&d, initializer* &&i,
				 asm_label* &&al,
				 attribute_specifier_list* &&asl_middle,
				 attribute_specifier_list* &&asl_after) noexcept
  : ast_entity(tr), _d(*mv_p(std::move(d))), _i(mv_p(std::move(i))),
    _al(mv_p(std::move(al))), _asl_before(nullptr),
    _asl_middle(mv_p(std::move(asl_middle))),
    _asl_after(mv_p(std::move(asl_after)))
{
  _d._set_parent(*this);
  if (_i)
    _i->_set_parent(*this);
  if (_al)
    _al->_set_parent(*this);
  if (_asl_after)
    _asl_after->_set_parent(*this);
}

init_declarator::~init_declarator() noexcept
{
  delete &_d;
  delete _i;
  delete _al;
  delete _asl_before;
  delete _asl_middle;
  delete _asl_after;
}

void init_declarator::set_asl_before(attribute_specifier_list* &&asl_before)
  noexcept
{
  assert(!_asl_before);
  _asl_before = mv_p(std::move(asl_before));
  if (_asl_before)
    _asl_before->_set_parent(*this);
}

_ast_entity* init_declarator::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_d;

  std::size_t _ix = i - 1;
  if (_i) {
    if (!_ix)
      return _i;

    --_ix;
  }

  if (_al) {
    if (!_ix)
      return _al;

    --_ix;
  }

  if (_asl_before) {
    if (!_ix)
      return _asl_before;

    --_ix;
  }

  if (_asl_middle) {
    if (!_ix)
      return _asl_middle;

    --_ix;
  }

  if (_asl_after) {
    if (!_ix)
      return _asl_after;

    --_ix;
  }

  return nullptr;
}

void init_declarator::_process(processor<void> &p)
{
  p(*this);
}

void init_declarator::_process(const_processor<void> &p) const
{
  p(*this);
}

bool init_declarator::_process(processor<bool> &p)
{
  return p(*this);
}

bool init_declarator::_process(const_processor<bool> &p) const
{
  return p(*this);
}


init_declarator_list::init_declarator_list(init_declarator* &&id)
  : ast_entity(id->get_tokens_range())
{
  auto _id = std::ref(*mv_p(std::move(id)));
  try {
    _ids.push_back(_id);
  } catch(...) {
    delete &_id.get();
    throw;
  }
  _id.get()._set_parent(*this);
}

init_declarator_list::~init_declarator_list() noexcept
{
  for (auto id : _ids)
    delete &id.get();
}

void init_declarator_list::extend(init_declarator* &&id)
{
  auto _id = std::ref(*mv_p(std::move(id)));
  try {
    _ids.push_back(_id);
  } catch(...) {
    delete &_id.get();
    throw;
  }
  _extend_tokens_range(_id.get().get_tokens_range());
  _id.get()._set_parent(*this);
}

_ast_entity* init_declarator_list::_get_child(const size_t i) const noexcept
{
  if (i >= _ids.size())
    return nullptr;

  return &_ids[i].get();
}

void init_declarator_list::_process(processor<void> &p)
{
  p(*this);
}

void init_declarator_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool init_declarator_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool init_declarator_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


declaration::declaration(const pp_tokens_range &tr,
			 declaration_specifiers* &&ds,
			 init_declarator_list* &&idl) noexcept
  : ast_entity(tr), _ds(*mv_p(std::move(ds))), _idl(mv_p(std::move(idl)))
{
  _ds._set_parent(*this);
  if (_idl)
    _idl->_set_parent(*this);
}

declaration::~declaration() noexcept
{
  delete &_ds;
  delete _idl;
}

bool declaration::is_at_file_scope() const noexcept
{
  return get_parent()->is_any_of<external_declaration>();
}

_ast_entity* declaration::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_ds;
  else if (i == 1)
    return _idl;

  return nullptr;
}

void declaration::_process(processor<void> &p)
{
  p(*this);
}

void declaration::_process(const_processor<void> &p) const
{
  p(*this);
}

bool declaration::_process(processor<bool> &p)
{
  return p(*this);
}

bool declaration::_process(const_processor<bool> &p) const
{
  return p(*this);
}


static_assertion::static_assertion(const pp_tokens_range &tr, expr* &&cond,
				   string_literal *&&msg) noexcept
  : ast_entity{tr}, _cond{*mv_p(std::move(cond))}, _msg{*mv_p(std::move(msg))}
{
  _cond._set_parent(*this);
  _msg._set_parent(*this);
}

static_assertion::~static_assertion() noexcept
{
  delete &_cond;
  delete &_msg;
}

_ast_entity* static_assertion::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_cond;
  else if (i == 1)
    return &_msg;

  return nullptr;
}

void static_assertion::_process(processor<void> &p)
{
  p(*this);
}

void static_assertion::_process(const_processor<void> &p) const
{
  p(*this);
}

bool static_assertion::_process(processor<bool> &p)
{
  return p(*this);
}

bool static_assertion::_process(const_processor<bool> &p) const
{
  return p(*this);
}


parameter_declaration::
parameter_declaration(const pp_tokens_range &tr,
		      declaration_specifiers* &&ds,
		      attribute_specifier_list* &&asl) noexcept
  : ast_entity(tr), _ds(*mv_p(std::move(ds))), _asl(mv_p(std::move(asl)))
{}

parameter_declaration::~parameter_declaration() noexcept
{
  delete &_ds;
  delete _asl;
}


parameter_declaration_declarator::
parameter_declaration_declarator(const pp_tokens_range &tr,
				 declaration_specifiers* &&ds,
				 declarator* &&d,
				 attribute_specifier_list* &&asl) noexcept
  : parameter_declaration(tr, std::move(ds), std::move(asl)),
    _d(*mv_p(std::move(d)))
{
  get_declaration_specifiers()._set_parent(*this);
  _d._set_parent(*this);
  if (_asl)
    _asl->_set_parent(*this);
}

parameter_declaration_declarator::~parameter_declaration_declarator() noexcept
{
  delete &_d;
}

_ast_entity* parameter_declaration_declarator::_get_child(const size_t i)
  const noexcept
{
  switch (i)
  {
  case 0:
    return &_ds;

  case 1:
    return &_d;

  case 2:
    return _asl;

  case 3:
    return nullptr;
  }

  __builtin_unreachable();
}

void parameter_declaration_declarator::_process(processor<void> &p)
{
  p(*this);
}

void parameter_declaration_declarator::_process(const_processor<void> &p) const
{
  p(*this);
}

bool parameter_declaration_declarator::_process(processor<bool> &p)
{
  return p(*this);
}

bool parameter_declaration_declarator::_process(const_processor<bool> &p) const
{
  return p(*this);
}


parameter_declaration_abstract::
parameter_declaration_abstract(const pp_tokens_range &tr,
			       declaration_specifiers* &&ds,
			       abstract_declarator* &&ad,
			       attribute_specifier_list* &&asl)
  noexcept
  : parameter_declaration(tr, std::move(ds), std::move(asl)),
    _ad(mv_p(std::move(ad)))
{
  get_declaration_specifiers()._set_parent(*this);
  if (_ad)
    _ad->_set_parent(*this);
  if (_asl)
    _asl->_set_parent(*this);
}

parameter_declaration_abstract::~parameter_declaration_abstract() noexcept
{
  delete _ad;
}

_ast_entity* parameter_declaration_abstract::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_ds;

  case 1:
    if (_ad)
      return _ad;
    else
      return _asl;

  case 2:
    return _asl;

  case 3:
    return nullptr;
  }

  __builtin_unreachable();
}

void parameter_declaration_abstract::_process(processor<void> &p)
{
  p(*this);
}

void parameter_declaration_abstract::_process(const_processor<void> &p) const
{
  p(*this);
}

bool parameter_declaration_abstract::_process(processor<bool> &p)
{
  return p(*this);
}

bool parameter_declaration_abstract::_process(const_processor<bool> &p) const
{
  return p(*this);
}


parameter_declaration_list::
parameter_declaration_list(parameter_declaration* &&pd)
  : ast_entity(pd->get_tokens_range()), _variadic(false)
{
  auto _pd = std::ref(*mv_p(std::move(pd)));
  try {
    _pds.push_back(_pd);
  } catch(...) {
    delete &_pd.get();
    throw;
  }
  _pd.get()._set_parent(*this);
}

parameter_declaration_list::~parameter_declaration_list() noexcept
{
  for (auto pd : _pds)
    delete &pd.get();
}

void parameter_declaration_list::extend(parameter_declaration* &&pd)
{
  auto _pd = std::ref(*mv_p(std::move(pd)));
  try {
    _pds.push_back(_pd);
  } catch(...) {
    delete &_pd.get();
    throw;
  }
  _extend_tokens_range(_pd.get().get_tokens_range());
  _pd.get()._set_parent(*this);
}

void
parameter_declaration_list::set_variadic(const pp_tokens_range &triple_dot_tr)
  noexcept
{
  _variadic = true;
  _extend_tokens_range(triple_dot_tr);
}

bool parameter_declaration_list::
is_single_void(ast &a) const
{
  if (_variadic || _pds.size() > 1)
    return false;
  assert(_pds.size() == 1);

  const parameter_declaration &pd = _pds[0].get();
  return (pd.process<bool, type_set<parameter_declaration_abstract>>
	  (wrap_callables<default_action_return_value<bool, false>::type>
	   ([&](const parameter_declaration_abstract &pda) {
	     if (pda.get_declaration_specifiers().get_storage_class(a) !=
		 storage_class::sc_none)
	       return false;

	     const types::void_type * const vt =
	       dynamic_cast<const types::void_type *>(pda.get_type().get());
	     if (!vt || vt->get_qualifiers().any())
	       return false;

	     return true;
	    })));
}

_ast_entity* parameter_declaration_list::_get_child(const size_t i) const noexcept
{
  if (i >= _pds.size())
    return nullptr;

  return &_pds[i].get();
}

void parameter_declaration_list::_process(processor<void> &p)
{
  p(*this);
}

void parameter_declaration_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool parameter_declaration_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool parameter_declaration_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


identifier_list::identifier_list(const pp_token_index id_tok)
  : ast_entity(pp_tokens_range{id_tok, id_tok + 1}),
    _ids(1, id_tok)
{}

identifier_list::~identifier_list() noexcept = default;

void identifier_list::extend(const pp_token_index id_tok)
{
  _ids.push_back(id_tok);
  _extend_tokens_range(pp_tokens_range{id_tok, id_tok + 1});
}

_ast_entity* identifier_list::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void identifier_list::_process(processor<void> &p)
{
  p(*this);
}

void identifier_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool identifier_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool identifier_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}

declaration_list::declaration_list(declaration* &&d)
  : ast_entity(d->get_tokens_range())
{
  auto _d = std::ref(*mv_p(std::move(d)));
  try {
    _ds.push_back(_d);
  } catch(...) {
    delete &_d.get();
    throw;
  }
  _d.get()._set_parent(*this);
}

declaration_list::~declaration_list() noexcept
{
  for (auto d : _ds)
    delete &d.get();
}

void declaration_list::extend(declaration* &&d)
{
  auto _d = std::ref(*mv_p(std::move(d)));
  try {
    _ds.push_back(_d);
  } catch(...) {
    delete &_d.get();
    throw;
  }
  _extend_tokens_range(_d.get().get_tokens_range());
  _d.get()._set_parent(*this);
}

_ast_entity* declaration_list::_get_child(const size_t i) const noexcept
{
  if (i >= _ds.size())
    return nullptr;

  return &_ds[i].get();
}

void declaration_list::_process(processor<void> &p)
{
  p(*this);
}

void declaration_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool declaration_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool declaration_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt::stmt(const pp_tokens_range &tr) noexcept
  : ast_entity(tr)
{}

stmt::~stmt() noexcept = default;


stmt_labeled::stmt_labeled(const pp_tokens_range &tr,
			   const pp_token_index label_tok,
			   stmt* &&s, attribute_specifier_list* &&asl) noexcept
  : stmt(tr), _label_tok(label_tok), _s(*mv_p(std::move(s))),
    _asl(mv_p(std::move(asl)))
{
  _s._set_parent(*this);
  if(_asl)
    _asl->_set_parent(*this);
}

stmt_labeled::~stmt_labeled() noexcept
{
  delete &_s;
  delete _asl;
}

_ast_entity* stmt_labeled::_get_child(const size_t i) const noexcept
{
    switch (i)
  {
  case 0:
    return &_s;

  case 1:
    return _asl;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void stmt_labeled::_process(processor<void> &p)
{
  p(*this);
}

void stmt_labeled::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_labeled::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_labeled::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_case::stmt_case(const pp_tokens_range &tr,
		     expr* &&e, stmt* &&s) noexcept
  : stmt(tr), _e(*mv_p(std::move(e))), _s(*mv_p(std::move(s)))
{
  _e._set_parent(*this);
  _s._set_parent(*this);
}

stmt_case::~stmt_case() noexcept
{
  delete &_e;
  delete &_s;
}

_ast_entity* stmt_case::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_e;

  case 1:
    return &_s;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void stmt_case::_process(processor<void> &p)
{
  p(*this);
}

void stmt_case::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_case::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_case::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_case_range::stmt_case_range(const pp_tokens_range &tr,
				 expr* &&e_low, expr* &&e_high, stmt* &&s)
  noexcept
  : stmt(tr), _e_low(*mv_p(std::move(e_low))),
    _e_high(*mv_p(std::move(e_high))), _s(*mv_p(std::move(s)))
{
  _e_low._set_parent(*this);
  _e_high._set_parent(*this);
  _s._set_parent(*this);
}

stmt_case_range::~stmt_case_range() noexcept
{
  delete &_e_low;
  delete &_e_high;
  delete &_s;
}

_ast_entity* stmt_case_range::_get_child(const size_t i) const noexcept
{
    switch (i)
  {
  case 0:
    return &_e_low;

  case 1:
    return &_e_high;

  case 2:
    return &_s;

  case 3:
    return nullptr;
  }

  __builtin_unreachable();
}

void stmt_case_range::_process(processor<void> &p)
{
  p(*this);
}

void stmt_case_range::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_case_range::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_case_range::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_default::stmt_default(const pp_tokens_range &tr, stmt* &&s) noexcept
  : stmt(tr), _s(*mv_p(std::move(s)))
{
  _s._set_parent(*this);
}

stmt_default::~stmt_default() noexcept
{
  delete &_s;
}

_ast_entity* stmt_default::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_s;

  return nullptr;
}

void stmt_default::_process(processor<void> &p)
{
  p(*this);
}

void stmt_default::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_default::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_default::_process(const_processor<bool> &p) const
{
  return p(*this);
}


local_label_declaration::local_label_declaration(const pp_tokens_range &tr,
						 identifier_list* &&idl)
  noexcept
  : ast_entity(tr), _idl(*mv_p(std::move(idl)))
{
  _idl._set_parent(*this);
}

local_label_declaration::~local_label_declaration() noexcept
{
  delete &_idl;
}

_ast_entity* local_label_declaration::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_idl;

  return nullptr;
}

void local_label_declaration::_process(processor<void> &p)
{
  p(*this);
}

void local_label_declaration::_process(const_processor<void> &p) const
{
  p(*this);
}

bool local_label_declaration::_process(processor<bool> &p)
{
  return p(*this);
}

bool local_label_declaration::_process(const_processor<bool> &p) const
{
  return p(*this);
}


local_label_declaration_list::
local_label_declaration_list(local_label_declaration* &&lld)
  : ast_entity(lld->get_tokens_range())
{
  auto _lld = std::ref(*mv_p(std::move(lld)));
  try {
    _llds.push_back(_lld);
  } catch(...) {
    delete &_lld.get();
    throw;
  }
  _lld.get()._set_parent(*this);
}

local_label_declaration_list::~local_label_declaration_list() noexcept
{
  for (auto lld : _llds)
    delete &lld.get();
}

void local_label_declaration_list::extend(local_label_declaration* &&lld)
{
  auto _lld = std::ref(*mv_p(std::move(lld)));
  try {
    _llds.push_back(_lld);
  } catch(...) {
    delete &_lld.get();
    throw;
  }
  _extend_tokens_range(_lld.get().get_tokens_range());
  _lld.get()._set_parent(*this);
}

_ast_entity* local_label_declaration_list::_get_child(const size_t i) const noexcept
{
  if (i >= _llds.size())
    return nullptr;

  return &_llds[i].get();
}

void local_label_declaration_list::_process(processor<void> &p)
{
  p(*this);
}

void local_label_declaration_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool local_label_declaration_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool local_label_declaration_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


block_item::block_item(const pp_tokens_range &tr) noexcept
  : ast_entity(tr)
{}

block_item::~block_item() noexcept = default;


block_item_decl::block_item_decl(declaration* &&d) noexcept
  : block_item(d->get_tokens_range()), _d(*mv_p(std::move(d)))
{
  _d._set_parent(*this);
}

block_item_decl::~block_item_decl() noexcept
{
  delete &_d;
}

_ast_entity* block_item_decl::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_d;

  return nullptr;
}

void block_item_decl::_process(processor<void> &p)
{
  p(*this);
}

void block_item_decl::_process(const_processor<void> &p) const
{
  p(*this);
}

bool block_item_decl::_process(processor<bool> &p)
{
  return p(*this);
}

bool block_item_decl::_process(const_processor<bool> &p) const
{
  return p(*this);
}


block_item_static_assert::
block_item_static_assert(static_assertion* &&sa) noexcept
  : block_item(sa->get_tokens_range()), _sa(*mv_p(std::move(sa)))
{
  _sa._set_parent(*this);
}

block_item_static_assert::~block_item_static_assert() noexcept
{
  delete &_sa;
}

_ast_entity* block_item_static_assert::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_sa;

  return nullptr;
}

void block_item_static_assert::_process(processor<void> &p)
{
  p(*this);
}

void block_item_static_assert::_process(const_processor<void> &p) const
{
  p(*this);
}

bool block_item_static_assert::_process(processor<bool> &p)
{
  return p(*this);
}

bool block_item_static_assert::_process(const_processor<bool> &p) const
{
  return p(*this);
}


block_item_stmt::block_item_stmt(stmt* &&s) noexcept
  : block_item(s->get_tokens_range()), _s(*mv_p(std::move(s)))
{
  _s._set_parent(*this);
}


block_item_stmt::~block_item_stmt() noexcept
{
  delete &_s;
}

_ast_entity* block_item_stmt::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_s;

  return nullptr;
}

void block_item_stmt::_process(processor<void> &p)
{
  p(*this);
}

void block_item_stmt::_process(const_processor<void> &p) const
{
  p(*this);
}

bool block_item_stmt::_process(processor<bool> &p)
{
  return p(*this);
}

bool block_item_stmt::_process(const_processor<bool> &p) const
{
  return p(*this);
}


block_item_function_definition::
block_item_function_definition(function_definition* &&fd) noexcept
  : block_item(fd->get_tokens_range()), _fd(*mv_p(std::move(fd)))
{
  _fd._set_parent(*this);
}

block_item_function_definition::~block_item_function_definition() noexcept
{
  delete &_fd;
}

_ast_entity* block_item_function_definition::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_fd;

  return nullptr;
}

void block_item_function_definition::_process(processor<void> &p)
{
  p(*this);
}

void block_item_function_definition::_process(const_processor<void> &p) const
{
  p(*this);
}

bool block_item_function_definition::_process(processor<bool> &p)
{
  return p(*this);
}

bool block_item_function_definition::_process(const_processor<bool> &p) const
{
  return p(*this);
}


block_item_list::block_item_list(block_item* &&bi)
  : ast_entity(bi->get_tokens_range())
{
  auto _bi = std::ref(*mv_p(std::move(bi)));
  try {
    _bis.push_back(_bi);
  } catch(...) {
    delete &_bi.get();
    throw;
  }
  _bi.get()._set_parent(*this);
}

block_item_list::~block_item_list() noexcept
{
  for (auto bi : _bis)
    delete &bi.get();
}

void block_item_list::extend(block_item* &&bi)
{
  auto _bi = std::ref(*mv_p(std::move(bi)));
  try {
    _bis.push_back(_bi);
  } catch(...) {
    delete &_bi.get();
    throw;
  }
  _extend_tokens_range(_bi.get().get_tokens_range());
  _bi.get()._set_parent(*this);
}

_ast_entity* block_item_list::_get_child(const size_t i) const noexcept
{
  if (i >= _bis.size())
    return nullptr;

  return &_bis[i].get();
}

void block_item_list::_process(processor<void> &p)
{
  p(*this);
}

void block_item_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool block_item_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool block_item_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_compound::stmt_compound(const pp_tokens_range &tr,
			     local_label_declaration_list* &&lldl,
			     block_item_list* &&bil) noexcept
  : stmt(tr), _lldl(mv_p(std::move(lldl))), _bil(mv_p(std::move(bil)))
{
  if (_lldl)
    _lldl->_set_parent(*this);
  if (_bil)
    _bil->_set_parent(*this);
}

stmt_compound::~stmt_compound() noexcept
{
  delete _lldl;
  delete _bil;
}

_ast_entity* stmt_compound::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_lldl)
      return _lldl;
    else
      return _bil;
  } else if (i == 1 && _lldl) {
    return _bil;
  }

  return nullptr;
}

void stmt_compound::_process(processor<void> &p)
{
  p(*this);
}

void stmt_compound::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_compound::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_compound::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_expr::stmt_expr(const pp_tokens_range &tr, expr* &&e) noexcept
  : stmt(tr), _e(mv_p(std::move(e)))
{
  if (_e)
    _e->_set_parent(*this);
}

stmt_expr::~stmt_expr() noexcept
{
  delete _e;
}

_ast_entity* stmt_expr::_get_child(const size_t i) const noexcept
{
  if (!i)
    return _e;

  return nullptr;
}

void stmt_expr::_process(processor<void> &p)
{
  p(*this);
}

void stmt_expr::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_expr::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_expr::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_if::stmt_if(const pp_tokens_range &tr,
		 expr* &&cond, stmt* &&s_true, stmt* &&s_false)
  noexcept
  : stmt(tr), _cond(*mv_p(std::move(cond))),
    _s_true(*mv_p(std::move(s_true))), _s_false(mv_p(std::move(s_false)))
{
  _cond._set_parent(*this);
  _s_true._set_parent(*this);
  if (_s_false)
    _s_false->_set_parent(*this);
}

stmt_if::~stmt_if() noexcept
{
  delete &_cond;
  delete &_s_true;
  delete _s_false;
}

_ast_entity* stmt_if::_get_child(const size_t i) const noexcept
{
    switch (i)
  {
  case 0:
    return &_cond;

  case 1:
    return &_s_true;

  case 2:
    return _s_false;

  case 3:
    return nullptr;
  }

  __builtin_unreachable();
}

void stmt_if::_process(processor<void> &p)
{
  p(*this);
}

void stmt_if::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_if::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_if::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_switch::stmt_switch(const pp_tokens_range &tr, expr* &&e, stmt* &&s)
  noexcept
  : stmt(tr), _e(*mv_p(std::move(e))), _s(*mv_p(std::move(s)))
{
  _e._set_parent(*this);
  _s._set_parent(*this);
}

stmt_switch::~stmt_switch() noexcept
{
  delete &_e;
  delete &_s;
}

_ast_entity* stmt_switch::_get_child(const size_t i) const noexcept
{
    switch (i)
  {
  case 0:
    return &_e;

  case 1:
    return &_s;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void stmt_switch::_process(processor<void> &p)
{
  p(*this);
}

void stmt_switch::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_switch::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_switch::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_while::stmt_while(const pp_tokens_range &tr, expr* &&e, stmt* &&s)
  noexcept
  : stmt(tr), _e(*mv_p(std::move(e))), _s(*mv_p(std::move(s)))
{
  _e._set_parent(*this);
  _s._set_parent(*this);
}

stmt_while::~stmt_while() noexcept
{
  delete &_e;
  delete &_s;
}

_ast_entity* stmt_while::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_e;

  case 1:
    return &_s;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void stmt_while::_process(processor<void> &p)
{
  p(*this);
}

void stmt_while::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_while::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_while::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_do::stmt_do(const pp_tokens_range &tr, expr* &&e, stmt* &&s)
  noexcept
  : stmt(tr), _e(*mv_p(std::move(e))), _s(*mv_p(std::move(s)))
{
  _e._set_parent(*this);
  _s._set_parent(*this);
}

stmt_do::~stmt_do() noexcept
{
  delete &_e;
  delete &_s;
}

_ast_entity* stmt_do::_get_child(const size_t i) const noexcept
{
  switch (i)
  {
  case 0:
    return &_e;

  case 1:
    return &_s;

  case 2:
    return nullptr;
  }

  __builtin_unreachable();
}

void stmt_do::_process(processor<void> &p)
{
  p(*this);
}

void stmt_do::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_do::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_do::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_for::stmt_for(const pp_tokens_range &tr, expr* &&cond, expr* &&next,
		   stmt* &&s) noexcept
  : stmt{tr}, _cond{mv_p(std::move(cond))}, _next{mv_p(std::move(next))},
    _s{*mv_p(std::move(s))}
{
  if (_cond)
    _cond->_set_parent(*this);
  if (_next)
    _next->_set_parent(*this);
  _s._set_parent(*this);
}

stmt_for::~stmt_for() noexcept
{
  delete _cond;
  delete _next;
  delete &_s;
}


stmt_for_init_expr::stmt_for_init_expr(const pp_tokens_range &tr,
				       expr* &&init, expr* &&cond, expr* &&next,
				       stmt* &&s) noexcept
  : stmt_for{
      tr, std::move(cond), std::move(next), std::move(s)
    },
    _init(mv_p(std::move(init)))
{
  if (_init)
    _init->_set_parent(*this);
}

stmt_for_init_expr::~stmt_for_init_expr() noexcept
{
  delete _init;
}

_ast_entity* stmt_for_init_expr::_get_child(const size_t i) const noexcept
{
  std::size_t _i = i;

  if (_init) {
    if (!_i)
      return _init;

    --_i;
  }

  if (_cond) {
    if (!_i)
      return _cond;

    --_i;
  }

  if (_next) {
    if (!_i)
      return _next;

    --_i;
  }

  if (!_i)
    return &_s;

  return nullptr;
}

void stmt_for_init_expr::_process(processor<void> &p)
{
  p(*this);
}

void stmt_for_init_expr::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_for_init_expr::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_for_init_expr::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_for_init_decl::stmt_for_init_decl(const pp_tokens_range &tr,
				       declaration* &&d,
				       expr* &&cond, expr* &&next,
				       stmt* &&s) noexcept
  : stmt_for{
      tr, std::move(cond), std::move(next), std::move(s)
    },
    _d(*mv_p(std::move(d)))
{
  _d._set_parent(*this);
}

stmt_for_init_decl::~stmt_for_init_decl() noexcept
{
  delete &_d;
}

_ast_entity* stmt_for_init_decl::_get_child(const size_t i) const noexcept
{
  if (!i)
      return &_d;

  std::size_t _i = i - 1;
  if (_cond) {
    if (!_i)
      return _cond;

    --_i;
  }

  if (_next) {
    if (!_i)
      return _next;

    --_i;
  }

  if (!_i)
    return &_s;

  return nullptr;
}

void stmt_for_init_decl::_process(processor<void> &p)
{
  p(*this);
}

void stmt_for_init_decl::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_for_init_decl::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_for_init_decl::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_for_init_static_assert::
stmt_for_init_static_assert(const pp_tokens_range &tr,
			    static_assertion* &&sa,
			    expr* &&cond, expr* &&next,
			    stmt* &&s) noexcept
  : stmt_for{
      tr, std::move(cond), std::move(next), std::move(s)
    },
    _sa(*mv_p(std::move(sa)))
{
  _sa._set_parent(*this);
}

stmt_for_init_static_assert::~stmt_for_init_static_assert() noexcept
{
  delete &_sa;
}

_ast_entity* stmt_for_init_static_assert::_get_child(const size_t i)
  const noexcept
{
  if (!i)
      return &_sa;

  std::size_t _i = i - 1;
  if (_cond) {
    if (!_i)
      return _cond;

    --_i;
  }

  if (_next) {
    if (!_i)
      return _next;

    --_i;
  }

  if (!_i)
    return &_s;

  return nullptr;
}

void stmt_for_init_static_assert::_process(processor<void> &p)
{
  p(*this);
}

void stmt_for_init_static_assert::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_for_init_static_assert::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_for_init_static_assert::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_goto::stmt_goto(const pp_tokens_range &tr, expr* &&e) noexcept
  : stmt(tr), _e(*mv_p(std::move(e)))
{
  _e._set_parent(*this);
}

stmt_goto::~stmt_goto() noexcept
{
  delete &_e;
}

_ast_entity* stmt_goto::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_e;

  return nullptr;
}

void stmt_goto::_process(processor<void> &p)
{
  p(*this);
}

void stmt_goto::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_goto::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_goto::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_continue::stmt_continue(const pp_tokens_range &tr) noexcept
  : stmt(tr)
{}

stmt_continue::~stmt_continue() noexcept = default;

_ast_entity* stmt_continue::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void stmt_continue::_process(processor<void> &p)
{
  p(*this);
}

void stmt_continue::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_continue::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_continue::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_break::stmt_break(const pp_tokens_range &tr) noexcept
  : stmt(tr)
{}

stmt_break::~stmt_break() noexcept = default;

_ast_entity* stmt_break::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void stmt_break::_process(processor<void> &p)
{
  p(*this);
}

void stmt_break::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_break::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_break::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_return::stmt_return(const pp_tokens_range &tr, expr* &&e) noexcept
  : stmt(tr), _e(mv_p(std::move(e)))
{
  if (_e)
    _e->_set_parent(*this);
}

stmt_return::~stmt_return() noexcept
{
  delete _e;
}

_ast_entity* stmt_return::_get_child(const size_t i) const noexcept
{
  if (!i)
    return _e;

  return nullptr;
}

void stmt_return::_process(processor<void> &p)
{
  p(*this);
}

void stmt_return::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_return::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_return::_process(const_processor<bool> &p) const
{
  return p(*this);
}


stmt_asm::stmt_asm(asm_directive* &&ad) noexcept
  : stmt(ad->get_tokens_range()), _ad(*mv_p(std::move(ad)))
{
  _ad._set_parent(*this);
}

stmt_asm::~stmt_asm() noexcept
{
  delete &_ad;
}

_ast_entity* stmt_asm::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_ad;

  return nullptr;
}

void stmt_asm::_process(processor<void> &p)
{
  p(*this);
}

void stmt_asm::_process(const_processor<void> &p) const
{
  p(*this);
}

bool stmt_asm::_process(processor<bool> &p)
{
  return p(*this);
}

bool stmt_asm::_process(const_processor<bool> &p) const
{
  return p(*this);
}


asm_qualifier_list::asm_qualifier_list(const pp_tokens_range &tr,
				       const asm_qualifier aq)
  : ast_entity(tr), _aqs(1, aq)
{}

asm_qualifier_list::~asm_qualifier_list() noexcept = default;

void asm_qualifier_list::extend(const pp_tokens_range &tr,
				const asm_qualifier aq)
{
  _aqs.push_back(aq);
  _extend_tokens_range(tr);
}

_ast_entity* asm_qualifier_list::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void asm_qualifier_list::_process(processor<void> &p)
{
  p(*this);
}

void asm_qualifier_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool asm_qualifier_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool asm_qualifier_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


asm_operand_name::asm_operand_name(const pp_tokens_range &tr,
				   const pp_token_index id_tok) noexcept
  : ast_entity(tr), _id_tok(id_tok)
{}

asm_operand_name::~asm_operand_name() noexcept = default;

_ast_entity* asm_operand_name::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void asm_operand_name::_process(processor<void> &p)
{
  p(*this);
}

void asm_operand_name::_process(const_processor<void> &p) const
{
  p(*this);
}

bool asm_operand_name::_process(processor<bool> &p)
{
  return p(*this);
}

bool asm_operand_name::_process(const_processor<bool> &p) const
{
  return p(*this);
}


asm_operand::asm_operand(const pp_tokens_range &tr, asm_operand_name* &&aon,
			 string_literal* &&constaint, expr* &&e)
  noexcept
  : ast_entity(tr), _aon(mv_p(std::move(aon))),
    _constraint(*mv_p(std::move(constaint))), _e(*mv_p(std::move(e)))
{
  if (_aon)
    _aon->_set_parent(*this);
  _constraint._set_parent(*this);
  _e._set_parent(*this);
}

asm_operand::~asm_operand() noexcept
{
  delete _aon;
  delete &_constraint;
  delete &_e;
}

_ast_entity* asm_operand::_get_child(const size_t i) const noexcept
{
  if (!i) {
    if (_aon)
      return _aon;
    else
      return &_constraint;
  } else if (i == 1) {
    if (_aon)
      return &_constraint;
    else
      return &_e;
  } else if (i == 2) {
    if (_aon)
      return &_e;
    else
      return nullptr;
  }

  return nullptr;
}

void asm_operand::_process(processor<void> &p)
{
  p(*this);
}

void asm_operand::_process(const_processor<void> &p) const
{
  p(*this);
}

bool asm_operand::_process(processor<bool> &p)
{
  return p(*this);
}

bool asm_operand::_process(const_processor<bool> &p) const
{
  return p(*this);
}


asm_operand_list::asm_operand_list(asm_operand* &&ao)
  : ast_entity(ao->get_tokens_range())
{
  auto _ao = std::ref(*mv_p(std::move(ao)));
  try {
    _aos.push_back(_ao);
  } catch(...) {
    delete &_ao.get();
    throw;
  }
  _ao.get()._set_parent(*this);
}

asm_operand_list::~asm_operand_list() noexcept
{
  for (auto ao : _aos)
    delete &ao.get();
}

void asm_operand_list::extend(asm_operand* &&ao)
{
  auto _ao = std::ref(*mv_p(std::move(ao)));
  try {
    _aos.push_back(_ao);
  } catch(...) {
    delete &_ao.get();
    throw;
  }
  _extend_tokens_range(_ao.get().get_tokens_range());
  _ao.get()._set_parent(*this);
}

_ast_entity* asm_operand_list::_get_child(const size_t i) const noexcept
{
  if (i >= _aos.size())
    return nullptr;

  return &_aos[i].get();
}

void asm_operand_list::_process(processor<void> &p)
{
  p(*this);
}

void asm_operand_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool asm_operand_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool asm_operand_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


asm_clobber_list::asm_clobber_list(string_literal* &&clobber)
  : ast_entity(clobber->get_tokens_range())
{
  auto _c = std::ref(*mv_p(std::move(clobber)));
  try {
    _clobbers.push_back(_c);
  } catch (...) {
    delete &_c.get();
    throw;
  }
  _c.get()._set_parent(*this);
}

asm_clobber_list::~asm_clobber_list() noexcept
{
  for (auto &c : _clobbers)
    delete &c.get();
}

void asm_clobber_list::extend(string_literal* && clobber)
{
  auto _c = std::ref(*mv_p(std::move(clobber)));
  try {
    _clobbers.push_back(_c);
  } catch (...) {
    delete &_c.get();
    throw;
  }
  _extend_tokens_range(_c.get().get_tokens_range());
  _c.get()._set_parent(*this);
}

_ast_entity* asm_clobber_list::_get_child(const size_t i) const noexcept
{
  if (i >= _clobbers.size())
    return nullptr;

  return &_clobbers[i].get();
}

void asm_clobber_list::_process(processor<void> &p)
{
  p(*this);
}

void asm_clobber_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool asm_clobber_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool asm_clobber_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


asm_jump_to_label_list::asm_jump_to_label_list(const pp_token_index label_tok)
  : ast_entity(pp_tokens_range{label_tok, label_tok + 1}),
    _label_toks(1, label_tok)
{}

asm_jump_to_label_list::~asm_jump_to_label_list() noexcept = default;

void asm_jump_to_label_list::extend(const pp_token_index label_tok)
{
  _label_toks.push_back(label_tok);
  _extend_tokens_range(pp_tokens_range{label_tok, label_tok + 1});
}

_ast_entity* asm_jump_to_label_list::_get_child(const size_t) const noexcept
{
  return nullptr;
}

void asm_jump_to_label_list::_process(processor<void> &p)
{
  p(*this);
}

void asm_jump_to_label_list::_process(const_processor<void> &p) const
{
  p(*this);
}

bool asm_jump_to_label_list::_process(processor<bool> &p)
{
  return p(*this);
}

bool asm_jump_to_label_list::_process(const_processor<bool> &p) const
{
  return p(*this);
}


asm_directive::asm_directive(const pp_tokens_range &tr,
		   asm_qualifier_list* &&aql, string_literal* &&asm_s,
		   asm_operand_list* &&output_aol,
		   asm_operand_list* &&input_aol,
		   asm_clobber_list* &&acl, asm_jump_to_label_list* &&ajtll)
  noexcept
  : ast_entity(tr), _aql(mv_p(std::move(aql))), _asm_s(*mv_p(std::move(asm_s))),
    _output_aol(mv_p(std::move(output_aol))),
    _input_aol(mv_p(std::move(input_aol))), _acl(mv_p(std::move(acl))),
    _ajtll(mv_p(std::move(ajtll)))
{
  if (_aql)
    _aql->_set_parent(*this);
  _asm_s._set_parent(*this);
  if (_output_aol)
    _output_aol->_set_parent(*this);
  if (_input_aol)
    _input_aol->_set_parent(*this);
  if (_acl)
    _acl->_set_parent(*this);
  if (_ajtll)
    _ajtll->_set_parent(*this);
}

asm_directive::~asm_directive() noexcept
{
  delete _aql;
  delete &_asm_s;
  delete _output_aol;
  delete _input_aol;
  delete _acl;
  delete _ajtll;
}

_ast_entity* asm_directive::_get_child(const size_t i) const noexcept
{
  std::size_t _i = i;

  if (_aql) {
    if (!_i)
      return _aql;

    --_i;
  }

  if (!_i)
    return &_asm_s;

  --_i;

  if (_output_aol) {
    if (!_i)
      return _output_aol;

    --_i;
  }

  if (_input_aol) {
    if (!_i)
      return _input_aol;

    --_i;
  }

  if (_acl) {
    if (!_i)
      return _acl;

    --_i;
  }

  if (_ajtll) {
    if (!_i)
      return _ajtll;

    --_i;
  }

  return nullptr;
}

void asm_directive::_process(processor<void> &p)
{
  p(*this);
}

void asm_directive::_process(const_processor<void> &p) const
{
  p(*this);
}

bool asm_directive::_process(processor<bool> &p)
{
  return p(*this);
}

bool asm_directive::_process(const_processor<bool> &p) const
{
  return p(*this);
}


function_definition::function_definition(const pp_tokens_range &tr,
					 declaration_specifiers* &&ds,
					 declarator* &&d,
					 attribute_specifier_list* &&asl,
					 declaration_list* &&dl,
					 stmt_compound* &&sc) noexcept
  : ast_entity(tr), _ds(*mv_p(std::move(ds))), _d(*mv_p(std::move(d))),
    _asl(mv_p(std::move(asl))), _dl(mv_p(std::move(dl))),
    _sc(*mv_p(std::move(sc))),
    _is_gnu_inline(false), _is_always_inline(false)
{
  _ds._set_parent(*this);
  _d._set_parent(*this);
  if (_asl)
    _asl->_set_parent(*this);
  if (_dl)
    _dl->_set_parent(*this);
  _sc._set_parent(*this);
}

function_definition::~function_definition() noexcept
{
  delete &_ds;
  delete &_d;
  delete _asl;
  delete _dl;
  delete &_sc;
}

bool function_definition::is_at_file_scope() const noexcept
{
  return get_parent()->is_any_of<external_declaration_func>();
}

void function_definition::set_is_gnu_inline() noexcept
{
  _is_gnu_inline = true;
}

void function_definition::set_is_always_inline() noexcept
{
  _is_always_inline = true;
}

_ast_entity* function_definition::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_ds;
  else if (i == 1)
    return &_d;

  std::size_t _i = i - 2;
  if (_asl) {
    if (!_i)
      return _asl;

    --_i;
  }

  if (_dl) {
    if (!_i)
      return _dl;

    --_i;
  }

  if (!_i)
    return &_sc;

  return nullptr;
}

void function_definition::_process(processor<void> &p)
{
  p(*this);
}

void function_definition::_process(const_processor<void> &p) const
{
  p(*this);
}

bool function_definition::_process(processor<bool> &p)
{
  return p(*this);
}

bool function_definition::_process(const_processor<bool> &p) const
{
  return p(*this);
}


external_declaration::external_declaration(const pp_tokens_range &tr) noexcept
  : ast_entity(tr)
{}

external_declaration::~external_declaration() noexcept = default;


external_declaration_decl::external_declaration_decl(declaration* &&d) noexcept
  : external_declaration(d->get_tokens_range()), _d(*mv_p(std::move(d)))
{
  _d._set_parent(*this);
}

external_declaration_decl::~external_declaration_decl() noexcept
{
  delete &_d;
}

_ast_entity* external_declaration_decl::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_d;

  return nullptr;
}

void external_declaration_decl::_process(processor<void> &p)
{
  p(*this);
}

void external_declaration_decl::_process(const_processor<void> &p) const
{
  p(*this);
}

bool external_declaration_decl::_process(processor<bool> &p)
{
  return p(*this);
}

bool external_declaration_decl::_process(const_processor<bool> &p) const
{
  return p(*this);
}


external_declaration_static_assert::
external_declaration_static_assert(static_assertion* &&sa) noexcept
  : external_declaration{sa->get_tokens_range()}, _sa{*mv_p(std::move(sa))}
{
  _sa._set_parent(*this);
}

external_declaration_static_assert::
~external_declaration_static_assert() noexcept
{
  delete &_sa;
}

_ast_entity* external_declaration_static_assert::
_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_sa;

  return nullptr;
}

void external_declaration_static_assert::
_process(processor<void> &p)
{
  p(*this);
}

void external_declaration_static_assert::
_process(const_processor<void> &p) const
{
  p(*this);
}

bool external_declaration_static_assert::
_process(processor<bool> &p)
{
  return p(*this);
}

bool external_declaration_static_assert::
_process(const_processor<bool> &p) const
{
  return p(*this);
}


external_declaration_func::
external_declaration_func(function_definition* &&fd) noexcept
  : external_declaration(fd->get_tokens_range()), _fd(*mv_p(std::move(fd)))
{
  _fd._set_parent(*this);
}

external_declaration_func::~external_declaration_func() noexcept
{
  delete &_fd;
}

_ast_entity* external_declaration_func::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_fd;

  return nullptr;
}

void external_declaration_func::_process(processor<void> &p)
{
  p(*this);
}

void external_declaration_func::_process(const_processor<void> &p) const
{
  p(*this);
}

bool external_declaration_func::_process(processor<bool> &p)
{
  return p(*this);
}

bool external_declaration_func::_process(const_processor<bool> &p) const
{
  return p(*this);
}


external_declaration_asm::
external_declaration_asm(asm_directive* &&ad) noexcept
  : external_declaration(ad->get_tokens_range()), _ad(*mv_p(std::move(ad)))
{
  _ad._set_parent(*this);
}

external_declaration_asm::~external_declaration_asm() noexcept
{
  delete &_ad;
}

_ast_entity* external_declaration_asm::_get_child(const size_t i) const noexcept
{
  if (!i)
    return &_ad;

  return nullptr;
}

void external_declaration_asm::_process(processor<void> &p)
{
  p(*this);
}

void external_declaration_asm::_process(const_processor<void> &p) const
{
  p(*this);
}

bool external_declaration_asm::_process(processor<bool> &p)
{
  return p(*this);
}

bool external_declaration_asm::_process(const_processor<bool> &p) const
{
  return p(*this);
}


translation_unit::translation_unit(const pp_tokens_range &r)
  : ast_entity(r)
{}

translation_unit::translation_unit(external_declaration* &&ed)
  : ast_entity(ed->get_tokens_range())
{
  auto _ed = std::ref(*mv_p(std::move(ed)));
  try {
    _eds.push_back(_ed);
  } catch(...) {
    delete &_ed.get();
    throw;
  }
  _ed.get()._set_parent(*this);
}

translation_unit::~translation_unit() noexcept
{
  for (auto ed : _eds)
    delete &ed.get();
}

void translation_unit::extend(external_declaration* &&ed)
{
  auto _ed = std::ref(*mv_p(std::move(ed)));
  try {
    _eds.push_back(_ed);
  } catch(...) {
    delete &_ed.get();
    throw;
  }
  _extend_tokens_range(_ed.get().get_tokens_range());
  _ed.get()._set_parent(*this);
}

void translation_unit::extend_tokens_range(const pp_tokens_range &tr) noexcept
{
  _extend_tokens_range(tr);
}

_ast_entity* translation_unit::_get_child(const size_t i) const noexcept
{
  if (i >= _eds.size())
    return nullptr;

  return &_eds[i].get();
}

void translation_unit::_process(processor<void> &p)
{
  p(*this);
}

void translation_unit::_process(const_processor<void> &p) const
{
  p(*this);
}

bool translation_unit::_process(processor<bool> &p)
{
  return p(*this);
}

bool translation_unit::_process(const_processor<bool> &p) const
{
  return p(*this);
}


ast::ast(const pp_result &pp_result, const bool is_pp_expr)
  : _pp_result(pp_result), _is_pp_expr(is_pp_expr)
{}

ast::ast(ast &&a)
  : _pp_result(a._pp_result), _is_pp_expr(a._is_pp_expr)
{}

ast::~ast() noexcept = default;

const klp::ccp::pp_tokens& ast::get_pp_tokens() const noexcept
{
  return _pp_result.get_pp_tokens();
}


ast_translation_unit::
ast_translation_unit(std::unique_ptr<const pp_result> &&pp_result,
		     std::unique_ptr<translation_unit> &&tu)
  : ast(*pp_result, false), _pp_result(std::move(pp_result)),
    _tu(std::move(tu))
{}

ast_translation_unit::ast_translation_unit(ast_translation_unit &&a)
  : ast(std::move(a)), _pp_result(std::move(a._pp_result)),
    _tu(std::move(a._tu))
{}

ast_translation_unit::~ast_translation_unit() noexcept = default;


ast_pp_expr::ast_pp_expr(const pp_result &pp_result,
			 std::unique_ptr<expr> &&e)
  : ast(pp_result, true), _e(std::move(e))
{}

ast_pp_expr::ast_pp_expr(ast_pp_expr &&a)
  : ast(std::move(a)), _e(std::move(a._e))
{}

ast_pp_expr::~ast_pp_expr() noexcept = default;
