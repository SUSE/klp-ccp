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

#ifndef ARCH_X86_64_GCC_HH
#define ARCH_X86_64_GCC_HH

#include "architecture.hh"

namespace klp
{
  namespace ccp
  {
    class arch_x86_64_gcc final : public architecture
    {
    public:
      arch_x86_64_gcc(const char * const version);

      virtual ~arch_x86_64_gcc() noexcept override = default;

      virtual void parse_command_line
		(int argc, const char *argv[],
		 header_resolver &hr,
		 preprocessor &pp,
		 const std::function<void(const std::string&)> &report_warning)
	override;

      virtual const builtin_typedef::factories&
      get_builtin_typedefs() const noexcept override;

      virtual bool is_char_signed() const noexcept override;
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
      static std::tuple<unsigned int, unsigned int, unsigned int>
      _parse_version(const char * const version);

      void _register_builtin_macros(preprocessor &pp) const;

      static types::std_int_type::kind
      _width_to_int_kind(const mpa::limbs::size_type w) noexcept;

      unsigned int _gcc_ver_major;
      unsigned int _gcc_ver_minor;
      unsigned int _gcc_ver_patchlevel;

      builtin_typedef::factories _builtin_typedefs;
    };
  }
}

#endif
