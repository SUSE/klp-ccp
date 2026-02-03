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

#include "builtins_impl.hh"
#include "ast.hh"
#include "ast_impl.hh"
#include "ast_evaluate.hh"
#include "constexpr_value.hh"
#include "target/target.hh"
#include "semantic_except.hh"

using namespace klp::ccp;
using namespace klp::ccp::builtins::impl;
using namespace klp::ccp::ast;
using namespace klp::ccp::types;

builtin_func_simple_proto::builtin_func_simple_proto
	(const bool variadic,
	 std::shared_ptr<const types::addressable_type> &&ret_type,
	 exp_arg_types_type &&exp_arg_types) noexcept
  : _variadic(variadic), _ret_type(std::move(ret_type)),
    _exp_arg_types(std::move(exp_arg_types))
{}

builtin_func_simple_proto::~builtin_func_simple_proto() noexcept = default;

builtin_func::evaluation_result_type builtin_func_simple_proto::
evaluate(klp::ccp::ast::ast &a, const target &tgt,
	 expr_func_invocation &efi) const
{
  expr_list * const args = efi.get_args();
  const std::size_t n_args = !args ? 0 : args->size();

  if (n_args < _exp_arg_types.size()) {
    code_remark remark
      (code_remark::severity::fatal,
       "too few arguments in builtin function invocation",
       a.get_pp_result(), efi.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);

  } else if (!_variadic && n_args > _exp_arg_types.size()) {
    code_remark remark
      (code_remark::severity::fatal,
       "too many arguments in builtin function invocation",
       a.get_pp_result(), efi.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (args)
    args->apply_lvalue_conversions(false);

  std::size_t i = 0;
  for (const auto &t : _exp_arg_types) {
    check_types_assignment(a, tgt, *t, (*args)[i], true);
    ++i;
  }

  return evaluation_result_type{_ret_type, nullptr, false};
}


std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_v(const target&)
{
  return void_type::create();
}
std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_i(const target&)
{
  return std_int_type::create(std_int_type::kind::k_int, true);
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_u(const target&)
{
  return std_int_type::create(std_int_type::kind::k_int, false);
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_l(const target&)
{
  return std_int_type::create(std_int_type::kind::k_long, true);
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_ul(const target&)
{
  return std_int_type::create(std_int_type::kind::k_long, false);
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_ll(const target&)
{
  return std_int_type::create(std_int_type::kind::k_long_long, true);
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_ull(const target&)
{
  return std_int_type::create(std_int_type::kind::k_long_long, false);
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_b(const target&)
{
  return bool_type::create();
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_imax(const target &tgt)
{
  return tgt.create_int_max_type(true);
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_umax(const target &tgt)
{
  return tgt.create_int_max_type(false);
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_sz(const target &tgt)
{
  return tgt.create_ptrdiff_type(false);
}

std::shared_ptr<const int_type>
klp::ccp::builtins::impl::mk_ssz(const target &tgt)
{
  return tgt.create_ptrdiff_type(true);
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_f(const target&)
{
  return std_float_type::create(std_float_type::kind::k_float);
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_d(const target&)
{
  return std_float_type::create(std_float_type::kind::k_double);
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_ld(const target&)
{
  return std_float_type::create(std_float_type::kind::k_long_double);
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_fC(const target&)
{
  return (complex_float_type::create
	  (std_float_type::create(std_float_type::kind::k_float)));
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_dC(const target&)
{
  return (complex_float_type::create
	  (std_float_type::create(std_float_type::kind::k_double)));
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_ldC(const target&)
{
  return (complex_float_type::create
	  (std_float_type::create(std_float_type::kind::k_long_double)));
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pv(const target&)
{
  return void_type::create()->derive_pointer();
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pcv(const target&)
{
  return (void_type::create(qualifiers{qualifier::q_const})
	  ->derive_pointer());
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pvv(const target&)
{
  return (void_type::create(qualifiers{qualifier::q_volatile})
	  ->derive_pointer());
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pcvv(const target&)
{
  return (void_type::create(qualifiers{qualifier::q_const,
	  qualifier::q_volatile})
    ->derive_pointer());
}
std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pc(const target&)
{
  return plain_char_type::create()->derive_pointer();
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pcc(const target&)
{
  return (plain_char_type::create(qualifiers{qualifier::q_const})
	  ->derive_pointer());
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pi(const target&)
{
  return (std_int_type::create(std_int_type::kind::k_int, true)
	  ->derive_pointer());
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pf(const target&)
{
  return (std_float_type::create(std_float_type::kind::k_float)
	  ->derive_pointer());
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pd(const target&)
{
  return (std_float_type::create(std_float_type::kind::k_double)
	  ->derive_pointer());
}

std::shared_ptr<const addressable_type>
klp::ccp::builtins::impl::mk_pld(const target&)
{
  return (std_float_type::create(std_float_type::kind::k_long_double)
	  ->derive_pointer());
}


builtin_func_constant_p::builtin_func_constant_p() noexcept = default;

builtin_func_constant_p::~builtin_func_constant_p() noexcept = default;

builtin_func::evaluation_result_type
builtin_func_constant_p::evaluate(klp::ccp::ast::ast &a, const target &tgt,
				  expr_func_invocation &efi) const
{
  const expr_list * const args = efi.get_args();
  const std::size_t n_args = !args ? 0 : args->size();

  if (n_args != 1) {
    code_remark remark
      (code_remark::severity::warning,
       "wrong number of arguments to __builtin_constant_p()",
       a.get_pp_result(), efi.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  std::shared_ptr<const std_int_type> it_result
    = std_int_type::create(std_int_type::kind::k_int, true);
  const bool is_signed = it_result->is_signed(tgt);
  const mpa::limbs::size_type prec =
    it_result->get_width(tgt) - is_signed;

  std::unique_ptr<constexpr_value> value;
  const expr &e = (*args)[0];
  if (e.is_constexpr() &&
      ((e.get_constexpr_value().has_constness
	(constexpr_value::constness::c_arithmetic_constant_expr)) ||
       (e.get_constexpr_value().has_constness
	(constexpr_value::constness::c_address_constant)))) {
    value.reset(new constexpr_value
		(constexpr_value::integer_constant_expr_tag{},
		 target_int::create_one(is_signed, prec)));
  } else {
    value.reset(new constexpr_value
		(constexpr_value::integer_constant_expr_tag{},
		 target_int::create_zero(is_signed, prec)));
  }

  return evaluation_result_type{std::move(it_result),
				std::move(value),
				false};
}

std::unique_ptr<builtin_func> builtin_func_constant_p::create()
{
  return (std::unique_ptr<builtin_func_constant_p>
	  (new builtin_func_constant_p()));
}
