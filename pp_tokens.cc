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
#include "pp_tokens.hh"
#include "pp_token.hh"

using namespace klp::ccp;

static std::string __escape_string(std::string &&s)
{
  for (auto &&it = s.begin(); it != s.end();) {
    if (*it == '"' || *it == '\\') {
      it = s.insert(it, '\\');
      it += 2;
    } else {
      ++it;
    }
  }

  return std::move(s);
}

std::string pp_tokens::stringify(const bool as_string) const
{
  std::string s;
  for (auto &&it = _tokens.begin(); it != _tokens.end(); ++it) {
    s += it->stringify();
  }

  if (as_string)
    s = __escape_string(std::move(s));

  return s;
}

void pp_tokens::shrink(const size_type new_size)
{
  assert(new_size <= _tokens.size());
  for (size_type n = _tokens.size() - new_size; n; --n)
    _tokens.pop_back();
}
