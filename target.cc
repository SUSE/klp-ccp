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

#include "target.hh"
#include <stdexcept>

using namespace klp::ccp;

mpa::limbs::size_type klp::ccp::int_mode_to_width(const int_mode_kind m) noexcept
{
  switch (m) {
  case int_mode_kind::imk_none:
    assert(0);
    __builtin_unreachable();

  case int_mode_kind::imk_QI:
    return 8;

  case int_mode_kind::imk_HI:
    return 16;

  case int_mode_kind::imk_SI:
    return 32;

  case int_mode_kind::imk_DI:
    return 64;

  case int_mode_kind::imk_TI:
    return 128;
  }
}

int_mode_kind klp::ccp::width_to_int_mode(const mpa::limbs::size_type w)
{
  if (w > 128) {
    throw std::overflow_error("no mode for integer width");
  } else if (w > 64) {
    return int_mode_kind::imk_TI;
  } else if (w > 32) {
    return int_mode_kind::imk_DI;
  } else if (w > 16) {
    return int_mode_kind::imk_SI;
  } else if (w > 8) {
    return int_mode_kind::imk_HI;
  } else {
    return int_mode_kind::imk_QI;
  }
}


target::~target() noexcept = default;

types::std_int_type::kind target::get_ptrdiff_kind() const noexcept
{
  return this->int_mode_to_std_int_kind(this->get_pointer_mode());
}
