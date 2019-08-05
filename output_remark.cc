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

#include <ostream>
#include "output_remark.hh"

using namespace klp::ccp;

output_remark::output_remark(const severity sev, const std::streamoff line,
			     const std::string &msg)
  : _sev(sev), _line(line), _msg(msg)
{}

std::ostream& klp::ccp::operator<<(std::ostream &o, const output_remark &remark)
{
  o << ((remark.get_severity() == output_remark::severity::fatal)
	? "error" : "warning")
    << ": output line " << remark._line << ": " << remark._msg << std::endl;
  return o;
}
