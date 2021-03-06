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

#ifndef CALLABLES_WRAPPER_HH
#define CALLABLES_WRAPPER_HH

#include <utility>
#include <type_traits>

namespace klp
{
  namespace ccp
  {
    namespace impl
    {
      template<typename enable, typename callable_type, typename... args_types>
      class _has_overload : public std::false_type
      {};

      template<typename callable_type, typename... args_types>
      class _has_overload
      <decltype(std::declval<callable_type>()(std::declval<args_types>()...),
		std::declval<void>()),
       callable_type, args_types...>
	: public std::true_type
      {};

      template<typename callable_type, typename... args_types>
      using has_overload =
	_has_overload<void, callable_type, args_types...>;
    }

    template<template<typename... __args_types> class default_action,
	     typename... callables_types>
    class callables_wrapper;

    template<template<typename... __args_types> class default_action,
	     typename callable_type, typename... callables_types>
    class callables_wrapper<default_action,
			    callable_type, callables_types...>
    {
    public:
      callables_wrapper(callable_type &&c, callables_types&&... cs) noexcept
	: _c(std::forward<callable_type>(c)),
	  _cs(std::forward<callables_types>(cs)...)
      {}

      static constexpr std::size_t size()
      {
	return 1 + sizeof...(callables_types);
      }

    private:
      callable_type _c;
      callables_wrapper<default_action, callables_types...> _cs;

    private:
      template<template<typename... __args_types> class __default_action,
	       typename... __callable_types>
      friend class callables_wrapper;

      template<typename enable, typename... args_types>
      struct _has_overload :
	public callables_wrapper<default_action, callables_types...>::
			template has_overload<args_types...>
      {};

      template<typename... args_types>
      struct _has_overload
      <typename std::enable_if<impl::has_overload<callable_type&&,
						  args_types...>
			       ::value>
		::type,
       args_types...>
	: public std::true_type
      {};

    public:
      template<typename... args_types>
      using has_overload = _has_overload<void, args_types...>;

    private:
      template<typename enable, typename... args_types>
      struct _caller;

      // This one handles the case where this instance can't handle the
      // arguments' types but either the tail can or there is a
      // non-no_default_action default_action.
      template<typename... args_types>
      struct _caller
      <typename std::enable_if
	<(!impl::has_overload<callable_type&&, args_types...>::value &&
	  (has_overload<args_types...>::value ||
	  !std::is_same<default_action<args_types...>, void>::value))>::type,
       args_types...>
      {
	_caller(callable_type&,
		callables_wrapper<default_action, callables_types...> &cs)
	  noexcept
	  : _cs(cs)
	{}

	callables_wrapper<default_action, callables_types...> &_cs;

	auto operator()(args_types&&... args) ->
	  decltype(_cs(std::forward<args_types>(args)...))
	{
	  return _cs(std::forward<args_types>(args)...);
	}
      };

      // This handles the case where this instance can handle the
      // arguments' types.
      template<typename... args_types>
      struct _caller
      <typename std::enable_if<impl::has_overload<callable_type&&,
						  args_types...>
			       ::value>
		::type,
       args_types...>
      {
	_caller(callable_type &c,
		callables_wrapper<default_action, callables_types...>&)
	  noexcept
	  : _c(c)
	{}

	callable_type &_c;

	auto operator()(args_types&&... args) ->
	  decltype(_c(std::forward<args_types>(args)...))
	{
	  return _c(std::forward<args_types>(args)...);
	}
      };

    public:
      template<typename... args_types>
      auto operator()(args_types&&... args) ->
	decltype(std::declval<_caller<void, args_types...> >()
		 (std::forward<args_types>(args)...))
      {
	_caller<void, args_types...> caller(_c, _cs);
	return caller(std::forward<args_types>(args)...);
      }
    };

    template<template<typename... __args_types> class default_action>
    class callables_wrapper<default_action>
    {
    public:
      callables_wrapper() noexcept
      {}

      static constexpr std::size_t size()
      {
	return 0;
      }

      template<typename... args_types>
      using has_overload = std::false_type;

      // No match for the arguments' types. Invoke the default_action
      // if it is not no_default_action.
      template<typename... args_types>
      auto operator()(args_types&&... args) ->
	typename std::enable_if
	  <!std::is_same<default_action<args_types...>, void>::value,
	   decltype(std::declval<const default_action<args_types...> >()
		    (std::forward<args_types>(args)...))>
	::type
      {
	const default_action<args_types...> _caller;
	return _caller(std::forward<args_types>(args)...);
      }
    };

    template<typename...>
    using no_default_action = void;

    template <typename val_type, val_type val>
    class default_action_return_value
    {
    private:
      template<typename... args_types>
      struct impl
      {
	impl() noexcept
	{}

	val_type operator()(args_types&&...) const noexcept
	{
	  return val;
	}
      };

    public:
      template<typename... args_types>
      using type = impl<args_types...>;
    };

    template<typename... args_types>
    struct default_action_nop
    {
      default_action_nop() noexcept
      {}

      void operator()(args_types&&...) const noexcept
      {}
    };

    template <typename ret_type, typename mask>
    class default_action_unreachable
    {
    private:

      template<typename enable, typename... args_types>
      struct impl;

      template<typename arg_type>
      struct impl
	<typename std::enable_if<!mask::template is_member<arg_type>()>::type,
	 arg_type>
      {
	impl() noexcept
	{}

	ret_type operator()(arg_type&&) const noexcept
	{
	  assert(0);
	  __builtin_unreachable();
	}
      };

      template<typename arg_type, typename... args_types>
      struct impl
	<typename std::enable_if<!mask::template is_member<arg_type>()>::type,
	 arg_type, args_types...>
      {
	impl() noexcept
	{}

	ret_type operator()(arg_type&&, args_types&&... args) const noexcept
	{
	  return impl<void, args_types...>()(std::forward<args_types>(args)...);
	}
      };

      public:
      template<typename... args_types>
      using type = impl<void, args_types...>;
    };


    template<template<typename... __args_types> class default_action,
	     typename... callables_types>
    callables_wrapper<default_action, callables_types...>
    wrap_callables(callables_types&&... cs)
    {
      return (callables_wrapper<default_action, callables_types...>
	      (std::forward<callables_types>(cs)...));
    }
  }
}

#endif
