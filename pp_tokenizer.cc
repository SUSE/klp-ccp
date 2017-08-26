#include <cassert>
#include <cerrno>
#include <system_error>

#include "pp_tokenizer.hh"
using namespace suse::cp;

pp_tokenizer::pp_tokenizer(const std::string &filename)
  : _filename(filename), _i(filename),
    _expect_qh_str(expect_qh_str::newline)
{
  if (!_i)
    throw std::system_error(errno, std::system_category(),
			    filename);

  _i.exceptions(std::ifstream::badbit);
  _cur = _read_next_char(_cur_loc, 0);
  _next_loc = _cur_loc;
  _next = _read_next_char(_next_loc, _cur);
}

char pp_tokenizer::_read_next_char(file_loc &loc, const char prev)
{
  char c = static_cast<char>(_i.get());
  loc += prev;
  if (_i.eof())
    return 0;

  // skip continuation lines
  while (c == '\\') {
    const char next = static_cast<char>(_i.get());
    if (_i.eof()) {
      _remarks.add(code_remark(code_remark::severity::fatal,
			       "no newline after continuation",
			       _filename, _cur_loc));
      return 0;
    }
    if (next != '\n') {
      _i.unget();
      break;
    } else {
      loc += file_loc::continuation;
      c = static_cast<char>(_i.get());
      if (_i.eof())
	c = 0;
    }
  }
  return c;
}

void pp_tokenizer::_advance_to_next_char()
{
  _cur_loc = _next_loc;
  _cur = _next;
  _next = _read_next_char(_next_loc, _next);
}

void pp_tokenizer::_skip_next_char()
{
  _advance_to_next_char();
  _advance_to_next_char();
}

pp_token pp_tokenizer::_tokenize_c_comment()
{
  while (_cur) {
    if (!(_cur == '*' && _next == '/')) {
      _advance_to_next_char();
    } else {
      _skip_next_char();
      return pp_token(pp_token::type::ws, " ",
		      _filename, _tok_loc, _cur_loc);
    }
  }
  _remarks.add(code_remark(code_remark::severity::fatal,
			   "EOF within C-style comment",
			   _filename, _cur_loc));
  return pp_token(pp_token::type::ws, " ",
		  _filename, _tok_loc, _cur_loc);
}

pp_token pp_tokenizer::_tokenize_cpp_comment()
{
  while(_cur) {
    if (_cur != '\n') {
      _advance_to_next_char();
    } else {
      return pp_token(pp_token::type::ws, " ",
		      _filename, _tok_loc, _cur_loc);
    }
  }
  _remarks.add(code_remark(code_remark::severity::warning,
			   "EOF within C++-style comment",
			   _filename, _cur_loc));
  return pp_token(pp_token::type::ws, " ",
		  _filename, _tok_loc, _cur_loc);
}

pp_token pp_tokenizer::_tokenize_string(const char delim,
					const bool delim_escapable,
					const pp_token::type tok_type)
{
  std::string value;
  bool in_escape = false;

  while(_cur) {
    if (_cur != delim || (delim_escapable && in_escape)) {
      if (in_escape) {
	in_escape = false;
      } else {
	in_escape = (_cur == '\\');
      }
      value.push_back(_cur);
      _advance_to_next_char();
    } else {
      _advance_to_next_char();
      return pp_token(tok_type, value, _filename, _tok_loc, _cur_loc);
    }
  }
  _remarks.add(code_remark(code_remark::severity::fatal,
			   "encountered EOF while searching for end of string",
			   _filename, _cur_loc));
  return pp_token(tok_type, value, _filename, _tok_loc, _cur_loc);
}

pp_token pp_tokenizer::_tokenize_punctuator()
{
  std::string value;

  if (_cur == '-' && _next == '>') {
    value = "->";
    _skip_next_char();
  } else if (_cur == '+' && _next == '+') {
    value = "++";
    _skip_next_char();
  } else if (_cur == '-' && _next == '-') {
    value = "--";
    _skip_next_char();
  } else if (_cur == '<' && _next == '<') {
    _skip_next_char();
    if (_cur != '=') {
      value = "<<";
    } else {
      value = "<<=";
      _advance_to_next_char();
    }
  } else if (_cur == '>' && _next == '>') {
    _skip_next_char();
    if (_cur != '=') {
      value = ">>";
    } else {
      value = ">>=";
      _advance_to_next_char();
    }
  } else if (_cur == '<' && _next == '=') {
    value = "<=";
    _skip_next_char();
  } else if (_cur == '>' && _next == '=') {
    value = ">=";
    _skip_next_char();
  } else if (_cur == '=' && _next == '=') {
    value = "==";
    _skip_next_char();
  } else if (_cur == '!' && _next == '=') {
    value = "!=";
    _skip_next_char();
  } else if (_cur == '*' && _next == '=') {
    value = "*=";
    _skip_next_char();
  } else if (_cur == '/' && _next == '=') {
    value = "/=";
    _skip_next_char();
  } else if (_cur == '%' && _next == '=') {
    value = "%=";
    _skip_next_char();
  } else if (_cur == '+' && _next == '=') {
    value = "+=";
    _skip_next_char();
  } else if (_cur == '-' && _next == '=') {
    value = "-=";
    _skip_next_char();
  } else if (_cur == '&' && _next == '=') {
    value = "&=";
    _skip_next_char();
  } else if (_cur == '^' && _next == '=') {
    value = "^=";
    _skip_next_char();
  } else if (_cur == '|' && _next == '=') {
    value = "|=";
    _skip_next_char();
  } else if (_cur == '&' && _next == '&') {
    value = "&&";
    _skip_next_char();
  } else if (_cur == '|' && _next == '|') {
    value = "||";
    _skip_next_char();
  } else if (_cur == '.' && _next == '.') {
    _advance_to_next_char();
    if (_next == '.') {
      value = "...";
      _skip_next_char();
    } else {
      value = ".";
    }
  } else if (_cur == '#' && _next == '#') {
    value = "##";
    _skip_next_char();
  } else if (_cur == '<' && _next == ':') {
    value = "<:";
    _skip_next_char();
  } else if (_cur == ':' && _next == '>') {
    value = ":>";
    _skip_next_char();
  } else if (_cur == '<' && _next == '%') {
    value = "<%";
    _skip_next_char();
  } else if (_cur == '%' && _next == '>') {
    value = "%>";
    _skip_next_char();
  } else if (_cur == '%' && _next == ':') {
    _skip_next_char();
    if (_cur == '%' && _next == ':') {
      _skip_next_char();
      value = "%:%:";
    } else {
      value = "%:";
    }
  } else {
    value = _cur;
    _advance_to_next_char();
  }
  return pp_token(pp_token::type::punctuator, value,
		  _filename, _tok_loc, _cur_loc);
}

pp_token pp_tokenizer::_tokenize_pp_number()
{
  std::string value;
  bool done = false;

  assert(_cur == '.' || ('0' <= _cur && _cur <= '9'));

  while(!done) {
    switch (_cur) {
    case 'e':
    case 'E':
    case 'p':
    case 'P':
      if (_next == '+' || _next == '-') {
	value.push_back(_cur);
	value.push_back(_next);
	_skip_next_char();
      } else {
	_advance_to_next_char();
      }
      break;

    case '.':
    case '0' ... '9':
    case '_':
    case 'a' ... 'd':
    case 'f' ... 'o':
    case 'q' ... 'z':
    case 'A' ... 'D':
    case 'F' ... 'O':
    case 'Q' ... 'Z':
      value.push_back(_cur);
      _advance_to_next_char();
      break;

    default:
      done = true;
    }
  }

  return pp_token(pp_token::type::pp_number, value,
		  _filename, _tok_loc, _cur_loc);
}

pp_token pp_tokenizer::_tokenize_id()
{
  std::string value;
  bool done = false;

  assert(_cur == '_' || ('a' <= _cur && _cur <= 'z') ||
	 ('A' <= _cur && _cur <= 'Z'));

  while (!done) {
    switch (_cur) {
    case '_':
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '0' ... '9':
      value.push_back(_cur);
      _advance_to_next_char();
      break;

    default:
      done = true;
    }
  }

  return pp_token(pp_token::type::id, value,
		  _filename, _tok_loc, _cur_loc);
}

pp_token pp_tokenizer::_tokenize_ws()
{
  std::string value;
  bool done = false;

  assert(_cur == ' ' || _cur == '\t');

  while (!done) {
    switch (_cur) {
    case ' ':
    case '\t':
      value.push_back(_cur);
      _advance_to_next_char();
      break;

    default:
      done = true;
    }
  }

  return pp_token(pp_token::type::ws, value,
		  _filename, _tok_loc, _cur_loc);
}

pp_token pp_tokenizer::read_next_token()
{
  _tok_loc = _cur_loc;
  switch (_cur) {
  case 0:
    return pp_token(pp_token::type::eof, "",
		    _filename, _cur_loc, _cur_loc);

  case '\n':
    {
      const file_loc cur_loc = _cur_loc;
      const char cur = _cur;
      _expect_qh_str = expect_qh_str::newline;
      _advance_to_next_char();
      return pp_token(pp_token::type::newline, std::string(1, cur), _filename,
		      _tok_loc, cur_loc + cur);
    }

  case ' ':
  case '\t':
    return _tokenize_ws();

  case '/':
    if (_next == '*') {
      _skip_next_char();
      return _tokenize_c_comment();
    } else if (_next == '/') {
      _skip_next_char();
      return _tokenize_cpp_comment();
    } else {
      _expect_qh_str = expect_qh_str::no;
      return _tokenize_punctuator();
    }

  case '\'':
    _expect_qh_str = expect_qh_str::no;
    _advance_to_next_char();
    return _tokenize_string('\'', true, pp_token::type::chr);

  case '\"':
    _advance_to_next_char();
    if (_expect_qh_str != expect_qh_str::expect_qh_str) {
      _expect_qh_str = expect_qh_str::no;
      return _tokenize_string('"', true, pp_token::type::str);
    } else {
      _expect_qh_str = expect_qh_str::no;
      return _tokenize_string('"', false, pp_token::type::qstr);
    }

  case 'L':
    _expect_qh_str = expect_qh_str::no;
    if (_next == '"') {
      _skip_next_char();
      return _tokenize_string('"', true, pp_token::type::wstr);
    } else if (_next == '\'') {
      _skip_next_char();
      return _tokenize_string('\'', true, pp_token::type::wchr);
    } else {
      return _tokenize_id();
    }

  case '_':
  case 'a' ... 'z':
  case 'A' ... 'K':
  case 'M' ... 'Z':
    {
      pp_token tok = _tokenize_id();
      if (_expect_qh_str == expect_qh_str::sharp_seen &&
	  tok.get_value() == "include") {
	_expect_qh_str = expect_qh_str::expect_qh_str;
      } else {
	_expect_qh_str = expect_qh_str::no;
      }
      return tok;
    }

  case '0' ... '9':
    _expect_qh_str = expect_qh_str::no;
    return _tokenize_pp_number();

  case '.':
    _expect_qh_str = expect_qh_str::no;
    if ('0' <= _next && _next <= '9') {
      return _tokenize_pp_number();
    } else {
      return _tokenize_punctuator();
    }

  case '#':
    if (_expect_qh_str == expect_qh_str::newline && _next != '#') {
      _expect_qh_str = expect_qh_str::sharp_seen;
    } else {
      _expect_qh_str = expect_qh_str::no;
    }
    return _tokenize_punctuator();

  case '<':
    if (_expect_qh_str != expect_qh_str::expect_qh_str) {
      _expect_qh_str = expect_qh_str::no;
      return _tokenize_punctuator();
    } else {
      _expect_qh_str = expect_qh_str::no;
      _advance_to_next_char();
      return _tokenize_string('>', false, pp_token::type::hstr);
    }

  case '[':
  case ']':
  case '(':
  case ')':
  case '{':
  case '}':
  case '-':
  case '+':
  case '&':
  case '*':
  case '~':
  case '!':
  case '%':
  case '>':
  case '=':
  case '^':
  case '|':
  case '?':
  case ':':
  case ';':
    _expect_qh_str = expect_qh_str::no;
    return _tokenize_punctuator();

  default:
    {
      const file_loc cur_loc = _cur_loc;
      const char cur = _cur;
      _expect_qh_str = expect_qh_str::no;
      _advance_to_next_char();
      return pp_token(pp_token::type::non_ws_char, std::string(1, cur),
		      _filename, _tok_loc, cur_loc + cur);
    }
  }
  // unreachable
}
