#include "macro_undef.hh"

using namespace klp::ccp;

macro_undef::macro_undef(std::shared_ptr<const macro> &&macro,
			 const raw_pp_tokens_range &directive_range)
  : _macro(macro), _directive_range(directive_range)
{}
