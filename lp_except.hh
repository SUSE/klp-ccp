#ifndef LP_EXCEPT_HH
#define LP_EXCEPT_HH

#include <exception>
#include <string>
#include "code_remark.hh"

namespace klp
{
  namespace ccp
  {
    class lp_except : std::exception
    {
    public:
      lp_except(const code_remark &remark);
      lp_except(lp_except &&) = default;

      virtual ~lp_except() noexcept;
      virtual const char* what() const noexcept;

    private:
      std::string _what;
    };
  }
}

#endif
