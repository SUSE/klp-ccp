#ifndef SEMANTIC_EXCEPT_HH
#define SEMANTIC_EXCEPT_HH

#include <exception>
#include <string>
#include "code_remark.hh"

namespace suse
{
  namespace cp
  {
    class semantic_except : std::exception
    {
    public:
      semantic_except(const code_remark &remark);
      semantic_except(semantic_except &&) = default;

      virtual ~semantic_except() noexcept;
      virtual const char* what() const noexcept;

    private:
      std::string _what;
    };
  }
}

#endif
