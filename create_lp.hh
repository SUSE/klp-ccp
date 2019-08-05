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

#ifndef CREATE_LP_HH
#define CREATE_LP_HH

#include <string>

namespace klp
{
  namespace ast
  {
    class ast_translation_unit;
  }

  class lp_creation_policy;
  class code_remarks;
  class output_remarks;

  namespace ccp
  {
    void create_lp(const std::string &outfile,
		   const ast::ast_translation_unit &atu,
		   const lp_creation_policy &pol,
		   code_remarks &remarks,
		   output_remarks &output_remarks);
  }
}

#endif
