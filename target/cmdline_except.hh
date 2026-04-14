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
