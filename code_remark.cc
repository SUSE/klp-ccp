#include <ostream>
#include "code_remark.hh"

using namespace suse::cp;


code_remark::code_remark(const severity sev, const std::string &msg,
			 const file_range &file_range)
  : _msg(msg), _file_range(file_range), _sev(sev)
{}

std::ostream& suse::cp::operator<<(std::ostream &o, const code_remark &remark)
{
  o << ((remark.get_severity() == code_remark::severity::fatal)
	? "error" : "warning")
    << ": "<< remark._file_range
    << ": " << remark._msg << std::endl;

  return o;
}
