#ifndef PP_TRACKING_HH
#define PP_TRACKING_HH

#include "raw_pp_tokens.hh"

namespace klp
{
  namespace ccp
  {
    class pp_tracking
    {
    private:
      friend class preprocessor;

      pp_tracking();

      void _append_token(const raw_pp_token &tok);
      void _append_token(raw_pp_token &&tok);

      raw_pp_tokens _raw_tokens;
    };
  }
}

#endif
