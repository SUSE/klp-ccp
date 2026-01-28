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

#include "ast.hh"
#include "execution_charset_encoder_iconv.hh"
#include "preprocessor.hh"
#include "target_aarch64_gcc.hh"

using namespace klp::ccp;
using namespace klp::ccp::types;

enum opt_code_aarch64
{
  opt_code_aarch64_unused = 0,
};

static gcc_cmdline_parser::option gcc_opt_table_aarch64[] = {
	#include "gcc_cmdline_opts_aarch64.cc"
	{ nullptr }
};


target_aarch64_gcc::target_aarch64_gcc(const char * const version)
  : target_gcc(version), _opts_aarch64(*this)
{}

target_aarch64_gcc::~target_aarch64_gcc() noexcept = default;

bool target_aarch64_gcc::is_wchar_signed() const noexcept
{
  // See gcc/config/arm/arm.h WCHAR_TYPE
  return false;
}

bool target_aarch64_gcc::is_bitfield_default_signed() const noexcept
{
  return true;
}

std::unique_ptr<execution_charset_encoder> target_aarch64_gcc::
get_execution_charset_encoder(const execution_charset_encoding e) const
{
  types::std_int_type::kind target_char_kind;
  const char *target_code;

  switch (e) {
  case target::execution_charset_encoding::ecse_char:
    target_code = "UTF-8";
    target_char_kind = types::std_int_type::kind::k_char;
    break;

  case target::execution_charset_encoding::ecse_wchar:
    target_code = "UTF-32LE";
    target_char_kind = types::std_int_type::kind::k_int;
    break;

  case target::execution_charset_encoding::ecse_char8:
    target_code = "UTF-8";
    target_char_kind = types::std_int_type::kind::k_char;
    break;

  case target::execution_charset_encoding::ecse_char16:
    target_code = "UTF-16LE";
    target_char_kind = types::std_int_type::kind::k_short;
    break;

  case target::execution_charset_encoding::ecse_char32:
    target_code = "UTF-32LE";
    target_char_kind = types::std_int_type::kind::k_int;
    break;
  }

  return (std::unique_ptr<execution_charset_encoder>
	  (new execution_charset_encoder_iconv(*this,
					       target_char_kind,
					       std::string(target_code),
					       false)));
}


struct target_aarch64_gcc::_impl_proxy
{

  constexpr _impl_proxy(const target_aarch64_gcc &tgt) noexcept
  : _tgt(tgt)
  {}

  std::shared_ptr<const types::object_type> _create_builtin_va_list_type() const
  {
    return _tgt._create_builtin_va_list_type();
  }

private:
  const target_aarch64_gcc &_tgt;
};

using _impl_proxy = target_aarch64_gcc::_impl_proxy;


void target_aarch64_gcc::_arch_register_int_modes()
{
  _int_mode_enable(common_int_mode_kind::cimk_TI);

  // gcc/config/aarch64/aarch64.h
  // NOTE: cannot find CHAR_TYPE, but I guess this is it.
  _set_std_int_mode(types::std_int_type::kind::k_char,
		    common_int_mode_kind::cimk_QI);
  // SHORT_TYPE_SIZE = 16 (Half Int)
  _set_std_int_mode(types::std_int_type::kind::k_short,
		    common_int_mode_kind::cimk_HI);
  // INT_TYPE_SIZE = 32 (Single Int)
  _set_std_int_mode(types::std_int_type::kind::k_int,
		    common_int_mode_kind::cimk_SI);
  // LONG_LONG_TYPE_SIZE = 64 (Double Int)
  _set_std_int_mode(types::std_int_type::kind::k_long,
		    common_int_mode_kind::cimk_DI);
  // LONG_LONG_TYPE_SIZE = 64 (Double Int)
  _set_std_int_mode(types::std_int_type::kind::k_long_long,
		    common_int_mode_kind::cimk_DI);
}

void target_aarch64_gcc::_arch_register_float_modes()
{
  // FLOAT_MODE defined as in x86_64 in aarch64-modes.def
  const types::ext_float_type::kind
    k_TF{static_cast<int>(float_mode_kind::fmk_TF)};

  _register_float_mode(k_TF, _ieee_quad_format, 16, 4, {"TF", "__TF__"});

  _set_float_n_mode(128, false, k_TF);

  _set_std_float_mode(types::std_float_type::kind::k_float,
		      common_float_mode_kind::cfmk_SF);
  _set_std_float_mode(types::std_float_type::kind::k_double,
		      common_float_mode_kind::cfmk_DF);
  _set_std_float_mode(types::std_float_type::kind::k_long_double, k_TF);
}

types::ext_int_type::kind target_aarch64_gcc::_get_int_max_mode() const noexcept
{
  // NOTE: is this just the max of all the registered mode for this target?
  return types::ext_int_type::kind{
		static_cast<int>(common_int_mode_kind::cimk_DI)
	 };
}

types::ext_int_type::kind target_aarch64_gcc::_get_pointer_mode() const noexcept
{
  return types::ext_int_type::kind{
		static_cast<int>(common_int_mode_kind::cimk_DI)
	 };
}

types::ext_int_type::kind target_aarch64_gcc::_get_word_mode() const noexcept
{
  return types::ext_int_type::kind{
		static_cast<int>(common_int_mode_kind::cimk_DI)
	 };
}

types::ext_int_type::kind target_aarch64_gcc::_get_unwind_word_mode() const noexcept
{
  // NOTE: on x86_64, unwind_word is the same as word. is it the same for aarch64 too?
  return this->_get_word_mode();
}

types::ext_int_type::kind target_aarch64_gcc::_get_wint_mode() const noexcept
{
  // NOTE: from gcc/defaults.h: #define WINT_TYPE "unsigned int"
  return types::ext_int_type::kind{
		static_cast<int>(common_int_mode_kind::cimk_SI)
	 };
}

bool target_aarch64_gcc::_is_wint_signed() const noexcept
{
  return false;
}

types::ext_int_type::kind target_aarch64_gcc::_get_pid_mode() const noexcept
{
  // NOTE: from gcc/defaults.h: #define PID_TYPE "int"
  return types::ext_int_type::kind{
		static_cast<int>(common_int_mode_kind::cimk_SI)
	 };
}

bool target_aarch64_gcc::_is_pid_signed() const noexcept
{
  return true;
}

mpa::limbs::size_type target_aarch64_gcc::_biggest_alignment_bytes_log2()
  const noexcept
{
  // from gcc/gcc/config/aarch64/aarch64.h: #define BIGGEST_ALIGNMENT 128
  return 4;
}

std::shared_ptr<const types::object_type>
target_aarch64_gcc::_create_builtin_va_list_type() const
{
  // TODO:

  // Use a common dummy struct_or_union_def node such that all
  // struct_or_union_type instances created here refer to the same
  // type.
  static std::unique_ptr<ast::struct_or_union_def> soud;

  if (!soud) {
    soud.reset(new ast::struct_or_union_def(pp_tokens_range{},
					    struct_or_union_kind::souk_struct,
					    nullptr, nullptr, nullptr));

    std::unique_ptr<target::sou_layouter> l{
      this->create_sou_layouter(struct_or_union_kind::souk_struct)
    };

    l->add_member("gp_offset",
		  std_int_type::create(std_int_type::kind::k_int, false));
    l->add_member("fp_offset",
		  std_int_type::create(std_int_type::kind::k_int, false));
    l->add_member("overflow_arg_area", void_type::create()->derive_pointer());
    l->add_member("reg_save_area", void_type::create()->derive_pointer());

    soud->set_content(l->grab_result());
  }

  return (struct_or_union_type::create(struct_or_union_kind::souk_struct,
				       soud->get_decl_list_node())
	  ->derive_array(mpa::limbs::from_size_type(1)));
}


namespace
{

  // TODO:
  class _builtin_typedef_va_list final : public builtin_typedef
  {
  public:
    _builtin_typedef_va_list(const target_aarch64_gcc &tgt) noexcept;

    virtual ~_builtin_typedef_va_list() noexcept override;

    virtual std::shared_ptr<const types::addressable_type>
    evaluate(ast::ast&, const target &tgt,
	     const ast::type_specifier_tdid&) const override;

    static std::unique_ptr<_builtin_typedef_va_list>
    create(const target_aarch64_gcc &tgt);

  private:
    const target_aarch64_gcc &_tgt;
  };
}

_builtin_typedef_va_list::_builtin_typedef_va_list(const target_aarch64_gcc &tgt)
  noexcept
  : _tgt(tgt)
{}

_builtin_typedef_va_list::~_builtin_typedef_va_list() noexcept = default;

std::shared_ptr<const types::addressable_type> _builtin_typedef_va_list::
evaluate(ast::ast&, const target &tgt,
	 const ast::type_specifier_tdid&) const
{
  return _impl_proxy{_tgt}._create_builtin_va_list_type();
}

std::unique_ptr<_builtin_typedef_va_list>
_builtin_typedef_va_list::create(const target_aarch64_gcc &tgt)
{
  return
    std::unique_ptr<_builtin_typedef_va_list>{
		new _builtin_typedef_va_list(tgt)
    };
}

void target_aarch64_gcc::_arch_register_builtin_typedefs()
{
  _register_builtin_typedef("__builtin_va_list",
			    std::bind(_builtin_typedef_va_list::create, *this));
}


const gcc_cmdline_parser::option *
target_aarch64_gcc::_arch_get_opt_table() const noexcept
{
  return gcc_opt_table_aarch64;
}

const target_gcc::default_option&
target_aarch64_gcc::_arch_get_option_optimization_table() const noexcept
{
  // TODO:
  static const default_option default_options_table[] = {
    { default_option::opt_levels_none }
  };

  return *default_options_table;
}

bool target_aarch64_gcc::_arch_default_char_is_signed() const noexcept
{
  // from config/aarch64/aarch64.h: #define DEFAULT_SIGNED_CHAR 0
  return false;
}

void target_aarch64_gcc::_arch_option_init_struct()
{
  // Not present in aarch64
}

void
target_aarch64_gcc::_arch_handle_opt(const gcc_cmdline_parser::option * const o,
				    const char *val, const bool negative,
				    const bool generated)
{
  _opts_aarch64.handle_opt(o, val, negative, generated);
}

void target_aarch64_gcc::_arch_option_override()
{
  // This corresponds to GCC's ix86_option_override().
  _opts_aarch64.option_override();
}

void target_aarch64_gcc::_arch_register_builtin_macros(preprocessor &pp) const
{
  // the trick: aarch64-suse-linux-gcc -dM -E - < /dev/null
  const std::initializer_list<std::pair<const char *, const char *>>
      builtin_object_macros = {
          {"__aarch64__", "1"},
          {"__ARM_64BIT_STATE", "1"},
          // TODO: Or 9, depending on the specific target, perhaps this macro
          // should be controlled by the arm version (given on the command line
          // I guess)
          {"__ARM_ARCH", "8"},

          {"__SIZE_TYPE__", "long unsigned int"},
          {"__SIG_ATOMIC_TYPE__", "int"},
          {"__PTRDIFF_TYPE__", "long int"},
          {"__INTPTR_TYPE__", "long int"},
          {"__UINTPTR_TYPE__", "long unsigned int"},
          {"__INTMAX_TYPE__", "long int"},
          {"__UINTMAX_TYPE__", "long unsigned int"},
          {"__INT8_TYPE__", "signed char"},
          {"__UINT8_TYPE__", "unsigned char"},
          {"__INT_LEAST8_TYPE__", "signed char"},
          {"__UINT_LEAST8_TYPE__", "unsigned char"},
          {"__INT16_TYPE__", "short int"},
          {"__UINT16_TYPE__", "short unsigned int"},
          {"__INT_LEAST16_TYPE__", "short int"},
          {"__UINT_LEAST16_TYPE__", "short unsigned int"},
          {"__INT32_TYPE__", "int"},
          {"__UINT32_TYPE__", "unsigned int"},
          {"__INT_LEAST32_TYPE__", "int"},
          {"__UINT_LEAST32_TYPE__", "unsigned int"},
          {"__INT64_TYPE__", "long int"},
          {"__UINT64_TYPE__", "long unsigned int"},
          {"__INT_LEAST64_TYPE__", "long int"},
          {"__UINT_LEAST64_TYPE__", "long unsigned int"},
          {"__INT_FAST8_TYPE__", "signed char"},
          {"__UINT_FAST8_TYPE__", "unsigned char"},
          {"__INT_FAST16_TYPE__", "long int"},
          {"__UINT_FAST16_TYPE__", "long unsigned int"},
          {"__INT_FAST32_TYPE__", "long int"},
          {"__UINT_FAST32_TYPE__", "long unsigned int"},
          {"__INT_FAST64_TYPE__", "long int"},
          {"__UINT_FAST64_TYPE__", "long unsigned int"},
          {"__CHAR8_TYPE__", "unsigned char"},
          {"__CHAR16_TYPE__", "short unsigned int"},
          {"__CHAR32_TYPE__", "unsigned int"},
          {"__WCHAR_TYPE__", "unsigned int"},
          {"__WINT_TYPE__", "unsigned int"},

          {"__CHAR_BIT__", "8"},
          {"__SIZE_MAX__", "0xffffffffffffffffUL"},
          {"__PTRDIFF_MAX__", "0x7fffffffffffffffL"},
          {"__SCHAR_MAX__", "0x7f"},
          {"__INT_MAX__", "0x7fffffff"},
          {"__LONG_LONG_MAX__", "0x7fffffffffffffffLL"},
          {"__LLONG_MAX__", "0x7fffffffffffffffLL"},
          {"__LONG_MAX__", "0x7fffffffffffffffL"},
          {"__SHRT_MAX__", "0x7fff"},
          {"__WCHAR_MAX__", "0xffffffffU"},

          {"__FLT_MIN__", "1.17549435082228750797e-38F"},
          {"__FLT_MAX__", "3.40282346638528859811704183484516925e+38F"},
          {"__DBL_MIN__", "2.22507385850720138309e-308"},
          {"__DBL_MAX__",
          "((double)1.79769313486231570814527423731704357e+308L)"},

          {"__LDBL_MIN__", "3.36210314311209350626267781732175260e-4932L"},
          {"__LDBL_MAX__", "1.18973149535723176508575932662800702e+4932L"},

          {"__FLT_MANT_DIG__", "24"},
          {"__DBL_MANT_DIG__", "53"},
          {"__LDBL_MANT_DIG__", "113"},

          // NOTE: these expands with the final size gcc, but I guess they are
          // fine
          {"__SIZEOF_POINTER__", "sizeof(void*)"},
          {"__SIZEOF_SIZE_T__", "sizeof(unsigned long)"},
          {"__SIZEOF_PTRDIFF_T__", "sizeof(long)"},
          {"__SIZEOF_SHORT__", "sizeof(short)"},
          {"__SIZEOF_INT__", "sizeof(int)"},
          {"__SIZEOF_LONG__", "sizeof(long)"},
          {"__SIZEOF_LONG_LONG__", "sizeof(long long)"},
          {"__SIZEOF_WCHAR_T__", "sizeof(int)"},
          {"__SIZEOF_WINT_T__", "sizeof(unsigned int)"},
          {"__SIZEOF_FLOAT__", "sizeof(float)"},
          {"__SIZEOF_DOUBLE__", "sizeof(double)"},
          {"__SIZEOF_LONG_DOUBLE__", "sizeof(long double)"},

          {"__BIGGEST_ALIGNMENT__", "16"},
      };

  for (const auto &bom : builtin_object_macros)
    pp.register_builtin_macro(bom.first, bom.second);

  if (_is_int_mode_enabled(common_int_mode_kind::cimk_TI)) {
    pp.register_builtin_macro("__SIZEOF_INT128__", "sizeof(__int128)");
  }
}

target_aarch64_gcc::opts_aarch64::_target_flags_type target_aarch64_gcc::opts_aarch64::
_init_valid_target_flags(const gcc_cmdline_parser::gcc_version &ver)
{
  _target_flags_type flags;
  flags.set();

  return flags;
}


target_aarch64_gcc::opts_aarch64::_isa_flags_type target_aarch64_gcc::opts_aarch64::
_init_valid_isa_flags(const gcc_cmdline_parser::gcc_version &ver)
{
  _isa_flags_type flags;
  flags.set();

  return flags;
}

target_aarch64_gcc::opts_aarch64::
opts_aarch64(target_aarch64_gcc &t) noexcept
  : _t(t), _valid_target_flags(_init_valid_target_flags(t.get_gcc_version())),
    _valid_isa_flags(_init_valid_isa_flags(t.get_gcc_version())),
    _arch(nullptr), _tune(nullptr),
    _cmodel(_cm_32), _cmodel_set(false), _fpmath(1UL << _fpmath_bit_387),
    _fpmath_set(false), force_align_arg_pointer(false),
    force_align_arg_pointer_set(false), preferred_stack_boundary_arg(0),
    preferred_stack_boundary_arg_set(false), preferred_stack_boundary(0),
    incoming_stack_boundary_arg(0), incoming_stack_boundary_arg_set(false),
    incoming_stack_boundary(0) {}

void target_aarch64_gcc::opts_aarch64::
handle_opt(const gcc_cmdline_parser::option * const o,
	   const char *val, const bool negative,
	   const bool generated) { }

void target_aarch64_gcc::opts_aarch64::option_override() { }
