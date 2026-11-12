/*
 * Copyright (C) 2024  SUSE Software Solutions Germany GmbH
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

#include "source_reader_cache.hh"
#include "source_reader.hh"

using namespace klp::ccp;

source_reader& source_reader_cache::
get(const pp_result::header_inclusion_node &h)
{
  const auto id = h.get_id();
  if (_cache.size() <= id)
    _cache.resize(id + 1);

  if (!_cache[id])
    _cache[id] = h.create_source_reader();

  return *_cache[id];
}
