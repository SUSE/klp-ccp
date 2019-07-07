#ifndef ARCH_GCC48_X86_64_HH
#define ARCH_GCC48_X86_64_HH

#include "architecture.hh"

namespace klp
{
  namespace ccp
  {
    class arch_gcc48_x86_64 final : public architecture
    {
    public:
      arch_gcc48_x86_64();

      virtual ~arch_gcc48_x86_64() noexcept override = default;

      virtual void register_builtin_macros(preprocessor &pp) const override;

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
      static types::std_int_type::kind
      _width_to_int_kind(const mpa::limbs::size_type w) noexcept;

      builtin_typedef::factories _builtin_typedefs;
    };
  }
}

#endif

