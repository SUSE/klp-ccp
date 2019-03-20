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

      bool is_builtin() const noexcept
      {
	return (begin == std::numeric_limits<pp_token_index>::max() &&
		end == 0);
      }

      pp_token_index begin;
      pp_token_index end;
    };

#ifdef DEBUG_PARSER
    std::ostream& operator<<(std::ostream &o, const pp_tokens_range &r);
#endif
  }
}

#endif
