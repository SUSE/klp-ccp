#ifndef MACRO_UNDEF_HH
#define MACRO_UNDEF_HH

#include <string>
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

      macro_undef(const std::string &name,
		  const raw_pp_tokens_range &directive_range);

      std::string _name;
      raw_pp_tokens_range _directive_range;
    };
  }
}

#endif
