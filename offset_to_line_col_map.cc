#include <limits>
#include <cassert>
#include "offset_to_line_col_map.hh"

using namespace suse::cp;

template<unsigned int val, unsigned int shift, typename = void>
struct _next_pow_of_two
{
  static constexpr unsigned int value = val;
};

template <unsigned int val, unsigned int shift>
struct _next_pow_of_two<val, shift,
	typename
	std::enable_if<(shift < std::numeric_limits<unsigned int>::digits)>
		::type>
{
  static constexpr unsigned int value
    = _next_pow_of_two<val | (val >> shift), shift * 2>::value;
};

template<unsigned int val>
struct next_pow_of_two
{
  static constexpr int value = _next_pow_of_two<val - 1, 1>::value + 1;
};

template<>
struct next_pow_of_two<0>
{
  static constexpr int value = 0;
};


template<unsigned int r, unsigned int val, typename = void>
struct _static_fls
{
  static constexpr unsigned int value = r;
};

template<unsigned int r, unsigned int val>
struct _static_fls<r, val,
		   typename std::enable_if<(val >> (r + 1))>::type>
{
  static constexpr unsigned int value = _static_fls<r + 1, val>::value;
};

template<unsigned int val>
struct static_fls
{
  static constexpr unsigned int value = _static_fls<0, val>::value + 1;
};

template<>
struct static_fls<0>
{
  static constexpr unsigned int value = 0;
};


template<typename T>
unsigned int fls(T val)
{
  unsigned int r = 0;

  // Optimize the common case of lines shorter than 256 columns
  if (val < (1 << 8)) {
    for (int i = 2; i >= 0; --i) {
      const unsigned int b = 1 << i;
      if (val & (((1 << b) - 1) << b)) {
	val >>= b;
	r |= b;
      }
    }

    assert(val == 1 || val == 0);
    return r + static_cast<unsigned int>(val);
  }

  // The general case
  for (int i =
	static_fls<next_pow_of_two<std::numeric_limits<T>::digits>::value / 2>
		::value - 1;
       i >= 0; --i) {
    const unsigned int b = 1 << i;
    if (val & (((1 << b) - 1) << b)) {
      val >>= b;
      r |= b;
    }
  }

  return r + 1;
}

void offset_to_line_col_map::add_line(const std::streamoff length)
{
  const _enc_map_type::const_iterator enc_it = _append_encoded_offset(length);
  const std::streamoff next_offset = _last_offset + length;

  // Add one index entry for each multiple of _offset_range_per_index
  // in the range [_last_offset, next_offset).
  const std::streamoff lo_rounded_up =
    ((_last_offset + _offset_range_per_index - 1)
     / _offset_range_per_index * _offset_range_per_index);
  const std::streamoff no_rounded_up =
    ((next_offset + _offset_range_per_index - 1)
     / _offset_range_per_index * _offset_range_per_index);
  for (std::streamoff o = lo_rounded_up; o < no_rounded_up;
       o += _offset_range_per_index) {
    assert(_last_offset <= _index.size() * _offset_range_per_index);
    _index.emplace_back(enc_it - _enc_map.cbegin(), _last_offset, _cur_line);
  }

  _last_offset = next_offset;
  ++_cur_line;
}

std::pair<std::streamoff, std::streamoff>
offset_to_line_col_map::offset_to_line_col(std::streamoff off)
  const noexcept
{
  _enc_map_type::const_iterator enc_it = _enc_map.cbegin();
  std::streamoff line = 1;

  if (!_index.empty()) {
    _index_type::size_type i = off / _offset_range_per_index;
    if (i >= _index.size())
      i = _index.size() - 1;

    off -= _index[i].offset;
    enc_it = _enc_map.cbegin() + _index[i].offset_in_enc;
    line = _index[i].line;
  }

  while (enc_it != _enc_map.cend()) {
    const std::streamoff line_length = _read_encoded_offset(enc_it);

    if (off < line_length)
      break;
    off -= line_length;
    ++line;
  }

  return std::make_pair(line, off);
}

offset_to_line_col_map::_enc_map_type::const_iterator
offset_to_line_col_map::_append_encoded_offset(const std::streamoff off)
{
  static_assert(std::numeric_limits<unsigned char>::digits >= sizeof(off),
		"too few bits in char -- multibyte headers not implemented");

  // Optimize the common case of a line length < 128 characters.
  if (off < (1 << (std::numeric_limits<unsigned char>::digits - 1))) {
    _enc_map.push_back(static_cast<unsigned char>(off));
    return _enc_map.cend() - 1;
  }

  // General case
  const unsigned int needed_payload_bits = fls(off);
  unsigned int needed_payload_bytes =
    needed_payload_bits / std::numeric_limits<unsigned char>::digits;

  unsigned int avail_payload_bits_in_header =
    std::numeric_limits<unsigned char>::digits - needed_payload_bytes;
  avail_payload_bits_in_header -= avail_payload_bits_in_header ? 1 : 0;
  if ((needed_payload_bits -
       needed_payload_bytes * std::numeric_limits<unsigned char>::digits) >
      avail_payload_bits_in_header) {
    ++needed_payload_bytes;
    --avail_payload_bits_in_header;
  }

  _enc_map.reserve(_enc_map.size() + 1 + needed_payload_bytes);
  // UTF8-style multibyte encoding: number of leading ones in header
  // denotes number of payload bytes to follow.
  unsigned char header
    = (static_cast<unsigned char>
       (((1 << needed_payload_bytes) - 1) <<
	 (std::numeric_limits<unsigned char>::digits - needed_payload_bytes)));

  const unsigned int non_header_payload_bits =
    needed_payload_bytes * std::numeric_limits<unsigned char>::digits;
  unsigned int remaining_payload_bits = needed_payload_bits;
  if (needed_payload_bits > non_header_payload_bits) {
    header |= off >> non_header_payload_bits;
    remaining_payload_bits = non_header_payload_bits;
  }

  _enc_map.push_back(header);

  if (remaining_payload_bits < non_header_payload_bits) {
    remaining_payload_bits -=
      remaining_payload_bits % std::numeric_limits<unsigned char>::digits;
    _enc_map.push_back((static_cast<unsigned char>
			(off >> remaining_payload_bits)));
  }

  while (remaining_payload_bits > 0) {
    remaining_payload_bits -= std::numeric_limits<unsigned char>::digits;
    _enc_map.push_back((off >> remaining_payload_bits) & 0xff);
  }

  return _enc_map.cend() - 1 - needed_payload_bytes;
}

std::streamoff
offset_to_line_col_map::_read_encoded_offset(_enc_map_type::const_iterator &it)
  const noexcept
{
  const unsigned char header = *it++;
  unsigned int payload_bytes =
    (std::numeric_limits<unsigned char>::digits -
     fls(static_cast<unsigned char>(~header)));
  unsigned int payload_bits_in_header =
    std::numeric_limits<unsigned char>::digits - payload_bytes;
  payload_bits_in_header -= payload_bits_in_header ? 1 : 0;

  std::streamoff r = header & ((1 << payload_bits_in_header) - 1);
  for (; payload_bytes > 0; --payload_bytes) {
    r <<= std::numeric_limits<unsigned char>::digits;
    r |= *it++;
  }

  return r;
}
