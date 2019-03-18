#include <cassert>
#include "pp_tokens_range.hh"

using namespace klp::ccp;

#ifdef DEBUG_PARSER
std::ostream&
klp::ccp::ast::operator<<(std::ostream &o, const pp_tokens_range &r)
{
  o << r.begin << '-' << r.end;
  return o;
}
#endif
