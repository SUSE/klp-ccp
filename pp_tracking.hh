#ifndef PP_TRACKING_HH
#define PP_TRACKING_HH

#include "raw_pp_tokens.hh"
#include "raw_pp_tokens_range.hh"

namespace klp
{
  namespace ccp
  {
    class pp_tracking
    {
    public:
      const raw_pp_tokens& get_raw_tokens() const noexcept
      { return _raw_tokens; }

    private:
      friend class preprocessor;

      pp_tracking();

      void _append_token(const raw_pp_token &tok);
      void _append_token(raw_pp_token &&tok);

      raw_pp_tokens::size_type _get_last_index() const noexcept;

      raw_pp_tokens _raw_tokens;
    };
  }
}

#endif
