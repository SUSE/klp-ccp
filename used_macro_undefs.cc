#include "used_macro_undefs.hh"

using namespace klp::ccp;

used_macro_undefs::used_macro_undefs(_used_macro_undefs_type &&um)
  : _used_macro_undefs(std::move(um))
{}

used_macro_undefs& used_macro_undefs::operator+=(const used_macro_undefs &rhs)
{
  _used_macro_undefs.insert(rhs._used_macro_undefs.cbegin(),
			    rhs._used_macro_undefs.cend());
  return *this;
}

used_macro_undefs& used_macro_undefs::operator+=(const macro_undef &rhs)
{
  _used_macro_undefs.insert(std::ref(rhs));
  return *this;
}

bool used_macro_undefs::_compare::
operator()(const std::reference_wrapper<const macro_undef> a,
	   const std::reference_wrapper<const macro_undef> b) const noexcept
{
  return &a.get() < &b.get();
}
