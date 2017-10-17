#include "parse_except.hh"
#include <sstream>

using namespace suse::cp;

parse_except::parse_except(const code_remark &remark)
{
  std::stringstream ss;

  ss << remark;
  _what = ss.str();
}

parse_except::~parse_except() noexcept = default;

const char* parse_except::what() const noexcept
{
  return _what.c_str();
}
