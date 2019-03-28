#include "used_macros.hh"

using namespace klp::ccp;

used_macros::used_macros(_used_macros_type &&um)
  : _used_macros(std::move(um))
{}

void used_macros::clear() noexcept
{
  _used_macros.clear();
}

std::size_t used_macros::count(const macro &m) const noexcept
{
  return _used_macros.count(std::ref(m));
}

used_macros& used_macros::operator+=(const used_macros &rhs)
{
  _used_macros.insert(rhs._used_macros.cbegin(),
		      rhs._used_macros.cend());
  return *this;
}

used_macros& used_macros::operator+=(const macro &rhs)
{
  _used_macros.insert(std::ref(rhs));
  return *this;
}

bool used_macros::_compare::
operator()(const std::reference_wrapper<const macro> a,
	   const std::reference_wrapper<const macro> b) const noexcept
{
  return &a.get() < &b.get();
}
