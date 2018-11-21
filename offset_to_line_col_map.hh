#ifndef OFFSET_TO_LINE_COL_MAP_HH
#define OFFSET_TO_LINE_COL_MAP_HH

#include <ios>
#include <vector>

#include "file_range.hh"

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

      void add_line(const file_range::loc_type length);

      std::pair<file_range::loc_type, file_range::loc_type>
      offset_to_line_col(const file_range::loc_type off) const noexcept;

    private:
      typedef std::vector<unsigned char> _enc_map_type;

      _enc_map_type::const_iterator
      _append_encoded_offset(const file_range::loc_type off);

      file_range::loc_type
      _read_encoded_offset(_enc_map_type::const_iterator &it) const noexcept;

      std::vector<unsigned char> _enc_map;

      static constexpr unsigned int _offset_range_per_index = 1024;

      struct _index_entry_type
      {
	_index_entry_type(const _enc_map_type::difference_type _offset_in_enc,
			  const file_range::loc_type _offset,
			  const file_range::loc_type _line) noexcept
	  : offset_in_enc(_offset_in_enc), offset(_offset), line(_line)
	{}

	_enc_map_type::difference_type offset_in_enc;
	file_range::loc_type offset;
	file_range::loc_type line;
      };

      typedef std::vector<_index_entry_type> _index_type;
      _index_type _index;

      file_range::loc_type _last_offset;
      file_range::loc_type _cur_line;
    };
  }
}
#endif
