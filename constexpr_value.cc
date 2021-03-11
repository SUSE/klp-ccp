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

#include "constexpr_value.hh"
#include "ast.hh"
#include "types.hh"
#include "pp_token.hh"
#include <cassert>

using namespace klp::ccp;
using namespace klp::ccp::ast;
using namespace klp::ccp::types;

constexpr_value::address_constant::
address_constant()
  : _bk(base_kind::bk_constant), _offset(0), _dereferenced(false)
{}

constexpr_value::address_constant::
address_constant(const target_int &base)
  : _bk(base_kind::bk_constant), _offset(_target_int_to_limbs(base)),
    _dereferenced(false)
{}

constexpr_value::address_constant::
address_constant(const init_declarator &base, const bool dereferenced)
  : _bk(base_kind::bk_init_declarator), _base_id(&base),
    _dereferenced(dereferenced)
{}

constexpr_value::address_constant::
address_constant(const function_definition &base, const bool dereferenced)
  : _bk(base_kind::bk_function_definition), _base_fd(&base),
    _dereferenced(dereferenced)
{}

constexpr_value::address_constant::
address_constant(const stmt_labeled &base, const bool dereferenced)
  : _bk(base_kind::bk_stmt_labeled), _base_stmt_labeled(&base),
    _dereferenced(dereferenced)
{}

constexpr_value::address_constant::
address_constant(const string_literal &base, const bool dereferenced)
  : _bk(base_kind::bk_string_literal), _base_string_literal(&base),
    _dereferenced(dereferenced)
{}

constexpr_value::address_constant::
address_constant(const expr_compound_literal &base,
		 const bool dereferenced)
  : _bk(base_kind::bk_compound_literal), _base_compound_literal(&base),
    _dereferenced(dereferenced)
{}

void constexpr_value::address_constant::undereference() noexcept
{
  assert(_dereferenced);
  _dereferenced = false;
}

void constexpr_value::address_constant::dereference() noexcept
{
  assert(!_dereferenced);
  _dereferenced = true;
}

void constexpr_value::address_constant::
add_to_offset(mpa::limbs &&ls)
{
  // Take ls to be positive. If the sign bit is not zero,
  // enlarge it.
  if (ls.test_bit(ls.width() - 1)) {
    ls.resize(mpa::limbs::width_to_size(ls.width() + 1));
  }

  _offset = _offset.add_signed(ls);
}

void constexpr_value::address_constant::
add_to_offset(bool negate, const target_int &i, const mpa::limbs &fac)
{
  mpa::limbs add = i.get_limbs();
  if (i.is_signed() && i.is_negative()) {
    // Multiplicate positive numbers only.
    negate = !negate;
    add = add.complement();
  }
  add = add * fac;

  // Make sure there's room for a sign bit.
  if (add.test_bit(add.width() - 1)) {
    add.resize(mpa::limbs::width_to_size(add.width()) + 1);
  }

  if (negate)
    add = add.complement();

  _offset = _offset.add_signed(add);
}

bool constexpr_value::address_constant::is_null() const noexcept
{
  return (_bk == base_kind::bk_constant && !_offset);
}

bool constexpr_value::address_constant::
has_same_base_as(const address_constant &ac) const noexcept
{
  switch (_bk) {
  case base_kind::bk_constant:
    return ac._bk == base_kind::bk_constant;

  case base_kind::bk_init_declarator:
    if (ac._bk == base_kind::bk_init_declarator) {
      return _base_id->get_linkage().is_linked_to(*ac._base_id);
    } else if (ac._bk == base_kind::bk_function_definition) {
      return _base_id->get_linkage().is_linked_to(*ac._base_fd);
    } else {
      return false;
    }

  case base_kind::bk_function_definition:
    if (ac._bk == base_kind::bk_init_declarator) {
      return _base_fd->get_linkage().is_linked_to(*ac._base_id);
    } else if (ac._bk == base_kind::bk_function_definition) {
      return _base_fd->get_linkage().is_linked_to(*ac._base_fd);
    } else {
      return false;
    }

  case base_kind::bk_stmt_labeled:
    return (ac._bk == base_kind::bk_stmt_labeled &&
	    _base_stmt_labeled == ac._base_stmt_labeled);

  case base_kind::bk_string_literal:
    return (ac._bk == base_kind::bk_string_literal &&
	    _base_string_literal == ac._base_string_literal);

  case base_kind::bk_compound_literal:
    return (ac._bk == base_kind::bk_compound_literal &&
	    _base_compound_literal == ac._base_compound_literal);
  };
}

mpa::limbs constexpr_value::address_constant::
_target_int_to_limbs(const target_int &ti)
{
  // Make sure there's a sign bit.
  if (ti.is_signed() ||
      ti.min_required_width() < ti.get_limbs().width())
    return ti.get_limbs();

  mpa::limbs ls = ti.get_limbs();
  ls.resize(ls.size() + 1);
  return ls;
}

constexpr_value::constexpr_value(const _constness_type &c, const target_int &ti)
  : _value_kind(value_kind::vk_int), _ti(ti), _c(c)
{}

constexpr_value::constexpr_value(const _constness_type &c,
				 const target_float &tf)
  : _value_kind(value_kind::vk_float), _tf(tf), _c(c)
{}

constexpr_value::constexpr_value(const _constness_type &c,
				 const address_constant &ac)
  : _value_kind(value_kind::vk_address), _ac(ac), _c(c)
{}

constexpr_value::constexpr_value(const integer_constant_expr_tag c_tag,
				 const target_int &ti)
  : constexpr_value(_constness_from_tag(c_tag), ti)
{}

constexpr_value::constexpr_value(const arithmetic_constant_expr_tag c_tag,
				 const target_int &ti)
  : constexpr_value(_constness_from_tag(c_tag), ti)
{}

constexpr_value::constexpr_value(const target_int &ti)
  : constexpr_value(_constness_from_tag(), ti)
{}

constexpr_value::constexpr_value(const literal_fp_constant_tag c_tag,
				 const target_float &tf)
  : constexpr_value(_constness_from_tag(c_tag), tf)
{}

constexpr_value::constexpr_value(const arithmetic_constant_expr_tag c_tag,
				 const target_float &tf)
  : constexpr_value(_constness_from_tag(c_tag), tf)
{}

constexpr_value::constexpr_value(const target_float &tf)
  : constexpr_value(_constness_from_tag(), tf)
{}

constexpr_value::constexpr_value(const address_constant_tag c_tag,
				 const address_constant &ac)
  : constexpr_value(_constness_from_tag(c_tag), ac)
{}

constexpr_value::constexpr_value(const address_constant &ac)
  : constexpr_value(_constness_from_tag(), ac)
{}

constexpr_value::~constexpr_value() noexcept
{
  switch(_value_kind) {
  case value_kind::vk_int:
    _ti.~target_int();
    break;

  case value_kind::vk_float:
    _tf.~target_float();
    break;

  case value_kind::vk_address:
    _ac.~address_constant();
    break;
  }
}

std::unique_ptr<constexpr_value> constexpr_value::clone() const
{
  std::unique_ptr<constexpr_value> c;
  switch (_value_kind) {
  case value_kind::vk_int:
    c.reset(new constexpr_value{_c, _ti});
    break;

  case value_kind::vk_float:
    c.reset(new constexpr_value{_c, _tf});
    break;

  case value_kind::vk_address:
    c.reset(new constexpr_value{_c, _ac});
    break;
  };

  return c;
}

bool constexpr_value::has_constness(const constness c) const noexcept
{
  return _c[_constness_to_size_t(c)];
}

void constexpr_value::drop_constness() noexcept
{
  _c = _constness_from_tag();
}

void constexpr_value::drop_constness(const constness c) noexcept
{
  _c[_constness_to_size_t(c)] = false;
  if (c == constness::c_arithmetic_constant_expr) {
    _c[_constness_to_size_t(constness::c_literal_fp_constant)] = false;
    _c[_constness_to_size_t(constness::c_integer_constant_expr)] = false;
  }
}

const target_int& constexpr_value::get_int_value() const noexcept
{
  assert(_value_kind == value_kind::vk_int);
  return _ti;
}

const target_float& constexpr_value::get_float_value() const noexcept
{
  assert(_value_kind == value_kind::vk_float);
  return _tf;
}

const constexpr_value::address_constant&
constexpr_value::get_address_value() const noexcept
{
  assert(_value_kind == value_kind::vk_address);
  return _ac;
}

constexpr_value::address_constant& constexpr_value::get_address_value() noexcept
{
  assert(_value_kind == value_kind::vk_address);
  return _ac;
}

target_int constexpr_value::convert_to(const target &tgt,
				       const integral_type &it) const
{
  const bool is_signed = it.is_signed(tgt);
  const mpa::limbs::size_type prec = it.get_width(tgt) - is_signed;

  switch(_value_kind) {
  case value_kind::vk_int:
    return _ti.convert(prec, is_signed);

  case value_kind::vk_float:
    return _tf.to_int(prec, is_signed);

  case value_kind::vk_address:
    assert(0);
    __builtin_unreachable();
  };
}

target_float constexpr_value::convert_to(const target &tgt,
					 const types::real_float_type &ft) const
{
  const mpa::limbs::size_type significand_width =
    ft.get_significand_width(tgt);
  const mpa::limbs::size_type exponent_width = ft.get_exponent_width(tgt);

  switch(_value_kind) {
  case value_kind::vk_int:
    return _ti.to_float(significand_width, exponent_width);

  case value_kind::vk_float:
    return _tf.convert(significand_width, exponent_width);

  case value_kind::vk_address:
    assert(0);
    __builtin_unreachable();
  };
}

bool constexpr_value::is_zero() const noexcept
{
  switch(_value_kind) {
  case value_kind::vk_int:
    return !_ti;

  case value_kind::vk_float:
    return _tf.is_zero();

  case value_kind::vk_address:
    return _ac.is_null();
  };
}

constexpr_value::_constness_type constexpr_value::_constness_from_tag() noexcept
{
  return _constness_type{};
}

constexpr_value::_constness_type
constexpr_value::_constness_from_tag(const integer_constant_expr_tag) noexcept
{
  _constness_type c = _constness_from_tag();
  c[_constness_to_size_t(constness::c_integer_constant_expr)] = true;
  c[_constness_to_size_t(constness::c_arithmetic_constant_expr)] = true;
  return c;
}

constexpr_value::_constness_type
constexpr_value::_constness_from_tag(const literal_fp_constant_tag) noexcept
{
  _constness_type c = _constness_from_tag();
  c[_constness_to_size_t(constness::c_literal_fp_constant)] = true;
  c[_constness_to_size_t(constness::c_arithmetic_constant_expr)] = true;
  return c;
}

constexpr_value::_constness_type
constexpr_value::_constness_from_tag(const arithmetic_constant_expr_tag)
  noexcept
{
  _constness_type c = _constness_from_tag();
  c[_constness_to_size_t(constness::c_arithmetic_constant_expr)] = true;
  return c;
}

constexpr_value::_constness_type
constexpr_value::_constness_from_tag(const address_constant_tag) noexcept
{
  _constness_type c = _constness_from_tag();
  c[_constness_to_size_t(constness::c_address_constant)] = true;
  return c;
}

constexpr_value::_constness_type::size_type
constexpr_value::_constness_to_size_t(const constness c) noexcept
{
  return static_cast<constexpr_value::_constness_type::size_type>(c);
}
