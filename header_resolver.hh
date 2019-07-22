#ifndef HEADER_RESOLVER_HH
#define HEADER_RESOLVER_HH

#include <vector>
#include <string>
#include <utility>

namespace klp
{
  namespace ccp
  {
    class header_resolver
    {
    public:
      header_resolver() = default;

      template <typename T>
      header_resolver(T &&begin, T &&end)
	: _search_dirs(std::forward<T>(begin), std::forward<T>(end))
      {}

      template <typename T>
      void append_search_dir(T &&dir)
      {
	_search_dirs.push_back(std::forward<T>(dir));
      }

      template <typename T>
      void append_search_dir_quoted(T &&dir)
      {
	_search_dirs_quoted.push_back(std::forward<T>(dir));
      }

      std::string resolve(const std::string &header) const;
      std::string resolve(const std::string &header,
			  const std::string &referring_file) const;

      static const struct cwd_tag
      {
	cwd_tag() noexcept {}
      } cwd;

      std::string resolve(const std::string &header,
			  const std::string &referring_file,
			  const cwd_tag&) const;

    private:
      typedef std::vector<std::string> _search_dirs_type;

      std::string _resolve(const std::string &header,
			   const _search_dirs_type &search_dirs) const;

      _search_dirs_type _search_dirs_quoted;
      _search_dirs_type _search_dirs;
    };
  }
}

#endif
