
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

#include "pp_result.hh"

namespace klp
{
  namespace ccp
  {
    class source_reader;

    class source_reader_cache
    {
    public:
      source_reader& get(const pp_result::header_inclusion_node &h);

    private:
      std::vector<std::unique_ptr<source_reader>> _cache;
    };
  }
}
