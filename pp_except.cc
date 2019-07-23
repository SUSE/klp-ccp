#include "pp_except.hh"
#include <sstream>

using namespace klp::ccp;

pp_except::pp_except(const code_remark &remark)
{
  std::stringstream ss;

  ss << remark;
  _what = ss.str();
}

pp_except::pp_except(const std::string &msg)
  : _what(msg)
{}

pp_except::~pp_except() noexcept = default;

const char* pp_except::what() const noexcept
{
  return _what.c_str();
}
