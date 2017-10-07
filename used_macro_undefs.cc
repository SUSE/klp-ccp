#include "used_macro_undefs.hh"
#include <iterator>
#include <algorithm>

using namespace suse::cp;

used_macro_undefs::used_macro_undefs(_used_macro_undefs_type &&um)
  : _used_macro_undefs(std::move(um))
{}

used_macro_undefs& used_macro_undefs::operator+=(const used_macro_undefs &rhs)
{
  _used_macro_undefs.insert(rhs._used_macro_undefs.begin(),
		      rhs._used_macro_undefs.end());
  return *this;
}

used_macro_undefs used_macro_undefs::operator+(const used_macro_undefs &rhs)
  const
{
  _used_macro_undefs_type result;

  std::set_union(this->begin(), this->end(),
		 rhs.begin(), rhs.end(),
		 std::inserter(result, result.end()));
  return used_macro_undefs(std::move(result));
}

used_macro_undefs& used_macro_undefs::operator+=(const value_type &rhs)
{
  _used_macro_undefs.insert(rhs);
  return *this;
}

used_macro_undefs used_macro_undefs::operator+(const value_type &rhs) const
{
  used_macro_undefs result(*this);
  result += rhs;
  return result;
}
