#include "builtins.hh"
#include "ast.hh"
#include "ast_impl.hh"
#include "constexpr_value.hh"
#include "ast_evaluate.hh"
#include "architecture.hh"
#include "semantic_except.hh"
#include <map>
#include <functional>
#include <initializer_list>

using namespace suse::cp;
using namespace suse::cp::ast;
using namespace suse::cp::types;

constexpr builtin_func::builtin_func() noexcept = default;
builtin_func::~builtin_func() noexcept = default;


namespace
{
  typedef std::shared_ptr<const addressable_type>
    (*t_fac)(const architecture&);

  class builtin_func_simple_proto final : public builtin_func
  {
  public:
    builtin_func_simple_proto(const t_fac ret_fac,
			      const std::initializer_list<t_fac> &exp_arg_facs,
			      const bool variadic) noexcept;

    virtual ~builtin_func_simple_proto() noexcept override;

    virtual evaluation_result_type
    evaluate(suse::cp::ast::ast &a, const architecture &arch,
	     const expr_func_invocation &efi) const override;

  private:
    const t_fac _ret_fac;
    const std::initializer_list<t_fac> &_exp_arg_facs;
    const bool _variadic;
  };

  template <bool variadic, t_fac ret_fac, t_fac... exp_arg_facs>
  struct builtin_func_simple_proto_fac
  {
    static std::unique_ptr<builtin_func> create()
    {
      static const std::initializer_list<t_fac> _exp_arg_facs =
	{ exp_arg_facs... };
      return (std::unique_ptr<builtin_func>
	      (new builtin_func_simple_proto(ret_fac, _exp_arg_facs, variadic)));
    }
  };
}

builtin_func_simple_proto::
builtin_func_simple_proto(const t_fac ret_fac,
			  const std::initializer_list<t_fac> &exp_arg_facs,
			  const bool variadic) noexcept
  : _ret_fac(ret_fac), _exp_arg_facs(exp_arg_facs), _variadic(variadic)
{}

builtin_func_simple_proto::~builtin_func_simple_proto() noexcept = default;

builtin_func::evaluation_result_type builtin_func_simple_proto::
evaluate(suse::cp::ast::ast &a, const architecture &arch,
	 const expr_func_invocation &efi) const
{
  const expr_list * const args = efi.get_args();
  const std::size_t n_args = !args ? 0 : args->size();

  if (n_args < _exp_arg_facs.size()) {
    const pp_token &tok =
      a.get_pp_tokens()[efi.get_tokens_range().begin];
    code_remark remark
      (code_remark::severity::warning,
       "too few arguments in builtin function invocation",
       tok.get_file_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);

  } else if (!_variadic && n_args > _exp_arg_facs.size()) {
    const pp_token &tok =
      a.get_pp_tokens()[efi.get_tokens_range().begin];
    code_remark remark
      (code_remark::severity::fatal,
       "too many arguments in builtin function invocation",
       tok.get_file_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  std::size_t i = 0;
  for (const auto tf : _exp_arg_facs) {
    check_types_assignment(a, arch, *tf(arch), (*args)[i]);
    ++i;
  }

  return evaluation_result_type{_ret_fac(arch), nullptr, false};
}


namespace
{
  class builtin_func_pi_overload final : public builtin_func
  {
  public:
    builtin_func_pi_overload(const unsigned int pi_arg_index,
			     const builtin_func::factory fac8,
			     const builtin_func::factory fac16,
			     const builtin_func::factory fac32,
			     const builtin_func::factory fac64,
			     const builtin_func::factory fac128) noexcept;

    virtual ~builtin_func_pi_overload() noexcept override;

    virtual evaluation_result_type
    evaluate(suse::cp::ast::ast &a, const architecture &arch,
	     const expr_func_invocation &efi) const override;

  private:
    const unsigned int _pi_arg_index;
    const builtin_func::factory _fac8;
    const builtin_func::factory _fac16;
    const builtin_func::factory _fac32;
    const builtin_func::factory _fac64;
    const builtin_func::factory _fac128;
  };

  template <const unsigned int pi_arg_index,
	    const builtin_func::factory fac8,
	    const builtin_func::factory fac16,
	    const builtin_func::factory fac32,
	    const builtin_func::factory fac64,
	    const builtin_func::factory fac128>
  struct builtin_func_pi_overload_fac
  {
    static std::unique_ptr<builtin_func> create()
    {
      return (std::unique_ptr<builtin_func>
	      (new builtin_func_pi_overload(pi_arg_index, fac8, fac16,
					    fac32, fac64, fac128)));;
    }
  };
}

builtin_func_pi_overload::
builtin_func_pi_overload(const unsigned int pi_arg_index,
			 const builtin_func::factory fac8,
			 const builtin_func::factory fac16,
			 const builtin_func::factory fac32,
			 const builtin_func::factory fac64,
			 const builtin_func::factory fac128) noexcept
  : _pi_arg_index(pi_arg_index), _fac8(fac8), _fac16(fac16),
    _fac32(fac32), _fac64(fac64), _fac128(fac128)
{}

builtin_func_pi_overload::~builtin_func_pi_overload() noexcept = default;

builtin_func::evaluation_result_type builtin_func_pi_overload::
evaluate(suse::cp::ast::ast &a, const architecture &arch,
	     const expr_func_invocation &efi) const
{
  const expr_list * const args = efi.get_args();
  const std::size_t n_args = !args ? 0 : args->size();

  if (n_args <= _pi_arg_index) {
    const pp_token &tok =
      a.get_pp_tokens()[efi.get_tokens_range().begin];
    code_remark remark
      (code_remark::severity::warning,
       "too few arguments in builtin function invocation",
       tok.get_file_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  // The argument at index pi_arg_index shall have pointer to integer
  // type. Demultiplex according to that integer type's width.
  const expr &e_pi_arg = (*args)[_pi_arg_index];
  const mpa::limbs::size_type width =
    (handle_types<mpa::limbs::size_type>
     ((wrap_callables<default_action_nop>
       ([&](const pointer_type &pt) {
	  return
	    (handle_types<mpa::limbs::size_type>
	     ((wrap_callables<default_action_nop>
	       ([&](const int_type &_it) {
		  return _it.get_width(arch);
		},
		[&](const pointer_type&) {
		  // Undocumented, but gcc also accepts pointers to pointers
		  // for the atomic primitives.
		  return arch.get_std_int_width(arch.get_ptrdiff_kind());
		},
		[&](const type&) -> mpa::limbs::size_type {
		  const pp_token &tok =
		    a.get_pp_tokens()[e_pi_arg.get_tokens_range().begin];
		  code_remark remark
		    (code_remark::severity::warning,
		     "expected pointer to int argument to builtin invocation",
		     tok.get_file_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		})),
	      *pt.get_pointed_to_type()));
	},
	[&](const type&) -> mpa::limbs::size_type {
	  const pp_token &tok =
	    a.get_pp_tokens()[e_pi_arg.get_tokens_range().begin];
	  code_remark remark
	    (code_remark::severity::warning,
	     "expected pointer argument to builtin invocation",
	     tok.get_file_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	})),
      *e_pi_arg.get_type()));

  builtin_func::factory fac;
  switch (width) {
  case 8:
    fac = _fac8;
    break;

  case 16:
    fac = _fac16;
    break;

  case 32:
    fac = _fac32;
    break;

  case 64:
    fac = _fac64;
    break;

  case 128:
    fac = _fac128;
    break;

  default:
    {
      const pp_token &tok =
	a.get_pp_tokens()[e_pi_arg.get_tokens_range().begin];
      code_remark remark
	(code_remark::severity::warning,
	 "can't handle argument's integer width at builtin invocation",
	 tok.get_file_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }
  };

  return fac()->evaluate(a, arch, efi);
}


namespace
{
  class builtin_func_choose_expr final : public builtin_func
  {
  public:
    builtin_func_choose_expr() noexcept;

    virtual ~builtin_func_choose_expr() noexcept override;

    virtual evaluation_result_type
    evaluate(suse::cp::ast::ast &a, const architecture &arch,
	     const expr_func_invocation &efi) const override;
  };
}

builtin_func_choose_expr::builtin_func_choose_expr() noexcept = default;

builtin_func_choose_expr::~builtin_func_choose_expr() noexcept = default;

builtin_func::evaluation_result_type
builtin_func_choose_expr::evaluate(suse::cp::ast::ast &a, const architecture&,
				   const expr_func_invocation &efi) const
{
  const expr_list * const args = efi.get_args();
  const std::size_t n_args = !args ? 0 : args->size();

  if (n_args != 3) {
    const pp_token &tok =
      a.get_pp_tokens()[efi.get_tokens_range().begin];
    code_remark remark
      (code_remark::severity::warning,
       "wrong number of arguments to __builtin_choose_expr()",
       tok.get_file_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  // Note: GCC really requires an integer constant expression
  // for the first argument.
  const expr &e_cond = (*args)[0];
  if (!e_cond.is_constexpr() ||
      !(e_cond.get_constexpr_value().has_constness
	(constexpr_value::constness::c_integer_constant_expr))) {
    const pp_token &tok =
      a.get_pp_tokens()[e_cond.get_tokens_range().begin];
    code_remark remark
      (code_remark::severity::warning,
       "first argument to __builtin_choose_expr() is not a constant",
       tok.get_file_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const constexpr_value &cv_cond = e_cond.get_constexpr_value();
  const expr &e_result = cv_cond.is_zero() ? (*args)[2] : (*args)[1];
  std::unique_ptr<constexpr_value> value;
  if (e_result.is_constexpr()) {
    value = e_result.get_constexpr_value().clone();
  }

  return evaluation_result_type{e_result.get_type(),
				std::move(value),
				e_result.is_lvalue()};
}

std::unique_ptr<builtin_func> suse::cp::builtin_func_choose_expr_create()
{
  return (std::unique_ptr<builtin_func_choose_expr>
	  (new builtin_func_choose_expr()));
}


namespace
{
  class builtin_func_constant_p final : public builtin_func
  {
  public:
    builtin_func_constant_p() noexcept;

    virtual ~builtin_func_constant_p() noexcept override;

    virtual evaluation_result_type
    evaluate(suse::cp::ast::ast &a, const architecture &arch,
	     const expr_func_invocation &efi) const override;

    static std::unique_ptr<builtin_func> create();
  };
}

builtin_func_constant_p::builtin_func_constant_p() noexcept = default;

builtin_func_constant_p::~builtin_func_constant_p() noexcept = default;

builtin_func::evaluation_result_type
builtin_func_constant_p::evaluate(suse::cp::ast::ast &a, const architecture &arch,
				  const expr_func_invocation &efi) const
{
  const expr_list * const args = efi.get_args();
  const std::size_t n_args = !args ? 0 : args->size();

  if (n_args != 1) {
    const pp_token &tok =
      a.get_pp_tokens()[efi.get_tokens_range().begin];
    code_remark remark
      (code_remark::severity::warning,
       "wrong number of arguments to __builtin_constant_p()",
       tok.get_file_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  std::shared_ptr<const std_int_type> it_result
    = std_int_type::create(std_int_type::kind::k_int, true);
  const bool is_signed = it_result->is_signed(arch);
  const mpa::limbs::size_type prec =
    it_result->get_width(arch) - is_signed;

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


namespace
{
  static std::shared_ptr<const addressable_type>
  mk_v(const architecture&)
  {
    return void_type::create();
  }

  static std::shared_ptr<const addressable_type>
  mk_i(const architecture&)
  {
    return std_int_type::create(std_int_type::kind::k_int, true);
  }

  static std::shared_ptr<const addressable_type>
  mk_u(const architecture&)
  {
    return std_int_type::create(std_int_type::kind::k_int, false);
  }

  static std::shared_ptr<const addressable_type>
  mk_l(const architecture&)
  {
    return std_int_type::create(std_int_type::kind::k_long, true);
  }

  static std::shared_ptr<const addressable_type>
  mk_ul(const architecture&)
  {
    return std_int_type::create(std_int_type::kind::k_long, false);
  }

  static std::shared_ptr<const addressable_type>
  mk_ll(const architecture&)
  {
    return std_int_type::create(std_int_type::kind::k_long_long, true);
  }

  static std::shared_ptr<const addressable_type>
  mk_ull(const architecture&)
  {
    return std_int_type::create(std_int_type::kind::k_long_long, false);
  }

  static std::shared_ptr<const addressable_type>
  mk_b(const architecture&)
  {
    return bool_type::create();
  }

  static std::shared_ptr<const addressable_type>
  mk_imax(const architecture &arch)
  {
    return std_int_type::create(arch.get_int_max_kind(), true);
  }

  static std::shared_ptr<const addressable_type>
  mk_umax(const architecture &arch)
  {
    return std_int_type::create(arch.get_int_max_kind(), false);
  }

  static std::shared_ptr<const addressable_type>
  mk_i8(const architecture &arch)
  {
    const int_mode_kind m = width_to_int_mode(8);
    return std_int_type::create(arch.int_mode_to_std_int_kind(m), true);
  }

  static std::shared_ptr<const addressable_type>
  mk_i16(const architecture &arch)
  {
    const int_mode_kind m = width_to_int_mode(16);
    return std_int_type::create(arch.int_mode_to_std_int_kind(m), true);
  }

  static std::shared_ptr<const addressable_type>
  mk_u16(const architecture &arch)
  {
    const int_mode_kind m = width_to_int_mode(16);
    return std_int_type::create(arch.int_mode_to_std_int_kind(m), false);
  }

  static std::shared_ptr<const addressable_type>
  mk_i32(const architecture &arch)
  {
    const int_mode_kind m = width_to_int_mode(32);
    return std_int_type::create(arch.int_mode_to_std_int_kind(m), true);
  }

  static std::shared_ptr<const addressable_type>
  mk_u32(const architecture &arch)
  {
    const int_mode_kind m = width_to_int_mode(32);
    return std_int_type::create(arch.int_mode_to_std_int_kind(m), false);
  }

  static std::shared_ptr<const addressable_type>
  mk_i64(const architecture &arch)
  {
    const int_mode_kind m = width_to_int_mode(64);
    return std_int_type::create(arch.int_mode_to_std_int_kind(m), true);
  }

  static std::shared_ptr<const addressable_type>
  mk_u64(const architecture &arch)
  {
    const int_mode_kind m = width_to_int_mode(64);
    return std_int_type::create(arch.int_mode_to_std_int_kind(m), false);
  }

  static std::shared_ptr<const addressable_type>
  mk_i128(const architecture &arch)
  {
    const int_mode_kind m = width_to_int_mode(128);
    return std_int_type::create(arch.int_mode_to_std_int_kind(m), true);
  }

  static std::shared_ptr<const addressable_type>
  mk_wi(const architecture &arch)
  {
    return std_int_type::create(arch.get_wint_kind(), arch.is_wint_signed());
  }

  static std::shared_ptr<const addressable_type>
  mk_sz(const architecture &arch)
  {
    return std_int_type::create(arch.get_ptrdiff_kind(), false);
  }

  static std::shared_ptr<const addressable_type>
  mk_ssz(const architecture &arch)
  {
    return std_int_type::create(arch.get_ptrdiff_kind(), true);
  }

  static std::shared_ptr<const addressable_type>
  mk_pid(const architecture &arch)
  {
    return std_int_type::create(arch.get_pid_t_kind(), arch.is_pid_t_signed());
  }

  static std::shared_ptr<const addressable_type>
  mk_f(const architecture&)
  {
    return real_float_type::create(real_float_type::kind::k_float);
  }

  static std::shared_ptr<const addressable_type>
  mk_d(const architecture&)
  {
    return real_float_type::create(real_float_type::kind::k_double);
  }

  static std::shared_ptr<const addressable_type>
  mk_ld(const architecture&)
  {
    return real_float_type::create(real_float_type::kind::k_long_double);
  }

  static std::shared_ptr<const addressable_type>
  mk_fC(const architecture&)
  {
    return complex_float_type::create(real_float_type::kind::k_float);
  }

  static std::shared_ptr<const addressable_type>
  mk_dC(const architecture&)
  {
    return complex_float_type::create(real_float_type::kind::k_double);
  }

  static std::shared_ptr<const addressable_type>
  mk_ldC(const architecture&)
  {
    return complex_float_type::create(real_float_type::kind::k_long_double);
  }

  static std::shared_ptr<const addressable_type>
  mk_pv(const architecture&)
  {
    return void_type::create()->derive_pointer();
  }

  static std::shared_ptr<const addressable_type>
  mk_pcv(const architecture&)
  {
    return (void_type::create(qualifiers{qualifier::q_const})
	    ->derive_pointer());
  }

  static std::shared_ptr<const addressable_type>
  mk_pvv(const architecture&)
  {
    return (void_type::create(qualifiers{qualifier::q_volatile})
	    ->derive_pointer());
  }

  static std::shared_ptr<const addressable_type>
  mk_pcvv(const architecture&)
  {
    return (void_type::create(qualifiers{qualifier::q_const,
					 qualifier::q_volatile})
	    ->derive_pointer());
  }
  static std::shared_ptr<const addressable_type>
  mk_pc(const architecture&)
  {
    return plain_char_type::create()->derive_pointer();
  }

  static std::shared_ptr<const addressable_type>
  mk_pcc(const architecture&)
  {
    return (plain_char_type::create(qualifiers{qualifier::q_const})
	    ->derive_pointer());
  }

  static std::shared_ptr<const addressable_type>
  mk_pi(const architecture&)
  {
    return (std_int_type::create(std_int_type::kind::k_int, true)
	    ->derive_pointer());
  }

  static std::shared_ptr<const addressable_type>
  mk_pf(const architecture&)
  {
    return (real_float_type::create(real_float_type::kind::k_float)
	    ->derive_pointer());
  }

  static std::shared_ptr<const addressable_type>
  mk_pd(const architecture&)
  {
    return (real_float_type::create(real_float_type::kind::k_double)
	    ->derive_pointer());
  }

  static std::shared_ptr<const addressable_type>
  mk_pld(const architecture&)
  {
    return (real_float_type::create(real_float_type::kind::k_long_double)
	    ->derive_pointer());
  }

  static std::shared_ptr<const addressable_type>
  mk_pF_v_var(const architecture&)
  {
    return (void_type::create()
	    ->derive_function(false)
	    ->derive_pointer());
  }

  static std::shared_ptr<const addressable_type>
  mk_val(const architecture &arch)
  {
    std::shared_ptr<const addressable_type> t =
      arch.create_builtin_va_list_type();
    handle_types<void>
      ((wrap_callables<default_action_nop>
	([&](const array_type &at) {
	   t = at.get_element_type()->derive_pointer();
	 })),
       *t);
    return t;
  }
}

using bfspf_v = builtin_func_simple_proto_fac<false, mk_v>;
using bfspf_v_i = builtin_func_simple_proto_fac<false, mk_v, mk_i>;
using bfspf_v_f_pf_pf = builtin_func_simple_proto_fac<false, mk_v, mk_f, mk_pf, mk_pf>;
using bfspf_v_d_pd_pd = builtin_func_simple_proto_fac<false, mk_v, mk_d, mk_pd, mk_pd>;
using bfspf_v_ld_pld_pld = builtin_func_simple_proto_fac<false, mk_v, mk_ld, mk_pld, mk_pld>;
using bfspf_v_ssz_pv = builtin_func_simple_proto_fac<false, mk_v, mk_ssz, mk_pv>;
using bfspf_v_pv = builtin_func_simple_proto_fac<false, mk_v, mk_pv>;
using bfspf_v_pv_i = builtin_func_simple_proto_fac<false, mk_v, mk_pv, mk_i>;
using bfspf_v_pv_sz = builtin_func_simple_proto_fac<false, mk_v, mk_pv, mk_sz>;
using bfspf_v_pv_pv = builtin_func_simple_proto_fac<false, mk_v, mk_pv, mk_pv>;
using bfspf_v_pcv_pv_sz = builtin_func_simple_proto_fac<false, mk_v, mk_pcv, mk_pv, mk_sz>;
using bfspf_v_pcvv_var = builtin_func_simple_proto_fac<true, mk_v, mk_pcvv>;
using bfspf_v_pvv = builtin_func_simple_proto_fac<false, mk_v, mk_pvv>;
using bfspf_v_pvv_i = builtin_func_simple_proto_fac<false, mk_v, mk_pvv, mk_i>;
using bfspf_v_pvv_pv_i = builtin_func_simple_proto_fac<false, mk_v, mk_pvv, mk_pv, mk_i>;
using bfspf_v_pvv_pv_pv_i = builtin_func_simple_proto_fac<false, mk_v, mk_pvv, mk_pv, mk_pv, mk_i>;
using bfspf_v_pvv_var = builtin_func_simple_proto_fac<true, mk_v, mk_pvv>;
using bfspf_v_pcvv_pv_i = builtin_func_simple_proto_fac<false, mk_v, mk_pcvv, mk_pv, mk_i>;
using bfspf_v_val = builtin_func_simple_proto_fac<false, mk_v, mk_val>;
using bfspf_v_val_val = builtin_func_simple_proto_fac<false, mk_v, mk_val, mk_val>;
using bfspf_v_val_var = builtin_func_simple_proto_fac<true, mk_v, mk_val>;
using bfspf_pv_pF_v_var__pv_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_pF_v_var, mk_pv, mk_sz>;
using bfspf_v_var = builtin_func_simple_proto_fac<true, mk_v>;

using bfspf_i = builtin_func_simple_proto_fac<false, mk_i>;
using bfspf_i_i = builtin_func_simple_proto_fac<false, mk_i, mk_i>;
using bfspf_i_i_i_i_i_i_var = builtin_func_simple_proto_fac<true, mk_i, mk_i, mk_i, mk_i, mk_i, mk_i>;
using bfspf_i_i_pv = builtin_func_simple_proto_fac<false, mk_i, mk_i, mk_pv>;
using bfspf_i_i_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_i, mk_pcc, mk_val>;
using bfspf_i_i_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_i, mk_pcc>;
using bfspf_i_u = builtin_func_simple_proto_fac<false, mk_i, mk_u>;
using bfspf_i_l = builtin_func_simple_proto_fac<false, mk_i, mk_l>;
using bfspf_i_ul = builtin_func_simple_proto_fac<false, mk_i, mk_ul>;
using bfspf_i_ll = builtin_func_simple_proto_fac<false, mk_i, mk_ll>;
using bfspf_i_ull = builtin_func_simple_proto_fac<false, mk_i, mk_ull>;
using bfspf_i_imax = builtin_func_simple_proto_fac<false, mk_i, mk_imax>;
using bfspf_i_umax = builtin_func_simple_proto_fac<false, mk_i, mk_umax>;
using bfspf_i_wi = builtin_func_simple_proto_fac<false, mk_i, mk_wi>;
using bfspf_i_f = builtin_func_simple_proto_fac<false, mk_i, mk_f>;
using bfspf_i_d = builtin_func_simple_proto_fac<false, mk_i, mk_d>;
using bfspf_i_ld = builtin_func_simple_proto_fac<false, mk_i, mk_ld>;
using bfspf_i_pv = builtin_func_simple_proto_fac<false, mk_i, mk_pv>;
using bfspf_i_pv_i_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_pv, mk_i, mk_pcc, mk_val>;
using bfspf_i_pv_i_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pv, mk_i, mk_pcc>;
using bfspf_i_pv_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_pv, mk_pcc, mk_val>;
using bfspf_i_pv_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pv, mk_pcc>;
using bfspf_i_pcv_pcv_sz = builtin_func_simple_proto_fac<false, mk_i, mk_pcv, mk_pcv, mk_sz>;
using bfspf_i_pc_i_sz_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pc, mk_i, mk_sz, mk_pcc>;
using bfspf_i_pc_i_sz_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_pc, mk_i, mk_sz, mk_pcc, mk_val>;
using bfspf_i_pc_sz_i_sz_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pc, mk_sz, mk_i, mk_sz, mk_pcc>;
using bfspf_i_pc_sz_i_sz_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_pc, mk_sz, mk_i, mk_sz, mk_pcc, mk_val>;
using bfspf_i_pc_sz_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_pc, mk_sz, mk_pcc, mk_val>;
using bfspf_i_pc_sz_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pc, mk_sz, mk_pcc>;
using bfspf_i_pc_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_pc, mk_pcc, mk_val>;
using bfspf_i_pc_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pc, mk_pcc>;
using bfspf_i_pcc = builtin_func_simple_proto_fac<false, mk_i, mk_pcc>;
using bfspf_i_pcc_sz_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pcc, mk_sz, mk_pcc>;
using bfspf_i_pcc_pv = builtin_func_simple_proto_fac<false, mk_i, mk_pcc, mk_pv>;
using bfspf_i_pcc_pv_pcc = builtin_func_simple_proto_fac<false, mk_i, mk_pcc, mk_pv, mk_pcc>;
using bfspf_i_pcc_pv_pcc_pv_pcc = builtin_func_simple_proto_fac<false, mk_i, mk_pcc, mk_pv, mk_pcc, mk_pv, mk_pcc>;
using bfspf_i_pcc_pcc = builtin_func_simple_proto_fac<false, mk_i, mk_pcc, mk_pcc>;
using bfspf_i_pcc_pcc_sz = builtin_func_simple_proto_fac<false, mk_i, mk_pcc, mk_pcc, mk_sz>;
using bfspf_i_pcc_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_pcc, mk_pcc, mk_val>;
using bfspf_i_pcc_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pcc, mk_pcc>;
using bfspf_i_pcc_val = builtin_func_simple_proto_fac<false, mk_i, mk_pcc, mk_val>;
using bfspf_i_pcc_var = builtin_func_simple_proto_fac<true, mk_i, mk_pcc>;
using bfspf_i_var = builtin_func_simple_proto_fac<true, mk_i>;

using bfspf_u = builtin_func_simple_proto_fac<false, mk_u, mk_ll>;

using bfspf_l_l = builtin_func_simple_proto_fac<false, mk_l, mk_l>;
using bfspf_l_l_l = builtin_func_simple_proto_fac<false, mk_l, mk_l, mk_l>;
using bfspf_l_f = builtin_func_simple_proto_fac<false, mk_l, mk_f>;
using bfspf_l_d = builtin_func_simple_proto_fac<false, mk_l, mk_d>;
using bfspf_l_ld = builtin_func_simple_proto_fac<false, mk_l, mk_ld>;

using bfspf_ll_ll = builtin_func_simple_proto_fac<false, mk_ll, mk_ll>;
using bfspf_ll_f = builtin_func_simple_proto_fac<false, mk_ll, mk_f>;
using bfspf_ll_d = builtin_func_simple_proto_fac<false, mk_ll, mk_d>;
using bfspf_ll_ld = builtin_func_simple_proto_fac<false, mk_ll, mk_ld>;

using bfspf_b_sz_pcvv = builtin_func_simple_proto_fac<false, mk_b, mk_sz, mk_pcvv>;
using bfspf_b_pvv_i = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_i>;
using bfspf_b_pvv_pv_pv_b_i_i = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_pv, mk_pv, mk_b, mk_i, mk_i>;
using bfspf_b_var = builtin_func_simple_proto_fac<true, mk_b>;

using bfspf_imax_imax = builtin_func_simple_proto_fac<false, mk_imax, mk_imax>;

using bfspf_u16_u16 = builtin_func_simple_proto_fac<false, mk_u16, mk_u16>;
using bfspf_u32_u32 = builtin_func_simple_proto_fac<false, mk_u32, mk_u32>;
using bfspf_u64_u64 = builtin_func_simple_proto_fac<false, mk_u64, mk_u64>;

using bfspf_wi_wi = builtin_func_simple_proto_fac<false, mk_wi, mk_wi>;

using bfspf_sz_pv = builtin_func_simple_proto_fac<false, mk_sz, mk_pv>;
using bfspf_sz_pcv_i = builtin_func_simple_proto_fac<false, mk_sz, mk_pcv, mk_i>;
using bfspf_sz_pc_sz_pcc_pcv = builtin_func_simple_proto_fac<false, mk_sz, mk_pc, mk_sz, mk_pcc, mk_pcv>;
using bfspf_sz_pcc = builtin_func_simple_proto_fac<false, mk_sz, mk_pcc>;
using bfspf_sz_pcc_sz_sz_pv = builtin_func_simple_proto_fac<false, mk_sz, mk_pcc, mk_sz, mk_sz, mk_pv>;
using bfspf_sz_pcc_pcc = builtin_func_simple_proto_fac<false, mk_sz, mk_pcc, mk_pcc>;

using bfspf_ssz_pc_sz_pcc_var = builtin_func_simple_proto_fac<true, mk_ssz, mk_pc, mk_sz, mk_pcc>;

using bfspf_pid = builtin_func_simple_proto_fac<false, mk_pid, mk_pv>;

using bfspf_f = builtin_func_simple_proto_fac<false, mk_f>;
using bfspf_f_i_f = builtin_func_simple_proto_fac<false, mk_f, mk_i, mk_f>;
using bfspf_f_f = builtin_func_simple_proto_fac<false, mk_f, mk_f>;
using bfspf_f_f_i = builtin_func_simple_proto_fac<false, mk_f, mk_f, mk_i>;
using bfspf_f_f_l = builtin_func_simple_proto_fac<false, mk_f, mk_f, mk_l>;
using bfspf_f_f_f = builtin_func_simple_proto_fac<false, mk_f, mk_f, mk_f>;
using bfspf_f_f_f_f = builtin_func_simple_proto_fac<false, mk_f, mk_f, mk_f, mk_f>;
using bfspf_f_f_f_pi = builtin_func_simple_proto_fac<false, mk_f, mk_f, mk_f, mk_pi>;
using bfspf_f_f_pi = builtin_func_simple_proto_fac<false, mk_f, mk_f, mk_pi>;
using bfspf_f_f_pf = builtin_func_simple_proto_fac<false, mk_f, mk_f, mk_pf>;
using bfspf_f_fC = builtin_func_simple_proto_fac<false, mk_f, mk_fC>;
using bfspf_f_pcc = builtin_func_simple_proto_fac<false, mk_f, mk_pcc>;

using bfspf_d = builtin_func_simple_proto_fac<false, mk_d>;
using bfspf_d_i_d = builtin_func_simple_proto_fac<false, mk_d, mk_i, mk_d>;
using bfspf_d_d = builtin_func_simple_proto_fac<false, mk_d, mk_d>;
using bfspf_d_d_i = builtin_func_simple_proto_fac<false, mk_d, mk_d, mk_i>;
using bfspf_d_d_l = builtin_func_simple_proto_fac<false, mk_d, mk_d, mk_l>;
using bfspf_d_d_d = builtin_func_simple_proto_fac<false, mk_d, mk_d, mk_d>;
using bfspf_d_d_d_d = builtin_func_simple_proto_fac<false, mk_d, mk_d, mk_d, mk_d>;
using bfspf_d_d_d_pi = builtin_func_simple_proto_fac<false, mk_d, mk_d, mk_d, mk_pi>;
using bfspf_d_d_pi = builtin_func_simple_proto_fac<false, mk_d, mk_d, mk_pi>;
using bfspf_d_d_pd = builtin_func_simple_proto_fac<false, mk_d, mk_d, mk_pd>;
using bfspf_d_dC = builtin_func_simple_proto_fac<false, mk_d, mk_dC>;
using bfspf_d_pcc = builtin_func_simple_proto_fac<false, mk_d, mk_pcc>;

using bfspf_ld = builtin_func_simple_proto_fac<false, mk_ld>;
using bfspf_ld_i_ld = builtin_func_simple_proto_fac<false, mk_ld, mk_i, mk_ld>;
using bfspf_ld_ld = builtin_func_simple_proto_fac<false, mk_ld, mk_ld>;
using bfspf_ld_ld_i = builtin_func_simple_proto_fac<false, mk_ld, mk_ld, mk_i>;
using bfspf_ld_ld_l = builtin_func_simple_proto_fac<false, mk_ld, mk_ld, mk_l>;
using bfspf_ld_ld_ld = builtin_func_simple_proto_fac<false, mk_ld, mk_ld, mk_ld>;
using bfspf_ld_ld_ld_ld = builtin_func_simple_proto_fac<false, mk_ld, mk_ld, mk_ld, mk_ld>;
using bfspf_ld_ld_ld_pi = builtin_func_simple_proto_fac<false, mk_ld, mk_ld, mk_ld, mk_pi>;
using bfspf_ld_ld_pi = builtin_func_simple_proto_fac<false, mk_ld, mk_ld, mk_pi>;
using bfspf_ld_ld_pld = builtin_func_simple_proto_fac<false, mk_ld, mk_ld, mk_pld>;
using bfspf_ld_ldC = builtin_func_simple_proto_fac<false, mk_ld, mk_ldC>;
using bfspf_ld_pcc = builtin_func_simple_proto_fac<false, mk_ld, mk_pcc>;

using bfspf_fC_f = builtin_func_simple_proto_fac<false, mk_fC, mk_f>;
using bfspf_fC_fC = builtin_func_simple_proto_fac<false, mk_fC, mk_fC>;
using bfspf_fC_fC_fC = builtin_func_simple_proto_fac<false, mk_fC, mk_fC, mk_fC>;

using bfspf_dC_d = builtin_func_simple_proto_fac<false, mk_dC, mk_d>;
using bfspf_dC_dC = builtin_func_simple_proto_fac<false, mk_dC, mk_dC>;
using bfspf_dC_dC_dC = builtin_func_simple_proto_fac<false, mk_dC, mk_dC, mk_dC>;

using bfspf_ldC_ld = builtin_func_simple_proto_fac<false, mk_ldC, mk_ld>;
using bfspf_ldC_ldC = builtin_func_simple_proto_fac<false, mk_ldC, mk_ldC>;
using bfspf_ldC_ldC_ldC = builtin_func_simple_proto_fac<false, mk_ldC, mk_ldC, mk_ldC>;


using bfspf_pv = builtin_func_simple_proto_fac<false, mk_pv>;
using bfspf_pv_u = builtin_func_simple_proto_fac<false, mk_pv, mk_u>;
using bfspf_pv_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_sz>;
using bfspf_pv_sz_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_sz, mk_sz>;
using bfspf_pv_pv = builtin_func_simple_proto_fac<false, mk_pv, mk_pv>;
using bfspf_pv_pv_i_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_pv, mk_i, mk_sz>;
using bfspf_pv_pv_i_sz_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_pv, mk_i, mk_sz, mk_sz>;
using bfspf_pv_pv_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_pv, mk_sz>;
using bfspf_pv_pv_pcv_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_pv, mk_pcv, mk_sz>;
using bfspf_pv_pv_pcv_sz_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_pv, mk_pcv, mk_sz, mk_sz>;
using bfspf_pv_pcv_i_sz = builtin_func_simple_proto_fac<false, mk_pv, mk_pcv, mk_i, mk_sz>;
using bfspf_pv_pcv_sz_var = builtin_func_simple_proto_fac<true, mk_pv, mk_pv, mk_sz>;
using bfspf_pv_var = builtin_func_simple_proto_fac<true, mk_pv>;

using bfspf_pc_pc_pcc = builtin_func_simple_proto_fac<false, mk_pc, mk_pc, mk_pcc>;
using bfspf_pc_pc_pcc_sz = builtin_func_simple_proto_fac<false, mk_pc, mk_pc, mk_pcc, mk_sz>;
using bfspf_pc_pcc = builtin_func_simple_proto_fac<false, mk_pc, mk_pcc>;
using bfspf_pc_pcc_i = builtin_func_simple_proto_fac<false, mk_pc, mk_pcc, mk_i>;
using bfspf_pc_pcc_sz = builtin_func_simple_proto_fac<false, mk_pc, mk_pcc, mk_sz>;
using bfspf_pc_pcc_pcc = builtin_func_simple_proto_fac<false, mk_pc, mk_pcc, mk_pcc>;
using bfspf_pc_pcc_pcc_i = builtin_func_simple_proto_fac<false, mk_pc, mk_pcc, mk_pcc, mk_i>;
using bfspf_pc_pc_pcc_sz_sz = builtin_func_simple_proto_fac<false, mk_pc, mk_pc, mk_pcc, mk_sz, mk_sz>;

using bfspf_pcc = builtin_func_simple_proto_fac<false, mk_pcc>;


using bfspf_i8_pvv_i8 = builtin_func_simple_proto_fac<false, mk_i8, mk_pvv, mk_i8>;
using bfspf_i16_pvv_i16 = builtin_func_simple_proto_fac<false, mk_i16, mk_pvv, mk_i16>;
using bfspf_i32_pvv_i32 = builtin_func_simple_proto_fac<false, mk_i32, mk_pvv, mk_i32>;
using bfspf_i64_pvv_i64 = builtin_func_simple_proto_fac<false, mk_i64, mk_pvv, mk_i64>;
using bfspf_i128_pvv_i128 = builtin_func_simple_proto_fac<false, mk_i128, mk_pvv, mk_i128>;

using bfspf_i8_pvv_i8_var = builtin_func_simple_proto_fac<true, mk_i8, mk_pvv, mk_i8>;
using bfspf_i16_pvv_i16_var = builtin_func_simple_proto_fac<true, mk_i16, mk_pvv, mk_i16>;
using bfspf_i32_pvv_i32_var = builtin_func_simple_proto_fac<true, mk_i32, mk_pvv, mk_i32>;
using bfspf_i64_pvv_i64_var = builtin_func_simple_proto_fac<true, mk_i64, mk_pvv, mk_i64>;
using bfspf_i128_pvv_i128_var = builtin_func_simple_proto_fac<true, mk_i128, mk_pvv, mk_i128>;
using bfpiof0_iN_pvv_iN_var = builtin_func_pi_overload_fac<0,
							   bfspf_i8_pvv_i8_var::create,
							   bfspf_i16_pvv_i16_var::create,
							   bfspf_i32_pvv_i32_var::create,
							   bfspf_i64_pvv_i64_var::create,
							   bfspf_i128_pvv_i128_var::create>;


using bfspf_b_pvv_i8_i8 = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_i8, mk_i8>;
using bfspf_b_pvv_i16_i16 = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_i16, mk_i16>;
using bfspf_b_pvv_i32_i32 = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_i32, mk_i32>;
using bfspf_b_pvv_i64_i64 = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_i64, mk_i64>;
using bfspf_b_pvv_i128_i128 = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_i128, mk_i128>;

using bfspf_b_pvv_i8_i8_var = builtin_func_simple_proto_fac<true, mk_b, mk_pvv, mk_i8, mk_i8>;
using bfspf_b_pvv_i16_i16_var = builtin_func_simple_proto_fac<true, mk_b, mk_pvv, mk_i16, mk_i16>;
using bfspf_b_pvv_i32_i32_var = builtin_func_simple_proto_fac<true, mk_b, mk_pvv, mk_i32, mk_i32>;
using bfspf_b_pvv_i64_i64_var = builtin_func_simple_proto_fac<true, mk_b, mk_pvv, mk_i64, mk_i64>;
using bfspf_b_pvv_i128_i128_var = builtin_func_simple_proto_fac<true, mk_b, mk_pvv, mk_i128, mk_i128>;
using bfpiof0_b_pvv_iN_iN_var = builtin_func_pi_overload_fac<0,
							     bfspf_b_pvv_i8_i8_var::create,
							     bfspf_b_pvv_i16_i16_var::create,
							     bfspf_b_pvv_i32_i32_var::create,
							     bfspf_b_pvv_i64_i64_var::create,
							     bfspf_b_pvv_i128_i128_var::create>;

using bfspf_i8_pvv_i8_i8 = builtin_func_simple_proto_fac<false, mk_i8, mk_pvv, mk_i8, mk_i8>;
using bfspf_i16_pvv_i16_i16 = builtin_func_simple_proto_fac<false, mk_i16, mk_pvv, mk_i16, mk_i16>;
using bfspf_i32_pvv_i32_i32 = builtin_func_simple_proto_fac<false, mk_i32, mk_pvv, mk_i32, mk_i32>;
using bfspf_i64_pvv_i64_i64 = builtin_func_simple_proto_fac<false, mk_i64, mk_pvv, mk_i64, mk_i64>;
using bfspf_i128_pvv_i128_i128 = builtin_func_simple_proto_fac<false, mk_i128, mk_pvv, mk_i128, mk_i128>;

using bfspf_i8_pvv_i8_i8_var = builtin_func_simple_proto_fac<true, mk_i8, mk_pvv, mk_i8, mk_i8>;
using bfspf_i16_pvv_i16_i16_var = builtin_func_simple_proto_fac<true, mk_i16, mk_pvv, mk_i16, mk_i16>;
using bfspf_i32_pvv_i32_i32_var = builtin_func_simple_proto_fac<true, mk_i32, mk_pvv, mk_i32, mk_i32>;
using bfspf_i64_pvv_i64_i64_var = builtin_func_simple_proto_fac<true, mk_i64, mk_pvv, mk_i64, mk_i64>;
using bfspf_i128_pvv_i128_i128_var = builtin_func_simple_proto_fac<true, mk_i128, mk_pvv, mk_i128, mk_i128>;
using bfpiof0_iN_pvv_iN_iN_var = builtin_func_pi_overload_fac<0,
							      bfspf_i8_pvv_i8_i8_var::create,
							      bfspf_i16_pvv_i16_i16_var::create,
							      bfspf_i32_pvv_i32_i32_var::create,
							      bfspf_i64_pvv_i64_i64_var::create,
							      bfspf_i128_pvv_i128_i128_var::create>;

using bfpiof0_v_pvv_var = builtin_func_pi_overload_fac<0,
						       bfspf_v_pvv_var::create,
						       bfspf_v_pvv_var::create,
						       bfspf_v_pvv_var::create,
						       bfspf_v_pvv_var::create,
						       bfspf_v_pvv_var::create>;


using bfspf_i8_pvv_i8_i = builtin_func_simple_proto_fac<false, mk_i8, mk_pvv, mk_i8, mk_i>;
using bfspf_i16_pvv_i16_i = builtin_func_simple_proto_fac<false, mk_i16, mk_pvv, mk_i16, mk_i>;
using bfspf_i32_pvv_i32_i = builtin_func_simple_proto_fac<false, mk_i32,  mk_pvv, mk_i32, mk_i>;
using bfspf_i64_pvv_i64_i = builtin_func_simple_proto_fac<false, mk_i64, mk_pvv, mk_i64, mk_i>;
using bfspf_i128_pvv_i128_i = builtin_func_simple_proto_fac<false, mk_i128, mk_pvv, mk_i128, mk_i>;

using bfpiof0_iN_pvv_iN_i = builtin_func_pi_overload_fac<0,
							 bfspf_i8_pvv_i8_i::create,
							 bfspf_i16_pvv_i16_i::create,
							 bfspf_i32_pvv_i32_i::create,
							 bfspf_i64_pvv_i64_i::create,
							 bfspf_i128_pvv_i128_i::create>;


using bfspf_i8_pcvv_i = builtin_func_simple_proto_fac<false, mk_i8, mk_pcvv, mk_i>;
using bfspf_i16_pcvv_i = builtin_func_simple_proto_fac<false, mk_i16, mk_pcvv, mk_i>;
using bfspf_i32_pcvv_i = builtin_func_simple_proto_fac<false, mk_i32, mk_pcvv, mk_i>;
using bfspf_i64_pcvv_i = builtin_func_simple_proto_fac<false, mk_i64, mk_pcvv, mk_i>;
using bfspf_i128_pcvv_i = builtin_func_simple_proto_fac<false, mk_i128, mk_pcvv, mk_i>;

using bfpiof0_iN_pcvv_i = builtin_func_pi_overload_fac<0,
						      bfspf_i8_pcvv_i::create,
						      bfspf_i16_pcvv_i::create,
						      bfspf_i32_pcvv_i::create,
						      bfspf_i64_pcvv_i::create,
						      bfspf_i128_pcvv_i::create>;


using bfspf_b_pvv_pv_i8_b_i_i = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_pv, mk_i8, mk_b, mk_i, mk_i>;
using bfspf_b_pvv_pv_i16_b_i_i = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_pv, mk_i16, mk_b, mk_i, mk_i>;
using bfspf_b_pvv_pv_i32_b_i_i = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_pv, mk_i32, mk_b, mk_i, mk_i>;
using bfspf_b_pvv_pv_i64_b_i_i = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_pv, mk_i64, mk_b, mk_i, mk_i>;
using bfspf_b_pvv_pv_i128_b_i_i = builtin_func_simple_proto_fac<false, mk_b, mk_pvv, mk_pv, mk_i128, mk_b, mk_i, mk_i>;

using bfpiof0_b_pvv_pv_iN_b_i_i = builtin_func_pi_overload_fac<0,
							       bfspf_b_pvv_pv_i8_b_i_i::create,
							       bfspf_b_pvv_pv_i16_b_i_i::create,
							       bfspf_b_pvv_pv_i32_b_i_i::create,
							       bfspf_b_pvv_pv_i64_b_i_i::create,
							       bfspf_b_pvv_pv_i128_b_i_i::create>;


using bfspf_v_pvv_i8_i = builtin_func_simple_proto_fac<false, mk_v, mk_pvv, mk_i8, mk_i>;
using bfspf_v_pvv_i16_i = builtin_func_simple_proto_fac<false, mk_v, mk_pvv, mk_i16, mk_i>;
using bfspf_v_pvv_i32_i = builtin_func_simple_proto_fac<false, mk_v,  mk_pvv, mk_i32, mk_i>;
using bfspf_v_pvv_i64_i = builtin_func_simple_proto_fac<false, mk_v, mk_pvv, mk_i64, mk_i>;
using bfspf_v_pvv_i128_i = builtin_func_simple_proto_fac<false, mk_v, mk_pvv, mk_i128, mk_i>;

using bfpiof0_v_pvv_iN_i = builtin_func_pi_overload_fac<0,
							 bfspf_v_pvv_i8_i::create,
							 bfspf_v_pvv_i16_i::create,
							 bfspf_v_pvv_i32_i::create,
							 bfspf_v_pvv_i64_i::create,
							 bfspf_v_pvv_i128_i::create>;


builtin_func::factory builtin_func::lookup(const std::string &id)
{
  static std::map<const std::string,
		  const builtin_func::factory > m = {
    // Category: math builtins.
    { "__builtin_acos", bfspf_d_d::create },
    { "__builtin_acosf", bfspf_f_f::create },
    { "__builtin_acosl", bfspf_ld_ld::create },
    { "__builtin_acosh", bfspf_d_d::create },
    { "__builtin_acoshf", bfspf_f_f::create },
    { "__builtin_acoshl", bfspf_ld_ld::create },
    { "__builtin_asin", bfspf_d_d::create },
    { "__builtin_asinf", bfspf_f_f::create },
    { "__builtin_asinl", bfspf_ld_ld::create },
    { "__builtin_asinh", bfspf_d_d::create },
    { "__builtin_asinhf", bfspf_f_f::create },
    { "__builtin_asinhl", bfspf_ld_ld::create },
    { "__builtin_atan", bfspf_d_d::create },
    { "__builtin_atanf", bfspf_f_f::create },
    { "__builtin_atanl", bfspf_ld_ld::create },
    { "__builtin_atanh", bfspf_d_d::create },
    { "__builtin_atanhf", bfspf_f_f::create },
    { "__builtin_atanhl", bfspf_ld_ld::create },
    { "__builtin_atan2", bfspf_d_d_d::create },
    { "__builtin_atan2f", bfspf_f_f_f::create },
    { "__builtin_atan2l", bfspf_ld_ld_ld::create },
    { "__builtin_cbrt", bfspf_d_d::create },
    { "__builtin_cbrtf", bfspf_f_f::create },
    { "__builtin_cbrtl", bfspf_ld_ld::create },
    { "__builtin_ceil", bfspf_d_d::create },
    { "__builtin_ceilf", bfspf_f_f::create },
    { "__builtin_ceill", bfspf_ld_ld::create },
    { "__builtin_copysign", bfspf_d_d_d::create },
    { "__builtin_copysignf", bfspf_f_f_f::create },
    { "__builtin_copysignl", bfspf_ld_ld_ld::create },
    { "__builtin_cos", bfspf_d_d::create },
    { "__builtin_cosf", bfspf_f_f::create },
    { "__builtin_cosl", bfspf_ld_ld::create },
    { "__builtin_cosh", bfspf_d_d::create },
    { "__builtin_coshf", bfspf_f_f::create },
    { "__builtin_coshl", bfspf_ld_ld::create },
    { "__builtin_drem", bfspf_d_d_d::create },
    { "__builtin_dremf", bfspf_f_f_f::create },
    { "__builtin_dreml", bfspf_ld_ld_ld::create },
    { "__builtin_erf", bfspf_d_d::create },
    { "__builtin_erff", bfspf_f_f::create },
    { "__builtin_erfl", bfspf_ld_ld::create },
    { "__builtin_erfc", bfspf_d_d::create },
    { "__builtin_erfcf", bfspf_f_f::create },
    { "__builtin_erfcl", bfspf_ld_ld::create },
    { "__builtin_exp", bfspf_d_d::create },
    { "__builtin_expf", bfspf_f_f::create },
    { "__builtin_expl", bfspf_ld_ld::create },
    { "__builtin_exp10", bfspf_d_d::create },
    { "__builtin_exp10f", bfspf_f_f::create },
    { "__builtin_exp10l", bfspf_ld_ld::create },
    { "__builtin_exp2", bfspf_d_d::create },
    { "__builtin_exp2f", bfspf_f_f::create },
    { "__builtin_exp2l", bfspf_ld_ld::create },
    { "__builtin_expm1", bfspf_d_d::create },
    { "__builtin_expm1f", bfspf_f_f::create },
    { "__builtin_expm1l", bfspf_ld_ld::create },
    { "__builtin_fabs", bfspf_d_d::create },
    { "__builtin_fabsf", bfspf_f_f::create },
    { "__builtin_fabsl", bfspf_ld_ld::create },
    { "__builtin_fdim", bfspf_d_d_d::create },
    { "__builtin_fdimf", bfspf_f_f_f::create },
    { "__builtin_fdiml", bfspf_ld_ld_ld::create },
    { "__builtin_floor", bfspf_d_d::create },
    { "__builtin_floor", bfspf_d_d::create },
    { "__builtin_floorf", bfspf_f_f::create },
    { "__builtin_floorl", bfspf_ld_ld::create },
    { "__builtin_fma", bfspf_d_d_d_d::create },
    { "__builtin_fmaf", bfspf_f_f_f_f::create },
    { "__builtin_fmal", bfspf_ld_ld_ld_ld::create },
    { "__builtin_fmax", bfspf_d_d_d::create },
    { "__builtin_fmaxf", bfspf_f_f_f::create },
    { "__builtin_fmaxl", bfspf_ld_ld_ld::create },
    { "__builtin_fmin", bfspf_d_d_d::create },
    { "__builtin_fminf", bfspf_f_f_f::create },
    { "__builtin_fminl", bfspf_ld_ld_ld::create },
    { "__builtin_fmod", bfspf_d_d_d::create },
    { "__builtin_fmodf", bfspf_f_f_f::create },
    { "__builtin_fmodl", bfspf_ld_ld_ld::create },
    { "__builtin_frexp", bfspf_d_d_pi::create },
    { "__builtin_frexpf", bfspf_f_f_pi::create },
    { "__builtin_frexpl", bfspf_ld_ld_pi::create },
    { "__builtin_gamma", bfspf_d_d::create },
    { "__builtin_gammaf", bfspf_f_f::create },
    { "__builtin_gammal", bfspf_ld_ld::create },
    { "__builtin_gamma_r", bfspf_d_d_pi::create },
    { "__builtin_gammaf_r", bfspf_f_f_pi::create },
    { "__builtin_gammal_r", bfspf_ld_ld_pi::create },
    { "__builtin_huge_val", bfspf_d::create },
    { "__builtin_huge_valf", bfspf_f::create },
    { "__builtin_huge_vall", bfspf_ld::create },
    { "__builtin_hypot", bfspf_d_d_d::create },
    { "__builtin_hypotf", bfspf_f_f_f::create },
    { "__builtin_hypotl", bfspf_ld_ld_ld::create },
    { "__builtin_iceil", bfspf_i_d::create },
    { "__builtin_iceilf", bfspf_i_f::create },
    { "__builtin_iceild", bfspf_i_ld::create },
    { "__builtin_ifloor", bfspf_i_d::create },
    { "__builtin_ifloorf", bfspf_i_f::create },
    { "__builtin_ifloorl", bfspf_i_ld::create },
    { "__builtin_ilogb", bfspf_i_d::create },
    { "__builtin_ilogbf", bfspf_i_f::create },
    { "__builtin_ilogbl", bfspf_i_ld::create },
    { "__builtin_inf", bfspf_d::create },
    { "__builtin_inff", bfspf_f::create },
    { "__builtin_infl", bfspf_ld::create },
    { "__builtin_irint", bfspf_i_d::create },
    { "__builtin_irintf", bfspf_i_f::create },
    { "__builtin_irintl", bfspf_i_ld::create },
    { "__builtin_iround", bfspf_i_d::create },
    { "__builtin_iroundf", bfspf_i_f::create },
    { "__builtin_iroundl", bfspf_i_ld::create },
    { "__builtin_j0", bfspf_d_d::create },
    { "__builtin_j0f", bfspf_f_f::create },
    { "__builtin_j0l", bfspf_ld_ld::create },
    { "__builtin_j1", bfspf_d_d::create },
    { "__builtin_j1f", bfspf_f_f::create },
    { "__builtin_j1l", bfspf_ld_ld::create },
    { "__builtin_jn", bfspf_d_i_d::create },
    { "__builtin_jnf", bfspf_f_i_f::create },
    { "__builtin_jnl", bfspf_ld_i_ld::create },
    { "__builtin_lceil", bfspf_l_d::create },
    { "__builtin_lceilf", bfspf_l_f::create },
    { "__builtin_lceill", bfspf_l_ld::create },
    { "__builtin_ldexp", bfspf_d_d_i::create },
    { "__builtin_ldexpf", bfspf_f_f_i::create },
    { "__builtin_ldexpl", bfspf_ld_ld_i::create },
    { "__builtin_lfloor", bfspf_l_d::create },
    { "__builtin_lfloorf", bfspf_l_f::create },
    { "__builtin_lfloorl", bfspf_l_ld::create },
    { "__builtin_lgamma", bfspf_d_d::create },
    { "__builtin_lgammaf", bfspf_f_f::create },
    { "__builtin_lgammal", bfspf_ld_ld::create },
    { "__builtin_lgamma_r", bfspf_d_d_pi::create },
    { "__builtin_lgammaf_r", bfspf_f_f_pi::create },
    { "__builtin_lgammal_r", bfspf_ld_ld_pi::create },
    { "__builtin_llceil", bfspf_ll_d::create },
    { "__builtin_llceilf", bfspf_ll_f::create },
    { "__builtin_llceill", bfspf_ll_ld::create },
    { "__builtin_llfloor", bfspf_ll_d::create },
    { "__builtin_llfloorf", bfspf_ll_f::create },
    { "__builtin_llfloorl", bfspf_ll_ld::create },
    { "__builtin_llrint", bfspf_ll_d::create },
    { "__builtin_llrintf", bfspf_ll_f::create },
    { "__builtin_llrintl", bfspf_ll_ld::create },
    { "__builtin_llround", bfspf_ll_d::create },
    { "__builtin_llroundf", bfspf_ll_f::create },
    { "__builtin_llroundl", bfspf_ll_ld::create },
    { "__builtin_log", bfspf_d_d::create },
    { "__builtin_logf", bfspf_f_f::create },
    { "__builtin_logl", bfspf_ld_ld::create },
    { "__builtin_log10", bfspf_d_d::create },
    { "__builtin_log10f", bfspf_f_f::create },
    { "__builtin_log10l", bfspf_ld_ld::create },
    { "__builtin_log1p", bfspf_d_d::create },
    { "__builtin_log1pf", bfspf_f_f::create },
    { "__builtin_log1pl", bfspf_ld_ld::create },
    { "__builtin_log2", bfspf_d_d::create },
    { "__builtin_log2f", bfspf_f_f::create },
    { "__builtin_log2l", bfspf_ld_ld::create },
    { "__builtin_logb", bfspf_d_d::create },
    { "__builtin_logbf", bfspf_f_f::create },
    { "__builtin_logbl", bfspf_ld_ld::create },
    { "__builtin_lrint", bfspf_l_d::create },
    { "__builtin_lrintf", bfspf_l_f::create },
    { "__builtin_lrintl", bfspf_l_ld::create },
    { "__builtin_lround", bfspf_l_d::create },
    { "__builtin_lroundf", bfspf_l_f::create },
    { "__builtin_lroundl", bfspf_l_ld::create },
    { "__builtin_modf", bfspf_d_d_pd::create },
    { "__builtin_modff", bfspf_f_f_pf::create },
    { "__builtin_modfl", bfspf_ld_ld_pld::create },
    { "__builtin_nan", bfspf_d_pcc::create },
    { "__builtin_nanf", bfspf_f_pcc::create },
    { "__builtin_nanl", bfspf_ld_pcc::create },
    { "__builtin_nans", bfspf_d_pcc::create },
    { "__builtin_nansf", bfspf_f_pcc::create },
    { "__builtin_nansl", bfspf_ld_pcc::create },
    { "__builtin_nearbyint", bfspf_d_d::create },
    { "__builtin_nearbyintf", bfspf_f_f::create },
    { "__builtin_nearbyintl", bfspf_ld_ld::create },
    { "__builtin_nextafter", bfspf_d_d_d::create },
    { "__builtin_nextafterf", bfspf_f_f_f::create },
    { "__builtin_nextafterl", bfspf_ld_ld_ld::create },
    { "__builtin_nexttoward", bfspf_d_d_d::create },
    { "__builtin_nexttowardf", bfspf_f_f_f::create },
    { "__builtin_nexttowardl", bfspf_ld_ld_ld::create },
    { "__builtin_pow", bfspf_d_d_d::create },
    { "__builtin_powf", bfspf_f_f_f::create },
    { "__builtin_powl", bfspf_ld_ld_ld::create },
    { "__builtin_pow10", bfspf_d_d::create },
    { "__builtin_pow10f", bfspf_f_f::create },
    { "__builtin_pow10l", bfspf_ld_ld::create },
    { "__builtin_powi", bfspf_d_d_i::create },
    { "__builtin_powif", bfspf_f_f_i::create },
    { "__builtin_powil", bfspf_ld_ld_i::create },
    { "__builtin_remainder", bfspf_d_d_d::create },
    { "__builtin_remainderf", bfspf_f_f_f::create },
    { "__builtin_remainderl", bfspf_ld_ld_ld::create },
    { "__builtin_remquo", bfspf_d_d_d_pi::create },
    { "__builtin_remquof", bfspf_f_f_f_pi::create },
    { "__builtin_remquol", bfspf_ld_ld_ld_pi::create },
    { "__builtin_rint", bfspf_d_d::create },
    { "__builtin_rintf", bfspf_f_f::create },
    { "__builtin_rintl", bfspf_ld_ld::create },
    { "__builtin_round", bfspf_d_d::create },
    { "__builtin_roundf", bfspf_f_f::create },
    { "__builtin_roundl", bfspf_ld_ld::create },
    { "__builtin_scalb", bfspf_d_d_d::create },
    { "__builtin_scalbf", bfspf_f_f_f::create },
    { "__builtin_scalbl", bfspf_ld_ld_ld::create },
    { "__builtin_scalbln", bfspf_d_d_l::create },
    { "__builtin_scalblnf", bfspf_f_f_l::create },
    { "__builtin_scalblnl", bfspf_ld_ld_l::create },
    { "__builtin_scalbn", bfspf_d_d_i::create },
    { "__builtin_scalbnf", bfspf_f_f_i::create },
    { "__builtin_scalbnl", bfspf_ld_ld_i::create },
    { "__builtin_signbit", bfspf_i_d::create },
    { "__builtin_signbitf", bfspf_i_f::create },
    { "__builtin_signbitl", bfspf_i_ld::create },
    { "__builtin_significand", bfspf_d_d::create },
    { "__builtin_significandf", bfspf_f_f::create },
    { "__builtin_significandl", bfspf_ld_ld::create },
    { "__builtin_sin", bfspf_d_d::create },
    { "__builtin_sinf", bfspf_f_f::create },
    { "__builtin_sinl", bfspf_ld_ld::create },
    { "__builtin_sincos", bfspf_v_d_pd_pd::create },
    { "__builtin_sincosf", bfspf_v_f_pf_pf::create },
    { "__builtin_sincosl", bfspf_v_ld_pld_pld::create },
    { "__builtin_sinh", bfspf_d_d::create },
    { "__builtin_sinhf", bfspf_f_f::create },
    { "__builtin_sinhl", bfspf_ld_ld::create },
    { "__builtin_sqrt", bfspf_d_d::create },
    { "__builtin_sqrtf", bfspf_f_f::create },
    { "__builtin_sqrtl", bfspf_ld_ld::create },
    { "__builtin_tan", bfspf_d_d::create },
    { "__builtin_tanf", bfspf_f_f::create },
    { "__builtin_tanl", bfspf_ld_ld::create },
    { "__builtin_tanh", bfspf_d_d::create },
    { "__builtin_tanhf", bfspf_f_f::create },
    { "__builtin_tanhl", bfspf_ld_ld::create },
    { "__builtin_tgamma", bfspf_d_d::create },
    { "__builtin_tgammaf", bfspf_f_f::create },
    { "__builtin_tgammal", bfspf_ld_ld::create },
    { "__builtin_trunc", bfspf_d_d::create },
    { "__builtin_truncf", bfspf_f_f::create },
    { "__builtin_truncl", bfspf_ld_ld::create },
    { "__builtin_y0", bfspf_d_d::create },
    { "__builtin_y0f", bfspf_f_f::create },
    { "__builtin_y0l", bfspf_ld_ld::create },
    { "__builtin_y1", bfspf_d_d::create },
    { "__builtin_y1f", bfspf_f_f::create },
    { "__builtin_y1l", bfspf_ld_ld::create },
    { "__builtin_yn", bfspf_d_i_d::create },
    { "__builtin_ynf", bfspf_f_i_f::create },
    { "__builtin_ynl", bfspf_ld_i_ld::create },

    // Category: _Complex math builtins.
    { "__builtin_cabs", bfspf_d_dC::create },
    { "__builtin_cabsf", bfspf_f_fC::create },
    { "__builtin_cabsl", bfspf_ld_ldC::create },
    { "__builtin_cacos", bfspf_dC_dC::create },
    { "__builtin_cacosf", bfspf_fC_fC::create },
    { "__builtin_cacosl", bfspf_ldC_ldC::create },
    { "__builtin_cacosh", bfspf_dC_dC::create },
    { "__builtin_cacoshf", bfspf_fC_fC::create },
    { "__builtin_cacoshl", bfspf_ldC_ldC::create },
    { "__builtin_carg", bfspf_d_dC::create },
    { "__builtin_cargf", bfspf_f_fC::create },
    { "__builtin_cargl", bfspf_ld_ldC::create },
    { "__builtin_casin", bfspf_dC_dC::create },
    { "__builtin_casinf", bfspf_fC_fC::create },
    { "__builtin_casinl", bfspf_ldC_ldC::create },
    { "__builtin_casinh", bfspf_dC_dC::create },
    { "__builtin_casinhf", bfspf_fC_fC::create },
    { "__builtin_casinhl", bfspf_ldC_ldC::create },
    { "__builtin_catan", bfspf_dC_dC::create },
    { "__builtin_catanf", bfspf_fC_fC::create },
    { "__builtin_catanl", bfspf_ldC_ldC::create },
    { "__builtin_catanh", bfspf_dC_dC::create },
    { "__builtin_catanhf", bfspf_fC_fC::create },
    { "__builtin_catanhl", bfspf_ldC_ldC::create },
    { "__builtin_ccos", bfspf_dC_dC::create },
    { "__builtin_ccosf", bfspf_fC_fC::create },
    { "__builtin_ccosl", bfspf_ldC_ldC::create },
    { "__builtin_ccosh", bfspf_dC_dC::create },
    { "__builtin_ccoshf", bfspf_fC_fC::create },
    { "__builtin_ccoshl", bfspf_ldC_ldC::create },
    { "__builtin_cexp", bfspf_dC_dC::create },
    { "__builtin_cexpf", bfspf_fC_fC::create },
    { "__builtin_cexpl", bfspf_ldC_ldC::create },
    { "__builtin_cexpi", bfspf_dC_d::create },
    { "__builtin_cexpif", bfspf_fC_f::create },
    { "__builtin_cexpil", bfspf_ldC_ld::create },
    { "__builtin_cimag", bfspf_d_dC::create },
    { "__builtin_cimagf", bfspf_f_fC::create },
    { "__builtin_cimagl", bfspf_ld_ldC::create },
    { "__builtin_clog", bfspf_dC_dC::create },
    { "__builtin_clogf", bfspf_fC_fC::create },
    { "__builtin_clogl", bfspf_ldC_ldC::create },
    { "__builtin_clog10", bfspf_dC_dC::create },
    { "__builtin_clog10f", bfspf_fC_fC::create },
    { "__builtin_clog10l", bfspf_ldC_ldC::create },
    { "__builtin_conj", bfspf_dC_dC::create },
    { "__builtin_conjf", bfspf_fC_fC::create },
    { "__builtin_conjl", bfspf_ldC_ldC::create },
    { "__builtin_cpow", bfspf_dC_dC_dC::create },
    { "__builtin_cpowf", bfspf_fC_fC_fC::create },
    { "__builtin_cpowl", bfspf_ldC_ldC_ldC::create },
    { "__builtin_cproj", bfspf_dC_dC::create },
    { "__builtin_cprojf", bfspf_fC_fC::create },
    { "__builtin_cprojl", bfspf_ldC_ldC::create },
    { "__builtin_creal", bfspf_d_dC::create },
    { "__builtin_crealf", bfspf_f_fC::create },
    { "__builtin_creall", bfspf_ld_ldC::create },
    { "__builtin_csin", bfspf_dC_dC::create },
    { "__builtin_csinf", bfspf_fC_fC::create },
    { "__builtin_csinl", bfspf_ldC_ldC::create },
    { "__builtin_csinh", bfspf_dC_dC::create },
    { "__builtin_csinhf", bfspf_fC_fC::create },
    { "__builtin_csinhl", bfspf_ldC_ldC::create },
    { "__builtin_csqrt", bfspf_dC_dC::create },
    { "__builtin_csqrtf", bfspf_fC_fC::create },
    { "__builtin_csqrtl", bfspf_ldC_ldC::create },
    { "__builtin_ctan", bfspf_dC_dC::create },
    { "__builtin_ctanf", bfspf_fC_fC::create },
    { "__builtin_ctanl", bfspf_ldC_ldC::create },
    { "__builtin_ctanh", bfspf_dC_dC::create },
    { "__builtin_ctanhf", bfspf_fC_fC::create },
    { "__builtin_ctanhl", bfspf_ldC_ldC::create },

    // Category string/memory builtins.
    { "__builtin_bcmp", bfspf_i_pcv_pcv_sz::create },
    { "__builtin_bcopy", bfspf_v_pcv_pv_sz::create },
    { "__builtin_bzero", bfspf_v_pv_sz::create },
    { "__builtin_index", bfspf_pc_pcc_i::create },
    { "__builtin_memchr", bfspf_pv_pcv_i_sz::create },
    { "__builtin_memcmp", bfspf_i_pcv_pcv_sz::create },
    { "__builtin_memcpy", bfspf_pv_pv_pcv_sz::create },
    { "__builtin_memmove", bfspf_pv_pv_pcv_sz::create },
    { "__builtin_mempcpy", bfspf_pv_pv_pcv_sz::create },
    { "__builtin_memset", bfspf_pv_pv_i_sz::create },
    { "__builtin_rindex", bfspf_pc_pcc_i::create },
    { "__builtin_stpcpy", bfspf_pc_pc_pcc::create },
    { "__builtin_stpncpy", bfspf_pc_pc_pcc_sz::create },
    { "__builtin_strcasecmp", bfspf_i_pcc_pcc::create },
    { "__builtin_strcat", bfspf_pc_pc_pcc::create },
    { "__builtin_strchr", bfspf_pc_pcc_i::create },
    { "__builtin_strcmp", bfspf_i_pcc_pcc::create },
    { "__builtin_strcpy", bfspf_pc_pc_pcc::create },
    { "__builtin_strcspn", bfspf_sz_pcc_pcc::create },
    { "__builtin_strdup", bfspf_pc_pcc::create },
    { "__builtin_strndup", bfspf_pc_pcc_sz::create },
    { "__builtin_strlen", bfspf_sz_pcc::create },
    { "__builtin_strncasecmp", bfspf_i_pcc_pcc_sz::create },
    { "__builtin_strncat", bfspf_pc_pc_pcc_sz::create },
    { "__builtin_strncmp", bfspf_i_pcc_pcc_sz::create },
    { "__builtin_strncpy", bfspf_pc_pc_pcc_sz::create },
    { "__builtin_strpbrk", bfspf_pc_pcc_pcc::create },
    { "__builtin_strrchr", bfspf_pc_pcc_i::create },
    { "__builtin_strspn", bfspf_sz_pcc_pcc::create },
    { "__builtin_strstr", bfspf_pc_pcc_pcc::create },

    // Category: stdio builtins.
    { "__builtin_fprintf", bfspf_i_pv_pcc_var::create },
    { "__builtin_fprintf_unlocked", bfspf_i_pv_pcc_var::create },
    { "__builtin_putc", bfspf_i_i_pv::create },
    { "__builtin_putc_unlocked", bfspf_i_i_pv::create },
    { "__builtin_fputc", bfspf_i_i_pv::create },
    { "__builtin_fputc_unlocked", bfspf_i_i_pv::create },
    { "__builtin_fputs", bfspf_i_pcc_pv::create },
    { "__builtin_fputs_unlocked", bfspf_i_pcc_pv::create },
    { "__builtin_fscanf", bfspf_i_pv_pcc_var::create },
    { "__builtin_fwrite", bfspf_sz_pcc_sz_sz_pv::create },
    { "__builtin_fwrite_unlocked", bfspf_sz_pcc_sz_sz_pv::create },
    { "__builtin_printf", bfspf_i_pcc_var::create },
    { "__builtin_printf_unlocked", bfspf_i_pcc_var::create },
    { "__builtin_putchar", bfspf_i_i::create },
    { "__builtin_putchar_unlocked", bfspf_i_i::create },
    { "__builtin_puts", bfspf_i_pcc::create },
    { "__builtin_puts_unlocked", bfspf_i_pcc::create },
    { "__builtin_scanf", bfspf_i_pcc_var::create },
    { "__builtin_snprintf", bfspf_i_pc_sz_pcc_var::create },
    { "__builtin_sprintf", bfspf_i_pc_pcc_var::create },
    { "__builtin_sscanf", bfspf_i_pcc_sz_pcc_var::create },
    { "__builtin_vfprintf", bfspf_i_pv_pcc_val::create },
    { "__builtin_vfscanf", bfspf_i_pv_pcc_val::create },
    { "__builtin_vprintf", bfspf_i_pcc_val::create },
    { "__builtin_vscanf", bfspf_i_pcc_val::create },
    { "__builtin_vsnprintf", bfspf_i_pc_sz_pcc_val::create },
    { "__builtin_vsprintf", bfspf_i_pc_pcc_val::create },
    { "__builtin_vsscanf", bfspf_i_pcc_pcc_val::create },

    // Category: ctype builtins.
    { "__builtin_isalnum", bfspf_i_i::create },
    { "__builtin_isalpha", bfspf_i_i::create },
    { "__builtin_isascii", bfspf_i_i::create },
    { "__builtin_isblank", bfspf_i_i::create },
    { "__builtin_iscntrl", bfspf_i_i::create },
    { "__builtin_isdigit", bfspf_i_i::create },
    { "__builtin_isgraph", bfspf_i_i::create },
    { "__builtin_islower", bfspf_i_i::create },
    { "__builtin_isprint", bfspf_i_i::create },
    { "__builtin_ispunct", bfspf_i_i::create },
    { "__builtin_isspace", bfspf_i_i::create },
    { "__builtin_isupper", bfspf_i_i::create },
    { "__builtin_isxdigit", bfspf_i_i::create },
    { "__builtin_toascii", bfspf_i_i::create },
    { "__builtin_tolower", bfspf_i_i::create },
    { "__builtin_toupper", bfspf_i_i::create },
    { "__builtin_iswalnum", bfspf_i_wi::create },
    { "__builtin_iswalpha", bfspf_i_wi::create },
    { "__builtin_iswblank", bfspf_i_wi::create },
    { "__builtin_iswcntrl", bfspf_i_wi::create },
    { "__builtin_iswdigit", bfspf_i_wi::create },
    { "__builtin_iswgraph", bfspf_i_wi::create },
    { "__builtin_iswlower", bfspf_i_wi::create },
    { "__builtin_iswprint", bfspf_i_wi::create },
    { "__builtin_iswpunct", bfspf_i_wi::create },
    { "__builtin_iswspace", bfspf_i_wi::create },
    { "__builtin_iswupper", bfspf_i_wi::create },
    { "__builtin_iswxdigit", bfspf_i_wi::create },
    { "__builtin_toascii", bfspf_i_wi::create },
    { "__builtin_towlower", bfspf_wi_wi::create },
    { "__builtin_towupper", bfspf_wi_wi::create },

    // Category: miscellaneous builtins.
    { "__builtin_abort", bfspf_v::create },
    { "__builtin_abs", bfspf_i_i::create },
    { "__builtin_aggregate_incoming_address", bfspf_pv_var::create },
    { "__builtin_aligned_alloc", bfspf_pv_sz_sz::create },
    { "__builtin_alloca", bfspf_pv_sz::create },
    { "__builtin_alloca_with_align", bfspf_pv_sz_sz::create },
    { "__builtin_apply", bfspf_pv_pF_v_var__pv_sz::create },
    { "__builtin_apply_args", bfspf_pv_var::create },
    { "__builtin_assume_aligned", bfspf_pv_pcv_sz_var::create },
    { "__builtin_bswap16", bfspf_u16_u16::create },
    { "__builtin_bswap32", bfspf_u32_u32::create },
    { "__builtin_bswap64", bfspf_u64_u64::create },
    { "__builtin_calloc", bfspf_pv_sz_sz::create },
    { "__builtin_choose_expr", builtin_func_choose_expr_create },
    { "__builtin___clear_cache", bfspf_v_pv_pv::create },
    { "__builtin_classify_type", bfspf_i_var::create },
    { "__builtin_clz", bfspf_i_u::create },
    { "__builtin_clzimax", bfspf_i_umax::create },
    { "__builtin_clzl", bfspf_i_ul::create },
    { "__builtin_clzll", bfspf_i_ull::create },
    { "__builtin_constant_p", builtin_func_constant_p::create },
    { "__builtin_ctz", bfspf_i_u::create },
    { "__builtin_ctzimax", bfspf_i_umax::create },
    { "__builtin_ctzl", bfspf_i_ul::create },
    { "__builtin_ctzll", bfspf_i_ull::create },
    { "__builtin_clrsb", bfspf_i_i::create },
    { "__builtin_clrsbimax", bfspf_i_imax::create },
    { "__builtin_clrsbl", bfspf_i_l::create },
    { "__builtin_clrsbll", bfspf_i_ll::create },
    { "__builtin_dcgettext", bfspf_pc_pcc_pcc_i::create },
    { "__builtin_dgettext", bfspf_pc_pcc_pcc::create },
    { "__builtin_dwarf_cfa", bfspf_pv::create },
    { "__builtin_dwarf_sp_column", bfspf_u::create },
    { "__builtin_eh_return", bfspf_v_ssz_pv::create },
    { "__builtin_eh_return_data_regno)", bfspf_i_i::create },
    { "__builtin_execl", bfspf_i_pcc_pcc_var::create },
    { "__builtin_execlp", bfspf_i_pcc_pcc_var::create },
    { "__builtin_execle", bfspf_i_pcc_pcc_var::create },
    { "__builtin_execv", bfspf_i_pcc_pv_pcc::create },
    { "__builtin_execvp", bfspf_i_pcc_pv_pcc::create },
    { "__builtin_execve", bfspf_i_pcc_pv_pcc_pv_pcc::create },
    { "__builtin_exit", bfspf_v_i::create },
    { "__builtin_expect", bfspf_l_l_l::create },
    { "__builtin_extend_pointer", bfspf_sz_pv::create },
    { "__builtin_extract_return_addr", bfspf_pv_pv::create },
    { "__builtin_ffs", bfspf_i_i::create },
    { "__builtin_ffsimax", bfspf_i_imax::create },
    { "__builtin_ffsl", bfspf_i_l::create },
    { "__builtin_ffsll", bfspf_i_ll::create },
    { "__builtin_fork", bfspf_pid::create },
    { "__builtin_frame_address", bfspf_pv_u::create },
    { "__builtin_free", bfspf_v_pv::create },
    { "__builtin_frob_return_address", bfspf_pv_pv::create },
    { "__builtin_gettext", bfspf_pc_pcc::create },
    { "__builtin_imaxabs", bfspf_imax_imax::create },
    { "__builtin_init_dwarf_reg_size_table", bfspf_v_pv::create },
    { "__builtin_finite", bfspf_i_d::create },
    { "__builtin_finitef", bfspf_i_f::create },
    { "__builtin_finitel", bfspf_i_ld::create },
    { "__builtin_fpclassify", bfspf_i_i_i_i_i_i_var::create },
    { "__builtin_isfinite", bfspf_i_var::create },
    { "__builtin_isinf_sign", bfspf_i_var::create },
    { "__builtin_isinf", bfspf_i_var::create },
    { "__builtin_isinff", bfspf_i_f::create },
    { "__builtin_isinfl", bfspf_i_ld::create },
    { "__builtin_isnan", bfspf_i_var::create },
    { "__builtin_isnanf", bfspf_i_f::create },
    { "__builtin_isnanl", bfspf_i_ld::create },
    { "__builtin_isnormal", bfspf_i_var::create },
    { "__builtin_isgreater", bfspf_i_var::create },
    { "__builtin_isgreaterequal", bfspf_i_var::create },
    { "__builtin_isless", bfspf_i_var::create },
    { "__builtin_islessequal", bfspf_i_var::create },
    { "__builtin_islessgreater", bfspf_i_var::create },
    { "__builtin_isunordered", bfspf_i_var::create },
    { "__builtin_labs", bfspf_l_l::create },
    { "__builtin_llabs", bfspf_ll_ll::create },
    { "__builtin_longjmp", bfspf_v_pv_i::create },
    { "__builtin_malloc", bfspf_pv_sz::create },
    { "__builtin_next_arg", bfspf_pv_var::create },
    { "__builtin_parity", bfspf_i_u::create },
    { "__builtin_parityimax", bfspf_i_umax::create },
    { "__builtin_parityl", bfspf_i_ul::create },
    { "__builtin_parityll", bfspf_i_ull::create },
    { "__builtin_popcount", bfspf_i_u::create },
    { "__builtin_popcountimax", bfspf_i_umax::create },
    { "__builtin_popcountl", bfspf_i_ul::create },
    { "__builtin_popcountll", bfspf_i_ull::create },
    { "__builtin_prefetch", bfspf_v_pcvv_var::create },
    { "__builtin_realloc", bfspf_pv_pv_sz::create },
    { "__builtin_return", bfspf_v_pv::create },
    { "__builtin_return_address", bfspf_pv_u::create },
    { "__builtin_saveregs", bfspf_pv_var::create },
    { "__builtin_setjmp", bfspf_i_pv::create },
    { "__builtin_strfmon", bfspf_ssz_pc_sz_pcc_var::create },
    { "__builtin_strftime", bfspf_sz_pc_sz_pcc_pcv::create },
    { "__builtin_trap", bfspf_v::create },
    { "__builtin_unreachable", bfspf_v::create },
    { "__builtin_unwind_init", bfspf_v::create },
    { "__builtin_update_setjmp_buf", bfspf_v_pv_i::create },
    { "__builtin_va_copy", bfspf_v_val_val::create },
    { "__builtin_va_end", bfspf_v_val::create },
    { "__builtin_va_start", bfspf_v_val_var::create },
    { "__builtin_va_arg_pack", bfspf_i::create },
    { "__builtin_va_arg_pack_len", bfspf_i::create },
    { "__builtin__exit", bfspf_v_i::create },
    { "__builtin__Exit", bfspf_v_i::create },

    // Object size checking builtins.
    { "__builtin_object_size", bfspf_sz_pcv_i::create },
    { "__builtin___memcpy_chk", bfspf_pv_pv_pcv_sz_sz::create },
    { "__builtin___memmove_chk", bfspf_pv_pv_pcv_sz_sz::create },
    { "__builtin___mempcpy_chk", bfspf_pv_pv_pcv_sz_sz::create },
    { "__builtin___memset_chk", bfspf_pv_pv_i_sz_sz::create },
    { "__builtin___stpcpy_chk", bfspf_pc_pc_pcc_sz::create },
    { "__builtin___stpncpy_chk", bfspf_pc_pc_pcc_sz_sz::create },
    { "__builtin___strcat_chk", bfspf_pc_pc_pcc_sz::create },
    { "__builtin___strcpy_chk", bfspf_pc_pc_pcc_sz::create },
    { "__builtin___strncat_chk", bfspf_pc_pc_pcc_sz_sz::create },
    { "__builtin___strncpy_chk", bfspf_pc_pc_pcc_sz_sz::create },
    { "__builtin___snprintf_chk", bfspf_i_pc_sz_i_sz_pcc_var::create },
    { "__builtin___vsnprintf_chk", bfspf_i_pc_sz_i_sz_pcc_val::create },
    { "__builtin___sprintf_chk", bfspf_i_pc_i_sz_pcc_var::create },
    { "__builtin___vsprintf_chk", bfspf_i_pc_i_sz_pcc_val::create },
    { "__builtin___fprintf_chk", bfspf_i_pv_i_pcc_var::create },
    { "__builtin___vfprintf_chk", bfspf_i_pv_i_pcc_val::create },
    { "__builtin___printf_chk", bfspf_i_i_pcc_var::create },
    { "__builtin___vprintf_chk", bfspf_i_i_pcc_val::create },

    // Profiling hooks.
    { "__cyg_profile_func_enter", bfspf_v_pv_pv::create },
    { "__cyg_profile_func_exit", bfspf_v_pv_pv::create },

    // TLS thread pointer related builtins.
    { "__builtin_thread_pointer", bfspf_pv::create },
    { "__builtin_set_thread_pointer", bfspf_v_pv::create },

    // __FILE__, __LINE__, __FUNCTION__ as builtins.
    { "__builtin_FILE", bfspf_pcc::create },
    { "__builtin_FUNCTION", bfspf_pcc::create },
    { "__builtin_LINE", bfspf_i::create },

    // Synchronization Primitives.
    { "__sync_fetch_and_add", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_fetch_and_add_1", bfspf_i8_pvv_i8::create },
    { "__sync_fetch_and_add_2", bfspf_i16_pvv_i16::create },
    { "__sync_fetch_and_add_4", bfspf_i32_pvv_i32::create },
    { "__sync_fetch_and_add_8", bfspf_i64_pvv_i64::create },
    { "__sync_fetch_and_add_16", bfspf_i128_pvv_i128::create },
    { "__sync_fetch_and_sub", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_fetch_and_sub_1", bfspf_i8_pvv_i8::create },
    { "__sync_fetch_and_sub_2", bfspf_i16_pvv_i16::create },
    { "__sync_fetch_and_sub_4", bfspf_i32_pvv_i32::create },
    { "__sync_fetch_and_sub_8", bfspf_i64_pvv_i64::create },
    { "__sync_fetch_and_sub_16", bfspf_i128_pvv_i128::create },
    { "__sync_fetch_and_or", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_fetch_and_or_1", bfspf_i8_pvv_i8::create },
    { "__sync_fetch_and_or_2", bfspf_i16_pvv_i16::create },
    { "__sync_fetch_and_or_4", bfspf_i32_pvv_i32::create },
    { "__sync_fetch_and_or_8", bfspf_i64_pvv_i64::create },
    { "__sync_fetch_and_or_16", bfspf_i128_pvv_i128::create },
    { "__sync_fetch_and_and", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_fetch_and_and_1", bfspf_i8_pvv_i8::create },
    { "__sync_fetch_and_and_2", bfspf_i16_pvv_i16::create },
    { "__sync_fetch_and_and_4", bfspf_i32_pvv_i32::create },
    { "__sync_fetch_and_and_8", bfspf_i64_pvv_i64::create },
    { "__sync_fetch_and_and_16", bfspf_i128_pvv_i128::create },
    { "__sync_fetch_and_xor", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_fetch_and_xor_1", bfspf_i8_pvv_i8::create },
    { "__sync_fetch_and_xor_2", bfspf_i16_pvv_i16::create },
    { "__sync_fetch_and_xor_4", bfspf_i32_pvv_i32::create },
    { "__sync_fetch_and_xor_8", bfspf_i64_pvv_i64::create },
    { "__sync_fetch_and_xor_16", bfspf_i128_pvv_i128::create },
    { "__sync_fetch_and_nand", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_fetch_and_nand_1", bfspf_i8_pvv_i8::create },
    { "__sync_fetch_and_nand_2", bfspf_i16_pvv_i16::create },
    { "__sync_fetch_and_nand_4", bfspf_i32_pvv_i32::create },
    { "__sync_fetch_and_nand_8", bfspf_i64_pvv_i64::create },
    { "__sync_fetch_and_nand_16", bfspf_i128_pvv_i128::create },
    { "__sync_add_and_fetch", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_add_and_fetch_1", bfspf_i8_pvv_i8::create },
    { "__sync_add_and_fetch_2", bfspf_i16_pvv_i16::create },
    { "__sync_add_and_fetch_4", bfspf_i32_pvv_i32::create },
    { "__sync_add_and_fetch_8", bfspf_i64_pvv_i64::create },
    { "__sync_add_and_fetch_16", bfspf_i128_pvv_i128::create },
    { "__sync_sub_and_fetch", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_sub_and_fetch_1", bfspf_i8_pvv_i8::create },
    { "__sync_sub_and_fetch_2", bfspf_i16_pvv_i16::create },
    { "__sync_sub_and_fetch_4", bfspf_i32_pvv_i32::create },
    { "__sync_sub_and_fetch_8", bfspf_i64_pvv_i64::create },
    { "__sync_sub_and_fetch_16", bfspf_i128_pvv_i128::create },
    { "__sync_or_and_fetch", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_or_and_fetch_1", bfspf_i8_pvv_i8::create },
    { "__sync_or_and_fetch_2", bfspf_i16_pvv_i16::create },
    { "__sync_or_and_fetch_4", bfspf_i32_pvv_i32::create },
    { "__sync_or_and_fetch_8", bfspf_i64_pvv_i64::create },
    { "__sync_or_and_fetch_16", bfspf_i128_pvv_i128::create },
    { "__sync_and_and_fetch", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_and_and_fetch_1", bfspf_i8_pvv_i8::create },
    { "__sync_and_and_fetch_2", bfspf_i16_pvv_i16::create },
    { "__sync_and_and_fetch_4", bfspf_i32_pvv_i32::create },
    { "__sync_and_and_fetch_8", bfspf_i64_pvv_i64::create },
    { "__sync_and_and_fetch_16", bfspf_i128_pvv_i128::create },
    { "__sync_xor_and_fetch", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_xor_and_fetch_1", bfspf_i8_pvv_i8::create },
    { "__sync_xor_and_fetch_2", bfspf_i16_pvv_i16::create },
    { "__sync_xor_and_fetch_4", bfspf_i32_pvv_i32::create },
    { "__sync_xor_and_fetch_8", bfspf_i64_pvv_i64::create },
    { "__sync_xor_and_fetch_16", bfspf_i128_pvv_i128::create },
    { "__sync_nand_and_fetch", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_nand_and_fetch_1", bfspf_i8_pvv_i8::create },
    { "__sync_nand_and_fetch_2", bfspf_i16_pvv_i16::create },
    { "__sync_nand_and_fetch_4", bfspf_i32_pvv_i32::create },
    { "__sync_nand_and_fetch_8", bfspf_i64_pvv_i64::create },
    { "__sync_nand_and_fetch_16", bfspf_i128_pvv_i128::create },
    { "__sync_bool_compare_and_swap", bfpiof0_b_pvv_iN_iN_var::create },
    { "__sync_bool_compare_and_swap_1", bfspf_b_pvv_i8_i8::create },
    { "__sync_bool_compare_and_swap_2", bfspf_b_pvv_i16_i16::create },
    { "__sync_bool_compare_and_swap_4", bfspf_b_pvv_i32_i32::create },
    { "__sync_bool_compare_and_swap_8", bfspf_b_pvv_i64_i64::create },
    { "__sync_bool_compare_and_swap_16", bfspf_b_pvv_i128_i128::create },
    { "__sync_val_compare_and_swap", bfpiof0_iN_pvv_iN_iN_var::create },
    { "__sync_val_compare_and_swap_1", bfspf_i8_pvv_i8_i8::create },
    { "__sync_val_compare_and_swap_2", bfspf_i16_pvv_i16_i16::create },
    { "__sync_val_compare_and_swap_4", bfspf_i32_pvv_i32_i32::create },
    { "__sync_val_compare_and_swap_8", bfspf_i64_pvv_i64_i64::create },
    { "__sync_val_compare_and_swap_16", bfspf_i128_pvv_i128_i128::create },
    { "__sync_lock_test_and_set", bfpiof0_iN_pvv_iN_var::create },
    { "__sync_lock_test_and_set_1", bfspf_i8_pvv_i8::create },
    { "__sync_lock_test_and_set_2", bfspf_i16_pvv_i16::create },
    { "__sync_lock_test_and_set_4", bfspf_i32_pvv_i32::create },
    { "__sync_lock_test_and_set_8", bfspf_i64_pvv_i64::create },
    { "__sync_lock_test_and_set_16", bfspf_i128_pvv_i128::create },
    { "__sync_lock_release", bfpiof0_v_pvv_var::create },
    { "__sync_lock_release_1", bfspf_v_pvv::create },
    { "__sync_lock_release_2", bfspf_v_pvv::create },
    { "__sync_lock_release_4", bfspf_v_pvv::create },
    { "__sync_lock_release_8", bfspf_v_pvv::create },
    { "__sync_lock_release_16", bfspf_v_pvv::create },
    { "__sync_synchronize", bfspf_v::create },
    { "__atomic_test_and_set", bfspf_b_pvv_i::create },
    { "__atomic_clear", bfspf_v_pvv_i::create },
    { "__atomic_exchange", bfspf_v_pvv_pv_pv_i::create },
    { "__atomic_exchange_n", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_exchange_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_exchange_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_exchange_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_exchange_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_exchange_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_load", bfspf_v_pcvv_pv_i::create },
    { "__atomic_load_n", bfpiof0_iN_pcvv_i::create },
    { "__atomic_load_1", bfspf_i8_pcvv_i::create },
    { "__atomic_load_2", bfspf_i16_pcvv_i::create },
    { "__atomic_load_4", bfspf_i32_pcvv_i::create },
    { "__atomic_load_8", bfspf_i64_pcvv_i::create },
    { "__atomic_load_16", bfspf_i128_pcvv_i::create },
    { "__atomic_compare_exchange", bfspf_b_pvv_pv_pv_b_i_i::create },
    { "__atomic_compare_exchange_n", bfpiof0_b_pvv_pv_iN_b_i_i::create },
    { "__atomic_compare_exchange_1", bfspf_b_pvv_pv_i8_b_i_i::create },
    { "__atomic_compare_exchange_2", bfspf_b_pvv_pv_i16_b_i_i::create },
    { "__atomic_compare_exchange_4", bfspf_b_pvv_pv_i32_b_i_i::create },
    { "__atomic_compare_exchange_8", bfspf_b_pvv_pv_i64_b_i_i::create },
    { "__atomic_compare_exchange_16", bfspf_b_pvv_pv_i128_b_i_i::create },
    { "__atomic_store", bfspf_v_pvv_pv_i::create },
    { "__atomic_store_n", bfpiof0_v_pvv_iN_i::create },
    { "__atomic_store_1", bfspf_v_pvv_i8_i::create },
    { "__atomic_store_2", bfspf_v_pvv_i16_i::create },
    { "__atomic_store_4", bfspf_v_pvv_i32_i::create },
    { "__atomic_store_8", bfspf_v_pvv_i64_i::create },
    { "__atomic_store_16", bfspf_v_pvv_i128_i::create },
    { "__atomic_add_fetch", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_add_fetch_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_add_fetch_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_add_fetch_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_add_fetch_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_add_fetch_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_sub_fetch", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_sub_fetch_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_sub_fetch_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_sub_fetch_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_sub_fetch_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_sub_fetch_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_and_fetch", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_and_fetch_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_and_fetch_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_and_fetch_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_and_fetch_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_and_fetch_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_nand_fetch", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_nand_fetch_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_nand_fetch_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_nand_fetch_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_nand_fetch_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_nand_fetch_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_xor_fetch", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_xor_fetch_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_xor_fetch_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_xor_fetch_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_xor_fetch_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_xor_fetch_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_or_fetch", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_or_fetch_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_or_fetch_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_or_fetch_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_or_fetch_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_or_fetch_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_fetch_add", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_fetch_add_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_fetch_add_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_fetch_add_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_fetch_add_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_fetch_add_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_fetch_sub", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_fetch_sub_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_fetch_sub_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_fetch_sub_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_fetch_sub_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_fetch_sub_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_fetch_and", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_fetch_and_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_fetch_and_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_fetch_and_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_fetch_and_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_fetch_and_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_fetch_nand", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_fetch_nand_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_fetch_nand_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_fetch_nand_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_fetch_nand_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_fetch_nand_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_fetch_xor", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_fetch_xor_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_fetch_xor_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_fetch_xor_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_fetch_xor_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_fetch_xor_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_fetch_or", bfpiof0_iN_pvv_iN_i::create },
    { "__atomic_fetch_or_1", bfspf_i8_pvv_i8_i::create },
    { "__atomic_fetch_or_2", bfspf_i16_pvv_i16_i::create },
    { "__atomic_fetch_or_4", bfspf_i32_pvv_i32_i::create },
    { "__atomic_fetch_or_8", bfspf_i64_pvv_i64_i::create },
    { "__atomic_fetch_or_16", bfspf_i128_pvv_i128_i::create },
    { "__atomic_always_lock_free", bfspf_b_sz_pcvv::create },
    { "__atomic_is_lock_free", bfspf_b_sz_pcvv::create },
    { "__atomic_thread_fence", bfspf_v_i::create },
    { "__atomic_signal_fence", bfspf_v_i::create },
  };

  auto it = m.find(id);
  if (it == m.end())
    return nullptr;

  return it->second;
}
