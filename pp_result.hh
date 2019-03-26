#ifndef PP_RESULT_HH
#define PP_RESULT_HH

#include <vector>
#include <memory>
#include <utility>
#include "raw_pp_tokens.hh"
#include "raw_pp_tokens_range.hh"
#include "pp_tokens.hh"

namespace klp
{
  namespace ccp
  {
    class pp_result
    {
    public:
      class macro_invocation
      {

      private:
	friend class pp_result;
	friend class preprocessor;

	macro_invocation(const raw_pp_tokens_range &invocation_range);

	raw_pp_tokens_range _invocation_range;
      };

      const raw_pp_tokens& get_raw_tokens() const noexcept
      { return _raw_tokens; }

      const pp_tokens &get_pp_tokens() const noexcept
      { return _pp_tokens; }

      pp_tokens &get_pp_tokens() noexcept
      { return _pp_tokens; }

    private:
      friend class preprocessor;

      pp_result();

      void _append_token(const raw_pp_token &tok);
      void _append_token(raw_pp_token &&tok);
      void _append_token(pp_token &&tok);

      raw_pp_tokens::size_type _get_last_raw_index() const noexcept;
      pp_tokens::size_type _get_last_pp_index() const noexcept;

      macro_invocation&
      _add_macro_invocation(const raw_pp_tokens_range &invocation_range);

      void _extend_macro_invocation_range(macro_invocation &invocation,
					  const raw_pp_token_index new_end);

      raw_pp_tokens _raw_tokens;
      pp_tokens _pp_tokens;

      std::vector<std::unique_ptr<macro_invocation>> _macro_invocations;
    };
  }
}

#endif
