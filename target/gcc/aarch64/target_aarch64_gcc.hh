/*
 * Copyright (C) 2026  SUSE Software Solutions Germany GmbH
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

	unsigned int get_arch_version() const noexcept;

      private:
	// Kept for future use: needed when opts_aarch64 grows
	// feature flag or tuning logic that calls back into the
	// target (e.g. _t.get_gcc_version()).
	target_aarch64_gcc &_t;

	struct _arch_info;

	const _arch_info *_arch;
	std::string _arch_string;
	unsigned int _arch_version;
      };

      opts_aarch64 _opts_aarch64;
    };
  }
}

#endif


