#ifndef PP_RESULT_HH
#define PP_RESULT_HH

#include <vector>
#include <memory>
#include <utility>
#include "raw_pp_tokens.hh"
#include "raw_pp_tokens_range.hh"
#include "pp_tokens.hh"
#include "used_macros.hh"
#include "used_macro_undefs.hh"

namespace klp
{
  namespace ccp
  {
    class macro_undef;

    class pp_result
    {
    public:
      class macro_invocation
      {
      public:
	const raw_pp_tokens_range& get_source_range() const noexcept
	{ return _invocation_range; }

      private:
	friend class pp_result;
	friend class preprocessor;

	macro_invocation(const macro &m,
			 const raw_pp_tokens_range &invocation_range);

	raw_pp_tokens_range _invocation_range;
	used_macros _used_macros;
	used_macro_undefs _used_macro_undefs;
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
      _add_macro_invocation(const macro &m,
			    const raw_pp_tokens_range &invocation_range);

      void _extend_macro_invocation_range(macro_invocation &invocation,
					  const raw_pp_token_index new_end);

      const macro& _add_macro(const std::string &name,
			      const bool func_like,
			      const bool variadic,
			      std::vector<std::string> &&arg_names,
			      raw_pp_tokens &&repl,
			      const raw_pp_tokens_range &directive_range,
			      const macro_undef *prev_macro_undef);

      const macro_undef&
      _add_macro_undef(const macro &m,
		       const raw_pp_tokens_range &directive_range);

      raw_pp_tokens _raw_tokens;
      pp_tokens _pp_tokens;

      std::vector<std::unique_ptr<macro_invocation>> _macro_invocations;
      std::vector<std::unique_ptr<const macro>> _macros;
      std::vector<std::unique_ptr<const macro_undef>> _macro_undefs;
    };
  }
}

#endif
