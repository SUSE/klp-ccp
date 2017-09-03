#ifndef PP_TOKEN_HH
#define PP_TOKEN_HH

#include <string>
#include <memory>
#include "file_range.hh"
#include "used_macros.hh"

namespace suse
{
  namespace cp
  {
    class code_remarks;
    class macro;

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

	empty,
      };

      pp_token(const type type, const std::string &value,
	       const file_range &file_range);

      pp_token(const type type, const std::string &value,
	       const file_range &file_range, used_macros &&um);

      bool operator==(const pp_token &rhs) const noexcept;

      type get_type() const noexcept
      {
	return _type;
      }

      const std::string& get_value() const noexcept
      {
	return _value;
      }

      void set_type_and_value(const type type, const std::string &value);

      const class used_macros& used_macros() const noexcept
      {
	return _used_macros;
      }

      class used_macros& used_macros() noexcept
      {
	return _used_macros;
      }

      const file_range& get_file_range() const noexcept
      {
	return _file_range;
      }

      operator bool() const noexcept
      {
	return _type != type::eof;
      }

      template <typename T>
      bool is_punctuator(T &&s) const noexcept
      {
	return (_type == type::punctuator &&
		_value == std::forward<T>(s));
      }

      template <type... types>
      bool is_type_any_of() const noexcept;

      template <type... types>
      static bool is_type_any_of_predicate(const pp_token &tok) noexcept;

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

      bool is_empty() const noexcept
      {
	return is_type_any_of<type::empty>();
      }

      static bool is_ws_predicate(const pp_token &tok)
      {
	return is_type_any_of_predicate<type::ws>(tok);
      }

      std::string stringify() const;

      void
      concat(const pp_token &tok, code_remarks &remarks);

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
      file_range _file_range;
      class used_macros _used_macros;
      type _type;
    };

    template <pp_token::type... types>
    bool pp_token::is_type_any_of() const noexcept
    {
      return __is_type_any_of<types...>()(*this);
    }

    template <pp_token::type... types>
    bool pp_token::is_type_any_of_predicate(const pp_token &tok) noexcept
    {
      return tok.is_type_any_of<types...>();
    }
  }
}

#endif
