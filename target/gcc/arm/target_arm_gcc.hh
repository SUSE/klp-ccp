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

#ifndef TARGET_AARCH64_GCC_HH
#define TARGET_AARCH64_GCC_HH

#include <bitset>
#include "../target_gcc.hh"
#include "../gcc_cmdline_parser.hh"

namespace klp
{
  namespace ccp
  {
    class target_aarch64_gcc final : public target_gcc
    {
    public:
      target_aarch64_gcc(const char * const version);

      virtual ~target_aarch64_gcc() noexcept override;

      virtual bool is_wchar_signed() const noexcept override;

      virtual bool is_bitfield_default_signed() const noexcept override;

      virtual std::unique_ptr<execution_charset_encoder>
      get_execution_charset_encoder(const execution_charset_encoding e)
	const override;

    public:
      struct _impl_proxy;
      friend struct _impl_proxy;

    private:
      enum class float_mode_kind
      {
	fmk_TF = static_cast<int>(common_float_mode_kind::cfmk_LAST) + 1,
      };

      virtual void _arch_register_int_modes() override;

      virtual void _arch_register_float_modes() override;

      virtual types::ext_int_type::kind _get_int_max_mode()
	const noexcept override;

      virtual types::ext_int_type::kind _get_pointer_mode()
	const noexcept override;

      virtual types::ext_int_type::kind _get_word_mode()
	const noexcept override;

      virtual types::ext_int_type::kind _get_unwind_word_mode()
	const noexcept override;

      virtual types::ext_int_type::kind _get_wint_mode()
	const noexcept override;
      virtual bool _is_wint_signed() const noexcept override;

      virtual types::ext_int_type::kind _get_pid_mode() const noexcept override;
      virtual bool _is_pid_signed() const noexcept override;

      virtual mpa::limbs::size_type _biggest_alignment_bytes_log2()
	const noexcept override;

      virtual std::shared_ptr<const types::object_type>
      _create_builtin_va_list_type() const override;

      virtual void _arch_register_builtin_typedefs() override;

    private:
      virtual const gcc_cmdline_parser::option *
      _arch_get_opt_table() const noexcept override;

      virtual const default_option&
      _arch_get_option_optimization_table() const noexcept override;

      virtual bool _arch_default_char_is_signed() const noexcept override;

      virtual void _arch_option_init_struct() override;

      virtual void
      _arch_handle_opt(const gcc_cmdline_parser::option * const o,
		       const char *val, const bool negative,
		       const bool generated) override;

      virtual void _arch_option_override() override;

      virtual void _arch_register_builtin_macros(preprocessor &pp)
	const override;

      struct opts_aarch64
      {
      public:
	opts_aarch64(target_aarch64_gcc &t) noexcept;

	void handle_opt(const gcc_cmdline_parser::option * const o,
			const char *val, const bool negative,
			const bool generated);

	void option_override();

      private:
	enum target_flag
	{
	  // TODO:
	  _target_flag_max,
	};

	enum isa_flag
	{
	  // TODO:
	  _isa_flag_max,
	};

	typedef std::bitset<_target_flag_max> _target_flags_type;
	typedef std::bitset<_isa_flag_max> _isa_flags_type;

	static _target_flags_type
	_init_valid_target_flags(const gcc_cmdline_parser::gcc_version &ver);

	static _isa_flags_type
	_init_valid_isa_flags(const gcc_cmdline_parser::gcc_version &ver);

	template <target_flag b>
	void __set_target_flag(_target_flags_type &flags) noexcept;

	template <target_flag b>
	void __unset_target_flag(_target_flags_type &flags) noexcept;

	template <target_flag b>
	void _set_target_flag_user(const bool value, const bool generated)
	  noexcept;

	template <isa_flag b>
	void __set_isa_flag(_isa_flags_type &flags) const noexcept;

	template <isa_flag b>
	void _set_isa_flag_explicit() noexcept;

	template <isa_flag b>
	void __unset_isa_flag(_isa_flags_type &flags, const bool set)
	  const noexcept;

	template <isa_flag b>
	void _unset_isa_flag_explicit() noexcept;

	template <isa_flag b>
	void _set_isa_flag_user(const bool value, const bool generated)
	  noexcept;


	target_aarch64_gcc &_t;

	const _target_flags_type _valid_target_flags;
	_target_flags_type _target_flags;
	_target_flags_type _target_flags_set;

	const _isa_flags_type _valid_isa_flags;
	_isa_flags_type _isa_flags;
	_isa_flags_type _isa_flags_explicit;
	_isa_flags_type _isa_flags_set;

	struct _pta;
	struct _tune_features;

	const _pta *_arch;
	const _pta *_tune;
	std::string _arch_string;
	std::string _tune_string;
	std::string _tune_ctrl_string;

	enum _cmodel
	{
	  // TODO:
	  _cm_32,
	  _cm_small,
	  _cm_kernel,
	  _cm_medium,
	  _cm_large,
	  _cm_small_pic,
	  _cm_medium_pic,
	  _cm_large_pic,
	};
	_cmodel _cmodel;
	bool _cmodel_set;

	enum _fpmath_bit
	{
	  // TODO:
	  _fpmath_bit_387,
	  _fpmath_bit_sse,
	  __fpmath_bit_max,
	};
	std::bitset<__fpmath_bit_max> _fpmath;
	bool _fpmath_set;

	bool force_align_arg_pointer;
	bool force_align_arg_pointer_set;

	unsigned int preferred_stack_boundary_arg;
	bool preferred_stack_boundary_arg_set;
	unsigned int preferred_stack_boundary;

	unsigned int incoming_stack_boundary_arg;
	bool incoming_stack_boundary_arg_set;
	unsigned int incoming_stack_boundary;
      };

      opts_aarch64 _opts_aarch64;
    };
  }
}

#endif


