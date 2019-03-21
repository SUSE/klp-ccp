#ifndef CODE_REMARK_RAW_HH
#define CODE_REMARK_RAW_HH

#include <string>
#include <ios>
#include <memory>
#include "file_range.hh"

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
		      const file_range &file_range);

      severity get_severity() const noexcept
      {
	return _sev;
      }

      friend std::ostream& operator<<(std::ostream &o,
				      const code_remark_raw &remark);

    private:
      std::string _msg;
      file_range _file_range;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const code_remark_raw &remark);
  }
}

#endif