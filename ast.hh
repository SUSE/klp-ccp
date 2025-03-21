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

#ifndef AST_HH
#define AST_HH

#include <vector>
#include <cassert>
#include <initializer_list>
#include <functional>
#include <memory>
#include <utility>
#include "type_set.hh"
#include "pp_tokens.hh"
#include "code_remarks.hh"
#include "types.hh"
#include "builtins.hh"
#include "pp_tokens_range.hh"
#include "pp_result.hh"

namespace klp
{
  namespace ccp
  {
    class target;

    namespace ast
    {
      template<typename ret_type>
      class processor;

      template<typename ret_type>
      class const_processor;

      class constexpr_value;

      class ast;

      template<typename derived>
      class ast_entity;

      class _ast_entity
      {
      public:
	_ast_entity(const pp_tokens_range &tr) noexcept;

	_ast_entity(const _ast_entity &ae) noexcept;

	virtual ~_ast_entity() noexcept;

	const pp_tokens_range& get_tokens_range() const noexcept
	{ return _tokens_range; }

	_ast_entity* get_parent() noexcept
	{ return _parent; }

	const _ast_entity* get_parent() const noexcept
	{ return _parent; }

	template <typename ret_type, typename handled_types,
		  typename callables_wrapper_type>
	ret_type process(callables_wrapper_type &&c) const;

	template <typename ret_type, typename handled_types,
		  typename callables_wrapper_type>
	ret_type process(callables_wrapper_type &&c);

	template<typename... types>
	bool is_any_of() const noexcept;

	template <typename handled_types_pre,
		  typename handled_types_post,
		  typename callables_wrapper_type_pre,
		  typename callables_wrapper_type_post>
	void for_each_dfs_pre_and_po(callables_wrapper_type_pre &&c_pre,
				     callables_wrapper_type_post &&c_post);

	template <typename handled_types_pre,
		  typename handled_types_post,
		  typename callables_wrapper_type_pre,
		  typename callables_wrapper_type_post>
	void for_each_dfs_pre_and_po(callables_wrapper_type_pre &&c_pre,
				     callables_wrapper_type_post &&c_post)
	  const;

	template <typename handled_types, typename callables_wrapper_type>
	void for_each_dfs_po(callables_wrapper_type &&c);

	template <typename handled_types, typename callables_wrapper_type>
	void for_each_dfs_po(callables_wrapper_type &&c) const;

      protected:
	void _extend_tokens_range(const pp_tokens_range &tr) noexcept;

	_ast_entity *_parent;

      private:
	template<typename derived>
	friend class ast_entity;

	virtual _ast_entity* _get_child(const size_t i) const noexcept = 0;

	virtual void _process(processor<void> &p) = 0;
	virtual void _process(const_processor<void> &p) const = 0;
	virtual bool _process(processor<bool> &p) = 0;
	virtual bool _process(const_processor<bool> &p) const = 0;

	template<typename callable_type_pre,
		 typename callable_type_post>
	void _for_each_dfs_pre_and_po(callable_type_pre &&c_pre,
				      callable_type_post &&c_post);

	template<typename callable_type_pre,
		 typename callable_type_post>
	void _for_each_dfs_pre_and_po(callable_type_pre &&c_pre,
				      callable_type_post &&c_post) const;

	template <typename callable_type>
	void _for_each_dfs_po(callable_type &&c);

	template <typename callable_type>
	void _for_each_dfs_po(callable_type &&c) const;

	pp_tokens_range _tokens_range;
      };


      template <typename derived>
      class ast_entity : public _ast_entity
      {
      public:
	ast_entity(const pp_tokens_range &tr) noexcept;

	ast_entity(const ast_entity &ae) noexcept;

	virtual ~ast_entity() noexcept = default;


	template<typename boundary_type_set, typename callable_type>
	void for_each_ancestor(callable_type &&c);

	template<typename boundary_type_set, typename callable_type>
	void for_each_ancestor(callable_type &&c) const;

	template <typename callable_type>
	void process_parent(callable_type &&c) const;

	template <typename parent_type>
	const parent_type& get_unique_parent() const noexcept
	{
	  static_assert((derived::parent_types::size() == 1 &&
			 derived::parent_types::template is_member<parent_type>()),
			"parent type not unique");
	  assert(_parent);
	  return dynamic_cast<const parent_type&>(*_parent);
	}

	template <typename parent_type>
	parent_type& get_unique_parent() noexcept
	{
	  static_assert((derived::parent_types::size() == 1 &&
			 derived::parent_types::template is_member<parent_type>()),
			"parent type not unique");
	  assert(_parent);
	  return dynamic_cast<parent_type&>(*_parent);
	}

	template<typename parent_type>
	void _set_parent(parent_type &p) noexcept
	{
	  static_assert(
		derived::parent_types::template is_member<parent_type>(),
		"attempted to set non-listed parent");
	  assert(!_parent);
	  _parent = &p;
	}

	template<typename parent_type>
	void _reset_parent(parent_type &p, const parent_type &old_p) noexcept
	{
	  static_assert(
		derived::parent_types::template is_member<parent_type>(),
		"attempted to set non-listed parent");
	  assert(_parent == &old_p);
	  _parent = &p;
	}
      };


      class _typed
      {
      public:
	virtual ~_typed() noexcept;

	virtual void evaluate_type(ast &a, const target &tgt) = 0;

	virtual bool is_evaluated() const noexcept = 0;

      protected:
	_typed() noexcept = default;
      };


      template <typename derived, typename type>
      class typed_ast_entity : public ast_entity<derived>, public _typed
      {
      public:
	virtual ~typed_ast_entity() noexcept;

	const std::shared_ptr<const type>& get_type() const noexcept;

	virtual bool is_evaluated() const noexcept override;

      protected:
	typed_ast_entity(const pp_tokens_range &tr) noexcept;

	void _set_type(std::shared_ptr<const type> &&t) noexcept;
	void _set_type(const std::shared_ptr<const type> &t) noexcept;
	void _reset_type(std::shared_ptr<const type> &&t) noexcept;

      private:
	std::shared_ptr<const type> _type;
      };

      class ast;
      class expr;
      class expr_list;
      class attribute;
      class attribute_list;
      class attribute_specifier;
      class storage_class_specifier;
      class type_qualifier;
      class type_qualifier_list;
      class type_specifier;
      class struct_declaration;
      class struct_declaration_list;
      class enumerator;
      class enumerator_list;
      class function_specifier;
      class pointer;
      class abstract_declarator;
      class type_name;
      class direct_declarator;
      class direct_declarator_parenthesized;
      class direct_declarator_id;
      class declarator;
      class initializer;
      class designator;
      class designator_list;
      class designation;
      class asm_label;
      class init_declarator_list;
      class declaration;
      class static_assertion;
      class parameter_declaration;
      class parameter_declaration_declarator;
      class parameter_declaration_abstract;
      class identifier_list;
      class declaration_list;
      class stmt;
      class local_label_declaration;
      class block_item;
      class block_item_list;
      class asm_qualifier_list;
      class asm_operand_name;
      class asm_operand;
      class asm_operand_list;
      class asm_clobber_list;
      class asm_jump_to_label_list;
      class function_definition;
      class translation_unit;
      class external_declaration;
      class external_declaration_asm;
      class expr;
      class expr_list;
      class expr_comma;
      class expr_assignment;
      class expr_conditional;
      class expr_binop;
      class expr_cast;
      class expr_unop_pre;
      class expr_sizeof_expr;
      class expr_alignof_expr;
      class generic_association;
      class generic_association_list;
      class expr_generic;
      class expr_builtin_choose_expr;
      class offset_member_designator;
      class expr_builtin_offsetof;
      class expr_builtin_va_arg;
      class expr_sizeof_type_name;
      class expr_array_subscript;
      class expr_func_invocation;
      class expr_member_deref;
      class expr_unop_post;
      class expr_compound_literal;
      class expr_statement;
      class expr_id;
      class expr_constant;
      class expr_string_literal;
      class expr_parenthesized;
      class attribute;
      class attribute_list;
      class attribute_specifier;
      class attribute_specifier_list;
      class storage_class_specifier;
      class type_qualifier;
      class type_qualifier_list;
      class type_specifier;
      class type_specifier_pod;
      class struct_declaration;
      class struct_declarator;
      class struct_declarator_list;
      class struct_declaration_c99;
      class unnamed_struct_or_union;
      class struct_declaration_unnamed_sou;
      class struct_declaration_static_assert;
      class struct_declaration_list;
      class struct_or_union_def;
      class struct_or_union_ref;
      class enumerator;
      class enumerator_list;
      class enum_def;
      class enum_ref;
      class typeof_expr;
      class typeof_type_name;
      class function_specifier;
      class specifier_qualifier_list;
      class declaration_specifiers;
      class pointer;
      class direct_abstract_declarator;
      class direct_abstract_declarator_parenthesized;
      class direct_abstract_declarator_array;
      class direct_abstract_declarator_func;
      class abstract_declarator;
      class type_name;
      class direct_declarator;
      class direct_declarator_array;
      class direct_declarator_func;
      class declarator;
      class initializer;
      class initializer_expr;
      class designator;
      class designator_member;
      class designator_array;
      class designator_list;
      class designation;
      class initializer_list;
      class asm_label;
      class init_declarator;
      class init_declarator_list;
      class declaration;
      class parameter_declaration;
      class parameter_declaration_list;
      class identifier_list;
      class declaration_list;
      class stmt;
      class stmt_labeled;
      class stmt_case;
      class stmt_case_range;
      class stmt_default;
      class local_label_declaration;
      class local_label_declaration_list;
      class block_item;
      class block_item_decl;
      class block_item_static_assert;
      class block_item_stmt;
      class block_item_function_definition;
      class block_item_list;
      class stmt_compound;
      class stmt_expr;
      class stmt_if;
      class stmt_switch;
      class stmt_while;
      class stmt_do;
      class stmt_for;
      class stmt_for_init_expr;
      class stmt_for_init_decl;
      class stmt_for_init_static_assert;
      class stmt_goto;
      class stmt_continue;
      class stmt_break;
      class stmt_return;
      class asm_qualifier_list;
      class asm_operand_name;
      class asm_operand;
      class asm_operand_list;
      class asm_clobber_list;
      class asm_jump_to_label_list;
      class asm_directive;
      class function_definition;
      class translation_unit;
      class external_declaration;
      class external_declaration_decl;
      class external_declaration_static_assert;
      class external_declaration_func;

      class expr_list final : public ast_entity<expr_list>
      {
      public:
	typedef type_set<attribute, expr_func_invocation> parent_types;

	expr_list(expr* &&e);

	expr_list(expr_list &&el);

	virtual ~expr_list() noexcept override;

	void extend(expr* &&e);

	std::size_t size() const noexcept
	{ return _exprs.size(); }

	expr& operator[](const std::size_t i) noexcept;

	const expr& operator[](const std::size_t i) const noexcept;

	void apply_lvalue_conversions(const bool only_decay);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<expr> > _exprs;
      };

      class expr : public typed_ast_entity<expr, types::type>
      {
      public:
	typedef type_set<expr_list,
			 expr_cast, expr_unop_pre,
			 expr_sizeof_expr, expr_alignof_expr,
			 generic_association, expr_generic,
			 expr_builtin_choose_expr,
			 offset_member_designator, expr_array_subscript,
			 expr_func_invocation, expr_member_deref,
			 expr_unop_post, expr_parenthesized, expr_comma,
			 expr_assignment, expr_conditional, expr_binop,
			 expr_builtin_va_arg,
			 direct_abstract_declarator_array,
			 direct_declarator_array,
			 struct_declarator,
			 enumerator,
			 typeof_expr,
			 initializer_expr,
			 designator_array,
			 static_assertion,
			 stmt_case, stmt_case_range,
			 stmt_expr, stmt_if, stmt_switch,
			 stmt_while, stmt_do,
			 stmt_for, stmt_for_init_expr,
			 stmt_goto, stmt_return,
			 asm_operand> parent_types;

	expr(expr&&) = delete;

	virtual ~expr() noexcept = 0;

	bool is_constexpr() const noexcept;

	bool is_dereferenced_const_addr() const noexcept;

	const constexpr_value&
	get_constexpr_value() const noexcept;

	virtual const expr& skip_parens_down() const noexcept;
	virtual const _ast_entity* skip_parens_up() const noexcept;
	virtual const _ast_entity* skip_parens_up() noexcept;
	const _ast_entity* get_non_parens_parent() const noexcept;
	_ast_entity* get_non_parens_parent() noexcept;

	bool is_lvalue() const noexcept;

	void apply_lvalue_conversion(const bool only_decay);

      protected:
	expr(const pp_tokens_range &tr) noexcept;

	void _set_value(std::unique_ptr<constexpr_value> &&cv);

	template<typename... args_types>
	void _set_value(args_types&&... args);

	void _set_lvalue(const bool is_lvalue) noexcept;

      private:
	std::unique_ptr<constexpr_value> _value;
	bool _is_lvalue;
      };

      class expr_comma final : public expr
      {
      public:
	expr_comma(expr* &&l, expr* &&r) noexcept;

	virtual ~expr_comma() noexcept override;

	virtual void evaluate_type(ast&, const target&) override;

	const expr& get_left() const noexcept
	{ return _left; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_left;
	expr &_right;
      };

      enum class assign_op
      {
	set,
	mul,
	div,
	mod,
	add,
	sub,
	lshift,
	rshift,
	bin_and,
	bin_xor,
	bin_or,
      };

      class expr_assignment final : public expr
      {
      public:
	expr_assignment(const assign_op op, expr* &&lhs, expr* &&rhs) noexcept;

	virtual ~expr_assignment() noexcept override;

	assign_op get_op() const noexcept
	{ return _op; }

	const expr& get_lhs() const noexcept
	{ return _lhs; }

	const expr& get_rhs() const noexcept
	{ return _rhs; }

	virtual void evaluate_type(ast &a, const target &tgt) override;


      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	assign_op _op;
	expr &_lhs;
	expr &_rhs;
      };

      class expr_conditional final : public expr
      {
      public:
	expr_conditional(expr* &&cond, expr* &&expr_true, expr* &&expr_false)
	  noexcept;

	expr_conditional(expr* &&cond, expr* &&expr_false) noexcept;

	virtual ~expr_conditional() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const expr& get_cond() const noexcept
	{ return _cond; }

	const expr* get_expr_true() const noexcept
	{ return _expr_true; }

	const expr& get_expr_false() const noexcept
	{ return _expr_false; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_cond;
	expr *_expr_true;
	expr &_expr_false;
      };

      enum class binary_op
      {
	mul,
	div,
	mod,
	add,
	sub,
	lshift,
	rshift,
	bin_and,
	bin_xor,
	bin_or,
	logical_and,
	logical_or,
	eq,
	neq,
	lt,
	gt,
	leq,
	geq,
      };

      class expr_binop final : public expr
      {
      public:
	expr_binop(const binary_op op, expr* &&l, expr* &&r) noexcept;

	virtual ~expr_binop() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	binary_op get_op() const noexcept
	{ return _op; }

	const expr& get_left_expr() const noexcept
	{ return _left; }

	const expr& get_right_expr() const noexcept
	{ return _right; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	void _evaluate_arith_binop(const types::arithmetic_type &at_left,
				   const types::arithmetic_type &at_right,
				   ast &a, const target &tgt);
	void _evaluate_ptr_sub(const types::pointer_type &pt_left,
			       const types::pointer_type &pt_right,
			       ast &a, const target &tgt);
	void _evaluate_shift(const types::int_type &it_left,
			     const types::int_type &it_right,
			     ast &a, const target &tgt);
	void _evaluate_bin_binop(const types::int_type &it_left,
				 const types::int_type &it_right,
				 ast &a, const target &tgt);
	void _evaluate_logical_binop(const ast &a, const target &tgt);

	void _evaluate_cmp(const types::pointer_type &pt_left,
			   const types::pointer_type &pt_right,
			   ast &a, const target &tgt);
	void _evaluate_cmp(const types::pointer_type &pt_left,
			   const types::int_type &it_right,
			   ast &a, const target &tgt);
	void _evaluate_cmp(const types::int_type &it_left,
			   const types::pointer_type &pt_right,
			   ast &a, const target &tgt);
	void _evaluate_cmp(const types::arithmetic_type &at_left,
			   const types::arithmetic_type &at_right,
			   ast &a, const target &tgt);

	binary_op _op;
	expr &_left;
	expr &_right;
      };

      class expr_cast final : public expr
      {
      public:
	expr_cast(const pp_tokens_range &tr, type_name* &&tn, expr* &&e)
	  noexcept;

	virtual ~expr_cast() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const type_name& get_type_name() const noexcept
	{ return _tn; }

	const expr& get_expr() const noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name &_tn;
	expr &_e;
      };

      enum class unary_op_pre
      {
	inc,
	dec,
	addr,
	deref,
	plus,
	neg,
	bin_neg,
	logical_not,
      };

      class expr_label_addr final : public expr
      {
      public:
	expr_label_addr(const pp_tokens_range &tr,
			const pp_token_index label_tok) noexcept;

	virtual ~expr_label_addr() noexcept override;

	pp_token_index get_label_tok() const noexcept
	{ return _label_tok; }

	void set_resolved(const stmt_labeled &resolved) noexcept;

	virtual void evaluate_type(ast&, const target&) override;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _label_tok;
	const stmt_labeled *_resolved;
      };

      class expr_unop_pre final : public expr
      {
      public:
	expr_unop_pre(const pp_tokens_range &tr,
		      const unary_op_pre op, expr* &&e) noexcept;

	virtual ~expr_unop_pre() noexcept override;

	unary_op_pre get_op() const noexcept
	{ return _op; }

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const expr& get_expr() const noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	unary_op_pre _op;
	expr &_e;
      };

      class expr_sizeof_expr final : public expr
      {
      public:
	expr_sizeof_expr(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~expr_sizeof_expr() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
      };

      class expr_sizeof_type_name final : public expr
      {
      public:
	expr_sizeof_type_name(const pp_tokens_range &tr, type_name* &&tn)
	  noexcept;

	virtual ~expr_sizeof_type_name() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const type_name& get_type_name() const noexcept
	{ return _tn; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name &_tn;
      };

      class expr_alignof_expr final : public expr
      {
      public:
	expr_alignof_expr(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~expr_alignof_expr() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
      };

      class expr_alignof_type_name final : public expr
      {
      public:
	expr_alignof_type_name(const pp_tokens_range &tr, type_name* &&tn)
	  noexcept;

	virtual ~expr_alignof_type_name() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const type_name& get_type_name() const noexcept
	{ return _tn; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name &_tn;
      };

      class generic_association final : public ast_entity<generic_association>
      {
      public:
	typedef type_set<generic_association_list> parent_types;

	generic_association(const pp_tokens_range &tr,
			    type_name* &&tn, expr *&&e) noexcept;

	generic_association(const pp_tokens_range &tr, expr *&&e) noexcept;

	generic_association(generic_association&&) = delete;

	virtual ~generic_association() noexcept override;

	const type_name *get_type_name() const noexcept
	{ return _tn; }

	const expr& get_expr() const noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name *_tn;
	expr &_e;
      };

      class generic_association_list final :
	public ast_entity<generic_association_list>
      {
      public:
	typedef type_set<expr_generic> parent_types;

	generic_association_list(generic_association *&&ga);

	generic_association_list(generic_association_list&&) = delete;

	virtual ~generic_association_list() noexcept override;

	void extend(generic_association *&&ga);

	template <typename callable_type>
	void for_each(callable_type &&c) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<generic_association> > _gas;
      };

      class expr_generic final : public expr
      {
      public:
	expr_generic(const pp_tokens_range &tr,
		     expr *&&ctrl_e,
		     generic_association_list *&&gal) noexcept;

	virtual ~expr_generic() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const expr& get_control_expr() const noexcept
	{ return _ctrl_e; }

	const generic_association_list& get_association_list() const noexcept
	{ return _gal; }

	const expr& get_match() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_ctrl_e;
	generic_association_list &_gal;
	const expr *_match;
      };

      class expr_builtin_choose_expr final : public expr
      {
      public:
	expr_builtin_choose_expr(const pp_tokens_range &tr, expr *&&cond,
				 expr *&&expr_true, expr *&&expr_false)
	  noexcept;

	virtual ~expr_builtin_choose_expr() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const expr& get_cond() const noexcept
	{ return _cond; }

	const expr& get_expr_true() const noexcept
	{ return _expr_true; }

	const expr& get_expr_false() const noexcept
	{ return _expr_false; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_cond;
	expr &_expr_true;
	expr &_expr_false;
      };

      class offset_member_designator
	: public ast_entity<offset_member_designator>
      {
      public:
	struct member
	{
	  member(const pp_token_index _member_tok, const bool _ptr_base)
	    noexcept;

	  pp_token_index member_tok;
	  bool ptr_base;
	};

	typedef type_set<expr_builtin_offsetof> parent_types;

	offset_member_designator(const pp_token_index member_tok);

	virtual ~offset_member_designator() noexcept override;

	void extend(const pp_token_index member_tok, const bool ptr_base);
	void extend(expr* &&index_expr);

	template <typename callable_type>
	void for_each(callable_type &&c) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	class component
	{
	public:
	  enum class kind
	  {
	    k_member,
	    k_array_subscript,
	  };

	  component(const member &m) noexcept;
	  component(expr &index_expr) noexcept;

	  component(const component&) = delete;
	  component(component &&c) noexcept;

	  ~component() noexcept;

	  kind get_kind() const noexcept
	  { return _k; }

	  const member& get_member() const noexcept;

	  expr& get_index_expr() noexcept;
	  const expr& get_index_expr() const noexcept;

	private:
	  kind _k;
	  union
	  {
	    member _m;
	    expr * _index_expr;
	  };
	};

	std::vector<component> _components;
      };

      class expr_builtin_offsetof final : public expr
      {
      public:
	expr_builtin_offsetof(const pp_tokens_range &tr, type_name* &&tn,
			      offset_member_designator* &&member_designator)
	  noexcept;

	virtual ~expr_builtin_offsetof() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const type_name& get_type_name() const noexcept
	{ return _tn; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name &_tn;
	offset_member_designator &_member_designator;
      };

      class expr_builtin_types_compatible_p final : public expr
      {
      public:
	expr_builtin_types_compatible_p(const pp_tokens_range &tr,
					type_name* &&tn1, type_name* &&tn2)
	  noexcept;

	virtual ~expr_builtin_types_compatible_p() noexcept override;

	virtual void evaluate_type(ast&, const target &tgt) override;

	const type_name& get_type_name1() const noexcept
	{ return _tn1; }

	const type_name& get_type_name2() const noexcept
	{ return _tn2; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name &_tn1;
	type_name &_tn2;
      };

      class expr_builtin_va_arg final : public expr
      {
      public:
	expr_builtin_va_arg(const pp_tokens_range &tr,
			    expr* &&e, type_name* &&tn) noexcept;

	virtual ~expr_builtin_va_arg() noexcept override;

	virtual void evaluate_type(ast&, const target&) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
	type_name &_tn;
      };

      class expr_array_subscript final : public expr
      {
      public:
	expr_array_subscript(const pp_tokens_range &tr,
			     expr* &&base, expr* &&index) noexcept;

	virtual ~expr_array_subscript() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const expr& get_base() const noexcept
	{ return _base; }

	const expr& get_index() const noexcept
	{ return _index; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	void _evaluate_type(ast &a, const target &tgt,
			    const types::pointer_type &pt_base,
			    const expr &e_base, const expr &e_index);

	expr &_base;
	expr &_index;
      };

      class expr_func_invocation final : public expr
      {
      public:
	expr_func_invocation(const pp_tokens_range &tr,
			     expr* &&func, expr_list* &&args) noexcept;

	expr_func_invocation(const pp_tokens_range &tr, expr* &&func) noexcept;

	virtual ~expr_func_invocation() noexcept override;

	const expr& get_func() const noexcept
	{ return _func; }

	const expr_list * get_args() const noexcept
	{ return _args; }

	expr_list * get_args() noexcept
	{ return _args; }

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_func;
	expr_list *_args;
      };

      enum class member_deref_type
      {
	ptr_base,
	non_ptr_base,
      };

      class expr_member_deref final : public expr
      {
      public:
	expr_member_deref(const pp_tokens_range &tr,
			  const member_deref_type deref_type,
			  expr* &&base, const pp_token_index member_tok)
	  noexcept;

	virtual ~expr_member_deref() noexcept override;

	member_deref_type get_deref_type() const noexcept
	{ return _deref_type; }

	virtual void evaluate_type(ast &a, const target&) override;

	const expr& get_base() const noexcept
	{ return _base; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	member_deref_type _deref_type;
	expr &_base;
	pp_token_index _member_tok;
      };

      enum class unary_op_post
      {
	inc,
	dec,
      };

      class expr_unop_post final : public expr
      {
      public:
	expr_unop_post(const pp_tokens_range &tr,
		       const unary_op_post op, expr* &&e) noexcept;

	virtual ~expr_unop_post() noexcept override;

	unary_op_post get_op() const noexcept
	{ return _op; }

	const expr& get_expr() const noexcept
	{ return _e; }

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	unary_op_post _op;
	expr &_e;
      };

      class expr_compound_literal final : public expr
      {
      public:
	expr_compound_literal(const pp_tokens_range &tr,
			      type_name* &&tn, initializer_list* &&il) noexcept;

	virtual ~expr_compound_literal() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const type_name& get_type_name() const noexcept
	{ return _tn; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name &_tn;
	initializer_list *_il;
      };

      class expr_statement final : public expr
      {
      public:
	expr_statement(const pp_tokens_range &tr, stmt_compound* &&s) noexcept;

	virtual ~expr_statement() noexcept override;

	virtual void evaluate_type(ast&, const target&) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	stmt_compound &_s;
      };

      class expr_id final : public expr
      {
      public:
	class resolved
	{
	public:
	  resolved() noexcept;
	  resolved(const builtin_func::factory &builtin_func_fac) noexcept;
	  resolved(const builtin_var::factory builtin_var_fac) noexcept;
	  resolved(init_declarator &id) noexcept;
	  resolved(parameter_declaration_declarator &pdd) noexcept;
	  resolved(function_definition &fd) noexcept;
	  resolved(stmt_labeled &sl) noexcept;
	  resolved(enumerator &e) noexcept;
	  resolved(identifier_list &pil) noexcept;

	  enum class resolved_kind
	  {
	    none,
	    builtin_func,
	    builtin_var,
	    init_declarator,
	    parameter_declaration_declarator,
	    function_definition,
	    stmt_labeled,
	    enumerator,
	    in_param_id_list,
	  };

	  resolved_kind get_kind() const noexcept
	  { return _kind; }

	  const builtin_func::factory& get_builtin_func_factory()
	    const noexcept;
	  builtin_var::factory get_builtin_var_factory() const noexcept;
	  init_declarator& get_init_declarator() const noexcept;
	  parameter_declaration_declarator&
	  get_parameter_declaration_declarator() const noexcept;
	  function_definition& get_function_definition() const noexcept;
	  stmt_labeled& get_stmt_labeled() const noexcept;
	  enumerator& get_enumerator() const noexcept;
	  identifier_list& get_param_id_list() const noexcept;

	private:
	  resolved_kind _kind;

	  union
	  {
	    const builtin_func::factory *_builtin_func_fac;
	    builtin_var::factory _builtin_var_fac;
	    init_declarator *_id;
	    parameter_declaration_declarator *_pdd;
	    function_definition *_fd;
	    stmt_labeled *_sl;
	    enumerator *_e;
	    identifier_list *_pil;
	  };
	};

	expr_id(const pp_token_index id_tok) noexcept;

	virtual ~expr_id() noexcept override;

	pp_token_index get_id_tok() const noexcept
	{ return _id_tok; }

	void set_resolved(const resolved &r) noexcept;

	const resolved& get_resolved() const noexcept;

	bool is_resolved() const noexcept
	{ return _resolved.get_kind() != resolved::resolved_kind::none; }

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _id_tok;
	resolved _resolved;
      };

      class expr_constant final : public expr
      {
      public:
	expr_constant(const pp_token_index const_tok) noexcept;

	virtual ~expr_constant() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _const_tok;
      };

      class string_literal final : public ast_entity<string_literal>
      {
      public:
	typedef type_set<expr_string_literal,
			 static_assertion,
			 asm_directive,
			 asm_label,
			 asm_operand,
			 asm_clobber_list> parent_types;

	string_literal(const pp_token_index s);

	virtual ~string_literal() noexcept override;

	void extend(const pp_token_index s);

	const std::vector<pp_token_index>& get_strings() const noexcept
	{ return _strings; }

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<pp_token_index> _strings;
      };

      class expr_string_literal final : public expr
      {
      public:
	expr_string_literal(string_literal* &&sl) noexcept;

	virtual ~expr_string_literal() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	string_literal &_sl;
      };

      class expr_parenthesized final : public expr
      {
      public:
	expr_parenthesized(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~expr_parenthesized() noexcept override;

	virtual const expr& skip_parens_down() const noexcept override;
	virtual const _ast_entity* skip_parens_up() const noexcept override;
	virtual _ast_entity* skip_parens_up() noexcept override;

	virtual void evaluate_type(ast&, const target&) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
      };


      class attribute final : public ast_entity<attribute>
      {
      public:
	typedef type_set<attribute_list> parent_types;

	attribute(const pp_token_index name_tok) noexcept;
	attribute(const pp_tokens_range &tr, const pp_token_index name_tok,
		  expr_list* &&params) noexcept;

	virtual ~attribute() noexcept override;

	pp_token_index get_name_tok() const noexcept
	{ return _name_tok; }

	expr_list* get_params() noexcept
	{ return _params; }

	const expr_list* get_params() const noexcept
	{ return _params; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _name_tok;
	expr_list *_params;
      };

      class attribute_list final : public ast_entity<attribute_list>
      {
      public:
	typedef type_set<attribute_specifier> parent_types;

	attribute_list(const pp_tokens_range &tr, attribute* &&a);

	virtual ~attribute_list() noexcept override;

	void extend(const pp_tokens_range &tr, attribute* &&a);

	template <typename callable_type>
	bool for_each_attribute(callable_type &&c);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<attribute*> _attributes;
      };

      class attribute_specifier final : public ast_entity<attribute_specifier>
      {
      public:
	typedef type_set<attribute_specifier_list> parent_types;

      public:
	attribute_specifier(const pp_tokens_range &tr,
			    attribute_list* &&al) noexcept;

	virtual ~attribute_specifier() noexcept override;

	attribute_list& get_attribute_list()
	{ return _al; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	attribute_list &_al;
      };

      class attribute_specifier_list final
	: public ast_entity<attribute_specifier_list>
      {
      public:
	typedef type_set<direct_abstract_declarator_parenthesized,
			 direct_declarator_parenthesized,
			 struct_declarator,
			 unnamed_struct_or_union,
			 struct_or_union_def, struct_or_union_ref,
			 enum_def, enum_ref,
			 type_qualifier_list,
			 specifier_qualifier_list,
			 init_declarator,
			 stmt_labeled,
			 function_definition,
			 parameter_declaration_declarator,
			 parameter_declaration_abstract> parent_types;

	attribute_specifier_list(attribute_specifier* &&as);

	virtual ~attribute_specifier_list() noexcept override;

	void extend(attribute_specifier* &&as);

	template <typename callable_type>
	bool for_each_attribute(callable_type &&c);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<attribute_specifier> > _ass;
      };

      class pointer final : public ast_entity<pointer>
      {
      public:
	typedef type_set<abstract_declarator, declarator> parent_types;

	pointer(const pp_tokens_range &tr, type_qualifier_list* &&tql);

	virtual ~pointer() noexcept override;

	void extend(const pp_tokens_range &tr, type_qualifier_list* &&tql);

	const std::vector<type_qualifier_list*>&
	get_type_qualifier_lists() const noexcept
	{ return _tqls; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<type_qualifier_list*> _tqls;
      };

      class direct_abstract_declarator
	: public typed_ast_entity<direct_abstract_declarator,
				  types::addressable_type>
      {
      public:
	typedef type_set<abstract_declarator,
			 direct_abstract_declarator_array,
			 direct_abstract_declarator_func> parent_types;

	virtual ~direct_abstract_declarator() noexcept;

	virtual std::shared_ptr<const types::addressable_type>
	get_innermost_type() const noexcept = 0;

      protected:
	direct_abstract_declarator(const pp_tokens_range &tr) noexcept;

	std::shared_ptr<const types::addressable_type>
	_get_enclosing_type() const noexcept;
      };

      class direct_abstract_declarator_parenthesized final
	: public direct_abstract_declarator
      {
      public:
	direct_abstract_declarator_parenthesized(
					const pp_tokens_range &tr,
					attribute_specifier_list* &&asl,
					abstract_declarator* &&ad) noexcept;

	virtual ~direct_abstract_declarator_parenthesized() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	virtual std::shared_ptr<const types::addressable_type>
	get_innermost_type() const noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	attribute_specifier_list *_asl;
	abstract_declarator &_ad;
      };

      class direct_abstract_declarator_array final
	: public direct_abstract_declarator
      {
      public:
	direct_abstract_declarator_array(const pp_tokens_range &tr,
					 direct_abstract_declarator* &&dad,
					 type_qualifier_list* &&tql,
					 expr* &&size,
					 const bool is_static) noexcept;

	class vla_unspec_size_tag {};
	direct_abstract_declarator_array(const pp_tokens_range &tr,
					 direct_abstract_declarator* &&dad,
					 const vla_unspec_size_tag&) noexcept;

	virtual ~direct_abstract_declarator_array() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	virtual std::shared_ptr<const types::addressable_type>
	get_innermost_type() const noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	direct_abstract_declarator *_dad;
	type_qualifier_list *_tql;
	expr *_size;
	bool _static;
	bool _vla_unspec_size;
      };

      class direct_abstract_declarator_func final
	: public direct_abstract_declarator
      {
      public:
	direct_abstract_declarator_func(const pp_tokens_range &tr,
					direct_abstract_declarator* &&dad,
					parameter_declaration_list* &&ptl)
	  noexcept;

	virtual ~direct_abstract_declarator_func() noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	virtual std::shared_ptr<const types::addressable_type>
	get_innermost_type() const noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	direct_abstract_declarator *_dad;
	parameter_declaration_list *_ptl;
      };

      class abstract_declarator final
	: public typed_ast_entity<abstract_declarator, types::addressable_type>
      {
      public:
	typedef type_set<type_name, direct_abstract_declarator_parenthesized,
			 parameter_declaration_abstract> parent_types;

	abstract_declarator(const pp_tokens_range &tr, pointer* &&pt,
			    direct_abstract_declarator* &&dad) noexcept;

	virtual ~abstract_declarator() noexcept override;

	const direct_abstract_declarator*
	get_direct_abstract_declarator() const noexcept
	{
	  return _dad;
	}

	const pointer* get_pointer() const noexcept
	{
	  return _pt;
	}

	virtual void evaluate_type(ast &a, const target &tgt) override;

	std::shared_ptr<const types::addressable_type>
	get_innermost_type() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::shared_ptr<const types::addressable_type>
	_get_enclosing_type() const noexcept;

	pointer *_pt;
	direct_abstract_declarator *_dad;
      };

      class type_name final
	: public typed_ast_entity<type_name, types::addressable_type>
      {
      public:
	typedef type_set<typeof_type_name, expr_sizeof_type_name,
			 expr_alignof_type_name,
			 generic_association,
			 expr_builtin_offsetof,
			 expr_builtin_types_compatible_p,
			 expr_builtin_va_arg,
			 expr_cast, expr_compound_literal,
			 typeof_type_name> parent_types;

	type_name(const pp_tokens_range &tr, specifier_qualifier_list* &&sql,
		  abstract_declarator* &&ad) noexcept;

	virtual ~type_name() noexcept override;

	const specifier_qualifier_list& get_specifier_qualifier_list()
	  const noexcept
	{ return _sql; }

	specifier_qualifier_list& get_specifier_qualifier_list() noexcept
	{ return _sql; }

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	specifier_qualifier_list &_sql;
	abstract_declarator *_ad;
      };

      class direct_declarator
	: public typed_ast_entity<direct_declarator, types::addressable_type>
      {
      public:
	typedef type_set<declarator, direct_declarator_array,
			 direct_declarator_func> parent_types;

	virtual ~direct_declarator() noexcept = 0;

	virtual  const direct_declarator_id& get_direct_declarator_id()
	  const noexcept = 0;

	virtual direct_declarator_id& get_direct_declarator_id() noexcept = 0;

	const _ast_entity& get_first_non_declarator_parent() const noexcept;

      protected:
	direct_declarator(const pp_tokens_range &tr) noexcept;

	std::shared_ptr<const types::addressable_type>
	_get_enclosing_type() const noexcept;
      };

      class direct_declarator_id final : public direct_declarator
      {
      public:
	direct_declarator_id(const pp_token_index id_tok) noexcept;

	virtual ~direct_declarator_id() noexcept override;

	virtual const direct_declarator_id& get_direct_declarator_id()
	  const noexcept override;

	virtual direct_declarator_id& get_direct_declarator_id()
	  noexcept override;

	pp_token_index get_id_tok() const noexcept
	{ return _id_tok; }

	const direct_declarator_func* is_function() const noexcept;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	void deduce_auto_type(ast &a, const target &tgt,
			      init_declarator &id);

	void complete_type(std::shared_ptr<const types::array_type> &&at);

	template <typename ret_type, typename callables_wrapper_type>
	ret_type process_context(callables_wrapper_type &&c) const;

	template <typename ret_type, typename callables_wrapper_type>
	ret_type process_context(callables_wrapper_type &&c);

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _id_tok;
      };

      class direct_declarator_parenthesized final : public direct_declarator
      {
      public:
	direct_declarator_parenthesized(const pp_tokens_range &tr,
					declarator* &&d,
					attribute_specifier_list* &&asl)
	  noexcept;

	virtual ~direct_declarator_parenthesized() noexcept override;

	virtual const direct_declarator_id& get_direct_declarator_id()
	  const noexcept override;

	virtual direct_declarator_id& get_direct_declarator_id()
	  noexcept override;

	const declarator& get_declarator() const noexcept
	{
	  return _d;
	}

	virtual void evaluate_type(ast &a, const target &tgt) override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declarator &_d;
	attribute_specifier_list *_asl;
      };

      class direct_declarator_array final : public direct_declarator
      {
      public:
	direct_declarator_array(const pp_tokens_range &tr,
				direct_declarator* &&dd,
				type_qualifier_list* &&tql,
				expr* &&size,
				const bool is_static) noexcept;

	class vla_unspec_size_tag {};
	direct_declarator_array(const pp_tokens_range &tr,
				direct_declarator* &&dd,
				type_qualifier_list* &&tql,
				const vla_unspec_size_tag&) noexcept;

	virtual ~direct_declarator_array() noexcept override;

	virtual const direct_declarator_id& get_direct_declarator_id()
	  const noexcept override;

	virtual direct_declarator_id& get_direct_declarator_id()
	  noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const type_qualifier_list* get_type_qualifier_list() const noexcept
	{ return _tql; }

	const expr* get_size() const noexcept
	{ return _size; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	direct_declarator &_dd;
	type_qualifier_list *_tql;
	expr *_size;
	bool _static;
	bool _vla_unspec_size;
      };

      class direct_declarator_func final : public direct_declarator
      {
      public:
	direct_declarator_func(const pp_tokens_range &tr,
			       direct_declarator* &&dd,
			       parameter_declaration_list* &&ptl)
	  noexcept;

	direct_declarator_func(const pp_tokens_range &tr,
			       direct_declarator* &&dd,
			       identifier_list* &&il)
	  noexcept;

	virtual ~direct_declarator_func() noexcept override;

	virtual const direct_declarator_id& get_direct_declarator_id()
	  const noexcept override;

	virtual direct_declarator_id& get_direct_declarator_id()
	  noexcept override;

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const parameter_declaration_list* get_parameter_declaration_list()
	  const noexcept
	{ return _ptl; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	direct_declarator &_dd;
	parameter_declaration_list *_ptl;
	identifier_list *_il;
      };

      class declarator final
	: public typed_ast_entity<declarator, types::addressable_type>
      {
      public:
	typedef type_set<direct_declarator_parenthesized,
			 struct_declarator,
			 init_declarator,
			 parameter_declaration_declarator,
			 function_definition> parent_types;

	declarator(const pp_tokens_range &tr, pointer* &&pt,
		   direct_declarator* &&dd) noexcept;

	virtual ~declarator() noexcept override;

	const direct_declarator_id& get_direct_declarator_id() const noexcept
	{ return _cached_ddid; }

	direct_declarator_id& get_direct_declarator_id() noexcept
	{ return _cached_ddid; }

	pp_token_index get_id_tok() const noexcept;

	const pointer* get_pointer() const noexcept
	{
	  return _pt;
	}

	virtual void evaluate_type(ast &a, const target &tgt) override;

	std::shared_ptr<const types::addressable_type>
	get_innermost_type() const noexcept;

      private:
	friend class direct_declarator_id;

	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::shared_ptr<const types::addressable_type>
	_get_enclosing_type() const noexcept;

	void _set_deduced_auto_type
	  (std::shared_ptr<const types::addressable_type> &&t) noexcept;

	pointer *_pt;
	direct_declarator &_dd;
	direct_declarator_id &_cached_ddid;
      };

      enum class storage_class
      {
	sc_none,
	sc_typedef,
	sc_extern,
	sc_static,
	sc_auto,
	sc_register,
      };

      class storage_class_specifier final
	: public ast_entity<storage_class_specifier>
      {
      public:
	typedef type_set<specifier_qualifier_list> parent_types;

      public:
	storage_class_specifier(const pp_tokens_range &tr,
				const storage_class sc) noexcept;

	virtual ~storage_class_specifier() noexcept override;

	storage_class get_storage_class() const
	{ return _sc; }

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	storage_class _sc;
      };

      enum class type_qualifier_type
      {
	tqt_const,
	tqt_restrict,
	tqt_volatile,
      };

      class type_qualifier final : public ast_entity<type_qualifier>
      {
      public:
	typedef type_set<specifier_qualifier_list,
			 type_qualifier_list> parent_types;

      public:
	type_qualifier(const pp_tokens_range &tr,
		       const types::qualifier q) noexcept;

	virtual ~type_qualifier() noexcept override;

	types::qualifier get_kind() const noexcept
	{ return _q; }

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	types::qualifier _q;
      };

      class type_qualifier_list final : public ast_entity<type_qualifier_list>
      {
      public:
	typedef type_set<pointer,
			 direct_abstract_declarator_array,
			 direct_declarator_array> parent_types;

	type_qualifier_list(type_qualifier* &&tq);

	type_qualifier_list(attribute_specifier_list* &&asl);

	virtual ~type_qualifier_list() noexcept override;

	void extend(type_qualifier* &&tq);

	void extend(attribute_specifier_list* &&asl);

	const types::qualifiers
	get_qualifiers() const;

	template <typename callable_type>
	bool for_each_attribute(callable_type &&c);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<type_qualifier> > _tqs;
	std::vector<std::reference_wrapper<attribute_specifier_list> > _asls;
      };

      class type_specifier : public ast_entity<type_specifier>
      {
      public:
	typedef type_set<specifier_qualifier_list> parent_types;

	type_specifier(const pp_tokens_range &tr) noexcept;

	virtual ~type_specifier() noexcept = 0;
      };

      enum class pod_spec
      {
	ps_void,
	ps_char,
	ps_short,
	ps_int,
	ps_long,
	ps_float,
	ps_double,
	ps_signed,
	ps_unsigned,
	ps_bool,
	ps_complex,
      };

      class type_specifier_pod final : public type_specifier
      {
      public:
	type_specifier_pod(const pp_tokens_range &tr,
			   const pod_spec &pod_spec) noexcept;

	virtual ~type_specifier_pod() noexcept override;

	pod_spec get_pod_spec() const noexcept
	{
	  return _pod_spec;
	}

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pod_spec _pod_spec;
      };

      class type_specifier_ext_int final : public type_specifier
      {
      public:
	type_specifier_ext_int(const pp_tokens_range &tr,
			       const types::ext_int_type::kind &k) noexcept;

	virtual ~type_specifier_ext_int() noexcept override;

	types::ext_int_type::kind get_ext_int_kind() const noexcept
	{
	  return _k;
	}

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	types::ext_int_type::kind _k;
      };

      class type_specifier_ext_float final : public type_specifier
      {
      public:
	type_specifier_ext_float(const pp_tokens_range &tr,
			       const types::ext_float_type::kind &k) noexcept;

	virtual ~type_specifier_ext_float() noexcept override;

	types::ext_float_type::kind get_ext_float_kind() const noexcept
	{
	  return _k;
	}

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	types::ext_float_type::kind _k;
      };

      class type_specifier_tdid final : public type_specifier
      {
      public:
	class resolved
	{
	public:
	  resolved() noexcept;
	  resolved(const builtin_typedef::factory &fac) noexcept;
	  resolved(const init_declarator &id) noexcept;

	  enum class resolved_kind
	  {
	    none,
	    builtin_typedef,
	    init_declarator,
	  };

	  resolved_kind get_kind() const noexcept
	  { return _kind; }

	  const builtin_typedef::factory& get_builtin_typedef_factory()
	    const noexcept;
	  const init_declarator& get_init_declarator() const noexcept;

	private:
	  resolved_kind _kind;
	  union
	  {
	    const builtin_typedef::factory *_builtin_typedef_fac;
	    const init_declarator *_id;
	  };
	};

	type_specifier_tdid(const pp_token_index tdid_tok) noexcept;

	virtual ~type_specifier_tdid() noexcept override;

	pp_token_index get_id_tok() const noexcept
	{ return _tdid_tok; }

	void set_resolved(const resolved &r) noexcept;

	const resolved& get_resolved() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _tdid_tok;
	resolved _resolved;
      };

      class type_specifier_auto_type final : public type_specifier
      {
      public:
	type_specifier_auto_type(const pp_token_index auto_type_tok) noexcept;

	virtual ~type_specifier_auto_type() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;
      };

      class struct_declaration : public ast_entity<struct_declaration>
      {
      public:
	typedef type_set<struct_declaration_list> parent_types;

	struct_declaration(const pp_tokens_range &tr) noexcept;

	virtual ~struct_declaration() noexcept = 0;

	virtual void mark_last_member() noexcept = 0;
      };

      class struct_declaration_decl : public struct_declaration
      {
      public:
	struct_declaration_decl(const pp_tokens_range &tr,
				specifier_qualifier_list* &&sql) noexcept;

	virtual ~struct_declaration_decl() noexcept = 0;

	specifier_qualifier_list* get_specifier_qualifier_list() noexcept
	{ return _sql; }

	const specifier_qualifier_list* get_specifier_qualifier_list()
	  const noexcept
	{ return _sql; }

      protected:
	specifier_qualifier_list *_sql;
      };

      class struct_declarator final : public ast_entity<struct_declarator>
      {
      public:
	typedef type_set<struct_declarator_list> parent_types;

	struct_declarator(const pp_tokens_range &tr,
			  declarator* &&d, expr* &&width,
			  attribute_specifier_list* &&asl_after) noexcept;

	virtual ~struct_declarator() noexcept override;

	void set_asl_before(attribute_specifier_list* &&asl_before) noexcept;

	const expr* get_width() const noexcept
	{ return _width; }

	expr* get_width() noexcept
	{ return _width; }

	const declarator* get_declarator() const noexcept
	{ return _d; }

	attribute_specifier_list* get_asl_before() noexcept
	{
	  return _asl_before;
	}

	attribute_specifier_list* get_asl_after() noexcept
	{
	  return _asl_after;
	}

	bool is_last() const noexcept
	{ return _is_last; }

	void mark_last_member() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declarator *_d;
	expr *_width;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
	bool _is_last;
      };

      class struct_declarator_list final
	: public ast_entity<struct_declarator_list>
      {
      public:
	typedef type_set<struct_declaration_c99> parent_types;

	struct_declarator_list(struct_declarator* &&sd);

	virtual ~struct_declarator_list() noexcept override;

	void extend(struct_declarator* &&sd);

	bool empty() const noexcept;

	void mark_last_member() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<struct_declarator> > _sds;
      };

      class struct_declaration_c99 final : public struct_declaration_decl
      {
      public:
	struct_declaration_c99(const pp_tokens_range &tr,
			       specifier_qualifier_list* &&sql,
			       struct_declarator_list * &&sdl) noexcept;

	virtual ~struct_declaration_c99() noexcept override;

	const struct_declarator_list& get_struct_declarator_list()
	  const noexcept
	{ return _sdl; }

	virtual void mark_last_member() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	struct_declarator_list &_sdl;
      };

      class unnamed_struct_or_union final
	: public ast_entity<unnamed_struct_or_union>
      {
      public:
	typedef type_set<struct_declaration_unnamed_sou> parent_types;

	unnamed_struct_or_union(const pp_tokens_range &tr,
				const types::struct_or_union_kind souk,
				struct_declaration_list* &&sdl,
				attribute_specifier_list* &&asl_before,
				attribute_specifier_list* &&asl_after) noexcept;

	virtual ~unnamed_struct_or_union() noexcept override;

	types::struct_or_union_kind get_tag_kind() const noexcept
	{ return _souk; }

	attribute_specifier_list* get_asl_before() noexcept
	{ return _asl_before; }

	attribute_specifier_list* get_asl_after() noexcept
	{ return _asl_after; }

	const types::struct_or_union_content& get_content() const noexcept;

	void
	set_content(std::unique_ptr<const types::struct_or_union_content> &&c)
	  noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	types::struct_or_union_kind _souk;
	struct_declaration_list *_sdl;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;

	std::unique_ptr<const types::struct_or_union_content> _content;
      };

      class struct_declaration_unnamed_sou final :
	public struct_declaration_decl
      {
      public:
	struct_declaration_unnamed_sou(const pp_tokens_range &tr,
				       specifier_qualifier_list* &&sql,
				       unnamed_struct_or_union* &&unnamed_sou)
	  noexcept;

	virtual ~struct_declaration_unnamed_sou() noexcept override;

	const unnamed_struct_or_union& get_unnamed_struct_or_union()
	  const noexcept
	{ return _unnamed_sou; }

	virtual void mark_last_member() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	unnamed_struct_or_union &_unnamed_sou;
      };

      class struct_declaration_static_assert final : public struct_declaration
      {
      public:
	struct_declaration_static_assert(static_assertion* &&sa) noexcept;

	virtual ~struct_declaration_static_assert() noexcept override;

	virtual void mark_last_member() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	static_assertion &_sa;
      };

      class struct_declaration_list final
	: public ast_entity<struct_declaration_list>
      {
      public:
	typedef type_set<struct_or_union_def,
			 unnamed_struct_or_union> parent_types;

      public:
	struct_declaration_list(struct_declaration* &&sd);

	virtual ~struct_declaration_list() noexcept override;

	void extend(struct_declaration* &&sd);

	void mark_last_member() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<struct_declaration> > _sds;
      };

      class sou_decl_link
      {
	public:
	  sou_decl_link() noexcept;
	  sou_decl_link(struct_or_union_ref &sour) noexcept;
	  sou_decl_link(struct_or_union_def &soud) noexcept;

	  bool operator==(const sou_decl_link &rhs) const noexcept;

	  enum class target_kind
	  {
	    unlinked,
	    ref,
	    def,
	  };

	  target_kind get_target_kind() const noexcept
	  { return _target_kind; }

	  struct_or_union_ref& get_target_sou_ref() const noexcept;
	  struct_or_union_def& get_target_sou_def() const noexcept;

	  const sou_decl_list_node& get_target_decl_list_node() const noexcept;

	private:
	  target_kind _target_kind;

	  union {
	    struct_or_union_ref *_sour;
	    struct_or_union_def *_soud;
	  };
      };

      class sou_decl_list_node;

      class sou_id
      {
      public:
	sou_id() noexcept
	: _first_node(nullptr)
	{}

	sou_id(const sou_id &id) noexcept
	: _first_node(id._first_node)
	{}

	sou_id& operator=(const sou_id &id) noexcept
	{
	  _first_node = id._first_node;
	  return *this;
	}

	bool operator==(const sou_id op) const noexcept
	{
	  return this->_first_node == op._first_node;
	}

	bool operator!=(const sou_id op) const noexcept
	{
	  return this->_first_node != op._first_node;
	}

	bool operator<(const sou_id op) const noexcept
	{
	  return this->_first_node < op._first_node;
	}

      private:
	friend class sou_decl_list_node;

	sou_id(const sou_decl_list_node *first_node) noexcept
	  : _first_node(first_node)
	{}

	const sou_decl_list_node *_first_node;
      };

      class sou_decl_list_node
      {
      public:
	sou_decl_list_node(struct_or_union_ref &self) noexcept;
	sou_decl_list_node(struct_or_union_def &self) noexcept;

	void link_to(struct_or_union_ref &target) noexcept;
	void link_to(struct_or_union_def &target) noexcept;

	const sou_decl_link& get_next() const noexcept
	{ return _next; }

	template <typename callable_type>
	bool for_each(callable_type &&c) const;

	const sou_id get_id() const noexcept;

	struct_or_union_def* find_definition() const noexcept;

	bool is_first() const noexcept
	{ return _first_in_list; }

	bool is_visible_from(const sou_decl_list_node &from) const noexcept;

      private:
	template <typename target_type>
	void __link_to(target_type &target) noexcept;

	const sou_decl_list_node &_find_decl_node() const noexcept;
	const sou_decl_list_node &_find_first_decl_node() const noexcept;

	sou_decl_link _next;
	sou_decl_link *_prev;
	bool _first_in_list;
      };


      class struct_or_union_def final : public type_specifier
      {
      public:
	struct_or_union_def(const pp_tokens_range &tr,
			    const types::struct_or_union_kind souk,
			    const pp_token_index id_tok,
			    struct_declaration_list* &&sdl,
			    attribute_specifier_list* &&asl_before,
			    attribute_specifier_list* &&asl_after) noexcept;

	struct_or_union_def(const pp_tokens_range &tr,
			    const types::struct_or_union_kind souk,
			    struct_declaration_list* &&sdl,
			    attribute_specifier_list* &&asl_before,
			    attribute_specifier_list* &&asl_after) noexcept;

	virtual ~struct_or_union_def() noexcept override;

	bool has_id() const noexcept
	{ return _id_tok_valid; }

	pp_token_index get_id_tok() const noexcept
	{
	  assert(_id_tok_valid);
	  return _id_tok;
	}

	types::struct_or_union_kind get_tag_kind() const noexcept
	{ return _souk; }

	const sou_decl_list_node& get_decl_list_node() const noexcept
	{ return _decl_list_node; }

	sou_decl_list_node& get_decl_list_node() noexcept
	{ return _decl_list_node; }

	const struct_declaration_list*
	get_struct_declaration_list() const noexcept
	{ return _sdl; }

	attribute_specifier_list *get_asl_before() noexcept
	{ return _asl_before; }

	const attribute_specifier_list *get_asl_before() const noexcept
	{ return _asl_before; }

	attribute_specifier_list *get_asl_after() noexcept
	{ return _asl_after; }

	const types::struct_or_union_content* get_content() const noexcept;

	void
	set_content(std::unique_ptr<const types::struct_or_union_content> &&c)
	  noexcept;

      private:
	struct_or_union_def(const pp_tokens_range &tr,
			    const types::struct_or_union_kind souk,
			    const pp_token_index id_tok,
			    struct_declaration_list* &&sdl,
			    attribute_specifier_list* &&asl_before,
			    attribute_specifier_list* &&asl_after,
			    const bool id_tok_valid) noexcept;

	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	types::struct_or_union_kind _souk;
	pp_token_index _id_tok;
	struct_declaration_list *_sdl;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
	sou_decl_list_node _decl_list_node;
	bool _id_tok_valid;

	std::unique_ptr<const types::struct_or_union_content> _content;
      };

      class struct_or_union_ref final : public type_specifier
      {
      public:
	struct_or_union_ref(const pp_tokens_range &tr,
			    const types::struct_or_union_kind souk,
			    const pp_token_index id_tok,
			    attribute_specifier_list* &&asl) noexcept;

	virtual ~struct_or_union_ref() noexcept override;

	pp_token_index get_id_tok() const noexcept
	{ return _id_tok; }

	types::struct_or_union_kind get_tag_kind() const noexcept
	{ return _souk; }

	const sou_decl_list_node& get_decl_list_node() const noexcept
	{ return _decl_list_node; }

	sou_decl_list_node& get_decl_list_node() noexcept
	{ return _decl_list_node; }

	const sou_decl_link& get_link_to_decl() const noexcept
	{ return _link_to_decl; }

	void link_to_declaration(const sou_decl_link &target) noexcept;

	bool is_standalone_decl() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	types::struct_or_union_kind _souk;
	pp_token_index _id_tok;
	attribute_specifier_list *_asl;
	sou_decl_list_node _decl_list_node;
	sou_decl_link _link_to_decl;
      };


      template <typename callable_type>
      bool sou_decl_list_node::for_each(callable_type &&c) const
      {
	const sou_decl_list_node *cur = this;
	do {
	  bool r;
	  switch (cur->_next.get_target_kind()) {
	  case sou_decl_link::target_kind::ref:
	    r = c(cur->_next.get_target_sou_ref());
	    cur = &cur->_next.get_target_sou_ref().get_decl_list_node();
	    break;

	  case sou_decl_link::target_kind::def:
	    r = c(cur->_next.get_target_sou_def());
	    cur = &cur->_next.get_target_sou_def().get_decl_list_node();
	    break;

	  case sou_decl_link::target_kind::unlinked:
	    assert(0);
	    __builtin_unreachable();
	  }

	  if (!r)
	    return false;
	} while (cur != this);

	return true;
      }


      class enumerator final : public ast_entity<enumerator>
      {
      public:
	typedef type_set<enumerator_list> parent_types;

	enumerator(const pp_tokens_range &tr, const pp_token_index id_tok,
		   expr* &&value) noexcept;

	virtual ~enumerator() noexcept override;

	pp_token_index get_id_tok() const noexcept
	{ return _id_tok; }

	void register_at_parent(ast &a, const target &tgt);

	const types::enum_content::member& to_member(const ast &a)
	  const noexcept;

	expr* get_value_expr() noexcept
	{ return _value; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _id_tok;
	expr *_value;
      };

      class enumerator_list final : public ast_entity<enumerator_list>
      {
      public:
	typedef type_set<enum_def> parent_types;

	enumerator_list(enumerator* &&e);

	virtual ~enumerator_list() noexcept override;

	void extend(enumerator* &&e);

	const types::enum_content& get_content() const noexcept
	{ return _content; }

	types::enum_content& get_content() noexcept
	{ return _content; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<enumerator> > _es;

	types::enum_content _content;
      };

      class enum_decl_list_node;

      class enum_decl_link
      {
      public:
	enum_decl_link() noexcept;
	enum_decl_link(enum_ref &er) noexcept;
	enum_decl_link (enum_def &ed) noexcept;

	bool operator==(const enum_decl_link &rhs) const noexcept;

	enum class target_kind
	{
	  unlinked,
	  ref,
	  def,
	};

	target_kind get_target_kind() const noexcept
	{ return _target_kind; }

	enum_ref& get_target_enum_ref() const noexcept;

	enum_def& get_target_enum_def() const noexcept;

	const enum_decl_list_node& get_target_decl_list_node() const noexcept;

      private:
	target_kind _target_kind;

	union {
	  enum_ref *_er;
	  enum_def *_ed;
	};
      };

      class enum_id
      {
      public:
	enum_id() noexcept
	: _first_node(nullptr)
	{}

	enum_id(const enum_id &id) noexcept
	: _first_node(id._first_node)
	{}

	enum_id& operator=(const enum_id &id) noexcept
	{
	  _first_node = id._first_node;
	  return *this;
	}

	bool operator==(const enum_id op) const noexcept
	{
	  return this->_first_node == op._first_node;
	}

	bool operator!=(const enum_id op) const noexcept
	{
	  return this->_first_node != op._first_node;
	}

	bool operator<(const enum_id op) const noexcept
	{
	  return this->_first_node < op._first_node;
	}

      private:
	friend class enum_decl_list_node;

	enum_id(const enum_decl_list_node *first_node) noexcept
	  : _first_node(first_node)
	{}

	const enum_decl_list_node *_first_node;
      };

      class enum_decl_list_node
      {
      public:
	enum_decl_list_node(enum_ref &self) noexcept;
	enum_decl_list_node(enum_def &self) noexcept;

	void link_to(enum_ref &target) noexcept;
	void link_to(enum_def &target) noexcept;

	const enum_decl_link& get_next() const noexcept
	{ return _next; }

	const enum_id get_id() const noexcept;

	enum_def* find_definition() const noexcept;

	bool is_first() const noexcept
	{ return _first_in_list; }

	bool is_visible_from(const enum_decl_list_node &from) const noexcept;

      private:
	template <typename target_type>
	void __link_to(target_type &target) noexcept;

	const enum_decl_list_node& _find_decl_node() const noexcept;
	const enum_decl_list_node& _find_first_decl_node() const noexcept;

	enum_decl_link _next;
	enum_decl_link *_prev;
	bool _first_in_list;
      };


      class enum_def final : public type_specifier
      {
      public:
	enum_def(const pp_tokens_range &tr,
		 const pp_token_index id_tok,
		 enumerator_list* &&el,
		 attribute_specifier_list* &&asl_before,
		 attribute_specifier_list* &&asl_after) noexcept;

	enum_def(const pp_tokens_range &tr,
		 enumerator_list* &&el,
		 attribute_specifier_list* &&asl_before,
		 attribute_specifier_list* &&asl_after) noexcept;

	virtual ~enum_def() noexcept override;

	bool has_id() const noexcept
	{ return _id_tok_valid; }

	pp_token_index get_id_tok() const noexcept
	{
	  assert(_id_tok_valid);
	  return _id_tok;
	}

	const enum_decl_list_node& get_decl_list_node() const noexcept
	{ return _decl_list_node; }

	enum_decl_list_node& get_decl_list_node() noexcept
	{ return _decl_list_node; }

	const enumerator_list&
	get_enumerator_list() const noexcept
	{ return _el; }

	enumerator_list&
	get_enumerator_list() noexcept
	{ return _el; }

	attribute_specifier_list* get_asl_before() noexcept
	{ return _asl_before; }

	const attribute_specifier_list* get_asl_before() const noexcept
	{ return _asl_before; }

	attribute_specifier_list* get_asl_after() noexcept
	{ return _asl_after; }

      private:
	enum_def(const pp_tokens_range &tr,
		 const pp_token_index id_tok,
		 enumerator_list* &&el,
		 attribute_specifier_list* &&asl_before,
		 attribute_specifier_list* &&asl_after,
		 const bool id_tok_valid) noexcept;

	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _id_tok;
	enumerator_list &_el;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
	enum_decl_list_node _decl_list_node;
	bool _id_tok_valid;
      };

      class enum_ref final : public type_specifier
      {
      public:
	enum_ref(const pp_tokens_range &tr,
		 const pp_token_index id_tok,
		 attribute_specifier_list* &&asl) noexcept;

	virtual ~enum_ref() noexcept override;

	pp_token_index get_id_tok() const noexcept
	{ return _id_tok; }

	const enum_decl_list_node& get_decl_list_node() const noexcept
	{ return _decl_list_node; }

	enum_decl_list_node& get_decl_list_node() noexcept
	{ return _decl_list_node; }

	const enum_decl_link& get_link_to_decl() const noexcept
	{ return _link_to_decl; }

	void link_to_declaration(const enum_decl_link &target) noexcept;

	bool is_standalone_decl() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _id_tok;
	attribute_specifier_list *_asl;
	enum_decl_list_node _decl_list_node;
	enum_decl_link _link_to_decl;
      };

      class typeof_expr final : public type_specifier
      {
      public:
	typeof_expr(const pp_tokens_range &tr,
		    expr* &&e) noexcept;

	virtual ~typeof_expr() noexcept override;

	const expr& get_expr() const noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
      };

      class typeof_type_name final : public type_specifier
      {
      public:
	typeof_type_name(const pp_tokens_range &tr,
			 type_name* &&tn) noexcept;

	virtual ~typeof_type_name() noexcept override;

	const type_name& get_type_name() const noexcept
	{ return _tn; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name &_tn;
      };

      class function_specifier final : public ast_entity<function_specifier>
      {
      public:
	typedef type_set<specifier_qualifier_list> parent_types;
      public:
	function_specifier(const pp_token_index spec_tok) noexcept;

	virtual ~function_specifier() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _spec_tok;
      };

      class specifier_qualifier_list
	: public typed_ast_entity<specifier_qualifier_list,
				  types::addressable_type>
      {
      public:
	typedef type_set<struct_declaration_c99, struct_declaration_unnamed_sou,
			 type_name, declaration,
			 parameter_declaration_declarator,
			 parameter_declaration_abstract,
			 function_definition> parent_types;

	specifier_qualifier_list(type_specifier* &&ts);

	specifier_qualifier_list(type_qualifier* &&tq);

	specifier_qualifier_list(attribute_specifier_list* &&asl);

	virtual ~specifier_qualifier_list() noexcept;

	void extend(type_specifier* &&ts);

	void extend(type_qualifier* &&tq);

	void extend(attribute_specifier_list* &&asl);

	void extend(specifier_qualifier_list* &&sql);

	virtual void evaluate_type(ast &a, const target &tgt) override;

	const types::qualifiers
	get_qualifiers() const;

	bool is_signed_explicit() const noexcept;

	bool is_auto_type() const noexcept;

	template <typename callable_type>
	bool for_each_attribute(callable_type &&c);

      protected:
	specifier_qualifier_list(const pp_tokens_range &tr) noexcept;

	std::size_t _n_children() const noexcept;

	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

      private:
	std::vector<std::reference_wrapper<type_specifier> > _tss;
	std::vector<std::reference_wrapper<type_qualifier> > _tqs;
	std::vector<std::reference_wrapper<attribute_specifier_list> > _asls;
      };

      class declaration_specifiers final : public specifier_qualifier_list
      {
      public:
	declaration_specifiers(const pp_tokens_range &tr);

	declaration_specifiers(storage_class_specifier* &&scs);

	declaration_specifiers(function_specifier* &&fs);

	declaration_specifiers(type_specifier* &&ts);

	declaration_specifiers(type_qualifier* &&tq);

	declaration_specifiers(attribute_specifier_list* &&asl);

	virtual ~declaration_specifiers() noexcept override;

	void extend(storage_class_specifier* &&scs);

	void extend(function_specifier* &&fs);

	void extend(type_specifier* &&ts);

	void extend(type_qualifier* &&tq);

	void extend(attribute_specifier_list* &&asl);

	void extend(declaration_specifiers* &&ds);

	storage_class get_storage_class(ast &ast) const;

	bool is_inline() const noexcept;

	template <typename callable_type>
	void for_each_storage_class_specifier(callable_type &&c) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	std::vector<std::reference_wrapper<storage_class_specifier> > _scss;
	std::vector<std::reference_wrapper<function_specifier> > _fss;
      };

      class initializer : public ast_entity<initializer>
      {
      public:
	typedef type_set<init_declarator, initializer_list> parent_types;

	virtual ~initializer() noexcept = 0;

	virtual void set_designation(designation* &&d) noexcept = 0;

	const designation* get_designation() const noexcept
	{ return _d; }

      protected:
	void _set_designation(designation* &&d) noexcept;

	initializer(const pp_tokens_range &tr) noexcept;

      protected:
	designation *_d;
      };

      class initializer_expr final : public initializer
      {
      public:
	initializer_expr(expr* &&e) noexcept;

	virtual ~initializer_expr() noexcept override;

	virtual void set_designation(designation* &&d) noexcept override;

	const expr& get_expr() const noexcept
	{ return _e; }

	expr& get_expr() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
      };

      class initializer_init_list final : public initializer
      {
      public:
	initializer_init_list(const pp_tokens_range &tr,
			      initializer_list* &&il) noexcept;

	virtual ~initializer_init_list() noexcept override;

	virtual void set_designation(designation* &&d) noexcept override;

	std::size_t get_size() const noexcept;

	initializer_list* get_initializer_list() noexcept
	{ return _il; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	initializer_list *_il;
      };

      class designator : public ast_entity<designator>
      {
      public:
	typedef type_set<designator_list> parent_types;

	designator(const pp_tokens_range &tr) noexcept;

	virtual ~designator() noexcept = 0;
      };

      class designator_member final : public designator
      {
      public:
	designator_member(const pp_tokens_range &tr,
			  const pp_token_index member_tok) noexcept;

	virtual ~designator_member() noexcept override;

	pp_token_index get_member_tok() const noexcept
	{ return _member_tok; }

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _member_tok;
      };

      class designator_array final : public designator
      {
      public:
	designator_array(const pp_tokens_range &tr,
			 expr* &&index_first, expr* &&index_last) noexcept;

	virtual ~designator_array() noexcept override;

	const expr& get_index_last() const noexcept
	{ return _index_last ? *_index_last : _index_first; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_index_first;
	expr *_index_last;
      };

      class designator_list final : public ast_entity<designator_list>
      {
      public:
	typedef type_set<designation> parent_types;

      public:
	designator_list(designator* &&d);

	virtual ~designator_list() noexcept override;

	void extend(designator* &&d);

	template <typename callable_type>
	void for_each(callable_type &&c) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<designator> > _ds;
      };

      class designation final : public ast_entity<designation>
      {
      public:
	typedef type_set<initializer_expr, initializer_init_list> parent_types;

      public:
	designation(const pp_tokens_range &tr,
		    designator_list* &&dl) noexcept;

	virtual ~designation() noexcept override;

	const designator_list& get_designator_list() const noexcept
	{ return _dl; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	designator_list &_dl;
      };

      class initializer_list final : public ast_entity<initializer_list>
      {
      public:
	typedef type_set<initializer_init_list,
			 expr_compound_literal> parent_types;

	initializer_list(initializer* &&i);

	virtual ~initializer_list() noexcept override;

	void extend(initializer* &&i);

	template <typename callable_type>
	void for_each(callable_type &&c) const;

	template <typename callable_type>
	void for_each(callable_type &&c);

	std::size_t get_size() const noexcept
	{ return _is.size(); }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<initializer> > _is;
      };


      class asm_label final : public ast_entity<asm_label>
      {
      public:
	typedef type_set<init_declarator> parent_types;

      public:
	asm_label(const pp_tokens_range &tr,
		  string_literal* &&label) noexcept;

	virtual ~asm_label() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	string_literal &_label;
      };

      class linkage
      {
      public:
	class linkage_id
	{
	public:
	  linkage_id() noexcept
	    : _l(nullptr)
	  {}

	  linkage_id& operator=(const linkage_id &lid)
	  {
	    _l = lid._l;
	    return *this;
	  }

	  bool operator==(const linkage_id op) const noexcept
	  { return this->_l == op._l; }

	  bool operator!=(const linkage_id op) const noexcept
	  { return this->_l != op._l; }

	  bool operator<(const linkage_id op) const noexcept
	  { return this->_l < op._l; }

	private:
	  friend class linkage;

	  linkage_id(const linkage &l) noexcept
	    : _l(&l)
	  {}

	  const linkage * _l;
	};

	enum class linkage_kind
	  {
	    none,
	    internal,
	    external,
	    nested_fun_auto,
	  };

	enum class link_target_kind
	{
	    unlinked,
	    init_decl,
	    function_def,
	};

	linkage() noexcept;

	linkage_kind get_linkage_kind() const noexcept
	{ return _linkage_kind; }

	void set_linkage_kind(const linkage_kind kind) noexcept;

	static void link(init_declarator &source, init_declarator &target,
			 const linkage_kind kind, const bool target_is_visible,
			 const bool update_last_at_file_scope) noexcept;
	static void link(init_declarator &source, function_definition &target,
			 const linkage_kind kind, const bool target_is_visible,
			 const bool update_last_at_file_scope) noexcept;
	static void link(function_definition &source, init_declarator &target,
			 const linkage_kind kind, const bool target_is_visible,
			 const bool update_last_at_file_scope) noexcept;
	static void link(function_definition &source,
			 function_definition &target,
			 const linkage_kind kind, const bool target_is_visible,
			 const bool update_last_at_file_scope) noexcept;

	static void set_first_at_file_scope(init_declarator &first) noexcept;
	static void set_first_at_file_scope(function_definition &first)
	  noexcept;

	bool is_linked_to(const init_declarator &id) const noexcept;
	bool is_linked_to(const function_definition &fd) const noexcept;

	link_target_kind get_link_target_kind() const noexcept
	{ return _target_kind; }

	const init_declarator& get_target_init_declarator() const noexcept;
	const function_definition& get_target_function_definition()
	  const noexcept;

	bool is_target_visible() const noexcept
	{ return _target_is_visible; }

	linkage_id get_id() const noexcept;

	const linkage& find_last_in_file_scope_chain() const noexcept;

	const bool is_first_in_chain() const noexcept
	{ return _first_in_chain; }

	const bool is_last_in_file_scope_chain() const noexcept
	{ return _last_in_file_scope_chain; }

	template <typename callable_type>
	void for_each_visible(callable_type &&c) const;

	void set_type_modified_through_linkage() noexcept
	{ _type_modified_through_linkage = true; }

	bool is_type_modified_through_linkage() const noexcept
	{ return _type_modified_through_linkage; }

      private:
	void _link_to(init_declarator &target, const linkage_kind kind,
		      const bool target_is_visible) noexcept;

	void _link_to(function_definition &target, const linkage_kind kind,
		      const bool target_is_visible) noexcept;

	linkage& _set_last_in_file_scope_chain() noexcept;

	bool _is_linked_to(const linkage &target) const noexcept;

	linkage_kind _linkage_kind;

	link_target_kind _target_kind;

	union
	{
	  init_declarator *_target_id;
	  function_definition *_target_fd;
	};

	bool _target_is_visible;
	bool _first_in_chain;
	bool _last_in_file_scope_chain;
	bool _type_modified_through_linkage;
      };

      class init_declarator final : public ast_entity<init_declarator>
      {
      public:
	typedef type_set<init_declarator_list> parent_types;


	init_declarator(const pp_tokens_range &tr,
			declarator* &&d, initializer* &&i, asm_label* &&al,
			attribute_specifier_list* &&asl_middle,
			attribute_specifier_list* &&asl_after) noexcept;

	virtual ~init_declarator() noexcept override;

	void set_asl_before(attribute_specifier_list* &&asl_before) noexcept;

	const declaration& get_containing_declaration() const noexcept;

	declaration& get_containing_declaration() noexcept;

	linkage& get_linkage() noexcept
	{ return _linkage; }

	const linkage& get_linkage() const noexcept
	{ return _linkage; }

	const declarator& get_declarator() const noexcept
	{ return _d; }

	declarator& get_declarator() noexcept
	{ return _d; }

	const attribute_specifier_list *get_asl_before() const noexcept
	{ return _asl_before; }

	attribute_specifier_list *get_asl_before() noexcept
	{ return _asl_before; }

	const attribute_specifier_list *get_asl_middle() const noexcept
	{ return _asl_middle; }

	attribute_specifier_list *get_asl_middle() noexcept
	{ return _asl_middle; }

	const attribute_specifier_list *get_asl_after() const noexcept
	{ return _asl_after; }

	attribute_specifier_list *get_asl_after() noexcept
	{ return _asl_after; }

	initializer* get_initializer() noexcept
	{ return _i; }

	const initializer* get_initializer() const noexcept
	{ return _i; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declarator &_d;
	initializer *_i;
	asm_label *_al;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_middle;
	attribute_specifier_list *_asl_after;

	linkage _linkage;
      };

      class init_declarator_list final : public ast_entity<init_declarator_list>
      {
      public:
	typedef type_set<declaration> parent_types;

	init_declarator_list(init_declarator* &&id);

	virtual ~init_declarator_list() noexcept override;

	void extend(init_declarator* &&id);

	template <typename callable_type>
	void for_each(callable_type &&c) const;

	template <typename callable_type>
	void for_each(callable_type &&c);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<init_declarator> > _ids;
      };

      class declaration final : public ast_entity<declaration>
      {
      public:
	typedef type_set<declaration_list,
			 block_item_decl,
			 stmt_for_init_decl,
			 external_declaration_decl> parent_types;

      public:
	declaration(const pp_tokens_range &tr, declaration_specifiers* &&ds,
		    init_declarator_list* &&idl) noexcept;

	virtual ~declaration() noexcept override;

	bool is_at_file_scope() const noexcept;

	const declaration_specifiers& get_declaration_specifiers()
	  const noexcept
	{ return _ds; }

	declaration_specifiers& get_declaration_specifiers() noexcept
	{ return _ds; }

	const init_declarator_list* get_init_declarator_list() const noexcept
	{ return _idl; }

	init_declarator_list* get_init_declarator_list() noexcept
	{ return _idl; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration_specifiers &_ds;
	init_declarator_list *_idl;
      };

      class static_assertion final : public ast_entity<static_assertion>
      {
      public:
	typedef type_set<struct_declaration_static_assert,
			 stmt_for_init_static_assert,
			 block_item_static_assert,
			 external_declaration_static_assert> parent_types;

	static_assertion(const pp_tokens_range &tr, expr* &&cond,
			 string_literal *&&msg) noexcept;

	virtual ~static_assertion() noexcept override;

	void test_assertion(ast &a, const target &tgt) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_cond;
	string_literal &_msg;
      };

      class parameter_declaration : public ast_entity<parameter_declaration>
      {
      public:
	typedef type_set<parameter_declaration_list> parent_types;

	parameter_declaration(const pp_tokens_range &tr,
			      declaration_specifiers* &&ds,
			      attribute_specifier_list* &&asl) noexcept;

	virtual ~parameter_declaration() noexcept = 0;

	declaration_specifiers& get_declaration_specifiers() noexcept
	{ return _ds; }

	const declaration_specifiers& get_declaration_specifiers()
	  const noexcept
	{ return _ds; }

	attribute_specifier_list *get_asl() noexcept
	{
	  return _asl;
	}

	virtual std::shared_ptr<const types::addressable_type> get_type()
	  const = 0;

      protected:
	declaration_specifiers &_ds;
	attribute_specifier_list *_asl;
      };

      class parameter_declaration_declarator final
	: public parameter_declaration
      {
      public:
	parameter_declaration_declarator(const pp_tokens_range &tr,
					 declaration_specifiers* &&ds,
					 declarator* &&d,
					 attribute_specifier_list* &&asl)
	  noexcept;

	virtual ~parameter_declaration_declarator() noexcept override;

	virtual std::shared_ptr<const types::addressable_type> get_type()
	  const override;

	const declarator& get_declarator() const noexcept
	{
	  return _d;
	}

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declarator &_d;
      };

      class parameter_declaration_abstract final
	: public parameter_declaration
      {
      public:
	parameter_declaration_abstract(const pp_tokens_range &tr,
				       declaration_specifiers* &&ds,
				       abstract_declarator* &&ad,
				       attribute_specifier_list* &&asl)
	  noexcept;

	virtual ~parameter_declaration_abstract() noexcept override;

	virtual std::shared_ptr<const types::addressable_type> get_type()
	  const override;

	void evaluate_type(ast &a, const target &tgt);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	abstract_declarator *_ad;
	std::shared_ptr<const types::addressable_type> _type;
      };

      class parameter_declaration_list final
	: public ast_entity<parameter_declaration_list>
      {
      public:
	typedef type_set<direct_abstract_declarator_func,
			 direct_declarator_func> parent_types;

	parameter_declaration_list(parameter_declaration* &&pd);

	virtual ~parameter_declaration_list() noexcept override;

	void extend(parameter_declaration* &&pd);

	void set_variadic(const pp_tokens_range &triple_dot_tr) noexcept;

	bool is_variadic() const noexcept
	{ return _variadic; }

	template<typename callable_type>
	void for_each(callable_type&& c) const;

	bool is_single_void(ast &a) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<parameter_declaration> > _pds;
	bool _variadic;
      };

      class identifier_list final : public ast_entity<identifier_list>
      {
      public:
	typedef type_set<direct_abstract_declarator_func,
			 direct_declarator_func,
			 local_label_declaration> parent_types;

	identifier_list(const pp_token_index id_tok);

	virtual ~identifier_list() noexcept override;

	void extend(const pp_token_index id_tok);

	const std::vector<pp_token_index>&
	get_identifiers() const noexcept
	{ return _ids; }

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<pp_token_index> _ids;
      };

      class declaration_list final : public ast_entity<declaration_list>
      {
      public:
	typedef type_set<function_definition> parent_types;

	declaration_list(declaration* &&d);

	virtual ~declaration_list() noexcept override;

	void extend(declaration* &&d);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<declaration> > _ds;
      };


      class stmt : public ast_entity<stmt>
      {
      public:
	typedef type_set<stmt_labeled, stmt_case, stmt_case_range,
			 stmt_default, stmt_if, stmt_switch,
			 stmt_while, stmt_do, stmt_for,
			 block_item_stmt, function_definition,
			 expr_statement> parent_types;

	virtual ~stmt() noexcept = 0;

      protected:
	stmt(const pp_tokens_range &tr) noexcept;
      };

      class stmt_labeled final : public stmt
      {
      public:
	stmt_labeled(const pp_tokens_range &tr,
		     const pp_token_index label_tok,
		     stmt* &&s, attribute_specifier_list* &&asl) noexcept;

	virtual ~stmt_labeled() noexcept override;

	pp_token_index get_label_tok() const noexcept
	{ return _label_tok; }

	const stmt& get_stmt() const noexcept
	{ return _s; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _label_tok;
	stmt &_s;
	attribute_specifier_list *_asl;
      };

      class stmt_case final : public stmt
      {
      public:
	stmt_case(const pp_tokens_range &tr,
		  expr* &&e, stmt* &&s) noexcept;

	virtual ~stmt_case() noexcept override;

	expr& get_expr() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
	stmt &_s;
      };

      class stmt_case_range final : public stmt
      {
      public:
	stmt_case_range(const pp_tokens_range &tr,
			expr* &&e_low, expr* &&e_high, stmt* &&s) noexcept;

	virtual ~stmt_case_range() noexcept override;

	expr& get_expr_lo() noexcept
	{ return _e_low; }

	expr& get_expr_hi() noexcept
	{ return _e_high; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e_low;
	expr &_e_high;
	stmt &_s;
      };

      class stmt_default final : public stmt
      {
      public:
	stmt_default(const pp_tokens_range &tr, stmt* &&s) noexcept;

	virtual ~stmt_default() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	stmt &_s;
      };


      class local_label_declaration final
	: public ast_entity<local_label_declaration>
      {
      public:
	typedef type_set<local_label_declaration_list> parent_types;

	local_label_declaration(const pp_tokens_range &tr,
				identifier_list* &&idl) noexcept;

	virtual ~local_label_declaration() noexcept override;

	const identifier_list& get_identifier_list() const noexcept
	{ return _idl; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	identifier_list &_idl;
      };

      class local_label_declaration_list final
	: public ast_entity<local_label_declaration_list>
      {
      public:
	typedef type_set<stmt_compound> parent_types;

	local_label_declaration_list(local_label_declaration* &&lld);

	virtual ~local_label_declaration_list() noexcept override;

	void extend(local_label_declaration* &&lld);

	const std::vector<std::reference_wrapper<local_label_declaration> >&
	get_local_label_declarations() const noexcept
	{ return _llds; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<local_label_declaration> > _llds;
      };

      class block_item : public ast_entity<block_item>
      {
      public:
	typedef type_set<block_item_list> parent_types;

	block_item(const pp_tokens_range &tr) noexcept;

	virtual ~block_item() noexcept = 0;
      };

      class block_item_decl final : public block_item
      {
      public:
	block_item_decl(declaration* &&d) noexcept;

	virtual ~block_item_decl() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration &_d;
      };

      class block_item_static_assert final : public block_item
      {
      public:
	block_item_static_assert(static_assertion* &&sa) noexcept;

	virtual ~block_item_static_assert() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	static_assertion &_sa;
      };

      class block_item_stmt final : public block_item
      {
      public:
	block_item_stmt(stmt* &&s) noexcept;

	virtual ~block_item_stmt() noexcept override;

	const stmt& get_stmt() const noexcept
	{ return _s; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	stmt &_s;
      };

      class block_item_function_definition final : public block_item
      {
      public:
	block_item_function_definition(function_definition* &&fd) noexcept;

	virtual ~block_item_function_definition() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	function_definition &_fd;
      };

      class block_item_list final : public ast_entity<block_item_list>
      {
      public:
	typedef type_set<stmt_compound> parent_types;

	block_item_list(block_item* &&bi);

	virtual ~block_item_list() noexcept override;

	void extend(block_item* &&bi);

	template <typename callable_type>
	bool for_each_reverse(callable_type &&c) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<block_item> > _bis;
      };

      class stmt_compound final : public stmt
      {
      public:
	stmt_compound(const pp_tokens_range &tr,
		      local_label_declaration_list* &&lldl,
		      block_item_list* &&bil) noexcept;

	virtual ~stmt_compound() noexcept override;

	void register_label(stmt_labeled &label);

	stmt_labeled* lookup_label(const ast &ast,
				   const pp_token_index &label_tok) noexcept;

	bool is_local_label(const ast &ast,
			    const pp_token_index &label_tok) noexcept;

	const block_item_list * get_block_item_list() const noexcept
	{ return _bil; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	local_label_declaration_list *_lldl;
	block_item_list *_bil;

	std::vector<std::reference_wrapper<stmt_labeled> > _labels;
      };

      class stmt_expr final : public stmt
      {
      public:
	stmt_expr(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~stmt_expr() noexcept override;

	const expr* get_expr() const noexcept
	{ return _e; }

	expr* get_expr() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr *_e;
      };

      class stmt_if final : public stmt
      {
      public:
	stmt_if(const pp_tokens_range &tr,
		expr* &&cond, stmt* &&s_true, stmt* &&s_false)
	  noexcept;

	virtual ~stmt_if() noexcept override;

	const expr& get_cond() const noexcept
	{ return _cond; }

	expr& get_cond() noexcept
	{ return _cond; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_cond;
	stmt &_s_true;
	stmt *_s_false;
      };

      class stmt_switch final : public stmt
      {
      public:
	stmt_switch(const pp_tokens_range &tr, expr* &&e, stmt* &&s) noexcept;

	virtual ~stmt_switch() noexcept override;

	const expr& get_expr() const noexcept
	{ return _e; }

	expr& get_expr() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
	stmt &_s;
      };

      class stmt_while final : public stmt
      {
      public:
	stmt_while(const pp_tokens_range &tr, expr* &&e, stmt* &&s) noexcept;

	virtual ~stmt_while() noexcept override;

	const expr& get_cond() const noexcept
	{ return _e; }

	expr& get_cond() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
	stmt &_s;
      };

      class stmt_do final : public stmt
      {
      public:
	stmt_do(const pp_tokens_range &tr, expr* &&e, stmt* &&s) noexcept;

	virtual ~stmt_do() noexcept override;

	const expr& get_cond() const noexcept
	{ return _e; }

	expr& get_cond() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
	stmt &_s;
      };

      class stmt_for : public stmt
      {
      public:
	stmt_for(const pp_tokens_range &tr, expr* &&cond, expr* &&next,
		 stmt* &&s) noexcept;

	virtual ~stmt_for() noexcept override = 0;

	const expr* get_cond() const noexcept
	{ return _cond; }

	expr* get_cond() noexcept
	{ return _cond; }

	const expr* get_next_expr() const noexcept
	{ return _next; }

	expr* get_next_expr() noexcept
	{ return _next; }

      protected:
	expr *_cond;
	expr *_next;
	stmt &_s;
      };

      class stmt_for_init_expr final : public stmt_for
      {
      public:
	stmt_for_init_expr(const pp_tokens_range &tr,
			   expr* &&init, expr* &&cond, expr* &&next,
			   stmt* &&s) noexcept;

	virtual ~stmt_for_init_expr() noexcept override;

	const expr* get_init_expr() const noexcept
	{ return _init; }

	expr* get_init_expr() noexcept
	{ return _init; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr *_init;
      };

      class stmt_for_init_decl final : public stmt_for
      {
      public:
	stmt_for_init_decl(const pp_tokens_range &tr,
			   declaration* &&d,
			   expr* &&cond, expr* &&next,
			   stmt* &&s) noexcept;

	virtual ~stmt_for_init_decl() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration &_d;
      };

      class stmt_for_init_static_assert final : public stmt_for
      {
      public:
	stmt_for_init_static_assert(const pp_tokens_range &tr,
			   static_assertion* &&sa,
			   expr* &&cond, expr* &&next,
			   stmt* &&s) noexcept;

	virtual ~stmt_for_init_static_assert() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	static_assertion &_sa;
      };

      class stmt_goto final : public stmt
      {
      public:
	stmt_goto(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~stmt_goto() noexcept override;

	expr& get_expr() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
      };

      class stmt_continue final : public stmt
      {
      public:
	stmt_continue(const pp_tokens_range &tr) noexcept;

	virtual ~stmt_continue() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;
      };

      class stmt_break final : public stmt
      {
      public:
	stmt_break(const pp_tokens_range &tr) noexcept;

	virtual ~stmt_break() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;
      };

      class stmt_return final : public stmt
      {
      public:
	stmt_return(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~stmt_return() noexcept override;

	const expr *get_expr() const noexcept
	{ return _e; }

	expr *get_expr() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr *_e;
      };

      class stmt_asm final : public stmt
      {
      public:
	stmt_asm(asm_directive* &&ad) noexcept;

	virtual ~stmt_asm() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	asm_directive &_ad;
      };


      enum class asm_qualifier
      {
	aq_goto,
	aq_volatile,
	aq_inline,
      };

      class asm_qualifier_list final : public ast_entity<asm_qualifier_list>
      {
      public:
	typedef type_set<asm_directive> parent_types;

	asm_qualifier_list(const pp_tokens_range &tr,
			   const asm_qualifier aq);

	virtual ~asm_qualifier_list() noexcept override;

	void extend(const pp_tokens_range &tr,
		    const asm_qualifier aq);

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<asm_qualifier> _aqs;
      };

      class asm_operand_name final : public ast_entity<asm_operand_name>
      {
      public:
	typedef type_set<asm_operand> parent_types;

	asm_operand_name(const pp_tokens_range &tr,
			 const pp_token_index id_tok) noexcept;

	virtual ~asm_operand_name() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _id_tok;
      };

      class asm_operand final : public ast_entity<asm_operand>
      {
      public:
	typedef type_set<asm_operand_list> parent_types;

	asm_operand(const pp_tokens_range &tr, asm_operand_name* &&aon,
		    string_literal* &&constaint, expr* &&e) noexcept;

	virtual ~asm_operand() noexcept override;

	const expr& get_expr() const noexcept
	{ return _e; }

	expr& get_expr() noexcept
	{ return _e; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	asm_operand_name *_aon;
	string_literal &_constraint;
	expr &_e;
      };

      class asm_operand_list final : public ast_entity<asm_operand_list>
      {
      public:
	typedef type_set<asm_directive> parent_types;

      public:
	asm_operand_list(asm_operand* &&ao);

	virtual ~asm_operand_list() noexcept override;

	void extend(asm_operand* &&ao);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<asm_operand> > _aos;
      };

      class asm_clobber_list final : public ast_entity<asm_clobber_list>
      {
      public:
	typedef type_set<asm_directive> parent_types;

	asm_clobber_list(string_literal* &&clobber);

	virtual ~asm_clobber_list() noexcept override;

	void extend(string_literal* && clobber);

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<string_literal> > _clobbers;
      };

      class asm_jump_to_label_list final
	: public ast_entity<asm_jump_to_label_list>
      {
      public:
	typedef type_set<asm_directive> parent_types;

	asm_jump_to_label_list(const pp_token_index label_tok);

	virtual ~asm_jump_to_label_list() noexcept override;

	void extend(const pp_token_index label_tok);

      private:
	virtual _ast_entity* _get_child(const size_t) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<pp_token_index> _label_toks;
      };

      class asm_directive final : public ast_entity<asm_directive>
      {
      public:
	typedef type_set<stmt_asm, external_declaration_asm> parent_types;

	asm_directive(const pp_tokens_range &tr,
		      asm_qualifier_list* &&aql, string_literal* &&asm_s,
		      asm_operand_list* &&output_aol,
		      asm_operand_list* &&input_aol, asm_clobber_list* &&acl,
		      asm_jump_to_label_list* &&ajtll)
	  noexcept;

	virtual ~asm_directive() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	asm_qualifier_list *_aql;
	string_literal &_asm_s;
	asm_operand_list *_output_aol;
	asm_operand_list *_input_aol;
	asm_clobber_list *_acl;
	asm_jump_to_label_list *_ajtll;
      };

      class function_definition final : public ast_entity<function_definition>
      {
      public:
	typedef type_set<external_declaration_func,
			 block_item_function_definition> parent_types;

	function_definition(const pp_tokens_range &tr,
			    declaration_specifiers* &&ds,
			    declarator* &&d, attribute_specifier_list* &&asl,
			    declaration_list* &&dl, stmt_compound* &&sc)
	  noexcept;

	virtual ~function_definition() noexcept override;

	const declarator& get_declarator() const noexcept
	{ return _d; }

	linkage& get_linkage() noexcept
	{ return _linkage; }

	const linkage& get_linkage() const noexcept
	{ return _linkage; }

	const declaration_specifiers& get_declaration_specifiers()
	  const noexcept
	{ return _ds; }

	declaration_specifiers& get_declaration_specifiers() noexcept
	{ return _ds; }

	bool is_at_file_scope() const noexcept;

	const attribute_specifier_list* get_asl() const noexcept
	{ return _asl; }

	attribute_specifier_list* get_asl() noexcept
	{ return _asl; }

	const declaration_list* get_declaration_list() const noexcept
	{ return _dl; }

	const stmt_compound& get_stmt_compound() const noexcept
	{ return _sc; }

	void set_is_gnu_inline() noexcept;

	bool is_gnu_inline() const noexcept
	{ return _is_gnu_inline; }

	void set_is_always_inline() noexcept;

	bool is_always_inline() const noexcept
	{ return _is_always_inline; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration_specifiers &_ds;
	declarator &_d;
	attribute_specifier_list *_asl;
	declaration_list *_dl;
	stmt_compound &_sc;

	linkage _linkage;
	bool _is_gnu_inline;
	bool _is_always_inline;
      };

      class external_declaration : public ast_entity<external_declaration>
      {
      public:
	typedef type_set<translation_unit> parent_types;

	external_declaration(const pp_tokens_range &tr) noexcept;

	virtual ~external_declaration() noexcept = 0;
      };

      class external_declaration_decl final : public external_declaration
      {
      public:
	external_declaration_decl(declaration* &&d) noexcept;

	virtual ~external_declaration_decl() noexcept override;

	const declaration& get_declaration() const noexcept
	{ return _d; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration &_d;
      };

      class external_declaration_static_assert final
	: public external_declaration
      {
      public:
	external_declaration_static_assert(static_assertion* &&sa) noexcept;

	virtual ~external_declaration_static_assert() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	static_assertion &_sa;
      };

      class external_declaration_func final : public external_declaration
      {
      public:
	external_declaration_func(function_definition* &&fd) noexcept;

	virtual ~external_declaration_func() noexcept override;

	const function_definition &get_function_definition() const noexcept
	{ return _fd; }

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	function_definition &_fd;
      };

      class external_declaration_asm final : public external_declaration
      {
      public:
	external_declaration_asm(asm_directive* &&ad) noexcept;

	virtual ~external_declaration_asm() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	asm_directive &_ad;
      };


      class translation_unit final : public ast_entity<translation_unit>
      {
      public:
	typedef type_set<> parent_types;

	translation_unit(const pp_tokens_range &r);

	translation_unit(external_declaration* &&ed);

	virtual ~translation_unit() noexcept override;

	void extend(external_declaration* &&ed);

	void extend_tokens_range(const pp_tokens_range &tr) noexcept;

	template <typename callable_type>
	void for_each_external_declaration(callable_type &&c) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) const noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<external_declaration> > _eds;
      };


      class ast
      {
      public:
	ast(const pp_result &pp_result, const bool is_pp_expr);

	ast(ast &&a);

	virtual ~ast() noexcept = 0;

	const pp_result& get_pp_result() const noexcept
	{ return _pp_result; }

	const pp_tokens& get_pp_tokens() const noexcept;

	code_remarks& get_remarks() noexcept
	{ return _remarks; }

	bool is_pp_expr() const noexcept
	{ return _is_pp_expr; }

      private:
	const pp_result &_pp_result;
	code_remarks _remarks;
	bool _is_pp_expr;
      };

      class ast_translation_unit final : public ast
      {
      public:
	ast_translation_unit(std::unique_ptr<const pp_result> &&pp_result,
			     std::unique_ptr<translation_unit> &&tu);

	ast_translation_unit(ast_translation_unit &&a);

	virtual ~ast_translation_unit() noexcept override;

	template <typename handled_types, typename callables_wrapper_type>
	void for_each_dfs_po(callables_wrapper_type &&c);

	template <typename handled_types_pre,
		  typename handled_types_post,
		  typename callables_wrapper_type_pre,
		  typename callables_wrapper_type_post>
	void for_each_dfs_pre_and_po(callables_wrapper_type_pre &&c_pre,
				     callables_wrapper_type_post &&c_post);

	template <typename handled_types_pre,
		  typename handled_types_post,
		  typename callables_wrapper_type_pre,
		  typename callables_wrapper_type_post>
	void for_each_dfs_pre_and_po(callables_wrapper_type_pre &&c_pre,
				     callables_wrapper_type_post &&c_post)
	  const;

	void resolve(const target &tgt);

	void evaluate(const target &tgt);

	const translation_unit& get_translation_unit() const
	{ return *_tu; }

      private:
	void _register_labels();

	void _resolve_ids(const target &tgt);

	std::unique_ptr<const pp_result> _pp_result;
	std::unique_ptr<translation_unit> _tu;
      };

      class ast_pp_expr final : public ast
      {
      public:
	ast_pp_expr(const pp_result &pp_result, std::unique_ptr<expr> &&e);

	ast_pp_expr(ast_pp_expr &&a);

	virtual ~ast_pp_expr() noexcept override;

	bool evaluate(const target &tgt);

      private:
	std::unique_ptr<expr> _e;
      };
    }
  }
}

#endif
