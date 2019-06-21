#include "macro_nondef_constraints.hh"

using namespace klp::ccp;

macro_nondef_constraint::macro_nondef_constraint(const std::string &id,
						 bool func_like_allowed)
  : _id(id), _func_like_allowed(func_like_allowed)
{}

macro_nondef_constraints::
macro_nondef_constraints(macro_nondef_constraints &&mnc)
  : _macro_nondef_constraints(std::move(mnc._macro_nondef_constraints))
{}

bool macro_nondef_constraint::operator<(const macro_nondef_constraint &rhs)
  const
{
  return this->_id < rhs._id;
}

macro_nondef_constraints&
macro_nondef_constraints::operator=(macro_nondef_constraints &&rhs)
{
  if (this == &rhs)
    return *this;

  this->_macro_nondef_constraints = std::move(rhs._macro_nondef_constraints);
  return *this;
}

macro_nondef_constraints&
macro_nondef_constraints::operator+=(const macro_nondef_constraints &rhs)
{
  for (auto &mnc : rhs._macro_nondef_constraints)
    *this += mnc;
  return *this;
}

macro_nondef_constraints&
macro_nondef_constraints::operator+=(const macro_nondef_constraint &rhs)
{
  auto it = _macro_nondef_constraints.find(rhs);

  if (it != _macro_nondef_constraints.end()) {
    if (it->is_func_like_allowed() && !rhs.is_func_like_allowed()) {
      it = _macro_nondef_constraints.erase(it);
      _macro_nondef_constraints.insert(it, rhs);
    }
  } else {
      _macro_nondef_constraints.insert(rhs);
  }

  return *this;
}
