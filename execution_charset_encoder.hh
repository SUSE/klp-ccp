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

#ifndef EXECUTION_CHARSET_ENCODER_HH
#define EXECUTION_CHARSET_ENCODER_HH

#include <vector>
#include "mp_arithmetic.hh"
#include "types.hh"
#include "pp_tokens_range.hh"

namespace klp
{
  namespace ccp
  {
    namespace ast
    {
      class ast;
    }

    class execution_charset_encoder
    {
    public:
      virtual ~execution_charset_encoder() noexcept = default;

      std::vector<mpa::limbs>
      encode_string(ast::ast &a, const pp_token_index &tok_ix);

      types::std_int_type::kind get_target_char_kind() const noexcept
      { return _target_char_kind; }

    protected:
      execution_charset_encoder(const architecture &arch,
			const types::std_int_type::kind target_char_kind)
	noexcept;

      virtual std::vector<mpa::limbs>
      encode_char(ast::ast &a, const pp_token_index error_reporting_tok_ix,
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
