#ifndef CODE_REMARK_HH
#define CODE_REMARK_HH

#include <string>
#include <ios>
#include "file_loc.hh"

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
		  const std::string &filename, const file_loc loc);

      severity get_severity() const noexcept
      {
	return _sev;
      }

      friend std::ostream& operator<<(std::ostream &o,
				      const code_remark &remark);

    private:
      std::string _msg;
      std::string _filename;
      file_loc _loc;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const code_remark &remark);
  }
}

#endif
