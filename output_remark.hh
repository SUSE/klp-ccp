#ifndef OUTPUT_REMARK_HH
#define OUTPUT_REMARK_HH

#include <string>
#include <ios>

namespace klp
{
  namespace ccp
  {
    class output_remark
    {
    public:
      enum class severity
      {
	fatal,
	warning
      };

      output_remark(const severity sev, const std::streamoff line,
		    const std::string &msg);

      severity get_severity() const noexcept
      { return _sev; }

      friend std::ostream& operator<<(std::ostream &o,
				      const output_remark &remark);

    private:
      std::string _msg;
      std::streamoff _line;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const output_remark &remark);
  }
}

#endif
