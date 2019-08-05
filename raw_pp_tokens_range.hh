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

#ifndef RAW_PP_TOKENS_RANGE_HH
#define RAW_PP_TOKENS_RANGE_HH

#include <limits>
#include <algorithm>
#include <cassert>

namespace klp
{
  namespace ccp
  {
    typedef std::size_t raw_pp_token_index;
    struct raw_pp_tokens_range
    {
      raw_pp_tokens_range() noexcept
      : begin(std::numeric_limits<raw_pp_token_index>::max()),
	end(0)
      {}

      raw_pp_tokens_range(const raw_pp_token_index _begin,
			  const raw_pp_token_index _end) noexcept
	: begin(_begin), end(_end)
      {
	assert(begin <= end);
      }

      raw_pp_tokens_range(const raw_pp_token_index index) noexcept
	: raw_pp_tokens_range(index, index + 1)
      {
	assert(begin <= end);
      }

      bool operator==(const raw_pp_tokens_range &op) const noexcept
      { return (this->begin == op.begin && this->end == op.end); }

      bool operator!=(const raw_pp_tokens_range &op) const noexcept
      { return !(*this == op); }

      bool operator<(const raw_pp_tokens_range &op) const noexcept
      { return this->end <= op.begin; }

      bool is_contained_in(const raw_pp_tokens_range &r) const noexcept
      { return this->begin >= r.begin && this->end <= r.end; }

      raw_pp_tokens_range crop(const raw_pp_tokens_range &to) const noexcept
      {
	raw_pp_tokens_range cropped {
		std::max(this->begin, to.begin),
		std::min(this->end, to.end)
	};
	assert(cropped.begin <= cropped.end);
	return cropped;
      }

      raw_pp_token_index begin;
      raw_pp_token_index end;
    };
  }
}


#endif
