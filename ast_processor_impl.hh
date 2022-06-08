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

#ifndef AST_PROCESSOR_IMPL_HH
#define AST_PROCESSOR_IMPL_HH

#include "ast_processor.hh"

namespace klp
{
  namespace ccp
  {
    namespace ast
    {
      template<typename ret_type>
      processor<ret_type>::~processor() noexcept = default;


      template<typename ret_type, typename callable_type>
      _processor<ret_type, callable_type>::_processor(callable_type &&c)
	noexcept
	: _c(std::forward<callable_type>(c))
      {}

      template<typename ret_type, typename callable_type>
      _processor<ret_type, callable_type>::_processor(const _processor& p)
	noexcept
	: _c(p._c)
      {}

      template<typename ret_type, typename callable_type>
      _processor<ret_type, callable_type>::~_processor() noexcept = default;

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(expr_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(expr_comma &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_assignment &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_conditional &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(expr_binop &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(expr_cast &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_label_addr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(expr_unop_pre &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_sizeof_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_sizeof_type_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_alignof_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_alignof_type_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(generic_association &ga)
      {
	return _c(ga);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(generic_association_list &gal)
      {
	return _c(gal);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(expr_generic &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(expr_builtin_choose_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(offset_member_designator &omd)
      {
	return _c(omd);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_builtin_offsetof &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(expr_builtin_types_compatible_p &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_builtin_va_arg &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_array_subscript &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_func_invocation &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_member_deref &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_unop_post &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_compound_literal &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_statement &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(expr_id &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(expr_constant &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(string_literal &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_string_literal &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(expr_parenthesized &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(attribute &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(attribute_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(attribute_specifier &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(attribute_specifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(pointer &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(direct_abstract_declarator_parenthesized &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(direct_abstract_declarator_array &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(direct_abstract_declarator_func &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(abstract_declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(type_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(direct_declarator_id &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(direct_declarator_parenthesized &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(direct_declarator_array &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(direct_declarator_func &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(storage_class_specifier &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(type_qualifier &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(type_qualifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(type_specifier_pod &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(type_specifier_ext_int &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(type_specifier_ext_float &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(type_specifier_tdid &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(struct_declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(struct_declarator_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(struct_declaration_c99 &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(unnamed_struct_or_union &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(struct_declaration_unnamed_sou &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(struct_declaration_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(struct_or_union_def &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(struct_or_union_ref &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(enumerator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(enumerator_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(enum_def &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(enum_ref &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(typeof_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(typeof_type_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(function_specifier &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(specifier_qualifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(initializer_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(initializer_init_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(designator_member &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(designator_array &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(designator_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(designation &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(initializer_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(asm_label &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(init_declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(init_declarator_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(declaration &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(parameter_declaration_declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(parameter_declaration_abstract &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(parameter_declaration_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(identifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(declaration_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_labeled &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_case &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(stmt_case_range &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_default &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(local_label_declaration &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(local_label_declaration_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(block_item_decl &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(block_item_stmt &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(block_item_function_definition &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(block_item_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_compound &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_if &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_switch &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_while &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_do &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(stmt_for_init_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(stmt_for_init_decl &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_goto &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_continue &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_break &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_return &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(stmt_asm &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(asm_qualifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(asm_operand_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(asm_operand &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(asm_operand_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(asm_clobber_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(asm_jump_to_label_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::operator()(asm_directive &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(function_definition &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(external_declaration_decl &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(external_declaration_func &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _processor<ret_type, callable_type>::
      operator()(external_declaration_asm &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _processor<ret_type, callable_type>::operator()(translation_unit &e)
      {
	return _c(e);
      }


      template<typename ret_type>
      const_processor<ret_type>::~const_processor() noexcept = default;


      template<typename ret_type, typename callable_type>
      _const_processor<ret_type, callable_type>::
      _const_processor(callable_type &&c) noexcept
	: _c(std::forward<callable_type>(c))
      {}

      template<typename ret_type, typename callable_type>
      _const_processor<ret_type, callable_type>::
      _const_processor(const _const_processor& p) noexcept
	: _c(p._c)
      {}

      template<typename ret_type, typename callable_type>
      _const_processor<ret_type, callable_type>::~_const_processor() noexcept
	= default;

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const expr_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const expr_comma &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_assignment &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_conditional &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const expr_binop &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const expr_cast &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_label_addr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_unop_pre &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_sizeof_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_sizeof_type_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_alignof_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_alignof_type_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const generic_association &ga)
      {
	return _c(ga);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const generic_association_list &gal)
      {
	return _c(gal);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_generic &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_builtin_choose_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const offset_member_designator &omd)
      {
	return _c(omd);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_builtin_offsetof &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_builtin_types_compatible_p &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_builtin_va_arg &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_array_subscript &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_func_invocation &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_member_deref &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_unop_post &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_compound_literal &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_statement &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const expr_id &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_constant &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const string_literal &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_string_literal &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const expr_parenthesized &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const attribute &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const attribute_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const attribute_specifier &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const attribute_specifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const pointer &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const direct_abstract_declarator_parenthesized &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const direct_abstract_declarator_array &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const direct_abstract_declarator_func &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const abstract_declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const type_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const direct_declarator_id &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const direct_declarator_parenthesized &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const direct_declarator_array &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const direct_declarator_func &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const storage_class_specifier &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const type_qualifier &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const type_qualifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const type_specifier_pod &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const type_specifier_ext_int &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const type_specifier_ext_float &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const type_specifier_tdid &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const struct_declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const struct_declarator_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const struct_declaration_c99 &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const unnamed_struct_or_union &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const struct_declaration_unnamed_sou &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const struct_declaration_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const struct_or_union_def &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const struct_or_union_ref &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
       operator()(const enumerator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const enumerator_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const enum_def &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const enum_ref &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const typeof_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const typeof_type_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const function_specifier &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const specifier_qualifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const initializer_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const initializer_init_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const designator_member &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const designator_array &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const designator_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const designation &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const initializer_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const asm_label &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const init_declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const init_declarator_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const declaration &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const parameter_declaration_declarator &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const parameter_declaration_abstract &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const parameter_declaration_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const identifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const declaration_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_labeled &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const stmt_case &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_case_range &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_default &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const local_label_declaration &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const local_label_declaration_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const block_item_decl &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const block_item_stmt &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const block_item_function_definition &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const block_item_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_compound &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const stmt_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const stmt_if &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_switch &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const stmt_while &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const stmt_do &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_for_init_expr &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_for_init_decl &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const stmt_goto &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_continue &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const stmt_break &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const stmt_return &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type
      _const_processor<ret_type, callable_type>::operator()(const stmt_asm &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const asm_qualifier_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const asm_operand_name &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const asm_operand &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const asm_operand_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const asm_clobber_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const asm_jump_to_label_list &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const asm_directive &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const function_definition &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const external_declaration_decl &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const external_declaration_func &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const external_declaration_asm &e)
      {
	return _c(e);
      }

      template<typename ret_type, typename callable_type>
      ret_type _const_processor<ret_type, callable_type>::
      operator()(const translation_unit &e)
      {
	return _c(e);
      }
    }
  }
}

#endif
