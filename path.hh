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

#ifndef PATH_HH
#define PATH_HH

#include <string>

namespace klp
{
  namespace ccp
  {
    std::string normalize_path(std::string &&p);
    bool path_is_absolute(const std::string &p);
    std::string dirname(const std::string &p);
    bool file_readable(const std::string &p);
    std::string getcwd();
  }
}

#endif
