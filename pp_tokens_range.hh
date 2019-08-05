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

#ifndef PP_TOKENS_RANGE_HH
#define PP_TOKENS_RANGE_HH

#include <limits>
#include <cassert>

#ifdef DEBUG_PARSER
#include <iostream>
#endif

namespace klp
{
  namespace ccp
  {
    typedef std::size_t pp_token_index;
    struct pp_tokens_range
    {
      pp_tokens_range() noexcept
      : begin(std::numeric_limits<pp_token_index>::max()),
	end(0)
      {}

      pp_tokens_range(const pp_token_index _begin,
		      const pp_token_index _end) noexcept
	: begin(_begin), end(_end)
      {
	assert(begin <= end);
      }

      bool operator==(const pp_tokens_range &op) const noexcept
      { return (this->begin == op.begin && this->end == op.end); }

      bool operator!=(const pp_tokens_range &op) const noexcept
      { return !(*this == op); }

      bool operator<(const pp_tokens_range &op) const noexcept
      { return this->end <= op.begin; }

      bool is_contained_in(const pp_tokens_range &r) const noexcept
      { return this->begin >= r.begin && this->end <= r.end; }

      pp_token_index begin;
      pp_token_index end;
    };

#ifdef DEBUG_PARSER
    std::ostream& operator<<(std::ostream &o, const pp_tokens_range &r);
#endif
  }
}

#endif
