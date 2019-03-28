#ifndef USED_MACRO_UNDEFS_HH
#define USED_MACRO_UNDEFS_HH

#include <functional>
#include <set>

namespace klp
{
  namespace ccp
  {
    class macro_undef;

    class used_macro_undefs
    {
    private:
      struct _compare
      {
	bool operator()(const std::reference_wrapper<const macro_undef> a,
			const std::reference_wrapper<const macro_undef> b)
	  const noexcept;
      };

      typedef std::set<std::reference_wrapper<const macro_undef>, _compare>
	_used_macro_undefs_type;

    public:
      used_macro_undefs() = default;

      bool empty() const noexcept
      { return _used_macro_undefs.empty(); }

      used_macro_undefs& operator+=(const used_macro_undefs &rhs);

      used_macro_undefs& operator+=(const macro_undef &rhs);

    private:
       used_macro_undefs(_used_macro_undefs_type &&um);

      _used_macro_undefs_type _used_macro_undefs;
    };
  }
}

#endif
