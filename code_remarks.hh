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

#ifndef CODE_REMARKS_HH
#define CODE_REMARKS_HH

#include <vector>
#include <ios>

#include "code_remark.hh"

namespace klp
{
  namespace ccp
  {
    class code_remarks
    {
    public:
      void add(const code_remark &r);
      void add(code_remark &&r);
      void clear() noexcept;
      bool empty() const noexcept;
      bool any_fatal() const noexcept;

      code_remarks& operator+=(const code_remarks &remarks);

      friend std::ostream& operator<<(std::ostream &o, const code_remarks &rs);

    private:
      std::vector<code_remark> _remarks;
    };

    std::ostream& operator<<(std::ostream &o, const code_remarks &rs);
  }
}

#endif
