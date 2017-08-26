#include <ostream>
#include "file_loc.hh"

using namespace suse::cp;

std::ostream& suse::cp::operator<<(std::ostream &o, const file_loc &loc)
{
  o << "line " <<  loc._line << ", col " << loc._col;
  return o;
}
