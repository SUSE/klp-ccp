#include <cassert>
#include <cerrno>
#include <system_error>
#include "pp_tokenizer.hh"
#include "pp_except.hh"

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
      _remarks.add(code_remark(code_remark::severity::warning,
			       "no newline after continuation",
			       file_range(_filename, _cur_loc)));
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
      return pp_token(tok_type, value,
		      file_range(_filename, _tok_loc, _cur_loc));
    }
  }

  code_remark remark(code_remark::severity::fatal,
		     "encountered EOF while searching for end of string",
		     file_range(_filename, _cur_loc));
  _remarks.add(remark);
  throw(remark);
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
		  file_range(_filename, _tok_loc, _cur_loc));
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
		  file_range(_filename, _tok_loc, _cur_loc));
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
		  file_range(_filename, _tok_loc, _cur_loc));
}

std::size_t pp_tokenizer::_skip_c_comment(const std::size_t n_trailing_spaces)
{
  assert(_cur == '/' && _next == '*');
  std::size_t n_chars_in_last_line = n_trailing_spaces + 2;
  _skip_next_char();

  while (_cur) {
    if (_cur == '*' && _next == '/') {
      _skip_next_char();
      return n_chars_in_last_line + 2;
    } else {
      if (_cur != '\n')
	++n_chars_in_last_line;
      else
	n_chars_in_last_line = 0;
      _advance_to_next_char();
    }
  }

  code_remark remark(code_remark::severity::fatal,
		     "EOF within C-style comment",
		     file_range(_filename, _cur_loc));
  _remarks.add(remark);
  throw pp_except(remark);
}

void pp_tokenizer::_skip_cpp_comment()
{
  assert(_cur == '/' && _next == '/');
  _skip_next_char();
  while(_cur != '\n')
      _advance_to_next_char();

  if (!_cur) {
    _remarks.add(code_remark(code_remark::severity::warning,
			     "EOF within C++-style comment",
			     file_range(_filename, _cur_loc)));
  }
}

pp_token pp_tokenizer::_tokenize_ws()
{
  // According to the standard (5.1.1.2(3)):
  // - Each comment is replaced replaced by a single space.
  // - Newlines are retained.
  // - Implementations may replace whitespace sequences other than
  //   newlines with a single space.
  //
  // GNU cpp does the following:
  // At the beginning of lines:
  //   - Comments are transformed into a sequence of space characters
  //     equal in number to the number of comment's characters after its
  //     last newline.
  //   - Any whitespace character is transformed into a space.
  // At the end of lines:
  //   - Any space, including any arising from comments, is stripped.
  // In the middle of lines:
  //   - Comments are replaced with a single space
  //   - Sequences of whitespace are replaced with a single space.
  //
  // In addition, GNU CPP collapses multiple newlines into a single one.
  // We don't do this here.
  std::size_t n_spaces = 0;
  bool done = false;

  assert(_cur == ' ' || _cur == '\t' || _cur == '\v' || _cur == '\f' ||
	 _cur == '/' && (_next == '*' || _next == '/'));


  while (!done) {
    switch (_cur) {
    case '\n':
      // Whitespace at end of line, return a single newline.
      _advance_to_next_char();
      _expect_qh_str = expect_qh_str::newline;
      return pp_token(pp_token::type::newline, "\n",
		      file_range(_filename, _tok_loc, _cur_loc));

    case '/':
      if (_next == '*') {
	n_spaces = _skip_c_comment(n_spaces);
      } else if (_next == '/') {
	// Whitespace + comment at end of line, return a single newline.
	_skip_cpp_comment();
	assert(!_cur || _cur == '\n');
	_advance_to_next_char();
	_expect_qh_str = expect_qh_str::newline;
	return pp_token(pp_token::type::newline, "\n",
		      file_range(_filename, _tok_loc, _cur_loc));
      } else {
	done = true;
      }
      break;

    case ' ':
    case '\t':
    case '\v':
    case '\f':
      ++n_spaces;
      _advance_to_next_char();
      break;

    default:
      done = true;
    }
  }

  return pp_token(pp_token::type::ws,
	std::string(_expect_qh_str == expect_qh_str::newline ? n_spaces : 1,
		    ' '),
	file_range(_filename, _tok_loc, _cur_loc));
}

pp_token pp_tokenizer::read_next_token()
{
  _tok_loc = _cur_loc;
  switch (_cur) {
  case 0:
    return pp_token(pp_token::type::eof, "",
		    file_range(_filename, _cur_loc, _cur_loc));

  case '\n':
    {
      const file_loc cur_loc = _cur_loc;
      const char cur = _cur;
      _expect_qh_str = expect_qh_str::newline;
      _advance_to_next_char();
      return pp_token(pp_token::type::newline,
		      std::string(1, cur),
		      file_range(_filename, _tok_loc, cur_loc + cur));
    }

  case ' ':
  case '\t':
  case '\v':
  case '\f':
    return _tokenize_ws();

  case '/':
    if (_next == '*') {
      return _tokenize_ws();
    } else if (_next == '/') {
      return _tokenize_ws();
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
  case ',':
    _expect_qh_str = expect_qh_str::no;
    return _tokenize_punctuator();

  default:
    {
      const file_loc cur_loc = _cur_loc;
      const char cur = _cur;
      _expect_qh_str = expect_qh_str::no;
      _advance_to_next_char();
      return pp_token(pp_token::type::non_ws_char,
		      std::string(1, cur),
		      file_range(_filename, _tok_loc, cur_loc + cur));
    }
  }
  // unreachable
}
