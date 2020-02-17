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

#include <algorithm>
#include <cassert>
#include "pp_token.hh"
#include "code_remarks.hh"
#include "pp_except.hh"

using namespace klp::ccp;

pp_token::pp_token(const type type, const std::string &value,
		   const raw_pp_tokens_range &token_source)
  : _value(value), _token_source(token_source), _type(type)
{}

pp_token::pp_token(const type type, std::string &&value,
		   const raw_pp_tokens_range &token_source) noexcept
  : _value(std::move(value)), _token_source(token_source), _type(type)
{}

void pp_token::set_type_and_value(const type type, const std::string &value)
{
  _type = type;
  _value = value;
}

std::string pp_token::stringify(const type type, const std::string &value)
{
  switch (type) {
  case pp_token::type::ws:
    return value;

  case pp_token::type::newline:
    return value;

  case pp_token::type::id:
    return value;

  case pp_token::type::pp_number:
    return value;

  case pp_token::type::chr:
    return '\'' + value + '\'';

  case pp_token::type::wchr:
    return "L'" + value + '\'';

  case pp_token::type::uchr16:
    return "u'" + value + '\'';

  case pp_token::type::uchr32:
    return "U'" + value + '\'';

  case pp_token::type::str:
  case pp_token::type::qstr:
    return '"' + value + '"';

  case pp_token::type::wstr:
    return "L\"" + value + '"';

  case pp_token::type::ustr8:
    return "u\"" + value + '"';

  case pp_token::type::ustr16:
    return "u\"" + value + '"';

  case pp_token::type::ustr32:
    return "U\"" + value + '"';

  case pp_token::type::hstr:
    return '<' + value + '>';

  case pp_token::type::punctuator:
    return value;

  case pp_token::type::non_ws_char:
    return value;

  case pp_token::type::eof:
  case pp_token::type::empty:
   return "";
  }

  __builtin_unreachable();
}

std::string pp_token::stringify() const
{
  return stringify(_type, _value);
}
