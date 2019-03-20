#include "semantic_except.hh"
#include <sstream>

using namespace klp::ccp;

semantic_except::semantic_except(const code_remark_pp &remark)
{
  std::stringstream ss;

  ss << remark;
  _what = ss.str();
}

semantic_except::~semantic_except() noexcept = default;

const char* semantic_except::what() const noexcept
{
  return _what.c_str();
}
