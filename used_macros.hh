#ifndef USED_MACROS_HH
#define USED_MACROS_HH

#include <functional>
#include <set>

namespace klp
{
  namespace ccp
  {
    class macro;

    class used_macros
    {
    private:
      struct _compare
      {
	bool operator()(const std::reference_wrapper<const macro> a,
			const std::reference_wrapper<const macro> b)
	  const noexcept;
      };

      typedef
      std::set<std::reference_wrapper<const macro>, _compare> _used_macros_type;

    public:
      used_macros() = default;

      bool empty() const noexcept
      { return _used_macros.empty(); }

      void clear() noexcept;

      std::size_t count(const macro &m) const noexcept;

      used_macros& operator+=(const used_macros &rhs);

      used_macros& operator+=(const macro &rhs);

    private:
       used_macros(_used_macros_type &&um);

      _used_macros_type _used_macros;
    };
  }
}

#endif
