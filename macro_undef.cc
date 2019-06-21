#include "macro_undef.hh"

using namespace klp::ccp;

macro_undef::macro_undef(const std::string &name,
			 const raw_pp_tokens_range &directive_range)
  : _name(name), _directive_range(directive_range)
{}
