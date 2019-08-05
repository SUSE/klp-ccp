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

#include "parse_except.hh"
#include <sstream>

using namespace klp::ccp;

parse_except::parse_except(const code_remark &remark)
{
  std::stringstream ss;

  ss << remark;
  _what = ss.str();
}

parse_except::~parse_except() noexcept = default;

const char* parse_except::what() const noexcept
{
  return _what.c_str();
}
