#ifndef EXECUTION_CHARSET_ENCODER_ICONV_HH
#define EXECUTION_CHARSET_ENCODER_ICONV_HH

#include "execution_charset_encoder.hh"
#include <iconv.h>

namespace suse
{
  namespace cp
  {
    class execution_charset_encoder_iconv final
      : public execution_charset_encoder
    {
    public:
      execution_charset_encoder_iconv
		(const architecture &arch,
		 const types::std_int_type::kind target_char_kind,
		 const std::string &target_code,
		 const bool target_code_is_big_endian);

      virtual ~execution_charset_encoder_iconv() noexcept override;

    protected:
      virtual std::vector<mpa::limbs>
      encode_char(ast::ast &a, const file_range &file_range,
		  char32_t ucs4_char) override;

    private:
      iconv_t _iconv;
      const bool _target_code_is_big_endian;
    };
  }
}

#endif
