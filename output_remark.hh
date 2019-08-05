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

#ifndef OUTPUT_REMARK_HH
#define OUTPUT_REMARK_HH

#include <string>
#include <ios>

namespace klp
{
  namespace ccp
  {
    class output_remark
    {
    public:
      enum class severity
      {
	fatal,
	warning
      };

      output_remark(const severity sev, const std::streamoff line,
		    const std::string &msg);

      severity get_severity() const noexcept
      { return _sev; }

      friend std::ostream& operator<<(std::ostream &o,
				      const output_remark &remark);

    private:
      std::string _msg;
      std::streamoff _line;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const output_remark &remark);
  }
}

#endif
