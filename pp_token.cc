#include <algorithm>
#include <cassert>
#include "pp_token.hh"
#include "code_remarks.hh"
#include "pp_except.hh"

using namespace klp::ccp;

pp_token::pp_token(const type type, const std::string &value,
		   const file_range &file_range)
  : _value(value), _file_range(file_range), _type(type)
{}

pp_token::pp_token(const type type, const std::string &value,
		   const file_range &file_range,
		   const class used_macros &eh, class used_macros &&um,
		   const class used_macro_undefs &umu)
  : _value(value), _file_range(file_range),
    _expansion_history(std::move(eh)), _used_macros(std::move(um)),
    _used_macro_undefs(umu), _type(type)
{}

void pp_token::set_type_and_value(const type type, const std::string &value)
{
  _type = type;
  _value = value;
}

std::string pp_token::stringify() const
{
  switch (_type) {
  case type::ws:
    return _value;

  case type::newline:
    return _value;

  case type::id:
    return _value;

  case type::pp_number:
    return _value;

  case type::chr:
    return '\'' + _value + '\'';

  case type::wchr:
    return "L'" + _value + '\'';

  case type::uchr16:
    return "u'" + _value + '\'';

  case type::uchr32:
    return "U'" + _value + '\'';

  case type::str:
  case type::qstr:
    return '"' + _value + '"';

  case type::wstr:
    return "L\"" + _value + '"';

  case type::ustr8:
    return "u\"" + _value + '"';

  case type::ustr16:
    return "u\"" + _value + '"';

  case type::ustr32:
    return "U\"" + _value + '"';

  case type::hstr:
    return '<' + _value + '>';

  case type::punctuator:
    return _value;

  case type::non_ws_char:
    return _value;

  case type::eof:
  case type::empty:
   return "";
  }

  __builtin_unreachable();
}

void
pp_token::concat(const pp_token &tok, code_remarks &remarks)
{
  assert(_used_macros.empty());
  assert(_used_macro_undefs.empty());
  assert(tok._used_macros.empty());
  assert(tok._used_macro_undefs.empty());

  assert(_type != type::ws);
  assert(tok._type != type::ws);
  assert(_type != type::newline);
  assert(tok._type != type::newline);
  assert(_type != type::eof);
  assert(tok._type != type::eof);
  assert(_type != type::qstr);
  assert(tok._type != type::qstr);
  assert(_type != type::hstr);
  assert(tok._type != type::hstr);

  _expansion_history += tok._expansion_history;
  if (_type == type::empty) {
    _type = tok._type;
    _value = tok._value;
    return;
  } else if (tok._type == type::empty) {
    return;
  }

  if (_type == type::id && tok._type == type::pp_number) {
    if (!std::all_of(tok._value.cbegin(), tok._value.cend(),
		     [](const char c) {
		       return (c == '_' || ('a' <= c && c <= 'z') ||
			       ('A' <= c && c <= 'Z') ||
			       ('0' <= c && c <= '9'));
		     })) {
      code_remark_raw remark
	(code_remark_raw::severity::fatal,
	 "can't concatenate " + _value + " and " + tok._value,
	 tok.get_file_range());
      remarks.add(remark);
      throw pp_except(remark);
    }

    _value += tok._value;
    return;
  } else if (_type == type::pp_number && tok._type == type::id) {
    _value += tok._value;
    return;
  } else if (_type == type::pp_number && tok._type == type::punctuator &&
     (tok._value == "-" || tok._value == "+")) {
    _value += tok._value;
    return;
  }

  if (_type != tok._type) {
    code_remark_raw remark(code_remark_raw::severity::fatal,
			   "can't concatenate tokens of different type",
			   tok.get_file_range());
    remarks.add(remark);
    throw pp_except(remark);
  } else if(_type == type::chr || _type == type::wchr) {
    code_remark_raw remark(code_remark_raw::severity::fatal,
			   "can't concatenate character constants",
			   tok.get_file_range());
    remarks.add(remark);
    throw pp_except(remark);
  } else if (_type == type::non_ws_char) {
    code_remark_raw remark(code_remark_raw::severity::fatal,
			   "can't concatenate " + _value + " and " + tok._value,
			   tok.get_file_range());
    remarks.add(remark);
    throw pp_except(remark);
  }

  _value += tok._value;

  if (_type == type::punctuator) {
    if (_value != "->" && _value != "++" && _value != "--" && _value != "<<" &&
	_value != ">>" && _value != "<=" && _value != ">=" && _value != "==" &&
	_value != "!=" && _value != "&&" && _value != "||" && _value != "*=" &&
	_value != "/=" && _value != "%=" && _value != "+=" && _value != "-=" &&
	_value != "<<=" && _value != ">>=" && _value != "&=" &&
	_value != "^=" && _value != "|=" && _value != "##" && _value != "<:" &&
	_value != ":>" && _value != "<%" && _value != "%>" && _value != "%:" &&
	_value != "%:%:") {
      code_remark_raw remark
	(code_remark_raw::severity::fatal,
	 "can't concatenate " + _value + " and " + tok._value,
	 tok.get_file_range());
      remarks.add(remark);
      throw pp_except(remark);
    }
    return;
  }
}
