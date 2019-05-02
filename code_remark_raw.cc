#include <ostream>
#include "pp_token.hh"
#include "pp_tokens.hh"
#include "code_remark_raw.hh"

using namespace klp::ccp;

code_remark_raw::code_remark_raw(const severity sev, const std::string &msg,
				 const pp_result::header_inclusion_node &file,
				 const range_in_file &range_in_file)
  : _msg(msg), _file(&file), _range_in_file(range_in_file), _sev(sev)
{}

std::ostream& klp::ccp::operator<<(std::ostream &o,
				   const code_remark_raw &remark)
{
  o << ((remark.get_severity() == code_remark_raw::severity::fatal)
	? "error" : "warning");
  o << ": ";

  const auto start =
    remark._file->offset_to_line_col(remark._range_in_file.begin);
  const auto end =
    remark._file->offset_to_line_col(remark._range_in_file.end);
  o << remark._file->get_filename() << ": line " << start.first << ',';
  if (start.first == end.first) {
    if (start.second == end.second)
      o << " column " << start.second;
    else
      o << " columns " << start.second << '-' << end.second;
  } else {
    o << " column " << start.second
      << " to line " << end.first << ", column " << end.second;
  }

  o << ": " << remark._msg << std::endl;
  return o;
}
