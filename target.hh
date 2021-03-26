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

#ifndef TARGET_HH
#define TARGET_HH

#include <functional>
#include <map>
#include <string>
#include "types.hh"
#include "mp_arithmetic.hh"
#include "builtins.hh"

namespace klp
{
  namespace ccp
  {
    namespace ast
    {
      class ast;
      class expr;
      class attribute_specifier_list;
      class type_qualifier_list;
      class specifier_qualifier_list;
      class declaration_specifiers;
    };

    class header_resolver;
    class preprocessor;
    class execution_charset_encoder;

    class target
    {
    public:
      virtual ~target() noexcept;

      virtual void parse_command_line
		(int argc, const char *argv[],
		 header_resolver &hr,
		 preprocessor &pp,
		 const std::function<void(const std::string&)> &report_warning)
      = 0;

      virtual const builtin_func::factory*
      lookup_builtin_func(const std::string &id) const noexcept = 0;

      virtual const builtin_typedef::factories&
      get_builtin_typedefs() const noexcept = 0;

      // Attributes following a pointer declarator's asterisk.
      virtual std::shared_ptr<const types::pointer_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::pointer_type> &&t,
			  ast::type_qualifier_list &tql) const = 0;

      // Attributes following the opening parenthesis in an
      // direct_declarator_parenthesized
      // resp. abstract_declarator_parenthesized.
      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::attribute_specifier_list * const asl) const = 0;

      // Final attributes application on function parameters' types.
      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::declaration_specifiers &ds,
			  ast::attribute_specifier_list * const asl) const = 0;

      // Final attributes application on type names.
      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::specifier_qualifier_list &sql) const = 0;

      // Final attributes application to init declarators.
      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::declaration_specifiers &ds,
			  ast::attribute_specifier_list * const asl_before,
			  ast::attribute_specifier_list * const asl_middle,
			  ast::attribute_specifier_list * const asl_after)
	const = 0;

      // Final attributes application to non-bitfield struct/union
      // member declarators.
      virtual std::shared_ptr<const types::addressable_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::addressable_type> &&t,
			  ast::attribute_specifier_list * const soud_asl_before,
			  ast::attribute_specifier_list * const soud_asl_after,
			  ast::specifier_qualifier_list &sql,
			  ast::attribute_specifier_list * const asl_before,
			  ast::attribute_specifier_list * const asl_after)
	const = 0;

      // Final attributes application to bitfield struct/union member
      // declarators.
      virtual std::shared_ptr<const types::bitfield_type>
      evaluate_attributes(ast::ast &a,
			  const std::function<void(ast::expr&)> &eval_expr,
			  std::shared_ptr<const types::bitfield_type> &&t,
			  ast::attribute_specifier_list * const soud_asl_before,
			  ast::attribute_specifier_list * const soud_asl_after,
			  ast::specifier_qualifier_list &sql,
			  ast::attribute_specifier_list * const asl_before,
			  ast::attribute_specifier_list * const asl_after)
	const = 0;


      virtual bool is_char_signed() const noexcept = 0;
      virtual bool is_wchar_signed() const noexcept = 0;

      virtual bool is_bitfield_default_signed() const noexcept = 0;

      virtual mpa::limbs::size_type
      get_std_int_width(const types::std_int_type::kind k) const noexcept = 0;

      virtual mpa::limbs get_std_int_size(const types::std_int_type::kind k)
	const = 0;

      virtual mpa::limbs::size_type
      get_std_int_alignment(const types::std_int_type::kind k)
	const noexcept = 0;

      virtual mpa::limbs::size_type
      get_ext_int_width(const types::ext_int_type::kind k) const noexcept = 0;

      virtual mpa::limbs get_ext_int_size(const types::ext_int_type::kind k)
	const = 0;

      virtual mpa::limbs::size_type
      get_ext_int_alignment(const types::ext_int_type::kind k)
	const noexcept = 0;

      typedef std::map<std::string, types::ext_int_type::kind> ext_int_keywords;

      virtual ext_int_keywords get_ext_int_keywords() const = 0;

      virtual std::shared_ptr<const types::int_type>
      width_to_int_type(const mpa::limbs::size_type w, const bool is_signed,
			const bool std_int_required)
	const = 0;

      virtual std::shared_ptr<const types::int_type>
      create_int_max_type(const bool is_signed) const = 0;

      virtual types::real_float_type::format
      get_std_float_format(const types::std_float_type::kind k)
	const noexcept = 0;

      virtual mpa::limbs get_std_float_size(const types::std_float_type::kind k)
	const = 0;

      virtual mpa::limbs::size_type
      get_std_float_alignment(const types::std_float_type::kind k) const = 0;

      virtual types::real_float_type::format
      get_ext_float_format(const types::ext_float_type::kind k)
	const noexcept = 0;

      virtual mpa::limbs get_ext_float_size(const types::ext_float_type::kind k)
	const = 0;

      virtual mpa::limbs::size_type
      get_ext_float_alignment(const types::ext_float_type::kind k) const = 0;

      typedef std::map<std::string, types::ext_float_type::kind>
		ext_float_keywords;

      virtual ext_float_keywords get_ext_float_keywords() const = 0;

      virtual std::shared_ptr<const types::int_type>
      create_ptrdiff_type(const bool is_signed) const = 0;

      virtual mpa::limbs::size_type get_ptrdiff_width() const noexcept = 0;

      virtual mpa::limbs get_pointer_size() const = 0;

      virtual mpa::limbs::size_type get_pointer_alignment() const noexcept = 0;

      virtual mpa::limbs::size_type get_biggest_alignment_log2()
	const noexcept = 0;

      enum class execution_charset_encoding
      {
	ecse_char,
	ecse_wchar,
	ecse_char8,
	ecse_char16,
	ecse_char32,
      };

      virtual std::unique_ptr<execution_charset_encoder>
      get_execution_charset_encoder(const execution_charset_encoding e)
	const = 0;

      virtual void
      evaluate_enum_type(ast::ast &a,
			 const std::function<void(ast::expr&)> &eval_expr,
			 ast::attribute_specifier_list * const ed_asl_before,
			 ast::attribute_specifier_list * const ed_asl_after,
			 types::enum_content &ec) const = 0;

      virtual void
      layout_struct(ast::ast &a,
		    const std::function<void(ast::expr&)> &eval_expr,
		    ast::attribute_specifier_list * const soud_asl_before,
		    ast::attribute_specifier_list * const soud_asl_after,
		    types::struct_or_union_content &souc) const = 0;

      virtual void
      layout_union(ast::ast &a,
		   const std::function<void(ast::expr&)> &eval_expr,
		   ast::attribute_specifier_list * const soud_asl_before,
		   ast::attribute_specifier_list * const soud_asl_after,
		   types::struct_or_union_content &souc)
	const = 0;
    };
  }
}

#endif
