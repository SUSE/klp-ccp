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

target::~target() noexcept = default;


target::sou_layouter::
sou_layouter(const types::struct_or_union_kind souk)
  : _souk(souk), _c(new types::struct_or_union_content{})
{}

target::sou_layouter::~sou_layouter() noexcept = default;

std::unique_ptr<types::struct_or_union_content>
target::sou_layouter::sou_layouter::grab_result() noexcept
{
  return std::move(_c);
}
