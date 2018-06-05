#include "source_reader.hh"
#include <cerrno>
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace suse::cp;

source_reader::~source_reader() noexcept = default;


file_source_reader::file_source_reader(const std::string &filename)
  : _filename(filename)
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
  buffer_type buf;
  buf.resize(4096);

  const ssize_t r = ::read(_fd, &buf[0], buf.size());
  if (r < 0)
    throw std::system_error(errno, std::system_category(), _filename);

  buf.resize(static_cast<buffer_type::size_type>(r));
  return buf;
}


buf_source_reader::buf_source_reader(const std::string &buf)
  : _buf(buf.begin(), buf.end())
{}

buf_source_reader::~buf_source_reader() noexcept = default;

source_reader::buffer_type buf_source_reader::read()
{
  return std::move(_buf);
}
