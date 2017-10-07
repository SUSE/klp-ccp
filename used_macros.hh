#ifndef USED_MACROS_HH
#define USED_MACROS_HH

#include <memory>
#include "cow_set.hh"

namespace suse
{
  namespace cp
  {
    class macro;

    class used_macros
    {
    private:
      typedef cow_set<std::shared_ptr<const macro> > _used_macros_type;

      used_macros(_used_macros_type &&um);

    public:
      typedef _used_macros_type::const_iterator const_iterator;
      typedef _used_macros_type::iterator iterator;
      typedef _used_macros_type::size_type size_type;
      typedef _used_macros_type::value_type value_type;

      used_macros() = default;

      bool empty() const noexcept
      { return _used_macros.empty(); }

      const_iterator cbegin() const noexcept
      { return _used_macros.cbegin(); }

      const_iterator cend() const noexcept
      { return _used_macros.cend(); }

      template<typename... Targs>
      size_type count(Targs&&... args)
      {
	return _used_macros.count(std::forward<Targs>(args)...);
      }

      used_macros& operator+=(const used_macros &rhs);
      used_macros operator+(const used_macros &rhs) const;

      used_macros& operator+=(const value_type &rhs);
      used_macros operator+(const value_type &rhs) const;

    private:
      _used_macros_type _used_macros;
    };
  }
}

#endif
