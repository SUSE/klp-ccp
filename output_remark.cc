#include <ostream>
#include "output_remark.hh"

using namespace klp::ccp;

output_remark::output_remark(const severity sev, const std::streamoff line,
			     const std::string &msg)
  : _sev(sev), _line(line), _msg(msg)
{}

std::ostream& klp::ccp::operator<<(std::ostream &o, const output_remark &remark)
{
  o << ((remark.get_severity() == output_remark::severity::fatal)
	? "error" : "warning")
    << ": output line " << remark._line << ": " << remark._msg << std::endl;
  return o;
}
