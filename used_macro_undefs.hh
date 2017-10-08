#ifndef USED_MACRO_UNDEFS_HH
#define USED_MACRO_UNDEFS_HH

#include <memory>
#include "cow_set.hh"

namespace suse
{
  namespace cp
  {
    class macro_undef;

    class used_macro_undefs
    {
    private:
      typedef cow_set<std::shared_ptr<const macro_undef> >
	_used_macro_undefs_type;

      used_macro_undefs(_used_macro_undefs_type &&um);

    public:
      typedef _used_macro_undefs_type::const_iterator const_iterator;
      typedef _used_macro_undefs_type::iterator iterator;
      typedef _used_macro_undefs_type::size_type size_type;
      typedef _used_macro_undefs_type::value_type value_type;

      used_macro_undefs() = default;

      bool empty() const noexcept
      { return _used_macro_undefs.empty(); }

      const_iterator cbegin() const noexcept
      { return _used_macro_undefs.cbegin(); }

      const_iterator cend() const noexcept
      { return _used_macro_undefs.cend(); }

      template<typename... Targs>
      size_type count(Targs&&... args)
      {
	return _used_macro_undefs.count(std::forward<Targs>(args)...);
      }

      void clear() noexcept
      { _used_macro_undefs.clear(); }

      used_macro_undefs& operator+=(const used_macro_undefs &rhs);
      used_macro_undefs operator+(const used_macro_undefs &rhs) const;

      used_macro_undefs& operator+=(const value_type &rhs);
      used_macro_undefs operator+(const value_type &rhs) const;

    private:
      _used_macro_undefs_type _used_macro_undefs;
    };
  }
}

#endif
