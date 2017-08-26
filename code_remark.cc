#include <ostream>
#include "code_remark.hh"

using namespace suse::cp;

code_remark::code_remark(const severity sev, const std::string &msg,
			 const std::string &filename, const file_loc loc)
  : _msg(msg), _filename(filename), _loc(loc), _sev(sev)
{}


std::ostream& suse::cp::operator<<(std::ostream &o, const code_remark &remark)
{
  o << ((remark.get_severity() == code_remark::severity::fatal)
	? "error" : "warning");
  o << ": " << remark._filename << ": " << remark._loc
    << ": " << remark._msg << std::endl;

  return o;
}
