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
      class _scanned_macro_invocation;
      class _macro_expansion_stack_entry;
      struct _cond_incl_state;

      using _macro_expansion_forest =
	pp_result::macro_invocation::expansion_forest;

      template<typename T>
      void _grab_remarks_from(T &from);

      static pp_token_index _emit_pp_token(pp_result &r, pp_token &&tok);

      enum class _read_ctl
      {
	normal,
	no_file_crossing,
	peek,
      };

      const std::pair<raw_pp_token_index, bool>
      _read_next_plain_token(const _read_ctl ctl);

      void _handle_eof_from_tokenizer(const raw_pp_token &eof_tok);

      void _fixup_inclusion_node_ranges() noexcept;

      const pp_result::header_inclusion_node&
      _raw_tok_it_to_source(const raw_pp_tokens::const_iterator &it);

      const pp_result::header_inclusion_node&
      _raw_pp_tokens_range_to_source(const raw_pp_tokens_range &r);

      range_in_file
      _raw_pp_tokens_range_to_range_in_file(const raw_pp_tokens_range &r)
	const noexcept;

      void _handle_pp_directive(const _read_ctl ctl);

      std::pair<bool, std::vector<pp_token>>
      _try_expand_raw
	(const raw_pp_token_index id_tok_index,
	 const
	   std::function<std::pair<raw_pp_token_index, bool>(const _read_ctl)>
	   &raw_token_reader,
	 const bool from_cond_incl_cond);

      bool _macro_is_in_id_tok_expansion_history
	(const pp_result::macro &m,
	 const _macro_expansion_forest &forest,
	 _macro_expansion_forest::nodes_size_type emitting_node_index,
	 std::size_t id_tok_index) const noexcept;

      _scanned_macro_invocation
      _read_func_macro_invocation
		(const pp_result::macro &m,
		 raw_pp_tokens_range invocation_range,
		 const
		   std::function<std::pair<pp_token, raw_pp_tokens_range>
				 (const _read_ctl, const raw_pp_tokens_range&)>
		   &token_reader);

      std::tuple<std::vector<pp_token>, pp_token, std::size_t, std::size_t,
		 raw_pp_token_index>
      _read_macro_arg
		(raw_pp_tokens_range invocation_range,
		 const
		   std::function<std::pair<pp_token, raw_pp_tokens_range>
				 (const _read_ctl, const raw_pp_tokens_range&)>
		    &token_reader,
		 const bool variadic);

      _scanned_macro_invocation
      _read_defined_operator_invocation
		(raw_pp_tokens_range invocation_range,
		 const
		   std::function<std::pair<pp_token, raw_pp_tokens_range>
				 (const _read_ctl, const raw_pp_tokens_range&)>
		   &token_reader);

    private:
      std::vector<pp_token>
      _expand_one(_macro_expansion_forest::_rescan_macro_invocation &rmi,
		  pp_result::macro_invocation &mi,
		  std::vector<std::vector<pp_token>> &&args,
		  std::vector<std::vector<pp_token>> &&exp_args,
		  const raw_pp_tokens_range &invocation_range);

      std::vector<pp_token>
      _expand_one_builtin_special
			(_macro_expansion_forest::_rescan_macro_invocation &rmi,
			 std::vector<std::vector<pp_token>> &&args,
			 const raw_pp_tokens_range &invocation_range);

      static std::string _stringify(const std::vector<pp_token> &arg_toks);

      pp_token _concat(pp_token &&tok1, pp_token &&tok2,
		       const raw_pp_tokens_range &invocation);

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

      static void
      _check_macro_repl(raw_pp_tokens::const_iterator begin,
			raw_pp_tokens::const_iterator end,
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
      std::map<std::string,
	       std::reference_wrapper<const pp_result::macro> > _macros;

      code_remarks _remarks;

      std::size_t __counter__;
      const pp_result::macro &_m_defined_op;

      std::queue<pp_token> _expanded_tokens;
      bool _maybe_pp_directive;
      bool _eof;
      raw_pp_token_index _eof_index;
      raw_pp_token_index _peeked_begin;
      static constexpr raw_pp_token_index _none_peeked =
	std::numeric_limits<raw_pp_token_index>::max();
      bool _peeked_maybe_pp_directive;
   };
  }
}

#endif
