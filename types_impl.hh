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

#ifndef TYPES_IMPL_HH
#define TYPES_IMPL_HH

#include <cassert>
#include <type_traits>
#include <utility>
#include "callables_wrapper.hh"
#include "type_set.hh"

namespace klp
{
  namespace ccp
  {
    namespace types
    {
      // template <typename type_type>
      // std::shared_ptr<const type_type>
      // type::reset_user_requested_alignment(alignment &&user_align) const
      // {
      // 	std::unique_ptr<type> clone(_clone());
      // 	clone->_user_align = std::move(user_align);
      // 	std::shared_ptr<const type_type> new_type
      // 	  (dynamic_cast<const type_type*>(clone.get()));
      // 	assert(new_type);
      // 	clone.release();
      // 	return new_type;
      // }


      template <typename callable_type>
      void enum_content::for_each_member(callable_type &&c)
      {
	for (auto &m : _members)
	  c(m);
      }


      template<typename callable_type>
      void struct_or_union_content::for_each_member_flat(callable_type &&c)
	const
      {
	for (const auto &m : _members) {
	  if (!m.get_name().empty()) {
	    c(m);
	  } else {
	    // Possibly an unnamed struct or union
	    const struct_or_union_type * const sou =
	      dynamic_cast<const struct_or_union_type*>(m.get_type().get());
	    if (sou) {
	      assert(sou->get_content());
	      sou->get_content()->for_each_member_flat(c);
	    } else {
	      c(m);
	    }
	  }
	}
      }

      namespace impl
      {
	template<bool cond, template <typename> class trans, typename T>
	struct _cond_type_transform
	{
	  using type =
	    typename std::conditional<cond, typename trans<T>::type, T>::type;
	};

	template <typename callable_type, typename arg_type>
	struct _bind_1st_impl
	{
	private:
	  callable_type &&_c;
	  arg_type _arg;

	public:
	  template <typename __arg_type>
	  _bind_1st_impl(callable_type &&c, __arg_type &&arg) noexcept
	    : _c(std::forward<callable_type>(c)),
	      _arg(std::forward<__arg_type>(arg))
	  {}

	  template <typename... args_types>
	  auto operator()(args_types &&...args)
	    ->decltype(_c(std::forward<arg_type>(_arg),
			  std::forward<args_types>(args)...))
	  {
	    return (_c(std::forward<arg_type>(_arg),
		       std::forward<args_types>(args)...));
	  }
	};

	template <typename derived_type, typename arg_type>
	struct _downcast_and_bind_1st_impl
	{
	private:
	  using ___new_arg_type =
	    typename _cond_type_transform
	      <std::is_const<typename
			     std::remove_reference<arg_type>::type>::value,
	       std::add_const,
	       derived_type>
	    ::type;

	  using __new_arg_type =
	    typename _cond_type_transform
	      <std::is_lvalue_reference<arg_type>::value,
	       std::add_lvalue_reference,
	       ___new_arg_type>
	    ::type;

	  using _new_arg_type =
	    typename std::add_rvalue_reference<__new_arg_type>::type;

	public:
	  template<typename callable_type>
	  _bind_1st_impl<callable_type, _new_arg_type>
	  operator()(callable_type &&c, arg_type &&arg)
	  {
	    // A dynamic_cast between references cannot get eliminated,
	    // because it can throw. Here it is known that the cast
	    // will succeed. Cast between pointer types instead and
	    // turn the result into a reference afterwards.
	    //
	    // As a side effect, this also works around
	    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69390
	    using new_arg_type_ptr =
	      typename std::add_pointer
		<typename std::remove_reference<_new_arg_type>::type>
	      ::type;

	    return (_bind_1st_impl<callable_type, _new_arg_type>
		    (std::forward<callable_type>(c),
		     (std::forward<_new_arg_type>
		      (*dynamic_cast<new_arg_type_ptr>(&arg)))));
	  }
	};

	template <typename derived_type, typename callable_type,
		  typename arg_type>
	decltype(std::declval<_downcast_and_bind_1st_impl<derived_type,
							  arg_type>>()
		 (std::declval<callable_type>(), std::declval<arg_type>()))
	_downcast_and_bind_1st(callable_type &&c, arg_type &&arg)
	{
	  return (_downcast_and_bind_1st_impl<derived_type, arg_type>()
		  (std::forward<callable_type>(c),
		   std::forward<arg_type>(arg)));
	}

	template <typename derived_type, typename element_type>
	struct _downcast_and_bind_1st_impl<derived_type,
					   const std::shared_ptr<element_type>&>
	{
	private:
	  using _new_element_type =
	    typename _cond_type_transform
	      <std::is_const<element_type>::value,
	       std::add_const,
	       derived_type>
	    ::type;

	public:
	  template<typename callable_type>
	  _bind_1st_impl<callable_type,
			 std::shared_ptr<_new_element_type>>
	  operator()(callable_type &&c,
		     const std::shared_ptr<element_type> &arg)
	  {
	    return (_bind_1st_impl<callable_type,
				   std::shared_ptr<_new_element_type>>
		    (std::forward<callable_type>(c),
		     std::dynamic_pointer_cast<_new_element_type>(arg)));
	  }
	};

	template <typename derived_type, typename element_type>
	struct _downcast_and_bind_1st_impl<derived_type,
					   std::shared_ptr<element_type>&>
	{
	private:
	  using _new_element_type =
	    typename _cond_type_transform
	      <std::is_const<element_type>::value,
	       std::add_const,
	       derived_type>
	    ::type;

	  using _arg_type = const std::shared_ptr<element_type>&;

	public:
	  template<typename callable_type>
	  _bind_1st_impl<callable_type,
			 std::shared_ptr<_new_element_type>>
	  operator()(callable_type &&c, std::shared_ptr<element_type> &arg)
	  {
	    return (_downcast_and_bind_1st_impl<derived_type, _arg_type>()
		    (std::forward<callable_type>(c), arg));
	  }
	};

	template <typename derived_type, typename element_type>
	struct _downcast_and_bind_1st_impl<derived_type,
					   const std::shared_ptr<element_type>>
	{
	private:
	  using _new_element_type =
	    typename _cond_type_transform
	      <std::is_const<element_type>::value,
	       std::add_const,
	       derived_type>
	    ::type;

	  using _arg_type = const std::shared_ptr<element_type>&;

	public:
	  template<typename callable_type>
	  _bind_1st_impl<callable_type,
			 std::shared_ptr<_new_element_type>>
	  operator()(callable_type &&c,
		     const std::shared_ptr<element_type> &&arg)
	  {
	    return (_downcast_and_bind_1st_impl<derived_type, _arg_type>()
		    (std::forward<callable_type>(c), arg));
	  }
	};

	template <typename derived_type, typename element_type>
	struct _downcast_and_bind_1st_impl<derived_type,
					   std::shared_ptr<element_type>>
	{
	private:
	  using _new_element_type =
	    typename _cond_type_transform
	      <std::is_const<element_type>::value,
	       std::add_const,
	       derived_type>
	    ::type;

	  using _arg_type = const std::shared_ptr<element_type>&;

	public:
	  template<typename callable_type>
	  _bind_1st_impl<callable_type,
			 std::shared_ptr<_new_element_type>>
	  operator()(callable_type &&c, std::shared_ptr<element_type> &&arg)
	  {
	    return (_downcast_and_bind_1st_impl<derived_type, _arg_type>()
		    (std::forward<callable_type>(c), arg));
	  }
	};


	template <typename ret_type, typename callable_type,
		  typename... args_types>
	struct _handle_types_impl;

	template <typename ret_type, typename callable_type,
		  typename... args_types>
	ret_type _handle_types(callable_type &&c, args_types &&...args)
	{
	  return
	    _handle_types_impl<ret_type, callable_type, args_types...>()
	    (std::forward<callable_type>(c), std::forward<args_types>(args)...);
	}

	template<typename arg_type, typename enable = void>
	struct _arg_type_id
	{
	  static type::type_id get(arg_type &&arg) noexcept
	  {
	    return arg.get_type_id();
	  }
	};

	template<typename arg_type>
	struct _arg_type_id<arg_type, decltype(*std::declval<arg_type &&>(),
					       std::declval<void>())>
	  {
	    static type::type_id get(arg_type &&arg) noexcept
	    {
	      return arg->get_type_id();
	    }
	};

	template <typename ret_type, typename callable_type,
		  typename arg_type, typename... args_types>
	struct _handle_types_impl<ret_type, callable_type, arg_type,
				  args_types...>
	{
	public:
	  ret_type operator()(callable_type &&c, arg_type &&arg,
			      args_types &&...args)
	  {
	    switch(_arg_type_id<arg_type>::get(std::forward<arg_type>(arg))) {
	    case type::type_id::tid_prototyped_function:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<prototyped_function_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_unprototyped_function:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<unprototyped_function_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_array:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<array_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_void:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<void_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_pointer:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<pointer_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_struct_or_union:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<struct_or_union_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_std_int:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<std_int_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_plain_char:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<plain_char_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_bool:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<bool_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_enum:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<enum_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_bitfield:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<bitfield_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_std_float:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<std_float_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_complex_float:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<complex_float_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));

	    case type::type_id::tid_builtin_func:
	      return (_handle_types<ret_type>
		      ((_downcast_and_bind_1st<builtin_func_type>
			(std::forward<callable_type>(c),
			 std::forward<arg_type>(arg))),
		       std::forward<args_types>(args)...));
	    };
	    __builtin_unreachable();
	  }
	};

	template <typename ret_type, typename callable_type>
	struct _handle_types_impl<ret_type, callable_type>
	{
	  ret_type operator()(callable_type &&c)
	  {
	    return c();
	  }
	};
      }

      template <typename ret_type, typename callable_type,
		typename... args_types>
      ret_type handle_types(callable_type &&c, args_types &&...args)
      {
	return (impl::_handle_types<ret_type>
		(std::forward<callable_type>(c),
		 std::forward<args_types>(args)...));
      }

      template <typename type, typename arg_type>
      bool is_type(arg_type &&arg) noexcept
      {
	return (handle_types<bool>
		((wrap_callables<default_action_return_value<bool, false>::type>
		  ([](const type&) { return true; })),
		 std::forward<arg_type>(arg)));
      }

      template <typename arg_type>
      bool is_scalar_type(arg_type &&arg) noexcept
      {
	return (is_type<arithmetic_type>(std::forward<arg_type>(arg)) ||
		is_type<bitfield_type>(std::forward<arg_type>(arg)) ||
		is_type<pointer_type>(std::forward<arg_type>(arg)));
      }

    }
  }
}

#endif
