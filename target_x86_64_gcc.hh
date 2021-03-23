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

#ifndef TARGET_X86_64_GCC_HH
#define TARGET_X86_64_GCC_HH

#include <bitset>
#include "target_gcc.hh"
#include "gcc_cmdline_parser.hh"

namespace klp
{
  namespace ccp
  {
    class target_x86_64_gcc final : public target_gcc
    {
    public:
      target_x86_64_gcc(const char * const version);

      virtual ~target_x86_64_gcc() noexcept override;

      virtual const builtin_typedef::factories&
      get_builtin_typedefs() const noexcept override;

      virtual bool is_wchar_signed() const noexcept override;

      virtual bool is_bitfield_default_signed() const noexcept override;

      virtual mpa::limbs::size_type
      get_std_int_width(const types::std_int_type::kind k) const noexcept
	override;

      virtual mpa::limbs get_std_int_size(const types::std_int_type::kind k)
	const override;

      virtual mpa::limbs::size_type
      get_std_int_alignment(const types::std_int_type::kind k) const override;

      virtual mpa::limbs::size_type
      get_ext_int_width(const types::ext_int_type::kind k)
	const noexcept override;

      virtual mpa::limbs get_ext_int_size(const types::ext_int_type::kind k)
	const override;

      virtual mpa::limbs::size_type
      get_ext_int_alignment(const types::ext_int_type::kind k) const override;

      virtual mpa::limbs::size_type
      get_float_significand_width(const types::std_float_type::kind k)
	const noexcept override;

      virtual mpa::limbs::size_type
      get_float_exponent_width(const types::std_float_type::kind k)
	const noexcept override;

      virtual mpa::limbs get_float_size(const types::std_float_type::kind k)
	const override;

      virtual mpa::limbs::size_type
      get_float_alignment(const types::std_float_type::kind k) const override;

      virtual mpa::limbs get_pointer_size() const override;

      virtual mpa::limbs::size_type get_pointer_alignment() const override;

      virtual mpa::limbs::size_type get_biggest_alignment_log2()
	const noexcept override;

      virtual std::unique_ptr<execution_charset_encoder>
      get_execution_charset_encoder(const execution_charset_encoding e)
	const override;

      virtual mpa::limbs get_va_list_size() const override;

      virtual mpa::limbs::size_type get_va_list_alignment() const override;

    private:
      virtual types::std_int_type::kind
      _int_mode_to_std_int_kind(const int_mode_kind m) const noexcept override;

      virtual types::std_float_type::kind
      _float_mode_to_float_kind(const float_mode_kind m)
	const noexcept override;

      virtual int_mode_kind _get_pointer_mode() const noexcept override;

      virtual int_mode_kind _get_word_mode() const noexcept override;

      virtual void _evaluate_enum_type(ast::ast &a, types::enum_content &ec,
				       const bool packed,
				       const int_mode_kind mode,
				       types::alignment &&user_align)
	const override;

      virtual void _layout_struct(types::struct_or_union_content &souc,
				  const types::alignment &user_align)
	const override;

      virtual void _layout_union(types::struct_or_union_content &souc,
				 const types::alignment &user_align)
	const override;

    public:
      virtual std::shared_ptr<const types::object_type>
      create_builtin_va_list_type() const override;

      virtual types::std_int_type::kind
      get_wint_kind() const noexcept override;

      virtual bool is_wint_signed() const noexcept override;

      virtual types::std_int_type::kind
      get_int_max_kind() const noexcept override;

      virtual types::std_int_type::kind get_pid_t_kind() const noexcept override;
      virtual bool is_pid_t_signed() const noexcept override;

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

      struct opts_x86
      {
      public:
	opts_x86(target_x86_64_gcc &t) noexcept;

	void option_init_struct() noexcept;

	void handle_opt(const gcc_cmdline_parser::option * const o,
			const char *val, const bool negative,
			const bool generated);

	void option_override();

      private:
	enum target_flag
	{
	  target_flag_128bit_long_double,
	  target_flag_80387,
	  target_flag_accumulate_outgoing_args,
	  target_flag_align_double,
	  target_flag_avx256_split_unaligned_load,
	  target_flag_avx256_split_unaligned_store,
	  target_flag_call_ms2sysv_xlogues,
	  target_flag_cld,
	  target_flag_float_returns,
	  target_flag_general_regs_only,
	  target_flag_iamcu,
	  target_flag_ieee_fp,
	  target_flag_inline_all_stringops,
	  target_flag_inline_stringops_dynamically,
	  target_flag_long_double_128,
	  target_flag_long_double_64,
	  target_flag_ms_bitfield_layout,
	  target_flag_no_align_stringops,
	  target_flag_no_fancy_math_387,
	  target_flag_no_push_args,
	  target_flag_no_red_zone,
	  target_flag_omit_leaf_frame_pointer,
	  target_flag_prefer_avx128,
	  target_flag_recip,
	  target_flag_rtd,
	  target_flag_sseregparm,
	  target_flag_stack_probe,
	  target_flag_stv,
	  target_flag_tls_direct_seg_refs,
	  target_flag_use_8bit_idiv,
	  target_flag_vect8_returns,
	  target_flag_vzeroupper,

	  _target_flag_max,
	};

	enum isa_flag
	{
	  isa_flag_3dnow,
	  isa_flag_3dnow_a,
	  isa_flag_64bit,
	  isa_flag_abi_64,
	  isa_flag_abi_x32,
	  isa_flag_abm,
	  isa_flag_adx,
	  isa_flag_aes,
	  isa_flag_avx,
	  isa_flag_avx2,
	  isa_flag_avx5124fmaps,
	  isa_flag_avx5124vnniw,
	  isa_flag_avx512bitalg,
	  isa_flag_avx512bw,
	  isa_flag_avx512cd,
	  isa_flag_avx512dq,
	  isa_flag_avx512er,
	  isa_flag_avx512f,
	  isa_flag_avx512ifma,
	  isa_flag_avx512pf,
	  isa_flag_avx512vbmi,
	  isa_flag_avx512vbmi2,
	  isa_flag_avx512vl,
	  isa_flag_avx512vnni,
	  isa_flag_avx512vpopcntdq,
	  isa_flag_bmi,
	  isa_flag_bmi2,
	  isa_flag_cldemote,
	  isa_flag_clflushopt,
	  isa_flag_clwb,
	  isa_flag_clzero,
	  isa_flag_code16,
	  isa_flag_crc32,
	  isa_flag_cx16,
	  isa_flag_f16c,
	  isa_flag_fma,
	  isa_flag_fma4,
	  isa_flag_fsgsbase,
	  isa_flag_fxsr,
	  isa_flag_gfni,
	  isa_flag_hle,
	  isa_flag_lwp,
	  isa_flag_lzcnt,
	  isa_flag_mmx,
	  isa_flag_movbe,
	  isa_flag_movdir64b,
	  isa_flag_movdiri,
	  isa_flag_mwaitx,
	  isa_flag_mpx,
	  isa_flag_pclmul,
	  isa_flag_pcommit,
	  isa_flag_pconfig,
	  isa_flag_pku,
	  isa_flag_popcnt,
	  isa_flag_prefetchwt1,
	  isa_flag_prfchw,
	  isa_flag_ptwrite,
	  isa_flag_rdpid,
	  isa_flag_rdrnd,
	  isa_flag_rdseed,
	  isa_flag_rtm,
	  isa_flag_sahf,
	  isa_flag_sgx,
	  isa_flag_sha,
	  isa_flag_shstk,
	  isa_flag_sse,
	  isa_flag_sse2,
	  isa_flag_sse3,
	  isa_flag_sse4_1,
	  isa_flag_sse4_2,
	  isa_flag_sse4a,
	  isa_flag_ssse3,
	  isa_flag_tbm,
	  isa_flag_vaes,
	  isa_flag_vpclmulqdq,
	  isa_flag_waitpkg,
	  isa_flag_wbnoinvd,
	  isa_flag_xop,
	  isa_flag_xsave,
	  isa_flag_xsavec,
	  isa_flag_xsaveopt,
	  isa_flag_xsaves,

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


	target_x86_64_gcc &_t;

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

      opts_x86 _opts_x86;

      builtin_typedef::factories _builtin_typedefs;
    };
  }
}

#endif
