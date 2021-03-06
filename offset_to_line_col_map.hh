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

#ifndef OFFSET_TO_LINE_COL_MAP_HH
#define OFFSET_TO_LINE_COL_MAP_HH

#include <ios>
#include <vector>

namespace klp
{
  namespace ccp
  {
    class offset_to_line_col_map
    {
    public:
      offset_to_line_col_map()
	: _last_offset(0), _cur_line(1)
      {}

      void add_line(const std::streamoff length);

      std::pair<std::streamoff, std::streamoff>
      offset_to_line_col(const std::streamoff off) const noexcept;

    private:
      typedef std::vector<unsigned char> _enc_map_type;

      _enc_map_type::const_iterator
      _append_encoded_offset(const std::streamoff off);

      std::streamoff
      _read_encoded_offset(_enc_map_type::const_iterator &it) const noexcept;

      std::vector<unsigned char> _enc_map;

      static constexpr unsigned int _offset_range_per_index = 1024;

      struct _index_entry_type
      {
	_index_entry_type(const _enc_map_type::difference_type _offset_in_enc,
			  const std::streamoff _offset,
			  const std::streamoff _line) noexcept
	  : offset_in_enc(_offset_in_enc), offset(_offset), line(_line)
	{}

	_enc_map_type::difference_type offset_in_enc;
	std::streamoff offset;
	std::streamoff line;
      };

      typedef std::vector<_index_entry_type> _index_type;
      _index_type _index;

      std::streamoff _last_offset;
      std::streamoff _cur_line;
    };
  }
}
#endif
