#include <ostream>
#include "code_remarks.hh"

using namespace klp::ccp;

void code_remarks::add(const code_remark_pp &r)
{
  _remarks_pp.push_back(r);
}

void code_remarks::add(code_remark_pp &&r)
{
  _remarks_pp.push_back(std::move(r));
}

void code_remarks::add(const code_remark_raw &r)
{
  _remarks_raw.push_back(r);
}

void code_remarks::add(code_remark_raw &&r)
{
  _remarks_raw.push_back(std::move(r));
}

void code_remarks::clear() noexcept
{
  _remarks_raw.clear();
  _remarks_pp.clear();
}

bool code_remarks::empty() const noexcept
{
  return _remarks_raw.empty() && _remarks_pp.empty();
}

bool code_remarks::any_fatal() const noexcept
{
  for(auto &&r : _remarks_raw) {
    if (r.get_severity() == code_remark_raw::severity::fatal)
      return true;
  }

  for(auto &&r : _remarks_pp) {
    if (r.get_severity() == code_remark_pp::severity::fatal)
      return true;
  }

  return false;
}

code_remarks& code_remarks::operator+=(const code_remarks &remarks)
{
  _remarks_raw.insert(_remarks_raw.end(),
		      remarks._remarks_raw.begin(), remarks._remarks_raw.end());
  _remarks_pp.insert(_remarks_pp.end(),
		     remarks._remarks_pp.begin(), remarks._remarks_pp.end());
  return *this;
}

std::ostream& klp::ccp::operator<<(std::ostream &o, const code_remarks &rs)
{
  for(auto &&r : rs._remarks_raw) {
    o << r;
  }
  for(auto &&r : rs._remarks_pp) {
    o << r;
  }
  return o;
}
