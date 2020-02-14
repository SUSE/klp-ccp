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
#include "preprocessor.hh"
#include "cmdline_except.hh"
#include "target_x86_64_gcc.hh"

using namespace klp::ccp;
using namespace klp::ccp::types;

enum opt_code_i386
{
  opt_code_i386_unused = 0,

  opt_code_i386_m16,
  opt_code_i386_m32,
  opt_code_i386_m64,
  opt_code_i386_mx32,

  opt_code_i386_m3dnow,
  opt_code_i386_m3dnowa,
  opt_code_i386_mabm,
  opt_code_i386_madx,
  opt_code_i386_maes,
  opt_code_i386_march,
  opt_code_i386_mavx,
  opt_code_i386_mavx2,
  opt_code_i386_mavx5124fmaps,
  opt_code_i386_mavx5124vnniw,
  opt_code_i386_mavx512bitalg,
  opt_code_i386_mavx512bw,
  opt_code_i386_mavx512cd,
  opt_code_i386_mavx512dq,
  opt_code_i386_mavx512er,
  opt_code_i386_mavx512f,
  opt_code_i386_mavx512ifma,
  opt_code_i386_mavx512pf,
  opt_code_i386_mavx512vbmi,
  opt_code_i386_mavx512vbmi2,
  opt_code_i386_mavx512vl,
  opt_code_i386_mavx512vnni,
  opt_code_i386_mavx512vpopcntdq,
  opt_code_i386_mbmi,
  opt_code_i386_mbmi2,
  opt_code_i386_mcldemote,
  opt_code_i386_mclflushopt,
  opt_code_i386_mclwb,
  opt_code_i386_mclzero,
  opt_code_i386_mcrc32,
  opt_code_i386_mcx16,
  opt_code_i386_mf16c,
  opt_code_i386_mfma,
  opt_code_i386_mfma4,
  opt_code_i386_mfsgsbase,
  opt_code_i386_mfxsr,
  opt_code_i386_mgeneral_regs_only,
  opt_code_i386_mgfni,
  opt_code_i386_mhle,
  opt_code_i386_mlwp,
  opt_code_i386_mlzcnt,
  opt_code_i386_mmmx,
  opt_code_i386_mmovbe,
  opt_code_i386_mmovdir64b,
  opt_code_i386_mmovdiri,
  opt_code_i386_mmpx,
  opt_code_i386_mmwaitx,
  opt_code_i386_mno_sse4,
  opt_code_i386_mpclmul,
  opt_code_i386_mpcommit,
  opt_code_i386_mpconfig,
  opt_code_i386_mpku,
  opt_code_i386_mpopcnt,
  opt_code_i386_mprefetchwt1,
  opt_code_i386_mprfchw,
  opt_code_i386_mptwrite,
  opt_code_i386_mrdpid,
  opt_code_i386_mrdrnd,
  opt_code_i386_mrdseed,
  opt_code_i386_mrtm,
  opt_code_i386_msahf,
  opt_code_i386_msgx,
  opt_code_i386_msha,
  opt_code_i386_mshstk,
  opt_code_i386_msse,
  opt_code_i386_msse2,
  opt_code_i386_msse3,
  opt_code_i386_msse4,
  opt_code_i386_msse4_1,
  opt_code_i386_msse4_2,
  opt_code_i386_msse4a,
  opt_code_i386_mssse3,
  opt_code_i386_mtbm,
  opt_code_i386_mtune,
  opt_code_i386_mvaes,
  opt_code_i386_mvzeroupper,
  opt_code_i386_mvpclmulqdq,
  opt_code_i386_mwaitpkg,
  opt_code_i386_mwbnoinvd,
  opt_code_i386_mxop,
  opt_code_i386_mxsave,
  opt_code_i386_mxsavec,
  opt_code_i386_mxsaveopt,
  opt_code_i386_mxsaves,
};

static gcc_cmdline_parser::option gcc_opt_table_i386[] = {
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
  : target_gcc(version), _opts_x86(*this)
{
  _builtin_typedefs.emplace_back("__builtin_va_list",
				 _builtin_typedef_va_list::create);
  _builtin_typedefs.emplace_back("__int128_t",
				 _builtin_typedef__int128::create_signed);
  _builtin_typedefs.emplace_back("__uint128_t",
				 _builtin_typedef__int128::create_unsigned);
}

target_x86_64_gcc::~target_x86_64_gcc() noexcept = default;

const builtin_typedef::factories& target_x86_64_gcc::get_builtin_typedefs()
  const noexcept
{
  return _builtin_typedefs;
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

const gcc_cmdline_parser::option *
target_x86_64_gcc::_arch_get_opt_table() const noexcept
{
  return gcc_opt_table_i386;
}

const target_gcc::default_option&
target_x86_64_gcc::_arch_get_option_optimization_table() const noexcept
{
  static const default_option default_options_table[] = {
    { default_option::opt_levels_2_plus, "free", nullptr, false },
    {
      default_option::opt_levels_2_plus, "freorder-blocks-and-partition",
      nullptr, false,
      .min_gcc_version = {4, 9, 0},
    },
    {
      default_option::opt_levels_2_plus, "freorder-blocks-algorithm", "stc",
      false,
      .min_gcc_version = {6, 1, 0},
    },
    { default_option::opt_levels_all, "fschedule-insns", nullptr, true },

    { default_option::opt_levels_none }
  };

  return *default_options_table;
}

bool target_x86_64_gcc::_arch_default_char_is_signed() const noexcept
{
  return true;
}

void target_x86_64_gcc::_arch_option_init_struct()
{
  // This corresponds to GCC's ix86_option_init_struct().
  _opts_x86.option_init_struct();
}

void
target_x86_64_gcc::_arch_handle_opt(const gcc_cmdline_parser::option * const o,
				    const char *val, const bool negative,
				    const bool generated)
{
  _opts_x86.handle_opt(o, val, negative, generated);
}

void target_x86_64_gcc::_arch_option_override()
{
  // This corresponds to GCC's ix86_option_override().
  _opts_x86.option_override();
}

void target_x86_64_gcc::_arch_register_builtin_macros(preprocessor &pp) const
{
  const std::initializer_list<std::pair<const char *, const char*>>
    builtin_object_macros = {
	{ "__x86_64__", "1" },
	{ "__x86_64", "1" },
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
  };

  for (const auto &bom : builtin_object_macros)
    pp.register_builtin_macro(bom.first, bom.second);
}


target_x86_64_gcc::opts_x86::
opts_x86(target_x86_64_gcc &t) noexcept
  : _t(t),
    _valid_isa_flags(_init_valid_isa_flags(t.get_gcc_version())),
    _arch(nullptr), _tune(nullptr)
{
  // GCC's ix86_isa_flags is initialized with
  // TARGET_64BIT_DEFAULT | TARGET_SUBTARGET_ISA_DEFAULT.
  __set_isa_flag<isa_flag_64bit>(_isa_flags);
  __set_isa_flag<isa_flag_abi_64>(_isa_flags);
}

void target_x86_64_gcc::opts_x86::option_init_struct() noexcept
{
  // Strictly speaking, this gets set in GCC's init_options_struct(),
  // which would correspond more to
  // target_gcc::_init_options_struct().  However, the value depends
  // on the target, set it here.
  _t.get_opts_common().flag_unwind_tables = false;

  _t.get_opts_common().flag_asynchronous_unwind_tables = 2;
}

target_x86_64_gcc::opts_x86::_isa_flags_type target_x86_64_gcc::opts_x86::
_init_valid_isa_flags(const gcc_cmdline_parser::gcc_version &ver)
{
  _isa_flags_type flags;
  flags.set();

  using gcc_version = gcc_cmdline_parser::gcc_version;

  if (ver < gcc_version{4, 9, 0}) {
    flags.reset(isa_flag_code16);
    flags.reset(isa_flag_avx512cd);
    flags.reset(isa_flag_avx512er);
    flags.reset(isa_flag_avx512f);
    flags.reset(isa_flag_avx512pf);
    flags.reset(isa_flag_prefetchwt1);
    flags.reset(isa_flag_sha);
  }

  if (ver < gcc_version{5, 1, 0}) {
    flags.reset(isa_flag_avx512bw);
    flags.reset(isa_flag_avx512dq);
    flags.reset(isa_flag_avx512ifma);
    flags.reset(isa_flag_avx512vbmi);
    flags.reset(isa_flag_avx512vl);
    flags.reset(isa_flag_clflushopt);
    flags.reset(isa_flag_clwb);
    flags.reset(isa_flag_mpx);
    flags.reset(isa_flag_pcommit);
    flags.reset(isa_flag_xsavec);
    flags.reset(isa_flag_xsaves);
  }

  if (ver < gcc_version{5, 2, 0})
    flags.reset(isa_flag_mwaitx);

  if (ver.maj == 5 &&
      gcc_version{5, 4, std::numeric_limits<unsigned int>::max()} < ver)
    flags.reset(isa_flag_pcommit);

  if (ver < gcc_version{6, 1, 0}) {
    flags.reset(isa_flag_clzero);
    flags.reset(isa_flag_pku);
  }

  if (gcc_version{6, 2, std::numeric_limits<unsigned int>::max()} < ver)
    flags.reset(isa_flag_pcommit);

  if (ver < gcc_version{7, 1, 0}) {
    flags.reset(isa_flag_avx5124fmaps);
    flags.reset(isa_flag_avx5124vnniw);
    flags.reset(isa_flag_avx512vpopcntdq);
    flags.reset(isa_flag_rdpid);
    flags.reset(isa_flag_sgx);
  }

  if (ver < gcc_version{8, 1, 0}) {
    flags.reset(isa_flag_avx512bitalg);
    flags.reset(isa_flag_avx512vbmi2);
    flags.reset(isa_flag_avx512vnni);
    flags.reset(isa_flag_gfni);
    flags.reset(isa_flag_movdir64b);
    flags.reset(isa_flag_movdiri);
    flags.reset(isa_flag_pconfig);
    flags.reset(isa_flag_shstk);
    flags.reset(isa_flag_vaes);
    flags.reset(isa_flag_vpclmulqdq);
    flags.reset(isa_flag_wbnoinvd);
  }

  if (ver < gcc_version{9, 1, 0}) {
    flags.reset(isa_flag_cldemote);
    flags.reset(isa_flag_ptwrite);
    flags.reset(isa_flag_waitpkg);
  } else {
    flags.reset(isa_flag_mpx);
  }

  return flags;
}

// The following mimic the OPTION_MASK_ISA_<FOO>_SET #defines from the
// GCC sources.
template <target_x86_64_gcc::opts_x86::isa_flag b>
void target_x86_64_gcc::opts_x86::__set_isa_flag(_isa_flags_type &flags)
  const noexcept
{
  assert(_valid_isa_flags.test(b));
  flags.set(b);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_3dnow>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_3dnow));
  flags.set(isa_flag_3dnow);
  __set_isa_flag<isa_flag_mmx>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_3dnow_a>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_3dnow_a));
  flags.set(isa_flag_3dnow_a);
  __set_isa_flag<isa_flag_3dnow>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_abm>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_abm));
  flags.set(isa_flag_abm);
  __set_isa_flag<isa_flag_popcnt>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse2>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_sse2));
  flags.set(isa_flag_sse2);
  __set_isa_flag<isa_flag_sse>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_aes>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_aes));
  flags.set(isa_flag_aes);
  __set_isa_flag<isa_flag_sse2>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse3>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_sse3));
  flags.set(isa_flag_sse3);
  __set_isa_flag<isa_flag_sse2>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_ssse3>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_ssse3));
  flags.set(isa_flag_ssse3);
  __set_isa_flag<isa_flag_sse3>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse4_1>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_sse4_1));
  flags.set(isa_flag_sse4_1);
  __set_isa_flag<isa_flag_ssse3>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse4_2>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_sse4_2));
  flags.set(isa_flag_sse4_2);
  __set_isa_flag<isa_flag_sse4_1>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx));
  flags.set(isa_flag_avx);
  __set_isa_flag<isa_flag_sse4_2>(flags);
  __set_isa_flag<isa_flag_xsave>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx2>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx2));
  flags.set(isa_flag_avx2);
  __set_isa_flag<isa_flag_avx>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512f>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512f));
  flags.set(isa_flag_avx512f);
  __set_isa_flag<isa_flag_avx2>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512bitalg>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512bitalg));
  flags.set(isa_flag_avx512bitalg);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512bw>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512bw));
  flags.set(isa_flag_avx512bw);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512cd>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512cd));
  flags.set(isa_flag_avx512cd);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512dq>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512dq));
  flags.set(isa_flag_avx512dq);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512er>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512er));
  flags.set(isa_flag_avx512er);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512ifma>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512ifma));
  flags.set(isa_flag_avx512ifma);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512pf>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512pf));
  flags.set(isa_flag_avx512pf);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512vbmi>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512vbmi));
  flags.set(isa_flag_avx512vbmi);
  __set_isa_flag<isa_flag_avx512bw>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512vbmi2>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512vbmi2));
  flags.set(isa_flag_avx512vbmi2);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512vl>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512vl));
  flags.set(isa_flag_avx512vl);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512vnni>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512vnni));
  flags.set(isa_flag_avx512vnni);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512vpopcntdq>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_avx512vpopcntdq));
  flags.set(isa_flag_avx512vpopcntdq);
  __set_isa_flag<isa_flag_avx512f>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_f16c>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_f16c));
  flags.set(isa_flag_f16c);
  __set_isa_flag<isa_flag_avx>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_fma>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_fma));
  flags.set(isa_flag_fma);
  __set_isa_flag<isa_flag_avx>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse4a>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_sse4a));
  flags.set(isa_flag_sse4a);
  __set_isa_flag<isa_flag_sse3>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_fma4>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_fma4));
  flags.set(isa_flag_fma4);
  __set_isa_flag<isa_flag_sse4a>(flags);
  __set_isa_flag<isa_flag_avx>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_pclmul>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_pclmul));
  flags.set(isa_flag_pclmul);
  __set_isa_flag<isa_flag_sse2>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sha>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_sha));
  flags.set(isa_flag_sha);
  __set_isa_flag<isa_flag_sse2>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_xop>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_xop));
  flags.set(isa_flag_xop);
  __set_isa_flag<isa_flag_fma4>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_xsavec>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_xsavec));
  flags.set(isa_flag_xsavec);
  __set_isa_flag<isa_flag_xsave>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_xsaveopt>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_xsaveopt));
  flags.set(isa_flag_xsaveopt);
  __set_isa_flag<isa_flag_xsave>(flags);
}

template <>
void target_x86_64_gcc::opts_x86::
__set_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_xsaves>
			(_isa_flags_type &flags) const noexcept
{
  assert(_valid_isa_flags.test(isa_flag_xsaves));
  flags.set(isa_flag_xsaves);
  __set_isa_flag<isa_flag_xsave>(flags);
}

template <target_x86_64_gcc::opts_x86::isa_flag b>
void target_x86_64_gcc::opts_x86::_set_isa_flag_explicit() noexcept
{
  assert(_valid_isa_flags.test(b));
  __set_isa_flag<b>(_isa_flags);
  // Record that the given flag had been set explicitly.
  __set_isa_flag<b>(_isa_flags_explicit);
}

// The following mimic the OPTION_MASK_ISA_<FOO>_UNSET #defines from the
// GCC sources.
template <target_x86_64_gcc::opts_x86::isa_flag b>
void target_x86_64_gcc::opts_x86::__unset_isa_flag(_isa_flags_type &flags,
						   const bool set)
  const noexcept
{
  flags.set(b, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_3dnow>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_3dnow, set);
  __unset_isa_flag<isa_flag_3dnow_a>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512bw>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_avx512bw, set);
  __unset_isa_flag<isa_flag_avx512vbmi>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx512f>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_avx512f, set);
  __unset_isa_flag<isa_flag_avx512bitalg>(flags, set);
  __unset_isa_flag<isa_flag_avx512cd>(flags, set);
  __unset_isa_flag<isa_flag_avx512dq>(flags, set);
  __unset_isa_flag<isa_flag_avx512er>(flags, set);
  __unset_isa_flag<isa_flag_avx512ifma>(flags, set);
  __unset_isa_flag<isa_flag_avx512pf>(flags, set);
  __unset_isa_flag<isa_flag_avx512vbmi2>(flags, set);
  __unset_isa_flag<isa_flag_avx512vnni>(flags, set);
  __unset_isa_flag<isa_flag_avx512vl>(flags, set);

  using gcc_version = gcc_cmdline_parser::gcc_version;
  if (gcc_version{8, 1, 0} <= _t.get_gcc_version()) {
    // OPTION_MASK_ISA_AVX5124FMAPS, OPTION_MASK_ISA_AVX5124VNNIW and
    // OPTION_MASK_ISA_AVX512VPOPCNTDQ have been added with GCC
    // 7.1.0. However, they only became part of
    // OPTION_MASK_ISA_AVX512F_UNSET with GCC 8.1.0. Before that
    // version, they got explicly cleared as part of-mno-avx512f
    // option handling. Adding these flags to
    // OPTION_MASK_ISA_AVX512F_UNSET also covers the -mno-avx2,
    // -mno-avx, ..., -mno-sse cases as well.
    __unset_isa_flag<isa_flag_avx5124fmaps>(flags, set);
    __unset_isa_flag<isa_flag_avx5124vnniw>(flags, set);
    __unset_isa_flag<isa_flag_avx512vpopcntdq>(flags, set);
  }

  if (gcc_version{9, 1, 0} <= _t.get_gcc_version()) {
    // This had been missing from OPTION_MASK_ISA_AVX512F_UNSET until
    // GCC 9.1.0.
    __unset_isa_flag<isa_flag_avx512ifma>(flags, set);
  }
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx2>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_avx2, set);
  __unset_isa_flag<isa_flag_avx512f>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_fma4>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_fma4, set);
  __unset_isa_flag<isa_flag_xop>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_xsave>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  using gcc_version = gcc_cmdline_parser::gcc_version;

  flags.set(isa_flag_xsave, set);
  __unset_isa_flag<isa_flag_xsaveopt>(flags, set);

  if (gcc_version{8, 2, 0} <= _t.get_gcc_version()) {
    // These had been added to OPTION_MASK_ISA_XSAVE_UNSET with GCC
    // 8.2.0 only.
    __unset_isa_flag<isa_flag_xsavec>(flags, set);
    __unset_isa_flag<isa_flag_xsaves>(flags, set);
  }
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_avx>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_avx, set);
  __unset_isa_flag<isa_flag_avx2>(flags, set);
  __unset_isa_flag<isa_flag_f16c>(flags, set);
  __unset_isa_flag<isa_flag_fma>(flags, set);
  __unset_isa_flag<isa_flag_fma4>(flags, set);
  __unset_isa_flag<isa_flag_xsave>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_mmx>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_mmx, set);
  __unset_isa_flag<isa_flag_3dnow>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse4_2>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_sse4_2, set);
  __unset_isa_flag<isa_flag_avx>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse4_1>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_sse4_1, set);
  __unset_isa_flag<isa_flag_sse4_2>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse4a>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_sse4a, set);
  __unset_isa_flag<isa_flag_fma4>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_ssse3>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_ssse3, set);
  __unset_isa_flag<isa_flag_sse4_1>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse3>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_sse3, set);
  __unset_isa_flag<isa_flag_sse4a>(flags, set);
  __unset_isa_flag<isa_flag_ssse3>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse2>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_sse2, set);
  __unset_isa_flag<isa_flag_sse3>(flags, set);
}

template <>
void target_x86_64_gcc::opts_x86::
__unset_isa_flag<target_x86_64_gcc::opts_x86::isa_flag_sse>
			(_isa_flags_type &flags, const bool set) const noexcept
{
  flags.set(isa_flag_sse, set);
  __unset_isa_flag<isa_flag_sse2>(flags, set);
}

template <target_x86_64_gcc::opts_x86::isa_flag b>
void target_x86_64_gcc::opts_x86::_unset_isa_flag_explicit() noexcept
{
  __unset_isa_flag<b>(_isa_flags, false);
  // Record that the given flag had been unset explicitly.
  __unset_isa_flag<b>(_isa_flags_explicit, true);
  _isa_flags_explicit &= _valid_isa_flags;
}

template <target_x86_64_gcc::opts_x86::isa_flag b>
void target_x86_64_gcc::opts_x86::_set_isa_flag_user(const bool value,
						     const bool generated)
  noexcept
{
  if (!_valid_isa_flags.test(b))
    return;

  if (!generated)
    _isa_flags_set.set(b, true);

  if (value)
    _set_isa_flag_explicit<b>();
  else
    _unset_isa_flag_explicit<b>();
}

struct target_x86_64_gcc::opts_x86::_pta
{
  // These correspond to the PTA_<FOO> bits from the GCC sources.
  enum pta_flag
  {
    pta_flag_3dnow,
    pta_flag_3dnow_a,
    pta_flag_64bit,
    pta_flag_abm,
    pta_flag_adx,
    pta_flag_aes,
    pta_flag_avx,
    pta_flag_avx2,
    pta_flag_avx5124fmaps,
    pta_flag_avx5124vnniw,
    pta_flag_avx512bitalg,
    pta_flag_avx512bw,
    pta_flag_avx512cd,
    pta_flag_avx512dq,
    pta_flag_avx512er,
    pta_flag_avx512f,
    pta_flag_avx512ifma,
    pta_flag_avx512pf,
    pta_flag_avx512vbmi,
    pta_flag_avx512vbmi2,
    pta_flag_avx512vl,
    pta_flag_avx512vnni,
    pta_flag_avx512vpopcntdq,
    pta_flag_bmi,
    pta_flag_bmi2,
    pta_flag_clflushopt,
    pta_flag_clwb,
    pta_flag_clzero,
    pta_flag_cx16,
    pta_flag_f16c,
    pta_flag_fma,
    pta_flag_fma4,
    pta_flag_fsgsbase,
    pta_flag_fxsr,
    pta_flag_gfni,
    pta_flag_hle,
    pta_flag_lwp,
    pta_flag_lzcnt,
    pta_flag_mmx,
    pta_flag_movbe,
    pta_flag_mwaitx,
    pta_flag_no_80387,
    pta_flag_no_sahf,
    pta_flag_pclmul,
    pta_flag_pconfig,
    pta_flag_pku,
    pta_flag_popcnt,
    pta_flag_prefetchwt1,
    pta_flag_prefetch_sse,
    pta_flag_prfchw,
    pta_flag_ptwrite,
    pta_flag_rdpid,
    pta_flag_rdrnd,
    pta_flag_rdseed,
    pta_flag_rtm,
    pta_flag_sgx,
    pta_flag_sha,
    pta_flag_sse,
    pta_flag_sse2,
    pta_flag_sse3,
    pta_flag_sse4a,
    pta_flag_sse4_1,
    pta_flag_sse4_2,
    pta_flag_ssse3,
    pta_flag_tbm,
    pta_flag_vaes,
    pta_flag_vpclmulqdq,
    pta_flag_waitpkg,
    pta_flag_wbnoinvd,
    pta_flag_xop,
    pta_flag_xsave,
    pta_flag_xsavec,
    pta_flag_xsaveopt,
    pta_flag_xsaves,

    _pta_flag_max,
  };

  using gcc_version = gcc_cmdline_parser::gcc_version;

  typedef std::bitset<_pta_flag_max> pta_flags_type;

  typedef void(*apply_flags_type)(pta_flags_type &flags,
				  const gcc_version &ver);

  enum processor_type
  {
    processor_generic = 0,
    processor_generic32,
    processor_generic64,
    processor_i386,
    processor_i486,
    processor_pentium,
    processor_lakemont,
    processor_pentiumpro,
    processor_pentium4,
    processor_nocona,
    processor_core2,
    processor_corei7,
    processor_nehalem,
    processor_sandybridge,
    processor_haswell,
    processor_atom,
    processor_bonnell,
    processor_silvermont,
    processor_goldmont,
    processor_goldmont_plus,
    processor_tremont,
    processor_knl,
    processor_knm,
    processor_skylake,
    processor_skylake_avx512,
    processor_cannonlake,
    processor_icelake_client,
    processor_icelake_server,
    processor_cascadelake,
    processor_intel,
    processor_geode,
    processor_k6,
    processor_athlon,
    processor_k8,
    processor_amdfam10,
    processor_bdver1,
    processor_bdver2,
    processor_bdver3,
    processor_bdver4,
    processor_btver1,
    processor_btver2,
    processor_znver1,
    processor_znver2,

    _processor_max
  };

  const char * const name;
  const processor_type processor;
  const apply_flags_type apply_flags;
  const gcc_version min_gcc_version;
  const gcc_version max_gcc_version;

  static const struct _pta*
  lookup(const char * const name, const gcc_version &ver)
    noexcept;

private:
  template<pta_flag... bits>
  struct __flags_set;

  template<pta_flag bit, pta_flag... bits>
  struct __flags_set<bit, bits...>
  {
    static void apply(pta_flags_type &flags, const gcc_version &ver) noexcept
    {
      flags.set(bit);
      __flags_set<bits...>::apply(flags, ver);
    }
  };

  template<typename base_flags_set, pta_flag... bits>
  struct _flags_set
  {
    static void apply(pta_flags_type &flags, const gcc_version &ver) noexcept
    {
      base_flags_set::apply(flags, ver);
      __flags_set<bits...>::apply(flags, ver);
    }
  };
};

template<>
struct target_x86_64_gcc::opts_x86::_pta::__flags_set<>
{
  static void apply(pta_flags_type &flags, const gcc_version &ver) noexcept
  {}
};

const target_x86_64_gcc::opts_x86::_pta*
target_x86_64_gcc::opts_x86::_pta::
lookup(const char * const name, const gcc_version &ver)
  noexcept
{
  using flags_set_core2 =
    __flags_set<pta_flag_64bit, pta_flag_mmx,
		pta_flag_sse, pta_flag_sse2,
		pta_flag_sse3, pta_flag_ssse3,
		pta_flag_cx16, pta_flag_fxsr>;
  using flags_set_nehalem =
    _flags_set<flags_set_core2,
	       pta_flag_sse4_1, pta_flag_sse4_2,
	       pta_flag_popcnt>;

  struct flags_set_westmere
  {
    static void apply(pta_flags_type &flags, const gcc_version &ver)
    {
      _flags_set<flags_set_nehalem, pta_flag_pclmul>::apply(flags, ver);

      if (!(gcc_version{9, 1, 0} <= ver))
	__flags_set<pta_flag_aes>::apply(flags, ver);
    }
  };

  using flags_set_sandybridge =
    _flags_set<flags_set_westmere,
	       pta_flag_avx, pta_flag_xsave,
	       pta_flag_xsaveopt>;
  using flags_set_ivybridge =
    _flags_set<flags_set_sandybridge,
	       pta_flag_fsgsbase, pta_flag_rdrnd,
	       pta_flag_f16c>;

  struct flags_set_haswell
  {
    static void apply(pta_flags_type &flags, const gcc_version &ver)
    {
      _flags_set<flags_set_ivybridge,
		 pta_flag_avx2, pta_flag_bmi,
		 pta_flag_bmi2, pta_flag_lzcnt,
		 pta_flag_fma, pta_flag_movbe,
		 pta_flag_hle>::apply(flags, ver);
      if (ver == gcc_version{4, 9, 0})
	__flags_set<pta_flag_rtm>::apply(flags, ver);
    }
  };

  using flags_set_broadwell =
    _flags_set<flags_set_haswell,
	       pta_flag_adx, pta_flag_prfchw,
	       pta_flag_rdseed>;

  struct flags_set_skylake
  {
    static void apply(pta_flags_type &flags, const gcc_version &ver)
    {
      _flags_set<flags_set_broadwell,
		 pta_flag_clflushopt, pta_flag_xsavec,
		 pta_flag_xsaves>::apply(flags, ver);
      if (gcc_version{8, 1, 0} <= ver)
	__flags_set<pta_flag_sgx>::apply(flags, ver);
      if (gcc_version{9, 1, 0} <= ver)
	__flags_set<pta_flag_aes>::apply(flags, ver);
    }
  };

  struct flags_set_skylake_avx512
  {
    static void apply(pta_flags_type &flags, const gcc_version &ver)
    {
      _flags_set<flags_set_skylake,
		 pta_flag_avx512f, pta_flag_avx512cd,
		 pta_flag_avx512vl, pta_flag_avx512bw,
		 pta_flag_avx512dq, pta_flag_pku>::apply(flags, ver);
      if (gcc_version{8, 1, 0} <= ver)
	__flags_set<pta_flag_clwb>::apply(flags, ver);
    }
  };

  using flags_set_cascadelake =
    _flags_set<flags_set_skylake_avx512,
	       pta_flag_avx512vnni>;
  using flags_set_cannonlake =
    _flags_set<flags_set_skylake,
	       pta_flag_avx512f, pta_flag_avx512cd,
	       pta_flag_avx512vl, pta_flag_avx512bw,
	       pta_flag_avx512dq, pta_flag_pku,
	       pta_flag_avx512vbmi, pta_flag_avx512ifma,
	       pta_flag_sha>;
  using flags_set_icelake_client =
    _flags_set<flags_set_cannonlake,
	       pta_flag_avx512vnni, pta_flag_gfni,
	       pta_flag_vaes, pta_flag_avx512vbmi2,
	       pta_flag_vpclmulqdq, pta_flag_avx512bitalg,
	       pta_flag_rdpid, pta_flag_clwb>;
  using flags_set_icelake_server =
    _flags_set<flags_set_icelake_client,
	       pta_flag_pconfig, pta_flag_wbnoinvd>;
  using flags_set_knl =
    _flags_set<flags_set_broadwell,
	       pta_flag_avx512pf, pta_flag_avx512er,
	       pta_flag_avx512f, pta_flag_avx512cd>;
  using flags_set_bonnell =
    _flags_set<flags_set_core2, pta_flag_movbe>;

  struct flags_set_silvermont
  {
    static void apply(pta_flags_type &flags, const gcc_version &ver)
    {
      _flags_set<flags_set_westmere, pta_flag_movbe>::apply(flags, ver);
      if (gcc_version{8, 1, 0} <= ver)
	__flags_set<pta_flag_rdrnd>::apply(flags, ver);
    }
  };

  using flags_set_goldmont =
    _flags_set<flags_set_silvermont,
	       pta_flag_aes, pta_flag_sha,
	       pta_flag_xsave, pta_flag_rdseed,
	       pta_flag_xsavec, pta_flag_xsaves,
	       pta_flag_clflushopt, pta_flag_xsaveopt,
	       pta_flag_fsgsbase>;
  using flags_set_goldmont_plus =
    _flags_set<flags_set_goldmont,
	       pta_flag_rdpid, pta_flag_sgx,
	       pta_flag_ptwrite>;
  using flags_set_tremont =
    _flags_set<flags_set_goldmont_plus,
	       pta_flag_clwb, pta_flag_gfni>;
  using flags_set_knm =
    _flags_set<flags_set_knl,
	       pta_flag_avx5124vnniw, pta_flag_avx5124fmaps,
	       pta_flag_avx512vpopcntdq>;

  // This corresponds to the processor_alias_table from the GCC
  // sources.
  static const _pta processor_alias_table[] = {
    { "i386", processor_i386, __flags_set<>::apply },
    { "i486", processor_i486, __flags_set<>::apply },
    { "i586", processor_pentium, __flags_set<>::apply },
    { "pentium", processor_pentium, __flags_set<>::apply },
    {
      "lakemont", processor_lakemont,
      __flags_set<pta_flag_no_80387>::apply,
      .min_gcc_version = {6, 1, 0},
    },
    {
      "pentium-mmx", processor_pentium,
      __flags_set<pta_flag_mmx>::apply
    },
    {
      "winchip-c6", processor_i486,
      __flags_set<pta_flag_mmx>::apply
    },
    {
      "winchip2", processor_i486,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "winchip2", processor_i486,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_prfchw>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "winchip2", processor_i486,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "c3", processor_i486,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "c3", processor_i486,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_prfchw>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "c3", processor_i486,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "samuel-2", processor_i486,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "c3-2", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_sse>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "c3-2", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
    },
    {
      "nehemiah", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "c7", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_sse2, pta_flag_sse3,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "esther", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_sse2, pta_flag_sse3,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    { "i686", processor_pentiumpro, __flags_set<>::apply },
    { "pentiumpro", processor_pentiumpro, __flags_set<>::apply },
    {
      "pentium2", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_fxsr>::apply
    },
    {
      "pentium3", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_fxsr>::apply
    },
    {
      "pentium3m", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_fxsr>::apply
    },
    {
      "pentium-m", processor_pentiumpro,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_sse2, pta_flag_fxsr>::apply
    },
    {
      "pentium4", processor_pentium4,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_sse2, pta_flag_fxsr>::apply
    },
    {
      "pentium4m", processor_pentium4,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_sse2, pta_flag_fxsr>::apply
    },
    {
      "prescott", processor_nocona,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_sse2, pta_flag_sse3,
		  pta_flag_fxsr>::apply
    },
    {
      "nocona", processor_nocona,
      __flags_set<pta_flag_mmx, pta_flag_sse,
		  pta_flag_sse2, pta_flag_sse3,
		  pta_flag_cx16, pta_flag_no_sahf,
		  pta_flag_fxsr>::apply
    },
    {
      "core2", processor_core2,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_cx16, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 5},
    },
    {
      "core2", processor_core2, flags_set_core2::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "nehalem", processor_core2, flags_set_nehalem::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "corei7", processor_corei7,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_sse4_2,
		  pta_flag_cx16, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "corei7", processor_corei7,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_sse4_2,
		  pta_flag_cx16, pta_flag_popcnt,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {4, 8, 5},
    },
    {
      "corei7", processor_nehalem, flags_set_nehalem::apply,
      .min_gcc_version = {4, 9, 0}
    },
    {
      "westmere", processor_nehalem, flags_set_westmere::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "sandybridge", processor_sandybridge,
      flags_set_sandybridge::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "corei7-avx", processor_corei7,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_sse4_2,
		  pta_flag_avx, pta_flag_cx16,
		  pta_flag_popcnt, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_fxsr,
		  pta_flag_xsave, pta_flag_xsaveopt>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 5}
    },
    {
      "corei7-avx", processor_nehalem,
      flags_set_sandybridge::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "ivybridge", processor_sandybridge, flags_set_ivybridge::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "core-avx-i", processor_corei7,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_sse4_2,
		  pta_flag_avx, pta_flag_cx16,
		  pta_flag_popcnt, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_fsgsbase,
		  pta_flag_rdrnd, pta_flag_f16c,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 5},
    },
    {
      "core-avx-i", processor_sandybridge, flags_set_ivybridge::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "haswell", processor_haswell, flags_set_haswell::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "core-avx2", processor_haswell,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_sse4_2,
		  pta_flag_avx, pta_flag_avx2,
		  pta_flag_cx16, pta_flag_popcnt,
		  pta_flag_aes, pta_flag_pclmul,
		  pta_flag_fsgsbase, pta_flag_rdrnd,
		  pta_flag_f16c, pta_flag_bmi,
		  pta_flag_bmi2, pta_flag_lzcnt,
		  pta_flag_fma, pta_flag_movbe,
		  pta_flag_rtm, pta_flag_hle,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 2},
    },
    {
      "core-avx2", processor_haswell,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_sse4_2,
		  pta_flag_avx, pta_flag_avx2,
		  pta_flag_cx16, pta_flag_popcnt,
		  pta_flag_aes, pta_flag_pclmul,
		  pta_flag_fsgsbase, pta_flag_rdrnd,
		  pta_flag_f16c, pta_flag_bmi,
		  pta_flag_bmi2, pta_flag_lzcnt,
		  pta_flag_fma, pta_flag_movbe,
		  pta_flag_hle, pta_flag_fxsr,
		  pta_flag_xsave, pta_flag_xsaveopt>::apply,
      .min_gcc_version = {4, 8, 3},
      .max_gcc_version = {4, 8, 5},
    },
    {
      "core-avx2", processor_haswell, flags_set_haswell::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "broadwell", processor_haswell, flags_set_broadwell::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "skylake", processor_haswell, flags_set_skylake::apply,
      .min_gcc_version = {6, 1, 0},
      .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
    },
    {
      "skylake", processor_skylake, flags_set_skylake::apply,
      .min_gcc_version = {8, 1, 0},
    },
    {
      "skylake-avx512", processor_haswell,
      flags_set_skylake_avx512::apply,
      .min_gcc_version = {6, 1, 0},
      .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
    },
    {
      "skylake-avx512", processor_skylake_avx512,
      flags_set_skylake_avx512::apply,
      .min_gcc_version = {8, 1, 0},
    },
    {
      "cannonlake", processor_cannonlake, flags_set_cannonlake::apply,
      .min_gcc_version = {8, 1, 0},
    },
    {
      "icelake-client", processor_icelake_client,
      flags_set_icelake_client::apply,
      .min_gcc_version = {8, 1, 0},
    },
    {
      "icelake-server", processor_icelake_server,
      flags_set_icelake_server::apply,
      .min_gcc_version = {8, 1, 0},
    },
    {
      "cascadelake", processor_cascadelake,
      flags_set_cascadelake::apply,
      .min_gcc_version = {9, 1, 0},
    },
    {
      "bonnell", processor_bonnell, flags_set_bonnell::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "atom", processor_atom,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_cx16, pta_flag_movbe,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 5},
    },
    {
      "atom", processor_bonnell, flags_set_bonnell::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "silvermont", processor_silvermont, flags_set_silvermont::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "slm", processor_silvermont, flags_set_silvermont::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "goldmont", processor_goldmont, flags_set_goldmont::apply,
      .min_gcc_version = {9, 1, 0},
    },
    {
      "goldmont-plus", processor_goldmont_plus,
      flags_set_goldmont_plus::apply,
      .min_gcc_version = {9, 1, 0},
    },
    {
      "tremont", processor_tremont, flags_set_tremont::apply,
      .min_gcc_version = {9, 1, 0},
    },
    {
      "knl", processor_knl, flags_set_knl::apply,
      .min_gcc_version = {5, 1, 0},
    },
    {
      "knm", processor_knm, flags_set_knm::apply,
      .min_gcc_version = {8, 1, 0},
    },
    {
      "intel", processor_intel, flags_set_nehalem::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "geode", processor_geode,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "geode", processor_geode,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse,
		  pta_flag_prfchw>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "geode", processor_geode,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    { "k6", processor_k6, __flags_set<pta_flag_mmx>::apply },
    {
      "k6-2", processor_k6,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "k6-2", processor_k6,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_prfchw>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "k6-2", processor_k6,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "k6-3", processor_k6,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "k6-3", processor_k6,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_prfchw>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "k6-3", processor_k6,
      __flags_set<pta_flag_mmx, pta_flag_3dnow>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "athlon", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "athlon", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse,
		  pta_flag_prfchw>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "athlon", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "athlon-tbird", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "athlon-tbird", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse,
		  pta_flag_prfchw>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "athlon-tbird", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_prefetch_sse>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "athlon-4", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "athlon-4", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse,
		  pta_flag_prfchw, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "athlon-4", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "athlon-xp", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "athlon-xp", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse,
		  pta_flag_prfchw, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "athlon-xp", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "athlon-mp", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "athlon-mp", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse,
		  pta_flag_prfchw, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "athlon-mp", processor_athlon,
      __flags_set<pta_flag_mmx, pta_flag_3dnow,
		  pta_flag_3dnow_a, pta_flag_sse,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "x86-64", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "x86-64", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
    },
    {
      "eden-x2", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "nano", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "nano-1000", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "nano-2000", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "nano-3000", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "nano-x2", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "eden-x4", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "nano-x4", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4_1, pta_flag_fxsr>::apply,
      .min_gcc_version = {7, 1, 0},
    },
    {
      "k8", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "k8", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_prfchw,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "k8", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "k8-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "k8-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf,
		  pta_flag_prfchw, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "k8-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "opteron", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "opteron", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_prfchw,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "opteron", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "opteron-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "opteron-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf,
		  pta_flag_prfchw, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "opteron-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "athlon64", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "athlon64", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_prfchw,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "athlon64", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "athlon64-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "athlon64-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf,
		  pta_flag_prfchw, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "athlon64-sse3", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_no_sahf,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "athlon-fx", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "athlon-fx", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_prfchw,
		  pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
      .max_gcc_version = {6, 2, std::numeric_limits<unsigned int>::max()},
    },
    {
      "athlon-fx", processor_k8,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_no_sahf, pta_flag_fxsr>::apply,
      .min_gcc_version = {6, 3, 0},
    },
    {
      "amdfam10", processor_amdfam10,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "amdfam10", processor_amdfam10,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_prfchw, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
    },
    {
      "barcelona", processor_amdfam10,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 0},
    },
    {
      "barcelona", processor_amdfam10,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_3dnow, pta_flag_3dnow_a,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_prfchw, pta_flag_fxsr>::apply,
      .min_gcc_version = {4, 8, 1},
    },
    {
      "bdver1", processor_bdver1,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_fma4, pta_flag_xop,
		  pta_flag_lwp, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave>::apply
    },
    {
      "bdver2", processor_bdver2,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_fma4, pta_flag_xop,
		  pta_flag_lwp, pta_flag_bmi,
		  pta_flag_tbm, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave>::apply
    },
    {
      "bdver3", processor_bdver3,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_xop, pta_flag_lwp,
		  pta_flag_bmi, pta_flag_tbm,
		  pta_flag_f16c, pta_flag_fma,
		  pta_flag_prfchw, pta_flag_fxsr,
      pta_flag_xsave, pta_flag_xsaveopt>::apply,
      .min_gcc_version = {4, 8, 0},
      .max_gcc_version = {4, 8, 2},
    },
    {
      "bdver3", processor_bdver3,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_fma4, pta_flag_xop,
		  pta_flag_lwp, pta_flag_bmi,
		  pta_flag_tbm, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt>::apply,
      .min_gcc_version = {4, 8, 3},
      .max_gcc_version = {4, 8, 5},
    },
    {
      "bdver3", processor_bdver3,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_fma4, pta_flag_xop,
		  pta_flag_lwp, pta_flag_bmi,
		  pta_flag_tbm, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt, pta_flag_fsgsbase>::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "bdver4", processor_bdver4,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_avx2, pta_flag_fma4,
		  pta_flag_xop, pta_flag_lwp,
		  pta_flag_bmi, pta_flag_bmi2,
		  pta_flag_tbm, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt, pta_flag_fsgsbase>::apply,
      .min_gcc_version = {4, 9, 0},
      .max_gcc_version = {4, 9, 1},
    },
    {
      "bdver4", processor_bdver4,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_avx2, pta_flag_fma4,
		  pta_flag_xop, pta_flag_lwp,
		  pta_flag_bmi, pta_flag_bmi2,
		  pta_flag_tbm, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt, pta_flag_fsgsbase,
		  pta_flag_rdrnd, pta_flag_movbe>::apply,
      .min_gcc_version = {4, 9, 2},
      .max_gcc_version = {5, 1, std::numeric_limits<unsigned int>::max()},
    },
    {
      "bdver4", processor_bdver4,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_avx2, pta_flag_fma4,
		  pta_flag_xop, pta_flag_lwp,
		  pta_flag_bmi, pta_flag_bmi2,
		  pta_flag_tbm, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt, pta_flag_fsgsbase,
		  pta_flag_rdrnd, pta_flag_movbe,
		  pta_flag_mwaitx>::apply,
      .min_gcc_version = {5, 2, 0},
    },
    {
      "znver1", processor_znver1,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_avx2, pta_flag_bmi,
		  pta_flag_bmi2, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt, pta_flag_fsgsbase,
		  pta_flag_rdrnd, pta_flag_movbe,
		  pta_flag_movbe, pta_flag_mwaitx,
		  pta_flag_adx, pta_flag_rdseed,
		  pta_flag_clzero, pta_flag_clflushopt,
		  pta_flag_xsavec, pta_flag_xsaves,
		  pta_flag_sha, pta_flag_lzcnt,
		  pta_flag_popcnt>::apply,
      .min_gcc_version = {6, 1, 0},
    },
    {
      "znver2", processor_znver1,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_avx2, pta_flag_bmi,
		  pta_flag_bmi2, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt, pta_flag_fsgsbase,
		  pta_flag_rdrnd, pta_flag_movbe,
		  pta_flag_movbe, pta_flag_mwaitx,
		  pta_flag_adx, pta_flag_rdseed,
		  pta_flag_clzero, pta_flag_clflushopt,
		  pta_flag_xsavec, pta_flag_xsaves,
		  pta_flag_sha, pta_flag_lzcnt,
		  pta_flag_popcnt, pta_flag_clwb,
		  pta_flag_rdpid, pta_flag_wbnoinvd>::apply,
      .min_gcc_version = {9, 1, 0},
      .max_gcc_version = {9, 1, std::numeric_limits<unsigned int>::max()},
    },
    {
      "znver2", processor_znver2,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_sse4a,
		  pta_flag_cx16, pta_flag_abm,
		  pta_flag_ssse3, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_avx2, pta_flag_bmi,
		  pta_flag_bmi2, pta_flag_f16c,
		  pta_flag_fma, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt, pta_flag_fsgsbase,
		  pta_flag_rdrnd, pta_flag_movbe,
		  pta_flag_movbe, pta_flag_mwaitx,
		  pta_flag_adx, pta_flag_rdseed,
		  pta_flag_clzero, pta_flag_clflushopt,
		  pta_flag_xsavec, pta_flag_xsaves,
		  pta_flag_sha, pta_flag_lzcnt,
		  pta_flag_popcnt, pta_flag_clwb,
		  pta_flag_rdpid, pta_flag_wbnoinvd>::apply,
      .min_gcc_version = {9, 2, 0},
    },
    {
      "btver1", processor_btver1,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4a, pta_flag_abm,
		  pta_flag_cx16, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave>::apply
    },
    {
      "btver2", processor_btver2,
      __flags_set<pta_flag_64bit, pta_flag_mmx,
		  pta_flag_sse, pta_flag_sse2,
		  pta_flag_sse3, pta_flag_ssse3,
		  pta_flag_sse4a, pta_flag_abm,
		  pta_flag_cx16, pta_flag_sse4_1,
		  pta_flag_sse4_2, pta_flag_aes,
		  pta_flag_pclmul, pta_flag_avx,
		  pta_flag_bmi, pta_flag_f16c,
		  pta_flag_movbe, pta_flag_prfchw,
		  pta_flag_fxsr, pta_flag_xsave,
		  pta_flag_xsaveopt>::apply
    },
    {
      "generic", processor_generic,
      __flags_set<pta_flag_64bit, pta_flag_hle>::apply,
      .min_gcc_version = {4, 9, 0},
    },
    {
      "generic32", processor_generic32,
      __flags_set<pta_flag_hle>::apply,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 5},
    },
    {
      "generic64", processor_generic64,
      __flags_set<pta_flag_64bit, pta_flag_hle>::apply,
      .min_gcc_version = {0, 0, 0},
      .max_gcc_version = {4, 8, 5},
    },
    { nullptr },
  };

  for (const _pta *r = processor_alias_table; r->name; ++r) {
    if (!std::strcmp(r->name, name) &&
	r->min_gcc_version <= ver && ver <= r->max_gcc_version) {
      return r;
    }
  }

  return nullptr;
}

void target_x86_64_gcc::opts_x86::
handle_opt(const gcc_cmdline_parser::option * const o,
	   const char *val, const bool negative,
	   const bool generated)
{
  using gcc_version = gcc_cmdline_parser::gcc_version;

  assert(o);

  switch (o->code) {
  case opt_code_i386_unused:
    break;

  case opt_code_i386_m16:
    if (!generated)
      __set_isa_flag<isa_flag_code16>(_isa_flags_set);
    __set_isa_flag<isa_flag_code16>(_isa_flags);
    break;

  case opt_code_i386_m32:
    if (!generated)
      __set_isa_flag<isa_flag_64bit>(_isa_flags_set);
    __unset_isa_flag<isa_flag_64bit>(_isa_flags, false);
    break;

  case opt_code_i386_m64:
    if (!generated)
      __set_isa_flag<isa_flag_abi_64>(_isa_flags_set);
    __set_isa_flag<isa_flag_abi_64>(_isa_flags);
    break;

  case opt_code_i386_mx32:
    if (!generated)
      __set_isa_flag<isa_flag_abi_x32>(_isa_flags_set);
    __set_isa_flag<isa_flag_abi_x32>(_isa_flags);
    break;

  case opt_code_i386_m3dnow:
    _set_isa_flag_user<isa_flag_3dnow>(!negative, generated);
    break;

  case opt_code_i386_m3dnowa:
    if (_t.get_gcc_version() < gcc_version{7, 1, 0}) {
      // Before GCC 7.1.0, ix86_handle_option() used to return false
      // for this one, resulting in an "unrecognized option" error
      // reported from GCC's from read_cmdline_option().
      throw cmdline_except{"unrecognized gcc option '-m3dnowa'"};
    }
    _set_isa_flag_user<isa_flag_3dnow_a>(!negative, generated);
    break;

  case opt_code_i386_mabm:
    _set_isa_flag_user<isa_flag_abm>(!negative, generated);
    break;

  case opt_code_i386_madx:
    _set_isa_flag_user<isa_flag_adx>(!negative, generated);
    break;

  case opt_code_i386_maes:
    _set_isa_flag_user<isa_flag_aes>(!negative, generated);
    break;

  case opt_code_i386_march:
    assert(val);
    assert(!generated);
    _arch_string = val;
    break;

  case opt_code_i386_mavx:
    _set_isa_flag_user<isa_flag_avx>(!negative, generated);
    break;

  case opt_code_i386_mavx2:
    _set_isa_flag_user<isa_flag_avx2>(!negative, generated);
    break;

  case opt_code_i386_mavx5124fmaps:
    _set_isa_flag_user<isa_flag_avx5124fmaps>(!negative, generated);
    if (!negative)
      _set_isa_flag_explicit<isa_flag_avx512f>();
    break;

  case opt_code_i386_mavx5124vnniw:
    _set_isa_flag_user<isa_flag_avx5124vnniw>(!negative, generated);
    if (!negative)
      _set_isa_flag_explicit<isa_flag_avx512f>();
    break;

  case opt_code_i386_mavx512bitalg:
    _set_isa_flag_user<isa_flag_avx512bitalg>(!negative, generated);
    break;

  case opt_code_i386_mavx512bw:
    _set_isa_flag_user<isa_flag_avx512bw>(!negative, generated);
    break;

  case opt_code_i386_mavx512cd:
    _set_isa_flag_user<isa_flag_avx512cd>(!negative, generated);
    break;

  case opt_code_i386_mavx512dq:
    _set_isa_flag_user<isa_flag_avx512dq>(!negative, generated);
    break;

  case opt_code_i386_mavx512er:
    _set_isa_flag_user<isa_flag_avx512er>(!negative, generated);
    break;

  case opt_code_i386_mavx512f:
    _set_isa_flag_user<isa_flag_avx512f>(!negative, generated);

    if (negative && _t.get_gcc_version() < gcc_version{8, 1, 0}) {
      // C.f. the comment in the
      // __unset_isa_flag<isa_flag_avx512f>() specialization.
      _unset_isa_flag_explicit<isa_flag_avx5124fmaps>();
      _unset_isa_flag_explicit<isa_flag_avx5124vnniw>();
      _unset_isa_flag_explicit<isa_flag_avx512vpopcntdq>();
    }
    break;

  case opt_code_i386_mavx512ifma:
    _set_isa_flag_user<isa_flag_avx512ifma>(!negative, generated);
    break;

  case opt_code_i386_mavx512pf:
    _set_isa_flag_user<isa_flag_avx512pf>(!negative, generated);
    break;

  case opt_code_i386_mavx512vbmi:
    _set_isa_flag_user<isa_flag_avx512vbmi>(!negative, generated);
    break;

  case opt_code_i386_mavx512vbmi2:
    _set_isa_flag_user<isa_flag_avx512vbmi2>(!negative, generated);
    break;

  case opt_code_i386_mavx512vl:
    _set_isa_flag_user<isa_flag_avx512vl>(!negative, generated);
    break;

  case opt_code_i386_mavx512vnni:
    _set_isa_flag_user<isa_flag_avx512vnni>(!negative, generated);
    break;

  case opt_code_i386_mavx512vpopcntdq:
    _set_isa_flag_user<isa_flag_avx512vpopcntdq>(!negative, generated);
    break;

  case opt_code_i386_mbmi:
    _set_isa_flag_user<isa_flag_bmi>(!negative, generated);
    break;

  case opt_code_i386_mbmi2:
    _set_isa_flag_user<isa_flag_bmi2>(!negative, generated);
    break;

  case opt_code_i386_mcldemote:
    _set_isa_flag_user<isa_flag_cldemote>(!negative, generated);
    break;

  case opt_code_i386_mclflushopt:
    _set_isa_flag_user<isa_flag_clflushopt>(!negative, generated);
    break;

  case opt_code_i386_mclwb:
    _set_isa_flag_user<isa_flag_clwb>(!negative, generated);
    break;

  case opt_code_i386_mclzero:
    _set_isa_flag_user<isa_flag_clzero>(!negative, generated);
    break;

  case opt_code_i386_mcrc32:
    _set_isa_flag_user<isa_flag_crc32>(!negative, generated);
    break;

  case opt_code_i386_mcx16:
    _set_isa_flag_user<isa_flag_cx16>(!negative, generated);
    break;

  case opt_code_i386_mf16c:
    _set_isa_flag_user<isa_flag_f16c>(!negative, generated);
    break;

  case opt_code_i386_mfma:
    _set_isa_flag_user<isa_flag_fma>(!negative, generated);
    break;

  case opt_code_i386_mfma4:
    _set_isa_flag_user<isa_flag_fma4>(!negative, generated);
    break;

  case opt_code_i386_mfsgsbase:
    _set_isa_flag_user<isa_flag_fsgsbase>(!negative, generated);
    break;

  case opt_code_i386_mfxsr:
    _set_isa_flag_user<isa_flag_fxsr>(!negative, generated);
    break;

  case opt_code_i386_mgeneral_regs_only:
    assert(!negative);
    _unset_isa_flag_explicit<isa_flag_mmx>();
    _unset_isa_flag_explicit<isa_flag_sse>();
    // MPX support had been available only between GCC versions >=
    // 5.1.0 and < 9.1.0. _valid_isa_flags masking will handle it.
    _unset_isa_flag_explicit<isa_flag_mpx>();
    break;

  case opt_code_i386_mgfni:
    _set_isa_flag_user<isa_flag_gfni>(!negative, generated);
    break;

  case opt_code_i386_mhle:
    // GCC's ix86_handle_option() never handled -mhle explictly. As a
    // result, _isa_flags_explicit had never been set for this option.
    if (_valid_isa_flags.test(isa_flag_hle)) {
      if (!generated)
	_isa_flags_set.set(isa_flag_hle, true);
      _isa_flags.set(isa_flag_hle, !negative);
    }
    break;

  case opt_code_i386_mlwp:
    _set_isa_flag_user<isa_flag_lwp>(!negative, generated);
    break;

  case opt_code_i386_mlzcnt:
    // Before GCC 4.9.0, -mlzcnt had not been handled explictly in
    // ix86_handle_option(). As a consequence, _isa_flags_explicit had
    // not been set.
    if (gcc_version{4, 9, 0} <= _t.get_gcc_version()) {
      _set_isa_flag_user<isa_flag_lzcnt>(!negative, generated);
    } else {
      // Only set _isa_flags and _isa_flags_set.
      assert(_valid_isa_flags.test(isa_flag_lzcnt));
      if (!generated)
	_isa_flags_set.set(isa_flag_lzcnt, true);
      _isa_flags.set(isa_flag_lzcnt, !negative);
    }
    break;

  case opt_code_i386_mmmx:
    _set_isa_flag_user<isa_flag_mmx>(!negative, generated);
    break;

  case opt_code_i386_mmovbe:
    _set_isa_flag_user<isa_flag_movbe>(!negative, generated);
    break;

  case opt_code_i386_mmovdir64b:
    _set_isa_flag_user<isa_flag_movdir64b>(!negative, generated);
    break;

  case opt_code_i386_mmovdiri:
    _set_isa_flag_user<isa_flag_movdiri>(!negative, generated);
    break;

  case opt_code_i386_mmpx:
    // GCC's ix86_handle_option() never handled -mmpx explictly. As a
    // result, _isa_flags_explicit had never been set for this option
    // (which doesn't matter anyway, because PTA_MPX had never been
    //  set for any processor and -mmpx has been deprecated with GCC 9.1.0).
    if (_valid_isa_flags.test(isa_flag_mpx)) {
      if (!generated)
	_isa_flags_set.set(isa_flag_mpx, true);
      _isa_flags.set(isa_flag_mpx, !negative);
    }
    break;

  case opt_code_i386_mmwaitx:
    _set_isa_flag_user<isa_flag_mwaitx>(!negative, generated);
    break;

  case opt_code_i386_mno_sse4:
    assert(!negative);
    _set_isa_flag_user<isa_flag_sse4_1>(false, generated);
    break;

  case opt_code_i386_mpclmul:
    _set_isa_flag_user<isa_flag_pclmul>(!negative, generated);
    break;

  case opt_code_i386_mpku:
    _set_isa_flag_user<isa_flag_pku>(!negative, generated);
    break;

  case opt_code_i386_mpcommit:
    _set_isa_flag_user<isa_flag_pcommit>(!negative, generated);
    break;

  case opt_code_i386_mpconfig:
    _set_isa_flag_user<isa_flag_pconfig>(!negative, generated);
    break;

  case opt_code_i386_mpopcnt:
    _set_isa_flag_user<isa_flag_popcnt>(!negative, generated);
    break;

  case opt_code_i386_mprefetchwt1:
    _set_isa_flag_user<isa_flag_prefetchwt1>(!negative, generated);
    break;

  case opt_code_i386_mprfchw:
    _set_isa_flag_user<isa_flag_prfchw>(!negative, generated);
    break;

  case opt_code_i386_mptwrite:
    _set_isa_flag_user<isa_flag_ptwrite>(!negative, generated);
    break;

  case opt_code_i386_mrdpid:
    _set_isa_flag_user<isa_flag_rdpid>(!negative, generated);
    break;

  case opt_code_i386_mrdrnd:
    _set_isa_flag_user<isa_flag_rdrnd>(!negative, generated);
    break;

  case opt_code_i386_mrdseed:
    _set_isa_flag_user<isa_flag_rdseed>(!negative, generated);
    break;

  case opt_code_i386_mrtm:
    _set_isa_flag_user<isa_flag_rtm>(!negative, generated);
    break;

  case opt_code_i386_msgx:
    _set_isa_flag_user<isa_flag_sgx>(!negative, generated);
    break;

  case opt_code_i386_msha:
    _set_isa_flag_user<isa_flag_sha>(!negative, generated);
    break;

  case opt_code_i386_msahf:
    _set_isa_flag_user<isa_flag_sahf>(!negative, generated);
    break;

  case opt_code_i386_mshstk:
    _set_isa_flag_user<isa_flag_shstk>(!negative, generated);
    break;

  case opt_code_i386_msse:
    _set_isa_flag_user<isa_flag_sse>(!negative, generated);
    break;

  case opt_code_i386_msse2:
    _set_isa_flag_user<isa_flag_sse2>(!negative, generated);
    break;

  case opt_code_i386_msse3:
    _set_isa_flag_user<isa_flag_sse3>(!negative, generated);
    break;

  case opt_code_i386_msse4:
    assert(!negative);
    _set_isa_flag_user<isa_flag_sse4_2>(true, generated);
    break;

  case opt_code_i386_msse4_1:
    _set_isa_flag_user<isa_flag_sse4_1>(!negative, generated);
    break;

  case opt_code_i386_msse4_2:
    _set_isa_flag_user<isa_flag_sse4_2>(!negative, generated);
    break;

  case opt_code_i386_msse4a:
    _set_isa_flag_user<isa_flag_sse4a>(!negative, generated);
    break;

  case opt_code_i386_mssse3:
    _set_isa_flag_user<isa_flag_ssse3>(!negative, generated);
    break;

  case opt_code_i386_mtbm:
    _set_isa_flag_user<isa_flag_tbm>(!negative, generated);
    break;

  case opt_code_i386_mtune:
    assert(val);
    assert(!generated);
    _tune_string = val;
    break;

  case opt_code_i386_mvaes:
    _set_isa_flag_user<isa_flag_vaes>(!negative, generated);
    break;

  case opt_code_i386_mvpclmulqdq:
    _set_isa_flag_user<isa_flag_vpclmulqdq>(!negative, generated);
    break;

  case opt_code_i386_mwaitpkg:
    _set_isa_flag_user<isa_flag_waitpkg>(!negative, generated);
    break;

  case opt_code_i386_mwbnoinvd:
    _set_isa_flag_user<isa_flag_wbnoinvd>(!negative, generated);
    break;

  case opt_code_i386_mxop:
    _set_isa_flag_user<isa_flag_xop>(!negative, generated);
    break;

  case opt_code_i386_mxsave:
    _set_isa_flag_user<isa_flag_xsave>(!negative, generated);
    break;

  case opt_code_i386_mxsavec:
    _set_isa_flag_user<isa_flag_xsavec>(!negative, generated);
    break;

  case opt_code_i386_mxsaveopt:
    _set_isa_flag_user<isa_flag_xsaveopt>(!negative, generated);
    break;

  case opt_code_i386_mxsaves:
    _set_isa_flag_user<isa_flag_xsaves>(!negative, generated);
    break;
  }
}

void target_x86_64_gcc::opts_x86::option_override()
{
  using gcc_version = gcc_cmdline_parser::gcc_version;

  // This mimics GCC's ix86_option_override_internal for a biarch
  // compiler defaulting to 64bit ABI.
  if (!_isa_flags.test(isa_flag_64bit)) {
    _isa_flags.reset(isa_flag_abi_64);
    _isa_flags.reset(isa_flag_abi_x32);

  } else {
    if (_isa_flags.test(isa_flag_abi_x32))
      _isa_flags.reset(isa_flag_abi_64);
    }
  }

  if (_isa_flags.test(isa_flag_abi_x32)) {
    _isa_flags.set(isa_flag_64bit);
    _isa_flags.reset(isa_flag_abi_64);
  } else if (_isa_flags.test(isa_flag_code16)) {
    _isa_flags.reset(isa_flag_64bit);
    _isa_flags.reset(isa_flag_abi_x32);
    _isa_flags.reset(isa_flag_abi_64);
  } else if (_isa_flags.test(isa_flag_abi_64)) {
    _isa_flags.set(isa_flag_64bit);
    _isa_flags.reset(isa_flag_abi_x32);
  }

  const gcc_version &ver = _t.get_gcc_version();
  bool tune_defaulted = false;
  bool tune_specified = false;
  if (_tune_string.empty()) {
    if (!_arch_string.empty()) {
      _tune_string = _arch_string;
    } else {
      _tune_string = "generic";
      tune_defaulted = true;
    }

    // This changed after GCC 4.8.5
    if (gcc_version{4, 9, 0} <= ver) {
      if (_tune_string == "x86-64")
	_tune_string = "generic";
    } else {
      if (_tune_string == "generic" ||
	  _tune_string == "x86-64" ||
	  _tune_string == "i686") {
	if (_isa_flags.test(isa_flag_64bit))
	  _tune_string = "generic64";
	else
	  _tune_string = "generic32";
      }
    }

  } else {
    tune_specified = true;

    // This has changed after GCC 4.8.5
    if (ver < gcc_version{4, 9, 0}) {
      if (_tune_string == "generic" ||
	  _tune_string == "i686") {
	if (_isa_flags.test(isa_flag_64bit))
	  _tune_string = "generic64";
	else
	  _tune_string = "generic32";

      } else if (_tune_string == "generic64" ||
		 _tune_string == "generic32") {
	throw cmdline_except{
	  "bad value (" + _tune_string + ") + for -mtune switch"
	};
      }
    }
  }

  bool arch_specified = false;
  if (_arch_string.empty()) {
    _arch_string = _isa_flags.test(isa_flag_64bit) ? "x86-64" : "i386";
  } else {
    arch_specified = true;
  }

  _arch = _pta::lookup(_arch_string.c_str(), ver);
  if (!_arch) {
    throw cmdline_except{"unrecognized value for -march"};
  }

  _pta::pta_flags_type pta_flags;
  _arch->apply_flags(pta_flags, ver);
  if (!std::strcmp(_arch->name, "generic")) {
    throw cmdline_except{"\"generic\" CPU can be used only for -mtune switch"};
  } else if (!std::strcmp(_arch->name, "generic32") ||
	     !std::strcmp(_arch->name, "generic64")) {
    // Note that this can happen only with GCC <= 4.8.5
    throw cmdline_except{
      "bad value (" + _arch_string + ") + for -march switch"
    };
  } else if (!std::strcmp(_arch->name, "intel")) {
    throw cmdline_except{"\"intel\" CPU can be used only for -mtune switch"};
  } else if (_isa_flags.test(isa_flag_64bit) &&
	     !(pta_flags.test(_pta::pta_flag_64bit))) {
    throw cmdline_except{"selected CPU doesn't support x86-64 instruction set"};
  }

  auto &&set_isa_flag_from_pta =
    [&](const target_x86_64_gcc::opts_x86::isa_flag b) {
      if (_valid_isa_flags.test(b) && !_isa_flags_explicit.test(b))
	_isa_flags.set(b);
    };
  if (pta_flags.test(_pta::pta_flag_mmx))
    set_isa_flag_from_pta(isa_flag_mmx);
  if (pta_flags.test(_pta::pta_flag_3dnow))
    set_isa_flag_from_pta(isa_flag_3dnow);
  if (pta_flags.test(_pta::pta_flag_3dnow_a))
    set_isa_flag_from_pta(isa_flag_3dnow_a);
  if (pta_flags.test(_pta::pta_flag_sse))
    set_isa_flag_from_pta(isa_flag_sse);
  if (pta_flags.test(_pta::pta_flag_sse2))
    set_isa_flag_from_pta(isa_flag_sse2);
  if (pta_flags.test(_pta::pta_flag_sse3))
    set_isa_flag_from_pta(isa_flag_sse3);
  if (pta_flags.test(_pta::pta_flag_ssse3))
    set_isa_flag_from_pta(isa_flag_ssse3);
  if (pta_flags.test(_pta::pta_flag_sse4_1))
    set_isa_flag_from_pta(isa_flag_sse4_1);
  if (pta_flags.test(_pta::pta_flag_sse4_2))
    set_isa_flag_from_pta(isa_flag_sse4_2);
  if (pta_flags.test(_pta::pta_flag_avx))
    set_isa_flag_from_pta(isa_flag_avx);
  if (pta_flags.test(_pta::pta_flag_avx2))
    set_isa_flag_from_pta(isa_flag_avx2);
  if (pta_flags.test(_pta::pta_flag_fma))
    set_isa_flag_from_pta(isa_flag_fma);
  if (pta_flags.test(_pta::pta_flag_sse4a))
    set_isa_flag_from_pta(isa_flag_sse4a);
  if (pta_flags.test(_pta::pta_flag_fma4))
    set_isa_flag_from_pta(isa_flag_fma4);
  if (pta_flags.test(_pta::pta_flag_xop))
    set_isa_flag_from_pta(isa_flag_xop);
  if (pta_flags.test(_pta::pta_flag_lwp))
    set_isa_flag_from_pta(isa_flag_lwp);
  if (pta_flags.test(_pta::pta_flag_abm))
    set_isa_flag_from_pta(isa_flag_abm);
  if (pta_flags.test(_pta::pta_flag_bmi))
    set_isa_flag_from_pta(isa_flag_bmi);
  if (pta_flags.test(_pta::pta_flag_lzcnt) ||
      pta_flags.test(_pta::pta_flag_abm)) {
    set_isa_flag_from_pta(isa_flag_lzcnt);
  }
  if (pta_flags.test(_pta::pta_flag_tbm))
    set_isa_flag_from_pta(isa_flag_tbm);
  if (pta_flags.test(_pta::pta_flag_bmi2))
    set_isa_flag_from_pta(isa_flag_bmi2);
  if (pta_flags.test(_pta::pta_flag_cx16))
    set_isa_flag_from_pta(isa_flag_cx16);
  if (pta_flags.test(_pta::pta_flag_popcnt) ||
      pta_flags.test(_pta::pta_flag_abm)) {
    set_isa_flag_from_pta(isa_flag_popcnt);
  }
  if (_isa_flags.test(isa_flag_64bit) ||
      pta_flags.test(_pta::pta_flag_no_sahf)) {
    set_isa_flag_from_pta(isa_flag_sahf);
  }
  if (pta_flags.test(_pta::pta_flag_movbe))
    set_isa_flag_from_pta(isa_flag_movbe);
  if (pta_flags.test(_pta::pta_flag_aes))
    set_isa_flag_from_pta(isa_flag_aes);
  if (pta_flags.test(_pta::pta_flag_sha))
    set_isa_flag_from_pta(isa_flag_sha);
  if (pta_flags.test(_pta::pta_flag_pclmul))
    set_isa_flag_from_pta(isa_flag_pclmul);
  if (pta_flags.test(_pta::pta_flag_fsgsbase))
    set_isa_flag_from_pta(isa_flag_fsgsbase);
  if (pta_flags.test(_pta::pta_flag_rdrnd))
    set_isa_flag_from_pta(isa_flag_rdrnd);
  if (pta_flags.test(_pta::pta_flag_f16c))
    set_isa_flag_from_pta(isa_flag_f16c);
  if (pta_flags.test(_pta::pta_flag_rtm))
    set_isa_flag_from_pta(isa_flag_rtm);
  if (pta_flags.test(_pta::pta_flag_hle))
    set_isa_flag_from_pta(isa_flag_hle);
  if (pta_flags.test(_pta::pta_flag_prfchw))
    set_isa_flag_from_pta(isa_flag_prfchw);
  if (pta_flags.test(_pta::pta_flag_rdseed))
    set_isa_flag_from_pta(isa_flag_rdseed);
  if (pta_flags.test(_pta::pta_flag_adx))
    set_isa_flag_from_pta(isa_flag_adx);
  if (pta_flags.test(_pta::pta_flag_fxsr))
    set_isa_flag_from_pta(isa_flag_fxsr);
  if (pta_flags.test(_pta::pta_flag_xsave))
    set_isa_flag_from_pta(isa_flag_xsave);
  if (pta_flags.test(_pta::pta_flag_xsaveopt))
    set_isa_flag_from_pta(isa_flag_xsaveopt);
  if (pta_flags.test(_pta::pta_flag_avx512f))
    set_isa_flag_from_pta(isa_flag_avx512f);
  if (pta_flags.test(_pta::pta_flag_avx512er))
    set_isa_flag_from_pta(isa_flag_avx512er);
  if (pta_flags.test(_pta::pta_flag_avx512pf))
    set_isa_flag_from_pta(isa_flag_avx512pf);
  if (pta_flags.test(_pta::pta_flag_avx512cd))
    set_isa_flag_from_pta(isa_flag_avx512cd);
  if (pta_flags.test(_pta::pta_flag_prefetchwt1))
    set_isa_flag_from_pta(isa_flag_prefetchwt1);
  if (pta_flags.test(_pta::pta_flag_clwb))
    set_isa_flag_from_pta(isa_flag_clwb);
  if (pta_flags.test(_pta::pta_flag_clflushopt))
    set_isa_flag_from_pta(isa_flag_clflushopt);
  if (pta_flags.test(_pta::pta_flag_clzero))
    set_isa_flag_from_pta(isa_flag_clzero);
  if (pta_flags.test(_pta::pta_flag_xsavec))
    set_isa_flag_from_pta(isa_flag_xsavec);
  if (pta_flags.test(_pta::pta_flag_xsaves))
    set_isa_flag_from_pta(isa_flag_xsaves);
  if (pta_flags.test(_pta::pta_flag_avx512dq))
    set_isa_flag_from_pta(isa_flag_avx512dq);
  if (pta_flags.test(_pta::pta_flag_avx512bw))
    set_isa_flag_from_pta(isa_flag_avx512bw);
  if (pta_flags.test(_pta::pta_flag_avx512vl))
    set_isa_flag_from_pta(isa_flag_avx512vl);
  if (pta_flags.test(_pta::pta_flag_avx512vbmi))
    set_isa_flag_from_pta(isa_flag_avx512vbmi);
  if (pta_flags.test(_pta::pta_flag_avx512ifma))
    set_isa_flag_from_pta(isa_flag_avx512ifma);
  if (pta_flags.test(_pta::pta_flag_avx512vnni))
    set_isa_flag_from_pta(isa_flag_avx512vnni);
  if (pta_flags.test(_pta::pta_flag_gfni))
    set_isa_flag_from_pta(isa_flag_gfni);
  if (pta_flags.test(_pta::pta_flag_avx512vbmi2))
    set_isa_flag_from_pta(isa_flag_avx512vbmi2);
  if (pta_flags.test(_pta::pta_flag_vpclmulqdq))
    set_isa_flag_from_pta(isa_flag_vpclmulqdq);
  if (pta_flags.test(_pta::pta_flag_avx512bitalg))
    set_isa_flag_from_pta(isa_flag_avx512bitalg);
  if (pta_flags.test(_pta::pta_flag_avx5124vnniw))
    set_isa_flag_from_pta(isa_flag_avx5124vnniw);
  if (pta_flags.test(_pta::pta_flag_avx5124fmaps))
    set_isa_flag_from_pta(isa_flag_avx5124fmaps);
  if (pta_flags.test(_pta::pta_flag_avx512vpopcntdq))
    set_isa_flag_from_pta(isa_flag_avx512vpopcntdq);
  if (pta_flags.test(_pta::pta_flag_sgx))
    set_isa_flag_from_pta(isa_flag_sgx);
  if (pta_flags.test(_pta::pta_flag_vaes))
    set_isa_flag_from_pta(isa_flag_vaes);
  if (pta_flags.test(_pta::pta_flag_rdpid))
    set_isa_flag_from_pta(isa_flag_rdpid);
  if (pta_flags.test(_pta::pta_flag_pconfig))
    set_isa_flag_from_pta(isa_flag_pconfig);
  if (pta_flags.test(_pta::pta_flag_wbnoinvd))
    set_isa_flag_from_pta(isa_flag_wbnoinvd);
  if (pta_flags.test(_pta::pta_flag_ptwrite))
    set_isa_flag_from_pta(isa_flag_ptwrite);
  if (pta_flags.test(_pta::pta_flag_mwaitx))
    set_isa_flag_from_pta(isa_flag_mwaitx);
  if (pta_flags.test(_pta::pta_flag_pku))
    set_isa_flag_from_pta(isa_flag_pku);
  }

  _tune = _pta::lookup(_tune_string.c_str(), ver);
  if (!_tune) {
    throw cmdline_except{"unrecognized value for -mtune"};
  }

  _pta::pta_flags_type tune_pta_flags;
  _tune->apply_flags(tune_pta_flags, ver);
  if (_isa_flags.test(isa_flag_64bit) &&
      !(tune_pta_flags.test(_pta::pta_flag_64bit))) {
    throw cmdline_except{"selected CPU doesn't support x86-64 instruction set"};
  }

  if (_isa_flags.test(isa_flag_64bit)) {
    if (_t.get_opts_common().optimize >= 1 &&
	!_t.get_opts_common().flag_omit_frame_pointer_set) {
      // GCC's USE_X86_64_FRAME_POINTER is zero.
      _t.get_opts_common().flag_omit_frame_pointer = true;
    }

    if (_t.get_opts_common().flag_asynchronous_unwind_tables == 2) {
      _t.get_opts_common().flag_unwind_tables = true;
      _t.get_opts_common().flag_asynchronous_unwind_tables = 1;
    }
  } else {
    if (_t.get_opts_common().optimize >= 1 &&
	!_t.get_opts_common().flag_omit_frame_pointer_set) {
      // GCC's USE_IX86_FRAME_POINTER is zero.
      _t.get_opts_common().flag_omit_frame_pointer =
	!_t.get_opts_common().optimize_size;
    }

    if (_t.get_opts_common().flag_asynchronous_unwind_tables == 2) {
      // GCC's USE_IX86_FRAME_POINTER is zero.
      _t.get_opts_common().flag_asynchronous_unwind_tables = 1;
    }
  }

  if (_isa_flags.test(isa_flag_64bit)) {
    if (!arch_specified) {
      // Set GCC's TARGET_SUBTARGET64_ISA_DEFAULT flags if not explicitly
      // disabled.
      if (!_isa_flags_explicit.test(isa_flag_mmx))
	__set_isa_flag<isa_flag_mmx>(_isa_flags);
      if (!_isa_flags_explicit.test(isa_flag_sse))
	__set_isa_flag<isa_flag_sse>(_isa_flags);
      if (!_isa_flags_explicit.test(isa_flag_sse2))
	__set_isa_flag<isa_flag_sse2>(_isa_flags);
    }

  } else {
    // GCC's default TARGET_SUBTARGET32_ISA_DEFAULT flags are empty.
  }

  if (_isa_flags.test(isa_flag_sse)) {
    if (!_isa_flags_explicit.test(isa_flag_mmx))
      _isa_flags.set(isa_flag_mmx);
  }

  if (gcc_version{4, 8, 0} < ver && ver <= gcc_version{4, 8, 5}) {
    if (_isa_flags.test(isa_flag_3dnow)) {
      if (!_isa_flags_explicit.test(isa_flag_prfchw))
	_isa_flags.set(isa_flag_prfchw);
    }
  } else if (ver <=
	     gcc_version{6, 2, std::numeric_limits<unsigned int>::max()}) {
    if (_isa_flags.test(isa_flag_3dnow) ||
	_isa_flags.test(isa_flag_prefetchwt1)) {
      if (!_isa_flags_explicit.test(isa_flag_prfchw))
	_isa_flags.set(isa_flag_prfchw);
    }
  }

  if (_isa_flags.test(isa_flag_sse4_2) || _isa_flags.test(isa_flag_abm)) {
    if (!_isa_flags_explicit.test(isa_flag_popcnt))
      _isa_flags.set(isa_flag_popcnt);
  }

  if (_isa_flags.test(isa_flag_abm)) {
    if (!_isa_flags_explicit.test(isa_flag_lzcnt))
      _isa_flags.set(isa_flag_lzcnt);
  }

  if ((gcc_version{5, 5, 0} <= ver &&
       ver <= gcc_version{5, 5, std::numeric_limits<unsigned int>::max()}) ||
      (gcc_version{6, 5, 0} <= ver &&
       ver <= gcc_version{6, 5, std::numeric_limits<unsigned int>::max()}) ||
      gcc_version{7, 2, 0} <= ver) {
    if (_isa_flags.test(isa_flag_code16)) {
      if (!_isa_flags_explicit.test(isa_flag_bmi))
	_isa_flags.reset(isa_flag_bmi);
      if (!_isa_flags_explicit.test(isa_flag_bmi2))
	_isa_flags.reset(isa_flag_bmi2);
      if (!_isa_flags_explicit.test(isa_flag_tbm))
	_isa_flags.reset(isa_flag_tbm);
    }
  }
}
