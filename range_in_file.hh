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
