/*
 * Copyright (C) 2020  SUSE Software Solutions Germany GmbH
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

#ifndef BUILTINS_IMPL_HH
#define BUILTINS_IMPL_HH

#include "builtins.hh"
#include <vector>
#include <functional>

namespace klp
{
  namespace ccp
  {
    namespace builtins
    {
      namespace impl
      {
	class builtin_func_simple_proto final : public builtin_func
	{
	private:
	  typedef std::vector<std::shared_ptr<const types::addressable_type>>
		  exp_arg_types_type;

	public:
	  virtual ~builtin_func_simple_proto() noexcept override;

	  virtual evaluation_result_type
	  evaluate(klp::ccp::ast::ast &a, const target &tgt,
		   ast::expr_func_invocation &efi) const override;

	  template<typename target_type,
		   typename ret_type_fac_type,
		   typename... exp_arg_types_facs_types>
	  static builtin_func::factory
	  factory(const target_type &tgt, const bool variadic,
		  const ret_type_fac_type &ret_type_fac,
		  const exp_arg_types_facs_types &...exp_arg_types_facs);

	private:
	  builtin_func_simple_proto
		(const bool variadic,
		 std::shared_ptr<const types::addressable_type> &&ret_type,
		 exp_arg_types_type &&exp_arg_types) noexcept;

	public:
	  template <typename target_type,
		    typename ret_type_fac_type,
		    typename... exp_arg_types_facs_types>
	  static std::unique_ptr<builtin_func>
	  _create(const std::reference_wrapper<const target_type> &tgt,
		  const bool variadic,
		  const ret_type_fac_type &ret_type_fac,
		  const exp_arg_types_facs_types &...exp_arg_types_facs)
	  {
	    exp_arg_types_type exp_arg_types =
	      { exp_arg_types_facs(tgt.get())... };
	    return (std::unique_ptr<builtin_func>
		    {new builtin_func_simple_proto(variadic,
						   ret_type_fac(tgt.get()),
						   std::move(exp_arg_types))});
	  }

	  const bool _variadic;
	  std::shared_ptr<const types::addressable_type> _ret_type;
	  exp_arg_types_type _exp_arg_types;
	};

	template<typename target_type,
		 typename ret_type_fac_type,
		 typename... exp_arg_types_facs_types>
	builtin_func::factory builtin_func_simple_proto::
	factory(const target_type &tgt, const bool variadic,
		const ret_type_fac_type &ret_type_fac,
		const exp_arg_types_facs_types &...exp_arg_types_facs)
	{
	  auto b = std::bind
	    (_create<target_type,
		     typename std::decay<ret_type_fac_type>::type,
		     typename std::decay<exp_arg_types_facs_types>::type...>,
	     std::cref(tgt),
	     variadic,
	     ret_type_fac,
	     exp_arg_types_facs...);
	  return b;
	}


	std::shared_ptr<const types::addressable_type>
	mk_v(const target&);

	std::shared_ptr<const types::int_type>
	mk_i(const target&);

	std::shared_ptr<const types::int_type>
	mk_u(const target&);

	std::shared_ptr<const types::int_type>
	mk_l(const target&);

	std::shared_ptr<const types::int_type>
	mk_ul(const target&);

	std::shared_ptr<const types::int_type>
	mk_ll(const target&);

	std::shared_ptr<const types::int_type>
	mk_ull(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_b(const target&);

	std::shared_ptr<const types::int_type>
	mk_imax(const target &tgt);

	std::shared_ptr<const types::int_type>
	mk_umax(const target &tgt);

	std::shared_ptr<const types::int_type>
	mk_sz(const target &tgt);

	std::shared_ptr<const types::int_type>
	mk_ssz(const target &tgt);

	std::shared_ptr<const types::addressable_type>
	mk_f(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_d(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_ld(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_fC(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_dC(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_ldC(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pv(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pcv(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pvv(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pcvv(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pc(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pcc(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pi(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pf(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pd(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_pld(const target&);


	class builtin_func_constant_p final : public builtin_func
	{
	public:
	  builtin_func_constant_p() noexcept;

	  virtual ~builtin_func_constant_p() noexcept override;

	  virtual evaluation_result_type
	  evaluate(klp::ccp::ast::ast &a, const target &tgt,
		   ast::expr_func_invocation &efi) const override;

	  static std::unique_ptr<builtin_func> create();
	};
      }
    }
  }
}

#endif
