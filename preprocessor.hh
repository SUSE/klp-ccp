#ifndef PREPROCESSOR_HH
#define PREPROCESSOR_HH

#include <stack>
#include <map>
#include <memory>
#include <string>
#include <queue>
#include <functional>
#include "pp_tokens.hh"
#include "raw_pp_tokens.hh"
#include "code_remarks.hh"
#include "header_resolver.hh"
#include "macro.hh"
#include "pp_tokenizer.hh"

namespace klp
{
  namespace ccp
  {
    class pp_tokenizer;
    class header_inclusion_root;
    class inclusion_node;
    class conditional_inclusion_node;
    class architecture;

    class preprocessor
    {
    public:
      typedef std::vector<std::unique_ptr<header_inclusion_root> >
	header_inclusion_roots_type;

      preprocessor(header_inclusion_roots_type &header_inclusion_roots,
		   const header_resolver &header_resolver,
		   const architecture &arch);

      pp_token read_next_token();

      code_remarks& get_remarks() noexcept
      { return _remarks; }

      header_inclusion_roots_type
      grab_header_inclusion_roots()
      { return std::move(_header_inclusion_roots); }

    private:
      class _macro_instance
      {
      public:
	_macro_instance(const std::shared_ptr<const macro> &macro,
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

	bool _is_stringification(raw_pp_tokens::const_iterator it)
	  const noexcept;

	raw_pp_tokens::const_iterator
	_skip_stringification_or_single(const raw_pp_tokens::const_iterator &it)
	  const noexcept;

	bool _is_concat_op(const raw_pp_tokens::const_iterator &it)
	  const noexcept;

	pp_token _handle_stringification();

	void _add_concat_token(const pp_token &tok);
	void _add_concat_token(const raw_pp_token &raw_tok);
	pp_token _yield_concat_token();

	const std::shared_ptr<const macro> &_macro;
	const used_macros _used_macros_base;
	const used_macro_undefs _used_macro_undefs_base;
	std::map<std::string,
		 std::pair<pp_tokens, pp_tokens> > _args;
	const file_range _file_range;
	code_remarks _remarks;

	raw_pp_tokens::const_iterator _it_repl;
	const pp_tokens *_cur_arg;
	pp_tokens::const_iterator _cur_arg_it;


	std::unique_ptr<pp_token> _concat_token;
	bool _in_concat;

	bool _anything_emitted;
      };

      struct _cond_incl_state
      {
	_cond_incl_state(const file_range::loc_type _start_loc);

	file_range::loc_type start_loc;
	used_macros um;
	used_macro_undefs umu;
	conditional_inclusion_node *incl_node;
	bool branch_active;
      };

      struct _expansion_state
      {
	_expansion_state();

	std::vector<_macro_instance> macro_instances;
	std::queue<pp_token> pending_tokens;
      };

      template<typename T>
      void _grab_remarks_from(T &from);

      pp_token _read_next_plain_token();
      void _handle_eof_from_tokenizer(raw_pp_token &&eof_tok);
      void _handle_pp_directive(raw_pp_token &&sharp_tok);

      pp_token
      _expand(_expansion_state &state,
	      const std::function<pp_token()> &token_reader,
	      const bool from_cond_incl_cond = false);

      _macro_instance
      _handle_object_macro_invocation(const std::shared_ptr<const macro> &macro,
				      pp_token &&id_tok);

      _macro_instance
      _handle_func_macro_invocation(
			const std::shared_ptr<const macro> &macro,
			used_macros &&used_macros_base,
			used_macro_undefs &&used_macro_undefs_base,
			const file_range &id_tok_file_range,
			const std::function<pp_token()> &token_reader);

      std::tuple<pp_tokens, pp_tokens, pp_token>
      _create_macro_arg(const std::function<pp_token()> &token_reader,
			const bool expand, const bool variadic,
			used_macros &used_macros_base,
			used_macro_undefs &used_macro_undefs_base);

      void _handle_include(raw_pp_tokens &&directive_toks);

      bool _cond_incl_inactive() const noexcept;

      bool _cur_incl_node_is_cond() const noexcept;

      void _enter_cond_incl();

      void _pop_cond_incl(const file_range::loc_type end_loc);

      bool _eval_conditional_inclusion(raw_pp_tokens &&directive_toks);


      const header_resolver &_header_resolver;
      const architecture &_arch;

      header_inclusion_roots_type &_header_inclusion_roots;
      header_inclusion_roots_type::iterator _cur_header_inclusion_root;
      std::stack<std::reference_wrapper<inclusion_node> > _inclusions;
      std::stack<_cond_incl_state> _cond_incl_states;
      std::size_t _cond_incl_nesting;

      std::stack<pp_tokenizer> _tokenizers;
      _expansion_state _root_expansion_state;
      std::map<std::string, std::shared_ptr<const macro> > _macros;
      std::map<std::string, std::shared_ptr<const macro_undef> > _macro_undefs;

      code_remarks _remarks;

      std::size_t __counter__;
      pp_token _eof_tok;
      std::queue<pp_token> _pending_tokens;
      bool _maybe_pp_directive;
      bool _line_empty;
   };
  }
}

#endif
