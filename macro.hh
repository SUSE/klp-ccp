#ifndef MACRO_HH
#define MACRO_HH

#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include "pp_tokens.hh"
#include "file_range.hh"
#include "code_remark.hh"
#include "code_remarks.hh"

namespace suse
{
  namespace cp
  {
    class macro
    {
    public:
      class instance
      {
      public:
	instance(const std::shared_ptr<const macro> &macro,
		 used_macros &&used_macros_base,
		 used_macro_undefs &&used_macro_undefs_base,
		 std::vector<pp_tokens> &&args,
		 std::vector<pp_tokens> &&exp_args,
		 const file_range &file_range);

	pp_token read_next_token();

	const macro& get_macro() const noexcept
	{ return *_macro; }

	code_remarks& get_remarks() noexcept
	{ return _remarks; }

      private:
	const pp_tokens*
	_resolve_arg(const std::string &name, const bool expanded)
	  const noexcept;

	used_macros _tok_expansion_history_init() const;

	used_macros _tok_used_macros_init() const;

	used_macro_undefs _tok_used_macro_undefs_init() const;

	pp_token _handle_stringification();

	void _add_concat_token(const pp_token &tok);
	pp_token _yield_concat_token();

	const std::shared_ptr<const macro> &_macro;
	const used_macros _used_macros_base;
	const used_macro_undefs _used_macro_undefs_base;
	std::map<std::string,
		 std::pair<pp_tokens, pp_tokens> > _args;
	const file_range _file_range;
	code_remarks _remarks;

	pp_tokens::const_iterator _it_repl;
	const pp_tokens *_cur_arg;
	pp_tokens::const_iterator _cur_arg_it;


	std::unique_ptr<pp_token> _concat_token;
	bool _in_concat;

	bool _anything_emitted;
      };

      static std::shared_ptr<const macro>
      parse_macro_definition(const pp_tokens::const_iterator begin,
			     const pp_tokens::const_iterator end,
			     std::shared_ptr<const macro_undef> &&macro_undef,
			     code_remarks &remarks);

      bool operator==(const macro &rhs) const noexcept;

      bool operator!=(const macro &rhs) const noexcept
      { return !(*this == rhs); }

      bool is_func_like() const noexcept
      { return _func_like; }

      bool is_variadic() const noexcept
      { return _variadic; }

      size_t non_va_arg_count() const noexcept;
      bool shall_expand_arg(const size_t pos) const noexcept;

      const std::string& get_name() const noexcept
      { return _name; }

      const file_range& get_file_range() const noexcept
      { return _file_range; }

    private:
      macro(const std::string &name,
	    const bool func_like,
	    const bool variadic,
	    std::vector<std::string> &&arg_names,
	    pp_tokens &&repl,
	    const file_range &file_range,
	    std::shared_ptr<const macro_undef> &&prev_macro_undef);

      template<typename... Targs>
      static std::shared_ptr<const macro> create(Targs&&... args);

      static pp_tokens _normalize_repl(const pp_tokens::const_iterator begin,
				       const pp_tokens::const_iterator end,
				       const bool func_like,
				       const std::set<std::string> &arg_names,
				       code_remarks &remarks);

      pp_tokens::const_iterator
      _next_non_ws_repl(const pp_tokens::const_iterator it) const noexcept;

      bool _is_stringification(pp_tokens::const_iterator it) const noexcept;

      pp_tokens::const_iterator
      _skip_stringification_or_single(const pp_tokens::const_iterator &it)
	const noexcept;

      bool _is_concat_op(const pp_tokens::const_iterator &it) const noexcept;

      std::string _name;
      std::vector<std::string> _arg_names;
      pp_tokens _repl;
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
