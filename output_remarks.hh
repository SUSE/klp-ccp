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

#ifndef OUTPUT_REMARKS_HH
#define OUTPUT_REMARKS_HH

#include <vector>
#include <ios>
#include "output_remark.hh"

namespace klp
{
  namespace ccp
  {
    class output_remarks
    {
    public:
      void add(const output_remark &r);
      void add(output_remark &&r);
      void clear() noexcept;
      bool empty() const noexcept;
      bool any_fatal() const noexcept;

      output_remarks& operator+=(const output_remarks &remarks);

      friend std::ostream& operator<<(std::ostream &o,
				      const output_remarks &rs);

    private:
      std::vector<output_remark> _remarks;
    };

    std::ostream& operator<<(std::ostream &o, const output_remarks &rs);
  }
}

#endif
