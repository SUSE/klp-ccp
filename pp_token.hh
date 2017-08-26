#ifndef PP_TOKEN_HH
#define PP_TOKEN_HH

#include <string>
#include "file_range.hh"

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

      pp_token(const type type, const std::string &value,
	       const file_range &file_range)
	: _value(value), _file_range(file_range), _type(type)
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
      file_range _file_range;
      type _type;
    };
  }
}

#endif
