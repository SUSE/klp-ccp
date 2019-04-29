#ifndef RAW_PP_TOKENS_RANGE_HH
#define RAW_PP_TOKENS_RANGE_HH

#include <limits>
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

      bool is_builtin() const noexcept
      {
	return (begin == std::numeric_limits<raw_pp_token_index>::max() &&
		end == 0);
      }

      bool operator<(const raw_pp_tokens_range &op) const noexcept
      { return this->end <= op.begin; }

      raw_pp_token_index begin;
      raw_pp_token_index end;
    };
  }
}


#endif
