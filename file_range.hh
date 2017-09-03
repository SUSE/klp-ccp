#ifndef FILE_RANGE_HH
#define FILE_RANGE_HH

#include <string>
#include <ios>
#include "file_loc.hh"

namespace suse
{
namespace cp
  {
    class file_range
    {
    public:
      file_range() = default;
      file_range(const std::string &filename,
		 const file_loc &start_loc,
		 const file_loc &end_loc);
      file_range(const std::string &filename,
		 const file_loc &file_loc);

      file_range(const file_range &start,
		 const file_range &end);

      const std::string&
      get_filename() const noexcept
      { return _filename; }

      const file_loc&
      get_start_loc() const noexcept
      { return _start_loc; }

      const file_loc&
      get_end_loc() const noexcept
      { return _end_loc; }

      bool operator==(const file_range &rhs) const noexcept;

      friend std::ostream& operator<<(std::ostream &o, const file_range &r);

    private:
      std::string _filename;
      file_loc _start_loc;
      file_loc _end_loc;
    };

    std::ostream& operator<<(std::ostream &o, const file_range &r);
  }
}

#endif
