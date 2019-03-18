#include <ostream>
#include "code_remark_raw.hh"

using namespace klp::ccp;

code_remark_raw::code_remark_raw(const severity sev, const std::string &msg,
		       const file_range &file_range)
  : _msg(msg), _file_range(file_range), _sev(sev)
{}

std::ostream& klp::ccp::operator<<(std::ostream &o,
				   const code_remark_raw &remark)
{
  o << ((remark.get_severity() == code_remark_raw::severity::fatal)
	? "error" : "warning");
  o << ": " << remark._file_range << ": " << remark._msg << std::endl;
  return o;
}
