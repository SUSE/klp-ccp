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

#include <cstring>
#include "cmdline_except.hh"
#include "pp_except.hh"
#include "preprocessor.hh"
#include "ast.hh"
#include "ast_impl.hh"
#include "code_remark.hh"
#include "semantic_except.hh"
#include "constexpr_value.hh"
#include "builtins_impl.hh"
#include "target_gcc.hh"

using namespace klp::ccp;

enum opt_code_common
{
  opt_code_common_unused = 0,

  opt_code_common_O,
  opt_code_common_Ofast,
  opt_code_common_Og,
  opt_code_common_Os,

  opt_code_common_fexpensive_optimizations,

  opt_code_common_fassociative_math,
  opt_code_common_fexcess_precision,
  opt_code_common_ffinite_math_only,
  opt_code_common_fmath_errno,
  opt_code_common_freciprocal_math,
  opt_code_common_fsignaling_nans,
  opt_code_common_fsigned_zeros,
  opt_code_common_ftrapping_math,

  opt_code_common_funsafe_math_optimizations,
  opt_code_common_ffast_math,

  opt_code_common_ffp_contract,
  opt_code_common_fpermitted_flt_eval_methods,
  opt_code_common_fsingle_precision_constant,

  opt_code_common_fabi_version,
  opt_code_common_fleading_underscore,
  opt_code_common_finline,

  opt_code_common_fPIC,
  opt_code_common_fPIE,
  opt_code_common_fpic,
  opt_code_common_fpie,

  opt_code_common_fomit_frame_pointer,
  opt_code_common_fexceptions,
  opt_code_common_fnon_call_exceptions,
  opt_code_common_funwind_tables,
  opt_code_common_fasynchronous_unwind_tables,

  opt_code_common_fstack_protector,
  opt_code_common_fstack_protector_all,
  opt_code_common_fstack_protector_explicit,
  opt_code_common_fstack_protector_strong,

  opt_code_common_fsanitize,
  opt_code_common_fsanitize_address,
  opt_code_common_fsanitize_thread,
};

static gcc_cmdline_parser::option gcc_opt_table_common[] = {
	#include "gcc_cmdline_opts_common.cc"
	{ nullptr }
};

enum opt_code_c_family
{
  opt_code_c_family_unused = 0,

  opt_code_c_family_D,
  opt_code_c_family_I,
  opt_code_c_family_U,
  opt_code_c_family_idirafter,
  opt_code_c_family_include,
  opt_code_c_family_iquote,
  opt_code_c_family_isystem,
  opt_code_c_family_undef,
  opt_code_c_family_fgnu89_inline,
  opt_code_c_family_fsigned_char,
  opt_code_c_family_funsigned_char,
  opt_code_c_family_fopenacc,
  opt_code_c_family_fopenmp,
  opt_code_c_family_fhosted,
  opt_code_c_family_ffreestanding,

  opt_code_c_family_ansi,

  opt_code_c_family_std_c90,
  opt_code_c_family_std_iso9899_199409,
  opt_code_c_family_std_c99,
  opt_code_c_family_std_c11,
  opt_code_c_family_std_c17,
  opt_code_c_family_std_c2x,

  opt_code_c_family_std_gnu90,
  opt_code_c_family_std_gnu99,
  opt_code_c_family_std_gnu11,
  opt_code_c_family_std_gnu17,
  opt_code_c_family_std_gnu2x,

  opt_code_c_family_std_cxx98,
  opt_code_c_family_std_cxx11,
  opt_code_c_family_std_cxx14,
  opt_code_c_family_std_cxx17,
  opt_code_c_family_std_cxx1y,
  opt_code_c_family_std_cxx1z,
  opt_code_c_family_std_cxx2a,

  opt_code_c_family_std_gnuxx98,
  opt_code_c_family_std_gnuxx11,
  opt_code_c_family_std_gnuxx14,
  opt_code_c_family_std_gnuxx17,
  opt_code_c_family_std_gnuxx1y,
  opt_code_c_family_std_gnuxx1z,
  opt_code_c_family_std_gnuxx2a,
};

static gcc_cmdline_parser::option gcc_opt_table_c_family[] = {
	#include "gcc_cmdline_opts_c_family.cc"
	{ nullptr }
};


target_gcc::target_gcc(const char * const version)
  : _gcc_version(_parse_version(version)),
    _opts_common(_gcc_version), _opts_c_family(),
    _builtin_funcs(_register_builtin_funcs())
{}

target_gcc::~target_gcc() noexcept = default;

void target_gcc::parse_command_line
		(int argc, const char *argv[],
		 header_resolver &hr,
		 preprocessor &pp,
		 const std::function<void(const std::string&)> &report_warning)
{
  _init_options_struct();
  _c_lang_init_options_struct();
  _c_lang_init_options();
  _decode_options(argc, argv, report_warning);
  _process_options();

  for (const auto dir : _opts_c_family.include_dirs_quoted)
    hr.append_search_dir_quoted(dir);

  for (const auto dir : _opts_c_family.include_dirs)
    hr.append_search_dir(dir);

  for (const auto dir : _opts_c_family.include_dirs_system)
    hr.append_search_dir(dir);

  for (const auto dir : _opts_c_family.include_dirs_after)
    hr.append_search_dir(dir);

  for (const auto i : _opts_c_family.pre_includes) {
    std::string resolved = hr.resolve(i, _opts_common.base_file,
				      header_resolver::cwd);
    if (resolved.empty()) {
      throw cmdline_except{
	      std::string{"file '"} + i + "' not found"
	    };
    }

    pp.add_root_source(resolved, true);
  }

  pp.add_root_source(_opts_common.base_file, false);
  pp.set_base_file(_opts_common.base_file);

  pp.register_builtin_macro("__STDC__", "1");
  pp.register_builtin_macro("__STDC_HOSTED__", "1");
  pp.register_builtin_macro("__STDC_VERSION__", "199901L");

  if (!_opts_c_family.flag_undef) {
    _register_builtin_macros(pp);
    if (_opts_common.optimize)
      pp.register_builtin_macro("__OPTIMIZE__", "1");
    if (_opts_common.optimize_size)
      pp.register_builtin_macro("__OPTIMIZE_SIZE__", "1");
  }

  for (const auto &m : _opts_c_family.macro_defs_and_undefs) {
    if (!m.undef) {
      const auto &_report_warning =
	[&](const std::string &msg) {
	  report_warning(std::string{"predefined macro '"} +
			 m.arg + "': " + msg);
	};

      const std::string::size_type repl_pos = m.arg.find('=');
      if (repl_pos == std::string::npos) {
	try {
	  pp.register_predefined_macro(m.arg, "1", _report_warning);
	} catch (const pp_except &e) {
	  throw cmdline_except{
		  std::string{"failed to parse predefined macro '"} +
		  m.arg + "': " + e.what()
		};
	}

      } else {
	const std::string signature{m.arg.cbegin(),
				    m.arg.cbegin() + repl_pos};
	const std::string repl{m.arg.cbegin() + repl_pos + 1,
			       m.arg.cend()};
	try {
	  pp.register_predefined_macro(signature, repl, _report_warning);
	} catch (const pp_except &e) {
	  throw cmdline_except{
		  std::string{"failed to parse predefined macro '"} +
		  m.arg + "': " + e.what()
		};
	}
      }

    } else {
      const auto &_report_warning =
	[&](const std::string &msg) {
	  report_warning(std::string{"macro undef '"} +
			 m.arg + "': " + msg);
	};

      try {
	pp.register_predefined_macro_undef(m.arg, _report_warning);
      } catch (const pp_except &e) {
	throw cmdline_except{
		std::string{"failed to parse macro undef '"} +
		m.arg + "': " + e.what()
	      };
      }
    }
  }

  _register_int_modes();
}


const builtin_func::factory*
target_gcc::lookup_builtin_func(const std::string &id) const noexcept
{
  auto it = _builtin_funcs.find(id);
  if (it == _builtin_funcs.end())
    return nullptr;

  return &it->second;
}


struct target_gcc::_impl_proxy
{
  constexpr _impl_proxy(const target_gcc &tgt) noexcept
  : _tgt(tgt), _int_mode_names(tgt._int_mode_names)
  {}

  using int_mode_kind = target_gcc::int_mode_kind;
  using float_mode_kind = target_gcc::float_mode_kind;

  std::shared_ptr<const types::int_type>
  _int_mode_to_type(const types::ext_int_type::kind mode,
		    const bool is_signed,
		    const types::qualifiers &qs = types::qualifiers{}) const
  {
    return _tgt._int_mode_to_type(mode, is_signed, qs);
  }

  std::shared_ptr<const types::int_type>
  _int_mode_to_type(const int_mode_kind imk,
		    const bool is_signed,
		    const types::qualifiers &qs = types::qualifiers{}) const
  {
    return _tgt._int_mode_to_type(imk, is_signed, qs);
  }

  types::std_float_type::kind
  _float_mode_to_float_kind(const float_mode_kind m) const noexcept
  {
    return _tgt._float_mode_to_float_kind(m);
  }

  types::ext_int_type::kind _get_pointer_mode() const noexcept
  {
    return _tgt._get_pointer_mode();
  }

  types::ext_int_type::kind _get_word_mode() const noexcept
  {
    return _tgt._get_word_mode();
  }

  types::ext_int_type::kind _get_wint_mode() const noexcept
  {
    return _tgt._get_wint_mode();
  }

  bool _is_wint_signed() const noexcept
  {
    return _tgt._is_wint_signed();
  }

  types::ext_int_type::kind _get_pid_mode() const noexcept
  {
    return _tgt._get_pid_mode();
  }

  bool _is_pid_signed() const noexcept
  {
    return _tgt._is_pid_signed();
  }

  const decltype(std::declval<target_gcc>()._int_mode_names) &_int_mode_names;

private:
  const target_gcc &_tgt;
};

using _impl_proxy = target_gcc::_impl_proxy;
using int_mode_kind = _impl_proxy::int_mode_kind;
using float_mode_kind = _impl_proxy::float_mode_kind;


namespace
{
  class _aligned_attribute_finder
  {
  public:
    _aligned_attribute_finder(ast::ast &a,
			      const std::function<void(ast::expr&)> &_eval_expr,
			      const target_gcc &tgt, const bool choose_max)
      noexcept;

    bool operator()(ast::attribute &attr);

    types::alignment grab_result();

  private:
    types::alignment _result;
    ast::ast &_a;
    const std::function<void(ast::expr&)> &_eval_expr;
    const target_gcc &_tgt;
    const bool _choose_max;
  };
}

_aligned_attribute_finder::
_aligned_attribute_finder(klp::ccp::ast::ast &a,
			const std::function<void(ast::expr&)> &eval_expr,
			const target_gcc &tgt,
			const bool choose_max) noexcept
  : _a(a), _tgt(tgt), _eval_expr(eval_expr), _choose_max(choose_max)
{}

bool _aligned_attribute_finder::operator()(ast::attribute &attr)
{
  if (_a.get_pp_tokens()[attr.get_name_tok()].get_value() != "aligned")
    return true;

  ast::expr_list *params = attr.get_params();
  mpa::limbs::size_type log2_value = 0;
  if (!params) {
    // An single 'aligned' attribute w/o any parameters
    // means the target's "biggest alignment".
    log2_value = _tgt.get_biggest_alignment_log2();

  } else {
    if (!params || params->size() != 1) {
      code_remark remark
	(code_remark::severity::fatal,
	 "wrong number of parameters for 'aligned' attribute",
	 _a.get_pp_result(), attr.get_tokens_range());
      _a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    ast::expr &e = (*params)[0];
    if (!e.is_evaluated())
      _eval_expr(e);
    if (!e.is_evaluated()) {
      code_remark remark
	(code_remark::severity::fatal,
	 "failed to evaluate 'aligned' attribute's parameter",
	 _a.get_pp_result(), attr.get_tokens_range());
      _a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    if (!e.is_constexpr()) {
      code_remark remark
	(code_remark::severity::fatal,
	 "'aligned' attribute's parameter is not a constant expression",
	 _a.get_pp_result(), attr.get_tokens_range());
      _a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    const ast::constexpr_value &cv = e.get_constexpr_value();
    if (!(cv.has_constness
	  (ast::constexpr_value::constness::c_integer_constant_expr))) {
      code_remark remark
	(code_remark::severity::fatal,
	 "'aligned' attribute's parameter is not an integer constant",
	 _a.get_pp_result(), attr.get_tokens_range());
      _a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    assert(cv.get_value_kind() == ast::constexpr_value::value_kind::vk_int);
    const target_int &ti = cv.get_int_value();
    if (ti.is_negative()) {
      code_remark remark(code_remark::severity::fatal,
			 "'aligned' attribute's parameter is negative",
			 _a.get_pp_result(), attr.get_tokens_range());
      _a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    const mpa::limbs &ls = ti.get_limbs();
    const mpa::limbs::size_type ls_fls = ls.fls();
    if (!ls_fls || ls.is_any_set_below(ls_fls - 1)) {
      code_remark remark(code_remark::severity::fatal,
			 "'aligned' attribute value is not a power of two",
			 _a.get_pp_result(), attr.get_tokens_range());
      _a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    log2_value = ls_fls - 1;
  }

  // In order to choose between multiple 'aligned' attributes, either
  // take the maximum or the last one encountered, depending on the
  // context.
  if (!_result.is_set() || !_choose_max ||
      _result.get_log2_value() < log2_value) {
    _result = log2_value;
  }

  return true;
}

types::alignment _aligned_attribute_finder::grab_result()
{
  return std::move(_result);
}


namespace
{
  class _packed_attribute_finder
  {
  public:
    _packed_attribute_finder(ast::ast &a) noexcept;

    bool operator()(const ast::attribute &attr);

    bool get_result() const noexcept
    { return _has_packed_attribute; }

  private:
    ast::ast &_a;
    bool _has_packed_attribute;
  };
}

_packed_attribute_finder::_packed_attribute_finder(ast::ast &a) noexcept
  : _a(a), _has_packed_attribute(false)
{}

bool _packed_attribute_finder::operator()(const ast::attribute &attr)
{
  if (_a.get_pp_tokens()[attr.get_name_tok()].get_value() != "packed")
    return true;

  const ast::expr_list *params = attr.get_params();
  if (params) {
    code_remark remark(code_remark::severity::fatal,
		       "unexpected parameters to 'packed' attribute",
		       _a.get_pp_result(), attr.get_tokens_range());
    _a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _has_packed_attribute = true;
  return false;
}


namespace
{
  class _mode_attribute_finder
  {
  public:
    _mode_attribute_finder(ast::ast &a, const target_gcc &tgt)
      noexcept;

    bool operator()(const ast::attribute &attr);

    const types::ext_int_type::kind* get_int_mode_result() const noexcept
    { return _im_set ? &_im : nullptr; }

    const float_mode_kind* get_float_mode_result() const noexcept
    { return _fmk_set ? &_fmk : nullptr; }

    pp_token_index get_mode_tok() const noexcept
    { return _mode_tok; }

    std::shared_ptr<const types::pointer_type>
    apply_to_type(std::shared_ptr<const types::pointer_type> &&orig_t) const;

    std::shared_ptr<const types::int_type>
    apply_to_type(std::shared_ptr<const types::int_type> &&orig_t)
      const;

    std::shared_ptr<const types::addressable_type>
    apply_to_type(std::shared_ptr<const types::addressable_type> &&orig_t)
      const;

    bool mode_attribute_found() const noexcept
    {
      return _im_set || _fmk_set;
    }

  private:
    ast::ast &_a;
    const target_gcc &_tgt;
    types::ext_int_type::kind _im;
    bool _im_set;
    float_mode_kind _fmk;
    bool _fmk_set;
    pp_token_index _mode_tok;
  };
}

_mode_attribute_finder::
_mode_attribute_finder(ast::ast &a, const target_gcc &tgt) noexcept
  : _a(a), _tgt(tgt), _im_set(false), _fmk_set(false)
{}

bool _mode_attribute_finder::operator()(const ast::attribute &attr)
{
  const std::string &attr_name =
    _a.get_pp_tokens()[attr.get_name_tok()].get_value();
  if (attr_name != "mode" && attr_name != "__mode__")
    return true;

  const ast::expr_list *params = attr.get_params();
  if (!params || params->size() != 1) {
    code_remark remark(code_remark::severity::fatal,
		       "wrong number of parameters for 'mode' attribute",
		       _a.get_pp_result(), attr.get_tokens_range());
    _a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const ast::expr_id *e_id = nullptr;
  (*params)[0].process<void, type_set<ast::expr_id>>
    (wrap_callables<default_action_nop>
     ([&](const ast::expr_id &_e_id) {
	e_id = &_e_id;
      }));
  if (!e_id) {
    code_remark remark(code_remark::severity::fatal,
		       "invalid expression for 'mode' attribute",
		       _a.get_pp_result(), (*params)[0].get_tokens_range());
    _a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const std::string &id = _a.get_pp_tokens()[e_id->get_id_tok()].get_value();
  const _impl_proxy impl_proxy{_tgt};
  const auto it_int_mode = impl_proxy._int_mode_names.find(id);
  if (it_int_mode != impl_proxy._int_mode_names.cend()) {
    _im = it_int_mode->second;
    _im_set = true;
  } else if (id == "word" || id == "__word__") {
    _im = impl_proxy._get_word_mode();
    _im_set = true;
  } else if (id == "pointer" || id == "__pointer__") {
    _im = impl_proxy._get_pointer_mode();
    _im_set = true;
  } else if (id == "SF" || id == "__SF__") {
    _fmk = float_mode_kind::fmk_SF;
    _fmk_set = true;
  } else if (id == "DF" || id == "__DF__") {
    _fmk = float_mode_kind::fmk_DF;
    _fmk_set = true;
  } else {
    code_remark remark(code_remark::severity::fatal,
		       "unrecognized 'mode' attribute specifier",
		       _a.get_pp_result(), e_id->get_tokens_range());
    _a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (_im_set && _fmk_set) {
    code_remark remark(code_remark::severity::fatal,
		       "inconsistent 'mode' attribute specifier domains",
		       _a.get_pp_result(), e_id->get_tokens_range());
    _a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _mode_tok = e_id->get_tokens_range().begin;

  return true;
}

std::shared_ptr<const types::pointer_type> _mode_attribute_finder::
apply_to_type(std::shared_ptr<const types::pointer_type> &&orig_t) const
{
  if (!this->mode_attribute_found())
    return std::move(orig_t);

  if (!_im_set || _im != _impl_proxy{_tgt}._get_pointer_mode()) {
    code_remark remark
      (code_remark::severity::fatal,
       "invalid 'mode' attribute specifier for pointer type",
       _a.get_pp_result(), _mode_tok);
    _a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  return std::move(orig_t);
}

std::shared_ptr<const types::int_type> _mode_attribute_finder::
apply_to_type(std::shared_ptr<const types::int_type> &&orig_t) const
{
  if (!this->mode_attribute_found())
    return std::move(orig_t);

  if (!_im_set) {
    code_remark remark
      (code_remark::severity::fatal,
       "invalid 'mode' attribute specifier for int type",
       _a.get_pp_result(), _mode_tok);
    _a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (!orig_t->is_complete()) {
    code_remark remark
      (code_remark::severity::fatal,
       "applying 'mode' attribute specifier to incomplete integer type",
       _a.get_pp_result(), _mode_tok);
    _a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  return _impl_proxy{_tgt}._int_mode_to_type(_im, orig_t->is_signed(_tgt),
					     orig_t->get_qualifiers());
}

std::shared_ptr<const types::addressable_type> _mode_attribute_finder::
apply_to_type(std::shared_ptr<const types::addressable_type> &&orig_t) const
{
  if (!this->mode_attribute_found())
    return std::move(orig_t);

  return types::handle_types<std::shared_ptr<const types::addressable_type>>
    ((wrap_callables<no_default_action>
      ([&](std::shared_ptr<const types::int_type> &&it)
		-> std::shared_ptr<const types::addressable_type> {
	return apply_to_type(std::move(it));
       },
       [&](const std::shared_ptr<const types::real_float_type> &rft) {
	 if (!_fmk_set) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "invalid 'mode' attribute specifier for float type",
	      _a.get_pp_result(), _mode_tok);
	   _a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }

	 return (types::std_float_type::create
		 (_impl_proxy{_tgt}._float_mode_to_float_kind(_fmk),
		  rft->get_qualifiers()));
       },
       [&](std::shared_ptr<const types::pointer_type> &&pt)
		-> std::shared_ptr<const types::addressable_type> {
	 return apply_to_type(std::move(pt));
       },
       [&](const std::shared_ptr<const types::type>&)
		-> std::shared_ptr<const types::addressable_type> {
	 code_remark remark
	   (code_remark::severity::fatal,
	    "'mode' attribute specifier not applicable to type",
	    _a.get_pp_result(), _mode_tok);
	 _a.get_remarks().add(remark);
	 throw semantic_except(remark);
       })),
     std::move(orig_t));
}


std::shared_ptr<const types::pointer_type> target_gcc::
evaluate_attributes(ast::ast &a,
		    const std::function<void(ast::expr&)> &eval_expr,
		    std::shared_ptr<const types::pointer_type> &&t,
		    ast::type_qualifier_list &tql) const
{
  // Process a pointer derivation's attributes, e.g.
  // void * __attribute__((aligned(16))) foo;
  _mode_attribute_finder maf(a, *this);
  _aligned_attribute_finder aaf(a, eval_expr, *this, false);
  tql.for_each_attribute(maf);
  tql.for_each_attribute(aaf);

  t = maf.apply_to_type(std::move(t));
  types::alignment align = aaf.grab_result();
  if (align.is_set())
    t = t->set_user_alignment(std::move(align));

  return std::move(t);
}

std::shared_ptr<const types::addressable_type> target_gcc::
evaluate_attributes(ast::ast &a,
		    const std::function<void(ast::expr&)> &eval_expr,
		    std::shared_ptr<const types::addressable_type> &&t,
		    ast::attribute_specifier_list * const asl) const
{
  // Process a direct_declarator_parenthesized's
  // resp. abstract_declarator_parenthesized's attributes, e.g.
  // int (__attribute__((aligned(16))) a);
  // Behaviour is the same as for pointers, but the more generic
  // addressable_type is returned.
  if (!asl)
    return std::move(t);

  _mode_attribute_finder maf(a, *this);
  _aligned_attribute_finder aaf(a, eval_expr, *this, false);
  asl->for_each_attribute(maf);
  asl->for_each_attribute(aaf);

  t = maf.apply_to_type(std::move(t));
  types::alignment align = aaf.grab_result();
  if (align.is_set())
    t = t->set_user_alignment(std::move(align));

  return std::move(t);
}

std::shared_ptr<const types::addressable_type> target_gcc::
evaluate_attributes(ast::ast &a,
		    const std::function<void(ast::expr&)> &eval_expr,
		    std::shared_ptr<const types::addressable_type> &&t,
		    ast::declaration_specifiers &ds,
		    ast::attribute_specifier_list * const asl) const
{
  // Process attributes in a parameter_declaration. Attributes of inner
  // declarator derivations have already been handled.
  // GCC doesn't permit direct application of 'aligned' attributes to
  // function parameters.
  auto &&check_for_aligned_attr =
    [&](const ast::attribute &attr) {
      if (a.get_pp_tokens()[attr.get_name_tok()].get_value() == "aligned") {
	code_remark remark
	(code_remark::severity::fatal,
	 "'aligned' attribute may not be specified for function parameter",
	 a.get_pp_result(), attr.get_tokens_range());
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }
      return true;
    };

  _mode_attribute_finder maf(a, *this);
  if (asl) {
    asl->for_each_attribute(maf);
    asl->for_each_attribute(check_for_aligned_attr);
  }
  ds.for_each_attribute(maf);
  ds.for_each_attribute(check_for_aligned_attr);

  t = maf.apply_to_type(std::move(t));
  return std::move(t);
}

std::shared_ptr<const types::addressable_type> target_gcc::
evaluate_attributes(ast::ast &a,
		    const std::function<void(ast::expr&)> &eval_expr,
		    std::shared_ptr<const types::addressable_type> &&t,
		    ast::specifier_qualifier_list &sql) const
{
  // Process a type name's attributes, i.e. thos from the specifier
  // qualifier list. Attributes of inner declarator derivations have
  // already been handled.
  _mode_attribute_finder maf(a, *this);
  _aligned_attribute_finder aaf(a, eval_expr, *this, true);
  sql.for_each_attribute(maf);
  sql.for_each_attribute(aaf);

  t = maf.apply_to_type(std::move(t));
  types::alignment align = aaf.grab_result();
  if (align.is_set())
    t = t->set_user_alignment(std::move(align));

  return std::move(t);
}

std::shared_ptr<const types::addressable_type> target_gcc::
evaluate_attributes(ast::ast &a,
		    const std::function<void(ast::expr&)> &eval_expr,
		    std::shared_ptr<const types::addressable_type> &&t,
		    ast::declaration_specifiers &ds,
		    ast::attribute_specifier_list * const asl_before,
		    ast::attribute_specifier_list * const asl_middle,
		    ast::attribute_specifier_list * const asl_after) const
{
  // Process an init_declarator's attributes, including the ones from
  // the enclosing declaration. Attributes of inner declarator
  // derivations have already been handled.

  // Check for declaration of a function.
  const bool is_fun = types::is_type<types::function_type>(*t);
  if (is_fun) {
    // Don't apply 'aligned' and 'mode' attributes to function types.
    return std::move(t);
  }

  _mode_attribute_finder maf(a, *this);
  _aligned_attribute_finder aaf(a, eval_expr, *this, true);
  if (asl_after) {
    asl_after->for_each_attribute(maf);
    asl_after->for_each_attribute(aaf);
  }
  if (asl_middle) {
    asl_middle->for_each_attribute(maf);
    asl_middle->for_each_attribute(aaf);
  }
  if (asl_before) {
    asl_before->for_each_attribute(maf);
    asl_before->for_each_attribute(aaf);
  }
  ds.for_each_attribute(maf);
  ds.for_each_attribute(aaf);

  t = maf.apply_to_type(std::move(t));
  types::alignment align = aaf.grab_result();
  if (align.is_set())
    t = t->set_user_alignment(std::move(align));

  return std::move(t);
}

std::shared_ptr<const types::addressable_type> target_gcc::
evaluate_attributes(ast::ast &a,
		    const std::function<void(ast::expr&)> &eval_expr,
		    std::shared_ptr<const types::addressable_type> &&t,
		    ast::attribute_specifier_list * const soud_asl_before,
		    ast::attribute_specifier_list * const soud_asl_after,
		    ast::specifier_qualifier_list &sql,
		    ast::attribute_specifier_list * const asl_before,
		    ast::attribute_specifier_list * const asl_after) const
{
  // Process a non-bitfield struct declarator's attributes, including
  // the ones from the enclosing declaration. Attributes of inner
  // declarator derivations have already been handled.

  // First, search for 'packed' attribute in surrounding struct/union
  // definition: these are to be applied to each member individually.
  bool packed = false;
  if (soud_asl_before || soud_asl_after) {
    _packed_attribute_finder paf(a);
    if (soud_asl_before)
      soud_asl_before->for_each_attribute(paf);
    if (soud_asl_after)
      soud_asl_after->for_each_attribute(paf);
    packed = paf.get_result();
  }

  // Next process the struct declarator's attributes, including the
  // ones from the enclosing declaration.
  _mode_attribute_finder maf(a, *this);
  _aligned_attribute_finder aaf(a, eval_expr, *this, true);
  _packed_attribute_finder paf(a);
  if (asl_after) {
    asl_after->for_each_attribute(maf);
    asl_after->for_each_attribute(aaf);
    asl_after->for_each_attribute(paf);
  }
  if (asl_before) {
    asl_before->for_each_attribute(maf);
    asl_before->for_each_attribute(aaf);
    asl_before->for_each_attribute(paf);
  }
  sql.for_each_attribute(maf);
  sql.for_each_attribute(aaf);
  sql.for_each_attribute(paf);

  t = maf.apply_to_type(std::move(t));

  packed |= paf.get_result();
  types::alignment align = aaf.grab_result();
  if (packed) {
    if (align.is_set())
      t = t->set_user_alignment(std::move(align));
    else
      t = t->set_user_alignment(types::alignment(0));

  } else if (align.is_set()) {
    if (t->get_effective_alignment(*this) <= align.get_log2_value())
      t = t->set_user_alignment(std::move(align));
  }

  return std::move(t);
}

std::shared_ptr<const types::bitfield_type> target_gcc::
evaluate_attributes(ast::ast &a,
		    const std::function<void(ast::expr&)> &eval_expr,
		    std::shared_ptr<const types::bitfield_type> &&t,
		    ast::attribute_specifier_list * const soud_asl_before,
		    ast::attribute_specifier_list * const soud_asl_after,
		    ast::specifier_qualifier_list &sql,
		    ast::attribute_specifier_list * const asl_before,
		    ast::attribute_specifier_list * const asl_after) const
{
  // Process a bitfield struct declarator's attributes, including the
  // ones from the enclosing declaration. Attributes of inner
  // declarator derivations have already been handled.

  // First, search for 'packed' attribute in surrounding struct/union
  // definition: these are to be applied to each member individually.
  bool packed = false;
  if (soud_asl_before || soud_asl_after) {
    _packed_attribute_finder paf(a);
    if (soud_asl_before)
      soud_asl_before->for_each_attribute(paf);
    if (soud_asl_after)
      soud_asl_after->for_each_attribute(paf);
    packed = paf.get_result();
  }

  // Next process the struct declarator's attributes, including the
  // ones from the enclosing declaration.
  // For bitfields, GCC does apply the mode attribute to the
  // underlying integer type, but only after the width has been set
  // (and verified).
  _mode_attribute_finder maf(a, *this);
  _aligned_attribute_finder aaf(a, eval_expr, *this, true);
  _packed_attribute_finder paf(a);
  if (asl_after) {
    asl_after->for_each_attribute(maf);
    asl_after->for_each_attribute(aaf);
    asl_after->for_each_attribute(paf);
  }
  if (asl_before) {
    asl_before->for_each_attribute(maf);
    asl_before->for_each_attribute(aaf);
    asl_before->for_each_attribute(paf);
  }
  sql.for_each_attribute(maf);
  sql.for_each_attribute(aaf);
  sql.for_each_attribute(paf);

  if (maf.mode_attribute_found()) {
    std::shared_ptr<const types::int_type> base_t =
      t->get_base_type();
    base_t = maf.apply_to_type(std::move(base_t));
    t = types::bitfield_type::create(std::move(base_t), t->get_width(*this));
  }

  packed |= paf.get_result();
  types::alignment align = aaf.grab_result();
  if (packed)
    t = t->set_packed();
  if (align.is_set())
    t = t->set_user_alignment(std::move(align));

  return std::move(t);
}


bool target_gcc::is_char_signed() const noexcept
{
  return _opts_c_family.flag_signed_char;
}

mpa::limbs::size_type
target_gcc::get_std_int_width(const types::std_int_type::kind k) const noexcept
{
  const int_mode &im = _std_int_kind_to_int_mode(k);

  return im.width;
}

mpa::limbs target_gcc::get_std_int_size(const types::std_int_type::kind k)
  const
{
  const int_mode &im = _std_int_kind_to_int_mode(k);

  return mpa::limbs::from_size_type(im.size);
}

mpa::limbs::size_type
target_gcc::get_std_int_alignment(const types::std_int_type::kind k)
  const noexcept
{
  const int_mode &im = _std_int_kind_to_int_mode(k);

  return im.alignment;
}

mpa::limbs::size_type
target_gcc::get_ext_int_width(const types::ext_int_type::kind k)
  const noexcept
{
  const int m = static_cast<int>(k);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return im.width;
}

mpa::limbs target_gcc::get_ext_int_size(const types::ext_int_type::kind k)
	const
{
  const int m = static_cast<int>(k);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return mpa::limbs::from_size_type(im.size);
}

mpa::limbs::size_type
target_gcc::get_ext_int_alignment(const types::ext_int_type::kind k)
  const noexcept
{
  const int m = static_cast<int>(k);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return im.alignment;
}

target::ext_int_keywords target_gcc::get_ext_int_keywords() const
{
  ext_int_keywords kws;

  for (const auto &im : _int_modes) {
    if (!im.enabled || !im.create_int_n_type_specifier)
      continue;

    kws.emplace("__int" + std::to_string(im.width), im.mode);
  }

  return kws;
}

std::shared_ptr<const types::int_type>
target_gcc::width_to_int_type(const mpa::limbs::size_type w,
			      const bool is_signed,
			      const bool std_int_required) const
{
  auto it_int_mode =
    std::lower_bound(_int_modes_sorted_by_width.cbegin(),
		     _int_modes_sorted_by_width.cend(),
		     w,
		     [this](const types::ext_int_type::kind mode,
			    const mpa::limbs::size_type width) {
		       const int m = static_cast<int>(mode);
		       assert(m < _int_modes.size());
		       return _int_modes[m].width < width;
		     });

  for (; it_int_mode != _int_modes_sorted_by_width.cend(); ++it_int_mode) {
    const int m = static_cast<int>(*it_int_mode);
    assert(m < _int_modes.size());
    const int_mode &im = _int_modes[m];

    if (!im.enabled || (std_int_required && !im.is_std_int))
      continue;

    return _int_mode_to_type(*it_int_mode, is_signed);
  }

  return nullptr;
}

std::shared_ptr<const types::int_type>
target_gcc::create_int_max_type(const bool is_signed) const
{
  const types::ext_int_type::kind mode = this->_get_pointer_mode();

  return _int_mode_to_type(mode, is_signed);
}

std::shared_ptr<const types::int_type>
target_gcc::create_ptrdiff_type(const bool is_signed) const
{
  const types::ext_int_type::kind mode = this->_get_pointer_mode();

  return _int_mode_to_type(mode, is_signed);
}


mpa::limbs::size_type target_gcc::get_ptrdiff_width() const noexcept
{
  const types::ext_int_type::kind mode = this->_get_pointer_mode();
  const int m = static_cast<int>(mode);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return im.width;
}

mpa::limbs target_gcc::get_pointer_size() const
{
  const types::ext_int_type::kind mode = this->_get_pointer_mode();
  const int m = static_cast<int>(mode);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return mpa::limbs::from_size_type(im.size);
}

mpa::limbs::size_type target_gcc::get_pointer_alignment() const noexcept
{
  const types::ext_int_type::kind mode = this->_get_pointer_mode();
  const int m = static_cast<int>(mode);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return im.alignment;
}

void target_gcc::
evaluate_enum_type(ast::ast &a,
		   const std::function<void(ast::expr&)> &eval_expr,
		   ast::attribute_specifier_list * const ed_asl_before,
		   ast::attribute_specifier_list * const ed_asl_after,
		   types::enum_content &ec) const
{
  _mode_attribute_finder maf(a, *this);
  _aligned_attribute_finder aaf(a, eval_expr, *this, true);
  _packed_attribute_finder paf(a);
  if (ed_asl_before) {
    ed_asl_before->for_each_attribute(aaf);
    ed_asl_before->for_each_attribute(paf);
    ed_asl_before->for_each_attribute(maf);
  }
  if (ed_asl_after) {
    ed_asl_after->for_each_attribute(aaf);
    ed_asl_after->for_each_attribute(paf);
    ed_asl_after->for_each_attribute(maf);
  }

  if (maf.get_float_mode_result()) {
    code_remark remark
      (code_remark::severity::fatal,
       "float domain 'mode' attribute at enum definition",
       a.get_pp_result(), maf.get_mode_tok());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  this->_evaluate_enum_type(a, ec, paf.get_result(),
			    maf.get_int_mode_result(), aaf.grab_result());
}

void target_gcc::
layout_struct(ast::ast &a,
	      const std::function<void(ast::expr&)> &eval_expr,
	      ast::attribute_specifier_list * const soud_asl_before,
	      ast::attribute_specifier_list * const soud_asl_after,
	      types::struct_or_union_content &souc) const
{
  _aligned_attribute_finder aaf(a, eval_expr, *this, false);
  if (soud_asl_before)
    soud_asl_before->for_each_attribute(aaf);
  if (soud_asl_after)
    soud_asl_after->for_each_attribute(aaf);

  this->_layout_struct(souc, aaf.grab_result());
}

void target_gcc::
layout_union(ast::ast &a,
	     const std::function<void(ast::expr&)> &eval_expr,
	     ast::attribute_specifier_list * const soud_asl_before,
	     ast::attribute_specifier_list * const soud_asl_after,
	     types::struct_or_union_content &souc) const
{
  _aligned_attribute_finder aaf(a, eval_expr, *this, false);
  if (soud_asl_before)
    soud_asl_before->for_each_attribute(aaf);
  if (soud_asl_after)
    soud_asl_after->for_each_attribute(aaf);

  this->_layout_union(souc, aaf.grab_result());
}


target_gcc::int_mode::int_mode() noexcept
  : mode(types::ext_int_type::kind{-1}),
    width(0), size(0), alignment(0),
    enabled(false), create_int_n_type_specifier(false), is_std_int(false)
{}

target_gcc::int_mode::int_mode(const types::ext_int_type::kind _mode,
			       const mpa::limbs::size_type _width,
			       const mpa::limbs::size_type _size,
			       const mpa::limbs::size_type _alignment,
			       const bool _enabled,
			       const bool _create_int_n_type_specifier) noexcept
  : mode(_mode), width(_width), size(_size), alignment(_alignment),
    enabled(_enabled),
    create_int_n_type_specifier(_create_int_n_type_specifier),
    is_std_int(false)
{}

void
target_gcc::_register_int_mode(const types::ext_int_type::kind mode,
			       const mpa::limbs::size_type width,
			       const mpa::limbs::size_type size,
			       const mpa::limbs::size_type alignment,
			       const std::initializer_list<const char *> names,
			       const bool create_int_n_type_specifier,
			       const bool enabled)
{
  const int m = static_cast<int>(mode);

  if (_int_modes.size() <= m)
    _int_modes.resize(m + 1);

  _int_modes[m] =
    int_mode{
      mode, width, size, alignment, enabled, create_int_n_type_specifier
    };

  const auto it_by_width =
    std::upper_bound(_int_modes_sorted_by_width.begin(),
		     _int_modes_sorted_by_width.end(),
		     width,
		     [this](const mpa::limbs::size_type width,
			    const types::ext_int_type::kind mode) {
		       const int m = static_cast<int>(mode);
		       assert(m < _int_modes.size());
		       return width < _int_modes[m].width;
		     });
  _int_modes_sorted_by_width.insert(it_by_width, mode);

  for (const auto &n : names)
    _int_mode_names.emplace(std::string{n}, mode);
}

void
target_gcc::_register_int_mode(const int_mode_kind imk,
			       const mpa::limbs::size_type width,
			       const mpa::limbs::size_type size,
			       const mpa::limbs::size_type alignment,
			       const std::initializer_list<const char *> names,
			       const bool create_int_n_type_specifier,
			       const bool enabled)
{
  _register_int_mode(types::ext_int_type::kind{static_cast<int>(imk)},
		     width, size, alignment, names,
		     create_int_n_type_specifier, enabled);
}

void target_gcc::_int_mode_enable(const types::ext_int_type::kind mode)
{
  const int m = static_cast<int>(mode);

  assert(m < _int_modes.size());
  _int_modes[m].enabled = true;
}

void target_gcc::_int_mode_enable(const int_mode_kind imk)
{
  _int_mode_enable(types::ext_int_type::kind{static_cast<int>(imk)});
}

void target_gcc::_set_std_int_mode(const types::std_int_type::kind std_int_kind,
				   const types::ext_int_type::kind mode)
{
  const int k = static_cast<int>(std_int_kind);
  const int m = static_cast<int>(mode);

  if (_std_int_modes.size() <= k)
    _std_int_modes.resize(k + 1);

  _std_int_modes[k] = mode;
  assert(m < _int_modes.size());
  assert(_int_modes[m].enabled);
  _int_modes[m].is_std_int = true;
}

void target_gcc::_set_std_int_mode(const types::std_int_type::kind std_int_kind,
				   const int_mode_kind imk)
{
  _set_std_int_mode(std_int_kind,
		    types::ext_int_type::kind{static_cast<int>(imk)});
}

void target_gcc::_register_int_modes()
{
  _register_int_mode(int_mode_kind::imk_QI,
		     8, 1, 0,
		     {"QI", "__QI__", "byte", "__byte__"});
  _register_int_mode(int_mode_kind::imk_HI,
		     16, 2, 1,
		     {"HI", "__HI__"});
  _register_int_mode(int_mode_kind::imk_SI,
		     32, 4, 2,
		     {"SI", "__SI__"});
  _register_int_mode(int_mode_kind::imk_DI,
		     64, 8, 3,
		     {"DI", "__DI__"});
  _register_int_mode(int_mode_kind::imk_TI,
		     128, 16, 4,
		     {"TI", "__TI__"},
		     true, false);

  _arch_register_int_modes();
}

std::shared_ptr<const types::int_type>
target_gcc::_int_mode_to_type(const types::ext_int_type::kind mode,
			      const bool is_signed,
			      const types::qualifiers &qs) const
{
  const int m = static_cast<int>(mode);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  // Prefer std_int_type, if there's a mapping.
  if (im.is_std_int) {
    const auto it =
      std::find_if(_std_int_modes.cbegin(),
		   _std_int_modes.cend(),
		   [mode](const types::ext_int_type::kind std_int_mode) {
		     return std_int_mode == mode;
		   });
    assert(it != _std_int_modes.cend());

    const types::std_int_type::kind k =
      static_cast<types::std_int_type::kind>(it - _std_int_modes.cbegin());
    return types::std_int_type::create(k, is_signed, qs);
  }

  return types::ext_int_type::create(mode, is_signed, qs);
}

std::shared_ptr<const types::int_type>
target_gcc::_int_mode_to_type(const int_mode_kind imk,
			      const bool is_signed,
			      const types::qualifiers &qs) const
{
  return _int_mode_to_type(types::ext_int_type::kind{static_cast<int>(imk)},
			   is_signed, qs);
}

const target_gcc::int_mode& target_gcc::
_std_int_kind_to_int_mode(const types::std_int_type::kind std_int_kind)
  const noexcept
{
  const int k = static_cast<int>(std_int_kind);

  assert(k < _std_int_modes.size());
  const types::ext_int_type::kind mode = _std_int_modes[k];
  const int m = static_cast<int>(mode);
  assert(m < _int_modes.size());
  return _int_modes[m];
}

mpa::limbs::size_type
target_gcc::_int_mode_to_width(const types::ext_int_type::kind mode) const
{
  const int m = static_cast<int>(mode);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return im.width;
}

mpa::limbs::size_type
target_gcc::_int_mode_to_size(const types::ext_int_type::kind mode) const
{
  const int m = static_cast<int>(mode);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return im.size;
}

mpa::limbs::size_type
target_gcc::_int_mode_to_alignment(const types::ext_int_type::kind mode) const
{
  const int m = static_cast<int>(mode);
  assert(m < _int_modes.size());
  const int_mode &im = _int_modes[m];

  return im.alignment;
}


void target_gcc::_init_options_struct() noexcept
{
  // This corresponds to GCC's language agnostic
  // init_options_struct().
  _opts_common.init_options_struct();
  _opts_c_family.init_options_struct(*this);
  this->_arch_option_init_struct();
}

void target_gcc::_c_lang_init_options_struct() noexcept
{
  // This corresponds to GCC's c_common_init_options_struct()
  _opts_common.c_lang_init_options_struct();
  _opts_c_family.c_lang_init_options_struct();
}

void target_gcc::_c_lang_init_options() noexcept
{
  // Thist corresponds to GCC's c_common_init().
  _opts_common.c_lang_init_options_struct();
  _opts_c_family.c_lang_init_options(_gcc_version);
}

void target_gcc::
_decode_options(int argc, const char *argv[],
		const std::function<void(const std::string&)> &report_warning)
{
  gcc_cmdline_parser p{_gcc_version};
  p.register_table(gcc_opt_table_common);
  p.register_table(gcc_opt_table_c_family);
  const gcc_cmdline_parser::option * const opt_table_arch =
    this->_arch_get_opt_table();
  if (opt_table_arch)
    p.register_table(opt_table_arch);

  const gcc_cmdline_parser::decoded_opts_type decoded_opts{p(argc, argv)};

  _default_options_optimization(decoded_opts, p, *opt_table_arch);

  for (const auto &decoded_opt : decoded_opts)
    _handle_opt(decoded_opt.table, decoded_opt.o, decoded_opt.val,
		decoded_opt.negative, false, *opt_table_arch);

  if (_opts_common.base_file.empty()) {
    throw cmdline_except{"no input file"};
  }
  _finish_options();
}

void target_gcc::_default_options_optimization
	(const gcc_cmdline_parser::decoded_opts_type &decoded_opts,
	 const gcc_cmdline_parser &p,
	 const gcc_cmdline_parser::option &opt_table_arch)
{
  // This corresponds to GCC's default_options_optimization().
  // Pre-scan for the optimization options as these imply default
  // settings, c.f. GCC's default_options_optimization().
  for (const auto &decoded_opt : decoded_opts) {
    if (decoded_opt.table == gcc_opt_table_common) {
      switch (decoded_opt.o->code) {
      case opt_code_common_O:
	_opts_common.optimize_size = false;
	_opts_common.optimize_fast = false;
	_opts_common.optimize_debug = false;
	if (!decoded_opt.val) {
	  _opts_common.optimize = 1;
	} else {
	  int _optimize = -1;
	  std::size_t endpos;

	  try {
	    _optimize = std::stoi(std::string{decoded_opt.val}, &endpos);
	  } catch (...) {}

	  if (_optimize < 0 || decoded_opt.val[endpos] != '\0')
	    throw cmdline_except{"invalid argument to -O"};
	  else if (_optimize > 255)
	    _optimize = 255;

	  _opts_common.optimize = static_cast<unsigned int>(_optimize);
	}
	break;

      case opt_code_common_Ofast:
	_opts_common.optimize_size = false;
	_opts_common.optimize = 3;
	_opts_common.optimize_fast = true;
	_opts_common.optimize_debug = false;
	break;

      case opt_code_common_Og:
	_opts_common.optimize_size = false;
	_opts_common.optimize = 1;
	_opts_common.optimize_fast = false;
	_opts_common.optimize_debug = true;;
	break;

      case opt_code_common_Os:
	_opts_common.optimize_size = true;
	_opts_common.optimize = 2;
	_opts_common.optimize_fast = false;
	_opts_common.optimize_debug = false;
	break;
      }
    }
  }

  static const default_option default_options_table[] = {
    {
      default_option::opt_levels_1_plus, "fcombine-stack-adjustments",
      nullptr, false
    },
    { default_option::opt_levels_1_plus, "fcompare-elim", nullptr, false },
    { default_option::opt_levels_1_plus, "fcprop-registers", nullptr, false },
    { default_option::opt_levels_1_plus, "fdefer-pop", nullptr, false },
    { default_option::opt_levels_1_plus, "fforward-propagate", nullptr, false },
    {
      default_option::opt_levels_1_plus, "fguess-branch-probability",
      nullptr, false
    },
    { default_option::opt_levels_1_plus, "fipa-profile", nullptr, false },
    { default_option::opt_levels_1_plus, "fipa-pure-const", nullptr, false },
    { default_option::opt_levels_1_plus, "fipa-reference", nullptr, false },
    {
      default_option::opt_levels_1_plus, "fipa-reference-addressable",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    { default_option::opt_levels_1_plus, "fmerge-constants", nullptr, false },
    {
      default_option::opt_levels_1_plus, "fomit-frame-pointer", nullptr, false,
      .min_gcc_version = {8, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "freorder-blocks", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_1_plus, "freorder-blocks", nullptr, false,
      .min_gcc_version = {6, 1, 0},
    },
    { default_option::opt_levels_1_plus, "fshrink-wrap", nullptr, false },
    { default_option::opt_levels_1_plus, "fsplit-wide-types", nullptr, false },
    {
      default_option::opt_levels_2_plus, "ftree-builtin-call-dce",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_1_plus, "ftree-builtin-call-dce",
      nullptr, false,
      .min_gcc_version = {6, 1, 0},
    },
    { default_option::opt_levels_1_plus, "ftree-ccp", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-ch", nullptr, false },
    {
      default_option::opt_levels_1_plus, "ftree-coalesce-vars", nullptr, false,
      .min_gcc_version = {6, 1, 0},
    },
    { default_option::opt_levels_1_plus, "ftree-copy-prop", nullptr, false },
    {
      default_option::opt_levels_1_plus, "ftree-copyrename", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
    },
    { default_option::opt_levels_1_plus, "ftree-dce", nullptr, false },
    {
      default_option::opt_levels_1_plus, "ftree-dominator-opts", nullptr, false
    },
    { default_option::opt_levels_1_plus, "ftree-dse", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-fre", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-sink", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-slsr", nullptr, false },
    { default_option::opt_levels_1_plus, "ftree-ter", nullptr, false },
    {
      default_option::opt_levels_1_plus_not_debug, "fbranch-count-reg",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_1_plus, "fif-conversion",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 2},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fif-conversion",
      nullptr, false,
      .min_gcc_version = {4, 8, 3},
      .max_gcc_version = {4, 8, 5},
    },
    {
      default_option::opt_levels_1_plus, "fif-conversion",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
      .max_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fif-conversion",
      nullptr, false,
      .min_gcc_version = {4, 9, 1},
    },
    {
      default_option::opt_levels_1_plus, "fif-conversion2",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 2},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fif-conversion2",
      nullptr, false,
      .min_gcc_version = {4, 8, 3},
      .max_gcc_version = {4, 8, 5},
    },
    {
      default_option::opt_levels_1_plus, "fif-conversion2",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
      .max_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fif-conversion2",
      nullptr, false,
      .min_gcc_version = {4, 9, 1},
    },
    {
      default_option::opt_levels_1_plus_not_debug,
      "finline-functions-called-once", nullptr, false
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fmove-loop-invariants",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "fssa-phiopt",
      nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    {
      default_option::opt_levels_1_plus, "ftree-bit-ccp",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 9, 4},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "ftree-bit-ccp",
      nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    {
      default_option::opt_levels_1_plus_not_debug, "ftree-sra",
      nullptr, false
    },
    {
      default_option::opt_levels_1_plus_not_debug, "ftree-pta", nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    { default_option::opt_levels_2_plus, "fcaller-saves", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fcode-hoisting", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    { default_option::opt_levels_2_plus, "fcrossjumping", nullptr, false },
    { default_option::opt_levels_2_plus, "fcse-follow-jumps", nullptr, false },
    { default_option::opt_levels_2_plus, "fdevirtualize", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fdevirtualize-speculatively",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_2_plus, "fexpensive-optimizations",
      nullptr, false
    },
    { default_option::opt_levels_2_plus, "fgcse", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fhoist-adjacent-loads",
      nullptr, false
    },
    { default_option::opt_levels_2_plus, "findirect-inlining", nullptr, false },
    {
      default_option::opt_levels_2_plus, "finline-small-functions",
      nullptr, false
    },
    {
      default_option::opt_levels_2_plus, "fipa-bit-cp", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    { default_option::opt_levels_2_plus, "fipa-cp", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fipa-cp-alignment", nullptr, false,
      .min_gcc_version = {5, 1, 0},
      .max_gcc_version = {6, 5, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus, "fipa-icf", nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "fipa-ra", nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    { default_option::opt_levels_2_plus, "fipa-sra", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fipa-vrp", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "fisolate-erroneous-paths-dereference",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_2_plus, "flra-remat", nullptr, false,
      .min_gcc_version = {5, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "foptimize-sibling-calls",
      nullptr, false
    },
    { default_option::opt_levels_2_plus, "fpartial-inlining", nullptr, false },
    { default_option::opt_levels_2_plus, "fpeephole2", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fregmove", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 5},
    },
    { default_option::opt_levels_2_plus, "freorder-functions", nullptr, false },
    {
      default_option::opt_levels_2_plus, "frerun-cse-after-loop", nullptr, false
    },
    { default_option::opt_levels_2_plus, "fschedule-insns2", nullptr, false },
    { default_option::opt_levels_2_plus, "fstrict-aliasing", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fstrict-overflow", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus, "fstore-merging", nullptr, false,
      .min_gcc_version = {7, 1, std::numeric_limits<unsigned int>::max()},
    },
    { default_option::opt_levels_2_plus, "fthread-jumps", nullptr, false },
    { default_option::opt_levels_2_plus, "ftree-pre", nullptr, false },
    {
      default_option::opt_levels_2_plus, "ftree-switch-conversion",
      nullptr, false
    },
    { default_option::opt_levels_2_plus, "ftree-tail-merge", nullptr, false },
    { default_option::opt_levels_2_plus, "ftree-vrp", nullptr, false },
    {
      default_option::opt_levels_2_plus, "fvect-cost-model=", "cheap", false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_2_plus, "falign-functions",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "falign-functions",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "falign-jumps",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "falign-jumps",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "falign-labels",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "falign-labels",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    {
      default_option::opt_levels_2_plus, "falign-loops",
      nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "falign-loops",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "foptimize-strlen",
      nullptr, false
    },
    {
      default_option::opt_levels_2_plus_speed_only,
      "freorder-blocks-algorithm=", "stc", false,
      .min_gcc_version = {6, 1, 0},
    },
    {
      default_option::opt_levels_2_plus_speed_only, "fschedule-insns",
      nullptr, false
    },
    {
      default_option::opt_levels_3_plus_and_size, "finline-functions",
      nullptr, false
    },
    { default_option::opt_levels_3_plus, "fgcse-after-reload", nullptr, false },
    { default_option::opt_levels_3_plus, "fipa-cp-clone", nullptr, false },
    {
      default_option::opt_levels_3_plus, "floop-interchange", nullptr, false,
      .min_gcc_version = {8, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "floop-unroll-and-jam", nullptr, false,
      .min_gcc_version = {8, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "fpeel-loops", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "fpredictive-commoning", nullptr, false
    },
    {
      default_option::opt_levels_3_plus, "fsplit-loops", nullptr, false,
      .min_gcc_version = {7, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "fsplit-paths", nullptr, false,
      .min_gcc_version = {6, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "ftree-loop-distribute-patterns",
      nullptr, false
    },
    {
      default_option::opt_levels_3_plus, "ftree-loop-distribution",
      nullptr, false,
      .min_gcc_version = {8, 1, 0},
    },
    {
      default_option::opt_levels_3_plus, "ftree-loop-vectorize", nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    { default_option::opt_levels_3_plus, "ftree-partial-pre", nullptr, false },
    {
      default_option::opt_levels_3_plus, "ftree-slp-vectorize", nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_3_plus, "ftree-vectorize", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 5},
    },
    { default_option::opt_levels_3_plus, "funswitch-loops", nullptr, false },
    {
      default_option::opt_levels_3_plus, "fvect-cost-model", nullptr, false,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 5},
    },
    {
      default_option::opt_levels_3_plus, "fvect-cost-model=", "dynamic", false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_3_plus, "fversion-loops-for-strides",
      nullptr, false,
      .min_gcc_version = {9, 1, 0},
    },
    { default_option::opt_levels_fast, "ffast-math", nullptr, false },

    { default_option::opt_levels_none }
  };

  _maybe_default_options(*default_options_table, p, opt_table_arch);
  _maybe_default_options(this->_arch_get_option_optimization_table(), p,
			 opt_table_arch);
}

void target_gcc::
_maybe_default_options(const struct default_option &table,
		       const gcc_cmdline_parser &p,
		       const gcc_cmdline_parser::option &opt_table_arch)
{
  for (const default_option *default_opt = &table;
       default_opt->levels != default_option::opt_levels_none; ++default_opt) {
    if (!(default_opt->min_gcc_version <= _gcc_version &&
	  _gcc_version <= default_opt->max_gcc_version)) {
      continue;
    }

    const gcc_cmdline_parser::option *o;
    const gcc_cmdline_parser::option *t;
    std::tie(o, t) = p.find_option(default_opt->name);
    assert(o);

    const unsigned int level = _opts_common.optimize;
    const bool size = _opts_common.optimize_size;
    const bool fast = _opts_common.optimize_fast;
    const bool debug = _opts_common.optimize_debug;

    bool enabled = false;
    switch (default_opt->levels) {
    case default_option::opt_levels_none:
      assert(0);
      __builtin_unreachable();
      break;

    case default_option::opt_levels_all:
      enabled = true;
      break;

    case default_option::opt_levels_0_only:
      enabled = (level == 0);
      break;

    case default_option::opt_levels_1_plus:
      enabled = (level >= 1);
      break;

    case default_option::opt_levels_1_plus_speed_only:
      enabled = (level >= 1 && !size && !debug);
      break;

    case default_option::opt_levels_1_plus_not_debug:
      enabled = (level >= 1 && !debug);
      break;

    case default_option::opt_levels_2_plus:
      enabled = (level >= 2);
      break;

    case default_option::opt_levels_2_plus_speed_only:
      enabled = (level >= 2 && !size && !debug);
      break;

    case default_option::opt_levels_3_plus:
      enabled = (level >= 3);
      break;

    case default_option::opt_levels_3_plus_and_size:
      enabled = (level >=3 || size);
      break;

    case default_option::opt_levels_size:
      enabled = size;
      break;

    case default_option::opt_levels_fast:
      enabled = fast;
      break;
    }

    if (enabled) {
      assert (!default_opt->negative || !o->reject_negative);
      _handle_opt(t, o, default_opt->val, default_opt->negative,
		  true, opt_table_arch);
    } else if (!default_opt->val && !o->reject_negative) {
      _handle_opt(t, o, nullptr, !default_opt->negative, true,
		  opt_table_arch);
    } else {
      // For our purposes default_opt->val should imply
      // o->reject_negative.
      assert(o->reject_negative);
    }
  }
}

void target_gcc::
_handle_opt(const gcc_cmdline_parser::option * const opt_table,
	    const gcc_cmdline_parser::option * const o,
	    const char *val, const bool negative,
	    const bool generated,
	    const gcc_cmdline_parser::option &opt_table_arch)
{
  if (!o || opt_table == gcc_opt_table_common) {
    _opts_common.handle_opt(o, val, negative, generated, _gcc_version);
  } else if (opt_table == gcc_opt_table_c_family) {
    _opts_c_family.handle_opt(o, val, negative, generated, _gcc_version);
  } else {
    assert(opt_table == &opt_table_arch);
    this->_arch_handle_opt(o, val, negative, generated);
  }
}

void target_gcc::_finish_options()
{
  // This corresponds to GCC's finish_options().
  _opts_common.finish_options();
  _opts_c_family.finish_options();
}

void target_gcc::_process_options()
{
  // This corresponds to GCC's process_options().
  _c_lang_post_options();
  this->_arch_option_override();
  _opts_common.process_options(_gcc_version);
  _opts_c_family.process_options();
}

void target_gcc::_c_lang_post_options()
{
  // This corresponds to GCC's c_common_post_options().
  _opts_common.c_lang_post_options(_gcc_version, _opts_c_family);
  _opts_c_family.c_lang_post_options();
}

target_gcc::opts_common::opts_common(const gcc_cmdline_parser::gcc_version &ver)
  noexcept
  : optimize(0), optimize_debug(false), optimize_fast(false),
    optimize_size(false), flag_expensive_optimizations(false),
    flag_associative_math(false),
    flag_excess_precision(excess_precision::ep_default),
    flag_finite_math_only(false), flag_errno_math(true),
    flag_reciprocal_math(false), flag_signaling_nans(false),
    flag_signed_zeros(true), flag_trapping_math(true),
    flag_unsafe_math_optimizations(false),
    flag_fp_contract_mode(fp_contract_mode::fcm_fast),
    flag_fp_contract_mode_set(false),
    flag_permitted_flt_eval_methods(permitted_flt_eval_methods::pfem_default),
    flag_permitted_flt_eval_methods_set(false),
    flag_single_precision_constant(false),
    flag_abi_version(0), flag_leading_underscore(-1),
    flag_no_inline(false),
    flag_pic(-1), flag_pie(-1),
    flag_omit_frame_pointer(false), flag_omit_frame_pointer_set(false),
    flag_exceptions(false), flag_non_call_exceptions(false),
    flag_unwind_tables(false), flag_asynchronous_unwind_tables(false),
    flag_stack_protect(-1)
{
  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (!(gcc_version{5, 1, 0} <= ver)) {
    // GCC 4.8.x and 4.9.x used to initialize flag_abi_version to 2.
    flag_abi_version = 2;
  }
}

void target_gcc::opts_common::init_options_struct() noexcept
{
  // This gets called from target_gcc::_init_options_struct() which
  // corresponds to GCC's language agnostic init_options_struct().
}

void target_gcc::opts_common::c_lang_init_options_struct() noexcept
{
  // This gets called from target_gcc::_c_lang_init_options_struct()
  // which corresponds to GCC's c_common_init_options_struct().
  flag_exceptions = false;
}

void target_gcc::opts_common::c_lang_init_options() noexcept
{
  // This gets called from target_gcc::_c_lang_init_options()
  // which corresponds to GCC's c_common_init_options().
}

struct target_gcc::opts_common::_sanitizer_opts_impl
{
private:
  template<sanitizer_flags_bit... bits>
  struct __flags_set;

  template<sanitizer_flags_bit bit>
  struct __flags_set<bit>
  {
    static void set(sanitizer_flags_type &flags, const bool value) noexcept
    {
      flags.set(bit, value);
    }
  };

  template<sanitizer_flags_bit bit, sanitizer_flags_bit... bits>
  struct __flags_set<bit, bits...>
  {
    static void set(sanitizer_flags_type &flags, const bool value) noexcept
    {
      flags.set(bit, value);
      __flags_set<bits...>::set(flags, value);
    }
  };

  template<typename base_flags_set, sanitizer_flags_bit... bits>
  struct _flags_set
  {
    static void set(sanitizer_flags_type &flags, const bool value) noexcept
    {
      base_flags_set::set(flags, value);
      __flags_set<bits...>::set(flags, value);
    }
  };

  using _flags_set_shift =
    __flags_set<sanitizer_flags_bit_shift_base,
		sanitizer_flags_bit_shift_exponent>;

  using _flags_set_undefined =
    _flags_set<_flags_set_shift,
	       sanitizer_flags_bit_divide,
	       sanitizer_flags_bit_unreachable,
	       sanitizer_flags_bit_vla,
	       sanitizer_flags_bit_null,
	       sanitizer_flags_bit_return,
	       sanitizer_flags_bit_si_overflow,
	       sanitizer_flags_bit_bool,
	       sanitizer_flags_bit_enum,
	       sanitizer_flags_bit_bounds,
	       sanitizer_flags_bit_alignment,
	       sanitizer_flags_bit_nonnull_attribute,
	       sanitizer_flags_bit_returns_nonnull_attribute,
	       sanitizer_flags_bit_object_size,
	       sanitizer_flags_bit_vptr,
	       sanitizer_flags_bit_pointer_overflow,
	       sanitizer_flags_bit_builtin>;

  using gcc_version = gcc_cmdline_parser::gcc_version;

  static sanitizer_flags_type _get_valid_flags(const gcc_version &ver) noexcept;

  struct _table_entry
  {
    const char *name;
    void (*set_flags)(sanitizer_flags_type&, const bool);
    gcc_version min_gcc_version;
  };

  static const _table_entry _table[];

public:
  static void parse_and_apply_list(const char *list,
				   sanitizer_flags_type &flags,
				   const bool negative,
				   const gcc_version &ver);

};

target_gcc::opts_common::sanitizer_flags_type
target_gcc::opts_common::_sanitizer_opts_impl::
_get_valid_flags(const gcc_version &ver) noexcept
{
  sanitizer_flags_type valid_flags;

  valid_flags.set(sanitizer_flags_bit_address);
  valid_flags.set(sanitizer_flags_bit_thread);

  if (gcc_version{4, 9, 0} <= ver) {
    valid_flags.set(sanitizer_flags_bit_leak);
    valid_flags.set(sanitizer_flags_bit_shift_base);
    valid_flags.set(sanitizer_flags_bit_shift_exponent);
    valid_flags.set(sanitizer_flags_bit_divide);
    valid_flags.set(sanitizer_flags_bit_unreachable);
    valid_flags.set(sanitizer_flags_bit_vla);
    valid_flags.set(sanitizer_flags_bit_null);
    valid_flags.set(sanitizer_flags_bit_return);
    valid_flags.set(sanitizer_flags_bit_si_overflow);
    valid_flags.set(sanitizer_flags_bit_bool);
    valid_flags.set(sanitizer_flags_bit_enum);
  }

  if (gcc_version{5, 1, 0} <= ver) {
    valid_flags.set(sanitizer_flags_bit_user_address);
    valid_flags.set(sanitizer_flags_bit_kernel_address);
    valid_flags.set(sanitizer_flags_bit_float_divide);
    valid_flags.set(sanitizer_flags_bit_float_cast);
    valid_flags.set(sanitizer_flags_bit_bounds);
    valid_flags.set(sanitizer_flags_bit_alignment);
    valid_flags.set(sanitizer_flags_bit_nonnull_attribute);
    valid_flags.set(sanitizer_flags_bit_returns_nonnull_attribute);
    valid_flags.set(sanitizer_flags_bit_object_size);
    valid_flags.set(sanitizer_flags_bit_vptr);
  }

  if (gcc_version{6, 1, 0} <= ver) {
    valid_flags.set(sanitizer_flags_bit_bounds_strict);
  }

  if (gcc_version{8, 1, 0} <= ver) {
    valid_flags.set(sanitizer_flags_bit_pointer_overflow);
    valid_flags.set(sanitizer_flags_bit_builtin);
    valid_flags.set(sanitizer_flags_bit_pointer_compare);
    valid_flags.set(sanitizer_flags_bit_pointer_subtract);
  }

  return valid_flags;
}

const target_gcc::opts_common::_sanitizer_opts_impl::_table_entry
target_gcc::opts_common::_sanitizer_opts_impl::_table[] = {
  {
    "address",
    __flags_set<sanitizer_flags_bit_address,
		  sanitizer_flags_bit_user_address>::set,
  },
  {
    "kernel-address",
    __flags_set<sanitizer_flags_bit_address,
		  sanitizer_flags_bit_kernel_address>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "pointer-compare",
    __flags_set<sanitizer_flags_bit_pointer_compare>::set,
    .min_gcc_version = {8, 1, 0},
  },
  {
    "pointer-subtract",
    __flags_set<sanitizer_flags_bit_pointer_subtract>::set,
    .min_gcc_version = {8, 1, 0},
  },
  {
    "thread",
    __flags_set<sanitizer_flags_bit_thread>::set,
  },
  {
    "leak",
    __flags_set<sanitizer_flags_bit_leak>::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "shift",
    _flags_set_shift::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "shift-base",
    __flags_set<sanitizer_flags_bit_shift_base>::set,
    .min_gcc_version = {7, 1, 0},
  },
  {
    "shift-exponent",
    __flags_set<sanitizer_flags_bit_shift_exponent>::set,
    .min_gcc_version = {7, 1, 0},
  },
  {
    "integer-divide-by-zero",
    __flags_set<sanitizer_flags_bit_divide>::set,
   .min_gcc_version = {4, 9, 0},
   },
  {
    "undefined",
    _flags_set_undefined::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "unreachable",
    __flags_set<sanitizer_flags_bit_unreachable>::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "vla-bound",
    __flags_set<sanitizer_flags_bit_vla>::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "return",
    __flags_set<sanitizer_flags_bit_return>::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "null",
    __flags_set<sanitizer_flags_bit_null>::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "signed-integer-overflow",
    __flags_set<sanitizer_flags_bit_si_overflow>::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "bool",
    __flags_set<sanitizer_flags_bit_bool>::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "enum",
    __flags_set<sanitizer_flags_bit_enum>::set,
    .min_gcc_version = {4, 9, 0},
  },
  {
    "float-divide-by-zero",
    __flags_set<sanitizer_flags_bit_float_divide>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "float-cast-overflow",
    __flags_set<sanitizer_flags_bit_float_cast>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "bounds",
    __flags_set<sanitizer_flags_bit_bounds>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "bounds-strict",
    __flags_set<sanitizer_flags_bit_bounds,
		  sanitizer_flags_bit_bounds_strict>::set,
    .min_gcc_version = {6, 1, 0},
  },
  {
    "alignment",
    __flags_set<sanitizer_flags_bit_alignment>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "nonnull-attribute",
    __flags_set<sanitizer_flags_bit_nonnull_attribute>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "returns-nonnull-attribute",
    __flags_set<sanitizer_flags_bit_returns_nonnull_attribute>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "object-size",
    __flags_set<sanitizer_flags_bit_object_size>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "vptr",
    __flags_set<sanitizer_flags_bit_vptr>::set,
    .min_gcc_version = {5, 1, 0},
  },
  {
    "pointer-overflow",
    __flags_set<sanitizer_flags_bit_pointer_overflow>::set,
    .min_gcc_version = {8, 1, 0},
  },
  {
    "builtin",
    __flags_set<sanitizer_flags_bit_builtin>::set,
    .min_gcc_version = {8, 1, 0},
  },
  { nullptr }
};

void target_gcc::opts_common::_sanitizer_opts_impl::
parse_and_apply_list(const char *list,
		     sanitizer_flags_type &flags,
		     const bool negative,
		     const gcc_version &ver)
{
  const char *p = list;

  while (*p) {
    const char *cur_end;

    cur_end = std::strchr(p, ',');
    if (!cur_end)
      cur_end = p + std::strlen(p);

    const std::size_t len = cur_end - p;

    // "all" is special and hasn't got a table entry, treat it explicitly
    if (len == sizeof("all") && std::equal(p, cur_end, "all")) {
      if (!negative)
	throw cmdline_except{"-fsanitize=all option is not valid"};
      else
	flags.reset();

    } else {
      const auto *entry = _table;
      while (entry->name &&
	     (std::strlen(entry->name) != len ||
	      !std::equal(p, cur_end, entry->name) ||
	      !(entry->min_gcc_version <= ver))) {
	++entry;
      }

      if (!entry->name) {
	throw cmdline_except{
	  "unrecognized argument \"" + std::string{p, cur_end} +
	  "\"to -fsanitize"
	};
      }

      entry->set_flags(flags, !negative);
    }

    p = cur_end + 1;
  }

  flags &= _get_valid_flags(ver);
}

void target_gcc::opts_common::
handle_opt(const gcc_cmdline_parser::option * const o,
	   const char *val, const bool negative,
	   const bool generated,
	   const gcc_cmdline_parser::gcc_version &ver)
{
  if (!o) {
    if (!base_file.empty()) {
      throw cmdline_except{
	std::string{"more than one input file: '"} +
		    base_file + "' and '" + val + "'"
		   };
    }

    base_file = val;
    return;
  }

  switch (o->code) {
  case opt_code_common_unused:
    break;

  case opt_code_common_O:
    // Handled in pre-scan
    break;

  case opt_code_common_Ofast:
    // Handled in pre-scan
    break;

  case opt_code_common_Og:
    // Handled in pre-scan
    break;

  case opt_code_common_Os:
    // Handled in pre-scan
    break;

  case opt_code_common_fexpensive_optimizations:
    flag_expensive_optimizations = !negative;
    break;

  case opt_code_common_fassociative_math:
    flag_associative_math = !negative;
    break;

  case opt_code_common_fexcess_precision:
    assert(val);
    if (!std::strcmp(val, "standard")) {
      flag_excess_precision = excess_precision::ep_standard;
    } else if (!std::strcmp(val, "fast")) {
      flag_excess_precision = excess_precision::ep_fast;
    } else {
      throw cmdline_except{"invalid parameter for -fexcess-precision"};
    }
    break;

  case opt_code_common_ffinite_math_only:
    flag_finite_math_only = !negative;
    break;

  case opt_code_common_fmath_errno:
    flag_errno_math = !negative;
    break;

  case opt_code_common_freciprocal_math:
    flag_reciprocal_math = !negative;
    break;

  case opt_code_common_fsignaling_nans:
    flag_signaling_nans = !negative;
    break;

  case opt_code_common_fsigned_zeros:
    flag_signed_zeros = !negative;
    break;

  case opt_code_common_ftrapping_math:
    flag_trapping_math = !negative;
    break;

  case opt_code_common_funsafe_math_optimizations:
    flag_unsafe_math_optimizations = !negative;
    _set_unsafe_math_optimizations_flags(!negative);
    break;

  case opt_code_common_ffast_math:
    _set_fast_math_flags(!negative);
    break;

  case opt_code_common_ffp_contract:
    if (!std::strcmp(val, "off") || !std::strcmp(val, "on")) {
      flag_fp_contract_mode = fp_contract_mode::fcm_off;
    } else if (!std::strcmp(val, "fast")) {
      flag_fp_contract_mode = fp_contract_mode::fcm_fast;
    } else {
      throw cmdline_except{"invalid value for -ffp-contract"};
    }
    if (!generated)
      flag_fp_contract_mode_set = true;
    break;

  case opt_code_common_fpermitted_flt_eval_methods:
    if (!std::strcmp(val, "c11")) {
      flag_permitted_flt_eval_methods = permitted_flt_eval_methods::pfem_c11;
    } else if (!std::strcmp(val, "ts-18661-3")) {
      flag_permitted_flt_eval_methods =
	permitted_flt_eval_methods::pfem_ts_18661;
    } else {
      throw cmdline_except{"invalid value for -fpermitted-flt-eval-methods"};
    }
    if (!generated)
      flag_permitted_flt_eval_methods_set = true;
    break;

  case opt_code_common_fsingle_precision_constant:
    flag_single_precision_constant = !negative;
    break;

  case opt_code_common_fabi_version:
    {
      std::size_t endpos;
      int _abi_version = -1;

      try {
	_abi_version = std::stoi(std::string{val}, &endpos);
      } catch (...) {}

      if (_abi_version < 0 || val[endpos] != '\0')
	throw cmdline_except{"invalid argument to -fabi-version"};

      flag_abi_version = static_cast<unsigned int>(_abi_version);
    }
    break;

  case opt_code_common_fleading_underscore:
    flag_leading_underscore = !negative;
    break;

  case opt_code_common_finline:
    flag_no_inline = negative;
    break;

  case opt_code_common_fPIC:
    if (!negative)
      flag_pic = 2;
    else
      flag_pic = 0;
    break;

  case opt_code_common_fPIE:
    if (!negative)
      flag_pie = 2;
    else
      flag_pie = 0;
    break;

  case opt_code_common_fpic:
    if (!negative)
      flag_pic = 1;
    else
      flag_pic = 0;
    break;

  case opt_code_common_fpie:
    if (!negative)
      flag_pie = 1;
    else
      flag_pie = 0;
    break;

  case opt_code_common_fomit_frame_pointer:
    flag_omit_frame_pointer = !negative;
    if (!generated)
      flag_omit_frame_pointer_set = true;
    break;

  case opt_code_common_fexceptions:
    flag_exceptions = !negative;
    break;

  case opt_code_common_fnon_call_exceptions:
    flag_non_call_exceptions = !negative;
    break;

  case opt_code_common_funwind_tables:
    flag_unwind_tables = !negative;
    break;

  case opt_code_common_fasynchronous_unwind_tables:
    flag_asynchronous_unwind_tables = !negative;
    break;

  case opt_code_common_fstack_protector:
    flag_stack_protect = !negative;
    break;

  case opt_code_common_fstack_protector_all:
    flag_stack_protect = 2;
    break;

  case opt_code_common_fstack_protector_explicit:
    flag_stack_protect = 3;
    break;

  case opt_code_common_fstack_protector_strong:
    flag_stack_protect = 4;
    break;

  case opt_code_common_fsanitize:
    _sanitizer_opts_impl::parse_and_apply_list(val, flag_sanitize,
					       negative, ver);
    break;

  case opt_code_common_fsanitize_address:
    flag_sanitize.set(sanitizer_flags_bit_address, !negative);
    break;

  case opt_code_common_fsanitize_thread:
    flag_sanitize.set(sanitizer_flags_bit_thread, !negative);
    break;
  }
}

void target_gcc::opts_common::finish_options() noexcept
{
  // This gets called from target_gcc::_finish_options() which
  // corresponds to GCC's finish_options().

  // The code for defaulting flag_pie and flag_pic has been introduced
  // with GCC 6.1.0. Before that, both had been initialized to 0
  // (rather than -1), so there's no change in functionality here.
  if (flag_pie == -1) {
    if (flag_pic == -1)
      flag_pie = 0;  // Assume GCC's DEFAULT_FLAG_PIE == 0
    else
      flag_pie = 0;
  }

  if (flag_pie)
    flag_pic = flag_pie;
  else if (flag_pic == -1)
    flag_pic = 0;

  // The code for defaulting flag_stack_protect has been introduced
  // with GCC 6.1.0.  Before that, it had been initialized to 0
  // (rather than -1), so there's no change in functionality here.
  if (flag_stack_protect == -1)
    flag_stack_protect = 0; // Assume GCC's DEFAULT_FLAG_SSP == 0

  if (optimize == 0)
    flag_no_inline = true;
}

void target_gcc::opts_common::
c_lang_post_options(const gcc_cmdline_parser::gcc_version &ver,
		    const opts_c_family &c_opts)
  noexcept
{
  // This gets called from target_gcc::_c_lang_post_options() which
  // corresponds to GCC's c_common_post_options().
  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (flag_excess_precision == excess_precision::ep_default) {
    if (c_opts.is_iso())
      flag_excess_precision = excess_precision::ep_standard;
    else
      flag_excess_precision = excess_precision::ep_fast;
  }

  if (gcc_version{4, 9, 0} <= ver) {
    if (c_opts.is_iso() &&
	!flag_fp_contract_mode_set &&
	!flag_unsafe_math_optimizations) {
      flag_fp_contract_mode = fp_contract_mode::fcm_off;
    }
  }

  // This code exists as of GCC 7.1.0. Before that, the
  // flag_permitted_flt_eval_methods variable hadn't existed.
  if (gcc_version{7, 1, 0} <= ver) {
    if (!c_opts.is_iso() &&
	!flag_permitted_flt_eval_methods_set) {
      flag_permitted_flt_eval_methods =
	permitted_flt_eval_methods::pfem_ts_18661;
    } else {
      flag_permitted_flt_eval_methods = permitted_flt_eval_methods::pfem_c11;
    }
  }

  if (gcc_version{8, 2, 0} <= ver) {
    const unsigned int latest_abi_version = 13;

    if (!flag_abi_version || flag_abi_version > latest_abi_version)
      flag_abi_version = latest_abi_version;

  } else if (gcc_version{8, 1, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 12;

  } else if (gcc_version{7, 1, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 11;

  } else if (gcc_version{6, 1, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 10;

  } else if (gcc_version{5, 2, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 9;

  } else if (gcc_version{5, 1, 0} <= ver) {
    if (!flag_abi_version)
      flag_abi_version = 8;
  } else {
    // For GCC versions <= 4.9, leave a flag_abi_version == 0 as is.
    // Note that for these, flag_abi_version is initialized with '2',
    // hence the user would have to set a zero value
    // explicitly. Preprocessor defines are special cased on
    // flag_abi_version == 0 for these GCC versions.
  }
}

void target_gcc::opts_common::
process_options(const gcc_cmdline_parser::gcc_version &ver)
{
  // This gets called from target_gcc::_process_options() which
  // corresponds to GCC's process_options().
  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (flag_abi_version == 1 && gcc_version{5, 1, 0} <= ver)
    throw cmdline_except{"-fabi_version=1 only supported for gcc < 5.1.0"};

  if (flag_non_call_exceptions)
    flag_asynchronous_unwind_tables = 1;
  if (flag_asynchronous_unwind_tables)
    flag_unwind_tables = true;

  if (flag_signaling_nans)
    flag_trapping_math = true;

  if (flag_associative_math && (flag_trapping_math || flag_signed_zeros))
    flag_associative_math = false;
}

bool target_gcc::opts_common::fast_math_flags_set_p() const noexcept
{
  return (!flag_trapping_math &&
	  flag_unsafe_math_optimizations &&
	  flag_finite_math_only &&
	  !flag_signed_zeros &&
	  !flag_errno_math &&
	  flag_excess_precision == excess_precision::ep_fast);
}

void
target_gcc::opts_common::_set_unsafe_math_optimizations_flags(const bool set)
  noexcept
{
  flag_trapping_math = !set;
  flag_signed_zeros = !set;
  flag_associative_math = !set;
  flag_reciprocal_math = !set;
}

void target_gcc::opts_common::_set_fast_math_flags(const bool set) noexcept
{
  flag_unsafe_math_optimizations = set;
  _set_unsafe_math_optimizations_flags(set);

  flag_finite_math_only = set;
  flag_errno_math = !set;

  if (set) {
    flag_excess_precision = excess_precision::ep_fast;
    flag_signaling_nans = false;
  }
}


target_gcc::opts_c_family::opts_c_family() noexcept
  : flag_undef(false), c_std(c_lang_kind::clk_gnuc89),
    flag_gnu89_inline(-1), flag_signed_char(false),
    flag_openacc(false), flag_openmp(false), flag_hosted(true)
{}

void target_gcc::opts_c_family::init_options_struct(const target_gcc &target)
  noexcept
{
  // This gets called from target_gcc::_init_options_struct() which
  // corresponds to GCC's language agnostic init_options_struct().
  flag_signed_char = target._arch_default_char_is_signed();
}

void target_gcc::opts_c_family::c_lang_init_options_struct() noexcept
{
  // This gets called from target_gcc::_c_lang_init_options_struct()
  // which corresponds to GCC's c_common_init_options_struct().
}

void target_gcc::opts_c_family::
c_lang_init_options(const gcc_cmdline_parser::gcc_version &ver)
  noexcept
{
  // This gets called from target_gcc::_c_lang_init_options() which
  // corresponds to GCC's c_common_init_options().
  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (gcc_version{8, 1, 0} <= ver)
    _set_std_c17(false);
  else if (gcc_version{5, 1, 0} <= ver)
    _set_std_c11(false);
  else
    _set_std_c89(false, false);
}

void target_gcc::
opts_c_family::handle_opt(const gcc_cmdline_parser::option * const o,
			  const char *val, const bool negative,
			  const bool generated,
			  const gcc_cmdline_parser::gcc_version &ver)
{
  switch (o->code) {
  case opt_code_c_family_unused:
    break;

  case opt_code_c_family_D:
    macro_defs_and_undefs.emplace_back(macro_def_or_undef{false, val});
    break;

  case opt_code_c_family_I:
    if (!std::strcmp(val, "-")) {
      include_dirs_quoted.insert
	(include_dirs_quoted.end(),
	 std::make_move_iterator(include_dirs.begin()),
	 std::make_move_iterator(include_dirs.end()));
      include_dirs.clear();
      include_dirs.shrink_to_fit();
    } else {
      include_dirs.push_back(val);
    }
    break;

  case opt_code_c_family_U:
    macro_defs_and_undefs.emplace_back(macro_def_or_undef{true, val});
    break;

  case opt_code_c_family_idirafter:
    include_dirs_after.push_back(val);
    break;

  case opt_code_c_family_include:
    pre_includes.push_back(val);
    break;

  case opt_code_c_family_iquote:
    include_dirs_quoted.push_back(val);
    break;

  case opt_code_c_family_isystem:
    include_dirs_after.push_back(val);
    break;

  case opt_code_c_family_undef:
    flag_undef = true;
    break;

  case opt_code_c_family_fgnu89_inline:
    flag_undef = !negative;
    break;

  case opt_code_c_family_fsigned_char:
    flag_signed_char = !negative;
    break;

  case opt_code_c_family_funsigned_char:
    flag_signed_char = negative;
    break;

  case opt_code_c_family_fopenacc:
    flag_openacc = !negative;
    break;

  case opt_code_c_family_fopenmp:
   flag_openmp = !negative;
   break;

  case opt_code_c_family_fhosted:
    flag_hosted = !negative;
    break;

  case opt_code_c_family_ffreestanding:
    flag_hosted = negative;
    break;

  case opt_code_c_family_ansi:
    _set_std_c89(false, true);
    break;

  case opt_code_c_family_std_c90:
    _set_std_c89(false, true);
    break;

  case opt_code_c_family_std_iso9899_199409:
    _set_std_c89(true, true);
    break;

  case opt_code_c_family_std_c99:
    _set_std_c99(true);
    break;

  case opt_code_c_family_std_c11:
    _set_std_c11(true);
    break;

  case opt_code_c_family_std_c17:
    _set_std_c17(true);
    break;

  case opt_code_c_family_std_c2x:
    _set_std_c2x(true);
    break;

  case opt_code_c_family_std_gnu90:
    _set_std_c89(false, false);
    break;

  case opt_code_c_family_std_gnu99:
    _set_std_c99(false);
    break;

  case opt_code_c_family_std_gnu11:
    _set_std_c11(false);
    break;

  case opt_code_c_family_std_gnu17:
    _set_std_c17(false);
    break;

  case opt_code_c_family_std_gnu2x:
    _set_std_c2x(false);
    break;

  case opt_code_c_family_std_cxx98:
    /* fall through */
  case opt_code_c_family_std_cxx11:
    /* fall through */
  case opt_code_c_family_std_cxx14:
    /* fall through */
  case opt_code_c_family_std_cxx17:
    /* fall through */
  case opt_code_c_family_std_cxx1y:
    /* fall through */
  case opt_code_c_family_std_cxx1z:
    /* fall through */
  case opt_code_c_family_std_cxx2a:
    /* fall through */
  case opt_code_c_family_std_gnuxx98:
    /* fall through */
  case opt_code_c_family_std_gnuxx11:
    /* fall through */
  case opt_code_c_family_std_gnuxx14:
    /* fall through */
  case opt_code_c_family_std_gnuxx17:
    /* fall through */
  case opt_code_c_family_std_gnuxx1y:
    /* fall through */
  case opt_code_c_family_std_gnuxx1z:
    /* fall through */
  case opt_code_c_family_std_gnuxx2a:
    throw cmdline_except{"C++ standards not supported"};
    break;
  }
}

void target_gcc::opts_c_family::finish_options() noexcept
{
  // This gets called from target_gcc::_finish_options() which
  // corresponds to GCC's finish_options().
}

void target_gcc::opts_c_family::c_lang_post_options()
{
  // This gets called from target_gcc::_c_lang_post_options() which
  // corresponds to GCC's c_common_post_options().
  if (flag_gnu89_inline == -1) {
    flag_gnu89_inline = !this->is_std_geq_c99();
  } else if (!flag_gnu89_inline && !this->is_std_geq_c99()) {
    throw cmdline_except{
      "-fno-gnu89-inline is only supported in GNU99 or C99 mode"
    };
  }
}

void target_gcc::opts_c_family::process_options() noexcept
{
  // This gets called from target_gcc::_process_options() which
  // corresponds to GCC's process_options().
}

bool target_gcc::opts_c_family::is_iso() const noexcept
{
  switch (c_std) {
  case c_lang_kind::clk_stdc89:
    /* fall through */
  case c_lang_kind::clk_stdc94:
    /* fall through */
  case c_lang_kind::clk_stdc99:
    /* fall through */
  case c_lang_kind::clk_stdc11:
    /* fall through */
  case c_lang_kind::clk_stdc17:
    /* fall through */
  case c_lang_kind::clk_stdc2x:
    return true;

  case c_lang_kind::clk_gnuc89:
    /* fall through */
  case c_lang_kind::clk_gnuc99:
    /* fall through */
  case c_lang_kind::clk_gnuc11:
    /* fall through */
  case c_lang_kind::clk_gnuc17:
    /* fall through */
  case c_lang_kind::clk_gnuc2x:
    return false;
  }
}

bool target_gcc::opts_c_family::is_std_geq_c99() const noexcept
{
  switch (c_std) {
  case c_lang_kind::clk_stdc89:
    /* fall through */
  case c_lang_kind::clk_gnuc89:
    /* fall through */
    return false;

  case c_lang_kind::clk_stdc94:
    /* fall through */
  case c_lang_kind::clk_stdc99:
    /* fall through */
  case c_lang_kind::clk_stdc11:
    /* fall through */
  case c_lang_kind::clk_stdc17:
    /* fall through */
  case c_lang_kind::clk_stdc2x:
    /* fall through */
  case c_lang_kind::clk_gnuc99:
    /* fall through */
  case c_lang_kind::clk_gnuc11:
    /* fall through */
  case c_lang_kind::clk_gnuc17:
    /* fall through */
  case c_lang_kind::clk_gnuc2x:
    return true;
  }
}

void target_gcc::opts_c_family::_set_std_c89(const bool c94, const bool iso)
  noexcept
{
  if (c94) {
    c_std = c_lang_kind::clk_stdc94;
    return;
  }

  c_std = iso ? c_lang_kind::clk_stdc89 : c_lang_kind::clk_gnuc89;
}

void target_gcc::opts_c_family::_set_std_c99(const bool iso) noexcept
{
  c_std = iso ? c_lang_kind::clk_stdc99 : c_lang_kind::clk_gnuc99;
}

void target_gcc::opts_c_family::_set_std_c11(const bool iso) noexcept
{
  c_std = iso ? c_lang_kind::clk_stdc11 : c_lang_kind::clk_gnuc11;
}

void target_gcc::opts_c_family::_set_std_c17(const bool iso) noexcept
{
  c_std = iso ? c_lang_kind::clk_stdc17 : c_lang_kind::clk_gnuc17;
}

void target_gcc::opts_c_family::_set_std_c2x(const bool iso) noexcept
{
  c_std = iso ? c_lang_kind::clk_stdc2x : c_lang_kind::clk_gnuc2x;
}

gcc_cmdline_parser::gcc_version
target_gcc::_parse_version(const char * const version)
{
  const char * const pmajor = version;
  const char *pminor = std::strchr(pmajor, '.');
  const char *ppatchlevel = nullptr;

  if (pminor) {
    ++pminor;
    ppatchlevel = std::strchr(pminor, '.');
    if (ppatchlevel)
      ++ppatchlevel;
  }

  if (!ppatchlevel ||
      (pminor == pmajor + 1) ||
      (ppatchlevel == pminor + 1) ||
      (*ppatchlevel == '\0')) {
    throw cmdline_except {
		(std::string{"compiler version specifier \'"}
		 + version + "\' has invalid format")
	  };
  }

  std::size_t endpos;
  int major = std::stoi(std::string{pmajor, pminor - 1}, &endpos);
  if (*(pmajor + endpos) != '.')
    major = -1;

  int minor = std::stoi(std::string{pminor, ppatchlevel - 1}, &endpos);
  if (*(pminor + endpos) != '.')
    minor = -1;

  int patchlevel = std::stoi(std::string{ppatchlevel}, &endpos);
  if (*(ppatchlevel + endpos) != '\0')
    patchlevel = -1;

  if (major < 0 || minor < 0 || patchlevel < 0) {
    throw cmdline_except {
		(std::string{"compiler version specifier \'"}
		 + version + "\' has invalid format")
	  };
  }

  using gcc_version = gcc_cmdline_parser::gcc_version;
  const gcc_version parsed_version(major, minor, patchlevel);
  constexpr gcc_version MIN_SUPP_VER_48{4, 8, 0};
  constexpr gcc_version MAX_SUPP_VER_48{4, 8, 5};
  constexpr gcc_version MIN_SUPP_VER_49{4, 9, 0};
  constexpr gcc_version MAX_SUPP_VER_49{4, 9, 4};
  // It looks like from 5.1.0 onwards, the upstream patchlevel is
  // always set to zero and may be used freely by distributions.
  constexpr gcc_version MIN_SUPP_VER_5{5, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_5{
    5, 5, std::numeric_limits<unsigned int>::max()
  };
  constexpr gcc_version MIN_SUPP_VER_6{6, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_6{
    6, 5, std::numeric_limits<unsigned int>::max()
  };
  constexpr gcc_version MIN_SUPP_VER_7{7, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_7{
    7, 4, std::numeric_limits<unsigned int>::max()
  };
  constexpr gcc_version MIN_SUPP_VER_8{8, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_8{
    8, 3, std::numeric_limits<unsigned int>::max()
  };
  constexpr gcc_version MIN_SUPP_VER_9{9, 1, 0};
  constexpr gcc_version MAX_SUPP_VER_9{
    9, 2, std::numeric_limits<unsigned int>::max()
  };

  auto &&in_range = [&](const gcc_version &f, const gcc_version &l) {
    return (f <= parsed_version && parsed_version <= l);
  };

  if (!in_range(MIN_SUPP_VER_48, MAX_SUPP_VER_48) &&
      !in_range(MIN_SUPP_VER_49, MAX_SUPP_VER_49) &&
      !in_range(MIN_SUPP_VER_5, MAX_SUPP_VER_5) &&
      !in_range(MIN_SUPP_VER_6, MAX_SUPP_VER_6) &&
      !in_range(MIN_SUPP_VER_7, MAX_SUPP_VER_7) &&
      !in_range(MIN_SUPP_VER_8, MAX_SUPP_VER_8) &&
      !in_range(MIN_SUPP_VER_9, MAX_SUPP_VER_9)) {
    throw cmdline_except {
		(std::string{"unrecognized compiler version \'"}
		 + version + "\'")
	  };
  }

  return parsed_version;
}

void target_gcc::_register_builtin_macros(preprocessor &pp) const
{
  const std::initializer_list<std::pair<const char *, const char*>>
    builtin_object_macros = {
	{ "__unix__", "1" },
	{ "__unix", "1" },
	{ "__linux__", "1" },
	{ "__linux", "1" },

	{ "__GCC_ATOMIC_TEST_AND_SET_TRUEVAL", "1" },
	{ "__ATOMIC_RELAXED", "0" },
	{ "__ATOMIC_CONSUME", "1" },
	{ "__ATOMIC_ACQUIRE", "2" },
	{ "__ATOMIC_RELEASE", "3" },
	{ "__ATOMIC_ACQ_REL", "4" },
	{ "__ATOMIC_SEQ_CST", "5" },
  };

  pp.register_builtin_macro("__GNUC__", std::to_string(_gcc_version.maj));
  pp.register_builtin_macro("__GNUC_MINOR__", std::to_string(_gcc_version.min));
  pp.register_builtin_macro("__GNUC_PATCHLEVEL__",
			    std::to_string(_gcc_version.patchlevel));

  for (const auto &bom : builtin_object_macros)
    pp.register_builtin_macro(bom.first, bom.second);

  this->_arch_register_builtin_macros(pp);
}

namespace
{
  static std::shared_ptr<const types::addressable_type>
  __mk_iM(const target_gcc &tgt, const int_mode_kind imk, const bool is_signed)
  {
    return _impl_proxy{tgt}._int_mode_to_type(imk, is_signed);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_i8(const target_gcc &tgt)
  {
    return __mk_iM(tgt, int_mode_kind::imk_QI, true);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_i16(const target_gcc &tgt)
  {
    return __mk_iM(tgt, int_mode_kind::imk_HI, true);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_u16(const target_gcc &tgt)
  {
    return __mk_iM(tgt, int_mode_kind::imk_HI, false);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_i32(const target_gcc &tgt)
  {
    return __mk_iM(tgt, int_mode_kind::imk_SI, true);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_u32(const target_gcc &tgt)
  {
    return __mk_iM(tgt, int_mode_kind::imk_SI, false);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_i64(const target_gcc &tgt)
  {
    return __mk_iM(tgt, int_mode_kind::imk_DI, true);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_u64(const target_gcc &tgt)
  {
    return __mk_iM(tgt, int_mode_kind::imk_DI, false);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_i128(const target_gcc &tgt)
  {
    return __mk_iM(tgt, int_mode_kind::imk_TI, true);
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_wi(const target_gcc &tgt)
  {
    const _impl_proxy impl_proxy{tgt};

    return impl_proxy._int_mode_to_type(impl_proxy._get_wint_mode(),
					impl_proxy._is_wint_signed());
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_pid(const target_gcc &tgt)
  {
    const _impl_proxy impl_proxy{tgt};

    return impl_proxy._int_mode_to_type(impl_proxy._get_pid_mode(),
					impl_proxy._is_pid_signed());
  }

  static std::shared_ptr<const types::addressable_type>
  mk_pF_v_var(const target&)
  {
    return (types::void_type::create()
	    ->derive_function(false)
	    ->derive_pointer());
  }

  static std::shared_ptr<const types::addressable_type>
  _mk_val(const target &tgt)
  {
    std::shared_ptr<const types::addressable_type> t =
      tgt.create_builtin_va_list_type();
    types::handle_types<void>
      ((wrap_callables<default_action_nop>
	([&](const types::array_type &at) {
	   t = at.get_element_type()->derive_pointer();
	 })),
       *t);
    return t;
  }

  class _builtin_func_pi_overload final : public builtin_func
  {
  public:
    _builtin_func_pi_overload(const unsigned int pi_arg_index,
			      const builtin_func::factory fac8,
			      const builtin_func::factory fac16,
			      const builtin_func::factory fac32,
			      const builtin_func::factory fac64,
			      const builtin_func::factory fac128) noexcept;

    virtual ~_builtin_func_pi_overload() noexcept override;

    virtual evaluation_result_type
    evaluate(klp::ccp::ast::ast &a, const target &tgt,
	     const ast::expr_func_invocation &efi) const override;

  private:
    const unsigned int _pi_arg_index;
    const builtin_func::factory _fac8;
    const builtin_func::factory _fac16;
    const builtin_func::factory _fac32;
    const builtin_func::factory _fac64;
    const builtin_func::factory _fac128;
  };

  template <const unsigned int pi_arg_index>
  struct _builtin_func_pi_overload_fac
  {
    static std::unique_ptr<builtin_func>
    create(const builtin_func::factory fac8,
	   const builtin_func::factory fac16,
	   const builtin_func::factory fac32,
	   const builtin_func::factory fac64,
	   const builtin_func::factory fac128)
    {
      return (std::unique_ptr<builtin_func>
	      (new _builtin_func_pi_overload(pi_arg_index, fac8, fac16,
					     fac32, fac64, fac128)));;
    }
  };

  class _builtin_overflow : public builtin_func
  {
  public:
    enum class op
    {
      add,
      sub,
      mul,
    };

    typedef std::shared_ptr<const types::int_type>(*t_fac)();

    virtual evaluation_result_type
    evaluate(klp::ccp::ast::ast &a, const target &tgt,
	     const ast::expr_func_invocation &efi) const override;

    template <t_fac tfac, bool p_variant, op o>
    static std::unique_ptr<builtin_func> create();

    static std::shared_ptr<const types::int_type> fac_i();
    static std::shared_ptr<const types::int_type> fac_u();
    static std::shared_ptr<const types::int_type> fac_l();
    static std::shared_ptr<const types::int_type> fac_ul();
    static std::shared_ptr<const types::int_type> fac_ll();
    static std::shared_ptr<const types::int_type> fac_ull();

  private:
    t_fac _target_fac;
    bool _p_variant;
    op _op;

    _builtin_overflow(const t_fac expected_it_fac, const bool p_variant,
		      const op op) noexcept;
  };
}

_builtin_func_pi_overload::
_builtin_func_pi_overload(const unsigned int pi_arg_index,
			  const builtin_func::factory fac8,
			  const builtin_func::factory fac16,
			  const builtin_func::factory fac32,
			  const builtin_func::factory fac64,
			  const builtin_func::factory fac128) noexcept
  : _pi_arg_index(pi_arg_index), _fac8(fac8), _fac16(fac16),
    _fac32(fac32), _fac64(fac64), _fac128(fac128)
{}

_builtin_func_pi_overload::~_builtin_func_pi_overload() noexcept = default;

builtin_func::evaluation_result_type _builtin_func_pi_overload::
evaluate(klp::ccp::ast::ast &a, const target &tgt,
	 const ast::expr_func_invocation &efi) const
{
  const ast::expr_list * const args = efi.get_args();
  const std::size_t n_args = !args ? 0 : args->size();

  if (n_args <= _pi_arg_index) {
    code_remark remark
      (code_remark::severity::warning,
       "too few arguments in builtin function invocation",
       a.get_pp_result(), efi.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  // The argument at index pi_arg_index shall have pointer to integer
  // type. Demultiplex according to that integer type's width.
  const ast::expr &e_pi_arg = (*args)[_pi_arg_index];
  const mpa::limbs::size_type width =
    (types::handle_types<mpa::limbs::size_type>
     ((wrap_callables<default_action_nop>
       ([&](const types::pointer_type &pt) {
	  return
	    (types::handle_types<mpa::limbs::size_type>
	     ((wrap_callables<default_action_nop>
	       ([&](const types::int_type &_it) {
		  types::handle_types<void>
		    ((wrap_callables<default_action_nop>
		      ([&](const types::enum_type &et) {
			 if (!et.is_complete()) {
			   code_remark remark
			     (code_remark::severity::fatal,
			      "incomplete enum type passed to builtin function",
			      a.get_pp_result(), e_pi_arg.get_tokens_range());
			   a.get_remarks().add(remark);
			   throw semantic_except(remark);
			 }
		       })),
		     _it);

		  return _it.get_width(tgt);
		},
		[&](const types::pointer_type&) {
		  // Undocumented, but gcc also accepts pointers to pointers
		  // for the atomic primitives.
		  return tgt.get_ptrdiff_width();
		},
		[&](const types::type&) -> mpa::limbs::size_type {
		  code_remark remark
		    (code_remark::severity::warning,
		     "expected pointer to int argument to builtin invocation",
		     a.get_pp_result(), e_pi_arg.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		})),
	      *pt.get_pointed_to_type()));
	},
	[&](const types::type&) -> mpa::limbs::size_type {
	  code_remark remark
	    (code_remark::severity::warning,
	     "expected pointer argument to builtin invocation",
	     a.get_pp_result(), e_pi_arg.get_tokens_range());
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
      code_remark remark
	(code_remark::severity::warning,
	 "can't handle argument's integer width at builtin invocation",
	 a.get_pp_result(), e_pi_arg.get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }
  };

  return fac()->evaluate(a, tgt, efi);
}

_builtin_overflow::_builtin_overflow(const t_fac target_fac,
				     const bool p_variant,
				     const op op) noexcept
  : _target_fac(target_fac), _p_variant(p_variant), _op(op)
{}

template <_builtin_overflow::t_fac tfac, bool p_variant,
	  _builtin_overflow::op o>
std::unique_ptr<builtin_func> _builtin_overflow::create()
{
  return (std::unique_ptr<builtin_func>
	  (new _builtin_overflow(tfac, p_variant, o)));
}

builtin_func::evaluation_result_type
_builtin_overflow::evaluate(klp::ccp::ast::ast &a, const target &tgt,
			    const ast::expr_func_invocation &efi) const
{
  auto &&myname =
    [this]() -> std::string {
      switch (_op) {
      case op::add:
	if (_p_variant)
	  return std::string{"__builtin_add_overflow_p()"};
	else
	  return std::string{"__builtin_add_overflow()"};

      case op::sub:
	if (_p_variant)
	  return std::string{"__builtin_sub_overflow_p()"};
	else
	  return std::string{"__builtin_sub_overflow()"};

      case op::mul:
	if (_p_variant)
	  return std::string{"__builtin_mul_overflow_p()"};
	else
	  return std::string{"__builtin_mul_overflow()"};
      }
    };

  if (!efi.get_args() || efi.get_args()->size() != 3) {
    code_remark remark
      (code_remark::severity::warning,
       "wrong number of arguments to " + myname(),
       a.get_pp_result(), efi.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const ast::expr_list &args = *efi.get_args();
  bool types_ok = true;
  mpa::limbs::size_type target_width =
    std::numeric_limits<mpa::limbs::size_type>::max();
  bool target_is_signed;
  const std::shared_ptr<const types::int_type> it_target_spec =
    _target_fac ? _target_fac() : nullptr;
  assert(!_p_variant || !_target_fac);
  if (_p_variant) {
    // The first two or, for the _p variant, all arguments shall be of
    // integer type.
    for (unsigned int i = 0; i < 3; ++i) {
      types::handle_types<void>
	((wrap_callables<default_action_nop>
	  ([&](const std::shared_ptr<const types::int_type> &it) {
	     types::handle_types<void>
	       ((wrap_callables<default_action_nop>
		 ([&](const types::enum_type &et) {
		    if (!et.is_complete()) {
		      code_remark remark
			(code_remark::severity::fatal,
			 ("argument to " + myname() +
			  " has incomplete enum type"),
			 a.get_pp_result(), args[i].get_tokens_range());
		      a.get_remarks().add(remark);
		      throw semantic_except(remark);
		    }
		  })),
		*it);

	     if (i == 2) {
	       target_width = it->get_width(tgt);
	       target_is_signed = it->is_signed(tgt);
	     }
	   },
	   [&](const std::shared_ptr<const types::bitfield_type> &bft) {
	     if (i == 2) {
	       target_width = bft->get_width(tgt);
	       target_is_signed = bft->is_signed(tgt);
	     }
	   },
	   [&](const std::shared_ptr<const types::type>&) {
	     types_ok = false;

	     code_remark remark
	       (code_remark::severity::warning,
		"non-integer argument to " + myname(),
		a.get_pp_result(), args[i].get_tokens_range());
	     a.get_remarks().add(remark);
	   })),
	 args[i].get_type());
    }
  } else {
    // For the non-_p variant, the first two or shall be of arithmetic
    // type.
    for (unsigned int i = 0; i < 2; ++i) {
      types::handle_types<void>
	((wrap_callables<default_action_nop>
	  ([&](const std::shared_ptr<const types::arithmetic_type> &t) {
	     types::handle_types<void>
	       ((wrap_callables<default_action_nop>
		 ([&](const types::enum_type &et) {
		    if (!et.is_complete()) {
		      code_remark remark
			(code_remark::severity::fatal,
			 ("argument to " + myname() +
			  " has incomplete enum type"),
			 a.get_pp_result(), args[i].get_tokens_range());
		      a.get_remarks().add(remark);
		      throw semantic_except(remark);
		    }
		  })),
		*t);
	   },
	   [&](const std::shared_ptr<const types::bitfield_type> &) {
	   },
	   [&](const std::shared_ptr<const types::type>&) {
	     types_ok = false;

	     code_remark remark
	       (code_remark::severity::warning,
		"non-arithmetic argument to " + myname(),
		a.get_pp_result(), args[i].get_tokens_range());
	     a.get_remarks().add(remark);
	   })),
	 args[i].get_type());
    }

    // Extract the third argument's type for !_p_variant.
    // The third argument is expected to be a pointer to int.
    types::handle_types<void>
      ((wrap_callables<default_action_nop>
	([&](const types::pointer_type &pt) {
	   types::handle_types<void>
	     ((wrap_callables<default_action_nop>
	       ([&](const std::shared_ptr<const types::int_type> &it) {
		  types::handle_types<void>
		    ((wrap_callables<default_action_nop>
		      ([&](const types::enum_type &et) {
			 if (!et.is_complete()) {
			   code_remark remark
			     (code_remark::severity::fatal,
			      ("argument to " + myname() +
			       " is of pointer to incomplete enum type"),
			      a.get_pp_result(), args[2].get_tokens_range());
			   a.get_remarks().add(remark);
			   throw semantic_except(remark);
			 }
		       })),
		     *it);

		  if (it_target_spec &&
		      !it_target_spec->is_compatible_with(tgt, *it, true)) {
		    types_ok = false;

		    code_remark remark
		      (code_remark::severity::warning,
		       ("third argument to " + myname() +
			" has incompatible pointer type"),
		       a.get_pp_result(), args[2].get_tokens_range());
		    a.get_remarks().add(remark);
		  } else {
		    target_width = it->get_width(tgt);
		    target_is_signed = it->is_signed(tgt);
		  }
		},
		[&](const std::shared_ptr<const types::type>&) {
		  types_ok = false;

		  code_remark remark
		    (code_remark::severity::warning,
		     ("third argument to " + myname() +
		      " is not a pointer to integer"),
		     a.get_pp_result(), args[2].get_tokens_range());
		  a.get_remarks().add(remark);
		})),
	      pt.get_pointed_to_type());
	 },
	 [&](const types::type&) {
	   types_ok = false;

	   code_remark remark
	     (code_remark::severity::warning,
	      "third argument to " + myname() + " is not a pointer",
	      a.get_pp_result(), args[2].get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
       *args[2].get_type());
  }

  if (!args[0].is_constexpr() || !args[1].is_constexpr() || !types_ok) {
    return evaluation_result_type{types::bool_type::create(), nullptr, false};
  }

  assert(target_width !=
	 std::numeric_limits<mpa::limbs::size_type>::max());

  const ast::constexpr_value &cv0 = args[0].get_constexpr_value();
  const target_int &i0 =
    (!it_target_spec ?
     cv0.get_int_value() :
     cv0.convert_to(tgt, *it_target_spec));
  const ast::constexpr_value &cv1 = args[1].get_constexpr_value();
  const target_int &i1 =
    (!it_target_spec ?
     cv1.get_int_value() :
     cv1.convert_to(tgt, *it_target_spec));

  mpa::limbs r;
  if (_op == op::add || _op == op::sub) {
    mpa::limbs ls0 = i0.get_limbs();
    if (!i0.is_signed()) {
      // Make room for an additional sign bit.
      ls0.resize(mpa::limbs::width_to_size(i0.width() + 1));
    }
    mpa::limbs ls1 = i1.get_limbs();
    if (_op == op::sub) {
      // Make room for an additional sign bit unconditionally.
      ls1.resize(mpa::limbs::width_to_size(i1.width() + 1));
      if (i1.is_signed()) {
	ls1.set_bits_at_and_above(i1.width(), ls1.test_bit(i1.width() - 1));
      }
      // And complement
      ls1 = ls1.complement();
    } if (!i1.is_signed()) {
      // Make room for an additional sign bit.
      ls1.resize(mpa::limbs::width_to_size(i1.width() + 1));
    }

    r = ls0.add_signed(ls1);

  } else {
    assert(_op == op::mul);
    bool is_negative = false;
    mpa::limbs ls0 = i0.get_limbs();
    if (i0.is_signed() && i0.is_negative()) {
      is_negative = true;
      ls0 = ls0.complement();
    }
    mpa::limbs ls1 = i1.get_limbs();
    if (i1.is_signed() && i1.is_negative()) {
      is_negative ^= true;
      ls1 = ls1.complement();
    }

    r = ls0 * ls1;
    r.resize(mpa::limbs::width_to_size(r.width() + 1));
    if (is_negative)
      r = r.complement();
  }

  bool overflow = false;
  if (target_is_signed) {
    if (target_width < r.width() - r.clrsb())
      overflow = true;
  } else {
    if (r.test_bit(r.width() - 1))
      overflow = true;
    else if (r.is_any_set_at_or_above(target_width))
      overflow = true;
  }

  std::shared_ptr<const types::bool_type> t_result = types::bool_type::create();
  const mpa::limbs::size_type w = t_result->get_width(tgt);
  const bool is_signed = t_result->is_signed(tgt);
  target_int ti_result =
    (overflow ?
     target_int::create_one(w - is_signed, is_signed) :
     target_int::create_zero(w - is_signed, is_signed));
  std::unique_ptr<ast::constexpr_value> cv_result;
  if (_p_variant &&
      (cv0.has_constness
       (ast::constexpr_value::constness::c_integer_constant_expr)) &&
      (cv1.has_constness
       (ast::constexpr_value::constness::c_integer_constant_expr))) {
    cv_result.reset
      (new ast::constexpr_value
       (ast::constexpr_value::integer_constant_expr_tag{}, ti_result));
  } else if (_p_variant &&
	     (cv0.has_constness
	      (ast::constexpr_value::constness::c_arithmetic_constant_expr)) &&
	     (cv1.has_constness
	      (ast::constexpr_value::constness::c_arithmetic_constant_expr))) {
    cv_result.reset
      (new ast::constexpr_value
       (ast::constexpr_value::arithmetic_constant_expr_tag{}, ti_result));
  } else {
    cv_result.reset(new ast::constexpr_value(ti_result));
  }

  return evaluation_result_type{std::move(t_result), std::move(cv_result),
				false};
}

std::shared_ptr<const types::int_type> _builtin_overflow::fac_i()
{
  return types::std_int_type::create(types::std_int_type::kind::k_int, true);
}

std::shared_ptr<const types::int_type> _builtin_overflow::fac_u()
{
  return types::std_int_type::create(types::std_int_type::kind::k_int, false);
}

std::shared_ptr<const types::int_type> _builtin_overflow::fac_l()
{
  return types::std_int_type::create(types::std_int_type::kind::k_long, true);
}

std::shared_ptr<const types::int_type> _builtin_overflow::fac_ul()
{
  return types::std_int_type::create(types::std_int_type::kind::k_long, false);
}

std::shared_ptr<const types::int_type> _builtin_overflow::fac_ll()
{
  return types::std_int_type::create(types::std_int_type::kind::k_long_long,
				     true);
}

std::shared_ptr<const types::int_type> _builtin_overflow::fac_ull()
{
  return types::std_int_type::create(types::std_int_type::kind::k_long_long,
				     false);
}


std::map<const std::string, const builtin_func::factory>
target_gcc::_register_builtin_funcs()
{
  using namespace klp::ccp::builtins::impl;

  auto sp_v_fac = builtin_func_simple_proto::factory(*this, false, mk_v);
  auto sp_v_i_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_v, mk_i));
  auto sp_v_f_pf_pf_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_v, mk_f, mk_pf, mk_pf));
  auto sp_v_d_pd_pd_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_v, mk_d, mk_pd, mk_pd));
  auto sp_v_ld_pld_pld_fac = (builtin_func_simple_proto::factory
			      (*this, false, mk_v, mk_ld, mk_pld, mk_pld));
  auto sp_v_ssz_pv_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_v, mk_ssz, mk_pv));
  auto sp_v_pv_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_v, mk_pv));
  auto sp_v_pv_i_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_v, mk_pv, mk_i));
  auto sp_v_pv_sz_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_v, mk_pv, mk_sz));
  auto sp_v_pv_pv_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_v, mk_pv, mk_pv));
  auto sp_v_pcv_pv_sz_fac =(builtin_func_simple_proto::factory
			    (*this, false, mk_v, mk_pcv, mk_pv, mk_sz));
  auto sp_v_pcvv_var_fac =(builtin_func_simple_proto::factory
			   (*this, true, mk_v, mk_pcvv));
  auto sp_v_pvv_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_v, mk_pvv));
  auto sp_v_pvv_i_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_v, mk_pvv, mk_i));
  auto sp_v_pvv_pv_i_fac =(builtin_func_simple_proto::factory
			   (*this, false, mk_v, mk_pvv, mk_pv, mk_i));
  auto sp_v_pvv_pv_pv_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_v, mk_pvv, mk_pv, mk_pv, mk_i));
  auto sp_v_pvv_var_fac =(builtin_func_simple_proto::factory
			  (*this, true, mk_v, mk_pvv));
  auto sp_v_pcvv_pv_i_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_v, mk_pcvv, mk_pv, mk_i));
  auto sp_v_val_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_v, _mk_val));
  auto sp_v_val_val_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_v, _mk_val, _mk_val));
  auto sp_v_val_var_fac = (builtin_func_simple_proto::factory
			   (*this, true, mk_v, _mk_val));
  auto sp_pv_pF_v_var__pv_sz_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_pv, mk_pF_v_var, mk_pv, mk_sz));
  auto sp_v_var_fac =builtin_func_simple_proto::factory(*this, true, mk_v);

  auto sp_i_fac = builtin_func_simple_proto::factory(*this, false, mk_i);
  auto sp_i_i_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_i, mk_i));
  auto sp_i_i_i_i_i_i_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_i, mk_i, mk_i, mk_i, mk_i, mk_i));
  auto sp_i_i_pv_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_i, mk_i, mk_pv));
  auto sp_i_i_pcc_val_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_i, mk_i, mk_pcc, _mk_val));
  auto sp_i_i_pcc_var_fac = (builtin_func_simple_proto::factory
			     (*this, true, mk_i, mk_i, mk_pcc));
  auto sp_i_u_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_i, mk_u));
  auto sp_i_l_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_i, mk_l));
  auto sp_i_ul_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_i, mk_ul));
  auto sp_i_ll_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_i, mk_ll));
  auto sp_i_ull_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_i, mk_ull));
  auto sp_i_imax_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_i, mk_imax));
  auto sp_i_umax_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_i, mk_umax));
  auto sp_i_wi_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_i, _mk_wi));
  auto sp_i_f_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_i, mk_f));
  auto sp_i_d_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_i, mk_d));
  auto sp_i_ld_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_i, mk_ld));
  auto sp_i_pv_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_i, mk_pv));
  auto sp_i_pv_i_pcc_val_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_i, mk_pv, mk_i, mk_pcc, _mk_val));
  auto sp_i_pv_i_pcc_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_i, mk_pv, mk_i, mk_pcc));
  auto sp_i_pv_pcc_val_fac = (builtin_func_simple_proto::factory
			      (*this, false, mk_i, mk_pv, mk_pcc, _mk_val));
  auto sp_i_pv_pcc_var_fac = (builtin_func_simple_proto::factory
			      (*this, true, mk_i, mk_pv, mk_pcc));
  auto sp_i_pcv_pcv_sz_fac = (builtin_func_simple_proto::factory
			      (*this, false, mk_i, mk_pcv, mk_pcv, mk_sz));
  auto sp_i_pc_i_sz_pcc_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_i, mk_pc, mk_i, mk_sz, mk_pcc));
  auto sp_i_pc_i_sz_pcc_val_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_i, mk_pc, mk_i, mk_sz, mk_pcc, _mk_val));
  auto sp_i_pc_sz_i_sz_pcc_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_i, mk_pc, mk_sz, mk_i, mk_sz, mk_pcc));
  auto sp_i_pc_sz_i_sz_pcc_val_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_i, mk_pc, mk_sz, mk_i, mk_sz, mk_pcc, _mk_val));
  auto sp_i_pc_sz_pcc_val_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_i, mk_pc, mk_sz, mk_pcc, _mk_val));
  auto sp_i_pc_sz_pcc_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_i, mk_pc, mk_sz, mk_pcc));
  auto sp_i_pc_pcc_val_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_i, mk_pc, mk_pcc, _mk_val));
  auto sp_i_pc_pcc_var_fac =(builtin_func_simple_proto::factory
			     (*this, true, mk_i, mk_pc, mk_pcc));
  auto sp_i_pcc_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_i, mk_pcc));
  auto sp_i_pcc_sz_pcc_var_fac =(builtin_func_simple_proto::factory
				 (*this, true, mk_i, mk_pcc, mk_sz, mk_pcc));
  auto sp_i_pcc_pv_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_i, mk_pcc, mk_pv));
  auto sp_i_pcc_pv_pcc_fac =(builtin_func_simple_proto::factory
			     (*this, false, mk_i, mk_pcc, mk_pv, mk_pcc));
  auto sp_i_pcc_pv_pcc_pv_pcc_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_i, mk_pcc, mk_pv, mk_pcc, mk_pv, mk_pcc));
  auto sp_i_pcc_pcc_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_i, mk_pcc, mk_pcc));
  auto sp_i_pcc_pcc_sz_fac =(builtin_func_simple_proto::factory
			     (*this, false, mk_i, mk_pcc, mk_pcc, mk_sz));
  auto sp_i_pcc_pcc_val_fac =(builtin_func_simple_proto::factory
			      (*this, false, mk_i, mk_pcc, mk_pcc, _mk_val));
  auto sp_i_pcc_pcc_var_fac = (builtin_func_simple_proto::factory
			       (*this, true, mk_i, mk_pcc, mk_pcc));
  auto sp_i_pcc_val_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_i, mk_pcc, _mk_val));
  auto sp_i_pcc_var_fac = (builtin_func_simple_proto::factory
			   (*this, true, mk_i, mk_pcc));
  auto sp_i_var_fac = (builtin_func_simple_proto::factory
		       (*this, true, mk_i));

  auto sp_u_fac = (builtin_func_simple_proto::factory
		   (*this, false, mk_u, mk_ll));

  auto sp_l_l_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_l, mk_l));
  auto sp_l_l_l_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_l, mk_l, mk_l));
  auto sp_l_f_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_l, mk_f));
  auto sp_l_d_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_l, mk_d));
  auto sp_l_ld_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_l, mk_ld));

  auto sp_ll_ll_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_ll, mk_ll));
  auto sp_ll_f_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_ll, mk_f));
  auto sp_ll_d_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_ll, mk_d));
  auto sp_ll_ld_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_ll, mk_ld));

  auto sp_b_sz_pcvv_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_b, mk_sz, mk_pcvv));
  auto sp_b_pvv_i_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_b, mk_pvv, mk_i));
  auto sp_b_pvv_pv_pv_b_i_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_b, mk_pvv, mk_pv, mk_pv, mk_b, mk_i, mk_i));
  auto sp_b_var_fac = builtin_func_simple_proto::factory(*this, true, mk_b);

  auto sp_imax_imax_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_imax, mk_imax));

  auto sp_u16_u16_fac = (builtin_func_simple_proto::factory
			 (*this, false, _mk_u16, _mk_u16));
  auto sp_u32_u32_fac = (builtin_func_simple_proto::factory
			 (*this, false, _mk_u32, _mk_u32));
  auto sp_u64_u64_fac = (builtin_func_simple_proto::factory
			 (*this, false, _mk_u64, _mk_u64));

  auto sp_wi_wi_fac = (builtin_func_simple_proto::factory
		       (*this, false, _mk_wi, _mk_wi));

  auto sp_sz_pv_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_sz, mk_pv));
  auto sp_sz_pcv_i_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_sz, mk_pcv, mk_i));
  auto sp_sz_pc_sz_pcc_pcv_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_sz, mk_pc, mk_sz, mk_pcc, mk_pcv));
  auto sp_sz_pcc_fac = (builtin_func_simple_proto::factory(
				  *this, false, mk_sz, mk_pcc));
  auto sp_sz_pcc_sz_sz_pv_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_sz, mk_pcc, mk_sz, mk_sz, mk_pv));
  auto sp_sz_pcc_pcc_fac = (builtin_func_simple_proto::factory
			    (*this, false, mk_sz, mk_pcc, mk_pcc));

  auto sp_ssz_pc_sz_pcc_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_ssz, mk_pc, mk_sz, mk_pcc));

  auto sp_pid_fac = (builtin_func_simple_proto::factory
		     (*this, false, _mk_pid, mk_pv));

  auto sp_f_fac = builtin_func_simple_proto::factory(*this, false, mk_f);
  auto sp_f_i_f_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_f, mk_i, mk_f));
  auto sp_f_f_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_f, mk_f));
  auto sp_f_f_i_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_f, mk_f, mk_i));
  auto sp_f_f_l_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_f, mk_f, mk_l));
  auto sp_f_f_f_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_f, mk_f, mk_f));
  auto sp_f_f_f_f_fac =(builtin_func_simple_proto::factory
			(*this, false, mk_f, mk_f, mk_f, mk_f));
  auto sp_f_f_f_pi_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_f, mk_f, mk_f, mk_pi));
  auto sp_f_f_pi_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_f, mk_f, mk_pi));
  auto sp_f_f_pf_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_f, mk_f, mk_pf));
  auto sp_f_fC_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_f, mk_fC));
  auto sp_f_pcc_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_f, mk_pcc));

  auto sp_d_fac = (builtin_func_simple_proto::factory
		   (*this, false, mk_d));
  auto sp_d_i_d_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_d, mk_i, mk_d));
  auto sp_d_d_fac = (builtin_func_simple_proto::factory
		     (*this, false, mk_d, mk_d));
  auto sp_d_d_i_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_d, mk_d, mk_i));
  auto sp_d_d_l_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_d, mk_d, mk_l));
  auto sp_d_d_d_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_d, mk_d, mk_d));
  auto sp_d_d_d_d_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_d, mk_d, mk_d, mk_d));
  auto sp_d_d_d_pi_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_d, mk_d, mk_d, mk_pi));
  auto sp_d_d_pi_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_d, mk_d, mk_pi));
  auto sp_d_d_pd_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_d, mk_d, mk_pd));
  auto sp_d_dC_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_d, mk_dC));
  auto sp_d_pcc_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_d, mk_pcc));

  auto sp_ld_fac = builtin_func_simple_proto::factory(*this, false, mk_ld);
  auto sp_ld_i_ld_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_ld, mk_i, mk_ld));
  auto sp_ld_ld_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_ld, mk_ld));
  auto sp_ld_ld_i_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_ld, mk_ld, mk_i));
  auto sp_ld_ld_l_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_ld, mk_ld, mk_l));
  auto sp_ld_ld_ld_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_ld, mk_ld, mk_ld));
  auto sp_ld_ld_ld_ld_fac =(builtin_func_simple_proto::factory
			    (*this, false, mk_ld, mk_ld, mk_ld, mk_ld));
  auto sp_ld_ld_ld_pi_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_ld, mk_ld, mk_ld, mk_pi));
  auto sp_ld_ld_pi_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_ld, mk_ld, mk_pi));
  auto sp_ld_ld_pld_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_ld, mk_ld, mk_pld));
  auto sp_ld_ldC_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_ld, mk_ldC));
  auto sp_ld_pcc_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_ld, mk_pcc));

  auto sp_fC_f_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_fC, mk_f));
  auto sp_fC_fC_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_fC, mk_fC));
  auto sp_fC_fC_fC_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_fC, mk_fC, mk_fC));

  auto sp_dC_d_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_dC, mk_d));
  auto sp_dC_dC_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_dC, mk_dC));
  auto sp_dC_dC_dC_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_dC, mk_dC, mk_dC));

  auto sp_ldC_ld_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_ldC, mk_ld));
  auto sp_ldC_ldC_fac = (builtin_func_simple_proto::factory
			 (*this, false, mk_ldC, mk_ldC));
  auto sp_ldC_ldC_ldC_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_ldC, mk_ldC, mk_ldC));

  auto sp_pv_fac = builtin_func_simple_proto::factory(*this, false, mk_pv);
  auto sp_pv_u_fac = (builtin_func_simple_proto::factory
		      (*this, false, mk_pv, mk_u));
  auto sp_pv_sz_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_pv, mk_sz));
  auto sp_pv_sz_sz_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_pv, mk_sz, mk_sz));
  auto sp_pv_pv_fac = (builtin_func_simple_proto::factory
		       (*this, false, mk_pv, mk_pv));
  auto sp_pv_pv_i_sz_fac = (builtin_func_simple_proto::factory
			    (*this, false, mk_pv, mk_pv, mk_i, mk_sz));
  auto sp_pv_pv_i_sz_sz_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_pv, mk_pv, mk_i, mk_sz, mk_sz));
  auto sp_pv_pv_sz_fac = (builtin_func_simple_proto::factory
			  (*this, false, mk_pv, mk_pv, mk_sz));
  auto sp_pv_pv_pcv_sz_fac = (builtin_func_simple_proto::factory
			      (*this, false, mk_pv, mk_pv, mk_pcv, mk_sz));
  auto sp_pv_pv_pcv_sz_sz_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_pv, mk_pv, mk_pcv, mk_sz, mk_sz));
  auto sp_pv_pcv_i_sz_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_pv, mk_pcv, mk_i, mk_sz));
  auto sp_pv_pcv_sz_var_fac = (builtin_func_simple_proto::factory
			       (*this, true, mk_pv, mk_pv, mk_sz));
  auto sp_pv_var_fac = (builtin_func_simple_proto::factory
			(*this, true, mk_pv));

  auto sp_pc_pc_pcc_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_pc, mk_pc, mk_pcc));
  auto sp_pc_pc_pcc_sz_fac = (builtin_func_simple_proto::factory
			      (*this, false, mk_pc, mk_pc, mk_pcc, mk_sz));
  auto sp_pc_pcc_fac = (builtin_func_simple_proto::factory
			(*this, false, mk_pc, mk_pcc));
  auto sp_pc_pcc_i_fac =(builtin_func_simple_proto::factory
			 (*this, false, mk_pc, mk_pcc, mk_i));
  auto sp_pc_pcc_sz_fac = (builtin_func_simple_proto::factory
			   (*this, false, mk_pc, mk_pcc, mk_sz));
  auto sp_pc_pcc_pcc_fac = (builtin_func_simple_proto::factory
			    (*this, false, mk_pc, mk_pcc, mk_pcc));
  auto sp_pc_pcc_pcc_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_pc, mk_pcc, mk_pcc, mk_i));
  auto sp_pc_pc_pcc_sz_sz_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_pc, mk_pc, mk_pcc, mk_sz, mk_sz));

  auto sp_pcc_fac = builtin_func_simple_proto::factory(*this, false, mk_pcc);

  auto sp_i8_pvv_i8_fac = (builtin_func_simple_proto::factory
			   (*this, false, _mk_i8, mk_pvv, _mk_i8));
  auto sp_i16_pvv_i16_fac = (builtin_func_simple_proto::factory
			     (*this, false, _mk_i16, mk_pvv, _mk_i16));
  auto sp_i32_pvv_i32_fac = (builtin_func_simple_proto::factory
			     (*this, false, _mk_i32, mk_pvv, _mk_i32));
  auto sp_i64_pvv_i64_fac = (builtin_func_simple_proto::factory
			     (*this, false, _mk_i64, mk_pvv, _mk_i64));
  auto sp_i128_pvv_i128_fac =(builtin_func_simple_proto::factory
			      (*this, false, _mk_i128, mk_pvv, _mk_i128));

  auto sp_i8_pvv_i8_var_fac = (builtin_func_simple_proto::factory
			       (*this, true, _mk_i8, mk_pvv, _mk_i8));
  auto sp_i16_pvv_i16_var_fac = (builtin_func_simple_proto::factory
				 (*this, true, _mk_i16, mk_pvv, _mk_i16));
  auto sp_i32_pvv_i32_var_fac = (builtin_func_simple_proto::factory
				 (*this, true, _mk_i32, mk_pvv, _mk_i32));
  auto sp_i64_pvv_i64_var_fac = (builtin_func_simple_proto::factory
				 (*this, true, _mk_i64, mk_pvv, _mk_i64));
  auto sp_i128_pvv_i128_var_fac = (builtin_func_simple_proto::factory
				   (*this, true, _mk_i128, mk_pvv, _mk_i128));
  auto pio0_iN_pvv_iN_var_fac =
    std::bind(_builtin_func_pi_overload_fac<0>::create,
	      sp_i8_pvv_i8_var_fac,
	      sp_i16_pvv_i16_var_fac,
	      sp_i32_pvv_i32_var_fac,
	      sp_i64_pvv_i64_var_fac,
	      sp_i128_pvv_i128_var_fac);

  auto sp_b_pvv_i8_i8_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_b, mk_pvv, _mk_i8, _mk_i8));
  auto sp_b_pvv_i16_i16_fac = (builtin_func_simple_proto::factory
			       (*this, false, mk_b, mk_pvv, _mk_i16, _mk_i16));
  auto sp_b_pvv_i32_i32_fac = (builtin_func_simple_proto::factory
			       (*this, false, mk_b, mk_pvv, _mk_i32, _mk_i32));
  auto sp_b_pvv_i64_i64_fac = (builtin_func_simple_proto::factory
			       (*this, false, mk_b, mk_pvv, _mk_i64, _mk_i64));
  auto sp_b_pvv_i128_i128_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_b, mk_pvv, _mk_i128, _mk_i128));

  auto sp_b_pvv_i8_i8_var_fac = (builtin_func_simple_proto::factory
				 (*this, true, mk_b, mk_pvv, _mk_i8, _mk_i8));
  auto sp_b_pvv_i16_i16_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_b, mk_pvv, _mk_i16, _mk_i16));
  auto sp_b_pvv_i32_i32_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_b, mk_pvv, _mk_i32, _mk_i32));
  auto sp_b_pvv_i64_i64_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_b, mk_pvv, _mk_i64, _mk_i64));
  auto sp_b_pvv_i128_i128_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, mk_b, mk_pvv, _mk_i128, _mk_i128));
  auto pio0_b_pvv_iN_iN_var_fac =
    std::bind(_builtin_func_pi_overload_fac<0>::create,
	      sp_b_pvv_i8_i8_var_fac,
	      sp_b_pvv_i16_i16_var_fac,
	      sp_b_pvv_i32_i32_var_fac,
	      sp_b_pvv_i64_i64_var_fac,
	      sp_b_pvv_i128_i128_var_fac);

  auto sp_i8_pvv_i8_i8_fac = (builtin_func_simple_proto::factory
			      (*this, false, _mk_i8, mk_pvv, _mk_i8, _mk_i8));
  auto sp_i16_pvv_i16_i16_fac =
    (builtin_func_simple_proto::factory
     (*this, false, _mk_i16, mk_pvv, _mk_i16, _mk_i16));
  auto sp_i32_pvv_i32_i32_fac =
    (builtin_func_simple_proto::factory
     (*this, false, _mk_i32, mk_pvv, _mk_i32, _mk_i32));
  auto sp_i64_pvv_i64_i64_fac =
    (builtin_func_simple_proto::factory
     (*this, false, _mk_i64, mk_pvv, _mk_i64, _mk_i64));
  auto sp_i128_pvv_i128_i128_fac =
    (builtin_func_simple_proto::factory
     (*this, false, _mk_i128, mk_pvv, _mk_i128, _mk_i128));

  auto sp_i8_pvv_i8_i8_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, _mk_i8, mk_pvv, _mk_i8, _mk_i8));
  auto sp_i16_pvv_i16_i16_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, _mk_i16, mk_pvv, _mk_i16, _mk_i16));
  auto sp_i32_pvv_i32_i32_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, _mk_i32, mk_pvv, _mk_i32, _mk_i32));
  auto sp_i64_pvv_i64_i64_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, _mk_i64, mk_pvv, _mk_i64, _mk_i64));
  auto sp_i128_pvv_i128_i128_var_fac =
    (builtin_func_simple_proto::factory
     (*this, true, _mk_i128, mk_pvv, _mk_i128, _mk_i128));
  auto pio0_iN_pvv_iN_iN_var_fac =
    std::bind(_builtin_func_pi_overload_fac<0>::create,
	      sp_i8_pvv_i8_i8_var_fac,
	      sp_i16_pvv_i16_i16_var_fac,
	      sp_i32_pvv_i32_i32_var_fac,
	      sp_i64_pvv_i64_i64_var_fac,
	      sp_i128_pvv_i128_i128_var_fac);

  auto pio0_v_pvv_var_fac =
    std::bind(_builtin_func_pi_overload_fac<0>::create,
	      sp_v_pvv_var_fac,
	      sp_v_pvv_var_fac,
	      sp_v_pvv_var_fac,
	      sp_v_pvv_var_fac,
	      sp_v_pvv_var_fac);

  auto sp_i8_pvv_i8_i_fac = (builtin_func_simple_proto::factory
			     (*this, false, _mk_i8, mk_pvv, _mk_i8, mk_i));
  auto sp_i16_pvv_i16_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, _mk_i16, mk_pvv, _mk_i16, mk_i));
  auto sp_i32_pvv_i32_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, _mk_i32,  mk_pvv, _mk_i32, mk_i));
  auto sp_i64_pvv_i64_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, _mk_i64, mk_pvv, _mk_i64, mk_i));
  auto sp_i128_pvv_i128_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, _mk_i128, mk_pvv, _mk_i128, mk_i));
  auto pio0_iN_pvv_iN_i_fac =
    std::bind(_builtin_func_pi_overload_fac<0>::create,
	      sp_i8_pvv_i8_i_fac,
	      sp_i16_pvv_i16_i_fac,
	      sp_i32_pvv_i32_i_fac,
	      sp_i64_pvv_i64_i_fac,
	      sp_i128_pvv_i128_i_fac);

  auto sp_i8_pcvv_i_fac = (builtin_func_simple_proto::factory
			   (*this, false, _mk_i8, mk_pcvv, mk_i));
  auto sp_i16_pcvv_i_fac = (builtin_func_simple_proto::factory
			    (*this, false, _mk_i16, mk_pcvv, mk_i));
  auto sp_i32_pcvv_i_fac = (builtin_func_simple_proto::factory
			    (*this, false, _mk_i32, mk_pcvv, mk_i));
  auto sp_i64_pcvv_i_fac = (builtin_func_simple_proto::factory
			    (*this, false, _mk_i64, mk_pcvv, mk_i));
  auto sp_i128_pcvv_i_fac = (builtin_func_simple_proto::factory
			     (*this, false, _mk_i128, mk_pcvv, mk_i));
  auto pio0_iN_pcvv_i_fac =
    std::bind(_builtin_func_pi_overload_fac<0>::create,
	      sp_i8_pcvv_i_fac,
	      sp_i16_pcvv_i_fac,
	      sp_i32_pcvv_i_fac,
	      sp_i64_pcvv_i_fac,
	      sp_i128_pcvv_i_fac);

  auto sp_b_pvv_pv_i8_b_i_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_b, mk_pvv, mk_pv, _mk_i8, mk_b, mk_i, mk_i));
  auto sp_b_pvv_pv_i16_b_i_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_b, mk_pvv, mk_pv, _mk_i16, mk_b, mk_i, mk_i));
  auto sp_b_pvv_pv_i32_b_i_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_b, mk_pvv, mk_pv, _mk_i32, mk_b, mk_i, mk_i));
  auto sp_b_pvv_pv_i64_b_i_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_b, mk_pvv, mk_pv, _mk_i64, mk_b, mk_i, mk_i));
  auto sp_b_pvv_pv_i128_b_i_i_fac =
    (builtin_func_simple_proto::factory
     (*this, false, mk_b, mk_pvv, mk_pv, _mk_i128, mk_b, mk_i, mk_i));
  auto pio0_b_pvv_pv_iN_b_i_i_fac =
    std::bind(_builtin_func_pi_overload_fac<0>::create,
	      sp_b_pvv_pv_i8_b_i_i_fac,
	      sp_b_pvv_pv_i16_b_i_i_fac,
	      sp_b_pvv_pv_i32_b_i_i_fac,
	      sp_b_pvv_pv_i64_b_i_i_fac,
	      sp_b_pvv_pv_i128_b_i_i_fac);

  auto sp_v_pvv_i8_i_fac = (builtin_func_simple_proto::factory
			    (*this, false, mk_v, mk_pvv, _mk_i8, mk_i));
  auto sp_v_pvv_i16_i_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_v, mk_pvv, _mk_i16, mk_i));
  auto sp_v_pvv_i32_i_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_v,  mk_pvv, _mk_i32, mk_i));
  auto sp_v_pvv_i64_i_fac = (builtin_func_simple_proto::factory
			     (*this, false, mk_v, mk_pvv, _mk_i64, mk_i));
  auto sp_v_pvv_i128_i_fac = (builtin_func_simple_proto::factory
			      (*this, false, mk_v, mk_pvv, _mk_i128, mk_i));
  auto pio0_v_pvv_iN_i_fac =
    std::bind(_builtin_func_pi_overload_fac<0>::create,
	      sp_v_pvv_i8_i_fac,
	      sp_v_pvv_i16_i_fac,
	      sp_v_pvv_i32_i_fac,
	      sp_v_pvv_i64_i_fac,
	      sp_v_pvv_i128_i_fac);


  static std::map<const std::string,
		  const builtin_func::factory > m = {
    // Category: math builtins.
    { "__builtin_acos", sp_d_d_fac },
    { "__builtin_acosf", sp_f_f_fac },
    { "__builtin_acosl", sp_ld_ld_fac },
    { "__builtin_acosh", sp_d_d_fac },
    { "__builtin_acoshf", sp_f_f_fac },
    { "__builtin_acoshl", sp_ld_ld_fac },
    { "__builtin_asin", sp_d_d_fac },
    { "__builtin_asinf", sp_f_f_fac },
    { "__builtin_asinl", sp_ld_ld_fac },
    { "__builtin_asinh", sp_d_d_fac },
    { "__builtin_asinhf", sp_f_f_fac },
    { "__builtin_asinhl", sp_ld_ld_fac },
    { "__builtin_atan", sp_d_d_fac },
    { "__builtin_atanf", sp_f_f_fac },
    { "__builtin_atanl", sp_ld_ld_fac },
    { "__builtin_atanh", sp_d_d_fac },
    { "__builtin_atanhf", sp_f_f_fac },
    { "__builtin_atanhl", sp_ld_ld_fac },
    { "__builtin_atan2", sp_d_d_d_fac },
    { "__builtin_atan2f", sp_f_f_f_fac },
    { "__builtin_atan2l", sp_ld_ld_ld_fac },
    { "__builtin_cbrt", sp_d_d_fac },
    { "__builtin_cbrtf", sp_f_f_fac },
    { "__builtin_cbrtl", sp_ld_ld_fac },
    { "__builtin_ceil", sp_d_d_fac },
    { "__builtin_ceilf", sp_f_f_fac },
    { "__builtin_ceill", sp_ld_ld_fac },
    { "__builtin_copysign", sp_d_d_d_fac },
    { "__builtin_copysignf", sp_f_f_f_fac },
    { "__builtin_copysignl", sp_ld_ld_ld_fac },
    { "__builtin_cos", sp_d_d_fac },
    { "__builtin_cosf", sp_f_f_fac },
    { "__builtin_cosl", sp_ld_ld_fac },
    { "__builtin_cosh", sp_d_d_fac },
    { "__builtin_coshf", sp_f_f_fac },
    { "__builtin_coshl", sp_ld_ld_fac },
    { "__builtin_drem", sp_d_d_d_fac },
    { "__builtin_dremf", sp_f_f_f_fac },
    { "__builtin_dreml", sp_ld_ld_ld_fac },
    { "__builtin_erf", sp_d_d_fac },
    { "__builtin_erff", sp_f_f_fac },
    { "__builtin_erfl", sp_ld_ld_fac },
    { "__builtin_erfc", sp_d_d_fac },
    { "__builtin_erfcf", sp_f_f_fac },
    { "__builtin_erfcl", sp_ld_ld_fac },
    { "__builtin_exp", sp_d_d_fac },
    { "__builtin_expf", sp_f_f_fac },
    { "__builtin_expl", sp_ld_ld_fac },
    { "__builtin_exp10", sp_d_d_fac },
    { "__builtin_exp10f", sp_f_f_fac },
    { "__builtin_exp10l", sp_ld_ld_fac },
    { "__builtin_exp2", sp_d_d_fac },
    { "__builtin_exp2f", sp_f_f_fac },
    { "__builtin_exp2l", sp_ld_ld_fac },
    { "__builtin_expm1", sp_d_d_fac },
    { "__builtin_expm1f", sp_f_f_fac },
    { "__builtin_expm1l", sp_ld_ld_fac },
    { "__builtin_fabs", sp_d_d_fac },
    { "__builtin_fabsf", sp_f_f_fac },
    { "__builtin_fabsl", sp_ld_ld_fac },
    { "__builtin_fdim", sp_d_d_d_fac },
    { "__builtin_fdimf", sp_f_f_f_fac },
    { "__builtin_fdiml", sp_ld_ld_ld_fac },
    { "__builtin_floor", sp_d_d_fac },
    { "__builtin_floor", sp_d_d_fac },
    { "__builtin_floorf", sp_f_f_fac },
    { "__builtin_floorl", sp_ld_ld_fac },
    { "__builtin_fma", sp_d_d_d_d_fac },
    { "__builtin_fmaf", sp_f_f_f_f_fac },
    { "__builtin_fmal", sp_ld_ld_ld_ld_fac },
    { "__builtin_fmax", sp_d_d_d_fac },
    { "__builtin_fmaxf", sp_f_f_f_fac },
    { "__builtin_fmaxl", sp_ld_ld_ld_fac },
    { "__builtin_fmin", sp_d_d_d_fac },
    { "__builtin_fminf", sp_f_f_f_fac },
    { "__builtin_fminl", sp_ld_ld_ld_fac },
    { "__builtin_fmod", sp_d_d_d_fac },
    { "__builtin_fmodf", sp_f_f_f_fac },
    { "__builtin_fmodl", sp_ld_ld_ld_fac },
    { "__builtin_frexp", sp_d_d_pi_fac },
    { "__builtin_frexpf", sp_f_f_pi_fac },
    { "__builtin_frexpl", sp_ld_ld_pi_fac },
    { "__builtin_gamma", sp_d_d_fac },
    { "__builtin_gammaf", sp_f_f_fac },
    { "__builtin_gammal", sp_ld_ld_fac },
    { "__builtin_gamma_r", sp_d_d_pi_fac },
    { "__builtin_gammaf_r", sp_f_f_pi_fac },
    { "__builtin_gammal_r", sp_ld_ld_pi_fac },
    { "__builtin_huge_val", sp_d_fac },
    { "__builtin_huge_valf", sp_f_fac },
    { "__builtin_huge_vall", sp_ld_fac },
    { "__builtin_hypot", sp_d_d_d_fac },
    { "__builtin_hypotf", sp_f_f_f_fac },
    { "__builtin_hypotl", sp_ld_ld_ld_fac },
    { "__builtin_iceil", sp_i_d_fac },
    { "__builtin_iceilf", sp_i_f_fac },
    { "__builtin_iceild", sp_i_ld_fac },
    { "__builtin_ifloor", sp_i_d_fac },
    { "__builtin_ifloorf", sp_i_f_fac },
    { "__builtin_ifloorl", sp_i_ld_fac },
    { "__builtin_ilogb", sp_i_d_fac },
    { "__builtin_ilogbf", sp_i_f_fac },
    { "__builtin_ilogbl", sp_i_ld_fac },
    { "__builtin_inf", sp_d_fac },
    { "__builtin_inff", sp_f_fac },
    { "__builtin_infl", sp_ld_fac },
    { "__builtin_irint", sp_i_d_fac },
    { "__builtin_irintf", sp_i_f_fac },
    { "__builtin_irintl", sp_i_ld_fac },
    { "__builtin_iround", sp_i_d_fac },
    { "__builtin_iroundf", sp_i_f_fac },
    { "__builtin_iroundl", sp_i_ld_fac },
    { "__builtin_j0", sp_d_d_fac },
    { "__builtin_j0f", sp_f_f_fac },
    { "__builtin_j0l", sp_ld_ld_fac },
    { "__builtin_j1", sp_d_d_fac },
    { "__builtin_j1f", sp_f_f_fac },
    { "__builtin_j1l", sp_ld_ld_fac },
    { "__builtin_jn", sp_d_i_d_fac },
    { "__builtin_jnf", sp_f_i_f_fac },
    { "__builtin_jnl", sp_ld_i_ld_fac },
    { "__builtin_lceil", sp_l_d_fac },
    { "__builtin_lceilf", sp_l_f_fac },
    { "__builtin_lceill", sp_l_ld_fac },
    { "__builtin_ldexp", sp_d_d_i_fac },
    { "__builtin_ldexpf", sp_f_f_i_fac },
    { "__builtin_ldexpl", sp_ld_ld_i_fac },
    { "__builtin_lfloor", sp_l_d_fac },
    { "__builtin_lfloorf", sp_l_f_fac },
    { "__builtin_lfloorl", sp_l_ld_fac },
    { "__builtin_lgamma", sp_d_d_fac },
    { "__builtin_lgammaf", sp_f_f_fac },
    { "__builtin_lgammal", sp_ld_ld_fac },
    { "__builtin_lgamma_r", sp_d_d_pi_fac },
    { "__builtin_lgammaf_r", sp_f_f_pi_fac },
    { "__builtin_lgammal_r", sp_ld_ld_pi_fac },
    { "__builtin_llceil", sp_ll_d_fac },
    { "__builtin_llceilf", sp_ll_f_fac },
    { "__builtin_llceill", sp_ll_ld_fac },
    { "__builtin_llfloor", sp_ll_d_fac },
    { "__builtin_llfloorf", sp_ll_f_fac },
    { "__builtin_llfloorl", sp_ll_ld_fac },
    { "__builtin_llrint", sp_ll_d_fac },
    { "__builtin_llrintf", sp_ll_f_fac },
    { "__builtin_llrintl", sp_ll_ld_fac },
    { "__builtin_llround", sp_ll_d_fac },
    { "__builtin_llroundf", sp_ll_f_fac },
    { "__builtin_llroundl", sp_ll_ld_fac },
    { "__builtin_log", sp_d_d_fac },
    { "__builtin_logf", sp_f_f_fac },
    { "__builtin_logl", sp_ld_ld_fac },
    { "__builtin_log10", sp_d_d_fac },
    { "__builtin_log10f", sp_f_f_fac },
    { "__builtin_log10l", sp_ld_ld_fac },
    { "__builtin_log1p", sp_d_d_fac },
    { "__builtin_log1pf", sp_f_f_fac },
    { "__builtin_log1pl", sp_ld_ld_fac },
    { "__builtin_log2", sp_d_d_fac },
    { "__builtin_log2f", sp_f_f_fac },
    { "__builtin_log2l", sp_ld_ld_fac },
    { "__builtin_logb", sp_d_d_fac },
    { "__builtin_logbf", sp_f_f_fac },
    { "__builtin_logbl", sp_ld_ld_fac },
    { "__builtin_lrint", sp_l_d_fac },
    { "__builtin_lrintf", sp_l_f_fac },
    { "__builtin_lrintl", sp_l_ld_fac },
    { "__builtin_lround", sp_l_d_fac },
    { "__builtin_lroundf", sp_l_f_fac },
    { "__builtin_lroundl", sp_l_ld_fac },
    { "__builtin_modf", sp_d_d_pd_fac },
    { "__builtin_modff", sp_f_f_pf_fac },
    { "__builtin_modfl", sp_ld_ld_pld_fac },
    { "__builtin_nan", sp_d_pcc_fac },
    { "__builtin_nanf", sp_f_pcc_fac },
    { "__builtin_nanl", sp_ld_pcc_fac },
    { "__builtin_nans", sp_d_pcc_fac },
    { "__builtin_nansf", sp_f_pcc_fac },
    { "__builtin_nansl", sp_ld_pcc_fac },
    { "__builtin_nearbyint", sp_d_d_fac },
    { "__builtin_nearbyintf", sp_f_f_fac },
    { "__builtin_nearbyintl", sp_ld_ld_fac },
    { "__builtin_nextafter", sp_d_d_d_fac },
    { "__builtin_nextafterf", sp_f_f_f_fac },
    { "__builtin_nextafterl", sp_ld_ld_ld_fac },
    { "__builtin_nexttoward", sp_d_d_d_fac },
    { "__builtin_nexttowardf", sp_f_f_f_fac },
    { "__builtin_nexttowardl", sp_ld_ld_ld_fac },
    { "__builtin_pow", sp_d_d_d_fac },
    { "__builtin_powf", sp_f_f_f_fac },
    { "__builtin_powl", sp_ld_ld_ld_fac },
    { "__builtin_pow10", sp_d_d_fac },
    { "__builtin_pow10f", sp_f_f_fac },
    { "__builtin_pow10l", sp_ld_ld_fac },
    { "__builtin_powi", sp_d_d_i_fac },
    { "__builtin_powif", sp_f_f_i_fac },
    { "__builtin_powil", sp_ld_ld_i_fac },
    { "__builtin_remainder", sp_d_d_d_fac },
    { "__builtin_remainderf", sp_f_f_f_fac },
    { "__builtin_remainderl", sp_ld_ld_ld_fac },
    { "__builtin_remquo", sp_d_d_d_pi_fac },
    { "__builtin_remquof", sp_f_f_f_pi_fac },
    { "__builtin_remquol", sp_ld_ld_ld_pi_fac },
    { "__builtin_rint", sp_d_d_fac },
    { "__builtin_rintf", sp_f_f_fac },
    { "__builtin_rintl", sp_ld_ld_fac },
    { "__builtin_round", sp_d_d_fac },
    { "__builtin_roundf", sp_f_f_fac },
    { "__builtin_roundl", sp_ld_ld_fac },
    { "__builtin_scalb", sp_d_d_d_fac },
    { "__builtin_scalbf", sp_f_f_f_fac },
    { "__builtin_scalbl", sp_ld_ld_ld_fac },
    { "__builtin_scalbln", sp_d_d_l_fac },
    { "__builtin_scalblnf", sp_f_f_l_fac },
    { "__builtin_scalblnl", sp_ld_ld_l_fac },
    { "__builtin_scalbn", sp_d_d_i_fac },
    { "__builtin_scalbnf", sp_f_f_i_fac },
    { "__builtin_scalbnl", sp_ld_ld_i_fac },
    { "__builtin_signbit", sp_i_d_fac },
    { "__builtin_signbitf", sp_i_f_fac },
    { "__builtin_signbitl", sp_i_ld_fac },
    { "__builtin_significand", sp_d_d_fac },
    { "__builtin_significandf", sp_f_f_fac },
    { "__builtin_significandl", sp_ld_ld_fac },
    { "__builtin_sin", sp_d_d_fac },
    { "__builtin_sinf", sp_f_f_fac },
    { "__builtin_sinl", sp_ld_ld_fac },
    { "__builtin_sincos", sp_v_d_pd_pd_fac },
    { "__builtin_sincosf", sp_v_f_pf_pf_fac },
    { "__builtin_sincosl", sp_v_ld_pld_pld_fac },
    { "__builtin_sinh", sp_d_d_fac },
    { "__builtin_sinhf", sp_f_f_fac },
    { "__builtin_sinhl", sp_ld_ld_fac },
    { "__builtin_sqrt", sp_d_d_fac },
    { "__builtin_sqrtf", sp_f_f_fac },
    { "__builtin_sqrtl", sp_ld_ld_fac },
    { "__builtin_tan", sp_d_d_fac },
    { "__builtin_tanf", sp_f_f_fac },
    { "__builtin_tanl", sp_ld_ld_fac },
    { "__builtin_tanh", sp_d_d_fac },
    { "__builtin_tanhf", sp_f_f_fac },
    { "__builtin_tanhl", sp_ld_ld_fac },
    { "__builtin_tgamma", sp_d_d_fac },
    { "__builtin_tgammaf", sp_f_f_fac },
    { "__builtin_tgammal", sp_ld_ld_fac },
    { "__builtin_trunc", sp_d_d_fac },
    { "__builtin_truncf", sp_f_f_fac },
    { "__builtin_truncl", sp_ld_ld_fac },
    { "__builtin_y0", sp_d_d_fac },
    { "__builtin_y0f", sp_f_f_fac },
    { "__builtin_y0l", sp_ld_ld_fac },
    { "__builtin_y1", sp_d_d_fac },
    { "__builtin_y1f", sp_f_f_fac },
    { "__builtin_y1l", sp_ld_ld_fac },
    { "__builtin_yn", sp_d_i_d_fac },
    { "__builtin_ynf", sp_f_i_f_fac },
    { "__builtin_ynl", sp_ld_i_ld_fac },

    // Category: _Complex math builtins.
    { "__builtin_cabs", sp_d_dC_fac },
    { "__builtin_cabsf", sp_f_fC_fac },
    { "__builtin_cabsl", sp_ld_ldC_fac },
    { "__builtin_cacos", sp_dC_dC_fac },
    { "__builtin_cacosf", sp_fC_fC_fac },
    { "__builtin_cacosl", sp_ldC_ldC_fac },
    { "__builtin_cacosh", sp_dC_dC_fac },
    { "__builtin_cacoshf", sp_fC_fC_fac },
    { "__builtin_cacoshl", sp_ldC_ldC_fac },
    { "__builtin_carg", sp_d_dC_fac },
    { "__builtin_cargf", sp_f_fC_fac },
    { "__builtin_cargl", sp_ld_ldC_fac },
    { "__builtin_casin", sp_dC_dC_fac },
    { "__builtin_casinf", sp_fC_fC_fac },
    { "__builtin_casinl", sp_ldC_ldC_fac },
    { "__builtin_casinh", sp_dC_dC_fac },
    { "__builtin_casinhf", sp_fC_fC_fac },
    { "__builtin_casinhl", sp_ldC_ldC_fac },
    { "__builtin_catan", sp_dC_dC_fac },
    { "__builtin_catanf", sp_fC_fC_fac },
    { "__builtin_catanl", sp_ldC_ldC_fac },
    { "__builtin_catanh", sp_dC_dC_fac },
    { "__builtin_catanhf", sp_fC_fC_fac },
    { "__builtin_catanhl", sp_ldC_ldC_fac },
    { "__builtin_ccos", sp_dC_dC_fac },
    { "__builtin_ccosf", sp_fC_fC_fac },
    { "__builtin_ccosl", sp_ldC_ldC_fac },
    { "__builtin_ccosh", sp_dC_dC_fac },
    { "__builtin_ccoshf", sp_fC_fC_fac },
    { "__builtin_ccoshl", sp_ldC_ldC_fac },
    { "__builtin_cexp", sp_dC_dC_fac },
    { "__builtin_cexpf", sp_fC_fC_fac },
    { "__builtin_cexpl", sp_ldC_ldC_fac },
    { "__builtin_cexpi", sp_dC_d_fac },
    { "__builtin_cexpif", sp_fC_f_fac },
    { "__builtin_cexpil", sp_ldC_ld_fac },
    { "__builtin_cimag", sp_d_dC_fac },
    { "__builtin_cimagf", sp_f_fC_fac },
    { "__builtin_cimagl", sp_ld_ldC_fac },
    { "__builtin_clog", sp_dC_dC_fac },
    { "__builtin_clogf", sp_fC_fC_fac },
    { "__builtin_clogl", sp_ldC_ldC_fac },
    { "__builtin_clog10", sp_dC_dC_fac },
    { "__builtin_clog10f", sp_fC_fC_fac },
    { "__builtin_clog10l", sp_ldC_ldC_fac },
    { "__builtin_conj", sp_dC_dC_fac },
    { "__builtin_conjf", sp_fC_fC_fac },
    { "__builtin_conjl", sp_ldC_ldC_fac },
    { "__builtin_cpow", sp_dC_dC_dC_fac },
    { "__builtin_cpowf", sp_fC_fC_fC_fac },
    { "__builtin_cpowl", sp_ldC_ldC_ldC_fac },
    { "__builtin_cproj", sp_dC_dC_fac },
    { "__builtin_cprojf", sp_fC_fC_fac },
    { "__builtin_cprojl", sp_ldC_ldC_fac },
    { "__builtin_creal", sp_d_dC_fac },
    { "__builtin_crealf", sp_f_fC_fac },
    { "__builtin_creall", sp_ld_ldC_fac },
    { "__builtin_csin", sp_dC_dC_fac },
    { "__builtin_csinf", sp_fC_fC_fac },
    { "__builtin_csinl", sp_ldC_ldC_fac },
    { "__builtin_csinh", sp_dC_dC_fac },
    { "__builtin_csinhf", sp_fC_fC_fac },
    { "__builtin_csinhl", sp_ldC_ldC_fac },
    { "__builtin_csqrt", sp_dC_dC_fac },
    { "__builtin_csqrtf", sp_fC_fC_fac },
    { "__builtin_csqrtl", sp_ldC_ldC_fac },
    { "__builtin_ctan", sp_dC_dC_fac },
    { "__builtin_ctanf", sp_fC_fC_fac },
    { "__builtin_ctanl", sp_ldC_ldC_fac },
    { "__builtin_ctanh", sp_dC_dC_fac },
    { "__builtin_ctanhf", sp_fC_fC_fac },
    { "__builtin_ctanhl", sp_ldC_ldC_fac },

    // Category string/memory builtins.
    { "__builtin_bcmp", sp_i_pcv_pcv_sz_fac },
    { "__builtin_bcopy", sp_v_pcv_pv_sz_fac },
    { "__builtin_bzero", sp_v_pv_sz_fac },
    { "__builtin_index", sp_pc_pcc_i_fac },
    { "__builtin_memchr", sp_pv_pcv_i_sz_fac },
    { "__builtin_memcmp", sp_i_pcv_pcv_sz_fac },
    { "__builtin_memcpy", sp_pv_pv_pcv_sz_fac },
    { "__builtin_memmove", sp_pv_pv_pcv_sz_fac },
    { "__builtin_mempcpy", sp_pv_pv_pcv_sz_fac },
    { "__builtin_memset", sp_pv_pv_i_sz_fac },
    { "__builtin_rindex", sp_pc_pcc_i_fac },
    { "__builtin_stpcpy", sp_pc_pc_pcc_fac },
    { "__builtin_stpncpy", sp_pc_pc_pcc_sz_fac },
    { "__builtin_strcasecmp", sp_i_pcc_pcc_fac },
    { "__builtin_strcat", sp_pc_pc_pcc_fac },
    { "__builtin_strchr", sp_pc_pcc_i_fac },
    { "__builtin_strcmp", sp_i_pcc_pcc_fac },
    { "__builtin_strcpy", sp_pc_pc_pcc_fac },
    { "__builtin_strcspn", sp_sz_pcc_pcc_fac },
    { "__builtin_strdup", sp_pc_pcc_fac },
    { "__builtin_strndup", sp_pc_pcc_sz_fac },
    { "__builtin_strlen", sp_sz_pcc_fac },
    { "__builtin_strncasecmp", sp_i_pcc_pcc_sz_fac },
    { "__builtin_strncat", sp_pc_pc_pcc_sz_fac },
    { "__builtin_strncmp", sp_i_pcc_pcc_sz_fac },
    { "__builtin_strncpy", sp_pc_pc_pcc_sz_fac },
    { "__builtin_strpbrk", sp_pc_pcc_pcc_fac },
    { "__builtin_strrchr", sp_pc_pcc_i_fac },
    { "__builtin_strspn", sp_sz_pcc_pcc_fac },
    { "__builtin_strstr", sp_pc_pcc_pcc_fac },

    // Category: stdio builtins.
    { "__builtin_fprintf", sp_i_pv_pcc_var_fac },
    { "__builtin_fprintf_unlocked", sp_i_pv_pcc_var_fac },
    { "__builtin_putc", sp_i_i_pv_fac },
    { "__builtin_putc_unlocked", sp_i_i_pv_fac },
    { "__builtin_fputc", sp_i_i_pv_fac },
    { "__builtin_fputc_unlocked", sp_i_i_pv_fac },
    { "__builtin_fputs", sp_i_pcc_pv_fac },
    { "__builtin_fputs_unlocked", sp_i_pcc_pv_fac },
    { "__builtin_fscanf", sp_i_pv_pcc_var_fac },
    { "__builtin_fwrite", sp_sz_pcc_sz_sz_pv_fac },
    { "__builtin_fwrite_unlocked", sp_sz_pcc_sz_sz_pv_fac },
    { "__builtin_printf", sp_i_pcc_var_fac },
    { "__builtin_printf_unlocked", sp_i_pcc_var_fac },
    { "__builtin_putchar", sp_i_i_fac },
    { "__builtin_putchar_unlocked", sp_i_i_fac },
    { "__builtin_puts", sp_i_pcc_fac },
    { "__builtin_puts_unlocked", sp_i_pcc_fac },
    { "__builtin_scanf", sp_i_pcc_var_fac },
    { "__builtin_snprintf", sp_i_pc_sz_pcc_var_fac },
    { "__builtin_sprintf", sp_i_pc_pcc_var_fac },
    { "__builtin_sscanf", sp_i_pcc_sz_pcc_var_fac },
    { "__builtin_vfprintf", sp_i_pv_pcc_val_fac },
    { "__builtin_vfscanf", sp_i_pv_pcc_val_fac },
    { "__builtin_vprintf", sp_i_pcc_val_fac },
    { "__builtin_vscanf", sp_i_pcc_val_fac },
    { "__builtin_vsnprintf", sp_i_pc_sz_pcc_val_fac },
    { "__builtin_vsprintf", sp_i_pc_pcc_val_fac },
    { "__builtin_vsscanf", sp_i_pcc_pcc_val_fac },

    // Category: ctype builtins.
    { "__builtin_isalnum", sp_i_i_fac },
    { "__builtin_isalpha", sp_i_i_fac },
    { "__builtin_isascii", sp_i_i_fac },
    { "__builtin_isblank", sp_i_i_fac },
    { "__builtin_iscntrl", sp_i_i_fac },
    { "__builtin_isdigit", sp_i_i_fac },
    { "__builtin_isgraph", sp_i_i_fac },
    { "__builtin_islower", sp_i_i_fac },
    { "__builtin_isprint", sp_i_i_fac },
    { "__builtin_ispunct", sp_i_i_fac },
    { "__builtin_isspace", sp_i_i_fac },
    { "__builtin_isupper", sp_i_i_fac },
    { "__builtin_isxdigit", sp_i_i_fac },
    { "__builtin_toascii", sp_i_i_fac },
    { "__builtin_tolower", sp_i_i_fac },
    { "__builtin_toupper", sp_i_i_fac },
    { "__builtin_iswalnum", sp_i_wi_fac },
    { "__builtin_iswalpha", sp_i_wi_fac },
    { "__builtin_iswblank", sp_i_wi_fac },
    { "__builtin_iswcntrl", sp_i_wi_fac },
    { "__builtin_iswdigit", sp_i_wi_fac },
    { "__builtin_iswgraph", sp_i_wi_fac },
    { "__builtin_iswlower", sp_i_wi_fac },
    { "__builtin_iswprint", sp_i_wi_fac },
    { "__builtin_iswpunct", sp_i_wi_fac },
    { "__builtin_iswspace", sp_i_wi_fac },
    { "__builtin_iswupper", sp_i_wi_fac },
    { "__builtin_iswxdigit", sp_i_wi_fac },
    { "__builtin_toascii", sp_i_wi_fac },
    { "__builtin_towlower", sp_wi_wi_fac },
    { "__builtin_towupper", sp_wi_wi_fac },

    // Category: overflow builtins
    { "__builtin_add_overflow",
      _builtin_overflow::create<nullptr, false, _builtin_overflow::op::add> },
    { "__builtin_add_overflow_p",
      _builtin_overflow::create<nullptr, true, _builtin_overflow::op::add> },
    { "__builtin_sadd_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_i, false,
			       _builtin_overflow::op::add> },
    { "__builtin_saddl_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_l, false,
			       _builtin_overflow::op::add> },
    { "__builtin_saddll_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ll, false,
			       _builtin_overflow::op::add> },
    { "__builtin_uadd_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_u, false,
			       _builtin_overflow::op::add> },
    { "__builtin_uaddl_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ul, false,
			       _builtin_overflow::op::add> },
    { "__builtin_uaddll_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ull, false,
			       _builtin_overflow::op::add> },
    { "__builtin_sub_overflow",
      _builtin_overflow::create<nullptr, false, _builtin_overflow::op::sub> },
    { "__builtin_sub_overflow_p",
      _builtin_overflow::create<nullptr, true, _builtin_overflow::op::sub> },
    { "__builtin_ssub_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_i, false,
				_builtin_overflow::op::sub> },
    { "__builtin_ssubl_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_l, false,
				_builtin_overflow::op::sub> },
    { "__builtin_ssubll_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ll, false,
				_builtin_overflow::op::sub> },
    { "__builtin_usub_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_u, false,
				_builtin_overflow::op::sub> },
    { "__builtin_usubl_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ul, false,
				_builtin_overflow::op::sub> },
    { "__builtin_usubll_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ull, false,
				_builtin_overflow::op::sub> },
    { "__builtin_mul_overflow",
      _builtin_overflow::create<nullptr, false, _builtin_overflow::op::mul> },
    { "__builtin_mul_overflow_p",
      _builtin_overflow::create<nullptr, true, _builtin_overflow::op::mul> },
    { "__builtin_smul_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_i, false,
				_builtin_overflow::op::mul> },
    { "__builtin_smull_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_l, false,
				_builtin_overflow::op::mul> },
    { "__builtin_smulll_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ll, false,
				_builtin_overflow::op::mul> },
    { "__builtin_umul_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_u, false,
				_builtin_overflow::op::mul> },
    { "__builtin_umull_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ul, false,
				_builtin_overflow::op::mul> },
    { "__builtin_umulll_overflow",
      _builtin_overflow::create<_builtin_overflow::fac_ull, false,
				_builtin_overflow::op::mul> },

    // Category: miscellaneous builtins.
    { "__builtin_abort", sp_v_fac },
    { "__builtin_abs", sp_i_i_fac },
    { "__builtin_aggregate_incoming_address", sp_pv_var_fac },
    { "__builtin_aligned_alloc", sp_pv_sz_sz_fac },
    { "__builtin_alloca", sp_pv_sz_fac },
    { "__builtin_alloca_with_align", sp_pv_sz_sz_fac },
    { "__builtin_apply", sp_pv_pF_v_var__pv_sz_fac },
    { "__builtin_apply_args", sp_pv_var_fac },
    { "__builtin_assume_aligned", sp_pv_pcv_sz_var_fac },
    { "__builtin_bswap16", sp_u16_u16_fac },
    { "__builtin_bswap32", sp_u32_u32_fac },
    { "__builtin_bswap64", sp_u64_u64_fac },
    { "__builtin_calloc", sp_pv_sz_sz_fac },
    { "__builtin_choose_expr", builtin_func_choose_expr::create },
    { "__builtin___clear_cache", sp_v_pv_pv_fac },
    { "__builtin_classify_type", sp_i_var_fac },
    { "__builtin_clz", sp_i_u_fac },
    { "__builtin_clzimax", sp_i_umax_fac },
    { "__builtin_clzl", sp_i_ul_fac },
    { "__builtin_clzll", sp_i_ull_fac },
    { "__builtin_constant_p", builtin_func_constant_p::create },
    { "__builtin_ctz", sp_i_u_fac },
    { "__builtin_ctzimax", sp_i_umax_fac },
    { "__builtin_ctzl", sp_i_ul_fac },
    { "__builtin_ctzll", sp_i_ull_fac },
    { "__builtin_clrsb", sp_i_i_fac },
    { "__builtin_clrsbimax", sp_i_imax_fac },
    { "__builtin_clrsbl", sp_i_l_fac },
    { "__builtin_clrsbll", sp_i_ll_fac },
    { "__builtin_dcgettext", sp_pc_pcc_pcc_i_fac },
    { "__builtin_dgettext", sp_pc_pcc_pcc_fac },
    { "__builtin_dwarf_cfa", sp_pv_fac },
    { "__builtin_dwarf_sp_column", sp_u_fac },
    { "__builtin_eh_return", sp_v_ssz_pv_fac },
    { "__builtin_eh_return_data_regno)", sp_i_i_fac },
    { "__builtin_execl", sp_i_pcc_pcc_var_fac },
    { "__builtin_execlp", sp_i_pcc_pcc_var_fac },
    { "__builtin_execle", sp_i_pcc_pcc_var_fac },
    { "__builtin_execv", sp_i_pcc_pv_pcc_fac },
    { "__builtin_execvp", sp_i_pcc_pv_pcc_fac },
    { "__builtin_execve", sp_i_pcc_pv_pcc_pv_pcc_fac },
    { "__builtin_exit", sp_v_i_fac },
    { "__builtin_expect", sp_l_l_l_fac },
    { "__builtin_extend_pointer", sp_sz_pv_fac },
    { "__builtin_extract_return_addr", sp_pv_pv_fac },
    { "__builtin_ffs", sp_i_i_fac },
    { "__builtin_ffsimax", sp_i_imax_fac },
    { "__builtin_ffsl", sp_i_l_fac },
    { "__builtin_ffsll", sp_i_ll_fac },
    { "__builtin_fork", sp_pid_fac },
    { "__builtin_frame_address", sp_pv_u_fac },
    { "__builtin_free", sp_v_pv_fac },
    { "__builtin_frob_return_address", sp_pv_pv_fac },
    { "__builtin_gettext", sp_pc_pcc_fac },
    { "__builtin_imaxabs", sp_imax_imax_fac },
    { "__builtin_init_dwarf_reg_size_table", sp_v_pv_fac },
    { "__builtin_finite", sp_i_d_fac },
    { "__builtin_finitef", sp_i_f_fac },
    { "__builtin_finitel", sp_i_ld_fac },
    { "__builtin_fpclassify", sp_i_i_i_i_i_i_var_fac },
    { "__builtin_isfinite", sp_i_var_fac },
    { "__builtin_isinf_sign", sp_i_var_fac },
    { "__builtin_isinf", sp_i_var_fac },
    { "__builtin_isinff", sp_i_f_fac },
    { "__builtin_isinfl", sp_i_ld_fac },
    { "__builtin_isnan", sp_i_var_fac },
    { "__builtin_isnanf", sp_i_f_fac },
    { "__builtin_isnanl", sp_i_ld_fac },
    { "__builtin_isnormal", sp_i_var_fac },
    { "__builtin_isgreater", sp_i_var_fac },
    { "__builtin_isgreaterequal", sp_i_var_fac },
    { "__builtin_isless", sp_i_var_fac },
    { "__builtin_islessequal", sp_i_var_fac },
    { "__builtin_islessgreater", sp_i_var_fac },
    { "__builtin_isunordered", sp_i_var_fac },
    { "__builtin_labs", sp_l_l_fac },
    { "__builtin_llabs", sp_ll_ll_fac },
    { "__builtin_longjmp", sp_v_pv_i_fac },
    { "__builtin_malloc", sp_pv_sz_fac },
    { "__builtin_next_arg", sp_pv_var_fac },
    { "__builtin_parity", sp_i_u_fac },
    { "__builtin_parityimax", sp_i_umax_fac },
    { "__builtin_parityl", sp_i_ul_fac },
    { "__builtin_parityll", sp_i_ull_fac },
    { "__builtin_popcount", sp_i_u_fac },
    { "__builtin_popcountimax", sp_i_umax_fac },
    { "__builtin_popcountl", sp_i_ul_fac },
    { "__builtin_popcountll", sp_i_ull_fac },
    { "__builtin_prefetch", sp_v_pcvv_var_fac },
    { "__builtin_realloc", sp_pv_pv_sz_fac },
    { "__builtin_return", sp_v_pv_fac },
    { "__builtin_return_address", sp_pv_u_fac },
    { "__builtin_saveregs", sp_pv_var_fac },
    { "__builtin_setjmp", sp_i_pv_fac },
    { "__builtin_stack_restore", sp_v_pv_fac },
    { "__builtin_stack_save", sp_pv_fac },
    { "__builtin_strfmon", sp_ssz_pc_sz_pcc_var_fac },
    { "__builtin_strftime", sp_sz_pc_sz_pcc_pcv_fac },
    { "__builtin_trap", sp_v_fac },
    { "__builtin_unreachable", sp_v_fac },
    { "__builtin_unwind_init", sp_v_fac },
    { "__builtin_update_setjmp_buf", sp_v_pv_i_fac },
    { "__builtin_va_copy", sp_v_val_val_fac },
    { "__builtin_va_end", sp_v_val_fac },
    { "__builtin_va_start", sp_v_val_var_fac },
    { "__builtin_va_arg_pack", sp_i_fac },
    { "__builtin_va_arg_pack_len", sp_i_fac },
    { "__builtin__exit", sp_v_i_fac },
    { "__builtin__Exit", sp_v_i_fac },

    // Object size checking builtins.
    { "__builtin_object_size", sp_sz_pcv_i_fac },
    { "__builtin___memcpy_chk", sp_pv_pv_pcv_sz_sz_fac },
    { "__builtin___memmove_chk", sp_pv_pv_pcv_sz_sz_fac },
    { "__builtin___mempcpy_chk", sp_pv_pv_pcv_sz_sz_fac },
    { "__builtin___memset_chk", sp_pv_pv_i_sz_sz_fac },
    { "__builtin___stpcpy_chk", sp_pc_pc_pcc_sz_fac },
    { "__builtin___stpncpy_chk", sp_pc_pc_pcc_sz_sz_fac },
    { "__builtin___strcat_chk", sp_pc_pc_pcc_sz_fac },
    { "__builtin___strcpy_chk", sp_pc_pc_pcc_sz_fac },
    { "__builtin___strncat_chk", sp_pc_pc_pcc_sz_sz_fac },
    { "__builtin___strncpy_chk", sp_pc_pc_pcc_sz_sz_fac },
    { "__builtin___snprintf_chk", sp_i_pc_sz_i_sz_pcc_var_fac },
    { "__builtin___vsnprintf_chk", sp_i_pc_sz_i_sz_pcc_val_fac },
    { "__builtin___sprintf_chk", sp_i_pc_i_sz_pcc_var_fac },
    { "__builtin___vsprintf_chk", sp_i_pc_i_sz_pcc_val_fac },
    { "__builtin___fprintf_chk", sp_i_pv_i_pcc_var_fac },
    { "__builtin___vfprintf_chk", sp_i_pv_i_pcc_val_fac },
    { "__builtin___printf_chk", sp_i_i_pcc_var_fac },
    { "__builtin___vprintf_chk", sp_i_i_pcc_val_fac },

    // Profiling hooks.
    { "__cyg_profile_func_enter", sp_v_pv_pv_fac },
    { "__cyg_profile_func_exit", sp_v_pv_pv_fac },

    // TLS thread pointer related builtins.
    { "__builtin_thread_pointer", sp_pv_fac },
    { "__builtin_set_thread_pointer", sp_v_pv_fac },

    // __FILE__, __LINE__, __FUNCTION__ as builtins.
    { "__builtin_FILE", sp_pcc_fac },
    { "__builtin_FUNCTION", sp_pcc_fac },
    { "__builtin_LINE", sp_i_fac },

    // Synchronization Primitives.
    { "__sync_fetch_and_add", pio0_iN_pvv_iN_var_fac },
    { "__sync_fetch_and_add_1", sp_i8_pvv_i8_fac },
    { "__sync_fetch_and_add_2", sp_i16_pvv_i16_fac },
    { "__sync_fetch_and_add_4", sp_i32_pvv_i32_fac },
    { "__sync_fetch_and_add_8", sp_i64_pvv_i64_fac },
    { "__sync_fetch_and_add_16", sp_i128_pvv_i128_fac },
    { "__sync_fetch_and_sub", pio0_iN_pvv_iN_var_fac },
    { "__sync_fetch_and_sub_1", sp_i8_pvv_i8_fac },
    { "__sync_fetch_and_sub_2", sp_i16_pvv_i16_fac },
    { "__sync_fetch_and_sub_4", sp_i32_pvv_i32_fac },
    { "__sync_fetch_and_sub_8", sp_i64_pvv_i64_fac },
    { "__sync_fetch_and_sub_16", sp_i128_pvv_i128_fac },
    { "__sync_fetch_and_or", pio0_iN_pvv_iN_var_fac },
    { "__sync_fetch_and_or_1", sp_i8_pvv_i8_fac },
    { "__sync_fetch_and_or_2", sp_i16_pvv_i16_fac },
    { "__sync_fetch_and_or_4", sp_i32_pvv_i32_fac },
    { "__sync_fetch_and_or_8", sp_i64_pvv_i64_fac },
    { "__sync_fetch_and_or_16", sp_i128_pvv_i128_fac },
    { "__sync_fetch_and_and", pio0_iN_pvv_iN_var_fac },
    { "__sync_fetch_and_and_1", sp_i8_pvv_i8_fac },
    { "__sync_fetch_and_and_2", sp_i16_pvv_i16_fac },
    { "__sync_fetch_and_and_4", sp_i32_pvv_i32_fac },
    { "__sync_fetch_and_and_8", sp_i64_pvv_i64_fac },
    { "__sync_fetch_and_and_16", sp_i128_pvv_i128_fac },
    { "__sync_fetch_and_xor", pio0_iN_pvv_iN_var_fac },
    { "__sync_fetch_and_xor_1", sp_i8_pvv_i8_fac },
    { "__sync_fetch_and_xor_2", sp_i16_pvv_i16_fac },
    { "__sync_fetch_and_xor_4", sp_i32_pvv_i32_fac },
    { "__sync_fetch_and_xor_8", sp_i64_pvv_i64_fac },
    { "__sync_fetch_and_xor_16", sp_i128_pvv_i128_fac },
    { "__sync_fetch_and_nand", pio0_iN_pvv_iN_var_fac },
    { "__sync_fetch_and_nand_1", sp_i8_pvv_i8_fac },
    { "__sync_fetch_and_nand_2", sp_i16_pvv_i16_fac },
    { "__sync_fetch_and_nand_4", sp_i32_pvv_i32_fac },
    { "__sync_fetch_and_nand_8", sp_i64_pvv_i64_fac },
    { "__sync_fetch_and_nand_16", sp_i128_pvv_i128_fac },
    { "__sync_add_and_fetch", pio0_iN_pvv_iN_var_fac },
    { "__sync_add_and_fetch_1", sp_i8_pvv_i8_fac },
    { "__sync_add_and_fetch_2", sp_i16_pvv_i16_fac },
    { "__sync_add_and_fetch_4", sp_i32_pvv_i32_fac },
    { "__sync_add_and_fetch_8", sp_i64_pvv_i64_fac },
    { "__sync_add_and_fetch_16", sp_i128_pvv_i128_fac },
    { "__sync_sub_and_fetch", pio0_iN_pvv_iN_var_fac },
    { "__sync_sub_and_fetch_1", sp_i8_pvv_i8_fac },
    { "__sync_sub_and_fetch_2", sp_i16_pvv_i16_fac },
    { "__sync_sub_and_fetch_4", sp_i32_pvv_i32_fac },
    { "__sync_sub_and_fetch_8", sp_i64_pvv_i64_fac },
    { "__sync_sub_and_fetch_16", sp_i128_pvv_i128_fac },
    { "__sync_or_and_fetch", pio0_iN_pvv_iN_var_fac },
    { "__sync_or_and_fetch_1", sp_i8_pvv_i8_fac },
    { "__sync_or_and_fetch_2", sp_i16_pvv_i16_fac },
    { "__sync_or_and_fetch_4", sp_i32_pvv_i32_fac },
    { "__sync_or_and_fetch_8", sp_i64_pvv_i64_fac },
    { "__sync_or_and_fetch_16", sp_i128_pvv_i128_fac },
    { "__sync_and_and_fetch", pio0_iN_pvv_iN_var_fac },
    { "__sync_and_and_fetch_1", sp_i8_pvv_i8_fac },
    { "__sync_and_and_fetch_2", sp_i16_pvv_i16_fac },
    { "__sync_and_and_fetch_4", sp_i32_pvv_i32_fac },
    { "__sync_and_and_fetch_8", sp_i64_pvv_i64_fac },
    { "__sync_and_and_fetch_16", sp_i128_pvv_i128_fac },
    { "__sync_xor_and_fetch", pio0_iN_pvv_iN_var_fac },
    { "__sync_xor_and_fetch_1", sp_i8_pvv_i8_fac },
    { "__sync_xor_and_fetch_2", sp_i16_pvv_i16_fac },
    { "__sync_xor_and_fetch_4", sp_i32_pvv_i32_fac },
    { "__sync_xor_and_fetch_8", sp_i64_pvv_i64_fac },
    { "__sync_xor_and_fetch_16", sp_i128_pvv_i128_fac },
    { "__sync_nand_and_fetch", pio0_iN_pvv_iN_var_fac },
    { "__sync_nand_and_fetch_1", sp_i8_pvv_i8_fac },
    { "__sync_nand_and_fetch_2", sp_i16_pvv_i16_fac },
    { "__sync_nand_and_fetch_4", sp_i32_pvv_i32_fac },
    { "__sync_nand_and_fetch_8", sp_i64_pvv_i64_fac },
    { "__sync_nand_and_fetch_16", sp_i128_pvv_i128_fac },
    { "__sync_bool_compare_and_swap", pio0_b_pvv_iN_iN_var_fac },
    { "__sync_bool_compare_and_swap_1", sp_b_pvv_i8_i8_fac },
    { "__sync_bool_compare_and_swap_2", sp_b_pvv_i16_i16_fac },
    { "__sync_bool_compare_and_swap_4", sp_b_pvv_i32_i32_fac },
    { "__sync_bool_compare_and_swap_8", sp_b_pvv_i64_i64_fac },
    { "__sync_bool_compare_and_swap_16", sp_b_pvv_i128_i128_fac },
    { "__sync_val_compare_and_swap", pio0_iN_pvv_iN_iN_var_fac },
    { "__sync_val_compare_and_swap_1", sp_i8_pvv_i8_i8_fac },
    { "__sync_val_compare_and_swap_2", sp_i16_pvv_i16_i16_fac },
    { "__sync_val_compare_and_swap_4", sp_i32_pvv_i32_i32_fac },
    { "__sync_val_compare_and_swap_8", sp_i64_pvv_i64_i64_fac },
    { "__sync_val_compare_and_swap_16", sp_i128_pvv_i128_i128_fac },
    { "__sync_lock_test_and_set", pio0_iN_pvv_iN_var_fac },
    { "__sync_lock_test_and_set_1", sp_i8_pvv_i8_fac },
    { "__sync_lock_test_and_set_2", sp_i16_pvv_i16_fac },
    { "__sync_lock_test_and_set_4", sp_i32_pvv_i32_fac },
    { "__sync_lock_test_and_set_8", sp_i64_pvv_i64_fac },
    { "__sync_lock_test_and_set_16", sp_i128_pvv_i128_fac },
    { "__sync_lock_release", pio0_v_pvv_var_fac },
    { "__sync_lock_release_1", sp_v_pvv_fac },
    { "__sync_lock_release_2", sp_v_pvv_fac },
    { "__sync_lock_release_4", sp_v_pvv_fac },
    { "__sync_lock_release_8", sp_v_pvv_fac },
    { "__sync_lock_release_16", sp_v_pvv_fac },
    { "__sync_synchronize", sp_v_fac },
    { "__atomic_test_and_set", sp_b_pvv_i_fac },
    { "__atomic_clear", sp_v_pvv_i_fac },
    { "__atomic_exchange", sp_v_pvv_pv_pv_i_fac },
    { "__atomic_exchange_n", pio0_iN_pvv_iN_i_fac },
    { "__atomic_exchange_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_exchange_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_exchange_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_exchange_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_exchange_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_load", sp_v_pcvv_pv_i_fac },
    { "__atomic_load_n", pio0_iN_pcvv_i_fac },
    { "__atomic_load_1", sp_i8_pcvv_i_fac },
    { "__atomic_load_2", sp_i16_pcvv_i_fac },
    { "__atomic_load_4", sp_i32_pcvv_i_fac },
    { "__atomic_load_8", sp_i64_pcvv_i_fac },
    { "__atomic_load_16", sp_i128_pcvv_i_fac },
    { "__atomic_compare_exchange", sp_b_pvv_pv_pv_b_i_i_fac },
    { "__atomic_compare_exchange_n", pio0_b_pvv_pv_iN_b_i_i_fac },
    { "__atomic_compare_exchange_1", sp_b_pvv_pv_i8_b_i_i_fac },
    { "__atomic_compare_exchange_2", sp_b_pvv_pv_i16_b_i_i_fac },
    { "__atomic_compare_exchange_4", sp_b_pvv_pv_i32_b_i_i_fac },
    { "__atomic_compare_exchange_8", sp_b_pvv_pv_i64_b_i_i_fac },
    { "__atomic_compare_exchange_16", sp_b_pvv_pv_i128_b_i_i_fac },
    { "__atomic_store", sp_v_pvv_pv_i_fac },
    { "__atomic_store_n", pio0_v_pvv_iN_i_fac },
    { "__atomic_store_1", sp_v_pvv_i8_i_fac },
    { "__atomic_store_2", sp_v_pvv_i16_i_fac },
    { "__atomic_store_4", sp_v_pvv_i32_i_fac },
    { "__atomic_store_8", sp_v_pvv_i64_i_fac },
    { "__atomic_store_16", sp_v_pvv_i128_i_fac },
    { "__atomic_add_fetch", pio0_iN_pvv_iN_i_fac },
    { "__atomic_add_fetch_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_add_fetch_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_add_fetch_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_add_fetch_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_add_fetch_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_sub_fetch", pio0_iN_pvv_iN_i_fac },
    { "__atomic_sub_fetch_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_sub_fetch_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_sub_fetch_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_sub_fetch_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_sub_fetch_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_and_fetch", pio0_iN_pvv_iN_i_fac },
    { "__atomic_and_fetch_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_and_fetch_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_and_fetch_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_and_fetch_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_and_fetch_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_nand_fetch", pio0_iN_pvv_iN_i_fac },
    { "__atomic_nand_fetch_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_nand_fetch_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_nand_fetch_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_nand_fetch_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_nand_fetch_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_xor_fetch", pio0_iN_pvv_iN_i_fac },
    { "__atomic_xor_fetch_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_xor_fetch_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_xor_fetch_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_xor_fetch_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_xor_fetch_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_or_fetch", pio0_iN_pvv_iN_i_fac },
    { "__atomic_or_fetch_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_or_fetch_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_or_fetch_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_or_fetch_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_or_fetch_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_fetch_add", pio0_iN_pvv_iN_i_fac },
    { "__atomic_fetch_add_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_fetch_add_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_fetch_add_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_fetch_add_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_fetch_add_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_fetch_sub", pio0_iN_pvv_iN_i_fac },
    { "__atomic_fetch_sub_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_fetch_sub_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_fetch_sub_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_fetch_sub_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_fetch_sub_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_fetch_and", pio0_iN_pvv_iN_i_fac },
    { "__atomic_fetch_and_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_fetch_and_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_fetch_and_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_fetch_and_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_fetch_and_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_fetch_nand", pio0_iN_pvv_iN_i_fac },
    { "__atomic_fetch_nand_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_fetch_nand_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_fetch_nand_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_fetch_nand_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_fetch_nand_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_fetch_xor", pio0_iN_pvv_iN_i_fac },
    { "__atomic_fetch_xor_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_fetch_xor_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_fetch_xor_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_fetch_xor_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_fetch_xor_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_fetch_or", pio0_iN_pvv_iN_i_fac },
    { "__atomic_fetch_or_1", sp_i8_pvv_i8_i_fac },
    { "__atomic_fetch_or_2", sp_i16_pvv_i16_i_fac },
    { "__atomic_fetch_or_4", sp_i32_pvv_i32_i_fac },
    { "__atomic_fetch_or_8", sp_i64_pvv_i64_i_fac },
    { "__atomic_fetch_or_16", sp_i128_pvv_i128_i_fac },
    { "__atomic_always_lock_free", sp_b_sz_pcvv_fac },
    { "__atomic_is_lock_free", sp_b_sz_pcvv_fac },
    { "__atomic_thread_fence", sp_v_i_fac },
    { "__atomic_signal_fence", sp_v_i_fac },
  };

  return m;
}
