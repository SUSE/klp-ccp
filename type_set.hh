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

#ifndef TYPE_SET_HH
#define TYPE_SET_HH

#include <type_traits>
#include <utility>
#include <cassert>

namespace klp
{
  namespace ccp
  {
    template<typename... types>
    class type_set;

    template<typename enable,
	     template<typename> class types_member_accessor,
	     typename mask_type_set, typename result, typename pending_type_set>
    struct closure_impl;

    template <template<typename> class types_member_accessor,
	      typename mask_type_set, typename result,
	      typename T, typename... types>
    struct closure_impl<
      typename std::enable_if<!result::template is_member<T>() &&
			      !mask_type_set::template is_member<T>()>::type,
      types_member_accessor, mask_type_set, result,
      type_set<T, types...>
      >
    {
      typedef typename
      closure_impl
      <void,
       types_member_accessor, mask_type_set,
       typename result::template add_type<T>,
       typename type_set<types...>::
	 template add_types<types_member_accessor<T> >
       >::type
      type;
    };

    template <template<typename> class types_member_accessor,
	      typename mask_type_set, typename result,
	      typename T, typename... types>
    struct closure_impl<
      typename std::enable_if<!result::template is_member<T>() &&
			      mask_type_set::template is_member<T>()>::type,
      types_member_accessor, mask_type_set, result,
      type_set<T, types...>
      >
    {
      typedef typename
      closure_impl
      <void,
       types_member_accessor, mask_type_set,
       typename result::template add_type<T>,
       type_set<types...>
       >::type
      type;
    };

    template <template<typename> class types_member_accessor,
	      typename mask_type_set, typename result,
	      typename T, typename... types>
    struct closure_impl<
      typename std::enable_if<result::template is_member<T>()>::type,
      types_member_accessor, mask_type_set, result,
      type_set<T, types...>
      >
    {
      typedef typename
      closure_impl
      <void,
       types_member_accessor, mask_type_set, result,
       type_set<types...>
       >::type
      type;
    };

    template <template<typename> class types_member_accessor,
	      typename mask_type_set, typename result>
    struct closure_impl<void,
			types_member_accessor, mask_type_set, result,
			type_set<> >
    {
      typedef result type;
    };


    template<typename result, typename... pending_types>
    struct add_const_impl;

    template<typename result, typename T, typename... pending_types>
    struct add_const_impl<result, T, pending_types...>
    {
      typedef typename
      add_const_impl
	<typename result::template add_type<typename std::add_const<T>::type>,
	 pending_types...>::type
      type;
    };

    template <typename result>
    struct add_const_impl<result>
    {
      typedef result type;
    };


    template<typename T, typename... types>
    class type_set<T, types...>
    {
    public:
      static constexpr std::size_t size()
      {
	return 1 + sizeof...(types);
      }

      template<typename U>
      static constexpr bool is_member()
      {
	return (std::is_same<T,
			     typename std::remove_reference<U>::type>::value ||
		type_set<types...>::template is_member<U>());
      }

    private:
      template<typename U, typename = void>
      struct add_type_impl
      {
	typedef type_set<T, types..., U> type;
      };

      template<typename U>
      struct add_type_impl<U, typename std::enable_if<is_member<U>()>::type>
      {
	typedef type_set<T, types...> type;
      };

    public:
      template<typename U>
      using add_type = typename add_type_impl<U>::type;


    private:
      template<typename new_type_set>
      struct add_types_impl
      {
	typedef type_set type;
      };

      template<typename U, typename... new_types>
      struct add_types_impl<type_set<U, new_types...> >
      {
	typedef typename
	add_type<U>::template add_types_impl<type_set<new_types...> >::type
	type;
      };

    public:
      template<typename new_type_set>
      using add_types = typename add_types_impl<new_type_set>::type;

      template<template<typename> class types_member_accessor,
	       typename mask_type_set>
      using closure = typename closure_impl<void, types_member_accessor,
					    mask_type_set,
					    type_set<>, type_set
					    >::type;

      using add_const = typename add_const_impl<type_set<>,
						T, types...>::type;

    private:
      template<typename... other_types>
      friend class type_set;

    public:
      template<typename U, typename callable_type,
	       typename return_type
		 = decltype(std::declval<callable_type>()(std::declval<T&>()))>
      static return_type cast_and_call(callable_type &&callable, U &&value)
      {
	static_assert(!std::is_pointer<T>::value,
		      "casting to pointer is not implemented");

	const typename std::add_pointer<T>::type ptr_value =
	  dynamic_cast<typename std::add_pointer<T>::type>(&value);
	if (ptr_value) {
	  return callable(*ptr_value);
	} else {
	  return (type_set<types...>::
		  template cast_and_call<U, callable_type, return_type>
				(std::forward<callable_type>(callable),
				 std::forward<U>(value)));
	}
      }
    };

    template<>
    class type_set<>
    {
    public:
      static constexpr std::size_t size()
      {
	return 0;
      }

      template<typename U>
      static constexpr bool is_member()
      {
	return false;
      }

      template<typename U>
      using add_type = type_set<U>;

      template<typename new_type_set>
      using add_types = new_type_set;

      template<template<typename> class types_member_accessor>
      using closure = type_set<>;

    private:
      template<typename... other_types>
      friend class type_set;

      template<typename U, typename callable_type, typename return_type>
      static return_type cast_and_call(callable_type &&callable, U &&value)
      {
	return callable(std::forward<U>(value));
      }
    };
  }
}

#endif
