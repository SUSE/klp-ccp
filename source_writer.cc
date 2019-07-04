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
  : _filename(filename)
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
}

void source_writer::append(const newline_tag&)
{
  if (!_buffer_capacity())
    flush();
  _buf.push_back('\n');
}

source_writer::_buffer_type::size_type source_writer::_buffer_capacity()
  const noexcept
{
  assert(_buf.capacity());
  return _buf.capacity() - _buf.size();
}
