#ifndef MACRO_HH
#define MACRO_HH

#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include "raw_pp_tokens.hh"
#include "file_range.hh"
#include "code_remarks.hh"

namespace klp
{
  namespace ccp
  {
    class macro
    {
    public:
      static std::shared_ptr<const macro>
      parse_macro_definition(const raw_pp_tokens::const_iterator begin,
			     raw_pp_tokens::const_iterator end,
			     std::shared_ptr<const macro_undef> &&macro_undef,
			     code_remarks &remarks);

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

      const file_range& get_file_range() const noexcept
      { return _file_range; }

      const raw_pp_tokens& get_repl() const noexcept
      { return _repl; }

    private:
      macro(const std::string &name,
	    const bool func_like,
	    const bool variadic,
	    std::vector<std::string> &&arg_names,
	    raw_pp_tokens &&repl,
	    const file_range &file_range,
	    std::shared_ptr<const macro_undef> &&prev_macro_undef);

      template<typename... Targs>
      static std::shared_ptr<const macro> create(Targs&&... args);

      static raw_pp_tokens
      _normalize_repl(const raw_pp_tokens::const_iterator begin,
		      const raw_pp_tokens::const_iterator end,
		      const bool func_like,
		      const std::set<std::string> &arg_names,
		      code_remarks &remarks);

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
      file_range _file_range;
      std::shared_ptr<const macro_undef> _prev_macro_undef;
      std::vector<bool> _do_expand_args;
      bool _func_like;
      bool _variadic;
    };

    template<typename... Targs>
    std::shared_ptr<const macro> macro::create(Targs&&... args)
    {
      return std::make_shared<macro>(macro(std::forward<Targs>(args)...));
    }
  }
}

#endif
