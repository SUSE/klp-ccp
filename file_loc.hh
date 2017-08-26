#ifndef FILE_LOC_HH
#define FILE_LOC_HH

#include <ios>

namespace suse
{
  namespace cp
  {
    class file_loc
    {
    public:
      file_loc()
	: _raw_off(0), _line(1), _col(0)
      {}

      file_loc(const std::streamoff raw_off,
	       const std::streamoff line, const std::streamoff col)
	: _raw_off(raw_off), _line(line), _col(col)
      {}

      file_loc operator+(const char c) const noexcept
      {
	if (!c)
	  return *this;

	auto raw_off = _raw_off;
	auto line = _line;
	auto col = _col;

	if (c == '\n') {
	  ++line;
	  col = 0;
	}

	return file_loc(raw_off, line, col);
      }

      template <typename T>
      file_loc& operator+=(T v)
      {
	*this = *this + v;
	return *this;
      }

      struct continuation_tag
      {};
      static const continuation_tag continuation;

      file_loc operator+(const continuation_tag&) const noexcept
      {
	return file_loc(_raw_off + 2, _line + 1, 0);
      }

      std::streamoff raw_off() const noexcept
      {
	return _raw_off;
      }

      std::streamoff line() const noexcept
      {
	return _line;
      }

      std::streamoff col() const noexcept
      {
	return _col;
      }

      friend std::ostream& operator<<(std::ostream &o, const file_loc &loc);

    private:
      std::streamoff _raw_off;
      std::streamoff _line;
      std::streamoff _col;
    };

    std::ostream& operator<<(std::ostream &o, const file_loc &loc);
  }
}

#endif
