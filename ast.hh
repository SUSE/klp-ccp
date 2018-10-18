#ifndef AST_HH
#define AST_HH

#include <vector>
#include <cassert>
#include <initializer_list>
#include <functional>
#include <memory>
#include "type_set.hh"
#include "header_inclusion_tree.hh"
#include "pp_tokens.hh"
#include "code_remarks.hh"
#include "code_remark.hh"
#include "types.hh"

#ifdef DEBUG_PARSER
#include <iostream>
#endif

namespace suse
{
  namespace cp
  {
    namespace ast
    {
      typedef std::size_t pp_token_index;
      struct pp_tokens_range
      {
	pp_tokens_range() noexcept
	  : begin(0), end(0)
	{}

	pp_tokens_range(const pp_token_index _begin,
			const pp_token_index _end) noexcept
	  : begin(_begin), end(_end)
	{}

	pp_token_index begin;
	pp_token_index end;
      };

#ifdef DEBUG_PARSER
      std::ostream& operator<<(std::ostream &o, const pp_tokens_range &r);
#endif

      template<typename ret_type>
      class processor;

      template<typename ret_type>
      class const_processor;

      template<typename derived>
      class ast_entity;

      class _ast_entity
      {
      public:
	_ast_entity(const pp_tokens_range &tr) noexcept;

	_ast_entity(const _ast_entity &ae) noexcept;

	virtual ~_ast_entity() noexcept;

	const pp_tokens_range get_tokens_range() const noexcept
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

      protected:
	void _extend_tokens_range(const pp_tokens_range &tr) noexcept;

	_ast_entity *_parent;

      private:
	friend class ast;

	template<typename derived>
	friend class ast_entity;

	virtual _ast_entity* _get_child(const size_t i) noexcept = 0;

	virtual void _process(processor<void> &p) = 0;
	virtual void _process(const_processor<void> &p) const = 0;
	virtual bool _process(processor<bool> &p) = 0;
	virtual bool _process(const_processor<bool> &p) const = 0;

	template<typename callable_type_pre,
		 typename callable_type_post>
	void _for_each_dfs_pre_and_po(callable_type_pre &&c_pre,
				      callable_type_post &&c_post);

	template <typename callable_type>
	void for_each_dfs_po(callable_type &&c);

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

      template <typename derived>
      ast_entity<derived>::ast_entity(const pp_tokens_range &tr)
	noexcept
	: _ast_entity(tr)
      {}

      template <typename derived>
      ast_entity<derived>::ast_entity(const ast_entity &ae) noexcept
	: _ast_entity(ae)
      {}

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
      class block_item_stmt;
      class block_item_function_definition;
      class block_item_list;
      class stmt_compound;
      class stmt_expr;
      class stmt_if;
      class stmt_switch;
      class stmt_while;
      class stmt_do;
      class stmt_for_init_expr;
      class stmt_for_init_decl;
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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<expr> > _exprs;
      };

      class expr : public ast_entity<expr>
      {
      public:
	typedef type_set<expr_list,
			 expr_cast, expr_unop_pre,
			 expr_sizeof_expr, expr_alignof_expr,
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
			 stmt_case, stmt_case_range,
			 stmt_expr, stmt_if, stmt_switch,
			 stmt_while, stmt_do,
			 stmt_for_init_expr, stmt_for_init_decl,
			 stmt_goto, stmt_return,
			 asm_operand> parent_types;

	expr(expr&&) = delete;

	virtual ~expr() noexcept = 0;

	virtual const expr& skip_parens_down() const noexcept;
	virtual const _ast_entity* skip_parens_up() const noexcept;
	virtual const _ast_entity* skip_parens_up() noexcept;
	const _ast_entity* get_non_parens_parent() const noexcept;
	_ast_entity* get_non_parens_parent() noexcept;

      protected:
	expr(const pp_tokens_range &tr) noexcept;
      };

      class expr_comma final : public expr
      {
      public:
	expr_comma(expr* &&l, expr* &&r) noexcept;

	virtual ~expr_comma() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

	const expr& get_lhs() const noexcept
	{ return _lhs; }

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	type_name &_tn;
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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	  struct builtin_tag {};

	  resolved() noexcept;
	  resolved(const builtin_tag&) noexcept;
	  resolved(init_declarator &id) noexcept;
	  resolved(parameter_declaration_declarator &pdd) noexcept;
	  resolved(function_definition &fd) noexcept;
	  resolved(stmt_labeled &sl) noexcept;
	  resolved(enumerator &e) noexcept;
	  resolved(identifier_list &pil) noexcept;

	  enum class resolved_kind
	  {
	    none,
	    builtin,
	    init_declarator,
	    parameter_declaration_declarator,
	    function_definition,
	    stmt_labeled,
	    enumerator,
	    in_param_id_list,
	  };

	  resolved_kind get_kind() const noexcept
	  { return _kind; }

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

      private:
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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
			 asm_directive,
			 asm_label> parent_types;

	string_literal(const pp_token_index s);

	virtual ~string_literal() noexcept override;

	void extend(const pp_token_index s);

	const std::vector<pp_token_index>& get_strings() const noexcept
	{ return _strings; }

      private:
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<type_qualifier_list*> _tqls;
      };

      class direct_abstract_declarator
	: public ast_entity<direct_abstract_declarator>
      {
      public:
	typedef type_set<abstract_declarator,
			 direct_abstract_declarator_array,
			 direct_abstract_declarator_func> parent_types;

	virtual ~direct_abstract_declarator() noexcept;

	virtual const direct_abstract_declarator*
	skip_trivial_parens_down() const noexcept;

      protected:
	direct_abstract_declarator(const pp_tokens_range &tr) noexcept;
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

	virtual const direct_abstract_declarator*
	skip_trivial_parens_down() const noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	direct_abstract_declarator *_dad;
	parameter_declaration_list *_ptl;
      };

      class abstract_declarator final : public ast_entity<abstract_declarator>
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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pointer *_pt;
	direct_abstract_declarator *_dad;
      };

      class type_name final : public ast_entity<type_name>
      {
      public:
	typedef type_set<typeof_type_name, expr_sizeof_type_name,
			 expr_alignof_type_name, expr_builtin_offsetof,
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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	specifier_qualifier_list &_sql;
	abstract_declarator *_ad;
      };

      class direct_declarator : public ast_entity<direct_declarator>
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

	pp_token_index get_id_tok() const noexcept;

	bool is_function() const noexcept;




	template <typename ret_type, typename callables_wrapper_type>
	ret_type process_context(callables_wrapper_type &&c) const;

	template <typename ret_type, typename callables_wrapper_type>
	ret_type process_context(callables_wrapper_type &&c);

      private:
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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


      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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


      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	direct_declarator &_dd;
	parameter_declaration_list *_ptl;
	identifier_list *_il;
      };

      class declarator final : public ast_entity<declarator>
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

	const direct_declarator_id& get_direct_declarator_id() const noexcept;

	direct_declarator_id& get_direct_declarator_id() noexcept;

	pp_token_index get_id_tok() const noexcept;

	const pointer* get_pointer() const noexcept
	{
	  return _pt;
	}

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pointer *_pt;
	direct_declarator &_dd;
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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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

	template <typename callable_type>
	bool for_each_attribute(callable_type &&c);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	ps_int128,
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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pod_spec _pod_spec;
      };

      class type_specifier_tdid final : public type_specifier
      {
      public:
	type_specifier_tdid(const pp_token_index tdid_tok) noexcept;

	virtual ~type_specifier_tdid() noexcept override;

	pp_token_index get_id_tok() const noexcept
	{ return _tdid_tok; }

	void set_resolved(const direct_declarator_id &ddid) noexcept;

	const direct_declarator_id& get_resolved() const noexcept;

	void set_builtin() noexcept;

	bool is_builtin() const noexcept
	{ return _is_builtin; }

      private:
	virtual _ast_entity* _get_child(const size_t) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _tdid_tok;
	const direct_declarator_id *_resolved;
	bool _is_builtin;
      };

      class struct_declaration : public ast_entity<struct_declaration>
      {
      public:
	typedef type_set<struct_declaration_list> parent_types;

	struct_declaration(const pp_tokens_range &tr,
			   specifier_qualifier_list* &&sql) noexcept;

	virtual ~struct_declaration() noexcept = 0;

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

	const declarator* get_declarator() const noexcept
	{ return _d; }

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declarator *_d;
	expr *_width;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
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

	template <typename callable_type>
	void for_each(callable_type &&c) const;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<struct_declarator> > _sds;
      };

      class struct_declaration_c99 final : public struct_declaration
      {
      public:
	struct_declaration_c99(const pp_tokens_range &tr,
			       specifier_qualifier_list* &&sql,
			       struct_declarator_list * &&sdl) noexcept;

	virtual ~struct_declaration_c99() noexcept override;

	const struct_declarator_list& get_struct_declarator_list()
	  const noexcept
	{ return _sdl; }

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

	attribute_specifier_list* get_asl_before() noexcept
	{ return _asl_before; }

	attribute_specifier_list* get_asl_after() noexcept
	{ return _asl_after; }

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	types::struct_or_union_kind _souk;
	struct_declaration_list *_sdl;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
      };

      class struct_declaration_unnamed_sou final : public struct_declaration
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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	unnamed_struct_or_union &_unnamed_sou;
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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

	const sou_decl_link get_declaration() const noexcept;

	struct_or_union_def* find_definition() const noexcept;

      private:
	template <typename target_type>
	void __link_to(target_type &target) noexcept;

	sou_decl_link _next;
	sou_decl_link *_prev;
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

	attribute_specifier_list *get_asl_after() noexcept
	{ return _asl_after; }

      private:
	struct_or_union_def(const pp_tokens_range &tr,
			    const types::struct_or_union_kind souk,
			    const pp_token_index id_tok,
			    struct_declaration_list* &&sdl,
			    attribute_specifier_list* &&asl_before,
			    attribute_specifier_list* &&asl_after,
			    const bool id_tok_valid) noexcept;

	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<enumerator> > _es;
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

	const enumerator_list&
	get_enumerator_list() const noexcept
	{ return _el; }

	enumerator_list&
	get_enumerator_list() noexcept
	{ return _el; }

	attribute_specifier_list* get_asl_before() noexcept
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

	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _id_tok;
	enumerator_list &_el;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
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

	void link_to_definition(enum_def &ed) noexcept;

	const enum_def& get_definition() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _id_tok;
	attribute_specifier_list *_asl;
	enum_def *_def;
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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	pp_token_index _spec_tok;
      };

      class specifier_qualifier_list
	: public ast_entity<specifier_qualifier_list>
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

	bool is_signed_explicit() const noexcept;

	template <typename callable_type>
	bool for_each_attribute(callable_type &&c) const;

      protected:
	specifier_qualifier_list(const pp_tokens_range &tr) noexcept;

	std::size_t _n_children() const noexcept;

	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	string_literal &_label;
      };

      class linkage
      {
      public:
	enum class linkage_kind
	  {
	    none,
	    internal,
	    external,
	    nested_fun_auto,
	  };

	linkage(init_declarator &self) noexcept;
	linkage(function_definition &self) noexcept;

	linkage_kind get_linkage_kind() const noexcept
	{ return _linkage_kind; }

	void set_linkage_kind(const linkage_kind kind) noexcept;

	void link_to(init_declarator &target,
		     const linkage_kind kind) noexcept;
	void link_to(function_definition &target,
		     const linkage_kind kind) noexcept;

	bool is_linked_to(const init_declarator &id) const noexcept;
	bool is_linked_to(const function_definition &fd) const noexcept;

      private:
	class link
	{
	public:
	  enum class link_target_kind
	  {
	    init_decl,
	    function_def,
	  };

	  link(init_declarator &id) noexcept;
	  link(function_definition &fd) noexcept;

	  const linkage& get_target_linkage() const noexcept;

	private:

	  link_target_kind _target_kind;

	  union {
	    init_declarator *_target_id;
	    function_definition *_target_fd;
	  };
	};

	template<typename target_type>
	void __link_to(target_type &target, const linkage_kind kind) noexcept;

	bool __is_linked_to(const linkage &target) const noexcept;

	linkage_kind _linkage_kind;
	link _next;
	link *_prev;
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

	linkage& get_linkage() noexcept
	{ return _linkage; }

	const linkage& get_linkage() const noexcept
	{ return _linkage; }

	const declarator& get_declarator() const noexcept
	{ return _d; }

	declarator& get_declarator() noexcept
	{ return _d; }

	attribute_specifier_list *get_asl_before() noexcept
	{ return _asl_before; }

	attribute_specifier_list *get_asl_middle() noexcept
	{ return _asl_middle; }

	attribute_specifier_list *get_asl_after() noexcept
	{ return _asl_after; }

	initializer* get_initializer() noexcept
	{ return _i; }

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

	const init_declarator_list* get_init_declarator_list() const noexcept
	{ return _idl; }

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration_specifiers &_ds;
	init_declarator_list *_idl;
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

	const declarator& get_declarator() const noexcept
	{
	  return _d;
	}

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	abstract_declarator *_ad;
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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
			 stmt_while, stmt_do,
			 stmt_for_init_expr, stmt_for_init_decl,
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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration &_d;
      };

      class block_item_stmt final : public block_item
      {
      public:
	block_item_stmt(stmt* &&s) noexcept;

	virtual ~block_item_stmt() noexcept override;

	const stmt& get_stmt() const noexcept
	{ return _s; }

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr &_e;
	stmt &_s;
      };

      class stmt_for_init_expr final : public stmt
      {
      public:
	stmt_for_init_expr(const pp_tokens_range &tr,
			   expr* &&init, expr* &&cond, expr* &&next,
			   stmt* &&s) noexcept;

	virtual ~stmt_for_init_expr() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	expr *_init;
	expr *_cond;
	expr *_next;
	stmt &_s;
      };

      class stmt_for_init_decl final : public stmt
      {
      public:
	stmt_for_init_decl(const pp_tokens_range &tr,
			   declaration* &&d, expr* &&cond, expr* &&next,
			   stmt* &&s) noexcept;

	virtual ~stmt_for_init_decl() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration &_d;
	expr *_cond;
	expr *_next;
	stmt &_s;
      };

      class stmt_goto final : public stmt
      {
      public:
	stmt_goto(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~stmt_goto() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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
		    const pp_token_index constraint_tok, expr* &&e) noexcept;

	virtual ~asm_operand() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	asm_operand_name *_aon;
	pp_token_index _constraint_tok;
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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

	asm_clobber_list(const pp_token_index clobber_tok);

	virtual ~asm_clobber_list() noexcept override;

	void extend(const pp_token_index clobber_tok);

      private:
	virtual _ast_entity* _get_child(const size_t) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<pp_token_index> _clobber_toks;
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
	virtual _ast_entity* _get_child(const size_t) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

	bool is_at_file_scope() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	declaration &_d;
      };

      class external_declaration_func final : public external_declaration
      {
      public:
	external_declaration_func(function_definition* &&fd) noexcept;

	virtual ~external_declaration_func() noexcept override;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

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

	translation_unit(external_declaration* &&ed);

	virtual ~translation_unit() noexcept override;

	void extend(external_declaration* &&ed);

	void extend_tokens_range(const pp_tokens_range &tr) noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept override;

	virtual void _process(processor<void> &p) override;
	virtual void _process(const_processor<void> &p) const override;
	virtual bool _process(processor<bool> &p) override;
	virtual bool _process(const_processor<bool> &p) const override;

	std::vector<std::reference_wrapper<external_declaration> > _eds;
      };


      class ast
      {
      public:
	typedef std::vector<std::unique_ptr<header_inclusion_root> >
	header_inclusion_roots_type;

	ast(header_inclusion_roots_type &&hirs, pp_tokens &&tokens,
	    std::unique_ptr<translation_unit> &&tu);

	template <typename handled_types, typename callables_wrapper_type>
	void for_each_dfs_po(callables_wrapper_type &&c);

	template <typename handled_types_pre,
		  typename handled_types_post,
		  typename callables_wrapper_type_pre,
		  typename callables_wrapper_type_post>
	void for_each_dfs_pre_and_po(callables_wrapper_type_pre &&c_pre,
				     callables_wrapper_type_post &&c_post);

	const pp_tokens& get_pp_tokens() const noexcept
	{ return _tokens; }

	void resolve();

	code_remarks& get_remarks() noexcept
	{ return _remarks; }

      private:
	void _register_labels();

	void _resolve_ids();

	header_inclusion_roots_type _hirs;
	pp_tokens _tokens;
	std::unique_ptr<translation_unit> _tu;

	code_remarks _remarks;
      };
    }
  }
}

#endif
