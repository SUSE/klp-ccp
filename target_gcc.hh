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

#ifndef TARGET_GCC_HH
#define TARGET_GCC_HH

#include "target.hh"
#include "gcc_cmdline_parser.hh"
#include "preprocessor.hh"

namespace klp
{
  namespace ccp
  {
    class preprocessor;

    class target_gcc : public target
    {
    public:
      target_gcc(const char * const version);

      virtual ~target_gcc() noexcept = 0;

      virtual void parse_command_line
		(int argc, const char *argv[],
		 header_resolver &hr,
		 preprocessor &pp,
		 const std::function<void(const std::string&)> &report_warning)
	override;

      const gcc_cmdline_parser::gcc_version&
      get_gcc_version() const noexcept
      { return _gcc_version; }

    protected:
      struct opts_c_family;

      struct opts_common
      {
	opts_common(const gcc_cmdline_parser::gcc_version &ver) noexcept;

	void init_options_struct() noexcept;
	void c_lang_init_options_struct() noexcept;
	void c_lang_init_options() noexcept;

	void handle_opt(const gcc_cmdline_parser::option * const o,
			const char *val, const bool negative,
			const bool generated,
			const gcc_cmdline_parser::gcc_version &ver);

	void finish_options() noexcept;

	void c_lang_post_options(const gcc_cmdline_parser::gcc_version &ver,
				 const opts_c_family &c_opts)
	  noexcept;

	void process_options(const gcc_cmdline_parser::gcc_version &ver);

	std::string base_file;

	unsigned int optimize;
	bool optimize_debug;
	bool optimize_fast;
	bool optimize_size;

	enum class excess_precision
	{
	  ep_default,
	  ep_fast,
	  ep_standard,
	};

	bool flag_associative_math;
	excess_precision flag_excess_precision;
	bool flag_finite_math_only;
	bool flag_errno_math;
	bool flag_reciprocal_math;
	bool flag_signaling_nans;
	bool flag_signed_zeros;
	bool flag_trapping_math;
	bool flag_unsafe_math_optimizations;

	bool fast_math_flags_set_p() const noexcept;

	enum class fp_contract_mode
	{
	  fcm_off,
	  fcm_fast,
	};
	fp_contract_mode flag_fp_contract_mode;
	bool flag_fp_contract_mode_set;

	enum class permitted_flt_eval_methods
	{
	  pfem_default,
	  pfem_c11,
	  pfem_ts_18661
	};
	permitted_flt_eval_methods flag_permitted_flt_eval_methods;
	bool flag_permitted_flt_eval_methods_set;

	bool flag_single_precision_constant;

	unsigned int flag_abi_version;

      private:
	void _set_unsafe_math_optimizations_flags(const bool set) noexcept;
	void _set_fast_math_flags(const bool set) noexcept;
      };

      struct opts_c_family
      {
	opts_c_family() noexcept;

	void init_options_struct() noexcept;
	void c_lang_init_options_struct() noexcept;
	void c_lang_init_options(const gcc_cmdline_parser::gcc_version &ver)
	  noexcept;

	void handle_opt(const gcc_cmdline_parser::option * const o,
			const char *val, const bool negative,
			const bool generated,
			const gcc_cmdline_parser::gcc_version &ver);

	void finish_options() noexcept;

	void c_lang_post_options();

	void process_options() noexcept;

	bool is_iso() const noexcept;
	bool is_std_geq_c99() const noexcept;

	std::vector<std::string> pre_includes;
	std::vector<std::string> include_dirs;
	std::vector<std::string> include_dirs_quoted;
	std::vector<std::string> include_dirs_system;
	std::vector<std::string> include_dirs_after;

	struct macro_def_or_undef
	{
	  bool undef;
	  std::string arg;
	};
	std::vector<macro_def_or_undef> macro_defs_and_undefs;

	bool flag_undef;

	enum class c_lang_kind
	{
	  clk_stdc89,
	  clk_stdc94,
	  clk_stdc99,
	  clk_stdc11,
	  clk_stdc17,
	  clk_stdc2x,
	  clk_gnuc89,
	  clk_gnuc99,
	  clk_gnuc11,
	  clk_gnuc17,
	  clk_gnuc2x,
	};

	c_lang_kind c_std;

      private:
	void _set_std_c89(const bool c94, const bool iso) noexcept;
	void _set_std_c99(const bool iso) noexcept;
	void _set_std_c11(const bool iso) noexcept;
	void _set_std_c17(const bool iso) noexcept;
	void _set_std_c2x(const bool iso) noexcept;
      };

      struct default_option
      {
	enum opt_levels
	{
	  opt_levels_none,
	  opt_levels_all,
	  opt_levels_0_only,
	  opt_levels_1_plus,
	  opt_levels_1_plus_speed_only,
	  opt_levels_1_plus_not_debug,
	  opt_levels_2_plus,
	  opt_levels_2_plus_speed_only,
	  opt_levels_3_plus,
	  opt_levels_3_plus_and_size,
	  opt_levels_size,
	  opt_levels_fast,
	};

	opt_levels levels;
	const char *name;
	const char *val;
	bool negative;
	gcc_cmdline_parser::gcc_version min_gcc_version;
	gcc_cmdline_parser::gcc_version max_gcc_version;
      };

    private:
      virtual const gcc_cmdline_parser::option *
      _arch_get_opt_table() const noexcept = 0;

      virtual const default_option&
      _arch_get_option_optimization_table() const noexcept = 0;

      virtual void _arch_option_init_struct() = 0;

      virtual void
      _arch_handle_opt(const gcc_cmdline_parser::option * const o,
		       const char *val, const bool negative,
		       const bool generated) = 0;


      virtual void _arch_option_override() = 0;

      virtual void _arch_register_builtin_macros(preprocessor &pp) const = 0;

      static gcc_cmdline_parser::gcc_version
      _parse_version(const char * const version);

      void _init_options_struct() noexcept;
      void _c_lang_init_options_struct() noexcept;
      void _c_lang_init_options() noexcept;

      void _decode_options
	(int argc, const char *argv[],
	 const std::function<void(const std::string&)> &report_warning);

      void _default_options_optimization
	(const gcc_cmdline_parser::decoded_opts_type &decoded_opts,
	 const gcc_cmdline_parser &p,
	 const gcc_cmdline_parser::option &opt_table_arch);

      void _maybe_default_options
	(const struct default_option &table,
	 const gcc_cmdline_parser &p,
	 const gcc_cmdline_parser::option &opt_table_arch);

      void
      _handle_opt(const gcc_cmdline_parser::option * const opt_table,
		  const gcc_cmdline_parser::option * const o,
		  const char *val, const bool negative,
		  const bool generated,
		  const gcc_cmdline_parser::option &opt_table_arch);

      void _finish_options();
      void _process_options();
      void _c_lang_post_options();

      void _register_builtin_macros(preprocessor &pp) const;

      const gcc_cmdline_parser::gcc_version _gcc_version;

      opts_common _opts_common;
      opts_c_family _opts_c_family;
    };
  }
}

#endif
