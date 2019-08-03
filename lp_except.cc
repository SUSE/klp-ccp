#include "lp_except.hh"
#include <sstream>

using namespace klp::ccp;

lp_except::lp_except(const code_remark &remark)
{
  std::stringstream ss;

  ss << remark;
  _what = ss.str();
}

lp_except::~lp_except() noexcept = default;

const char* lp_except::what() const noexcept
{
  return _what.c_str();
}
