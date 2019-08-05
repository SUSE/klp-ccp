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

#ifndef PP_EXPR_PARSER_DRIVER_HH
#define PP_EXPR_PARSER_DRIVER_HH

#include <functional>
#include "pp_expr_parser.hh"
#include "pp_token.hh"
#include "pp_tokens.hh"
#include "code_remarks.hh"

namespace klp
{
  namespace ccp
  {
    namespace yy
    {
      class pp_expr_parser_driver
      {
      public:
	pp_expr_parser_driver
			(const std::function<pp_token_index()> &token_reader,
			 pp_result &pp_result);

	~pp_expr_parser_driver() noexcept;

	void parse();

	code_remarks& get_remarks() noexcept
	{ return _remarks; }

	ast::ast_pp_expr grab_result();

      private:
	klp::ccp::yy::pp_expr_parser::token_type
	lex(pp_expr_parser::semantic_type *value,
	    pp_expr_parser::location_type *loc);

	void error(const pp_expr_parser::location_type& loc,
		   const std::string& msg);

	std::function<pp_token_index()> _token_reader;
	pp_result &_pp_result;
	ast::expr *_result;

	pp_expr_parser _parser;

	code_remarks _remarks;

	friend class pp_expr_parser;
      };
    }
  }
}

#endif
