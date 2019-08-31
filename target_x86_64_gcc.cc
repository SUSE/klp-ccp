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
#include "execution_charset_encoder_iconv.hh"
#include "types_impl.hh"
#include "ast.hh"
#include "semantic_except.hh"
#include "target_x86_64_gcc.hh"
#include "pp_token.hh"
#include "preprocessor.hh"
#include "cmdline_except.hh"
#include "pp_except.hh"

using namespace klp::ccp;
using namespace klp::ccp::types;

static gcc_cmdline_parser::option gcc_opts_common[] = {
	#include "gcc_cmdline_opts_common.cc"
	{ nullptr }
};

static gcc_cmdline_parser::option gcc_opts_c_family[] = {
	#include "gcc_cmdline_opts_c_family.cc"
	{ nullptr }
};

static gcc_cmdline_parser::option gcc_opts_i386[] = {
	#include "gcc_cmdline_opts_i386.cc"
	{ nullptr }
};


namespace
{
  class _builtin_typedef_va_list final : public builtin_typedef
  {
  public:
    virtual ~_builtin_typedef_va_list() noexcept override;

    virtual std::shared_ptr<const types::addressable_type>
    evaluate(ast::ast&, const target &tgt,
	     const ast::type_specifier_tdid&) const override;

    static std::unique_ptr<_builtin_typedef_va_list> create();
  };
}

_builtin_typedef_va_list::~_builtin_typedef_va_list() noexcept = default;

std::shared_ptr<const types::addressable_type> _builtin_typedef_va_list::
evaluate(ast::ast&, const target &tgt,
	 const ast::type_specifier_tdid&) const
{
  return tgt.create_builtin_va_list_type();
}

std::unique_ptr<_builtin_typedef_va_list> _builtin_typedef_va_list::create()
{
  return
    std::unique_ptr<_builtin_typedef_va_list>{new _builtin_typedef_va_list()};
}


namespace
{
  class _builtin_typedef__int128 final : public builtin_typedef
  {
  public:
    _builtin_typedef__int128(const bool is_signed) noexcept;

    virtual ~_builtin_typedef__int128() noexcept override;

    virtual std::shared_ptr<const types::addressable_type>
    evaluate(ast::ast&, const target&,
	     const ast::type_specifier_tdid&) const override;

    static std::unique_ptr<_builtin_typedef__int128>
    create_signed();

    static std::unique_ptr<_builtin_typedef__int128>
    create_unsigned();

  private:
    static std::unique_ptr<_builtin_typedef__int128>
    _create(const bool is_signed);

    bool _is_signed;
  };
}

_builtin_typedef__int128::_builtin_typedef__int128(const bool is_signed)
  noexcept
  : _is_signed(is_signed)
{}

_builtin_typedef__int128::~_builtin_typedef__int128() noexcept = default;

std::shared_ptr<const types::addressable_type>
_builtin_typedef__int128::evaluate(ast::ast&, const target&,
				   const ast::type_specifier_tdid&) const
{
  return std_int_type::create(std_int_type::kind::k_int128,
			      _is_signed);
}

std::unique_ptr<_builtin_typedef__int128>
_builtin_typedef__int128::create_signed()
{
  return _create(true);
}

std::unique_ptr<_builtin_typedef__int128>
_builtin_typedef__int128::create_unsigned()
{
  return _create(false);
}

std::unique_ptr<_builtin_typedef__int128>
_builtin_typedef__int128::_create(const bool is_signed)
{
  return std::unique_ptr<_builtin_typedef__int128>{
		new _builtin_typedef__int128{is_signed}
	 };
}


target_x86_64_gcc::target_x86_64_gcc(const char * const version)
  : _gcc_version(_parse_version(version))
{
  _builtin_typedefs.emplace_back("__builtin_va_list",
				 _builtin_typedef_va_list::create);
  _builtin_typedefs.emplace_back("__int128_t",
				 _builtin_typedef__int128::create_signed);
  _builtin_typedefs.emplace_back("__uint128_t",
				 _builtin_typedef__int128::create_unsigned);
}


void target_x86_64_gcc::parse_command_line
		(int argc, const char *argv[],
		 header_resolver &hr,
		 preprocessor &pp,
		 const std::function<void(const std::string&)> &report_warning)
{
  if (!argv) {
    // This only happens for the testsuite programs. Provide a
    // default set of #defines.
    pp.register_builtin_macro("__STDC__", "1");
    pp.register_builtin_macro("__STDC_HOSTED__", "1");
    pp.register_builtin_macro("__STDC_VERSION__", "199901L");
    _register_builtin_macros(pp);
    return;
  }

  gcc_cmdline_parser p{_gcc_version};
  p.register_table(gcc_opts_common);
  p.register_table(gcc_opts_c_family);
  p.register_table(gcc_opts_i386);

  const char *_base_file = nullptr;
  std::vector<const char *> pre_includes;

  std::vector<const char *> include_dirs;
  std::vector<const char *> include_dirs_quoted;
  std::vector<const char *> include_dirs_system;
  std::vector<const char *> include_dirs_after;

  bool undef = false;

  struct macro_def_or_undef
  {
    bool undef;
    const char *arg;
  };
  std::vector<macro_def_or_undef> macro_defs_and_undefs;

  bool optimize = false;

  auto &&handle_opt =
    [&](const char *name, const char *val, const bool negative) {
      if (!name) {
	if (_base_file) {
	  throw cmdline_except{
		  std::string{"more than one input file: '"} + _base_file +
		  "' and '" + val + "'"
		};
	}

	_base_file = val;
	return;
      }

      if (!std::strcmp(name, "include")) {
	pre_includes.push_back(val);
	return;
      }

      if (!std::strcmp(name, "I")) {
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
	return;
      }

      if (!std::strcmp(name, "iquote")) {
	include_dirs_quoted.push_back(val);
	return;
      }

      if (!std::strcmp(name, "isystem")) {
	include_dirs_after.push_back(val);
	return;
      }

      if (!std::strcmp(name, "idirafter")) {
	include_dirs_after.push_back(val);
	return;
      }

      if (!std::strcmp(name, "undef")) {
	undef = true;
	return;
      }

      if (!std::strcmp(name, "D")) {
	macro_defs_and_undefs.emplace_back(macro_def_or_undef{false, val});
	return;
      }

      if (!std::strcmp(name, "U")) {
	macro_defs_and_undefs.emplace_back(macro_def_or_undef{true, val});
	return;
      }

      if (!std::strcmp(name, "O")) {
	if (val && !strcmp(val, "0"))
	  optimize = false;
	else
	  optimize = true;
	return;
      }
    };

  p(argc, argv, handle_opt);

  if (!_base_file) {
    throw cmdline_except{"no input file"};
  }

  for (const auto dir : include_dirs_quoted)
    hr.append_search_dir_quoted(dir);

  for (const auto dir : include_dirs)
    hr.append_search_dir(dir);

  for (const auto dir : include_dirs_system)
    hr.append_search_dir(dir);

  for (const auto dir : include_dirs_after)
    hr.append_search_dir(dir);

  const std::string base_file{_base_file};

  for (const auto i : pre_includes) {
    std::string resolved = hr.resolve(i, base_file, header_resolver::cwd);
    if (resolved.empty()) {
      throw cmdline_except{
	      std::string{"file '"} + i + "' not found"
	    };
    }

    pp.add_root_source(resolved, true);
  }

  pp.add_root_source(base_file, false);
  pp.set_base_file(base_file);

  pp.register_builtin_macro("__STDC__", "1");
  pp.register_builtin_macro("__STDC_HOSTED__", "1");
  pp.register_builtin_macro("__STDC_VERSION__", "199901L");

  if (!undef) {
    _register_builtin_macros(pp);
    if (optimize)
      pp.register_builtin_macro("__OPTIMIZE__", "1");
  }

  for (const auto &m : macro_defs_and_undefs) {
    if (!m.undef) {
      const auto &_report_warning =
	[&](const std::string &msg) {
	  report_warning(std::string{"predefined macro '"} +
			 m.arg + "': " + msg);
	};

      const char * const repl = std::strchr(m.arg, '=');
      if (!repl) {
	try {
	  pp.register_predefined_macro(m.arg, "1", _report_warning);
	} catch (const pp_except &e) {
	  throw cmdline_except{
		  std::string{"failed to parse predefined macro '"} +
		  m.arg + "': " + e.what()
		};
	}

      } else {
	const std::string signature{m.arg, repl};
	try {
	  pp.register_predefined_macro(signature, repl + 1, _report_warning);
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
}

void target_x86_64_gcc::_register_builtin_macros(preprocessor &pp) const
{
  const std::initializer_list<std::pair<const char *, const char*>>
    builtin_object_macros = {
	{ "__x86_64__", "1" },
	{ "__x86_64", "1" },
	{ "__unix__", "1" },
	{ "__unix", "1" },
	{ "__linux__", "1" },
	{ "__linux", "1" },
	{ "__SIZE_TYPE__", "unsigned long" },
	{ "__PTRDIFF_TYPE__", "long" },
	{ "__INTPTR_TYPE__", "long" },
	{ "__UINTPTR_TYPE__", "unsigned long" },
	{ "__INT8_TYPE__", "char" },
	{ "__UINT8_TYPE__", "unsigned char" },
	{ "__INT_LEAST8_TYPE__", "char" },
	{ "__UINT_LEAST8_TYPE__", "unsigned char" },
	{ "__INT16_TYPE__", "short" },
	{ "__UINT16_TYPE__", "unsigned short" },
	{ "__INT_LEAST16_TYPE__", "short" },
	{ "__UINT_LEAST16_TYPE__", "unsigned short" },
	{ "__INT32_TYPE__", "int" },
	{ "__UINT32_TYPE__", "unsigned int" },
	{ "__INT_LEAST32_TYPE__", "int" },
	{ "__UINT_LEAST32_TYPE__", "unsigned int" },
	{ "__INT64_TYPE__", "long" },
	{ "__UINT64_TYPE__", "unsigned long" },
	{ "__INT_LEAST64_TYPE__", "long" },
	{ "__UINT_LEAST64_TYPE__", "unsigned long" },
	{ "__CHAR16_TYPE__", "unsigned short" },
	{ "__CHAR32_TYPE__", "unsigned int" },
	{ "__WCHAR_TYPE__", "int" },
	{ "__WINT_TYPE__", "unsigned int" },

	{ "__CHAR_BIT__", "8" },
	{ "__SIZE_MAX__", "0xffffffffffffffffUL" },
	{ "__PTRDIFF_MAX__", "__LONG_MAX__" },
	{ "__SCHAR_MAX__", "0x7f" },
	{ "__INT_MAX__", "0x7fffffff" },
	{ "__LONG_MAX__", "0x7fffffffffffffffL" },
	{ "__LLONG_MAX__", "__LONG_MAX__" },
	{ "__LONG_LONG_MAX__", "__LONG_MAX__" },
	{ "__SHRT_MAX__", "0x7fff" },
	{ "__WCHAR_MAX__", "__INT_MAX__" },

	{ "__FLT_MIN__", "1.17549435082228750797e-38F" },
	{ "__FLT_MAX__", "3.40282346638528859812e+38F" },
	{ "__DBL_MIN__", "((double)2.22507385850720138309e-308L)" },
	{ "__DBL_MAX__", "((double)1.79769313486231570815e+308L)" },
	{ "__LDBL_MIN__", "3.36210314311209350626e-4932L" },
	{ "__LDBL_MAX__", "1.18973149535723176502e+4932L" },

	{ "__FLT_MANT_DIG__", "24" },
	{ "__DBL_MANT_DIG__", "53" },

	{ "__SIZEOF_POINTER__", "sizeof(void*)" },
	{ "__SIZEOF_SIZE_T__", "sizeof(unsigned long)" },
	{ "__SIZEOF_PTRDIFF_T__", "sizeof(long)" },
	{ "__SIZEOF_SHORT__", "sizeof(short)" },
	{ "__SIZEOF_INT__", "sizeof(int)" },
	{ "__SIZEOF_LONG__", "sizeof(long)" },
	{ "__SIZEOF_LONG_LONG__", "sizeof(long long)" },
	{ "__SIZEOF_WCHAR_T__", "sizeof(int)" },
	{ "__SIZEOF_WINT_T__", "sizeof(unsigned int)" },
	{ "__SIZEOF_FLOAT__", "sizeof(float)" },
	{ "__SIZEOF_DOUBLE__", "sizeof(double)" },
	{ "__SIZEOF_LONG_DOUBLE__", "sizeof(long double)" },

	{ "__BIGGEST_ALIGNMENT__", "16" },

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
}

const builtin_typedef::factories& target_x86_64_gcc::get_builtin_typedefs()
  const noexcept
{
  return _builtin_typedefs;
}

bool target_x86_64_gcc::is_char_signed() const noexcept
{
  return true;
}

bool target_x86_64_gcc::is_wchar_signed() const noexcept
{
  return true;
}

bool target_x86_64_gcc::is_bitfield_default_signed() const noexcept
{
  return true;
}

mpa::limbs::size_type
target_x86_64_gcc::get_std_int_width(const types::std_int_type::kind k)
  const noexcept
{
  switch (k) {
  case types::std_int_type::kind::k_char:
    return 8;

  case types::std_int_type::kind::k_short:
    return 16;

  case types::std_int_type::kind::k_int:
    return 32;

  case types::std_int_type::kind::k_long:
    return 64;

  case types::std_int_type::kind::k_long_long:
    return 64;

  case types::std_int_type::kind::k_int128:
    return 128;
  };
}

types::std_int_type::kind
target_x86_64_gcc::int_mode_to_std_int_kind(const int_mode_kind m)
  const noexcept
{
  switch (m) {
  case int_mode_kind::imk_none:
    assert(0);
    __builtin_unreachable();

  case int_mode_kind::imk_QI:
    return types::std_int_type::kind::k_char;

  case int_mode_kind::imk_HI:
    return types::std_int_type::kind::k_short;

  case int_mode_kind::imk_SI:
    return types::std_int_type::kind::k_int;

  case int_mode_kind::imk_DI:
    return types::std_int_type::kind::k_long;

  case int_mode_kind::imk_TI:
    return types::std_int_type::kind::k_int128;
  }
}


mpa::limbs::size_type target_x86_64_gcc::
get_float_significand_width(const types::float_type::kind k)
  const noexcept
{
  switch (k) {
  case types::float_type::kind::k_float:
    return 24;

  case types::float_type::kind::k_double:
    return 53;

  case types::float_type::kind::k_long_double:
    return 113;
  };
}

mpa::limbs::size_type target_x86_64_gcc::
get_float_exponent_width(const types::float_type::kind k)
  const noexcept
{
  switch (k) {
  case types::float_type::kind::k_float:
    return 8;

  case types::float_type::kind::k_double:
    return 11;

  case types::float_type::kind::k_long_double:
    return 15;
  };
}

types::float_type::kind target_x86_64_gcc::
float_mode_to_float_kind(const float_mode_kind m) const noexcept
{
  switch (m) {
  case float_mode_kind::fmk_none:
    assert(0);
    __builtin_unreachable();

  case float_mode_kind::fmk_SF:
    return types::float_type::kind::k_float;

  case float_mode_kind::fmk_DF:
    return types::float_type::kind::k_double;
  }
}

int_mode_kind target_x86_64_gcc::get_pointer_mode() const noexcept
{
  return int_mode_kind::imk_DI;
}

int_mode_kind target_x86_64_gcc::get_word_mode() const noexcept
{
  return int_mode_kind::imk_DI;
}

mpa::limbs::size_type target_x86_64_gcc::get_biggest_alignment_log2()
  const noexcept
{
  return 4;
}

std::unique_ptr<execution_charset_encoder> target_x86_64_gcc::
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

void target_x86_64_gcc::evaluate_enum_type(ast::ast &a, types::enum_content &ec,
					   const bool packed,
					   const int_mode_kind mode,
					   types::alignment &&user_align) const
{
  // Inspect each enumerator and find the maximum required width
  // and signedness.
  bool is_any_signed = false;
  mpa::limbs::size_type min_prec = 0;
  mpa::limbs::size_type width = 0;

  if (mode != int_mode_kind::imk_none)
      width = int_mode_to_width(mode);

  ec.for_each_member
    ([&](const types::enum_content::member &m) {
      const target_int &v = m.get_value();
      const bool v_is_negative = v.is_negative();
      is_any_signed = is_any_signed || v_is_negative;
      min_prec = std::max(min_prec, v.min_required_width() - v_is_negative);

      if (min_prec + is_any_signed > 64) {
	code_remark remark (code_remark::severity::fatal,
			    "enumerator value out of bounds",
			    a.get_pp_result(),
			    m.get_enumerator().get_tokens_range());
	a.get_remarks().add(remark);
	throw semantic_except(remark);

      } else if (mode != int_mode_kind::imk_none &&
		 min_prec + is_any_signed > width) {
	code_remark remark(code_remark::severity::fatal,
			   "enumerator value exceeds specified integer mode",
			   a.get_pp_result(),
			   m.get_enumerator().get_tokens_range());
	a.get_remarks().add(remark);
	throw semantic_except(remark);

      }
    });

  // Normalize the width.
  if (mode == int_mode_kind::imk_none) {
    width = min_prec + is_any_signed;
    assert(width <= 64);

    if (!packed) {
      if (width > 32)
	width = 64;
      else
	width = 32;
    } else {
      if (width > 32)
	width = 64;
      else if (width > 16)
	width = 32;
      else if (width > 8)
	width = 16;
      else
	width = 8;
    }
  }

  // Oddly, gcc enforces the type to be signed for widths <= 16.
  auto &&t = types::std_int_type::create(_width_to_int_kind(width),
					 width <= 16 || is_any_signed);
  if (user_align.is_set()) {
    const mpa::limbs::size_type align =
      static_cast<mpa::limbs::size_type>(1) << user_align.get_log2_value();
    if (align * 8 >= width) {
      t = t->set_user_alignment(std::move(user_align));
    }
  }

  ec.set_underlying_type(std::move(t));

  // Finally, calculate each member's type and massage its value
  // accordingly. GCC's behaviour is a bit strange here in that it
  // combines global signedness with each member's required precision
  // individually.
  //
  // More precisely, the rules for determination of each enum member's
  // type are as follows:
  // 1. If the value fits into a signed int, that's the type.
  // 2. Otherwise, if all members fit into an unsigned int,
  //    the type is unsigned int.
  // 3. Otherwise, if there are any negative members, then all values
  //    fit into a signed long and the type is signed long.
  // 4. Otherwise, the type is unsigned long.
  ec.for_each_member
    ([&](types::enum_content::member &m) {
      const target_int &v = m.get_value();
      const bool v_is_negative = v.is_negative();
      const mpa::limbs::size_type v_prec =
	v.min_required_width() - v_is_negative;

      mpa::limbs::size_type m_width;
      bool m_is_signed;
      assert(v_prec <= 64);
      if (v_prec < 32) {
	m_width = 32;
	m_is_signed = true;
      } else if (width <= 32 && !is_any_signed) {
	m_width = 32;
	m_is_signed = false;
      } else if (is_any_signed) {
	assert(v_prec < 64);
	m_width = 64;
	m_is_signed = true;
      } else {
	m_width = 64;
	m_is_signed = false;
      }

      m.convert_value(m_width - m_is_signed, m_is_signed);
      m.set_type(types::std_int_type::create(_width_to_int_kind(m_width),
					     m_is_signed));
     });
}

mpa::limbs target_x86_64_gcc::
get_std_int_size(const types::std_int_type::kind k) const
{
  return mpa::limbs::from_size_type(get_std_int_width(k) / 8);
}

mpa::limbs::size_type target_x86_64_gcc::
get_std_int_alignment(const types::std_int_type::kind k) const
{
  return get_std_int_size(k).ffs() - 1;
}

mpa::limbs target_x86_64_gcc::
get_float_size(const types::float_type::kind k,
	       const bool is_complex) const
{
  mpa::limbs size;

  switch (k) {
  case float_type::kind::k_float:
    size = mpa::limbs::from_size_type(4);
    break;

  case float_type::kind::k_double:
    size = mpa::limbs::from_size_type(8);
    break;

  case float_type::kind::k_long_double:
    size = mpa::limbs::from_size_type(16);
    break;
  };

  if (is_complex)
    size = size.lshift(1);

  return size;
}

mpa::limbs::size_type target_x86_64_gcc::
get_float_alignment(const types::float_type::kind k, const bool) const
{
  return get_float_size(k, false).ffs() - 1;
}

mpa::limbs target_x86_64_gcc::get_pointer_size() const
{
  return mpa::limbs::from_size_type(8);
}

mpa::limbs::size_type target_x86_64_gcc::get_pointer_alignment() const
{
  return 3;
}

mpa::limbs target_x86_64_gcc::get_va_list_size() const
{
  return mpa::limbs::from_size_type(24);
}

mpa::limbs::size_type target_x86_64_gcc::get_va_list_alignment() const
{
  return 3;
}


/*
 * Layout structs and unions as gcc does.
 *
 * Relevant code is in gcc/stor-layout.c, entry point is
 * layout_type().
 */
typedef int64_t hwi;
typedef uint64_t uhwi;

template <typename T>
static inline mpa::limbs::size_type
crop_ffs_like_gcc(const mpa::limbs::size_type ffs) noexcept
{
  if (ffs > std::numeric_limits<T>::digits)
    return 0;

  return ffs;
}

template <typename Tret, typename T>
static inline Tret crop_like_gcc(const T val) noexcept
{
  return static_cast<Tret>(val);
}

namespace
{
  class record_layout_info
  {
  public:
    record_layout_info(const target_x86_64_gcc &tgt,
		       const mpa::limbs::size_type user_align_ffs);

    void place_struct_field(struct_or_union_content::member &m);
    void place_union_field(struct_or_union_content::member &m);

    void finish_record_layout(struct_or_union_content &sc);

  private:
    mpa::limbs::size_type
    _layout_decl_field(const object_type &o_t) const;

    mpa::limbs::size_type
    _layout_decl_bitfield(const bitfield_type &bf_t) const;

    mpa::limbs::size_type
    _update_alignment_for_field(const object_type &o_t);

    mpa::limbs::size_type
    _update_alignment_for_bitfield(const struct_or_union_content::member &m,
				   const bitfield_type &bf_t);

    static bool
    _excess_unit_span(const hwi byte_offset, const hwi bit_offset,
		      const hwi size, const mpa::limbs::size_type align_log2,
		      const uhwi type_size) noexcept;

    void _normalize();

    void _align_to_byte();

    const target_x86_64_gcc &_tgt;
    mpa::limbs _offset;
    mpa::limbs _bitpos;
    mpa::limbs::size_type _record_align_ffs;
    bool _is_size_constant;
  };
}

record_layout_info::
record_layout_info(const target_x86_64_gcc &tgt,
		   const mpa::limbs::size_type user_align_ffs)
  : _tgt(tgt), _offset(0), _bitpos(),
    _record_align_ffs(std::max(static_cast<mpa::limbs::size_type>(4),
			       user_align_ffs + 3)),
    _is_size_constant(true)
{
  // c.f. start_record_layout()
}

mpa::limbs::size_type record_layout_info::
_layout_decl_field(const object_type &o_t) const
{
  // This already takes the packed and aligned attributes into
  // account.
  return 3 + o_t.get_effective_alignment(_tgt) + 1;
}

mpa::limbs::size_type record_layout_info::
_layout_decl_bitfield(const bitfield_type &bf_t) const
{
  // With gcc, all declaration nodes' DECL_ALIGN() fields gets
  // initialized to one, c.f. make_node_stat().
  mpa::limbs::size_type desired_align_ffs = 1;
  const alignment &user_align = bf_t.get_user_alignment();
  if (user_align.is_set())
    desired_align_ffs = 3 + user_align.get_log2_value() + 1;

  if (!bf_t.get_width(_tgt)) {
    // From gcc: A zero-length bit-field affects the alignment of the
    // next field.  In essence such bit-fields are not influenced by
    // any packing due to #pragma pack or attribute packed.
    const std::shared_ptr<const returnable_int_type>& base_type = bf_t.get_base_type();
    const mpa::limbs::size_type base_type_align_ffs =
      3 + base_type->get_effective_alignment(_tgt) + 1;
    desired_align_ffs = std::max(base_type_align_ffs, desired_align_ffs);

    // Note that this won't affect ->record_align in
    // _update_alignment_for_bitfield(), because zero-width bitfields
    // are always unnamed. Odd.
  }

  return desired_align_ffs;
}

mpa::limbs::size_type record_layout_info::
_update_alignment_for_field(const object_type &o_t)
{
  const mpa::limbs::size_type desired_align_ffs = _layout_decl_field(o_t);

  _record_align_ffs = std::max(_record_align_ffs, desired_align_ffs);

  return desired_align_ffs;
}

mpa::limbs::size_type record_layout_info::
_update_alignment_for_bitfield(const struct_or_union_content::member &m,
			       const bitfield_type &bf_t)
{
  const mpa::limbs::size_type desired_align_ffs =
    _layout_decl_bitfield(bf_t);

  if (!m.get_name().empty()) {
    const std::shared_ptr<const returnable_int_type>& base_type = bf_t.get_base_type();
    mpa::limbs::size_type type_align_ffs =
      3 + base_type->get_effective_alignment(_tgt) + 1;

    if (bf_t.get_width(_tgt) && bf_t.is_packed())
      type_align_ffs = 3 + 1;

    _record_align_ffs = std::max(_record_align_ffs, desired_align_ffs);
    _record_align_ffs = std::max(_record_align_ffs, type_align_ffs);
  }

  return desired_align_ffs;
}

void record_layout_info::_normalize()
{
  // Split the bit position into a byte offset and a bit position.
  if (_bitpos.is_any_set_at_or_above(3)) {
    mpa::limbs offset_add = _bitpos;
    offset_add.set_bits_below(3, false);
    offset_add = offset_add.rshift(3, false);
    _offset = _offset + offset_add;
    _bitpos.set_bits_at_and_above(3, false);
  }
}

void record_layout_info::_align_to_byte()
{
  if (!_bitpos)
    return;

  const mpa::limbs partial_bytes = _bitpos.align(3).rshift(3, false);
  _offset = _offset + partial_bytes;
  _bitpos = mpa::limbs::from_size_type(0);
}

bool record_layout_info::
_excess_unit_span(const hwi byte_offset, const hwi bit_offset,
		  const hwi size, const mpa::limbs::size_type align_log2,
		  const uhwi type_size) noexcept
{
  uhwi offset = crop_like_gcc<uhwi>(byte_offset * 8 + bit_offset);
  uhwi align = static_cast<uhwi>(1) << align_log2;

  offset = offset % align;
  return ((offset + crop_like_gcc<uhwi>(size) + align - 1) / align >
	  type_size / align);
}

void record_layout_info::place_struct_field(struct_or_union_content::member &m)
{
  const std::shared_ptr<const type>& t = m.get_type();

  const mpa::limbs::size_type desired_align_ffs =
    (crop_ffs_like_gcc<unsigned int>
     (handle_types<mpa::limbs::size_type>
      ((wrap_callables<default_action_unreachable<mpa::limbs::size_type,
						  type_set<> >::type>
	([&](const bitfield_type &bf_t) {
	   return _update_alignment_for_bitfield(m, bf_t);
	 },
	 [&](const object_type &o_t) {
	   return _update_alignment_for_field(o_t);
	 })),
       *t)));

  // Align the field as desired.
  assert(desired_align_ffs == 1 || desired_align_ffs >= 4);
  if (desired_align_ffs >= 3 + 1) {
    _align_to_byte();
    _offset = _offset.align(desired_align_ffs - 1 - 3);
  }

  // From gcc: Handle compatibility with PCC.  Note that if the record
  // has any variable-sized fields, we need not worry about
  // compatibility.
  handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const bitfield_type &bf_t) {
	 if (!bf_t.is_packed() &&
	     bf_t.get_width(_tgt) &&
	     bf_t.get_width(_tgt) <= std::numeric_limits<uhwi>::max() &&
	     _is_size_constant && _offset.fits_into_type<uhwi>() &&
	     (bf_t.get_base_type()->get_size(_tgt).lshift(3)
	      .fits_into_type<uhwi>())) {
	   const std::shared_ptr<const returnable_int_type>& base_type =
	     bf_t.get_base_type();
	   const mpa::limbs::size_type type_align_ffs =
	     3 + base_type->get_effective_alignment(_tgt) + 1;
	   const hwi field_size =
	     crop_like_gcc<hwi>(static_cast<uhwi>(bf_t.get_width(_tgt)));
	   const hwi offset = crop_like_gcc<hwi>(_offset.to_type<uhwi>());
	   const hwi bit_offset = crop_like_gcc<hwi>(_bitpos.to_type<uhwi>());
	   const uhwi type_size =
	     base_type->get_size(_tgt).lshift(3).to_type<uhwi>();

	   // From gcc: A bit field may not span more units of alignment of
	   // its type than its type itself.  Advance to next boundary if
	   // necessary.
	   if (_excess_unit_span(offset, bit_offset, field_size,
				 type_align_ffs - 1, type_size)) {
	     _bitpos = _bitpos.align(type_align_ffs - 1);
	   }
	 }
       })),
     *t);

  // From gcc: Offset so far becomes the position of this field after
  // normalizing.
  _normalize();
  m.set_offset(_offset);
  m.set_bitpos(_bitpos);
  m.set_has_constant_offset(_is_size_constant);

  handle_types<void>
    ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
      ([&](const object_type &o_t) {
	 if (!o_t.is_complete()) {
	   // From gcc: Do nothing.
	   // Note that this handles the case of the last member being
	   // a flexible array member.

	 } else if (!o_t.is_size_constant()) {
	   assert(!_bitpos);
	   _is_size_constant = false;

	 } else {
	   _bitpos = _bitpos + o_t.get_size(_tgt).lshift(3);
	   _normalize();
	 }
       },
       [&](const bitfield_type &bf_t) {
	 _bitpos = _bitpos + mpa::limbs::from_size_type(bf_t.get_width(_tgt));
	 _normalize();
       })),
     *t);
}

void record_layout_info::place_union_field(struct_or_union_content::member &m)
{
  const std::shared_ptr<const type>& t = m.get_type();

  handle_types<void>
    ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
      ([&](const bitfield_type &bf_t) {
	 _update_alignment_for_bitfield(m, bf_t);
       },
       [&](const object_type &o_t) {
	 _update_alignment_for_field(o_t);
       })),
     *t);

  m.set_offset(mpa::limbs::from_size_type(0));
  m.set_bitpos(mpa::limbs::from_size_type(0));
  m.set_has_constant_offset(true);

  // From gcc: We assume the union's size will be a multiple of a byte
  // so we don't bother with BITPOS.
  mpa::limbs size =
    handle_types<mpa::limbs>
      ((wrap_callables<default_action_unreachable<mpa::limbs,
						  type_set<> >::type>
	([&](const bitfield_type &bf_t) {
	   return (mpa::limbs::from_size_type(bf_t.get_width(_tgt))
		   .align(3).rshift(3, false));
	 },
	 [&](const object_type &o_t) {
	   if (o_t.is_size_constant()) {
	     return o_t.get_size(_tgt);
	   } else {
	     _is_size_constant = false;
	     return mpa::limbs{};
	   }
	 })),
       *t);

  if (_offset < size)
    _offset = std::move(size);
}

void record_layout_info::finish_record_layout(struct_or_union_content &sc)
{
  _normalize();

  const mpa::limbs unpadded_size = _offset.lshift(3) + _bitpos;
  const mpa::limbs type_size = unpadded_size.align(_record_align_ffs - 1);
  assert(!type_size.is_any_set_below(3));

  assert(_record_align_ffs >= 4);
  sc.set_alignment(_record_align_ffs - 3 - 1);
  if (_is_size_constant) {
    sc.set_size(type_size.rshift(3, false));
    sc.set_size_constant(true);
  } else {
    sc.set_size_constant(false);
  }
}

void target_x86_64_gcc::
layout_struct(types::struct_or_union_content &sc,
	      const types::alignment &user_align) const
{
  record_layout_info rli(*this,
			 (user_align.is_set() ?
			  user_align.get_log2_value() + 1 :
			  0));

  for (struct_or_union_content::member_iterator it_m = sc.members_begin();
       it_m != sc.members_end(); ++it_m) {
    rli.place_struct_field(*it_m);
  }

  rli.finish_record_layout(sc);
}

void target_x86_64_gcc::
layout_union(types::struct_or_union_content &sc,
	     const types::alignment &user_align) const
{
  record_layout_info rli(*this,
			 (user_align.is_set() ?
			  user_align.get_log2_value() + 1 :
			  0));

  for (struct_or_union_content::member_iterator it_m = sc.members_begin();
       it_m != sc.members_end(); ++it_m) {
    rli.place_union_field(*it_m);
  }

  rli.finish_record_layout(sc);
}

std::shared_ptr<const types::object_type>
target_x86_64_gcc::create_builtin_va_list_type() const
{
  // Use a common dummy struct_or_union_def node such that all
  // struct_or_union_type instances created here refer to the same
  // type.
  static std::unique_ptr<ast::struct_or_union_def> soud;

  if (!soud) {
    soud.reset(new ast::struct_or_union_def(pp_tokens_range{},
					    struct_or_union_kind::souk_struct,
					    nullptr, nullptr, nullptr));

    struct_or_union_content &c = soud->get_content();
    c.add_member
      (struct_or_union_content::member
       ("gp_offset",
	std_int_type::create(std_int_type::kind::k_int, false)));
    c.add_member
      (struct_or_union_content::member
       ("fp_offset",
	std_int_type::create(std_int_type::kind::k_int, false)));
    c.add_member
      (struct_or_union_content::member
       ("overflow_arg_area",
	void_type::create()->derive_pointer()));
    c.add_member
      (struct_or_union_content::member
       ("reg_save_area",
	void_type::create()->derive_pointer()));

    this->layout_struct(c, alignment{});
  }

  return (struct_or_union_type::create(struct_or_union_kind::souk_struct,
				       soud->get_decl_list_node())
	  ->derive_array(mpa::limbs::from_size_type(1)));
}

types::std_int_type::kind
target_x86_64_gcc::get_wint_kind() const noexcept
{
  return std_int_type::kind::k_int;
}

bool target_x86_64_gcc::is_wint_signed() const noexcept
{
  return true;
}

types::std_int_type::kind target_x86_64_gcc::get_int_max_kind() const noexcept
{
  return std_int_type::kind::k_long;
}

types::std_int_type::kind target_x86_64_gcc::get_pid_t_kind() const noexcept
{
  return std_int_type::kind::k_int;
}

bool target_x86_64_gcc::is_pid_t_signed() const noexcept
{
  return true;
}

types::std_int_type::kind
target_x86_64_gcc::_width_to_int_kind(const mpa::limbs::size_type w) noexcept
{
  switch (w) {
  case 8:
    return types::std_int_type::kind::k_char;

  case 16:
    return types::std_int_type::kind::k_short;

  case 32:
    return types::std_int_type::kind::k_int;

  case 64:
    return types::std_int_type::kind::k_long;

  case 128:
    return types::std_int_type::kind::k_int128;

  default:
    assert(0);
    __builtin_unreachable();
  };
}


gcc_cmdline_parser::gcc_version
target_x86_64_gcc::_parse_version(const char * const version)
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
