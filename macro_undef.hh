#ifndef MACRO_UNDEF_HH
#define MACRO_UNDEF_HH

#include <memory>
#include <utility>
#include "file_range.hh"

namespace klp
{
  namespace ccp
  {
    class macro;

    class macro_undef
    {
    public:
      template<typename... Targs>
      static std::shared_ptr<const macro_undef> create(Targs&&... args);

      const std::shared_ptr<const macro>&
      get_macro() const noexcept
      { return _macro; }

      const file_range& get_file_range() const noexcept
      { return _file_range; }

    private:
      macro_undef(std::shared_ptr<const macro> &&macro,
		  const file_range &file_range);

      std::shared_ptr<const macro> &_macro;
      file_range _file_range;
    };

    template<typename... Targs>
    std::shared_ptr<const macro_undef> macro_undef::create(Targs&&... args)
    {
      return std::make_shared<macro_undef>(
				macro_undef(std::forward<Targs>(args)...));
    }
  }
}

#endif
