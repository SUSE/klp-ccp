#ifndef AST_PROCESSOR_HH
#define AST_PROCESSOR_HH

#include <utility>
#include "ast.hh"

namespace suse
{
  namespace cp
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
	callable_type &&_c;
      };

      template<typename return_type, typename callable_type>
      static _processor<return_type, callable_type>
      make_processor(callable_type &&c) noexcept
      {
	return (_processor<return_type, callable_type>
		(std::forward<callable_type>(c)));
      }
    }
  }
}

#endif
