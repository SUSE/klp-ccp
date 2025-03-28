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
#include <algorithm>
#include <stdexcept>
#include "target.hh"
#include "ast.hh"
#include "callables_wrapper.hh"
#include "constexpr_value.hh"
#include "pp_token.hh"

using namespace klp::ccp;
using namespace klp::ccp::types;


// Explicit instantiation is needed by clang++.
template class std::shared_ptr<const types::struct_or_union_type>;


qualifiers::qualifiers() noexcept
  : _qs{}
{}

qualifiers::qualifiers(const std::initializer_list<qualifier> &il) noexcept
  : qualifiers{}
{
  for (qualifier q : il) {
    _qs[static_cast<std::size_t>(q)] = true;
  }
}

bool qualifiers::operator==(const qualifiers &qs) const noexcept
{
  return _qs == qs._qs;
}

void qualifiers::add(const qualifier q)
{
  _qs[static_cast<std::size_t>(q)] = true;
}

void qualifiers::add(const qualifiers qs)
{
  auto new_it = qs._qs.cbegin();

  for (auto my_it = _qs.begin(); my_it != _qs.end(); ++my_it, ++new_it)
    if (*new_it)
      *my_it = true;
}

bool qualifiers::any() const noexcept
{
  return std::any_of(_qs.cbegin(), _qs.cend(),
		     [](const bool enabled) { return enabled; });
}

bool qualifiers::is_subset_of(const qualifiers &qs) const noexcept
{
  auto it = qs._qs.cbegin();

  for (auto my_it = _qs.begin(); my_it != _qs.end(); ++my_it, ++it)
    if (*my_it && !*it)
      return false;

  return true;
}


alignment::alignment() noexcept
  : _align_ffs(0)
{}

alignment::alignment(const mpa::limbs::size_type log2_value) noexcept
  : _align_ffs(log2_value + 1)
{}

alignment& alignment::operator=(const mpa::limbs::size_type log2_value)
  noexcept
{
  _align_ffs = log2_value + 1;
  return *this;
}

bool alignment::operator==(const alignment &rhs) const noexcept
{
  return this->_align_ffs == rhs._align_ffs;
}

mpa::limbs::size_type alignment::get_log2_value() const noexcept
{
  assert(is_set());
  return _align_ffs - 1;
}

const alignment& alignment::max(const alignment &a1, const alignment &a2)
  noexcept
{
  if (!a2.is_set())
    return a1;
  else if (!a1.is_set())
    return a2;

  if (a1._align_ffs < a2._align_ffs)
    return a2;

  return a1;
}


template<typename self_type>
static std::shared_ptr<const self_type>
_strip_qualifiers(std::shared_ptr<const self_type> &&self,
		  self_type* (self_type::*clone)() const)
{
  if (!self->get_qualifiers().any())
    return std::move(self);

  std::shared_ptr<self_type> new_type{(self.get()->*clone)()};
  new_type->get_qualifiers() = qualifiers{};
  return new_type;
}


type::type() = default;

type::type(const qualifiers &qs)
  : _qs(qs)
{}

type::type(const type&) = default;

type::~type() noexcept = default;

bool type::is_compatible_with(const target&, const void_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const plain_char_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const bool_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const std_int_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const ext_int_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const enum_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const std_float_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const ext_float_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const complex_float_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const struct_or_union_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const array_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&,
			      const prototyped_function_type&, const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&,
			      const unprototyped_function_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const pointer_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const bitfield_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const target&, const builtin_func_type&,
			      const bool) const
{
  return false;
}

std::shared_ptr<const type> type::amend_qualifiers(const qualifiers &qs) const
{
  std::shared_ptr<type> new_t(this->_clone());
  new_t->_amend_qualifiers(qs);
  return new_t;
}

std::shared_ptr<const type> type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<type>(), &type::_clone);
}

void type::_amend_qualifiers(const qualifiers &qs)
{
  _qs.add(qs);
}


template<typename self_type>
static std::shared_ptr<const self_type>
_set_user_alignment(std::shared_ptr<const self_type> &&self,
		    const alignment &user_align,
		    self_type* (self_type::*clone)() const)
{
  std::shared_ptr<self_type> new_type{(self.get()->*clone)()};
  new_type->get_user_alignment() = user_align;
  return new_type;
}

addressable_type::addressable_type() = default;

addressable_type::addressable_type(const alignment &user_align)
  : _user_align(user_align)
{}

addressable_type::addressable_type(const addressable_type&) = default;

addressable_type::~addressable_type() noexcept = default;

std::shared_ptr<const pointer_type>
addressable_type::derive_pointer(const qualifiers &qs,
				 const alignment &user_align) const
{
  auto self = _self_ptr<addressable_type>();
  return (std::shared_ptr<const pointer_type>
	  (new pointer_type(std::move(self), qs, user_align)));
}

std::shared_ptr<const addressable_type>
addressable_type::amend_qualifiers(const qualifiers &qs) const
{
  std::shared_ptr<addressable_type> new_t(this->_clone());
  new_t->_amend_qualifiers(qs);
  return new_t;
}

std::shared_ptr<const addressable_type>
addressable_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<addressable_type>(),
			   &addressable_type::_clone);
}

std::shared_ptr<const addressable_type>
addressable_type::set_user_alignment(const alignment &user_align) const
{
  return _set_user_alignment(_self_ptr<addressable_type>(),
			     user_align, &addressable_type::_clone);
}

mpa::limbs::size_type
addressable_type::get_effective_alignment(const target &tgt)
  const noexcept
{
  if (_user_align.is_set())
    return _user_align.get_log2_value();

  return get_type_alignment(tgt);
}

std::shared_ptr<const addressable_type>
addressable_type::construct_composite(const target &tgt,
				      const addressable_type& prev_type) const
{
  // Function parameters' qualifiers are ignored in
  // prototyped_function_type::is_compatible_with(). So there can be a
  // difference in case we're currently construction a parameter's
  // composite type.
  assert(this->is_compatible_with(tgt, prev_type, true));
  // The default is to use the original, unmodified type.
  return nullptr;
}

std::shared_ptr<const array_type>
addressable_type::_construct_composite(const target&,
				       const array_type&) const
{
  // The default is to use the original, unmodified type.
  return nullptr;
}

std::shared_ptr<const prototyped_function_type> addressable_type::
_construct_composite(const target&,
		     const unprototyped_function_type&) const
{
  // The default is to use the original, unmodified type.
  return nullptr;
}

std::shared_ptr<const prototyped_function_type> addressable_type::
_construct_composite(const target&,
		     const prototyped_function_type&) const
{
  // The default is to use the original, unmodified type.
  return nullptr;
}

std::shared_ptr<const pointer_type>
addressable_type::_construct_composite(const target&,
				       const pointer_type&) const
{
  // The default is to use the original, unmodified type.
  return nullptr;
}



function_type::function_type(std::shared_ptr<const returnable_type> &&rt)
  : _return_type(std::move(rt))
{}

function_type::function_type(const function_type&) = default;

function_type::~function_type() noexcept = default;

bool function_type::is_complete() const noexcept
{
  // GCC extension: sizeof and pointer arithmetic works on functions.
  return true;
}

bool function_type::is_size_constant() const noexcept
{
  // GCC extension: sizeof and pointer arithmetic works on functions.
  return true;
}

mpa::limbs function_type::get_size(const target&) const
{
  // GCC extension: sizeof(void(void)) is 1.
  return mpa::limbs::from_size_type(1);
}

mpa::limbs::size_type
function_type::get_type_alignment(const target&) const noexcept
{
  // GCC extension: __alignof__(void(void)) is 1.
  return 0;
}

void function_type::_amend_qualifiers(const qualifiers &qs)
{
  // Application of qualifiers to function type is undefined.
  // GCC simply discards them.
}


object_type::object_type() = default;

object_type::object_type(const object_type &) = default;

object_type::~object_type() noexcept = default;

std::shared_ptr<const object_type>
object_type::amend_qualifiers(const qualifiers &qs) const
{
  std::shared_ptr<object_type> new_t(this->_clone());
  new_t->_amend_qualifiers(qs);
  return new_t;
}

std::shared_ptr<const object_type> object_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<object_type>(),
			   &object_type::_clone);
}

std::shared_ptr<const object_type>
object_type::set_user_alignment(const alignment &user_align) const
{
  return _set_user_alignment(_self_ptr<object_type>(),
			     user_align, &object_type::_clone);

}

std::shared_ptr<const array_type>
object_type::derive_array(const ast::expr * const length_expr) const
{
  std::shared_ptr<const object_type> self = _self_ptr<object_type>();
  return (std::shared_ptr<const array_type>
	  (new array_type(std::move(self), length_expr)));
}

std::shared_ptr<const array_type>
object_type::derive_array(mpa::limbs &&initializer_length) const
{
  std::shared_ptr<const object_type> self = _self_ptr<object_type>();
  return (std::shared_ptr<const array_type>
	  (new array_type(std::move(self), std::move(initializer_length))));
}

std::shared_ptr<const array_type>
object_type::derive_array(const bool unspec_vla) const
{
  std::shared_ptr<const object_type> self = _self_ptr<object_type>();
  return (std::shared_ptr<const array_type>
	  (new array_type(std::move(self), unspec_vla)));
}

std::shared_ptr<const object_type>
object_type::_construct_composite(const target &tgt,
				  const object_type& prev_type) const
{
  assert(this->is_compatible_with(tgt, prev_type, false));
  // The default is to use the original, unmodified type.
  return nullptr;
}



returnable_type::returnable_type() = default;

returnable_type::returnable_type(const returnable_type&) = default;

returnable_type::~returnable_type() noexcept = default;

std::shared_ptr<const prototyped_function_type>
returnable_type::derive_function(parameter_type_list &&ptl,
				 const bool variadic) const
{
  std::shared_ptr<const returnable_type> self = _self_ptr<returnable_type>();
  return (std::shared_ptr<const prototyped_function_type>
	  (new prototyped_function_type
	   (std::move(self), std::move(ptl), variadic)));
}

std::shared_ptr<const unprototyped_function_type>
returnable_type::derive_function(const bool from_definition) const
{
  std::shared_ptr<const returnable_type> self = _self_ptr<returnable_type>();
  return (std::shared_ptr<const unprototyped_function_type>
	  (new unprototyped_function_type
	   (std::move(self), from_definition)));
}

std::shared_ptr<const unprototyped_function_type>
returnable_type::derive_function(const std::size_t n_args) const
{
  std::shared_ptr<const returnable_type> self = _self_ptr<returnable_type>();
  return (std::shared_ptr<const unprototyped_function_type>
	  (new unprototyped_function_type
	   (std::move(self), n_args)));
}

std::shared_ptr<const returnable_type>
returnable_type::_construct_composite(const target &tgt,
				      const returnable_type& prev_type) const
{
  assert(this->is_compatible_with(tgt, prev_type, false));
  // The default is to use the original, unmodified type.
  return nullptr;
}


returnable_object_type::returnable_object_type() = default;

returnable_object_type::returnable_object_type(const returnable_object_type&)
= default;

returnable_object_type::~returnable_object_type() noexcept = default;


bool returnable_object_type::is_complete() const noexcept
{
  return true;
}

bool returnable_object_type::is_size_constant() const noexcept
{
  assert(this->is_complete());
  return true;
}


prototyped_function_type::
prototyped_function_type(std::shared_ptr<const returnable_type> &&rt,
			 parameter_type_list &&ptl,
			 const bool variadic)
  : function_type(std::move(rt)), _ptl(std::move(ptl)), _variadic(variadic)
{}

prototyped_function_type::
prototyped_function_type(const prototyped_function_type&) = default;

prototyped_function_type::~prototyped_function_type() noexcept = default;

prototyped_function_type* prototyped_function_type::_clone() const
{
  return new prototyped_function_type(*this);
}

type::type_id prototyped_function_type::get_type_id() const noexcept
{
  return type_id::tid_prototyped_function;
}

bool prototyped_function_type::is_compatible_with(const target &tgt,
						  const type &t,
						  const bool ignore_qualifiers)
  const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool prototyped_function_type::
is_compatible_with(const target &tgt,
		   const prototyped_function_type &t,
		   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (!this->get_return_type()->is_compatible_with(tgt, *t.get_return_type(),
						   false)) {
    return false;
  }

  if (_ptl.size() != t._ptl.size() ||
      _variadic != t._variadic) {
    return false;
  }

  for (auto it1 = _ptl.begin(), it2 = t._ptl.begin(); it1 != _ptl.end();
       ++it1, ++it2) {
    if (!(*it1)->is_compatible_with(tgt, **it2, true))
      return false;
  }

  return true;
}

bool prototyped_function_type::
is_compatible_with(const target &tgt,
		   const unprototyped_function_type &t,
		   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

std::shared_ptr<const addressable_type> prototyped_function_type::
construct_composite(const target &tgt,
		    const addressable_type& prev_type) const
{
  assert(this->is_compatible_with(tgt, prev_type, false));
  return prev_type._construct_composite(tgt, *this);
}

std::shared_ptr<const prototyped_function_type> prototyped_function_type::
_construct_composite(const target &tgt,
		     const unprototyped_function_type& next_type)
  const
{
  return this->_self_ptr<prototyped_function_type>();
}

std::shared_ptr<const prototyped_function_type> prototyped_function_type::
_construct_composite(const target &tgt,
		     const prototyped_function_type& next_type)
  const
{
  const parameter_type_list &prev_ptl = this->_ptl;
  const parameter_type_list &next_ptl = next_type._ptl;
  assert(prev_ptl.size() == next_ptl.size());
  assert(this->_variadic == next_type._variadic);
  const parameter_type_list::size_type n = next_ptl.size();

  parameter_type_list comp_ptl;
  comp_ptl.reserve(n);

  bool ptl_unchanged = true;
  for (parameter_type_list::size_type i = 0; i < n; ++i) {
    std::shared_ptr<const addressable_type> comp_param_type =
      next_ptl[i]->construct_composite(tgt, *prev_ptl[i]);
    if (!comp_param_type)
      comp_param_type = next_ptl[i];
    else
      ptl_unchanged = false;

    comp_ptl.push_back(std::move(comp_param_type));
  }

  std::shared_ptr<const returnable_type> comp_ret_type
    = (next_type.get_return_type()
       ->_construct_composite(tgt, *this->get_return_type()));
  if (!comp_ret_type) {
    if (ptl_unchanged) {
      // Type composition did not alter the type.
      return nullptr;
    }

    // Composition did not alter the return type.
    // Set it to the original for the code below
    comp_ret_type = next_type.get_return_type();

  } else if (comp_ret_type == this->get_return_type()) {
    // The composite return type equals the one from the previous
    // declaration. Compare the ptl against the one from the previous
    // declaration.
    ptl_unchanged = true;
    for (parameter_type_list::size_type i = 0; i < n; ++i) {
      if (comp_ptl[i] != this->_ptl[i]) {
	ptl_unchanged = false;
	break;
      }
    }

    if (ptl_unchanged)
      return this->_self_ptr<prototyped_function_type>();
  }

  // Ok, the composite type is neither of the two, create a new one
  return (std::shared_ptr<const prototyped_function_type>
	  (new prototyped_function_type
	   (std::move(comp_ret_type), std::move(comp_ptl), _variadic)));
}


unprototyped_function_type::
unprototyped_function_type(std::shared_ptr<const returnable_type> &&rt,
			   const bool from_definition)
  : function_type(std::move(rt)), _n_args(0), _from_definition(from_definition)
{}

unprototyped_function_type::
unprototyped_function_type(std::shared_ptr<const returnable_type> &&rt,
			   const std::size_t n_args)
  : function_type(std::move(rt)), _n_args(n_args), _from_definition(true)
{}

unprototyped_function_type::
unprototyped_function_type(const unprototyped_function_type&) = default;

unprototyped_function_type::~unprototyped_function_type() noexcept = default;

unprototyped_function_type* unprototyped_function_type::_clone() const
{
  return new unprototyped_function_type(*this);
}

type::type_id unprototyped_function_type::get_type_id() const noexcept
{
  return type_id::tid_unprototyped_function;
}

bool unprototyped_function_type::
is_compatible_with(const target &tgt, const type &t,
		   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool unprototyped_function_type::
is_compatible_with(const target &tgt,
		   const unprototyped_function_type &t,
		   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (!this->get_return_type()->is_compatible_with(tgt, *t.get_return_type(),
						   false)) {
    return false;
  }

  return true;
}

bool unprototyped_function_type::
is_compatible_with(const target &tgt,
		   const prototyped_function_type &t,
		   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (!this->get_return_type()->is_compatible_with(tgt, *t.get_return_type(),
						   false)) {
    return false;
  }

  if (t.is_variadic()) {
    return false;
  }

  if (_from_definition &&
      _n_args != t.get_parameter_type_list().size()) {
    return false;
  }

  for (auto pt : t.get_parameter_type_list()) {
    if (!(handle_types<bool>
	  ((wrap_callables<default_action_return_value<bool, true>::type>
	    ([&](const int_type &it) {
	       return it.is_compatible_with(tgt, *it.promote(tgt), true);
	     },
	     [&](const real_float_type &rft) {
	       return rft.is_compatible_with(tgt, *rft.promote(tgt), true);
	     },
	     [&](const complex_float_type &cft) {
	       return cft.is_compatible_with(tgt, *cft.promote(tgt), true);
	     })),
	   *pt))) {
      return false;
    }
  }

  return true;
}

std::size_t unprototyped_function_type::get_n_args() const noexcept
{
  assert(_from_definition);
  return _n_args;
}

std::shared_ptr<const addressable_type> unprototyped_function_type::
construct_composite(const target &tgt,
		    const addressable_type& prev_type) const
{
  assert(this->is_compatible_with(tgt, prev_type, false));
  return prev_type._construct_composite(tgt, *this);
}

std::shared_ptr<const prototyped_function_type> unprototyped_function_type::
_construct_composite(const target &tgt,
		     const prototyped_function_type& next_type) const
{
  // This reverses next_type and prev_type, but that doesn't matter.
  return next_type._construct_composite(tgt, *this);
}



array_type::array_type(std::shared_ptr<const object_type> &&element_type,
		       const ast::expr * const length_expr,
		       const alignment &user_align)
  : addressable_type(user_align), _element_type(std::move(element_type)),
    _length_expr(length_expr), _unspec_vla(false)
{}

array_type::array_type(std::shared_ptr<const object_type> &&element_type,
		       mpa::limbs &&initializer_length,
		       const alignment &user_align)
  : addressable_type(user_align), _element_type(std::move(element_type)),
    _length_expr(nullptr), _initializer_length(std::move(initializer_length)),
    _unspec_vla(false)
{}

array_type::array_type(std::shared_ptr<const object_type> &&element_type,
		       const bool unspec_vla,
		       const alignment &user_align)
  : addressable_type(user_align), _element_type(std::move(element_type)),
    _length_expr(nullptr), _unspec_vla(unspec_vla)
{}

array_type::array_type(const array_type&) = default;

array_type::~array_type() noexcept = default;

array_type* array_type::_clone() const
{
  return new array_type(*this);
}

type::type_id array_type::get_type_id() const noexcept
{
  return type_id::tid_array;
}

bool array_type::is_compatible_with(const target &tgt, const type &t,
				    const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool array_type::is_compatible_with(const target &tgt,
				    const array_type &t,
				    const bool ignore_qualifiers) const
{
  if (!_element_type->is_compatible_with(tgt, *t._element_type,
					 ignore_qualifiers)) {
    return false;
  }

  if (!this->is_complete() || !t.is_complete())
    return true;

  if (!this->is_length_constant() || !t.is_length_constant())
    return true;

  return this->get_length() == t.get_length();
}

bool array_type::is_complete() const noexcept
{
  if (!_element_type->is_complete())
    return false;
  return _length_expr || !_initializer_length.empty() || _unspec_vla;
}

bool array_type::is_size_constant() const noexcept
{
  assert(is_complete());

  if (!_element_type->is_size_constant())
    return false;

  if (!is_length_constant())
    return false;

  return true;
}

mpa::limbs array_type::get_size(const target &tgt) const
{
  assert(is_size_constant());
  const mpa::limbs &length = get_length();
  mpa::limbs size = length * _element_type->get_size(tgt);
  return size;
}

mpa::limbs::size_type
array_type::get_type_alignment(const target &tgt) const noexcept
{
  return _element_type->get_effective_alignment(tgt);
}

std::shared_ptr<const array_type>
array_type::set_user_alignment(const alignment &user_align) const
{
  return _set_user_alignment(_self_ptr<array_type>(),
			     user_align, &array_type::_clone);
}

bool array_type::is_length_constant() const noexcept
{
  if (_unspec_vla)
    return false;

  if (!_length_expr)
    return !_initializer_length.empty();

  if (!_length_expr->is_constexpr())
      return false;

  const ast::constexpr_value &length_expr_cv =
    _length_expr->get_constexpr_value();
  return (length_expr_cv.has_constness
	  (ast::constexpr_value::constness::c_integer_constant_expr));
}


const mpa::limbs& array_type::get_length() const noexcept
{
  assert(is_length_constant());
  if (_length_expr) {
    const ast::constexpr_value &length_expr_cv =
      _length_expr->get_constexpr_value();
    assert(length_expr_cv.get_value_kind() ==
	   ast::constexpr_value::value_kind::vk_int);
    assert(!length_expr_cv.get_int_value().is_negative());
    return length_expr_cv.get_int_value().get_limbs();
  }

  return _initializer_length;
}

std::shared_ptr<const array_type>
array_type::set_length_from_initializer(mpa::limbs &&l) const
{
  assert(!(_length_expr || !_initializer_length.empty() || _unspec_vla));
  return (std::shared_ptr<const array_type>
	  (new array_type
	   (std::shared_ptr<const object_type>{_element_type},
	    std::move(l))));
}

void array_type::_amend_qualifiers(const qualifiers &qs)
{
  // For array types, the qualifiers apply to the elements, not to the
  // array type itself.
  _element_type = _element_type->amend_qualifiers(qs);
}

std::shared_ptr<const addressable_type>
array_type::construct_composite(const target &tgt,
				const addressable_type& prev_type) const
{
  assert(this->is_compatible_with(tgt, prev_type, false));
  return prev_type._construct_composite(tgt, *this);
}

std::shared_ptr<const object_type>
array_type::_construct_composite(const target &tgt,
				 const object_type& prev_type) const
{
  assert(this->is_compatible_with(tgt, prev_type, false));
  return prev_type.addressable_type::_construct_composite(tgt, *this);
}

std::shared_ptr<const array_type>
array_type::_construct_composite(const target &tgt,
				 const array_type& next_type) const
{
  std::shared_ptr<const object_type> comp_element_type
    = next_type._element_type->_construct_composite(tgt, *_element_type);
  if (!comp_element_type)
    comp_element_type = next_type._element_type;

  const alignment &comp_user_align =
    alignment::max(this->get_user_alignment(), next_type.get_user_alignment());

  // If one of the two array's has known constant length, the
  // resulting composite array type has that length.
  if (next_type.is_length_constant()) {
    // Try to return the original type, if type composition doesn't
    // alter it.
    if (comp_element_type == next_type._element_type) {
      if (comp_user_align == next_type.get_user_alignment()) {
	// Signal that type composition does not modify the type.
	return nullptr;
      } else {
	return next_type.set_user_alignment(comp_user_align);
      }
    } else {
      return (std::shared_ptr<const array_type>
	      (new array_type(std::move(comp_element_type),
			      mpa::limbs{next_type.get_length()},
			      comp_user_align)));
    }
  } else if (this->is_length_constant()) {
    if (comp_element_type == this->_element_type) {
      if (comp_user_align == this->get_user_alignment()) {
	return this->_self_ptr<array_type>();
      } else {
	return this->set_user_alignment(comp_user_align);
      }
    } else {
      return (std::shared_ptr<const array_type>
	      (new array_type(std::move(comp_element_type),
			      mpa::limbs{this->get_length()},
			      comp_user_align)));
    }
  }

  // Non-constant array length. Return the array type with a size
  // expression, if any.
  if (next_type._length_expr) {
    // Try to return the original type, if type composition doesn't
    // alter it.
    if (comp_element_type == next_type._element_type) {
      if (comp_user_align == next_type.get_user_alignment()) {
	// Signal that type composition does not modify the type.
	return nullptr;
      } else {
	return next_type.set_user_alignment(comp_user_align);
      }
    } else {
      return (std::shared_ptr<const array_type>
	      (new array_type(std::move(comp_element_type),
			      next_type._length_expr,
			      comp_user_align)));
    }
  } else if (this->_length_expr) {
    if (comp_element_type == this->_element_type) {
      if (comp_user_align == this->get_user_alignment()) {
	return this->_self_ptr<array_type>();
      } else {
	return this->set_user_alignment(comp_user_align);
      }
    } else {
      return (std::shared_ptr<const array_type>
	      (new array_type(std::move(comp_element_type),
			      this->_length_expr,
			      comp_user_align)));
    }
  }

  // No length expression. Return an array type with _unspec_vla
  // set if any of the two array types has it set.
  if (next_type._unspec_vla) {
    // Try to return the original type, if type composition doesn't
    // alter it.
    if (comp_element_type == next_type._element_type) {
      if (comp_user_align == next_type.get_user_alignment()) {
	// Signal that type composition does not modify the type.
	return nullptr;
      } else {
	return next_type.set_user_alignment(comp_user_align);
      }
    } else {
      return (std::shared_ptr<const array_type>
	      (new array_type(std::move(comp_element_type), true,
			      comp_user_align)));
    }
  } else if (this->_unspec_vla) {
    if (comp_element_type == this->_element_type) {
      if (comp_user_align == this->get_user_alignment()) {
	return this->_self_ptr<array_type>();
      } else {
	return this->set_user_alignment(comp_user_align);
      }
    } else {
      return (std::shared_ptr<const array_type>
	      (new array_type(std::move(comp_element_type), true,
			      comp_user_align)));
    }
  }

  // Type composition does not alter the original type.
  if (comp_user_align != next_type.get_user_alignment())
    return next_type.set_user_alignment(comp_user_align);
  return nullptr;
}



void_type::void_type(const qualifiers &qs)
  : type(qs)
{}

void_type::void_type(const void_type&) = default;

void_type::~void_type() noexcept = default;

void_type* void_type::_clone() const
{
  return new void_type(*this);
}

std::shared_ptr<const void_type> void_type::create(const qualifiers &qs)
{
  return std::shared_ptr<const void_type>(new void_type(qs));
}

type::type_id void_type::get_type_id() const noexcept
{
  return type_id::tid_void;
}

bool void_type::is_compatible_with(const target &tgt, const type &t,
				   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool void_type::is_compatible_with(const target&, const void_type &t,
				   const bool ignore_qualifiers) const
{
  return ignore_qualifiers || (this->get_qualifiers() == t.get_qualifiers());
}

bool void_type::is_complete() const noexcept
{
  // GCC extension: sizeof and pointer arithmetic works on void.
  return false;
}

bool void_type::is_size_constant() const noexcept
{
  // GCC extension: sizeof and pointer arithmetic works on void.
  return true;
}

mpa::limbs void_type::get_size(const target&) const
{
  // GCC extension: sizeof(void) is 1.
  return mpa::limbs::from_size_type(1);
}

mpa::limbs::size_type
void_type::get_type_alignment(const target&) const noexcept
{
  // GCC extension: __alignof__(void) is 1.
  return 0;
}


pointer_type::
pointer_type(std::shared_ptr<const addressable_type> &&pointed_to_type,
	     const qualifiers &qs, const alignment &user_align)
  : type(qs), addressable_type(user_align),
    _pointed_to_type(std::move(pointed_to_type))
{}

pointer_type::pointer_type(const pointer_type&) = default;

pointer_type::~pointer_type() noexcept = default;

pointer_type* pointer_type::_clone() const
{
  return new pointer_type(*this);
}

type::type_id pointer_type::get_type_id() const noexcept
{
  return type_id::tid_pointer;
}

bool pointer_type::is_compatible_with(const target &tgt,
				      const type &t,
				      const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool pointer_type::is_compatible_with(const target &tgt,
				      const pointer_type &t,
				      const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  return _pointed_to_type->is_compatible_with(tgt, *t._pointed_to_type,
					      ignore_qualifiers);
}

mpa::limbs pointer_type::get_size(const target &tgt) const
{
  return tgt.get_pointer_size();
}

mpa::limbs::size_type
pointer_type::get_type_alignment(const target &tgt) const noexcept
{
  return tgt.get_pointer_alignment();
}

std::shared_ptr<const pointer_type>
pointer_type::set_user_alignment(const alignment &user_align) const
{
   return _set_user_alignment(_self_ptr<pointer_type>(),
			      user_align, &pointer_type::_clone);
}

std::shared_ptr<const addressable_type>
pointer_type::construct_composite(const target &tgt,
				  const addressable_type& prev_type) const
{
  // Function parameters' qualifiers are ignored in
  // prototyped_function_type::is_compatible_with(). So there can be a
  // difference in case we're currently construction a parameter's
  // composite type.
  assert(this->is_compatible_with(tgt, prev_type, true));
  return prev_type._construct_composite(tgt, *this);
}

std::shared_ptr<const object_type>
pointer_type::_construct_composite(const target &tgt,
				   const object_type& prev_type) const
{
  assert(this->is_compatible_with(tgt, prev_type, false));
  return prev_type.addressable_type::_construct_composite(tgt, *this);
}

std::shared_ptr<const returnable_type>
pointer_type::_construct_composite(const target &tgt,
				   const returnable_type& prev_type) const
{
  assert(this->is_compatible_with(tgt, prev_type, false));
  return prev_type.addressable_type::_construct_composite(tgt, *this);
}

std::shared_ptr<const pointer_type>
pointer_type::_construct_composite(const target &tgt,
				   const pointer_type& next_type) const
{
  std::shared_ptr<const addressable_type> comp_pointed_to_type =
    (next_type._pointed_to_type->construct_composite
     (tgt, *this->_pointed_to_type));

  const alignment &comp_user_align =
    alignment::max(this->get_user_alignment(), next_type.get_user_alignment());

  if (!comp_pointed_to_type) {
    if (comp_user_align == next_type.get_user_alignment()) {
      // Type composition does not alter the type
      return nullptr;
    } else {
      return next_type.set_user_alignment(comp_user_align);
    }
  }

  if (comp_pointed_to_type == this->_pointed_to_type) {
    if (comp_user_align == this->get_user_alignment()) {
      // The composite type comes unmodified from the previous declaration.
      return this->_self_ptr<pointer_type>();
    } else {
      return this->set_user_alignment(comp_user_align);
    }
  }

  return (std::shared_ptr<const pointer_type>
	  (new pointer_type(std::move(comp_pointed_to_type), get_qualifiers(),
			    comp_user_align)));
}

struct_or_union_content::member::
member(const std::string &name, const std::shared_ptr<const type> &type,
       mpa::limbs &&offset, mpa::limbs &&bitpos)
  : _name(name), _type(type),
    _offset(std::move(offset)), _bitpos(std::move(bitpos)),
    _is_unnamed_sou(false)
{
  assert(!_offset.empty());
}

struct_or_union_content::member::
member(const std::string &name, const std::shared_ptr<const type> &type)
  : _name(name), _type(type), _is_unnamed_sou(false)
{}

struct_or_union_content::member::
member(const std::shared_ptr<const struct_or_union_type> &sou_type,
       mpa::limbs &&offset)
  : _sou_type(sou_type), _offset(std::move(offset)), _is_unnamed_sou(true)
{
  assert(!_offset.empty());
}

struct_or_union_content::member::
member(const std::shared_ptr<const struct_or_union_type> &sou_type)
  : _sou_type(sou_type), _is_unnamed_sou(true)
{}

struct_or_union_content::member::member(member &&m)
  : _name(std::move(m._name)), _offset(std::move(m._offset)),
    _bitpos(std::move(m._bitpos)), _is_unnamed_sou(m._is_unnamed_sou)
{
  if (!_is_unnamed_sou) {
    new (&_type) std::shared_ptr<const type>(std::move(m._type));
  } else {
    new (&_sou_type)
      std::shared_ptr<const struct_or_union_type>(std::move(m._sou_type));
  }
}

struct_or_union_content::member::~member() noexcept
{
  if (!_is_unnamed_sou) {
    _type.~shared_ptr<const type>();
  } else {
    _sou_type.~shared_ptr<const struct_or_union_type>();
  }
}

std::shared_ptr<const type>
struct_or_union_content::member::get_type() const noexcept
{
  if (!_is_unnamed_sou)
    return _type;
  else
    return _sou_type;
}

const std::shared_ptr<const struct_or_union_type>&
struct_or_union_content::member::get_sou_type() const noexcept
{
  assert(_is_unnamed_sou);
  return _sou_type;
}

const mpa::limbs& struct_or_union_content::member::get_offset() const noexcept
{
  assert(!_offset.empty());
  return _offset;
}

const mpa::limbs& struct_or_union_content::member::get_bitpos() const noexcept
{
  return _bitpos;
}

bool struct_or_union_content::member::has_constant_offset() const noexcept
{
  return !_offset.empty();
}


struct_or_union_content::struct_or_union_content()
  : _align_ffs{0}, _size{0}, _is_size_constant{false},
    _is_transparent_union{false}
{}

void struct_or_union_content::
add_member(const std::string &name,
	   const std::shared_ptr<const type> &type,
	   mpa::limbs &&offset, mpa::limbs &&bitpos)
{
  _members.push_back(member{name, type, std::move(offset), std::move(bitpos)});
}

void struct_or_union_content::
add_member(const std::string &name,
	   const std::shared_ptr<const type> &type)
{
  _members.push_back(member{name, type});
}

void struct_or_union_content::
add_member(const std::shared_ptr<const struct_or_union_type> &sou_type,
	   mpa::limbs &&offset)
{
  _members.push_back(member{sou_type, std::move(offset)});
}

void struct_or_union_content::
add_member(const std::shared_ptr<const struct_or_union_type> &sou_type)
{
  _members.push_back(member{sou_type});
}

struct_or_union_content::lookup_result
struct_or_union_content::lookup(const std::string &name) const
{
  typedef std::pair<const_member_iterator, const_member_iterator> stack_entry;
  std::vector<stack_entry> stack;

  const_member_iterator it = _members.begin();
  const_member_iterator it_end = _members.end();

  while(it != it_end) {
    while (it != it_end && it->is_unnamed_sou()) {
      // Enter anonymous struct or union.
      const struct_or_union_content * const souc =
	it->get_sou_type()->get_content();

      if (souc && !souc->_members.empty()) {
	stack.push_back(std::make_pair(it, it_end));
	it = souc->_members.begin();
	it_end = souc->_members.end();
      } else {
	// Skip empty, anonymous struct or union.
	++it;
      }
    }

    if (it != it_end) {
      assert(!it->is_unnamed_sou());

      if (it->get_name() == name) {
	// Found it, return the full path to the member.
	lookup_result r;
	std::transform(stack.begin(), stack.end(), std::back_inserter(r),
		       [&](const stack_entry &se) {
			 return se.first;
			});
	r.push_back(it);
	return r;
      }

      ++it;
    }

    // Possibly go up the anonymous struct/union stack.
    while(it == it_end && !stack.empty()) {
      const stack_entry se = stack.back();
      stack.pop_back();
      it = se.first + 1;
      it_end = se.second;
    }
  }
  return lookup_result{};
}

void struct_or_union_content::
set_alignment(const mpa::limbs::size_type log2_align) noexcept
{
  _align_ffs = log2_align + 1;
}

void struct_or_union_content::set_size(mpa::limbs &&size)
{
  _size = std::move(size);
}

void struct_or_union_content::set_size_constant(const bool is_size_constant)
  noexcept
{
  _is_size_constant = is_size_constant;
}

bool struct_or_union_content::is_size_constant() const noexcept
{
  assert(_has_been_layout());
  return _is_size_constant;
}

const mpa::limbs& struct_or_union_content::get_size() const noexcept
{
  assert(is_size_constant());
  return _size;
}

mpa::limbs::size_type struct_or_union_content::get_alignment() const noexcept
{
  assert(_has_been_layout());
  return _align_ffs - 1;
}

bool struct_or_union_content::is_transparent_union() const noexcept
{
  return _is_transparent_union;
}

void struct_or_union_content::set_transparent_union() noexcept
{
  _is_transparent_union = true;
}

bool struct_or_union_content::_has_been_layout() const noexcept
{
  return _align_ffs;
}

struct_or_union_type::
struct_or_union_type(const struct_or_union_kind kind,
		     const ast::sou_decl_list_node &decl_node,
		     const qualifiers &qs)
  : type{qs}, _kind{kind}, _decl_node{&decl_node},
    _is_transparent_union{false},
    _content{nullptr}
{}

struct_or_union_type::
struct_or_union_type(const struct_or_union_kind kind,
		     const struct_or_union_content &content)
  : _kind{kind}, _decl_node{nullptr},
    _is_transparent_union{false},
    _content{&content}
{}

struct_or_union_type::struct_or_union_type(const struct_or_union_type&)
= default;

struct_or_union_type::~struct_or_union_type() noexcept = default;

struct_or_union_type* struct_or_union_type::_clone() const
{
    return new struct_or_union_type(*this);
}

std::shared_ptr<const struct_or_union_type>
struct_or_union_type::create(const struct_or_union_kind kind,
			     const ast::sou_decl_list_node &decl_node,
			     const qualifiers &qs)
{
  return (std::shared_ptr<const struct_or_union_type>
	  (new struct_or_union_type (kind, decl_node, qs)));
}

std::shared_ptr<const struct_or_union_type>
struct_or_union_type::create(const struct_or_union_kind kind,
			     const struct_or_union_content &content)
{
  return (std::shared_ptr<const struct_or_union_type>
	  (new struct_or_union_type (kind, content)));
}

type::type_id struct_or_union_type::get_type_id() const noexcept
{
  return type_id::tid_struct_or_union;
}

bool struct_or_union_type::is_compatible_with(const target &tgt,
					      const type &t,
					      const bool ignore_qualifiers)
  const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool struct_or_union_type::is_compatible_with(const target&,
					      const struct_or_union_type &t,
					      const bool ignore_qualifiers)
  const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (_content && t._content)
    return (_content == t._content);

  if (_decl_node && t._decl_node) {
    return _decl_node->get_id() == t._decl_node->get_id();
  }

  return false;
}

bool struct_or_union_type::is_complete() const noexcept
{
  const struct_or_union_content *content = get_content();
  if (!content)
    return false;

  return true;
}

bool struct_or_union_type::is_size_constant() const noexcept
{
  assert(is_complete());
  return get_content()->is_size_constant();
}

mpa::limbs struct_or_union_type::get_size(const target&) const
{
  assert(is_size_constant());
  return get_content()->get_size();
}

mpa::limbs::size_type
struct_or_union_type::get_type_alignment(const target&)
  const noexcept
{
  assert(is_complete());
  return get_content()->get_alignment();
}

const struct_or_union_content* struct_or_union_type::get_content()
  const noexcept
{
  if (_content) {
    // unnamed struct or union or cached from previous invocation
    return _content;
  }

  assert(_decl_node);
  const ast::struct_or_union_def * const soud = _decl_node->find_definition();
  if (!soud)
    return nullptr;

  _content = soud->get_content();
  return _content;
}

bool struct_or_union_type::is_transparent_union() const noexcept
{
  if (_kind != struct_or_union_kind::souk_union)
    return false;

  if (_is_transparent_union)
    return true;

  const auto *content = this->get_content();
  return content && content->is_transparent_union();
}

std::shared_ptr<const struct_or_union_type>
struct_or_union_type::set_transparent_union() const
{
  std::shared_ptr<struct_or_union_type> new_t(this->_clone());
  new_t->_is_transparent_union = true;
  return new_t;
}


arithmetic_type::arithmetic_type() = default;

arithmetic_type::arithmetic_type(const arithmetic_type&) = default;

arithmetic_type::~arithmetic_type() noexcept = default;

std::shared_ptr<const arithmetic_type>
arithmetic_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<arithmetic_type>(),
			   &arithmetic_type::_clone);
}


int_type::int_type() = default;

int_type::int_type(const int_type&) = default;

int_type::~int_type() noexcept = default;

std::shared_ptr<const int_type>
int_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<int_type>(), &int_type::_clone);
}

std::shared_ptr<const int_type>
int_type::set_user_alignment(const alignment &user_align) const
{
  return _set_user_alignment(_self_ptr<int_type>(),
			     user_align, &int_type::_clone);
}

std::shared_ptr<const arithmetic_type>
int_type::arithmetic_conversion(const target &tgt,
				const arithmetic_type &at) const
{
  // Double-dispatch, but promote the int first.
  return at.arithmetic_conversion(tgt, *this->promote(tgt));
}

std::shared_ptr<const arithmetic_type>
int_type::arithmetic_conversion(const target &tgt,
				const int_type &it) const
{
  return this->integer_conversion(tgt, it);
}

std::shared_ptr<const arithmetic_type>
int_type::arithmetic_conversion(const target &tgt,
				const real_float_type &ft) const
{
  return ft.arithmetic_conversion(tgt, *this->promote(tgt));
}

std::shared_ptr<const arithmetic_type>
int_type::arithmetic_conversion(const target &tgt,
				const complex_float_type &ct) const
{
  return ct.arithmetic_conversion(tgt, *this->promote(tgt));
}

std::shared_ptr<const int_type>
int_type::integer_conversion(const target &tgt, const std_int_type &it)
  const
{
  // Default for plain_char_type, bool_type and enum_type. The
  // generic overload will return some intermediate result of
  // different type (std_int_type most probably).
  return this->integer_conversion(tgt, static_cast<const int_type&>(it));
}

std::shared_ptr<const int_type>
int_type::integer_conversion(const target &tgt, const ext_int_type &it) const
{
  // Default for plain_char_type, bool_type, enum_type and
  // std_int_type. The generic overload will return some intermediate
  // result of different type (std_int_type most probably) in all but
  // the latter case. For std_int_type, the double-dispatch will
  // forward the call to
  // ext_int_type::integer_conversion(const std_int_type&).
  return this->integer_conversion(tgt, static_cast<const int_type&>(it));
}


std_int_type::std_int_type(const kind k, const bool is_signed,
			   const qualifiers &qs)
  : type(qs), _k(k), _signed(is_signed)
{}

std_int_type::std_int_type(const std_int_type&) = default;

std_int_type::~std_int_type() noexcept = default;

std_int_type* std_int_type::_clone() const
{
  return new std_int_type(*this);
}

std::shared_ptr<const std_int_type>
std_int_type::create(const kind k, const bool is_signed,
		     const qualifiers &qs)
{
  return (std::shared_ptr<const std_int_type>
	  (new std_int_type(k, is_signed, qs)));
}

type::type_id std_int_type::get_type_id() const noexcept
{
  return type_id::tid_std_int;
}

bool std_int_type::is_compatible_with(const target &tgt, const type &t,
				      const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool std_int_type::is_compatible_with(const target&,
				      const std_int_type &t,
				      const bool ignore_qualifiers) const
{
  return (this->_k == t._k &&
	  this->_signed == t._signed &&
	  (ignore_qualifiers ||
	   (this->get_qualifiers() == t.get_qualifiers())));
}

bool std_int_type::is_compatible_with(const target &tgt,
				      const enum_type &t,
				      const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

std::shared_ptr<const std_int_type> std_int_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<std_int_type>(), &std_int_type::_clone);
}

mpa::limbs std_int_type::get_size(const target &tgt) const
{
  return tgt.get_std_int_size(_k);
}

mpa::limbs::size_type std_int_type::get_type_alignment(const target &tgt)
  const noexcept
{
  return tgt.get_std_int_alignment(_k);
}

std::shared_ptr<const int_type>
std_int_type::integer_conversion(const target &tgt, const int_type &it) const
{
  // Double-dispatch, but promote the std_int_type first.
  return it.integer_conversion(tgt, *this->_promote());
}

std::shared_ptr<const int_type>
std_int_type::integer_conversion(const target &tgt,
				 const std_int_type &it) const
{
  if (_k < kind::k_int)
    return this->_promote()->integer_conversion(tgt, it);
  else if (it._k < kind::k_int)
    return this->integer_conversion(tgt, *it._promote());

  if (_signed == it._signed) {
    if (_k >= it._k) {
      return this->strip_qualifiers();
    } else {
      return it.strip_qualifiers();
    }
  }

  const std_int_type &t_signed = _signed ? *this : it;
  const std_int_type &t_unsigned = _signed ? it : *this;
  if (t_unsigned._k >= t_signed._k) {
    return t_unsigned.strip_qualifiers();

  } else if (tgt.get_std_int_width(t_signed._k) >
	     tgt.get_std_int_width(t_unsigned._k)) {
    return t_signed.strip_qualifiers();

  } else {
    return std_int_type::create(t_signed._k, false);
  }
}

bool std_int_type::is_signed(const target&) const noexcept
{
  return _signed;
}

mpa::limbs::size_type std_int_type::get_width(const target &tgt)
  const noexcept
{
  return tgt.get_std_int_width(_k);
}

std::shared_ptr<const int_type>
std_int_type::promote(const target&) const
{
  return this->_promote();
}

std::shared_ptr<const int_type> std_int_type::to_unsigned() const
{
  assert(_signed);
  return create(_k, false, get_qualifiers());
}

std::shared_ptr<const std_int_type> std_int_type::_promote() const
{
  switch (_k) {
  case kind::k_char:
    /* fall through */
  case kind::k_short:
    return std_int_type::create(std_int_type::kind::k_int, true,
				get_qualifiers());

  default:
    return _self_ptr<std_int_type>();
  }
}

ext_int_type::ext_int_type(const kind k, const bool is_signed,
			   const qualifiers &qs)
  : type(qs), _k(k), _signed(is_signed)
{}

ext_int_type::ext_int_type(const ext_int_type&) = default;

ext_int_type::~ext_int_type() noexcept = default;

ext_int_type* ext_int_type::_clone() const
{
  return new ext_int_type(*this);
}

std::shared_ptr<const ext_int_type>
ext_int_type::create(const kind k, const bool is_signed,
		     const qualifiers &qs)
{
  return (std::shared_ptr<const ext_int_type>
	  (new ext_int_type(k, is_signed, qs)));
}

type::type_id ext_int_type::get_type_id() const noexcept
{
  return type_id::tid_ext_int;
}

bool ext_int_type::is_compatible_with(const target &tgt, const type &t,
				      const bool ignore_qualifiers) const

{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool ext_int_type::is_compatible_with(const target&,
					const ext_int_type &t,
					const bool ignore_qualifiers)
	  const
{
  return (this->_k == t._k &&
	  this->_signed == t._signed &&
	  (ignore_qualifiers ||
	   (this->get_qualifiers() == t.get_qualifiers())));
}

bool ext_int_type::is_compatible_with(const target &tgt,
				      const enum_type &t,
				      const bool ignore_qualifiers)
  const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

std::shared_ptr<const ext_int_type> ext_int_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<ext_int_type>(), &ext_int_type::_clone);
}

mpa::limbs ext_int_type::get_size(const target &tgt) const
{
  return tgt.get_ext_int_size(_k);
}

mpa::limbs::size_type
ext_int_type::get_type_alignment(const target &tgt) const noexcept
{
  return tgt.get_ext_int_alignment(_k);
}

std::shared_ptr<const int_type>
ext_int_type::integer_conversion(const target &tgt, const int_type &it) const
{
  // Double-dispatch
  return it.integer_conversion(tgt, *this);
}

std::shared_ptr<const int_type>
ext_int_type::integer_conversion(const target &tgt, const std_int_type &it)
  const
{
  const mpa::limbs::size_type l_width = this->get_width(tgt);
  if (l_width <= tgt.get_std_int_width(std_int_type::kind::k_int))
    return this->promote(tgt)->integer_conversion(tgt, it);

  const std::shared_ptr<const int_type> pit = it.promote(tgt);
  const mpa::limbs::size_type r_width = pit->get_width(tgt);
  if (_signed == pit->is_signed(tgt)) {
    if (l_width >= r_width)
      return this->strip_qualifiers();
    else
      return pit->strip_qualifiers();
  }

  if (l_width == r_width) {
    if (!_signed)
      return pit->strip_qualifiers()->to_unsigned();
    else
      return pit->strip_qualifiers();
  } else if (l_width > r_width) {
    return this->strip_qualifiers();
  } else {
    return pit->strip_qualifiers();
  }
}

std::shared_ptr<const int_type>
ext_int_type::integer_conversion(const target &tgt, const ext_int_type &it)
  const
{
  const mpa::limbs::size_type int_width =
    tgt.get_std_int_width(std_int_type::kind::k_int);
  const mpa::limbs::size_type l_width = this->get_width(tgt);
  if (l_width <= int_width)
    return this->promote(tgt)->integer_conversion(tgt, it);
  const mpa::limbs::size_type r_width = it.get_width(tgt);
  if (r_width <= int_width)
    return this->integer_conversion(tgt, *it.promote(tgt));

  if (_signed == it._signed) {
    if (l_width >= r_width)
      return this->strip_qualifiers();
    else
      return it.strip_qualifiers();
  }

  if (l_width == r_width) {
    if (!_signed)
      return this->strip_qualifiers();
    else
      return it.strip_qualifiers();
  } else if (l_width > r_width) {
    return this->strip_qualifiers();
  } else {
    return it.strip_qualifiers();
  }
}

bool ext_int_type::is_signed(const target &tgt) const noexcept
{
  return _signed;
}

mpa::limbs::size_type ext_int_type::get_width(const target &tgt)
  const noexcept
{
    return tgt.get_ext_int_width(_k);
}

std::shared_ptr<const int_type>
ext_int_type::promote(const target &tgt) const
{
  const mpa::limbs::size_type int_width =
    tgt.get_std_int_width(types::std_int_type::kind::k_int);
  const mpa::limbs::size_type w = this->get_width(tgt);

  if (w > int_width)
    return _self_ptr<ext_int_type>();

  bool is_signed = true;
  if (w == int_width && !_signed)
    is_signed = false;

  return std_int_type::create(std_int_type::kind::k_int, is_signed,
			      get_qualifiers());
}

std::shared_ptr<const int_type> ext_int_type::to_unsigned() const
{
  return create(_k, false, get_qualifiers());
}


plain_char_type::plain_char_type(const qualifiers &qs)
  : type(qs)
{}

plain_char_type::plain_char_type(const plain_char_type&) = default;

plain_char_type::~plain_char_type() noexcept = default;

plain_char_type* plain_char_type::_clone() const
{
  return new plain_char_type(*this);
}

std::shared_ptr<const plain_char_type>
plain_char_type::create(const qualifiers &qs)
{
  return (std::shared_ptr<const plain_char_type>
	  (new plain_char_type(qs)));
}

type::type_id plain_char_type::get_type_id() const noexcept
{
  return type_id::tid_plain_char;
}

bool plain_char_type::is_compatible_with(const target &tgt,
					 const type &t,
					 const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool plain_char_type::is_compatible_with(const target&,
					 const plain_char_type &t,
					 const bool ignore_qualifiers) const
{
  return ignore_qualifiers || (this->get_qualifiers() == t.get_qualifiers());
}

mpa::limbs plain_char_type::get_size(const target&) const
{
  return mpa::limbs::from_size_type(1);
}

mpa::limbs::size_type
plain_char_type::get_type_alignment(const target&) const noexcept
{
  return 0;
}

std::shared_ptr<const int_type>
plain_char_type::integer_conversion(const target &tgt, const int_type &it) const
{
  // Promote and double-dispatch.
  return it.integer_conversion(tgt, *this->_promote());
}

bool plain_char_type::is_signed(const target &tgt) const noexcept
{
  return tgt.is_char_signed();
}

mpa::limbs::size_type plain_char_type::get_width(const target &tgt)
  const noexcept
{
  return tgt.get_std_int_width(std_int_type::kind::k_char);
}

std::shared_ptr<const int_type> plain_char_type::promote(const target&) const
{
  return this->_promote();
}

std::shared_ptr<const int_type> plain_char_type::to_unsigned() const
{
  return std_int_type::create(std_int_type::kind::k_char, false,
			      get_qualifiers());
}

std::shared_ptr<const std_int_type> plain_char_type::_promote() const
{
  return std_int_type::create(std_int_type::kind::k_int, true,
			      get_qualifiers());
}


bool_type::bool_type(const qualifiers &qs)
  : type(qs)
{}

bool_type::bool_type(const bool_type&) = default;

bool_type::~bool_type() noexcept = default;

bool_type* bool_type::_clone() const
{
  return new bool_type(*this);
}

std::shared_ptr<const bool_type>
bool_type::create(const qualifiers &qs)
{
  return std::shared_ptr<const bool_type>(new bool_type(qs));
}

type::type_id bool_type::get_type_id() const noexcept
{
  return type_id::tid_bool;
}

bool bool_type::is_compatible_with(const target &tgt, const type &t,
				   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool bool_type::is_compatible_with(const target&, const bool_type &t,
				   const bool ignore_qualifiers) const
{
  return ignore_qualifiers || (this->get_qualifiers() == t.get_qualifiers());
}

mpa::limbs bool_type::get_size(const target&) const
{
  return mpa::limbs::from_size_type(1);
}

mpa::limbs::size_type
bool_type::get_type_alignment(const target&) const noexcept
{
  return 0;
}

std::shared_ptr<const int_type>
bool_type::integer_conversion(const target &tgt, const int_type &it) const
{
  // Promote and double-dispatch.
  return it.integer_conversion(tgt, *this->_promote());
}

bool bool_type::is_signed(const target&) const noexcept
{
  return false;
}

mpa::limbs::size_type bool_type::get_width(const target&)
  const noexcept
{
  return 1;
}

std::shared_ptr<const int_type> bool_type::promote(const target&) const
{
  return this->_promote();
}

std::shared_ptr<const int_type> bool_type::to_unsigned() const
{
  // A _Bool is always unsigned, no need to call to_unsigned() on it.
  assert(0);
  return nullptr;
}

std::shared_ptr<const std_int_type> bool_type::_promote() const
{
  return std_int_type::create(std_int_type::kind::k_int, true,
			      get_qualifiers());
}


enum_content::member::
member(const ast::enumerator &e, const std::string &name,
       const std::shared_ptr<const int_type> &initial_type,
       const target_int &value)
  : _e(e), _name(name), _value(value), _type(initial_type)
{}

void enum_content::member::convert_value(const mpa::limbs::size_type prec,
					 const bool is_signed)
{
  if (_value.is_signed() != is_signed || _value.width() - is_signed != prec)
    _value = _value.convert(prec, is_signed);
}

void enum_content::member::set_type(const std::shared_ptr<const int_type> &t)
  noexcept
{
  _type = t;
}

void enum_content::member::set_type(std::shared_ptr<const int_type> &&t)
  noexcept
{
  _type = std::move(t);
}

const std::shared_ptr<const int_type>&
enum_content::member::get_type() const noexcept
{
  assert(_type);
  return _type;
}

void enum_content::
add_member(const ast::enumerator &e, const std::string &name,
	   const std::shared_ptr<const int_type> &initial_type,
	   const target_int &value)
{
  assert(!_underlying_type);
  assert(!lookup(name));
  _members.emplace_back(e, name, initial_type, value);
}

void enum_content::add_member(const ast::enumerator &e, const std::string &name,
			      const target &tgt)
{
  if (_members.empty()) {
    // The initial value is zero and the target_int's width will eventually
    // get adjusted in target::evaluate_enum_type().
    const mpa::limbs::size_type int_width =
      tgt.get_std_int_width(types::std_int_type::kind::k_int);
    mpa::limbs ls;
    ls.resize(mpa::limbs::width_to_size(int_width));

    add_member(e, name, std_int_type::create(std_int_type::kind::k_int,
					     true),
	       target_int(int_width - 1, true, std::move(ls)));

  } else {
    const member &last_member = _members.back();
    const target_int &last_val = last_member.get_value();
    const mpa::limbs::size_type last_width = last_val.width();
    const bool last_is_signed = last_val.is_signed();

    mpa::limbs ls_one;
    ls_one.resize(mpa::limbs::width_to_size(last_width));
    ls_one.set_bit(0, true);
    const target_int ti_one(last_width - last_is_signed, last_is_signed,
			    std::move(ls_one));

    add_member(e, name, last_member.get_type(), last_val + ti_one);
  }
}


const enum_content::member* enum_content::lookup(const std::string &name)
  const noexcept
{
  auto it = std::find_if(_members.begin(), _members.end(),
			 [&name](const member &m) {
			   return m.get_name() == name;
			 });
  if (it != _members.end())
    return &*it;

  return nullptr;
}

void enum_content::set_underlying_type(std::shared_ptr<const int_type> &&ut)
  noexcept
{
  assert(!_underlying_type);
  _underlying_type = std::move(ut);
}

const std::shared_ptr<const int_type>&
enum_content::get_underlying_type() const noexcept
{
  return _underlying_type;
}


enum_type::enum_type(const ast::enum_decl_list_node &decl_node,
		     const qualifiers qs)
  : type(qs), _decl_node(decl_node), _content(nullptr)
{}

enum_type::enum_type(const enum_type&) = default;

enum_type::~enum_type() noexcept = default;

enum_type* enum_type::_clone() const
{
  return new enum_type(*this);
}

std::shared_ptr<const enum_type>
enum_type::create(const ast::enum_decl_list_node &decl_node,
		  const qualifiers &qs)
{
  return std::shared_ptr<const enum_type>(new enum_type(decl_node, qs));
}

type::type_id enum_type::get_type_id() const noexcept
{
  return type_id::tid_enum;
}

bool enum_type::is_compatible_with(const target &tgt, const type &t,
				   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool enum_type::is_compatible_with(const target&, const enum_type &t,
				   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  return _decl_node.get_id() == t._decl_node.get_id();
}

bool enum_type::is_compatible_with(const target &tgt,
				   const std_int_type &t,
				   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (!this->is_complete())
    return false;
  return this->get_underlying_type()->is_compatible_with(tgt, t, false);
}

bool enum_type::is_compatible_with(const target &tgt,
				   const ext_int_type &t,
				   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (!this->is_complete())
    return false;
  return this->get_underlying_type()->is_compatible_with(tgt, t, false);
}

bool enum_type::is_complete() const noexcept
{
  const enum_content *content = _get_content();
  if (!content)
    return false;

  return !!content->get_underlying_type();
}

mpa::limbs enum_type::get_size(const target &tgt) const
{
  return get_underlying_type()->get_size(tgt);
}

mpa::limbs::size_type enum_type::get_type_alignment(const target &tgt)
  const noexcept
{
  return get_underlying_type()->get_type_alignment(tgt);
}

std::shared_ptr<const int_type>
enum_type::integer_conversion(const target &tgt, const int_type &it) const
{
  // Promote and forward to non-enum_type implemenation.
  return this->promote(tgt)->integer_conversion(tgt, it);
}

bool enum_type::is_signed(const target &tgt) const noexcept
{
  return get_underlying_type()->is_signed(tgt);
}

mpa::limbs::size_type enum_type::get_width(const target &tgt)
  const noexcept
{
  return get_underlying_type()->get_width(tgt);
}

std::shared_ptr<const int_type> enum_type::promote(const target &tgt) const
{
  return this->get_underlying_type()->promote(tgt);
}

std::shared_ptr<const int_type> enum_type::to_unsigned() const
{
  return get_underlying_type()->to_unsigned();
}

const std::shared_ptr<const int_type>& enum_type::get_underlying_type()
  const noexcept
{
  assert(this->is_complete());
  return _get_content()->get_underlying_type();
}

const enum_content* enum_type::_get_content() const noexcept
{
  if (_content) {
    // cached from previous invocation
    return _content;
  }

  const ast::enum_def * const ed = _decl_node.find_definition();
  if (!ed)
    return nullptr;

  _content = &ed->get_enumerator_list().get_content();
  return _content;
}


bitfield_type::
bitfield_type(std::shared_ptr<const int_type> &&base_type,
	      const mpa::limbs::size_type width)
  : type(base_type->get_qualifiers()),
    _base_type(base_type->strip_qualifiers()), _width(width), _packed(false)
{}

bitfield_type::bitfield_type(const bitfield_type&) = default;

bitfield_type::~bitfield_type() noexcept = default;

bitfield_type* bitfield_type::_clone() const
{
  return new bitfield_type(*this);
}

std::shared_ptr<const bitfield_type>
bitfield_type::create(std::shared_ptr<const int_type> &&base_type,
		      const mpa::limbs::size_type width)
{
  return (std::shared_ptr<const bitfield_type>
	  (new bitfield_type(std::move(base_type), width)));
}

type::type_id bitfield_type::get_type_id() const noexcept
{
  return type_id::tid_bitfield;
}

bool bitfield_type::is_compatible_with(const target &tgt, const type &t,
				       const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

std::shared_ptr<const bitfield_type> bitfield_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<bitfield_type>(), &bitfield_type::_clone);
}

bool bitfield_type::is_signed(const target &tgt) const noexcept
{
  return _base_type->is_signed(tgt);
}

mpa::limbs::size_type bitfield_type::get_width(const target&)
  const noexcept
{
  return _width;
}

std::shared_ptr<const int_type> bitfield_type::promote(const target &tgt) const
{
  return tgt.width_to_int_type(_width, _base_type->is_signed(tgt), false);
}

std::shared_ptr<const bitfield_type>
bitfield_type::set_user_alignment(const alignment &user_align) const
{
  std::shared_ptr<bitfield_type> new_type(_clone());

  new_type->_user_align = user_align;
  return new_type;
}

std::shared_ptr<const bitfield_type> bitfield_type::set_packed() const
{
  std::shared_ptr<bitfield_type> new_type(_clone());

  new_type->_packed = true;
  return new_type;
}


real_float_type::format::
format(const mpa::limbs::size_type _significand_width,
       const mpa::limbs::size_type _exponent_width) noexcept
  : significand_width(_significand_width),
    exponent_width(_exponent_width)
{}

bool real_float_type::format::operator==(const format &rhs) const noexcept
{
  return (this->significand_width == rhs.significand_width &&
	  this->exponent_width == rhs.exponent_width);
}

bool real_float_type::format::operator<(const format &rhs) const noexcept
{
  return ((this->significand_width < rhs.significand_width &&
	   this->exponent_width <= rhs.exponent_width) ||
	  (this->significand_width <= rhs.significand_width &&
	   this->exponent_width < rhs.exponent_width));
}

bool real_float_type::format::operator<=(const format &rhs) const noexcept
{
  return *this == rhs || *this < rhs;
}

real_float_type::real_float_type() = default;

real_float_type::real_float_type(const real_float_type&) = default;

real_float_type::~real_float_type() noexcept = default;

std::shared_ptr<const real_float_type> real_float_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<real_float_type>(),
			   &real_float_type::_clone);
}

std::shared_ptr<const arithmetic_type>
real_float_type::arithmetic_conversion(const target &tgt,
				       const arithmetic_type &at) const
{
  // Double-dispatch.
  return at.arithmetic_conversion(tgt, *this);
}

std::shared_ptr<const arithmetic_type>
real_float_type::arithmetic_conversion(const target&,
				       const int_type&) const
{
  return this->arithmetic_type::strip_qualifiers();
}

std::shared_ptr<const arithmetic_type>
real_float_type::arithmetic_conversion(const target &tgt,
				       const real_float_type &ft) const
{
  return this->real_float_conversion(tgt, ft);
}

std::shared_ptr<const arithmetic_type>
real_float_type::arithmetic_conversion(const target &tgt,
				       const complex_float_type &ct) const
{
  return ct.arithmetic_conversion(tgt, *this);
}



std_float_type::std_float_type(const kind k, const qualifiers &qs)
  : type(qs), _k(k)
{}

std_float_type::std_float_type(const std_float_type&) = default;

std_float_type::~std_float_type() noexcept = default;

std_float_type* std_float_type::_clone() const
{
  return new std_float_type(*this);
}

std::shared_ptr<const std_float_type>
std_float_type::create(const kind k, const qualifiers &qs)
{
  return (std::shared_ptr<const std_float_type>
	  (new std_float_type(k, qs)));
}

type::type_id std_float_type::get_type_id() const noexcept
{
  return type_id::tid_std_float;
}

bool std_float_type::is_compatible_with(const target &tgt,
					 const type &t,
					 const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool std_float_type::is_compatible_with(const target&,
					 const std_float_type &t,
					 const bool ignore_qualifiers) const
{
  return (this->get_kind() == t.get_kind() &&
	  (ignore_qualifiers ||
	   (this->get_qualifiers() == t.get_qualifiers())));
}

mpa::limbs std_float_type::get_size(const target &tgt) const
{
  return tgt.get_std_float_size(this->get_kind());
}

mpa::limbs::size_type
std_float_type::get_type_alignment(const target &tgt) const noexcept
{
  return tgt.get_std_float_alignment(this->get_kind());
}

std::shared_ptr<const real_float_type>
std_float_type::real_float_conversion(const target &tgt,
				      const real_float_type &ft) const
{
  return ft.real_float_conversion(tgt, *this);
}

std::shared_ptr<const real_float_type>
std_float_type::real_float_conversion(const target &tgt,
				      const std_float_type &ft) const
{
  if (this->get_kind() >= ft.get_kind()) {
    return this->real_float_type::strip_qualifiers();
  } else {
    return ft.real_float_type::strip_qualifiers();
  }
}

std::shared_ptr<const real_float_type>
std_float_type::real_float_conversion(const target &tgt,
				      const ext_float_type &ft) const
{
  return ft.real_float_conversion(tgt, *this);
}

std::shared_ptr<const real_float_type> std_float_type::promote(const target&)
  const
{
  if (this->get_kind() == kind::k_float)
    return std_float_type::create(kind::k_double, this->get_qualifiers());

  return _self_ptr<real_float_type>();
}

real_float_type::format std_float_type::get_format(const target &tgt)
  const noexcept
{
  return tgt.get_std_float_format(_k);
}


ext_float_type::ext_float_type(const kind k, const qualifiers &qs)
  : type(qs), _k(k)
{}

ext_float_type::ext_float_type(const ext_float_type&) = default;

ext_float_type::~ext_float_type() noexcept = default;

ext_float_type* ext_float_type::_clone() const
{
  return new ext_float_type(*this);
}

std::shared_ptr<const ext_float_type>
ext_float_type::create(const kind k, const qualifiers &qs)
{
  return (std::shared_ptr<const ext_float_type>
	  (new ext_float_type(k, qs)));
}

type::type_id ext_float_type::get_type_id() const noexcept
{
  return type_id::tid_ext_float;
}

bool ext_float_type::is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool ext_float_type::is_compatible_with(const target&,
					const ext_float_type &t,
					const bool ignore_qualifiers) const
{
  return (this->get_kind() == t.get_kind() &&
	  (ignore_qualifiers ||
	   (this->get_qualifiers() == t.get_qualifiers())));
}

mpa::limbs ext_float_type::get_size(const target &tgt) const
{
  return tgt.get_ext_float_size(this->get_kind());
}

mpa::limbs::size_type
ext_float_type::get_type_alignment(const target &tgt) const noexcept
{
  return tgt.get_ext_float_alignment(this->get_kind());
}

std::shared_ptr<const real_float_type>
ext_float_type::real_float_conversion(const target &tgt,
				      const real_float_type &ft) const
{
  return ft.real_float_conversion(tgt, *this);
}

std::shared_ptr<const real_float_type>
ext_float_type::real_float_conversion(const target &tgt,
				      const std_float_type &ft) const
{
  const format &this_fmt = this->get_format(tgt);
  const format &other_fmt = ft.get_format(tgt);

  if (this_fmt <= other_fmt) {
    return ft.real_float_type::strip_qualifiers();
  } else if (other_fmt < this_fmt) {
    return this->real_float_type::strip_qualifiers();
  } else {
    throw std::range_error("incompatible float ranges");
  }
}

std::shared_ptr<const real_float_type>
ext_float_type::real_float_conversion(const target &tgt,
				      const ext_float_type &ft) const
{
  const format &this_fmt = this->get_format(tgt);
  const format &other_fmt = ft.get_format(tgt);

  if (this_fmt <= other_fmt) {
    return ft.real_float_type::strip_qualifiers();
  } else if (other_fmt < this_fmt) {
    return this->real_float_type::strip_qualifiers();
  } else {
    throw std::range_error("incompatible float ranges");
  }
}

std::shared_ptr<const real_float_type>
ext_float_type::promote(const target &tgt) const
{
  // Only standard float gets promoted in the course of the default
  // argument promotions. In particular, the extended floating types
  // are left as-is.
  return _self_ptr<real_float_type>();
}

real_float_type::format
ext_float_type::get_format(const target &tgt) const noexcept
{
  return tgt.get_ext_float_format(_k);
}


complex_float_type::
complex_float_type(std::shared_ptr<const real_float_type> &&base_type,
		   const qualifiers &qs)
  : type(qs), _base_type(std::move(base_type))
{}

complex_float_type::complex_float_type(const complex_float_type&) = default;

complex_float_type::~complex_float_type() noexcept = default;

complex_float_type* complex_float_type::_clone() const
{
  return new complex_float_type(*this);
}

std::shared_ptr<const complex_float_type>
complex_float_type::create(std::shared_ptr<const real_float_type> &&base_type,
			   const qualifiers &qs)
{
  return (std::shared_ptr<const complex_float_type>
	  (new complex_float_type(std::move(base_type), qs)));
}

type::type_id complex_float_type::get_type_id() const noexcept
{
  return type_id::tid_complex_float;
}

bool complex_float_type::is_compatible_with(const target &tgt,
					    const type &t,
					    const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}

bool complex_float_type::is_compatible_with(const target &tgt,
					    const complex_float_type &t,
					    const bool ignore_qualifiers) const
{
  return (this->_base_type->is_compatible_with(tgt, *t._base_type, true) &&
	  (ignore_qualifiers ||
	   (this->get_qualifiers() == t.get_qualifiers())));
}

mpa::limbs complex_float_type::get_size(const target &tgt) const
{
  return _base_type->get_size(tgt).lshift(1);
}

mpa::limbs::size_type
complex_float_type::get_type_alignment(const target &tgt) const noexcept
{
  return this->_base_type->get_type_alignment(tgt);
}

std::shared_ptr<const arithmetic_type>
complex_float_type::arithmetic_conversion(const target &tgt,
					  const arithmetic_type &at) const
{
  // Double-dispatch.
  return at.arithmetic_conversion(tgt, *this);
}

std::shared_ptr<const arithmetic_type>
complex_float_type::arithmetic_conversion(const target&,
					  const int_type&) const
{
  return this->arithmetic_type::strip_qualifiers();
}

std::shared_ptr<const arithmetic_type>
complex_float_type::arithmetic_conversion(const target &tgt,
					  const real_float_type &ft) const
{
  return (complex_float_type::create
	  (this->_base_type->real_float_conversion(tgt, ft)));
}

std::shared_ptr<const arithmetic_type>
complex_float_type::arithmetic_conversion(const target &tgt,
					  const complex_float_type &ct) const
{
  return (complex_float_type::create
	  (this->_base_type->real_float_conversion(tgt, *ct._base_type)));
}

std::shared_ptr<const complex_float_type>
complex_float_type::promote(const target &tgt) const
{
  return complex_float_type::create(this->_base_type->promote(tgt),
				    this->get_qualifiers());
}


builtin_func_type::
builtin_func_type(const builtin_func::factory &builtin_func_fac)
  : _builtin_func_fac(&builtin_func_fac)
{}

builtin_func_type::builtin_func_type(const builtin_func_type&) = default;

builtin_func_type::~builtin_func_type() noexcept = default;

builtin_func_type* builtin_func_type::_clone() const
{
  return new builtin_func_type(*this);
}

std::shared_ptr<const builtin_func_type>
builtin_func_type::create(const builtin_func::factory &builtin_func_fac)
{
  return (std::shared_ptr<const builtin_func_type>
	  (new builtin_func_type(builtin_func_fac)));
}

type::type_id builtin_func_type::get_type_id() const noexcept
{
  return type_id::tid_builtin_func;
}

bool builtin_func_type::is_compatible_with(const target &tgt,
					   const type &t,
					   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(tgt, *this, ignore_qualifiers);
}
