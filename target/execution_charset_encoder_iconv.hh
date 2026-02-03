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

#ifndef EXECUTION_CHARSET_ENCODER_ICONV_HH
#define EXECUTION_CHARSET_ENCODER_ICONV_HH

#include "execution_charset_encoder.hh"
#include <iconv.h>

namespace klp
{
  namespace ccp
  {
    class execution_charset_encoder_iconv final
      : public execution_charset_encoder
    {
    public:
      execution_charset_encoder_iconv
		(const target &tgt,
		 const types::std_int_type::kind target_char_kind,
		 const std::string &target_code,
		 const bool target_code_is_big_endian);

      virtual ~execution_charset_encoder_iconv() noexcept override;

    protected:
      virtual std::vector<mpa::limbs>
      encode_char(ast::ast &a, const pp_token_index error_reporting_tok_ix,
		  char32_t ucs4_char) override;

    private:
      iconv_t _iconv;
      const bool _target_code_is_big_endian;
    };
  }
}

#endif
