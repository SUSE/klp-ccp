#include "source_reader.hh"
#include "range_in_file.hh"
#include <cerrno>
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace klp::ccp;

source_reader::~source_reader() noexcept = default;


file_source_reader::file_source_reader(const std::string &filename)
  : _filename(filename), _buf_pos(0)
{
  _fd = open(_filename.c_str(), O_RDONLY);
  if (_fd < 0)
    throw std::system_error(errno, std::system_category(), _filename);
}

file_source_reader::~file_source_reader() noexcept
{
  close (_fd);
}

source_reader::buffer_type file_source_reader::read()
{
  _fill_buffer();
  _buf_pos += _buf.size();
  return std::move(_buf);
}

void file_source_reader::read(buffer_type &to_buffer,
			      const range_in_file &from_range)
{
  if (from_range.begin == from_range.end)
    return;

  if (!(_buf_pos <= from_range.begin &&
	from_range.begin < _buf_pos + _buf.size())) {
    if (lseek(_fd, from_range.begin, SEEK_SET) ==
	static_cast<off_t>(-1)) {
      throw std::system_error(errno, std::system_category(), _filename);
    }

    _buf_pos = from_range.begin;
    _buf.clear();
  }

  range_in_file::loc_type pos = from_range.begin;
  while (pos != from_range.end) {
    if (pos == _buf_pos + _buf.size()) {
      _buf_pos += _buf.size();
      _fill_buffer();
      if (_buf.empty()) {
	// Preliminary EOF.
	throw std::system_error(ENXIO, std::system_category(), _filename);
      }
    }

    const buffer_type::const_iterator it_copy_begin
      = _buf.cbegin() + (pos - _buf_pos);
    const buffer_type::size_type n
      = std::min(_buf.cend() - it_copy_begin, from_range.end - pos);
    std::copy(it_copy_begin, it_copy_begin + n, std::back_inserter(to_buffer));
    pos += n;
  }
}

void file_source_reader::_fill_buffer()
{
  _buf.resize(4096);

  const ssize_t r = ::read(_fd, &_buf[0], _buf.size());
  if (r < 0)
    throw std::system_error(errno, std::system_category(), _filename);

  _buf.resize(static_cast<buffer_type::size_type>(r));
}
