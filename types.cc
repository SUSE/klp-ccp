#include <cassert>
#include <algorithm>
#include "architecture.hh"
#include "ast.hh"
#include "callables_wrapper.hh"
#include "constexpr_value.hh"

using namespace suse::cp;
using namespace suse::cp::types;


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

mpa::limbs::size_type alignment::get_log2_value() const noexcept
{
  assert(is_set());
  return _align_ffs - 1;
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

bool type::is_compatible_with(const architecture&, const void_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const plain_char_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const bool_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const std_int_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const enum_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const real_float_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const complex_float_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const struct_or_union_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const array_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&,
			      const prototyped_function_type&, const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture &,
			      const unprototyped_function_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const pointer_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const bitfield_type&,
			      const bool) const
{
  return false;
}

bool type::is_compatible_with(const architecture&, const builtin_func_type&,
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
addressable_type::get_effective_alignment(const architecture &arch)
  const noexcept
{
  if (_user_align.is_set())
    return _user_align.get_log2_value();

  return get_type_alignment(arch);
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

mpa::limbs function_type::get_size(const architecture&) const
{
  // GCC extension: sizeof(void(void)) is 1.
  return mpa::limbs::from_size_type(1);
}

mpa::limbs::size_type
function_type::get_type_alignment(const architecture&) const noexcept
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

bool prototyped_function_type::is_compatible_with(const architecture &arch,
						  const type &t,
						  const bool ignore_qualifiers)
  const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool prototyped_function_type::
is_compatible_with(const architecture &arch,
		   const prototyped_function_type &t,
		   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (!this->get_return_type()->is_compatible_with(arch, *t.get_return_type(),
						   false)) {
    return false;
  }

  if (_ptl.size() != t._ptl.size() ||
      _variadic != t._variadic) {
    return false;
  }

  for (auto it1 = _ptl.begin(), it2 = t._ptl.begin(); it1 != _ptl.end();
       ++it1, ++it2) {
    if (!(*it1)->is_compatible_with(arch, **it2, true))
      return false;
  }

  return true;
}

bool prototyped_function_type::
is_compatible_with(const architecture &arch,
		   const unprototyped_function_type &t,
		   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
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
is_compatible_with(const architecture &arch, const type &t,
		   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool unprototyped_function_type::
is_compatible_with(const architecture &arch,
		   const unprototyped_function_type &t,
		   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (!this->get_return_type()->is_compatible_with(arch, *t.get_return_type(),
						   false)) {
    return false;
  }

  return true;
}

bool unprototyped_function_type::
is_compatible_with(const architecture &arch,
		   const prototyped_function_type &t,
		   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (!this->get_return_type()->is_compatible_with(arch, *t.get_return_type(),
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
	       return it.is_compatible_with(arch, *it.promote(arch), true);
	     },
	     [&](const float_type &ft) {
	       return ft.is_compatible_with(arch, *ft.promote(), true);
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


array_type::array_type(std::shared_ptr<const object_type> &&element_type,
		       const ast::expr * const length_expr)
  : _element_type(std::move(element_type)), _length_expr(length_expr),
    _unspec_vla(false)
{}

array_type::array_type(std::shared_ptr<const object_type> &&element_type,
		       mpa::limbs &&initializer_length)
  : _element_type(std::move(element_type)), _length_expr(nullptr),
    _initializer_length(std::move(initializer_length)), _unspec_vla(false)
{}

array_type::array_type(std::shared_ptr<const object_type> &&element_type,
		       const bool unspec_vla)
  : _element_type(std::move(element_type)), _length_expr(nullptr),
    _unspec_vla(unspec_vla)
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

bool array_type::is_compatible_with(const architecture &arch, const type &t,
				    const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool array_type::is_compatible_with(const architecture &arch,
				    const array_type &t,
				    const bool ignore_qualifiers) const
{
  if (!_element_type->is_compatible_with(arch, *t._element_type,
					 ignore_qualifiers)) {
    return false;
  }

  if (!this->is_complete() || !t.is_complete())
    return true;

  if (!this->is_size_constant() || !t.is_size_constant())
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

mpa::limbs array_type::get_size(const architecture &arch) const
{
  assert(is_size_constant());
  const mpa::limbs &length = get_length();
  mpa::limbs size = length * _element_type->get_size(arch);
  return size;
}

mpa::limbs::size_type
array_type::get_type_alignment(const architecture &arch) const noexcept
{
  return _element_type->get_effective_alignment(arch);
}

const mpa::limbs& array_type::get_length() const noexcept
{
  assert(is_size_constant());
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

bool void_type::is_compatible_with(const architecture &arch, const type &t,
				   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool void_type::is_compatible_with(const architecture&, const void_type &t,
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

mpa::limbs void_type::get_size(const architecture&) const
{
  // GCC extension: sizeof(void) is 1.
  return mpa::limbs::from_size_type(1);
}

mpa::limbs::size_type
void_type::get_type_alignment(const architecture&) const noexcept
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

bool pointer_type::is_compatible_with(const architecture &arch,
				      const type &t,
				      const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool pointer_type::is_compatible_with(const architecture &arch,
				      const pointer_type &t,
				      const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  return _pointed_to_type->is_compatible_with(arch, *t._pointed_to_type, true);
}

mpa::limbs pointer_type::get_size(const architecture &arch) const
{
  return arch.get_pointer_size();
}

mpa::limbs::size_type
pointer_type::get_type_alignment(const architecture &arch) const noexcept
{
  return arch.get_pointer_alignment();
}


struct_or_union_content::member::
member(const std::string &name, std::shared_ptr<const type> &&type)
  : _name(name), _type(std::move(type)), _has_constant_offset(false),
    _is_unnamed_sou(false)
{
}

struct_or_union_content::member::
member(std::shared_ptr<const struct_or_union_type> &&sou_type)
  : _sou_type(std::move(sou_type)), _has_constant_offset(false),
    _is_unnamed_sou(true)
{}

struct_or_union_content::member::member(member &&m)
  : _name(std::move(m._name)), _offset(std::move(m._offset)),
    _bitpos(std::move(m._bitpos)), _has_constant_offset(m._has_constant_offset),
    _is_unnamed_sou(m._is_unnamed_sou)
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

void struct_or_union_content::member::set_offset(const mpa::limbs &offset)
{
  _offset = offset;
}

const mpa::limbs& struct_or_union_content::member::get_offset() const noexcept
{
  assert(!_offset.empty());
  return _offset;
}

void struct_or_union_content::member::set_bitpos(const mpa::limbs &bitpos)
{
  _bitpos = bitpos;
}

const mpa::limbs& struct_or_union_content::member::get_bitpos() const noexcept
{
  return _bitpos;
}

void struct_or_union_content::member::
set_has_constant_offset(const bool val) noexcept
{
  _has_constant_offset = val;
}

bool struct_or_union_content::member::has_constant_offset() const noexcept
{
  return _has_constant_offset;
}


struct_or_union_content::struct_or_union_content() = default;

void struct_or_union_content::add_member(member &&m)
{
  _members.emplace_back(std::move(m));
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

bool struct_or_union_content::has_been_layout() const noexcept
{
  return _align_ffs;
}

bool struct_or_union_content::is_size_constant() const noexcept
{
  assert(has_been_layout());
  return _is_size_constant;
}

const mpa::limbs& struct_or_union_content::get_size() const noexcept
{
  assert(is_size_constant());
  return _size;
}

mpa::limbs::size_type struct_or_union_content::get_alignment() const noexcept
{
  assert(has_been_layout());
  return _align_ffs - 1;
}


struct_or_union_type::
struct_or_union_type(const struct_or_union_kind kind,
		     const ast::sou_decl_list_node &decl_node,
		     const qualifiers &qs)
  : type(qs), _kind(kind), _decl_node(&decl_node), _content(nullptr)
{}

struct_or_union_type::
struct_or_union_type(const struct_or_union_kind kind,
		     const struct_or_union_content &content)
  : _kind(kind), _decl_node(nullptr), _content(&content)
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

bool struct_or_union_type::is_compatible_with(const architecture &arch,
					      const type &t,
					      const bool ignore_qualifiers)
  const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool struct_or_union_type::is_compatible_with(const architecture&,
					      const struct_or_union_type &t,
					      const bool ignore_qualifiers)
  const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  if (_content && t._content)
    return (_content == t._content);

  if (_decl_node && t._decl_node) {
    const ast::sou_decl_list_node &my_decl =
      _decl_node->get_declaration().get_target_decl_list_node();
    const ast::sou_decl_list_node &other_decl =
      t._decl_node->get_declaration().get_target_decl_list_node();

    return (!(other_decl.for_each
	      (wrap_callables<no_default_action>
	       ([&](const ast::struct_or_union_def &sd) {
		  return &my_decl != &sd.get_decl_list_node();
		},
		[&](const ast::struct_or_union_ref &sr) {
		  return &my_decl != &sr.get_decl_list_node();
		}))));
  }

  return false;
}

bool struct_or_union_type::is_complete() const noexcept
{
  const struct_or_union_content *content = get_content();
  if (!content)
    return false;

  return content->has_been_layout();
}

bool struct_or_union_type::is_size_constant() const noexcept
{
  assert(is_complete());
  return get_content()->is_size_constant();
}

mpa::limbs struct_or_union_type::get_size(const architecture&) const
{
  assert(is_size_constant());
  return get_content()->get_size();
}

mpa::limbs::size_type
struct_or_union_type::get_type_alignment(const architecture&)
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

  _content = &soud->get_content();
  return _content;
}


arithmetic_type::arithmetic_type() = default;

arithmetic_type::arithmetic_type(const arithmetic_type&) = default;

arithmetic_type::~arithmetic_type() noexcept = default;


int_type::int_type() = default;

int_type::int_type(const int_type&) = default;

int_type::~int_type() noexcept = default;

std::shared_ptr<const arithmetic_type>
arithmetic_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<arithmetic_type>(),
			   &arithmetic_type::_clone);
}

std::shared_ptr<const arithmetic_type>
int_type::arithmetic_conversion(const architecture &arch,
				const arithmetic_type &at) const
{
  // Gets called for all non-std_int_type integer types. Promote to
  // std_int_type and forward the call.
  return at.arithmetic_conversion(arch, *this->promote(arch));
}

std::shared_ptr<const arithmetic_type>
int_type::arithmetic_conversion(const architecture &arch,
				const std_int_type &it) const
{
  return this->integer_conversion(arch, it);
}

std::shared_ptr<const arithmetic_type>
int_type::arithmetic_conversion(const architecture &arch,
				const real_float_type &ft) const
{
  // Gets called for all non-std_int_type integer types. Promote to
  // std_int_type and forward the call.
  return ft.arithmetic_conversion(arch, *this->promote(arch));
}

std::shared_ptr<const arithmetic_type>
int_type::arithmetic_conversion(const architecture &arch,
				const complex_float_type &ct) const
{
  // Gets called for all non-std_int_type integer types. Promote to
  // std_int_type and forward the call.
  return ct.arithmetic_conversion(arch, *this->promote(arch));
}

std::shared_ptr<const std_int_type>
int_type::integer_conversion(const architecture &arch, const int_type &it) const
{
  return it.integer_conversion(arch, *this->promote(arch));
}

std::shared_ptr<const std_int_type>
int_type::integer_conversion(const architecture &arch, const std_int_type &it)
  const
{
  return it.integer_conversion(arch, *this->promote(arch));
}


returnable_int_type::returnable_int_type() = default;

returnable_int_type::returnable_int_type(const returnable_int_type&) = default;

returnable_int_type::~returnable_int_type() noexcept = default;

std::shared_ptr<const returnable_int_type>
returnable_int_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<returnable_int_type>(),
			   &returnable_int_type::_clone);
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

bool std_int_type::is_compatible_with(const architecture &arch, const type &t,
				      const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool std_int_type::is_compatible_with(const architecture&,
				      const std_int_type &t,
				      const bool ignore_qualifiers) const
{
  return (this->_k == t._k &&
	  this->_signed == t._signed &&
	  (ignore_qualifiers ||
	   (this->get_qualifiers() == t.get_qualifiers())));
}

bool std_int_type::is_compatible_with(const architecture &arch,
				      const enum_type &t,
				      const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

std::shared_ptr<const std_int_type> std_int_type::strip_qualifiers() const
{
  return _strip_qualifiers(_self_ptr<std_int_type>(), &std_int_type::_clone);
}

mpa::limbs std_int_type::get_size(const architecture &arch) const
{
  return arch.get_std_int_size(_k);
}

mpa::limbs::size_type std_int_type::get_type_alignment(const architecture &arch)
  const noexcept
{
  return arch.get_std_int_alignment(_k);
}

std::shared_ptr<const std_int_type>
std_int_type::set_user_alignment(const alignment &user_align) const
{
  return _set_user_alignment(_self_ptr<std_int_type>(),
			     user_align, &std_int_type::_clone);
}

std::shared_ptr<const arithmetic_type>
std_int_type::arithmetic_conversion(const architecture &arch,
				    const arithmetic_type &at) const
{
  return at.arithmetic_conversion(arch, *this);
}

std::shared_ptr<const arithmetic_type>
std_int_type::arithmetic_conversion(const architecture &arch,
				    const real_float_type &ft) const
{
  return ft.arithmetic_conversion(arch, *this);
}

std::shared_ptr<const arithmetic_type>
std_int_type::arithmetic_conversion(const architecture &arch,
				    const complex_float_type &ct) const
{
  return ct.arithmetic_conversion(arch, *this);
}

std::shared_ptr<const std_int_type>
std_int_type::integer_conversion(const architecture &arch, const int_type &it)
  const
{
  return it.integer_conversion(arch, *this);
}

std::shared_ptr<const std_int_type>
std_int_type::integer_conversion(const architecture &arch,
				 const std_int_type &it) const
{
  if (_k < kind::k_int)
    return this->promote(arch)->integer_conversion(arch, it);
  else if (it._k < kind::k_int)
    return this->integer_conversion(arch, *it.promote(arch));

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

  } else if (arch.get_std_int_width(t_signed._k) >
	     arch.get_std_int_width(t_unsigned._k)) {
    return t_signed.strip_qualifiers();

  } else {
    return std_int_type::create(t_signed._k, false);
  }
}

bool std_int_type::is_signed(const architecture&) const noexcept
{
  return _signed;
}

mpa::limbs::size_type std_int_type::get_width(const architecture &arch)
  const noexcept
{
  return arch.get_std_int_width(_k);
}

std::shared_ptr<const std_int_type>
std_int_type::promote(const architecture&) const
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

std::shared_ptr<const returnable_int_type> std_int_type::to_unsigned() const
{
  assert(_signed);
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

bool plain_char_type::is_compatible_with(const architecture &arch,
					 const type &t,
					 const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool plain_char_type::is_compatible_with(const architecture&,
					 const plain_char_type &t,
					 const bool ignore_qualifiers) const
{
  return ignore_qualifiers || (this->get_qualifiers() == t.get_qualifiers());
}

mpa::limbs plain_char_type::get_size(const architecture&) const
{
  return mpa::limbs::from_size_type(1);
}

mpa::limbs::size_type
plain_char_type::get_type_alignment(const architecture&) const noexcept
{
  return 0;
}

bool plain_char_type::is_signed(const architecture &arch) const noexcept
{
  return arch.is_char_signed();
}

mpa::limbs::size_type plain_char_type::get_width(const architecture &arch)
  const noexcept
{
  return arch.get_std_int_width(std_int_type::kind::k_char);
}

std::shared_ptr<const std_int_type>
plain_char_type::promote(const architecture&) const
{
  return std_int_type::create(std_int_type::kind::k_int, true,
			      get_qualifiers());
}

std::shared_ptr<const returnable_int_type> plain_char_type::to_unsigned() const
{
  return std_int_type::create(std_int_type::kind::k_char, false,
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

bool bool_type::is_compatible_with(const architecture &arch, const type &t,
				   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool bool_type::is_compatible_with(const architecture&, const bool_type &t,
				   const bool ignore_qualifiers) const
{
  return ignore_qualifiers || (this->get_qualifiers() == t.get_qualifiers());
}

mpa::limbs bool_type::get_size(const architecture&) const
{
  return mpa::limbs::from_size_type(1);
}

mpa::limbs::size_type
bool_type::get_type_alignment(const architecture&) const noexcept
{
  return 0;
}

bool bool_type::is_signed(const architecture&) const noexcept
{
  return false;
}

mpa::limbs::size_type bool_type::get_width(const architecture&)
  const noexcept
{
  return 1;
}

std::shared_ptr<const std_int_type>
bool_type::promote(const architecture&) const
{
  return std_int_type::create(std_int_type::kind::k_int, true,
			      get_qualifiers());
}

std::shared_ptr<const returnable_int_type> bool_type::to_unsigned() const
{
  // A _Bool is always unsigned, no need to call to_unsigned() on it.
  assert(0);
  return nullptr;
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
			      const architecture &arch)
{
  if (_members.empty()) {
    // The initial value is zero and the target_int's width will eventually
    // get adjusted in architecture::evaluate_enum_type().
    const mpa::limbs::size_type int_width =
      arch.get_std_int_width(types::std_int_type::kind::k_int);
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

void enum_content::set_underlying_type(std::shared_ptr<const std_int_type> &&ut)
  noexcept
{
  assert(!_underlying_type);
  _underlying_type = std::move(ut);
}

const std::shared_ptr<const std_int_type>&
enum_content::get_underlying_type() const noexcept
{
  assert(_underlying_type);
  return _underlying_type;
}


enum_type::enum_type(const ast::enum_def &ed, const qualifiers qs)
  : type(qs), _ed(ed)
{}

enum_type::enum_type(const enum_type&) = default;

enum_type::~enum_type() noexcept = default;

enum_type* enum_type::_clone() const
{
  return new enum_type(*this);
}

std::shared_ptr<const enum_type>
enum_type::create(const ast::enum_def &ed, const qualifiers &qs)
{
  return std::shared_ptr<const enum_type>(new enum_type(ed, qs));
}

type::type_id enum_type::get_type_id() const noexcept
{
  return type_id::tid_enum;
}

bool enum_type::is_compatible_with(const architecture &arch, const type &t,
				   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool enum_type::is_compatible_with(const architecture&, const enum_type &t,
				   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  return &_ed == &t._ed;
}

bool enum_type::is_compatible_with(const architecture &arch,
				   const std_int_type &t,
				   const bool ignore_qualifiers) const
{
  if (!ignore_qualifiers && (this->get_qualifiers() != t.get_qualifiers()))
    return false;

  return this->get_underlying_type()->is_compatible_with(arch, t, false);
}

mpa::limbs enum_type::get_size(const architecture &arch) const
{
  return get_underlying_type()->get_size(arch);
}

mpa::limbs::size_type enum_type::get_type_alignment(const architecture &arch)
  const noexcept
{
  return get_underlying_type()->get_type_alignment(arch);
}

bool enum_type::is_signed(const architecture &arch) const noexcept
{
  return get_underlying_type()->is_signed(arch);
}

mpa::limbs::size_type enum_type::get_width(const architecture &arch)
  const noexcept
{
  return get_underlying_type()->get_width(arch);
}

std::shared_ptr<const std_int_type> enum_type::promote(const architecture &arch)
  const
{
  return this->get_underlying_type()->promote(arch);
}

std::shared_ptr<const returnable_int_type> enum_type::to_unsigned() const
{
  return get_underlying_type()->to_unsigned();
}

const std::shared_ptr<const std_int_type>& enum_type::get_underlying_type()
  const noexcept
{
  return _ed.get_enumerator_list().get_content().get_underlying_type();
}


bitfield_type::
bitfield_type(std::shared_ptr<const returnable_int_type> &&base_type,
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
bitfield_type::create(std::shared_ptr<const returnable_int_type> &&base_type,
		      const mpa::limbs::size_type width)
{
  return (std::shared_ptr<const bitfield_type>
	  (new bitfield_type(std::move(base_type), width)));
}

type::type_id bitfield_type::get_type_id() const noexcept
{
  return type_id::tid_bitfield;
}

bool bitfield_type::is_compatible_with(const architecture &arch, const type &t,
				       const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool bitfield_type::is_signed(const architecture &arch) const noexcept
{
  return _base_type->is_signed(arch);
}

mpa::limbs::size_type bitfield_type::get_width(const architecture&)
  const noexcept
{
  return _width;
}

std::shared_ptr<const std_int_type>
bitfield_type::promote(const architecture &arch) const
{
  const int_mode_kind m = width_to_int_mode(_width);
  const std_int_type::kind k = arch.int_mode_to_std_int_kind(m);
  return std_int_type::create(k, _base_type->is_signed(arch));
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


float_type::float_type(const kind k)
  : _k(k)
{}

float_type::float_type(const float_type&) = default;

float_type::~float_type() noexcept = default;

mpa::limbs::size_type
float_type::get_significand_width(const architecture &arch) const noexcept
{
  return arch.get_float_significand_width(_k);
}

mpa::limbs::size_type
float_type::get_exponent_width(const architecture &arch) const noexcept
{
  return arch.get_float_exponent_width(_k);
}


real_float_type::real_float_type(const kind k, const qualifiers &qs)
  : type(qs), float_type(k)
{}

real_float_type::real_float_type(const real_float_type&) = default;

real_float_type::~real_float_type() noexcept = default;

real_float_type* real_float_type::_clone() const
{
  return new real_float_type(*this);
}

std::shared_ptr<const real_float_type>
real_float_type::create(const kind k, const qualifiers &qs)
{
  return (std::shared_ptr<const real_float_type>
	  (new real_float_type(k, qs)));
}

type::type_id real_float_type::get_type_id() const noexcept
{
  return type_id::tid_real_float;
}

bool real_float_type::is_compatible_with(const architecture &arch,
					 const type &t,
					 const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool real_float_type::is_compatible_with(const architecture&,
					 const real_float_type &t,
					 const bool ignore_qualifiers) const
{
  return (this->get_kind() == t.get_kind() &&
	  (ignore_qualifiers ||
	   (this->get_qualifiers() == t.get_qualifiers())));
}

mpa::limbs real_float_type::get_size(const architecture &arch) const
{
  return arch.get_float_size(this->get_kind(), false);
}

mpa::limbs::size_type
real_float_type::get_type_alignment(const architecture &arch) const noexcept
{
  return arch.get_float_alignment(this->get_kind(), false);
}

std::shared_ptr<const arithmetic_type>
real_float_type::arithmetic_conversion(const architecture &arch,
				       const arithmetic_type &at) const
{
  return at.arithmetic_conversion(arch, *this);
}

std::shared_ptr<const arithmetic_type>
real_float_type::arithmetic_conversion(const architecture&,
				       const std_int_type&) const
{
  return this->arithmetic_type::strip_qualifiers();
}

std::shared_ptr<const arithmetic_type>
real_float_type::arithmetic_conversion(const architecture&,
				       const real_float_type &ft) const
{
  if (this->get_kind() >= ft.get_kind()) {
    return this->arithmetic_type::strip_qualifiers();
  } else {
    return ft.arithmetic_type::strip_qualifiers();
  }
}

std::shared_ptr<const arithmetic_type>
real_float_type::arithmetic_conversion(const architecture &arch,
				       const complex_float_type &ct) const
{
  return ct.arithmetic_conversion(arch, *this);
}

std::shared_ptr<const float_type> real_float_type::promote() const
{
  if (this->get_kind() == kind::k_float)
    return real_float_type::create(kind::k_double, this->get_qualifiers());

  return _self_ptr<real_float_type>();
}


complex_float_type::complex_float_type(const kind k, const qualifiers &qs)
  : type(qs), float_type(k)
{}

complex_float_type::complex_float_type(const complex_float_type&) = default;

complex_float_type::~complex_float_type() noexcept = default;

complex_float_type* complex_float_type::_clone() const
{
  return new complex_float_type(*this);
}

std::shared_ptr<const complex_float_type>
complex_float_type::create(const kind k, const qualifiers &qs)
{
  return (std::shared_ptr<const complex_float_type>
	  (new complex_float_type(k, qs)));
}

type::type_id complex_float_type::get_type_id() const noexcept
{
  return type_id::tid_complex_float;
}

bool complex_float_type::is_compatible_with(const architecture &arch,
					    const type &t,
					    const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}

bool complex_float_type::is_compatible_with(const architecture&,
					    const complex_float_type &t,
					    const bool ignore_qualifiers) const
{
  return (this->get_kind() == t.get_kind() &&
	  (ignore_qualifiers ||
	   (this->get_qualifiers() == t.get_qualifiers())));
}

mpa::limbs complex_float_type::get_size(const architecture &arch) const
{
  return arch.get_float_size(this->get_kind(), true);
}

mpa::limbs::size_type
complex_float_type::get_type_alignment(const architecture &arch) const noexcept
{
  return arch.get_float_alignment(this->get_kind(), true);
}

std::shared_ptr<const arithmetic_type>
complex_float_type::arithmetic_conversion(const architecture &arch,
					  const arithmetic_type &at) const
{
  return at.arithmetic_conversion(arch, *this);
}

std::shared_ptr<const arithmetic_type>
complex_float_type::arithmetic_conversion(const architecture&,
					  const std_int_type&) const
{
  return this->arithmetic_type::strip_qualifiers();
}

std::shared_ptr<const arithmetic_type>
complex_float_type::arithmetic_conversion(const architecture&,
					  const real_float_type &ft) const
{
  if (this->get_kind() >= ft.get_kind()) {
    return this->arithmetic_type::strip_qualifiers();
  } else {
    return complex_float_type::create(ft.get_kind());
  }
}

std::shared_ptr<const arithmetic_type>
complex_float_type::arithmetic_conversion(const architecture&,
					  const complex_float_type &ct) const
{
  if (this->get_kind() >= ct.get_kind()) {
    return this->arithmetic_type::strip_qualifiers();
  } else {
    return ct.arithmetic_type::strip_qualifiers();
  }
}

std::shared_ptr<const float_type> complex_float_type::promote() const
{
  if (this->get_kind() == kind::k_float)
    return complex_float_type::create(kind::k_double, this->get_qualifiers());

  return _self_ptr<complex_float_type>();
}


builtin_func_type::
builtin_func_type(const builtin_func::factory builtin_func_fac)
  : _builtin_func_fac(builtin_func_fac)
{}

builtin_func_type::builtin_func_type(const builtin_func_type&) = default;

builtin_func_type::~builtin_func_type() noexcept = default;

builtin_func_type* builtin_func_type::_clone() const
{
  return new builtin_func_type(*this);
}

std::shared_ptr<const builtin_func_type>
builtin_func_type::create(const builtin_func::factory builtin_func_fac)
{
  return (std::shared_ptr<const builtin_func_type>
	  (new builtin_func_type(builtin_func_fac)));
}

type::type_id builtin_func_type::get_type_id() const noexcept
{
  return type_id::tid_builtin_func;
}

bool builtin_func_type::is_compatible_with(const architecture &arch,
					   const type &t,
					   const bool ignore_qualifiers) const
{
  return t.is_compatible_with(arch, *this, ignore_qualifiers);
}
