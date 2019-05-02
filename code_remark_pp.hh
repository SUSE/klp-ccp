#ifndef CODE_REMARK_PP_HH
#define CODE_REMARK_PP_HH

#include <string>
#include <ios>
#include <memory>
#include <functional>
#include "pp_tokens_range.hh"
#include "pp_result.hh"
#include "range_in_file.hh"

namespace klp
{
  namespace ccp
  {
    class pp_result;

    class code_remark_pp
    {
    public:
      enum class severity
	{
	  fatal,
	  warning,
	};

      code_remark_pp(const severity sev, const std::string &msg,
		     const pp_result &pp_result, const pp_tokens_range &range);

      code_remark_pp(const severity sev, const std::string &msg,
		     const pp_result &pp_result,
		     const pp_token_index first,
		     const pp_token_index last);

      code_remark_pp(const severity sev, const std::string &msg,
		     const pp_result &pp_result,
		     const pp_token_index tok_index);

      severity get_severity() const noexcept
      {
	return _sev;
      }

      friend std::ostream& operator<<(std::ostream &o,
				      const code_remark_pp &remark);

    private:
      const pp_result::header_inclusion_node *_first_file;
      const pp_result::header_inclusion_node *_last_file;
      range_in_file::loc_type _begin_loc;
      range_in_file::loc_type _end_loc;
      std::string _msg;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const code_remark_pp &remark);
  }
}

#endif
