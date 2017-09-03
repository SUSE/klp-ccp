#ifndef USED_MACROS_HH
#define USED_MACROS_HH

#include <set>
#include <memory>

namespace suse
{
  namespace cp
  {
    class macro;

    class used_macros
    {
    private:
      typedef std::set<std::shared_ptr<const macro> > _used_macros_type;

      used_macros(_used_macros_type &&um);

    public:
      typedef _used_macros_type::const_iterator const_iterator;
      typedef _used_macros_type::iterator iterator;
      typedef _used_macros_type::size_type size_type;
      typedef _used_macros_type::value_type value_type;

      used_macros() noexcept = default;

      bool empty() const noexcept
      { return _used_macros.empty(); }

      const_iterator cbegin() const noexcept
      { return _used_macros.cbegin(); }

      const_iterator begin() const noexcept
      { return _used_macros.begin(); }

      iterator begin() noexcept
      { return _used_macros.begin(); }

      const_iterator cend() const noexcept
      { return _used_macros.cend(); }

      const_iterator end() const noexcept
      { return _used_macros.end(); }

      iterator end() noexcept
      { return _used_macros.end(); }

      size_type size() const noexcept
      { return _used_macros.size(); }

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
