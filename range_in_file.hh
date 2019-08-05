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

#ifndef RANGE_IN_FILE_HH
#define RANGE_IN_FILE_HH

#include <ios>
#include <limits>

namespace klp
{
  namespace ccp
  {
    struct range_in_file
    {
      typedef std::streamoff loc_type;

      range_in_file() noexcept
	: begin(std::numeric_limits<loc_type>::max()), end(0)
      {}

      range_in_file(const loc_type _begin, const loc_type _end) noexcept
	: begin(_begin), end(_end)
      {}

      range_in_file(const loc_type loc) noexcept
	: range_in_file(loc, loc)
      {}

      loc_type begin;
      loc_type end;
    };
  }
}

#endif
