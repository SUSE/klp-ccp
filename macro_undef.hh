#ifndef MACRO_UNDEF_HH
#define MACRO_UNDEF_HH

#include <memory>
#include <utility>
#include "raw_pp_tokens_range.hh"

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

    private:
      macro_undef(std::shared_ptr<const macro> &&macro,
		  const raw_pp_tokens_range &directive_range);

      std::shared_ptr<const macro> &_macro;
      raw_pp_tokens_range _directive_range;
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
