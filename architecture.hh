#ifndef ARCHITECTURE_HH
#define ARCHITECTURE_HH

#include "types.hh"
#include "mp_arithmetic.hh"

namespace klp
{
  namespace ccp
  {
    namespace ast
    {
      class ast;
    }

    class execution_charset_encoder;

    enum class int_mode_kind
    {
      imk_none,
      imk_QI,
      imk_HI,
      imk_SI,
      imk_DI,
      imk_TI,
    };

    mpa::limbs::size_type int_mode_to_width(const int_mode_kind m) noexcept;
    int_mode_kind width_to_int_mode(const mpa::limbs::size_type w);

    enum class float_mode_kind
    {
      fmk_none,
      fmk_SF,
      fmk_DF,
    };

    class architecture
    {
    public:
      virtual ~architecture() noexcept;

      virtual bool is_char_signed() const noexcept = 0;
      virtual bool is_wchar_signed() const noexcept = 0;

      virtual bool is_bitfield_default_signed() const noexcept = 0;

      virtual mpa::limbs::size_type
      get_std_int_width(const types::std_int_type::kind k) const noexcept = 0;

      virtual types::std_int_type::kind
      int_mode_to_std_int_kind(const int_mode_kind m) const noexcept = 0;

      virtual mpa::limbs::size_type
      get_float_significand_width(const types::float_type::kind k)
	const noexcept = 0;

      virtual mpa::limbs::size_type
      get_float_exponent_width(const types::float_type::kind k)
	const noexcept = 0;

      virtual types::float_type::kind
      float_mode_to_float_kind(const float_mode_kind m) const noexcept = 0;

      virtual int_mode_kind get_pointer_mode() const noexcept = 0;

      types::std_int_type::kind get_ptrdiff_kind() const noexcept;

      virtual int_mode_kind get_word_mode() const noexcept = 0;

      virtual mpa::limbs::size_type get_biggest_alignment_log2()
	const noexcept = 0;

      virtual mpa::limbs get_std_int_size(const types::std_int_type::kind k)
	const = 0;

      virtual mpa::limbs::size_type
      get_std_int_alignment(const types::std_int_type::kind k) const = 0;

      virtual mpa::limbs get_float_size(const types::float_type::kind k,
					const bool is_complex) const = 0;

      virtual mpa::limbs::size_type
      get_float_alignment(const types::float_type::kind k,
			  const bool is_complex) const = 0;

      virtual mpa::limbs get_pointer_size() const = 0;

      virtual mpa::limbs::size_type get_pointer_alignment() const = 0;

      virtual mpa::limbs get_va_list_size() const = 0;

      virtual mpa::limbs::size_type get_va_list_alignment() const = 0;

      enum class execution_charset_encoding
      {
	ecse_char,
	ecse_wchar,
	ecse_char8,
	ecse_char16,
	ecse_char32,
      };

      virtual std::unique_ptr<execution_charset_encoder>
      get_execution_charset_encoder(const execution_charset_encoding e)
	const = 0;

      virtual void evaluate_enum_type(ast::ast &a, types::enum_content &ec,
				      const bool packed,
				      const int_mode_kind mode,
				      types::alignment &&user_align) const = 0;

      virtual void layout_struct(types::struct_or_union_content &souc,
				 const types::alignment &user_align)
	const = 0;

      virtual void layout_union(types::struct_or_union_content &souc,
				const types::alignment &user_align)
	const = 0;

      virtual std::shared_ptr<const types::object_type>
      create_builtin_va_list_type() const = 0;

      virtual types::std_int_type::kind get_wint_kind() const noexcept = 0;
      virtual bool is_wint_signed() const noexcept = 0;

      virtual types::std_int_type::kind get_int_max_kind() const noexcept = 0;

      virtual types::std_int_type::kind get_pid_t_kind() const noexcept = 0;
      virtual bool is_pid_t_signed() const noexcept = 0;
    };
  }
}

#endif
