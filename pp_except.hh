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

#ifndef PP_ERROR_HH
#define PP_ERROR_HH

#include <exception>
#include <string>
#include "code_remark.hh"

namespace klp
{
  namespace ccp
  {
    class pp_except : std::exception
    {
    public:
      pp_except(const code_remark &remark);
      pp_except(const std::string &msg);
      pp_except(pp_except &&) = default;

      virtual ~pp_except() noexcept;
      virtual const char* what() const noexcept;

    private:
      std::string _what;
    };
  }
}

#endif
