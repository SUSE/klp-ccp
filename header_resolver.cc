#include "header_resolver.hh"
#include "path.hh"

using namespace klp::ccp;

const struct header_resolver::cwd_tag header_resolver::cwd;

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
  const std::string dir = dirname(referring_file);
  std::string p;
  if (!dir.empty())
    p = normalize_path(dirname(referring_file) + '/' + header);
  else
    p = header;

  if (file_readable(p))
    return p;

  return resolve(header);
}

struct cwd_tag {};
std::string header_resolver::resolve(const std::string &header, const cwd_tag&)
  const
{
  std::string p{normalize_path(getcwd() + '/' + header)};

  if (file_readable(p))
    return p;

  return resolve(header);
}
