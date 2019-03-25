#ifndef CODE_REMARK_PP_HH
#define CODE_REMARK_PP_HH

#include <string>
#include <ios>
#include <memory>
#include <functional>
#include "pp_tokens_range.hh"
#include "file_range.hh"

namespace klp
{
  namespace ccp
  {
    class pp_tokens;

    class code_remark_pp
    {
    public:
      enum class severity
	{
	  fatal,
	  warning,
	};

      code_remark_pp(const severity sev, const std::string &msg,
		     const pp_tokens &tokens, const pp_tokens_range &range);

      code_remark_pp(const severity sev, const std::string &msg,
		     const pp_tokens &tokens,
		     const pp_token_index first,
		     const pp_token_index last);

      code_remark_pp(const severity sev, const std::string &msg,
		     const pp_tokens &tokens,
		     const pp_token_index tok_index);

      severity get_severity() const noexcept
      {
	return _sev;
      }

      friend std::ostream& operator<<(std::ostream &o,
				      const code_remark_pp &remark);

    private:
      std::string _msg;
      file_range _fr;
      file_range _fr_end;
      bool _fr_end_valid;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const code_remark_pp &remark);
  }
}

#endif
