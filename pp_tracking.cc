#include <cassert>
#include "pp_tracking.hh"

using namespace klp::ccp;

pp_tracking::pp_tracking()
  : _pp_tokens(*this)
{}

void pp_tracking::_append_token(const raw_pp_token &tok)
{
  _raw_tokens.push_back(tok);
}

void pp_tracking::_append_token(raw_pp_token &&tok)
{
  _raw_tokens.push_back(std::move(tok));
}

void pp_tracking::_append_token(pp_token &&tok)
{
  _pp_tokens.push_back(std::move(tok));
}

raw_pp_tokens::size_type pp_tracking::_get_last_raw_index() const noexcept
{
  assert(!_raw_tokens.empty());
  return _raw_tokens.size() - 1;
}

pp_tokens::size_type pp_tracking::_get_last_pp_index() const noexcept
{
  assert(!_pp_tokens.empty());
  return _pp_tokens.size() - 1;
}
