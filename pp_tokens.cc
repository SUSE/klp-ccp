#include <algorithm>
#include <cassert>
#include "pp_tokens.hh"
#include "pp_token.hh"

using namespace klp::ccp;

static std::string __escape_string(std::string &&s)
{
  for (auto &&it = s.begin(); it != s.end();) {
    if (*it == '"' || *it == '\\') {
      it = s.insert(it, '\\');
      it += 2;
    } else {
      ++it;
    }
  }

  return std::move(s);
}

std::string pp_tokens::stringify(const bool as_string) const
{
  std::string s;
  for (auto &&it = _tokens.begin(); it != _tokens.end(); ++it) {
    s += it->stringify();
  }

  if (as_string)
    s = __escape_string(std::move(s));

  return s;
}
