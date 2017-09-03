#include <ostream>
#include <cassert>
#include "file_range.hh"

using namespace suse::cp;

file_range::file_range(const std::string &filename,
		       const file_loc &start_loc,
		       const file_loc &end_loc)
  : _filename(filename), _start_loc(start_loc),
    _end_loc(end_loc)
{}

file_range::file_range(const std::string &filename,
		       const file_loc &file_loc)
  : _filename(filename), _start_loc(file_loc),
    _end_loc(file_loc)
{
  assert(_start_loc <= _end_loc);
}

file_range::file_range(const file_range &start,
		       const file_range &end)
  : _filename(start._filename),
    _start_loc(start._start_loc),
    _end_loc(end._end_loc)
{
  assert(start._filename == end._filename);
  assert(start == end || start._end_loc <= end._start_loc);
}

bool file_range::operator==(const file_range &rhs) const noexcept
{
  return (this->_filename == rhs._filename &&
	  this->_start_loc == rhs._start_loc &&
	  this->_end_loc == rhs._end_loc);
}

std::ostream& suse::cp::operator<<(std::ostream &o, const file_range &r)
{
  o << r._filename << ": line " << r._start_loc.line() << ',';
  if (r._start_loc.line() == r._end_loc.line()) {
    if (r._start_loc.col() == r._end_loc.col())
      o << " columns " << r._start_loc.col();
    else
      o << " columns " << r._start_loc.col() << '-' << r._end_loc.col();
  } else {
    o << " column " << r._start_loc.col()
      << " to line " << r._end_loc.line() << ", column " << r._end_loc.col();
  }

  return o;
}
