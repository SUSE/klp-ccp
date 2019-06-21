#ifndef MACRO_NONDEF_CONSTRAINTS_HH
#define MACRO_NONDEF_CONSTRAINTS_HH

#include <set>
#include <string>

namespace klp
{
  namespace ccp
  {
    class macro_nondef_constraint
    {
    public:
      macro_nondef_constraint(const std::string &id,
			      bool func_like_allowed = false);

      bool operator<(const macro_nondef_constraint &rhs) const;

      const std::string& get_id() const noexcept
      { return _id; }

      bool is_func_like_allowed() const noexcept
      { return _func_like_allowed; }


    private:
      std::string _id;
      bool _func_like_allowed;
    };

    class macro_nondef_constraints
    {
    private:
      typedef std::set<macro_nondef_constraint> _macro_nondef_constraints_type;

    public:
      macro_nondef_constraints() = default;

      macro_nondef_constraints(macro_nondef_constraints &&mnc);

      macro_nondef_constraints& operator=(macro_nondef_constraints &&rhs);

      macro_nondef_constraints& operator+=(const macro_nondef_constraints &rhs);

      macro_nondef_constraints& operator+=(const macro_nondef_constraint &rhs);

    private:
      _macro_nondef_constraints_type _macro_nondef_constraints;
    };
  }
}

#endif
