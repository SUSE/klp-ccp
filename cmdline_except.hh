#ifndef CMDLINE_EXCEPT_HH
#define CMDLINE_EXCEPT_HH

#include <stdexcept>
#include <string>

namespace klp
{
  namespace ccp
  {
    class cmdline_except : public std::exception
    {
    public:
      explicit cmdline_except(std::string &&what);

      virtual ~cmdline_except() noexcept override;

      virtual const char* what() const noexcept override;

    private:
      const std::string _what;
    };
  }
}


#endif
