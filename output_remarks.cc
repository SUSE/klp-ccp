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
#include "output_remarks.hh"

using namespace klp::ccp;

void output_remarks::add(const output_remark &r)
{
  _remarks.push_back(r);
}

void output_remarks::add(output_remark &&r)
{
  _remarks.push_back(std::move(r));
}

void output_remarks::clear() noexcept
{
  _remarks.clear();
}

bool output_remarks::empty() const noexcept
{
  return _remarks.empty();
}

bool output_remarks::any_fatal() const noexcept
{
  for(auto &&r : _remarks) {
    if (r.get_severity() == output_remark::severity::fatal)
      return true;
  }

  return false;
}

output_remarks& output_remarks::operator+=(const output_remarks &remarks)
{
  _remarks.insert(_remarks.end(),
		  remarks._remarks.begin(), remarks._remarks.end());
  return *this;
}

std::ostream& klp::ccp::operator<<(std::ostream &o, const output_remarks &rs)
{
  for(auto &&r : rs._remarks) {
    o << r;
  }
  return o;
}
