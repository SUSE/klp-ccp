#include "pp_tracking.hh"

using namespace klp::ccp;

pp_tracking::pp_tracking() = default;

void pp_tracking::_append_token(const raw_pp_token &tok)
{
  _raw_tokens.push_back(tok);
}

void pp_tracking::_append_token(raw_pp_token &&tok)
{
  _raw_tokens.push_back(std::move(tok));
}
