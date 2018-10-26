#ifndef FILE_RANGE_HH
#define FILE_RANGE_HH

#include <string>
#include <ios>

namespace suse
{
namespace cp
  {
    class inclusion_node;
    class header_inclusion_node;

    class file_range
    {
    public:
      file_range() = default;
      file_range(const inclusion_node &inclusion_node,
		 const std::streamoff &start_loc,
		 const std::streamoff &end_loc);
      file_range(const inclusion_node &inclusion_node,
		 const std::streamoff &loc);

      file_range(const file_range &start,
		 const file_range &end);

      bool operator==(const file_range &rhs) const noexcept;

      const header_inclusion_node& get_header_inclusion_node() const noexcept;

      std::streamoff get_start_loc() const noexcept
      { return _start_loc; }

      std::streamoff get_start_line() const noexcept;

      friend std::ostream& operator<<(std::ostream &o, const file_range &r);

    private:
      const inclusion_node *_inclusion_node;
      std::streamoff _start_loc;
      std::streamoff _end_loc;
    };

    std::ostream& operator<<(std::ostream &o, const file_range &r);
  }
}

#endif
