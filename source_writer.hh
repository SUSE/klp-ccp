#ifndef _SOURCE_WRITER_HH
#define _SOURCE_WRITER_HH

#include <vector>
#include <string>
#include "range_in_file.hh"

namespace klp
{
  namespace ccp
  {
    class source_reader;

    class source_writer
    {
    public:
      struct newline_tag{};

      source_writer(const std::string &filename);
      ~source_writer() noexcept;

      void flush();

      void append(source_reader &from_file, const range_in_file &r);
      void append(const std::string &s);
      void append(const newline_tag&);

    private:
      typedef std::vector<char> _buffer_type;

      _buffer_type::size_type _buffer_capacity() const noexcept;

      const std::string _filename;
      int _fd;
      _buffer_type _buf;
    };
  }
}

#endif
