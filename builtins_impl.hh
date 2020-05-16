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
#include <initializer_list>

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
	public:
	  typedef std::shared_ptr<const types::addressable_type>
	  (*t_fac)(const target&);

	  builtin_func_simple_proto
		(const t_fac ret_fac,
		 const std::initializer_list<t_fac> &exp_arg_facs,
		 const bool variadic) noexcept;

	  virtual ~builtin_func_simple_proto() noexcept override;

	  virtual evaluation_result_type
	  evaluate(klp::ccp::ast::ast &a, const target &tgt,
		   const ast::expr_func_invocation &efi) const override;

	  template <bool variadic, t_fac ret_fac, t_fac... exp_arg_facs>
	  struct fac
	  {
	    static std::unique_ptr<builtin_func> create()
	    {
	      static const std::initializer_list<t_fac> _exp_arg_facs =
		{ exp_arg_facs... };
	      return (std::unique_ptr<builtin_func>
		      (new builtin_func_simple_proto(ret_fac, _exp_arg_facs,
						     variadic)));
	    }
	  };

	private:
	  const t_fac _ret_fac;
	  const std::initializer_list<t_fac> &_exp_arg_facs;
	  const bool _variadic;
	};

	std::shared_ptr<const types::addressable_type>
	mk_v(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_i(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_u(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_l(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_ul(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_ll(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_ull(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_b(const target&);

	std::shared_ptr<const types::addressable_type>
	mk_imax(const target &tgt);

	std::shared_ptr<const types::addressable_type>
	mk_umax(const target &tgt);

	std::shared_ptr<const types::addressable_type>
	mk_sz(const target &tgt);

	std::shared_ptr<const types::addressable_type>
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


	class builtin_func_choose_expr final : public builtin_func
	{
	public:
	  virtual ~builtin_func_choose_expr() noexcept override;

	  virtual evaluation_result_type
	  evaluate(klp::ccp::ast::ast &a, const target &tgt,
		   const ast::expr_func_invocation &efi) const override;

	  static std::unique_ptr<builtin_func> create();

	private:
	  builtin_func_choose_expr() noexcept;
	};


	class builtin_func_constant_p final : public builtin_func
	{
	public:
	  builtin_func_constant_p() noexcept;

	  virtual ~builtin_func_constant_p() noexcept override;

	  virtual evaluation_result_type
	  evaluate(klp::ccp::ast::ast &a, const target &tgt,
		   const ast::expr_func_invocation &efi) const override;

	  static std::unique_ptr<builtin_func> create();
	};
      }
    }
  }
}

#endif
