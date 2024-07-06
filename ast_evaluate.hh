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

#ifndef AST_EVALUATE_HH
#define AST_EVALUATE_HH

#include <memory>

namespace klp
{
  namespace ccp
  {
    class target;

    namespace ast
    {
      class ast;
      class constexpr_value;
      class expr;
    }

    namespace types
    {
      class type;
    }

    void
    check_types_assignment(klp::ccp::ast::ast &a, const target &tgt,
			   const types::type &t_target,
			   const ast::expr &e_source,
			   const bool is_func_arg);
  }
}

#endif
