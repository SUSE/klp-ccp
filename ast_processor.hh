#ifndef AST_PROCESSOR_HH
#define AST_PROCESSOR_HH

#include <utility>
#include "ast.hh"

namespace klp
{
  namespace ccp
  {
    namespace ast
    {
      template <typename ret_type>
      class processor
      {
      public:
	virtual ~processor() noexcept = 0;

	virtual ret_type operator()(expr_list&) = 0;
	virtual ret_type operator()(expr_comma&) = 0;
	virtual ret_type operator()(expr_assignment&) = 0;
	virtual ret_type operator()(expr_conditional&) = 0;
	virtual ret_type operator()(expr_binop&) = 0;
	virtual ret_type operator()(expr_cast&) = 0;
	virtual ret_type operator()(expr_label_addr&) = 0;
	virtual ret_type operator()(expr_unop_pre&) = 0;
	virtual ret_type operator()(expr_sizeof_expr&) = 0;
	virtual ret_type operator()(expr_sizeof_type_name&) = 0;
	virtual ret_type operator()(expr_alignof_expr&) = 0;
	virtual ret_type operator()(expr_alignof_type_name&) = 0;
	virtual ret_type operator()(offset_member_designator&) = 0;
	virtual ret_type operator()(expr_builtin_offsetof&) = 0;
	virtual ret_type operator()(expr_builtin_types_compatible_p&) = 0;
	virtual ret_type operator()(expr_builtin_va_arg&) = 0;
	virtual ret_type operator()(expr_array_subscript&) = 0;
	virtual ret_type operator()(expr_func_invocation&) = 0;
	virtual ret_type operator()(expr_member_deref&) = 0;
	virtual ret_type operator()(expr_unop_post&) = 0;
	virtual ret_type operator()(expr_compound_literal&) = 0;
	virtual ret_type operator()(expr_statement&) = 0;
	virtual ret_type operator()(expr_id&) = 0;
	virtual ret_type operator()(expr_constant&) = 0;
	virtual ret_type operator()(string_literal&) = 0;
	virtual ret_type operator()(expr_string_literal&) = 0;
	virtual ret_type operator()(expr_parenthesized&) = 0;
	virtual ret_type operator()(attribute&) = 0;
	virtual ret_type operator()(attribute_list&) = 0;
	virtual ret_type operator()(attribute_specifier&) = 0;
	virtual ret_type operator()(attribute_specifier_list&) = 0;
	virtual ret_type operator()(pointer&) = 0;
	virtual ret_type
	operator()(direct_abstract_declarator_parenthesized&) = 0;
	virtual ret_type operator()(direct_abstract_declarator_array&) = 0;
	virtual ret_type operator()(direct_abstract_declarator_func&) = 0;
	virtual ret_type operator()(abstract_declarator&) = 0;
	virtual ret_type operator()(type_name&) = 0;
	virtual ret_type operator()(direct_declarator_id&) = 0;
	virtual ret_type operator()(direct_declarator_parenthesized&) = 0;
	virtual ret_type operator()(direct_declarator_array&) = 0;
	virtual ret_type operator()(direct_declarator_func&) = 0;
	virtual ret_type operator()(declarator&) = 0;
	virtual ret_type operator()(storage_class_specifier&) = 0;
	virtual ret_type operator()(type_qualifier&) = 0;
	virtual ret_type operator()(type_qualifier_list&) = 0;
	virtual ret_type operator()(type_specifier_pod&) = 0;
	virtual ret_type operator()(type_specifier_tdid&) = 0;
	virtual ret_type operator()(struct_declarator&) = 0;
	virtual ret_type operator()(struct_declarator_list&) = 0;
	virtual ret_type operator()(struct_declaration_c99&) = 0;
	virtual ret_type operator()(unnamed_struct_or_union&) = 0;
	virtual ret_type operator()(struct_declaration_unnamed_sou&) = 0;
	virtual ret_type operator()(struct_declaration_list&) = 0;
	virtual ret_type operator()(struct_or_union_def&) = 0;
	virtual ret_type operator()(struct_or_union_ref&) = 0;
	virtual ret_type operator()(enumerator&) = 0;
	virtual ret_type operator()(enumerator_list&) = 0;
	virtual ret_type operator()(enum_def&) = 0;
	virtual ret_type operator()(enum_ref&) = 0;
	virtual ret_type operator()(typeof_expr&) = 0;
	virtual ret_type operator()(typeof_type_name&) = 0;
	virtual ret_type operator()(function_specifier&) = 0;
	virtual ret_type operator()(specifier_qualifier_list&) = 0;
	virtual ret_type operator()(initializer_expr&) = 0;
	virtual ret_type operator()(initializer_init_list&) = 0;
	virtual ret_type operator()(designator_member&) = 0;
	virtual ret_type operator()(designator_array&) = 0;
	virtual ret_type operator()(designator_list&) = 0;
	virtual ret_type operator()(designation&) = 0;
	virtual ret_type operator()(initializer_list&) = 0;
	virtual ret_type operator()(asm_label&) = 0;
	virtual ret_type operator()(init_declarator&) = 0;
	virtual ret_type operator()(init_declarator_list&) = 0;
	virtual ret_type operator()(declaration&) = 0;
	virtual ret_type operator()(parameter_declaration_declarator&) = 0;
	virtual ret_type operator()(parameter_declaration_abstract&) = 0;
	virtual ret_type operator()(parameter_declaration_list&) = 0;
	virtual ret_type operator()(identifier_list&) = 0;
	virtual ret_type operator()(declaration_list&) = 0;
	virtual ret_type operator()(stmt_labeled&) = 0;
	virtual ret_type operator()(stmt_case&) = 0;
	virtual ret_type operator()(stmt_case_range&) = 0;
	virtual ret_type operator()(stmt_default&) = 0;
	virtual ret_type operator()(local_label_declaration&) = 0;
	virtual ret_type operator()(local_label_declaration_list&) = 0;
	virtual ret_type operator()(block_item_decl&) = 0;
	virtual ret_type operator()(block_item_stmt&) = 0;
	virtual ret_type operator()(block_item_function_definition&) = 0;
	virtual ret_type operator()(block_item_list&) = 0;
	virtual ret_type operator()(stmt_compound&) = 0;
	virtual ret_type operator()(stmt_expr&) = 0;
	virtual ret_type operator()(stmt_if&) = 0;
	virtual ret_type operator()(stmt_switch&) = 0;
	virtual ret_type operator()(stmt_while&) = 0;
	virtual ret_type operator()(stmt_do&) = 0;
	virtual ret_type operator()(stmt_for_init_expr&) = 0;
	virtual ret_type operator()(stmt_for_init_decl&) = 0;
	virtual ret_type operator()(stmt_goto&) = 0;
	virtual ret_type operator()(stmt_continue&) = 0;
	virtual ret_type operator()(stmt_break&) = 0;
	virtual ret_type operator()(stmt_return&) = 0;
	virtual ret_type operator()(stmt_asm&) = 0;
	virtual ret_type operator()(asm_qualifier_list&) = 0;
	virtual ret_type operator()(asm_operand_name&) = 0;
	virtual ret_type operator()(asm_operand&) = 0;
	virtual ret_type operator()(asm_operand_list&) = 0;
	virtual ret_type operator()(asm_clobber_list&) = 0;
	virtual ret_type operator()(asm_jump_to_label_list&) = 0;
	virtual ret_type operator()(asm_directive&) = 0;
	virtual ret_type operator()(function_definition&) = 0;
	virtual ret_type operator()(external_declaration_decl&) = 0;
	virtual ret_type operator()(external_declaration_func&) = 0;
	virtual ret_type operator()(external_declaration_asm&) = 0;
	virtual ret_type operator()(translation_unit&) = 0;
      };

      template<typename ret_type, typename callable_type>
      class _processor : public processor<ret_type>
      {
      public:
	_processor(callable_type &&c) noexcept;

	_processor(const _processor& p) noexcept;

	virtual ~_processor() noexcept override;

	virtual ret_type operator()(expr_list &e) override;
	virtual ret_type operator()(expr_comma &e) override;
	virtual ret_type operator()(expr_assignment &e) override;
	virtual ret_type operator()(expr_conditional &e) override;
	virtual ret_type operator()(expr_binop &e) override;
	virtual ret_type operator()(expr_cast &e) override;
	virtual ret_type operator()(expr_label_addr &e) override;
	virtual ret_type operator()(expr_unop_pre &e) override;
	virtual ret_type operator()(expr_sizeof_expr &e) override;
	virtual ret_type operator()(expr_sizeof_type_name &e) override;
	virtual ret_type operator()(expr_alignof_expr &e) override;
	virtual ret_type operator()(expr_alignof_type_name &e) override;
	virtual ret_type operator()(offset_member_designator&) override;
	virtual ret_type operator()(expr_builtin_offsetof &e) override;
	virtual ret_type operator()(expr_builtin_types_compatible_p &e)
	  override;
	virtual ret_type operator()(expr_builtin_va_arg &e) override;
	virtual ret_type operator()(expr_array_subscript &e) override;
	virtual ret_type operator()(expr_func_invocation &e) override;
	virtual ret_type operator()(expr_member_deref &e) override;
	virtual ret_type operator()(expr_unop_post &e) override;
	virtual ret_type operator()(expr_compound_literal &e) override;
	virtual ret_type operator()(expr_statement &e) override;
	virtual ret_type operator()(expr_id &e) override;
	virtual ret_type operator()(expr_constant &e) override;
	virtual ret_type operator()(string_literal &e) override;
	virtual ret_type operator()(expr_string_literal &e) override;
	virtual ret_type operator()(expr_parenthesized &e) override;
	virtual ret_type operator()(attribute &e) override;
	virtual ret_type operator()(attribute_list &e) override;
	virtual ret_type operator()(attribute_specifier &e) override;
	virtual ret_type operator()(attribute_specifier_list &e) override;
	virtual ret_type operator()(pointer &e) override;
	virtual ret_type operator()(direct_abstract_declarator_parenthesized &e)
	  override;
	virtual ret_type operator()(direct_abstract_declarator_array &e)
	  override;
	virtual ret_type operator()(direct_abstract_declarator_func &e)
	  override;
	virtual ret_type operator()(abstract_declarator &e) override;
	virtual ret_type operator()(type_name &e) override;
	virtual ret_type operator()(direct_declarator_id &e) override;
	virtual ret_type operator()(direct_declarator_parenthesized &e)
	  override;
	virtual ret_type operator()(direct_declarator_array &e) override;
	virtual ret_type operator()(direct_declarator_func &e) override;
	virtual ret_type operator()(declarator &e) override;
	virtual ret_type operator()(storage_class_specifier &e) override;
	virtual ret_type operator()(type_qualifier &e) override;
	virtual ret_type operator()(type_qualifier_list &e) override;
	virtual ret_type operator()(type_specifier_pod &e) override;
	virtual ret_type operator()(type_specifier_tdid &e) override;
	virtual ret_type operator()(struct_declarator &e) override;
	virtual ret_type operator()(struct_declarator_list &e) override;
	virtual ret_type operator()(struct_declaration_c99 &e) override;
	virtual ret_type operator()(unnamed_struct_or_union &e) override;
	virtual ret_type operator()(struct_declaration_unnamed_sou &e) override;
	virtual ret_type operator()(struct_declaration_list &e) override;
	virtual ret_type operator()(struct_or_union_def &e) override;
	virtual ret_type operator()(struct_or_union_ref &e) override;
	virtual ret_type operator()(enumerator &e) override;
	virtual ret_type operator()(enumerator_list &e) override;
	virtual ret_type operator()(enum_def &e) override;
	virtual ret_type operator()(enum_ref &e) override;
	virtual ret_type operator()(typeof_expr &e) override;
	virtual ret_type operator()(typeof_type_name &e) override;
	virtual ret_type operator()(function_specifier &e) override;
	virtual ret_type operator()(specifier_qualifier_list &e) override;
	virtual ret_type operator()(initializer_expr &e) override;
	virtual ret_type operator()(initializer_init_list &e) override;
	virtual ret_type operator()(designator_member &e) override;
	virtual ret_type operator()(designator_array &e) override;
	virtual ret_type operator()(designator_list &e) override;
	virtual ret_type operator()(designation &e) override;
	virtual ret_type operator()(initializer_list &e) override;
	virtual ret_type operator()(asm_label &e) override;
	virtual ret_type operator()(init_declarator &e) override;
	virtual ret_type operator()(init_declarator_list &e) override;
	virtual ret_type operator()(declaration &e) override;
	virtual ret_type operator()(parameter_declaration_declarator &e)
	  override;
	virtual ret_type operator()(parameter_declaration_abstract &e) override;
	virtual ret_type operator()(parameter_declaration_list &e) override;
	virtual ret_type operator()(identifier_list &e) override;
	virtual ret_type operator()(declaration_list &e) override;
	virtual ret_type operator()(stmt_labeled &e) override;
	virtual ret_type operator()(stmt_case &e) override;
	virtual ret_type operator()(stmt_case_range &e) override;
	virtual ret_type operator()(stmt_default &e) override;
	virtual ret_type operator()(local_label_declaration &e) override;
	virtual ret_type operator()(local_label_declaration_list &e) override;
	virtual ret_type operator()(block_item_decl &e) override;
	virtual ret_type operator()(block_item_stmt &e) override;
	virtual ret_type operator()(block_item_function_definition &e) override;
	virtual ret_type operator()(block_item_list &e) override;
	virtual ret_type operator()(stmt_compound &e) override;
	virtual ret_type operator()(stmt_expr &e) override;
	virtual ret_type operator()(stmt_if &e) override;
	virtual ret_type operator()(stmt_switch &e) override;
	virtual ret_type operator()(stmt_while &e) override;
	virtual ret_type operator()(stmt_do &e) override;
	virtual ret_type operator()(stmt_for_init_expr &e) override;
	virtual ret_type operator()(stmt_for_init_decl &e) override;
	virtual ret_type operator()(stmt_goto &e) override;
	virtual ret_type operator()(stmt_continue &e) override;
	virtual ret_type operator()(stmt_break &e) override;
	virtual ret_type operator()(stmt_return &e) override;
	virtual ret_type operator()(stmt_asm &e) override;
	virtual ret_type operator()(asm_qualifier_list &e) override;
	virtual ret_type operator()(asm_operand_name &e) override;
	virtual ret_type operator()(asm_operand &e) override;
	virtual ret_type operator()(asm_operand_list &e) override;
	virtual ret_type operator()(asm_clobber_list &e) override;
	virtual ret_type operator()(asm_jump_to_label_list &e) override;
	virtual ret_type operator()(asm_directive &e) override;
	virtual ret_type operator()(function_definition &e) override;
	virtual ret_type operator()(external_declaration_decl &e) override;
	virtual ret_type operator()(external_declaration_func &e) override;
	virtual ret_type operator()(external_declaration_asm &e) override;
	virtual ret_type operator()(translation_unit &e) override;

      private:
	callable_type _c;
      };

      template<typename return_type, typename callable_type>
      static _processor<return_type, callable_type>
      make_processor(callable_type &&c) noexcept
      {
	return (_processor<return_type, callable_type>
		(std::forward<callable_type>(c)));
      }

      template <typename ret_type>
      class const_processor
      {
      public:
	virtual ~const_processor() noexcept = 0;

	virtual ret_type operator()(const expr_list&) = 0;
	virtual ret_type operator()(const expr_comma&) = 0;
	virtual ret_type operator()(const expr_assignment&) = 0;
	virtual ret_type operator()(const expr_conditional&) = 0;
	virtual ret_type operator()(const expr_binop&) = 0;
	virtual ret_type operator()(const expr_cast&) = 0;
	virtual ret_type operator()(const expr_label_addr&) = 0;
	virtual ret_type operator()(const expr_unop_pre&) = 0;
	virtual ret_type operator()(const expr_sizeof_expr&) = 0;
	virtual ret_type operator()(const expr_sizeof_type_name&) = 0;
	virtual ret_type operator()(const expr_alignof_expr&) = 0;
	virtual ret_type operator()(const expr_alignof_type_name&) = 0;
	virtual ret_type operator()(const offset_member_designator&) = 0;
	virtual ret_type operator()(const expr_builtin_offsetof&) = 0;
	virtual ret_type operator()(const expr_builtin_types_compatible_p&) = 0;
	virtual ret_type operator()(const expr_builtin_va_arg&) = 0;
	virtual ret_type operator()(const expr_array_subscript&) = 0;
	virtual ret_type operator()(const expr_func_invocation&) = 0;
	virtual ret_type operator()(const expr_member_deref&) = 0;
	virtual ret_type operator()(const expr_unop_post&) = 0;
	virtual ret_type operator()(const expr_compound_literal&) = 0;
	virtual ret_type operator()(const expr_statement&) = 0;
	virtual ret_type operator()(const expr_id&) = 0;
	virtual ret_type operator()(const expr_constant&) = 0;
	virtual ret_type operator()(const string_literal&) = 0;
	virtual ret_type operator()(const expr_string_literal&) = 0;
	virtual ret_type operator()(const expr_parenthesized&) = 0;
	virtual ret_type operator()(const attribute&) = 0;
	virtual ret_type operator()(const attribute_list&) = 0;
	virtual ret_type operator()(const attribute_specifier&) = 0;
	virtual ret_type operator()(const attribute_specifier_list&) = 0;
	virtual ret_type operator()(const pointer&) = 0;
	virtual ret_type
	operator()(const direct_abstract_declarator_parenthesized&) = 0;
	virtual ret_type operator()(const direct_abstract_declarator_array&)
	  = 0;
	virtual ret_type operator()(const direct_abstract_declarator_func&) = 0;
	virtual ret_type operator()(const abstract_declarator&) = 0;
	virtual ret_type operator()(const type_name&) = 0;
	virtual ret_type operator()(const direct_declarator_id&) = 0;
	virtual ret_type operator()(const direct_declarator_parenthesized&) = 0;
	virtual ret_type operator()(const direct_declarator_array&) = 0;
	virtual ret_type operator()(const direct_declarator_func&) = 0;
	virtual ret_type operator()(const declarator&) = 0;
	virtual ret_type operator()(const storage_class_specifier&) = 0;
	virtual ret_type operator()(const type_qualifier&) = 0;
	virtual ret_type operator()(const type_qualifier_list&) = 0;
	virtual ret_type operator()(const type_specifier_pod&) = 0;
	virtual ret_type operator()(const type_specifier_tdid&) = 0;
	virtual ret_type operator()(const struct_declarator&) = 0;
	virtual ret_type operator()(const struct_declarator_list&) = 0;
	virtual ret_type operator()(const struct_declaration_c99&) = 0;
	virtual ret_type operator()(const unnamed_struct_or_union&) = 0;
	virtual ret_type operator()(const struct_declaration_unnamed_sou&) = 0;
	virtual ret_type operator()(const struct_declaration_list&) = 0;
	virtual ret_type operator()(const struct_or_union_def&) = 0;
	virtual ret_type operator()(const struct_or_union_ref&) = 0;
	virtual ret_type operator()(const enumerator&) = 0;
	virtual ret_type operator()(const enumerator_list&) = 0;
	virtual ret_type operator()(const enum_def&) = 0;
	virtual ret_type operator()(const enum_ref&) = 0;
	virtual ret_type operator()(const typeof_expr&) = 0;
	virtual ret_type operator()(const typeof_type_name&) = 0;
	virtual ret_type operator()(const function_specifier&) = 0;
	virtual ret_type operator()(const specifier_qualifier_list&) = 0;
	virtual ret_type operator()(const initializer_expr&) = 0;
	virtual ret_type operator()(const initializer_init_list&) = 0;
	virtual ret_type operator()(const designator_member&) = 0;
	virtual ret_type operator()(const designator_array&) = 0;
	virtual ret_type operator()(const designator_list&) = 0;
	virtual ret_type operator()(const designation&) = 0;
	virtual ret_type operator()(const initializer_list&) = 0;
	virtual ret_type operator()(const asm_label&) = 0;
	virtual ret_type operator()(const init_declarator&) = 0;
	virtual ret_type operator()(const init_declarator_list&) = 0;
	virtual ret_type operator()(const declaration&) = 0;
	virtual ret_type operator()(const parameter_declaration_declarator&)
	  = 0;
	virtual ret_type operator()(const parameter_declaration_abstract&) = 0;
	virtual ret_type operator()(const parameter_declaration_list&) = 0;
	virtual ret_type operator()(const identifier_list&) = 0;
	virtual ret_type operator()(const declaration_list&) = 0;
	virtual ret_type operator()(const stmt_labeled&) = 0;
	virtual ret_type operator()(const stmt_case&) = 0;
	virtual ret_type operator()(const stmt_case_range&) = 0;
	virtual ret_type operator()(const stmt_default&) = 0;
	virtual ret_type operator()(const local_label_declaration&) = 0;
	virtual ret_type operator()(const local_label_declaration_list&) = 0;
	virtual ret_type operator()(const block_item_decl&) = 0;
	virtual ret_type operator()(const block_item_stmt&) = 0;
	virtual ret_type operator()(const block_item_function_definition&) = 0;
	virtual ret_type operator()(const block_item_list&) = 0;
	virtual ret_type operator()(const stmt_compound&) = 0;
	virtual ret_type operator()(const stmt_expr&) = 0;
	virtual ret_type operator()(const stmt_if&) = 0;
	virtual ret_type operator()(const stmt_switch&) = 0;
	virtual ret_type operator()(const stmt_while&) = 0;
	virtual ret_type operator()(const stmt_do&) = 0;
	virtual ret_type operator()(const stmt_for_init_expr&) = 0;
	virtual ret_type operator()(const stmt_for_init_decl&) = 0;
	virtual ret_type operator()(const stmt_goto&) = 0;
	virtual ret_type operator()(const stmt_continue&) = 0;
	virtual ret_type operator()(const stmt_break&) = 0;
	virtual ret_type operator()(const stmt_return&) = 0;
	virtual ret_type operator()(const stmt_asm&) = 0;
	virtual ret_type operator()(const asm_qualifier_list&) = 0;
	virtual ret_type operator()(const asm_operand_name&) = 0;
	virtual ret_type operator()(const asm_operand&) = 0;
	virtual ret_type operator()(const asm_operand_list&) = 0;
	virtual ret_type operator()(const asm_clobber_list&) = 0;
	virtual ret_type operator()(const asm_jump_to_label_list&) = 0;
	virtual ret_type operator()(const asm_directive&) = 0;
	virtual ret_type operator()(const function_definition&) = 0;
	virtual ret_type operator()(const external_declaration_decl&) = 0;
	virtual ret_type operator()(const external_declaration_func&) = 0;
	virtual ret_type operator()(const external_declaration_asm&) = 0;
	virtual ret_type operator()(const translation_unit&) = 0;
      };

      template<typename ret_type, typename callable_type>
      class _const_processor : public const_processor<ret_type>
      {
      public:
	_const_processor(callable_type &&c) noexcept;

	_const_processor(const _const_processor& p) noexcept;

	virtual ~_const_processor() noexcept override;

	virtual ret_type operator()(const expr_list &e) override;
	virtual ret_type operator()(const expr_comma &e) override;
	virtual ret_type operator()(const expr_assignment &e) override;
	virtual ret_type operator()(const expr_conditional &e) override;
	virtual ret_type operator()(const expr_binop &e) override;
	virtual ret_type operator()(const expr_cast &e) override;
	virtual ret_type operator()(const expr_label_addr &e) override;
	virtual ret_type operator()(const expr_unop_pre &e) override;
	virtual ret_type operator()(const expr_sizeof_expr &e) override;
	virtual ret_type operator()(const expr_sizeof_type_name &e) override;
	virtual ret_type operator()(const expr_alignof_expr &e) override;
	virtual ret_type operator()(const expr_alignof_type_name &e) override;
	virtual ret_type operator()(const offset_member_designator&) override;
	virtual ret_type operator()(const expr_builtin_offsetof &e) override;
	virtual ret_type operator()(const expr_builtin_types_compatible_p &e)
	  override;
	virtual ret_type operator()(const expr_builtin_va_arg &e) override;
	virtual ret_type operator()(const expr_array_subscript &e) override;
	virtual ret_type operator()(const expr_func_invocation &e) override;
	virtual ret_type operator()(const expr_member_deref &e) override;
	virtual ret_type operator()(const expr_unop_post &e) override;
	virtual ret_type operator()(const expr_compound_literal &e) override;
	virtual ret_type operator()(const expr_statement &e) override;
	virtual ret_type operator()(const expr_id &e) override;
	virtual ret_type operator()(const expr_constant &e) override;
	virtual ret_type operator()(const string_literal &e) override;
	virtual ret_type operator()(const expr_string_literal &e) override;
	virtual ret_type operator()(const expr_parenthesized &e) override;
	virtual ret_type operator()(const attribute &e) override;
	virtual ret_type operator()(const attribute_list &e) override;
	virtual ret_type operator()(const attribute_specifier &e) override;
	virtual ret_type operator()(const attribute_specifier_list &e) override;
	virtual ret_type operator()(const pointer &e) override;
	virtual ret_type
	operator()(const direct_abstract_declarator_parenthesized &e) override;
	virtual ret_type operator()(const direct_abstract_declarator_array &e)
	  override;
	virtual ret_type operator()(const direct_abstract_declarator_func &e)
	  override;
	virtual ret_type operator()(const abstract_declarator &e) override;
	virtual ret_type operator()(const type_name &e) override;
	virtual ret_type operator()(const direct_declarator_id &e) override;
	virtual ret_type operator()(const direct_declarator_parenthesized &e)
	  override;
	virtual ret_type operator()(const direct_declarator_array &e) override;
	virtual ret_type operator()(const direct_declarator_func &e) override;
	virtual ret_type operator()(const declarator &e) override;
	virtual ret_type operator()(const storage_class_specifier &e) override;
	virtual ret_type operator()(const type_qualifier &e) override;
	virtual ret_type operator()(const type_qualifier_list &e) override;
	virtual ret_type operator()(const type_specifier_pod &e) override;
	virtual ret_type operator()(const type_specifier_tdid &e) override;
	virtual ret_type operator()(const struct_declarator &e) override;
	virtual ret_type operator()(const struct_declarator_list &e) override;
	virtual ret_type operator()(const struct_declaration_c99 &e) override;
	virtual ret_type operator()(const unnamed_struct_or_union &e) override;
	virtual ret_type operator()(const struct_declaration_unnamed_sou &e)
	  override;
	virtual ret_type operator()(const struct_declaration_list &e) override;
	virtual ret_type operator()(const struct_or_union_def &e) override;
	virtual ret_type operator()(const struct_or_union_ref &e) override;
	virtual ret_type operator()(const enumerator &e) override;
	virtual ret_type operator()(const enumerator_list &e) override;
	virtual ret_type operator()(const enum_def &e) override;
	virtual ret_type operator()(const enum_ref &e) override;
	virtual ret_type operator()(const typeof_expr &e) override;
	virtual ret_type operator()(const typeof_type_name &e) override;
	virtual ret_type operator()(const function_specifier &e) override;
	virtual ret_type operator()(const specifier_qualifier_list &e) override;
	virtual ret_type operator()(const initializer_expr &e) override;
	virtual ret_type operator()(const initializer_init_list &e) override;
	virtual ret_type operator()(const designator_member &e) override;
	virtual ret_type operator()(const designator_array &e) override;
	virtual ret_type operator()(const designator_list &e) override;
	virtual ret_type operator()(const designation &e) override;
	virtual ret_type operator()(const initializer_list &e) override;
	virtual ret_type operator()(const asm_label &e) override;
	virtual ret_type operator()(const init_declarator &e) override;
	virtual ret_type operator()(const init_declarator_list &e) override;
	virtual ret_type operator()(const declaration &e) override;
	virtual ret_type operator()(const parameter_declaration_declarator &e)
	  override;
	virtual ret_type operator()(const parameter_declaration_abstract &e)
	  override;
	virtual ret_type operator()(const parameter_declaration_list &e)
	  override;
	virtual ret_type operator()(const identifier_list &e) override;
	virtual ret_type operator()(const declaration_list &e) override;
	virtual ret_type operator()(const stmt_labeled &e) override;
	virtual ret_type operator()(const stmt_case &e) override;
	virtual ret_type operator()(const stmt_case_range &e) override;
	virtual ret_type operator()(const stmt_default &e) override;
	virtual ret_type operator()(const local_label_declaration &e) override;
	virtual ret_type operator()(const local_label_declaration_list &e)
	  override;
	virtual ret_type operator()(const block_item_decl &e) override;
	virtual ret_type operator()(const block_item_stmt &e) override;
	virtual ret_type operator()(const block_item_function_definition &e)
	  override;
	virtual ret_type operator()(const block_item_list &e) override;
	virtual ret_type operator()(const stmt_compound &e) override;
	virtual ret_type operator()(const stmt_expr &e) override;
	virtual ret_type operator()(const stmt_if &e) override;
	virtual ret_type operator()(const stmt_switch &e) override;
	virtual ret_type operator()(const stmt_while &e) override;
	virtual ret_type operator()(const stmt_do &e) override;
	virtual ret_type operator()(const stmt_for_init_expr &e) override;
	virtual ret_type operator()(const stmt_for_init_decl &e) override;
	virtual ret_type operator()(const stmt_goto &e) override;
	virtual ret_type operator()(const stmt_continue &e) override;
	virtual ret_type operator()(const stmt_break &e) override;
	virtual ret_type operator()(const stmt_return &e) override;
	virtual ret_type operator()(const stmt_asm &e) override;
	virtual ret_type operator()(const asm_qualifier_list &e) override;
	virtual ret_type operator()(const asm_operand_name &e) override;
	virtual ret_type operator()(const asm_operand &e) override;
	virtual ret_type operator()(const asm_operand_list &e) override;
	virtual ret_type operator()(const asm_clobber_list &e) override;
	virtual ret_type operator()(const asm_jump_to_label_list &e) override;
	virtual ret_type operator()(const asm_directive &e) override;
	virtual ret_type operator()(const function_definition &e) override;
	virtual ret_type operator()(const external_declaration_decl &e)
	  override;
	virtual ret_type operator()(const external_declaration_func &e)
	  override;
	virtual ret_type operator()(const external_declaration_asm &e) override;
	virtual ret_type operator()(const translation_unit &e) override;

      private:
	callable_type _c;
      };

      template<typename return_type, typename callable_type>
      static _const_processor<return_type, callable_type>
      make_const_processor(callable_type &&c) noexcept
      {
	return (_const_processor<return_type, callable_type>
		(std::forward<callable_type>(c)));
      }
    }
  }
}

#endif
