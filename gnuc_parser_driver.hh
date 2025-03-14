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

#ifndef GNUC_PARSER_DRIVER_HH
#define GNUC_PARSER_DRIVER_HH

#include <stack>
#include <string>
#include <set>

#include "gnuc_parser.hh"
#include "pp_tokens.hh"
#include "preprocessor.hh"
#include "target.hh"
#include "code_remarks.hh"

namespace klp
{
  namespace ccp
  {
    namespace yy
    {
      class gnuc_parser_driver
      {
      public:
	gnuc_parser_driver(preprocessor &&pp, const target &tgt);

	gnuc_parser_driver(gnuc_parser_driver&&) = delete;

	~gnuc_parser_driver() noexcept;

	void parse();

#ifdef DEBUG_PARSER
	void parse_debug();
#endif

	code_remarks& get_remarks() noexcept
	{ return _remarks; }


	ast::ast_translation_unit grab_result();

      private:
	klp::ccp::yy::gnuc_parser::token_type
	lex(gnuc_parser::semantic_type *value,
	    gnuc_parser::location_type *loc);

	void error(const gnuc_parser::location_type& loc,
		   const std::string& msg);

	void enter_td_scope();
	void leave_td_scope();

	void stash_td_scope();
	void restore_stashed_td_scope();

	void set_in_typedef() noexcept;
	void clear_in_typedef() noexcept;

	void begin_ignore_td_spec() noexcept;
	void end_ignore_td_spec() noexcept;

	void handle_decl_id(const pp_token_index tok_index);
	void handle_enumerator_id(const pp_token_index tok_index);
	void handle_param_id(const pp_token_index tok_index);

	bool is_typedef_id(const std::string &id) const noexcept;

	void _grab_pp_remarks();

	typedef std::set<std::string> _typedefs_type;
	typedef std::stack<_typedefs_type> _typedefs_scopes_type;

	_typedefs_scopes_type _typedefs_scopes;
	_typedefs_type _stashed_typedef_scope;

	preprocessor _pp;
	gnuc_parser _parser;

	ast::translation_unit *_result;

	code_remarks _remarks;

	unsigned int _ignore_td_spec;
	bool _in_typedef;

	const target::ext_int_keywords _ext_int_keywords;
	const target::ext_float_keywords _ext_float_keywords;

	friend class gnuc_parser;
      };
    }
  }
}

#endif
