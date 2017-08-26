#ifndef PP_TOKEN_HH
#define PP_TOKEN_HH

#include <string>
#include "file_loc.hh"

namespace suse
{
  namespace cp
  {
    class pp_token
    {
    public:
      enum class type
      {
	ws,
	newline,
	id,
	pp_number,
	chr,
	wchr,
	str,
	wstr,
	qstr,
	hstr,
	punctuator,
	non_ws_char,
	eof,
      };

      pp_token() = default;

      pp_token(type type, const std::string &value,
	       const std::string &filename,
	       const file_loc &start_loc, const file_loc &end_loc)
	: _value(value), _filename(filename),
	  _start_loc(start_loc), _end_loc(end_loc), _type(type)
      {}


      type get_type() const noexcept
      {
	return _type;
      }

      const std::string& get_value() const noexcept
      {
	return _value;
      }

      operator bool() const noexcept
      {
	return _type != type::eof;
      }

    private:
      std::string _value;
      std::string _filename;
      file_loc _start_loc;
      file_loc _end_loc;
      type _type;
    };
  }
}

#endif
