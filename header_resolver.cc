#include "header_resolver.hh"
#include "path.hh"

using namespace suse::cp;

std::string header_resolver::resolve(const std::string &header)
  const
{
  for (auto &&d : _search_dirs) {
    auto &&p = normalize_path(d + '/' + header);
    if (file_readable(p))
      return std::move(p);
  }

  return std::string{};
}

std::string header_resolver::resolve(const std::string &header,
				     const std::string &referring_file)
  const
{
  std::string p{normalize_path(dirname(referring_file) + '/' + header)};

  if (file_readable(p))
    return p;

  return resolve(header);
}
