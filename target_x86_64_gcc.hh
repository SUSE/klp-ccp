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

      virtual types::std_int_type::kind
      int_mode_to_std_int_kind(const int_mode_kind m) const noexcept override;

      virtual mpa::limbs::size_type
      get_float_significand_width(const types::float_type::kind k)
	const noexcept override;

      virtual mpa::limbs::size_type
      get_float_exponent_width(const types::float_type::kind k)
	const noexcept override;

      virtual types::float_type::kind
      float_mode_to_float_kind(const float_mode_kind m) const noexcept override;

      virtual int_mode_kind get_pointer_mode() const noexcept override;

      virtual int_mode_kind get_word_mode() const noexcept override;

      virtual mpa::limbs::size_type get_biggest_alignment_log2()
	const noexcept override;

      virtual std::unique_ptr<execution_charset_encoder>
      get_execution_charset_encoder(const execution_charset_encoding e)
	const override;

      virtual mpa::limbs get_std_int_size(const types::std_int_type::kind k)
	const override;

      virtual mpa::limbs::size_type
      get_std_int_alignment(const types::std_int_type::kind k) const override;

      virtual mpa::limbs get_float_size(const types::float_type::kind k,
					const bool is_complex) const override;

      virtual mpa::limbs::size_type
      get_float_alignment(const types::float_type::kind k,
			  const bool) const override;

      virtual mpa::limbs get_pointer_size() const override;

      virtual mpa::limbs::size_type get_pointer_alignment() const override;

      virtual mpa::limbs get_va_list_size() const override;

      virtual mpa::limbs::size_type get_va_list_alignment() const override;

      virtual void evaluate_enum_type(ast::ast &a, types::enum_content &ec,
				      const bool packed,
				      const int_mode_kind mode,
				      types::alignment &&user_align)
	const override;

      virtual void layout_struct(types::struct_or_union_content &souc,
				 const types::alignment &user_align)
	const override;

      virtual void layout_union(types::struct_or_union_content &souc,
				 const types::alignment &user_align)
	const override;

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
      static types::std_int_type::kind
      _width_to_int_kind(const mpa::limbs::size_type w) noexcept;

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
	target_x86_64_gcc &_t;

	struct _pta;

	const _pta *_arch;
	const _pta *_tune;
	std::string _arch_string;
	std::string _tune_string;
      };

      opts_x86 _opts_x86;

      builtin_typedef::factories _builtin_typedefs;
    };
  }
}

#endif
