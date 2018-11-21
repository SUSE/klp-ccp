#ifndef PARSE_EXCEPT_HH
#define PARSE_EXCEPT_HH

#include <exception>
#include <string>
#include "code_remark.hh"

namespace klp
{
  namespace ccp
  {
    class parse_except : std::exception
    {
    public:
      parse_except(const code_remark &remark);
      parse_except(parse_except &&) = default;

      virtual ~parse_except() noexcept;
      virtual const char* what() const noexcept;

    private:
      std::string _what;
    };
  }
}

#endif
