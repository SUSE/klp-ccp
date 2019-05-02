#ifndef CODE_REMARK_RAW_HH
#define CODE_REMARK_RAW_HH

#include <string>
#include <ios>
#include <memory>
#include "range_in_file.hh"
#include "pp_result.hh"

namespace klp
{
  namespace ccp
  {
    class code_remark_raw
    {
    public:
      enum class severity
	{
	  fatal,
	  warning,
	};

      code_remark_raw(const severity sev, const std::string &msg,
		      const pp_result::header_inclusion_node &file,
		      const range_in_file &range_in_file);

      severity get_severity() const noexcept
      {
	return _sev;
      }

      friend std::ostream& operator<<(std::ostream &o,
				      const code_remark_raw &remark);

    private:
      std::string _msg;
      const pp_result::header_inclusion_node *_file;
      range_in_file _range_in_file;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const code_remark_raw &remark);
  }
}

#endif
