#ifndef EXECUTION_CHARSET_ENCODER_HH
#define EXECUTION_CHARSET_ENCODER_HH

#include <vector>
#include "mp_arithmetic.hh"
#include "types.hh"

namespace suse
{
  namespace cp
  {
    class pp_token;
    class file_range;

    namespace ast
    {
      class ast;
    }

    class execution_charset_encoder
    {
    public:
      virtual ~execution_charset_encoder() noexcept = default;

      std::vector<mpa::limbs>
      encode_string(ast::ast &a, const pp_token &tok);

      types::std_int_type::kind get_target_char_kind() const noexcept
      { return _target_char_kind; }

    protected:
      execution_charset_encoder(const architecture &arch,
			const types::std_int_type::kind target_char_kind)
	noexcept;

      virtual std::vector<mpa::limbs>
      encode_char(ast::ast &a, const file_range &file_range,
		  char32_t ucs4_char) = 0;

      mpa::limbs::size_type
      get_target_char_width() const noexcept
      { return _target_char_width; }

    private:
      const types::std_int_type::kind _target_char_kind;
      const mpa::limbs::size_type _target_char_width;
    };
  }
}


#endif
