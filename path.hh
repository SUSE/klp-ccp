#ifndef PATH_HH
#define PATH_HH

#include <string>

namespace klp
{
  namespace ccp
  {
    std::string normalize_path(std::string &&p);
    std::string dirname(const std::string &p);
    bool file_readable(const std::string &p);
    std::string getcwd();
  }
}

#endif
