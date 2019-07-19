#include "cmdline_except.hh"

using namespace klp::ccp;

cmdline_except::cmdline_except(std::string &&what)
  : _what(std::move(what))
{}

cmdline_except::~cmdline_except() noexcept = default;

const char* cmdline_except::what() const noexcept
{
  return _what.c_str();
}
