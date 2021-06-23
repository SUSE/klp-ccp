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

#ifndef BUILTINS_HH
#define BUILTINS_HH

#include <utility>
#include <memory>
#include <functional>
#include <vector>
#include <string>

namespace klp
{
  namespace ccp
  {
    class target;

    namespace ast
    {
      class constexpr_value;
      class ast;
      class expr_func_invocation;
      class expr_id;
      class type_specifier_tdid;
    }

    namespace types
    {
      class type;
      class addressable_type;
    }

    class builtin_func
    {
    public:
      struct evaluation_result_type
      {
	std::shared_ptr<const types::type> type;
	std::unique_ptr<ast::constexpr_value> value;
	bool is_lvalue;
      };

      typedef std::function<std::unique_ptr<builtin_func>()> factory;

      constexpr builtin_func() noexcept
      {}

      virtual ~builtin_func() noexcept;

      virtual evaluation_result_type
      evaluate(ast::ast &a, const target &tgt,
	       const ast::expr_func_invocation &efi) const = 0;
    };

    class builtin_var
    {
    public:
      struct evaluation_result_type
      {
	std::shared_ptr<const types::type> type;
	bool is_lvalue;
      };

      typedef std::unique_ptr<builtin_var> (*factory)();

      constexpr builtin_var() noexcept;
      virtual ~builtin_var() noexcept;

      virtual evaluation_result_type
      evaluate(ast::ast &a, const target &tgt,
	       const ast::expr_id &eid) const = 0;

      static factory lookup(const std::string &id);
    };

    std::unique_ptr<builtin_var> builtin_var__func__create();


    class builtin_typedef
    {
    public:
      struct factory
      {
	typedef std::function<std::unique_ptr<builtin_typedef>()> create_type;

	factory(const std::string &_name, const create_type &_create);

	std::string name;
	create_type create;
      };

      typedef std::vector<factory> factories;

      virtual ~builtin_typedef() noexcept;

      virtual std::shared_ptr<const types::addressable_type>
      evaluate(ast::ast &a, const target &tgt,
	       const ast::type_specifier_tdid &ts_tdid) const = 0;
    };
  }
}


#endif
