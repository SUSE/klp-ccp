#ifndef MACRO_HH
#define MACRO_HH

#include <vector>
#include <string>
#include <map>
#include "raw_pp_tokens.hh"
#include "raw_pp_tokens_range.hh"
#include "code_remarks.hh"
#include "macro_undef.hh"

namespace klp
{
  namespace ccp
  {
    class macro
    {
    public:
      bool operator==(const macro &rhs) const noexcept;

      bool operator!=(const macro &rhs) const noexcept
      { return !(*this == rhs); }

      bool is_func_like() const noexcept
      { return _func_like; }

      bool is_variadic() const noexcept
      { return _variadic; }

      const std::vector<std::string>& get_arg_names() const noexcept;
      size_t non_va_arg_count() const noexcept;
      bool shall_expand_arg(const size_t pos) const noexcept;

      const std::string& get_name() const noexcept
      { return _name; }

      const raw_pp_tokens_range& get_directive_range() const noexcept
      { return _directive_range; }

      const raw_pp_tokens& get_repl() const noexcept
      { return _repl; }

    private:
      friend class pp_result;

      macro(const std::string &name,
	    const bool func_like,
	    const bool variadic,
	    std::vector<std::string> &&arg_names,
	    raw_pp_tokens &&repl,
	    const raw_pp_tokens_range &directive_range);

      raw_pp_tokens::const_iterator
      _next_non_ws_repl(const raw_pp_tokens::const_iterator it) const noexcept;

      raw_pp_tokens::const_iterator
      _skip_stringification_or_single(const raw_pp_tokens::const_iterator &it)
	const noexcept;

      bool _is_concat_op(const raw_pp_tokens::const_iterator &it)
	const noexcept;

      std::string _name;
      std::vector<std::string> _arg_names;
      raw_pp_tokens _repl;
      raw_pp_tokens_range _directive_range;
      std::vector<bool> _do_expand_args;
      bool _func_like;
      bool _variadic;
    };
  }
}

#endif
