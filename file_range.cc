#include <ostream>
#include <cassert>
#include "file_range.hh"
#include "inclusion_tree.hh"

using namespace suse::cp;

file_range::file_range(const inclusion_node &inclusion_node,
		       const loc_type &start_loc,
		       const loc_type &end_loc)
  : _inclusion_node(&inclusion_node), _start_loc(start_loc), _end_loc(end_loc)
{}

file_range::file_range(const inclusion_node &inclusion_node,
		       const loc_type &loc)
  : _inclusion_node(&inclusion_node), _start_loc(loc), _end_loc(loc)
{}

file_range::file_range(const file_range &start,
		       const file_range &end)
  : _inclusion_node(start._inclusion_node), _start_loc(start._start_loc),
    _end_loc(end._end_loc)
{
  assert(start._inclusion_node == end._inclusion_node);
  assert(start == end || start._end_loc <= end._start_loc);
}

bool file_range::operator==(const file_range &rhs) const noexcept
{
  return (this->_inclusion_node == rhs._inclusion_node &&
	  this->_start_loc == rhs._start_loc &&
	  this->_end_loc == rhs._end_loc);
}

const header_inclusion_node& file_range::get_header_inclusion_node()
  const noexcept
{
  return _inclusion_node->get_containing_header();
}

file_range::loc_type file_range::get_start_line() const noexcept
{
  return get_header_inclusion_node().offset_to_line_col(_start_loc).first;
}

std::ostream& suse::cp::operator<<(std::ostream &o, const file_range &r)
{
  const header_inclusion_node &file = r.get_header_inclusion_node();
  typedef file_range::loc_type loc_type;
  const std::pair<loc_type, loc_type> start
    = file.offset_to_line_col(r._start_loc);
  const std::pair<loc_type, loc_type> end
    = file.offset_to_line_col(r._end_loc);
  o << file.get_filename() << ": line " << start.first << ',';
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
