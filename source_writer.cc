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

#include "source_writer.hh"
#include "source_reader.hh"
#include <cerrno>
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <cassert>

using namespace klp::ccp;

source_writer::source_writer(const std::string &filename)
  : _filename(filename), _cur_line(1)
{
  _fd = open(_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (_fd < 0)
    throw std::system_error(errno, std::system_category(), _filename);

  _buf.reserve(4096);
}

source_writer::~source_writer() noexcept
{
  assert(!_buf.size());
  close(_fd);
}

void source_writer::flush()
{
  _buffer_type::size_type offset = 0;
  _buffer_type::size_type remaining = _buf.size() - offset;
  while (remaining) {
    const ssize_t r = write(_fd, &_buf[offset], remaining);

    if (r < 0)
      throw std::system_error(errno, std::system_category(), _filename);

    offset += r;
    remaining -= r;
  }
  _buf.clear();
}

void source_writer::append(source_reader &from_file, const range_in_file &r)
{
  range_in_file::loc_type pos = r.begin;
  while (pos != r.end) {
    _buffer_type::size_type n = _buffer_capacity();
    if (!n) {
      flush();
      n = _buffer_capacity();
    }

    const auto remaining = r.end - pos;
    if (n > remaining)
      n = remaining;

    from_file.read(_buf,
		   range_in_file{
		     pos,
		     static_cast<range_in_file::loc_type>(pos + n)
		   });
    _cur_line += std::count(_buf.cend() - n, _buf.cend(), '\n');
    pos += n;
  }
}

void source_writer::append(const std::string &s)
{
  std::string::const_iterator it_s = s.begin();
  while (it_s != s.end()) {
    _buffer_type::size_type n = _buffer_capacity();
    if (!n) {
      flush();
      n = _buffer_capacity();
    }

    const auto remaining = s.cend() - it_s;
    if (n > remaining)
      n = remaining;

    std::copy(it_s, it_s + n, std::back_inserter(_buf));
    it_s += n;
  }
  _cur_line += std::count(s.cbegin(), s.cend(), '\n');
}

void source_writer::append(const newline_tag&)
{
  if (!_buffer_capacity())
    flush();
  _buf.push_back('\n');
  ++_cur_line;
}

source_writer::_buffer_type::size_type source_writer::_buffer_capacity()
  const noexcept
{
  assert(_buf.capacity());
  return _buf.capacity() - _buf.size();
}
