#include "execution_charset_encoder_iconv.hh"
#include "types_impl.hh"
#include "ast.hh"
#include "semantic_except.hh"
#include "arch_gcc48_x86_64.hh"
#include "pp_token.hh"

using namespace klp::ccp;
using namespace klp::ccp::types;

bool arch_gcc48_x86_64::is_char_signed() const noexcept
{
  return true;
}

bool arch_gcc48_x86_64::is_wchar_signed() const noexcept
{
  return true;
}

bool arch_gcc48_x86_64::is_bitfield_default_signed() const noexcept
{
  return true;
}

mpa::limbs::size_type
arch_gcc48_x86_64::get_std_int_width(const types::std_int_type::kind k)
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
arch_gcc48_x86_64::int_mode_to_std_int_kind(const int_mode_kind m)
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


mpa::limbs::size_type arch_gcc48_x86_64::
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

mpa::limbs::size_type arch_gcc48_x86_64::
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

types::float_type::kind  arch_gcc48_x86_64::
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

int_mode_kind arch_gcc48_x86_64::get_pointer_mode() const noexcept
{
  return int_mode_kind::imk_DI;
}

int_mode_kind arch_gcc48_x86_64::get_word_mode() const noexcept
{
  return int_mode_kind::imk_DI;
}

mpa::limbs::size_type arch_gcc48_x86_64::get_biggest_alignment_log2()
  const noexcept
{
  return 4;
}

std::unique_ptr<execution_charset_encoder> arch_gcc48_x86_64::
get_execution_charset_encoder(const execution_charset_encoding e) const
{
  types::std_int_type::kind target_char_kind;
  const char *target_code;

  switch (e) {
  case architecture::execution_charset_encoding::ecse_char:
    target_code = "UTF-8";
    target_char_kind = types::std_int_type::kind::k_char;
    break;

  case architecture::execution_charset_encoding::ecse_wchar:
    target_code = "UTF-32LE";
    target_char_kind = types::std_int_type::kind::k_int;
    break;

  case architecture::execution_charset_encoding::ecse_char8:
    target_code = "UTF-8";
    target_char_kind = types::std_int_type::kind::k_char;
    break;

  case architecture::execution_charset_encoding::ecse_char16:
    target_code = "UTF-16LE";
    target_char_kind = types::std_int_type::kind::k_short;
    break;

  case architecture::execution_charset_encoding::ecse_char32:
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

void arch_gcc48_x86_64::evaluate_enum_type(ast::ast &a, types::enum_content &ec,
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

mpa::limbs arch_gcc48_x86_64::
get_std_int_size(const types::std_int_type::kind k) const
{
  return mpa::limbs::from_size_type(get_std_int_width(k) / 8);
}

mpa::limbs::size_type arch_gcc48_x86_64::
get_std_int_alignment(const types::std_int_type::kind k) const
{
  return get_std_int_size(k).ffs() - 1;
}

mpa::limbs arch_gcc48_x86_64::
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

mpa::limbs::size_type arch_gcc48_x86_64::
get_float_alignment(const types::float_type::kind k, const bool) const
{
  return get_float_size(k, false).ffs() - 1;
}

mpa::limbs arch_gcc48_x86_64::get_pointer_size() const
{
  return mpa::limbs::from_size_type(8);
}

mpa::limbs::size_type arch_gcc48_x86_64::get_pointer_alignment() const
{
  return 3;
}

mpa::limbs arch_gcc48_x86_64::get_va_list_size() const
{
  return mpa::limbs::from_size_type(24);
}

mpa::limbs::size_type arch_gcc48_x86_64::get_va_list_alignment() const
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
    record_layout_info(const arch_gcc48_x86_64 &arch,
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

    const arch_gcc48_x86_64 &_arch;
    mpa::limbs _offset;
    mpa::limbs _bitpos;
    mpa::limbs::size_type _record_align_ffs;
    bool _is_size_constant;
  };
}

record_layout_info::
record_layout_info(const arch_gcc48_x86_64 &arch,
		   const mpa::limbs::size_type user_align_ffs)
  : _arch(arch), _offset(0), _bitpos(),
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
  return 3 + o_t.get_effective_alignment(_arch) + 1;
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

  if (!bf_t.get_width(_arch)) {
    // From gcc: A zero-length bit-field affects the alignment of the
    // next field.  In essence such bit-fields are not influenced by
    // any packing due to #pragma pack or attribute packed.
    const std::shared_ptr<const returnable_int_type>& base_type = bf_t.get_base_type();
    const mpa::limbs::size_type base_type_align_ffs =
      3 + base_type->get_effective_alignment(_arch) + 1;
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
      3 + base_type->get_effective_alignment(_arch) + 1;

    if (bf_t.get_width(_arch) && bf_t.is_packed())
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
	     bf_t.get_width(_arch) &&
	     bf_t.get_width(_arch) <= std::numeric_limits<uhwi>::max() &&
	     _is_size_constant && _offset.fits_into_type<uhwi>() &&
	     (bf_t.get_base_type()->get_size(_arch).lshift(3)
	      .fits_into_type<uhwi>())) {
	   const std::shared_ptr<const returnable_int_type>& base_type =
	     bf_t.get_base_type();
	   const mpa::limbs::size_type type_align_ffs =
	     3 + base_type->get_effective_alignment(_arch) + 1;
	   const hwi field_size =
	     crop_like_gcc<hwi>(static_cast<uhwi>(bf_t.get_width(_arch)));
	   const hwi offset = crop_like_gcc<hwi>(_offset.to_type<uhwi>());
	   const hwi bit_offset = crop_like_gcc<hwi>(_bitpos.to_type<uhwi>());
	   const uhwi type_size =
	     base_type->get_size(_arch).lshift(3).to_type<uhwi>();

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
	   _bitpos = _bitpos + o_t.get_size(_arch).lshift(3);
	   _normalize();
	 }
       },
       [&](const bitfield_type &bf_t) {
	 _bitpos = _bitpos + mpa::limbs::from_size_type(bf_t.get_width(_arch));
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
	   return (mpa::limbs::from_size_type(bf_t.get_width(_arch))
		   .align(3).rshift(3, false));
	 },
	 [&](const object_type &o_t) {
	   if (o_t.is_size_constant()) {
	     return o_t.get_size(_arch);
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

  sc.set_alignment(_record_align_ffs - 1);
  if (_is_size_constant) {
    sc.set_size(type_size.rshift(3, false));
    sc.set_size_constant(true);
  } else {
    sc.set_size_constant(false);
  }
}

void arch_gcc48_x86_64::
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

void arch_gcc48_x86_64::
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



types::std_int_type::kind
arch_gcc48_x86_64::_width_to_int_kind(const mpa::limbs::size_type w) noexcept
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


std::shared_ptr<const types::object_type>
arch_gcc48_x86_64::create_builtin_va_list_type() const
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
arch_gcc48_x86_64::get_wint_kind() const noexcept
{
  return std_int_type::kind::k_int;
}

bool arch_gcc48_x86_64::is_wint_signed() const noexcept
{
  return true;
}

types::std_int_type::kind arch_gcc48_x86_64::get_int_max_kind() const noexcept
{
  return std_int_type::kind::k_long;
}

types::std_int_type::kind arch_gcc48_x86_64::get_pid_t_kind() const noexcept
{
  return std_int_type::kind::k_int;
}

bool arch_gcc48_x86_64::is_pid_t_signed() const noexcept
{
  return true;
}
