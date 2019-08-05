/*
 * Copyright (C) 2019  SUSE Software Solutions Germany GmbH
 *
 * This file is part of klp-ccp.
 *
 * klp-ccp is free software: you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * klp-ccp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iterator>
#include "header_resolver.hh"
#include "path.hh"

using namespace klp::ccp;

const struct header_resolver::cwd_tag header_resolver::cwd;

std::string header_resolver::resolve(const std::string &header)
  const
{
  if (path_is_absolute(header)) {
    if (!file_readable(header))
      return std::string{};
    return header;
  }

  return _resolve(header, _search_dirs);
}

std::string header_resolver::resolve(const std::string &header,
				     const std::string &referring_file)
  const
{
  if (path_is_absolute(header)) {
    if (!file_readable(header))
      return std::string{};
    return header;
  }

  const std::string dir = dirname(referring_file);
  std::string p;
  if (!dir.empty())
    p = normalize_path(dirname(referring_file) + '/' + header);
  else
    p = header;

  if (file_readable(p))
    return p;

  p = _resolve(header, _search_dirs_quoted);
  if (p.empty())
    p = _resolve(header, _search_dirs);
  return p;
}

struct cwd_tag {};
std::string header_resolver::resolve(const std::string &header,
				     const std::string &referring_file,
				     const cwd_tag&)
  const
{
  if (path_is_absolute(header)) {
    if (!file_readable(header))
      return std::string{};
    return header;
  }

  std::string p{normalize_path(getcwd() + '/' + header)};

  if (file_readable(p))
    return p;

  return resolve(header, referring_file);
}

std::string header_resolver::_resolve(const std::string &header,
				      const _search_dirs_type &search_dirs)
  const
{
  for (auto &&d : search_dirs) {
    auto &&p = normalize_path(d + '/' + header);
    if (file_readable(p))
      return std::move(p);
  }

  return std::string{};
}
