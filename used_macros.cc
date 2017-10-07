#include "used_macros.hh"
#include <iterator>
#include <algorithm>

using namespace suse::cp;

used_macros::used_macros(_used_macros_type &&um)
  : _used_macros(std::move(um))
{}

used_macros& used_macros::operator+=(const used_macros &rhs)
{
  _used_macros.insert(rhs._used_macros.cbegin(),
		      rhs._used_macros.cend());
  return *this;
}

used_macros used_macros::operator+(const used_macros &rhs) const
{
  _used_macros_type result;

  std::set_union(this->cbegin(), this->cend(),
		 rhs.cbegin(), rhs.cend(),
		 std::inserter(result, result.cend()));
  return used_macros(std::move(result));
}

used_macros& used_macros::operator+=(const value_type &rhs)
{
  _used_macros.insert(rhs);
  return *this;
}

used_macros used_macros::operator+(const value_type &rhs) const
{
  used_macros result(*this);
  result += rhs;
  return result;
}
