#include <ostream>
#include "code_remark.hh"
#include "code_remarks.hh"

using namespace suse::cp;

void code_remarks::add(const code_remark &r)
{
  _remarks.push_back(r);
}

void code_remarks::add(code_remark &&r)
{
  _remarks.push_back(std::move(r));
}

void code_remarks::clear() noexcept
{
  _remarks.clear();
}

bool code_remarks::empty() const noexcept
{
  return _remarks.empty();
}

bool code_remarks::any_fatal() const noexcept
{
  for(auto &&r : _remarks) {
    if (r.get_severity() == code_remark::severity::fatal)
      return true;
  }

  return false;
}

std::ostream& suse::cp::operator<<(std::ostream &o, const code_remarks &rs)
{
  for(auto &&r : rs._remarks) {
    o << r;
  }
  return o;
}
