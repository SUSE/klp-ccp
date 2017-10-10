#include <ostream>
#include <cassert>
#include "file_range.hh"
#include "header_inclusion_tree.hh"

using namespace suse::cp;

file_range::file_range(const header_inclusion_node &file,
		       const std::streamoff &start_loc,
		       const std::streamoff &end_loc)
  : _file(&file), _start_loc(start_loc), _end_loc(end_loc)
{}

file_range::file_range(const header_inclusion_node &file,
		       const std::streamoff &loc)
  : _file(&file), _start_loc(loc), _end_loc(loc)
{}

file_range::file_range(const file_range &start,
		       const file_range &end)
  : _file(start._file), _start_loc(start._start_loc), _end_loc(end._end_loc)
{
  assert(start._file == end._file);
  assert(start == end || start._end_loc <= end._start_loc);
}

bool file_range::operator==(const file_range &rhs) const noexcept
{
  return (this->_file == rhs._file &&
	  this->_start_loc == rhs._start_loc &&
	  this->_end_loc == rhs._end_loc);
}

std::streamoff file_range::get_start_line() const noexcept
{
  return _file->offset_to_line_col(_start_loc).first;
}

std::ostream& suse::cp::operator<<(std::ostream &o, const file_range &r)
{
  const std::pair<std::streamoff, std::streamoff> start
    = r._file->offset_to_line_col(r._start_loc);
  const std::pair<std::streamoff, std::streamoff> end
    = r._file->offset_to_line_col(r._end_loc);
  o << r._file->get_filename() << ": line " << start.first << ',';
  if (start.first == end.first) {
    if (start.second == end.second)
      o << " column " << start.second;
    else
      o << " columns " << start.second << '-' << end.second;
  } else {
    o << " column " << start.second
      << " to line " << end.first << ", column " << end.second;
  }

  return o;
}
