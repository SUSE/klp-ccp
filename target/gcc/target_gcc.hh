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

#include <bitset>
#include <initializer_list>
#include <vector>
#include <map>
#include "target/target.hh"
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

      virtual const builtin_func::factory*
      lookup_builtin_func(const std::string &id) const noexcept override;

      virtual const builtin_typedef::factories&
      get_builtin_typedefs() const noexcept override;

      virtual std::shared_ptr<const types::pointer_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::pointer_type> &&t,
			  ast::type_qualifier_list &tql) const override;

      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::attribute_specifier_list * const asl)
	const override;

      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::declaration_specifiers &ds,
			  ast::attribute_specifier_list * const asl)
	const override;

      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::specifier_qualifier_list &sql) const override;

      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::declaration_specifiers &ds,
			  ast::attribute_specifier_list * const asl_before,
			  ast::attribute_specifier_list * const asl_middle,
			  ast::attribute_specifier_list * const asl_after)
	const override;


      virtual bool
      is_gnu_inline(ast::ast &a,
		    ast::function_definition &fd) const final;

      virtual bool
      is_always_inline(ast::ast &a,
		       ast::function_definition &fd) const final;

      virtual bool is_char_signed() const noexcept final;

      virtual mpa::limbs::size_type
      get_std_int_width(const types::std_int_type::kind k)
	const noexcept override;

      virtual mpa::limbs get_std_int_size(const types::std_int_type::kind k)
	const override;

      virtual mpa::limbs::size_type
      get_std_int_alignment(const types::std_int_type::kind k)
	const noexcept override;

      virtual mpa::limbs::size_type
      get_ext_int_width(const types::ext_int_type::kind k)
	const noexcept override;

      virtual mpa::limbs get_ext_int_size(const types::ext_int_type::kind k)
	const override;

      virtual mpa::limbs::size_type
      get_ext_int_alignment(const types::ext_int_type::kind k)
	const noexcept override;

      virtual ext_int_keywords get_ext_int_keywords() const override;

      virtual std::shared_ptr<const types::int_type>
      width_to_int_type(const mpa::limbs::size_type w, const bool is_signed,
			const bool std_int_required)
	const override;

      virtual std::shared_ptr<const types::int_type>
      create_int_max_type(const bool is_signed) const override;

      virtual const types::real_float_type::format&
      get_std_float_format(const types::std_float_type::kind k)
	const noexcept override;

      virtual mpa::limbs get_std_float_size(const types::std_float_type::kind k)
	const override;

      virtual mpa::limbs::size_type
      get_std_float_alignment(const types::std_float_type::kind k)
	const override;

      virtual const types::real_float_type::format&
      get_ext_float_format(const types::ext_float_type::kind k)
	const noexcept override;

      virtual mpa::limbs get_ext_float_size(const types::ext_float_type::kind k)
	const override;

      virtual mpa::limbs::size_type
      get_ext_float_alignment(const types::ext_float_type::kind k)
	const override;

      virtual ext_float_keywords get_ext_float_keywords() const override;

      virtual std::shared_ptr<const types::int_type>
      create_ptrdiff_type(const bool is_signed) const override;

      virtual mpa::limbs::size_type get_ptrdiff_width() const noexcept override;

      virtual mpa::limbs get_pointer_size() const override;

      virtual mpa::limbs::size_type get_pointer_alignment()
	const noexcept override;

      virtual void
      evaluate_enum_type(ast::ast &a,
			 const std::function<void(ast::expr&)> &eval_expr,
			 ast::attribute_specifier_list * const ed_asl_before,
			 ast::attribute_specifier_list * const ed_asl_after,
			 types::enum_content &ec) const override;

      virtual void
      apply_sou_attributes(ast::ast &a,
			   const std::function<void(ast::expr&)> &eval_expr,
			   const types::struct_or_union_kind souk,
			   ast::attribute_specifier_list * const asl_before,
			   ast::attribute_specifier_list * const asl_after,
			   types::struct_or_union_content &t) const override;

    public:
      virtual std::unique_ptr<target::sou_layouter>
      create_sou_layouter(const types::struct_or_union_kind souk,
			  ast::attribute_specifier_list * const soud_asl_before,
			  ast::attribute_specifier_list * const soud_asl_after,
			  klp::ccp::ast::ast &a,
			  const sou_layouter::expr_evaluator_type &expr_eval)
	const override;

    protected:
      std::unique_ptr<target::sou_layouter>
      create_sou_layouter(const types::struct_or_union_kind souk) const;

    private:
      class sou_layouter;

    public:
      struct _impl_proxy;
      friend struct _impl_proxy;

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

	bool flag_expensive_optimizations;

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
	int flag_leading_underscore;
	bool flag_no_inline;

	int flag_pic;
	int flag_pie;

	bool flag_omit_frame_pointer;
	bool flag_omit_frame_pointer_set;
	bool flag_exceptions;
	bool flag_non_call_exceptions;
	bool flag_unwind_tables;
	int flag_asynchronous_unwind_tables;

	bool flag_pack_struct;
	unsigned int initial_max_fld_align_ffs;

	int flag_stack_protect;

	enum sanitizer_flags_bit
	{
	  sanitizer_flags_bit_address,
	  sanitizer_flags_bit_user_address,
	  sanitizer_flags_bit_kernel_address,
	  sanitizer_flags_bit_thread,
	  sanitizer_flags_bit_leak,
	  sanitizer_flags_bit_shift_base,
	  sanitizer_flags_bit_shift_exponent,
	  sanitizer_flags_bit_divide,
	  sanitizer_flags_bit_unreachable,
	  sanitizer_flags_bit_vla,
	  sanitizer_flags_bit_null,
	  sanitizer_flags_bit_return,
	  sanitizer_flags_bit_si_overflow,
	  sanitizer_flags_bit_bool,
	  sanitizer_flags_bit_enum,
	  sanitizer_flags_bit_float_divide,
	  sanitizer_flags_bit_float_cast,
	  sanitizer_flags_bit_bounds,
	  sanitizer_flags_bit_alignment,
	  sanitizer_flags_bit_nonnull_attribute,
	  sanitizer_flags_bit_returns_nonnull_attribute,
	  sanitizer_flags_bit_object_size,
	  sanitizer_flags_bit_vptr,
	  sanitizer_flags_bit_bounds_strict,
	  sanitizer_flags_bit_pointer_overflow,
	  sanitizer_flags_bit_builtin,
	  sanitizer_flags_bit_pointer_compare,
	  sanitizer_flags_bit_pointer_subtract,

	  _sanitizer_flags_bit_max,
	};

	typedef std::bitset<_sanitizer_flags_bit_max> sanitizer_flags_type;
	sanitizer_flags_type flag_sanitize;

      private:
	struct _sanitizer_opts_impl;

	void _set_unsafe_math_optimizations_flags(const bool set) noexcept;
	void _set_fast_math_flags(const bool set) noexcept;
      };

      struct opts_c_family
      {
	opts_c_family() noexcept;

	void init_options_struct(const target_gcc &target) noexcept;
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

	int flag_gnu89_inline;
	bool flag_signed_char;

	bool flag_openacc;
	bool flag_openmp;

	bool flag_hosted;

      private:
	void _set_std_c89(const bool c94, const bool iso) noexcept;
	void _set_std_c99(const bool iso) noexcept;
	void _set_std_c11(const bool iso) noexcept;
	void _set_std_c17(const bool iso) noexcept;
	void _set_std_c2x(const bool iso) noexcept;
      };

      opts_common& get_opts_common() noexcept
      { return _opts_common; }

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

      virtual bool _arch_default_char_is_signed() const noexcept = 0;

      virtual void _arch_option_init_struct() = 0;

      virtual void
      _arch_handle_opt(const gcc_cmdline_parser::option * const o,
		       const char *val, const bool negative,
		       const bool generated) = 0;


      virtual void _arch_option_override() = 0;

      virtual void _arch_register_builtin_macros(preprocessor &pp) const = 0;

    protected:
      enum class common_int_mode_kind
      {
	cimk_QI,
	cimk_HI,
	cimk_SI,
	cimk_DI,
	cimk_TI,
      };

    private:
      struct int_mode
      {
	int_mode() noexcept;

	int_mode(const types::ext_int_type::kind _mode,
		 const mpa::limbs::size_type _width,
		 const mpa::limbs::size_type _size,
		 const mpa::limbs::size_type _alignment,
		 const bool _enabled,
		 const bool _create_int_n_type_specifier) noexcept;

	types::ext_int_type::kind mode;
	mpa::limbs::size_type width;
	mpa::limbs::size_type size;
	mpa::limbs::size_type alignment;
	bool enabled;
	bool create_int_n_type_specifier;
	bool is_std_int;
      };

    protected:
      void _register_int_mode(const types::ext_int_type::kind mode,
			      const mpa::limbs::size_type width,
			      const mpa::limbs::size_type size,
			      const mpa::limbs::size_type alignment,
			      const std::initializer_list<const char *> names,
			      const bool create_int_n_type_specifier = false,
			      const bool enabled = true);

      void _register_int_mode(const common_int_mode_kind cimk,
			      const mpa::limbs::size_type width,
			      const mpa::limbs::size_type size,
			      const mpa::limbs::size_type alignment,
			      const std::initializer_list<const char *> names,
			      const bool create_int_n_type_specifier = false,
			      const bool enabled = true);

      void _int_mode_enable(const types::ext_int_type::kind mode);
      void _int_mode_enable(const common_int_mode_kind cimk);

      bool _is_int_mode_enabled(const types::ext_int_type::kind mode)
	const noexcept;
      bool _is_int_mode_enabled(const common_int_mode_kind cimk)
	const noexcept;

      void _set_std_int_mode(const types::std_int_type::kind std_int_kind,
			     const types::ext_int_type::kind mode);
      void _set_std_int_mode(const types::std_int_type::kind std_int_kind,
			     const common_int_mode_kind cimk);

    private:
      void _register_int_modes();
      virtual void _arch_register_int_modes() = 0;

      std::shared_ptr<const types::int_type>
      _int_mode_to_type(const types::ext_int_type::kind mode,
			const bool is_signed,
			const types::qualifiers &qs = types::qualifiers{})
	const;

      std::shared_ptr<const types::int_type>
      _int_mode_to_type(const common_int_mode_kind cimk,
			const bool is_signed,
			const types::qualifiers &qs = types::qualifiers{})
	const;

      const int_mode&
      _std_int_kind_to_int_mode(const types::std_int_type::kind std_int_kind)
	const noexcept;

      const int_mode* _width_to_int_mode(const mpa::limbs::size_type w)
	const noexcept;

    protected:
      mpa::limbs::size_type
      _int_mode_to_width(const types::ext_int_type::kind mode) const;

    private:
      mpa::limbs::size_type
      _int_mode_to_size(const types::ext_int_type::kind mode) const;

      mpa::limbs::size_type
      _int_mode_to_alignment(const types::ext_int_type::kind mode) const;

    protected:
      enum class common_float_mode_kind
      {
	cfmk_SF,
	cfmk_DF,
	cfmk_LAST = cfmk_DF,
      };

    private:
      struct float_mode
      {
	float_mode() noexcept;

	float_mode(const types::ext_float_type::kind _mode,
		   const types::real_float_type::format &_format,
		   const mpa::limbs::size_type _size,
		   const mpa::limbs::size_type _alignment) noexcept;

	types::ext_float_type::kind mode;
	std::reference_wrapper<const types::real_float_type::format> format;
	mpa::limbs::size_type size;
	mpa::limbs::size_type alignment;
	bool is_std_float;
      };

    protected:
      static const types::real_float_type::format _ieee_single_format;
      static const types::real_float_type::format _ieee_double_format;
      static const types::real_float_type::format _ieee_quad_format;

      void
      _register_float_mode(const types::ext_float_type::kind mode,
			   const types::real_float_type::format &format,
			   const mpa::limbs::size_type size,
			   const mpa::limbs::size_type alignment,
			   const std::initializer_list<const char *> names);

      void
      _register_float_mode(const common_float_mode_kind cfmk,
			   const types::real_float_type::format &format,
			   const mpa::limbs::size_type size,
			   const mpa::limbs::size_type alignment,
			   const std::initializer_list<const char *> names);

      void _set_std_float_mode(const types::std_float_type::kind std_float_kind,
			       const types::ext_float_type::kind mode);
      void _set_std_float_mode(const types::std_float_type::kind std_float_kind,
			       const common_float_mode_kind cfmk);

      void _set_float_n_mode(const unsigned int n, const bool extended,
			     const types::ext_float_type::kind mode);
      void _set_float_n_mode(const unsigned int n, const bool extended,
			     const common_float_mode_kind cfmk);

      void _register_ext_float_type_specifier
			(const char * const name,
			 const types::ext_float_type::kind mode);

    private:
      void _register_float_modes();
      virtual void _arch_register_float_modes() = 0;

      std::shared_ptr<const types::real_float_type>
      _float_mode_to_type(const types::ext_float_type::kind mode,
			  const types::qualifiers &qs = types::qualifiers{})
	const;

      const float_mode&
      _std_float_kind_to_float_mode
		(const types::std_float_type::kind std_float_kind)
	const noexcept;

      const types::real_float_type::format&
      _float_mode_to_format(const types::ext_float_type::kind mode) const;

      mpa::limbs::size_type
      _float_mode_to_size(const types::ext_float_type::kind mode) const;

      mpa::limbs::size_type
      _float_mode_to_alignment(const types::ext_float_type::kind mode) const;

      virtual types::ext_int_type::kind _get_int_max_mode() const noexcept = 0;

      virtual types::ext_int_type::kind _get_pointer_mode() const noexcept = 0;

      virtual types::ext_int_type::kind _get_word_mode() const noexcept = 0;

      virtual types::ext_int_type::kind _get_unwind_word_mode() const noexcept = 0;

      virtual types::ext_int_type::kind _get_wint_mode() const noexcept = 0;
      virtual bool _is_wint_signed() const noexcept = 0;

      virtual types::ext_int_type::kind _get_pid_mode() const noexcept = 0;
      virtual bool _is_pid_signed() const noexcept = 0;

      // Corresponds to GCC's BIGGEST_ALIGNMENT.
      virtual mpa::limbs::size_type _biggest_alignment_bytes_log2()
	const noexcept = 0;

      virtual std::shared_ptr<const types::object_type>
      _create_builtin_va_list_type() const = 0;

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

      std::map<const std::string, const builtin_func::factory>
      _register_builtin_funcs();

    protected:
      void _register_builtin_typedef
		(const std::string &name,
		 const builtin_typedef::factory::create_type &factory);

    private:
      void _register_builtin_typedefs();
      virtual void _arch_register_builtin_typedefs() = 0;

      const gcc_cmdline_parser::gcc_version _gcc_version;

      opts_common _opts_common;
      opts_c_family _opts_c_family;

      std::vector<int_mode> _int_modes;
      std::map<std::string, types::ext_int_type::kind> _int_mode_names;
      std::vector<types::ext_int_type::kind> _std_int_modes;
      std::vector<types::ext_int_type::kind> _int_modes_sorted_by_width;

      std::vector<float_mode> _float_modes;
      std::map<std::string, types::ext_float_type::kind> _float_mode_names;
      std::vector<types::ext_float_type::kind> _std_float_modes;
      std::vector<std::tuple<unsigned int, bool, types::ext_float_type::kind>>
	_float_n_modes;
      std::vector<std::pair<const char *, types::ext_float_type::kind>>
	_ext_float_type_specifiers;

    private:
      std::map<const std::string, const builtin_func::factory> _builtin_funcs;
      builtin_typedef::factories _builtin_typedefs;
    };
  }
}

#endif
