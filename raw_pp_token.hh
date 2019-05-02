#ifndef RAW_PP_TOKEN_HH
#define RAW_PP_TOKEN_HH

#include <string>
#include "pp_token.hh"
#include "range_in_file.hh"

namespace klp
{
  namespace ccp
  {
    class raw_pp_token
    {
    public:
      raw_pp_token(const pp_token::type type, const std::string &value,
		   const range_in_file &range_in_file);

      bool operator==(const raw_pp_token &rhs) const noexcept;

      pp_token::type get_type() const noexcept
      {
	return _type;
      }

      const std::string& get_value() const noexcept
      {
	return _value;
      }

      const range_in_file& get_range_in_file() const noexcept
      {
	return _range_in_file;
      }

      template <typename T>
      bool is_punctuator(T &&s) const noexcept
      {
	return (_type == pp_token::type::punctuator &&
		_value == std::forward<T>(s));
      }

      template <pp_token::type... types>
      bool is_type_any_of() const noexcept;

      bool is_id() const noexcept
      {
	return is_type_any_of<pp_token::type::id>();
      }

      bool is_ws() const noexcept
      {
	return is_type_any_of<pp_token::type::ws>();
      }

      bool is_newline() const noexcept
      {
	return is_type_any_of<pp_token::type::newline>();
      }

      bool is_eof() const noexcept
      {
	return is_type_any_of<pp_token::type::eof>();
      }

    private:
      template <pp_token::type... types>
      struct __is_type_any_of
      {
	// This is the sizeof...(types) == 0 case,
	// c.f. the specialization below.
	bool operator()(const raw_pp_token &) const noexcept
	{ return false; }
      };

      template <pp_token::type type0, pp_token::type... types>
      struct __is_type_any_of<type0, types...>
      {
	bool operator()(const raw_pp_token &tok) const noexcept
	{
	  return tok._type == type0 || __is_type_any_of<types...>()(tok);
	}
      };


      std::string _value;
      range_in_file _range_in_file;
      pp_token::type _type;
    };

    template <pp_token::type... types>
    bool raw_pp_token::is_type_any_of() const noexcept
    {
      return __is_type_any_of<types...>()(*this);
    }
  }
}

#endif
