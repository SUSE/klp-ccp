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

      std::streamoff get_cur_line() const noexcept
      { return _cur_line; }

    private:
      typedef std::vector<char> _buffer_type;

      _buffer_type::size_type _buffer_capacity() const noexcept;

      const std::string _filename;
      int _fd;
      _buffer_type _buf;
      std::streamoff _cur_line;
    };
  }
}

#endif
