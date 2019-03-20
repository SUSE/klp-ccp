#include "raw_pp_token.hh"

using namespace klp::ccp;

raw_pp_token::raw_pp_token(const pp_token::type type, const std::string &value,
			   const file_range &file_range)
  : _type(type), _value(value), _file_range(file_range)
{}

bool raw_pp_token::operator==(const raw_pp_token &rhs) const noexcept
{
  return _type == rhs._type && _value == rhs._value;
}
