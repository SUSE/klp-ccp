#ifndef MACRO_UNDEF_HH
#define MACRO_UNDEF_HH

#include "raw_pp_tokens_range.hh"

namespace klp
{
  namespace ccp
  {
    class macro;

    class macro_undef
    {
    private:
      friend class pp_result;

      macro_undef(const macro &macro,
		  const raw_pp_tokens_range &directive_range);

      const macro &_macro;
      raw_pp_tokens_range _directive_range;
    };
  }
}

#endif
