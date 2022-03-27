/*
 * Copyright (C) 2019  SUSE Software Solutions Germany GmbH
 *
 * This file is part of klp-ccp.
 *
 * klp-ccp is free software: you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * klp-ccp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
 */

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
#include "pp_tokenizer.hh"
#include "pp_result.hh"

namespace klp
{
  namespace ccp
  {
    class pp_tokenizer;
    class target;

    class preprocessor
    {
    public:
      preprocessor(const header_resolver &header_resolver,
		   const target &tgt);

      preprocessor(preprocessor &&pp);

      ~preprocessor() noexcept;

      void add_root_source(const std::string &filename,
			   const bool is_preinclude);

      void set_base_file(const std::string &filename);

      pp_token_index read_next_token();

      const pp_result::header_inclusion_node&
      get_pending_token_source(const raw_pp_token_index tok);

      code_remarks& get_remarks() noexcept
      { return _remarks; }

      std::unique_ptr<const pp_result> grab_result() noexcept
      { return std::move(_pp_result); }

      const pp_result& get_result() const noexcept
      { return *_pp_result; }

      void register_builtin_macro(const std::string &name,
				  const std::string &repl);
      void register_builtin_macro
		(const std::string &name,
		 const std::initializer_list<const char *> &arg_list,
		 const bool variadic,
		 const std::string &repl);

      void register_predefined_macro
	(const std::string &signature,
	 const std::string &repl,
	 const std::function<void(const std::string)> &report_warning);

      void register_predefined_macro_undef
	(const std::string &name,
	 const std::function<void(const std::string)> &report_warning);

    private:
      class _macro_expander;
      friend class _macro_expander;

      class _pp_token;
      typedef std::vector<_pp_token> _pp_tokens;
      struct _cond_incl_state;
      struct _expansion_state;

      template<typename T>
      void _grab_remarks_from(T &from);

      static pp_token_index _emit_pp_token(pp_result &r, _pp_token &&tok);

      _pp_token _read_next_plain_token();

      void _handle_eof_from_tokenizer(raw_pp_token &&eof_tok);

      void _fixup_inclusion_node_ranges() noexcept;

      const pp_result::header_inclusion_node&
      _raw_tok_it_to_source(const raw_pp_tokens::const_iterator &it);

      const pp_result::header_inclusion_node&
      _raw_pp_tokens_range_to_source(const raw_pp_tokens_range &r);

      range_in_file
      _raw_pp_tokens_range_to_range_in_file(const raw_pp_tokens_range &r)
	const noexcept;

      const pp_result::header_inclusion_node&
      _pp_token_to_source(const _pp_token &tok);

      range_in_file _pp_token_to_range_in_file(const _pp_token &tok)
	const noexcept;

      void _handle_pp_directive();

      _pp_token
      _expand(_expansion_state &state,
	      const std::function<_pp_token()> &token_reader,
	      const bool from_cond_incl_cond = false);

      std::vector<_pp_token>
      _handle_object_macro_invocation(const pp_result::macro &macro,
				      _pp_token &&id_tok);

      std::vector<_pp_token>
      _handle_func_macro_invocation(
			const pp_result::macro &macro,
			const raw_pp_token_index invocation_begin,
			const std::function<_pp_token()> &token_reader,
			const bool *update_cur_macro_invocation_range);

      std::tuple<_pp_tokens, _pp_tokens, _pp_token>
      _create_macro_arg(const std::function<_pp_token()> &token_reader,
			const bool expand, const bool variadic);

      void _handle_include(const raw_pp_tokens_range &directive_range);

      void _push_cond_incl(const raw_pp_token_index range_begin);
      void _pop_cond_incl(const raw_pp_token_index range_end);
      bool _cur_incl_node_is_cond() const noexcept;
      void _enter_cond_incl();
      bool _cond_incl_inactive() const noexcept;

      bool
      _eval_conditional_inclusion(const raw_pp_tokens_range &directive_range);

      const pp_result::macro&
      _handle_macro_definition(const raw_pp_tokens_range &directive_range);


      typedef std::function<pp_except(const std::string&,
				      const raw_pp_tokens::const_iterator&)>
	_report_fatal_at_raw_tok_type;

      static std::tuple<raw_pp_tokens::const_iterator,
			std::string,
			bool,
			std::vector<std::string>,
			bool>
      _parse_macro_signature
			(const raw_pp_tokens::const_iterator begin,
			 const raw_pp_tokens::const_iterator end,
			 const _report_fatal_at_raw_tok_type &report_fatal);

      static raw_pp_tokens
      _normalize_macro_repl(const raw_pp_tokens::const_iterator begin,
			    const raw_pp_tokens::const_iterator end,
			    const bool func_like,
			    const std::vector<std::string> &arg_names,
			    const _report_fatal_at_raw_tok_type &report_fatal);

      std::string _base_file;
      const header_resolver &_header_resolver;
      const target &_tgt;

      std::unique_ptr<pp_result> _pp_result;
      pp_result::header_inclusion_roots::iterator _cur_header_inclusion_root;
      std::stack<std::reference_wrapper<pp_result::inclusion_node>> _inclusions;
      std::stack<_cond_incl_state> _cond_incl_states;
      std::size_t _cond_incl_nesting;

      std::stack<pp_tokenizer> _tokenizers;
      std::unique_ptr<_expansion_state> _root_expansion_state;
      std::map<std::string,
	       std::reference_wrapper<const pp_result::macro> > _macros;

      pp_result::macro_invocation *_cur_macro_invocation;

      code_remarks _remarks;

      std::size_t __counter__;
      std::queue<_pp_token> _pending_tokens;
      bool _maybe_pp_directive;
      bool _line_empty;
      bool _eof;
      raw_pp_token_index _eof_index;
   };
  }
}

#endif
