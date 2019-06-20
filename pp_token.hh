#ifndef PP_TOKEN_HH
#define PP_TOKEN_HH

#include <string>
#include <memory>
#include "raw_pp_tokens_range.hh"
#include "pp_result.hh"

namespace klp
{
  namespace ccp
  {
    class code_remarks;

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
	uchr16,
	uchr32,
	str,
	wstr,
	ustr8,
	ustr16,
	ustr32,
	qstr,
	hstr,
	punctuator,
	non_ws_char,
	eof,

	empty,
      };

      pp_token(const type type, const std::string &value,
	       const raw_pp_tokens_range &token_source);

      type get_type() const noexcept
      {
	return _type;
      }

      const std::string& get_value() const noexcept
      {
	return _value;
      }

      void set_type_and_value(const type type, const std::string &value);

      const raw_pp_tokens_range& get_token_source() const noexcept
      { return _token_source; }

      operator bool() const noexcept
      {
	return _type != type::eof;
      }

      template <typename T>
      bool is_punctuator(T &&s) const noexcept
      {
	return (_type == pp_token::type::punctuator &&
		_value == std::forward<T>(s));
      }

      template <type... types>
      bool is_type_any_of() const noexcept;

      bool is_id() const noexcept
      {
	return is_type_any_of<type::id>();
      }

      bool is_ws() const noexcept
      {
	return is_type_any_of<type::ws>();
      }

      bool is_newline() const noexcept
      {
	return is_type_any_of<type::newline>();
      }

      bool is_eof() const noexcept
      {
	return is_type_any_of<type::eof>();
      }

      static std::string stringify(const type type, const std::string &value);

      std::string stringify() const;

    private:
      template <type... types>
      struct __is_type_any_of
      {
	// This is the sizeof...(types) == 0 case,
	// c.f. the specialization below.
	bool operator()(const pp_token &) const noexcept
	{ return false; }
      };

      template <type type0, type... types>
      struct __is_type_any_of<type0, types...>
      {
	bool operator()(const pp_token &tok) const noexcept
	{
	  return tok._type == type0 || __is_type_any_of<types...>()(tok);
	}
      };

      std::string _value;
      raw_pp_tokens_range _token_source;
      type _type;
    };

    template <pp_token::type... types>
    bool pp_token::is_type_any_of() const noexcept
    {
      return __is_type_any_of<types...>()(*this);
    }
  }
}

#endif
