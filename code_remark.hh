#ifndef CODE_REMARK_HH
#define CODE_REMARK_HH

#include <string>
#include <ios>
#include "file_loc.hh"
#include "file_range.hh"

namespace suse
{
  namespace cp
  {
    class code_remark
    {
    public:
      enum class severity
	{
	  fatal,
	  warning,
	};

      code_remark(const severity sev, const std::string &msg,
		  const file_range &file_range);

      severity get_severity() const noexcept
      {
	return _sev;
      }

      friend std::ostream& operator<<(std::ostream &o,
				      const code_remark &remark);

    private:
      std::string _msg;
      file_range _file_range;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const code_remark &remark);
  }
}

#endif
