#ifndef HEADER_RESOLVER_HH
#define HEADER_RESOLVER_HH

#include <vector>
#include <string>
#include <utility>

namespace suse
{
  namespace cp
  {
    class header_resolver
    {
    public:
      template <typename T>
      header_resolver(T &&begin, T &&end)
	: _search_dirs(std::forward<T>(begin), std::forward<T>(end))
      {}

      template <typename T>
      void append_search_dir(T &&dir)
      {
	_search_dirs.push_back(std::forward<T>(dir));
      }

      std::string resolve(const std::string &header);
      std::string resolve(const std::string &header,
			  const std::string &referring_file);

    private:
      std::vector<std::string> _search_dirs;
    };
  }
}

#endif
