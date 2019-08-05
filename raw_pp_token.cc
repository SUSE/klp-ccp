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

#include "raw_pp_token.hh"

using namespace klp::ccp;

raw_pp_token::raw_pp_token(const pp_token::type type, const std::string &value,
			   const range_in_file &range_in_file)
  : _type(type), _value(value), _range_in_file(range_in_file)
{}

bool raw_pp_token::operator==(const raw_pp_token &rhs) const noexcept
{
  return _type == rhs._type && _value == rhs._value;
}
