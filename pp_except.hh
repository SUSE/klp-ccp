#ifndef PP_ERROR_HH
#define PP_ERROR_HH

#include <exception>
#include <string>
#include "code_remark.hh"

namespace klp
{
  namespace ccp
  {
    class pp_except : std::exception
    {
    public:
      pp_except(const code_remark &remark);
      pp_except(pp_except &&) = default;

      virtual ~pp_except() noexcept;
      virtual const char* what() const noexcept;

    private:
      std::string _what;
    };
  }
}

#endif
