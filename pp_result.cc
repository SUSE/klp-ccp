#include <cassert>
#include "pp_result.hh"

using namespace klp::ccp;

pp_result::pp_result()
  : _pp_tokens(*this)
{}

void pp_result::_append_token(const raw_pp_token &tok)
{
  _raw_tokens.push_back(tok);
}

void pp_result::_append_token(raw_pp_token &&tok)
{
  _raw_tokens.push_back(std::move(tok));
}

void pp_result::_append_token(pp_token &&tok)
{
  _pp_tokens.push_back(std::move(tok));
}

raw_pp_tokens::size_type pp_result::_get_last_raw_index() const noexcept
{
  assert(!_raw_tokens.empty());
  return _raw_tokens.size() - 1;
}

pp_tokens::size_type pp_result::_get_last_pp_index() const noexcept
{
  assert(!_pp_tokens.empty());
  return _pp_tokens.size() - 1;
}
