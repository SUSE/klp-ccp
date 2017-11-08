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

      protected:
	void _extend_tokens_range(const pp_tokens_range &tr) noexcept;

	_ast_entity *_parent;

      private:
	friend class ast;

	virtual _ast_entity* _get_child(const size_t i) noexcept = 0;

	template <typename callable_type>
	void for_each_dfs_po(callable_type &&c);

	pp_tokens_range _tokens_range;
      };

      template <typename callable_type>
      void _ast_entity::for_each_dfs_po(callable_type &&c)
      {
	  for (std::size_t i_child = 0;; ++i_child) {
	    _ast_entity * const ae = _get_child(i_child);
	    if (!ae)
	      break;
	    ae->for_each_dfs_po(std::forward<callable_type>(c));
	  }

	  c(this);
      }

      template <typename U>
      using parents_of = typename U::parent_types;

      template <typename derived>
      class ast_entity : public _ast_entity
      {
      public:
	ast_entity(const pp_tokens_range &tr) noexcept;

	ast_entity(const ast_entity &ae) noexcept;

	virtual ~ast_entity() noexcept = default;


	template<typename mask_type_set, typename callable_type>
	bool for_each_parent(callable_type &&c)
	{
	  bool r = true;
	  _ast_entity *p = _parent;
	  typedef
	    typename
	    derived::parent_types::template closure<parents_of, mask_type_set>
	    ancestor_types;

	  while (p && r) {
	    r = ancestor_types::cast_and_call(std::forward<callable_type>(c),
					      *p);

	    p = p->get_parent();
	  }

	  return r;
	};

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
      class expr_builtin_offsetof;
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

      class expr_list : public ast_entity<expr_list>
      {
      public:
	typedef type_set<attribute, expr_func_invocation> parent_types;

	expr_list(expr* &&e);

	expr_list(expr_list &&el);

	virtual ~expr_list() noexcept;

	void extend(expr* &&e);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<expr> > _exprs;
      };

      class expr : public ast_entity<expr>
      {
      public:
	typedef type_set<expr_list,
			 expr_cast, expr_unop_pre,
			 expr_sizeof_expr, expr_alignof_expr,
			 expr_builtin_offsetof, expr_array_subscript,
			 expr_func_invocation, expr_member_deref,
			 expr_unop_post, expr_parenthesized, expr_comma,
			 expr_assignment, expr_conditional, expr_binop,
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

	virtual ~expr() noexcept;

      protected:
	expr(const pp_tokens_range &tr) noexcept;
      };

      class expr_comma : public expr
      {
      public:
	expr_comma(expr* &&l, expr* &&r) noexcept;

	virtual ~expr_comma() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

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

      class expr_assignment : public expr
      {
      public:
	expr_assignment(const assign_op op, expr* &&lhs, expr* &&rhs) noexcept;

	virtual ~expr_assignment() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	assign_op _op;
	expr &_lhs;
	expr &_rhs;
      };

      class expr_conditional : public expr
      {
      public:
	expr_conditional(expr* &&cond, expr* &&expr_true, expr* &&expr_false)
	  noexcept;

	expr_conditional(expr* &&cond, expr* &&expr_false) noexcept;

	virtual ~expr_conditional() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

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

      class expr_binop : public expr
      {
      public:
	expr_binop(const binary_op op, expr* &&l, expr* &&r) noexcept;

	virtual ~expr_binop() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	binary_op _op;
	expr &_left;
	expr &_right;
      };

      class expr_cast : public expr
      {
      public:
	expr_cast(const pp_tokens_range &tr, type_name* &&tn, expr* &&e)
	  noexcept;

	virtual ~expr_cast() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

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

      class expr_label_addr : public expr
      {
      public:
	expr_label_addr(const pp_tokens_range &tr,
			const pp_token_index label_tok) noexcept;

	virtual ~expr_label_addr() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _label_tok;
      };

      class expr_unop_pre : public expr
      {
      public:
	expr_unop_pre(const pp_tokens_range &tr,
		      const unary_op_pre op, expr* &&e) noexcept;

	virtual ~expr_unop_pre() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	unary_op_pre _op;
	expr &_e;
      };

      class expr_sizeof_expr : public expr
      {
      public:
	expr_sizeof_expr(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~expr_sizeof_expr() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
      };

      class expr_sizeof_type_name : public expr
      {
      public:
	expr_sizeof_type_name(const pp_tokens_range &tr, type_name* &&tn)
	  noexcept;

	virtual ~expr_sizeof_type_name() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	type_name &_tn;
      };

      class expr_alignof_expr : public expr
      {
      public:
	expr_alignof_expr(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~expr_alignof_expr() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
      };

      class expr_alignof_type_name : public expr
      {
      public:
	expr_alignof_type_name(const pp_tokens_range &tr, type_name* &&tn)
	  noexcept;

	virtual ~expr_alignof_type_name() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	type_name &_tn;
      };

      class expr_builtin_offsetof : public expr
      {
      public:
	expr_builtin_offsetof(const pp_tokens_range &tr, type_name* &&tn,
			      expr *&&member_designator)
	  noexcept;

	virtual ~expr_builtin_offsetof() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	type_name &_tn;
	expr &_member_designator;
      };

      class expr_builtin_types_compatible_p : public expr
      {
      public:
	expr_builtin_types_compatible_p(const pp_tokens_range &tr,
					type_name* &&tn1, type_name* &&tn2)
	  noexcept;

	virtual ~expr_builtin_types_compatible_p() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	type_name &_tn1;
	type_name &_tn2;
      };

      class expr_array_subscript : public expr
      {
      public:
	expr_array_subscript(const pp_tokens_range &tr,
			     expr* &&base, expr* &&index) noexcept;

	virtual ~expr_array_subscript() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_base;
	expr &_index;
      };

      class expr_func_invocation : public expr
      {
      public:
	expr_func_invocation(const pp_tokens_range &tr,
			     expr* &&func, expr_list* &&args) noexcept;

	expr_func_invocation(const pp_tokens_range &tr, expr* &&func) noexcept;

	virtual ~expr_func_invocation() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_func;
	expr_list *_args;
      };

      enum class member_deref_type
      {
	ptr_base,
	non_ptr_base,
      };

      class expr_member_deref : public expr
      {
      public:
	expr_member_deref(const pp_tokens_range &tr,
			  const member_deref_type deref_type,
			  expr* &&base, const pp_token_index member_tok)
	  noexcept;

	virtual ~expr_member_deref() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	member_deref_type _deref_type;
	expr &_base;
	pp_token_index _member_tok;
      };

      enum class unary_op_post
      {
	inc,
	dec,
      };

      class expr_unop_post : public expr
      {
      public:
	expr_unop_post(const pp_tokens_range &tr,
		       const unary_op_post op, expr* &&e) noexcept;

	virtual ~expr_unop_post() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	unary_op_post _op;
	expr &_e;
      };

      class expr_compound_literal : public expr
      {
      public:
	expr_compound_literal(const pp_tokens_range &tr,
			      type_name* &&tn, initializer_list* &&il) noexcept;

	virtual ~expr_compound_literal() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	type_name &_tn;
	initializer_list *_il;
      };

      class expr_statement : public expr
      {
      public:
	expr_statement(const pp_tokens_range &tr, stmt* &&s) noexcept;

	virtual ~expr_statement() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	stmt &_s;
      };

      class expr_id : public expr
      {
      public:
	expr_id(const pp_token_index id_tok) noexcept;

	virtual ~expr_id() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _id_tok;
      };

      class expr_constant : public expr
      {
      public:
	expr_constant(const pp_token_index const_tok) noexcept;

	virtual ~expr_constant() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _const_tok;
      };

      class string_literal : public ast_entity<string_literal>
      {
      public:
	typedef type_set<expr_string_literal,
			 asm_directive,
			 asm_label> parent_types;

	string_literal(const pp_token_index s);

	virtual ~string_literal() noexcept;

	void extend(const pp_token_index s);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<pp_token_index> _strings;
      };

      class expr_string_literal : public expr
      {
      public:
	expr_string_literal(string_literal* &&sl) noexcept;

	virtual ~expr_string_literal() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	string_literal &_sl;
      };

      class expr_parenthesized : public expr
      {
      public:
	expr_parenthesized(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~expr_parenthesized() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
      };


      class attribute : public ast_entity<attribute>
      {
      public:
	typedef type_set<attribute_list> parent_types;

	attribute(const pp_token_index name_tok) noexcept;
	attribute(const pp_tokens_range &tr, const pp_token_index name_tok,
		  expr_list* &&params) noexcept;

	virtual ~attribute() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _name_tok;
	expr_list *_params;
      };

      class attribute_list : public ast_entity<attribute_list>
      {
      public:
	typedef type_set<attribute_specifier> parent_types;

	attribute_list(const pp_tokens_range &tr, attribute* &&a);

	~attribute_list() noexcept;

	void extend(const pp_tokens_range &tr, attribute* &&a);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<attribute*> _attributes;
      };

      class attribute_specifier : public ast_entity<attribute_specifier>
      {
      public:
	typedef type_set<attribute_specifier_list> parent_types;

      public:
	attribute_specifier(const pp_tokens_range &tr,
			    attribute_list* &&al) noexcept;

	virtual ~attribute_specifier() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	attribute_list &_al;
      };

      class attribute_specifier_list
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
			 parameter_declaration_declarator> parent_types;

	attribute_specifier_list(attribute_specifier* &&as);

	virtual ~attribute_specifier_list() noexcept;

	void extend(attribute_specifier* &&as);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<attribute_specifier> > _ass;
      };

      class pointer : public ast_entity<pointer>
      {
      public:
	typedef type_set<abstract_declarator, declarator> parent_types;

	pointer(const pp_tokens_range &tr, type_qualifier_list* &&tql);

	virtual ~pointer() noexcept;

	void extend(const pp_tokens_range &tr, type_qualifier_list* &&tql);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

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

      protected:
	direct_abstract_declarator(const pp_tokens_range &tr) noexcept;
      };

      class direct_abstract_declarator_parenthesized
	: public direct_abstract_declarator
      {
      public:
	direct_abstract_declarator_parenthesized(
					const pp_tokens_range &tr,
					attribute_specifier_list* &&asl,
					abstract_declarator* &&ad) noexcept;

	virtual ~direct_abstract_declarator_parenthesized() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	attribute_specifier_list *_asl;
	abstract_declarator &_ad;
      };

      class direct_abstract_declarator_array : public direct_abstract_declarator
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

	virtual ~direct_abstract_declarator_array() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	direct_abstract_declarator *_dad;
	type_qualifier_list *_tql;
	expr *_size;
	bool _static;
	bool _vla_unspec_size;
      };

      class direct_abstract_declarator_func : public direct_abstract_declarator
      {
      public:
	direct_abstract_declarator_func(const pp_tokens_range &tr,
					direct_abstract_declarator* &&dad,
					parameter_declaration_list* &&ptl)
	  noexcept;

	virtual ~direct_abstract_declarator_func() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	direct_abstract_declarator *_dad;
	parameter_declaration_list *_ptl;
      };

      class abstract_declarator : public ast_entity<abstract_declarator>
      {
      public:
	typedef type_set<type_name, direct_abstract_declarator_parenthesized,
			 parameter_declaration_abstract> parent_types;

	abstract_declarator(const pp_tokens_range &tr, pointer* &&pt,
			    direct_abstract_declarator* &&dad) noexcept;

	virtual ~abstract_declarator() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pointer *_pt;
	direct_abstract_declarator *_dad;
      };

      class type_name : public ast_entity<type_name>
      {
      public:
	typedef type_set<typeof_type_name, expr_sizeof_type_name,
			 expr_alignof_type_name, expr_builtin_offsetof,
			 expr_builtin_types_compatible_p,
			 expr_cast, expr_compound_literal,
			 typeof_type_name> parent_types;

	type_name(const pp_tokens_range &tr, specifier_qualifier_list* &&sql,
		  abstract_declarator* &&ad) noexcept;

	virtual ~type_name() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	specifier_qualifier_list &_sql;
	abstract_declarator *_ad;
      };

      class direct_declarator : public ast_entity<direct_declarator>
      {
      public:
	typedef type_set<declarator, direct_declarator_array,
			 direct_declarator_func> parent_types;

	virtual ~direct_declarator() noexcept;

	virtual pp_token_index get_id_tok() const noexcept = 0;

      protected:
	direct_declarator(const pp_tokens_range &tr) noexcept;
      };

      class direct_declarator_id : public direct_declarator
      {
      public:
	direct_declarator_id(const pp_token_index id_tok) noexcept;

	virtual ~direct_declarator_id() noexcept;

	virtual pp_token_index get_id_tok() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _id_tok;
      };

      class direct_declarator_parenthesized : public direct_declarator
      {
      public:
	direct_declarator_parenthesized(const pp_tokens_range &tr,
					declarator* &&d,
					attribute_specifier_list* &&asl)
	  noexcept;

	virtual ~direct_declarator_parenthesized() noexcept;

	virtual pp_token_index get_id_tok() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declarator &_d;
	attribute_specifier_list *_asl;
      };

      class direct_declarator_array : public direct_declarator
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

	virtual ~direct_declarator_array() noexcept;

	virtual pp_token_index get_id_tok() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	direct_declarator &_dd;
	type_qualifier_list *_tql;
	expr *_size;
	bool _static;
	bool _vla_unspec_size;
      };

      class direct_declarator_func : public direct_declarator
      {
      public:
	typedef type_set<struct_declaration_c99, function_definition,
			 parameter_declaration_declarator,
			 init_declarator_list>parent_types;

	direct_declarator_func(const pp_tokens_range &tr,
			       direct_declarator* &&dd,
			       parameter_declaration_list* &&ptl)
	  noexcept;

	direct_declarator_func(const pp_tokens_range &tr,
			       direct_declarator* &&dd,
			       identifier_list* &&il)
	  noexcept;

	virtual ~direct_declarator_func() noexcept;

	virtual pp_token_index get_id_tok() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	direct_declarator &_dd;
	parameter_declaration_list *_ptl;
	identifier_list *_il;
      };

      class declarator : public ast_entity<declarator>
      {
      public:
	typedef type_set<direct_declarator_parenthesized,
			 struct_declarator,
			 init_declarator,
			 parameter_declaration_declarator,
			 function_definition> parent_types;

	declarator(const pp_tokens_range &tr, pointer* &&pt,
		   direct_declarator* &&dd) noexcept;

	virtual ~declarator() noexcept;

	pp_token_index get_id_tok() const noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pointer *_pt;
	direct_declarator &_dd;
      };

      enum class storage_class
      {
	sc_typedef,
	sc_extern,
	sc_static,
	sc_auto,
	sc_register,
      };

      class storage_class_specifier : public ast_entity<storage_class_specifier>
      {
      public:
	typedef type_set<specifier_qualifier_list> parent_types;

      public:
	storage_class_specifier(const pp_tokens_range &tr,
				const storage_class sc) noexcept;

	virtual ~storage_class_specifier() noexcept;

	storage_class get_storage_class() const noexcept
	{ return _sc; }

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	storage_class _sc;
      };

      enum class type_qualifier_type
      {
	tqt_const,
	tqt_restrict,
	tqt_volatile,
      };

      class type_qualifier : public ast_entity<type_qualifier>
      {
      public:
	typedef type_set<specifier_qualifier_list,
			 type_qualifier_list> parent_types;

      public:
	type_qualifier(const pp_tokens_range &tr,
		       const type_qualifier_type &tqt) noexcept;

	virtual ~type_qualifier() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	type_qualifier_type _tqt;
      };

      class type_qualifier_list : public ast_entity<type_qualifier_list>
      {
      public:
	typedef type_set<pointer,
			 direct_abstract_declarator_array,
			 direct_declarator_array> parent_types;

	type_qualifier_list(type_qualifier* &&tq);

	type_qualifier_list(attribute_specifier_list* &&asl);

	virtual ~type_qualifier_list() noexcept;

	void extend(type_qualifier* &&tq);

	void extend(attribute_specifier_list* &&asl);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<type_qualifier> > _tqs;
	std::vector<std::reference_wrapper<attribute_specifier_list> > _asls;
      };

      class type_specifier : public ast_entity<type_specifier>
      {
      public:
	typedef type_set<specifier_qualifier_list> parent_types;

	type_specifier(const pp_tokens_range &tr) noexcept;

	virtual ~type_specifier() noexcept;
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

      class type_specifier_pod : public type_specifier
      {
      public:
	type_specifier_pod(const pp_tokens_range &tr,
			   const pod_spec &pod_spec) noexcept;

	virtual ~type_specifier_pod() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pod_spec _pod_spec;
      };

      class type_specifier_tdid : public type_specifier
      {
      public:
	type_specifier_tdid(const pp_token_index tdid_tok) noexcept;

	virtual ~type_specifier_tdid() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _tdid_tok;
      };

      class struct_declaration : public ast_entity<struct_declaration>
      {
      public:
	typedef type_set<struct_declaration_list> parent_types;

	struct_declaration(const pp_tokens_range &tr,
			   specifier_qualifier_list* &&sql) noexcept;

	virtual ~struct_declaration() noexcept;

	specifier_qualifier_list* get_specifier_qualifier_list() noexcept
	{ return _sql; }

      protected:
	specifier_qualifier_list *_sql;
      };

      class struct_declarator : public ast_entity<struct_declarator>
      {
      public:
	typedef type_set<struct_declarator_list> parent_types;

	struct_declarator(const pp_tokens_range &tr,
			  declarator* &&d, expr* &&width,
			  attribute_specifier_list* &&asl_after) noexcept;

	virtual ~struct_declarator() noexcept;

	void set_asl_before(attribute_specifier_list* &&asl_before) noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declarator *_d;
	expr *_width;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
      };

      class struct_declarator_list : public ast_entity<struct_declarator_list>
      {
      public:
	typedef type_set<struct_declaration_c99> parent_types;

	struct_declarator_list(struct_declarator* &&sd);

	virtual ~struct_declarator_list() noexcept;

	void extend(struct_declarator* &&sd);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<struct_declarator> > _sds;
      };

      class struct_declaration_c99 : public struct_declaration
      {
      public:
	struct_declaration_c99(const pp_tokens_range &tr,
			       specifier_qualifier_list* &&sql,
			       struct_declarator_list * &&sdl) noexcept;

	virtual ~struct_declaration_c99() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	struct_declarator_list &_sdl;
      };

      enum class struct_or_union
      {
	sou_struct,
	sou_union,
      };

      class unnamed_struct_or_union : public ast_entity<unnamed_struct_or_union>
      {
      public:
	typedef type_set<struct_declaration_unnamed_sou> parent_types;

	unnamed_struct_or_union(const pp_tokens_range &tr,
				const struct_or_union sou,
				struct_declaration_list* &&sdl,
				attribute_specifier_list* &&asl_before,
				attribute_specifier_list* &&asl_after) noexcept;

	virtual ~unnamed_struct_or_union() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	struct_or_union _sou;
	struct_declaration_list *_sdl;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
      };

      class struct_declaration_unnamed_sou : public struct_declaration
      {
      public:
	struct_declaration_unnamed_sou(const pp_tokens_range &tr,
				       specifier_qualifier_list* &&sql,
				       unnamed_struct_or_union* &&unnamed_sou)
	  noexcept;

	virtual ~struct_declaration_unnamed_sou() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	unnamed_struct_or_union &_unnamed_sou;
      };

      class struct_declaration_list : public ast_entity<struct_declaration_list>
      {
      public:
	typedef type_set<struct_or_union_def,
			 unnamed_struct_or_union> parent_types;

      public:
	struct_declaration_list(struct_declaration* &&sd);

	virtual ~struct_declaration_list() noexcept;

	void extend(struct_declaration* &&sd);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<struct_declaration> > _sds;
      };

      class struct_or_union_def : public type_specifier
      {
      public:
	struct_or_union_def(const pp_tokens_range &tr,
			    const struct_or_union sou,
			    const pp_token_index id_tok,
			    struct_declaration_list* &&sdl,
			    attribute_specifier_list* &&asl_before,
			    attribute_specifier_list* &&asl_after) noexcept;

	struct_or_union_def(const pp_tokens_range &tr,
			    const struct_or_union sou,
			    struct_declaration_list* &&sdl,
			    attribute_specifier_list* &&asl_before,
			    attribute_specifier_list* &&asl_after) noexcept;

	virtual ~struct_or_union_def() noexcept;

      private:
	struct_or_union_def(const pp_tokens_range &tr,
			    const struct_or_union sou,
			    const pp_token_index id_tok,
			    struct_declaration_list* &&sdl,
			    attribute_specifier_list* &&asl_before,
			    attribute_specifier_list* &&asl_after,
			    const bool id_tok_valid) noexcept;

	virtual _ast_entity* _get_child(const size_t i) noexcept;

	struct_or_union _sou;
	pp_token_index _id_tok;
	struct_declaration_list *_sdl;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
	bool _id_tok_valid;
      };

      class struct_or_union_ref : public type_specifier
      {
      public:
	struct_or_union_ref(const pp_tokens_range &tr,
			    const struct_or_union sou,
			    const pp_token_index id_tok,
			    attribute_specifier_list* &&asl) noexcept;

	virtual ~struct_or_union_ref() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	struct_or_union _sou;
	pp_token_index _id_tok;
	attribute_specifier_list *_asl;
      };


      class enumerator : public ast_entity<enumerator>
      {
      public:
	typedef type_set<enumerator_list> parent_types;

	enumerator(const pp_tokens_range &tr, const pp_token_index id_tok,
		   expr* &&value) noexcept;

	virtual ~enumerator() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _id_tok;
	expr *_value;
      };

      class enumerator_list : public ast_entity<enumerator_list>
      {
      public:
	typedef type_set<enum_def> parent_types;

	enumerator_list(enumerator* &&e);

	virtual ~enumerator_list() noexcept;

	void extend(enumerator* &&e);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<enumerator> > _es;
      };

      class enum_def : public type_specifier
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

	virtual ~enum_def() noexcept;

      private:
	enum_def(const pp_tokens_range &tr,
		 const pp_token_index id_tok,
		 enumerator_list* &&el,
		 attribute_specifier_list* &&asl_before,
		 attribute_specifier_list* &&asl_after,
		 const bool id_tok_valid) noexcept;

	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _id_tok;
	enumerator_list &_el;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_after;
	bool _id_tok_valid;
      };

      class enum_ref : public type_specifier
      {
      public:
	enum_ref(const pp_tokens_range &tr,
		 const pp_token_index id_tok,
		 attribute_specifier_list* &&asl) noexcept;

	virtual ~enum_ref() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _id_tok;
	attribute_specifier_list *_asl;
      };

      class typeof_expr : public type_specifier
      {
      public:
	typeof_expr(const pp_tokens_range &tr,
		    expr* &&e) noexcept;

	virtual ~typeof_expr() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
      };

      class typeof_type_name : public type_specifier
      {
      public:
	typeof_type_name(const pp_tokens_range &tr,
			 type_name* &&tn) noexcept;

	virtual ~typeof_type_name() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	type_name &_tn;
      };

      class function_specifier : public ast_entity<function_specifier>
      {
      public:
	typedef type_set<specifier_qualifier_list> parent_types;
      public:
	function_specifier(const pp_token_index spec_tok) noexcept;

	virtual ~function_specifier() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

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

      protected:
	specifier_qualifier_list(const pp_tokens_range &tr) noexcept;

	std::size_t _n_children() const noexcept;

	virtual _ast_entity* _get_child(const size_t i) noexcept;

      private:
	std::vector<std::reference_wrapper<type_specifier> > _tss;
	std::vector<std::reference_wrapper<type_qualifier> > _tqs;
	std::vector<std::reference_wrapper<attribute_specifier_list> > _asls;
      };

      class declaration_specifiers : public specifier_qualifier_list
      {
      public:
	declaration_specifiers(storage_class_specifier* &&scs);

	declaration_specifiers(function_specifier* &&fs);

	declaration_specifiers(type_specifier* &&ts);

	declaration_specifiers(type_qualifier* &&tq);

	declaration_specifiers(attribute_specifier_list* &&asl);

	virtual ~declaration_specifiers() noexcept;

	void extend(storage_class_specifier* &&scs);

	void extend(function_specifier* &&fs);

	void extend(type_specifier* &&ts);

	void extend(type_qualifier* &&tq);

	void extend(attribute_specifier_list* &&asl);

	void extend(declaration_specifiers* &&ds);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<storage_class_specifier> > _scss;
	std::vector<std::reference_wrapper<function_specifier> > _fss;
      };

      class initializer : public ast_entity<initializer>
      {
      public:
	typedef type_set<init_declarator, initializer_list> parent_types;

	virtual ~initializer() noexcept;

	virtual void set_designation(designation* &&d) noexcept = 0;

      protected:
	void _set_designation(designation* &&d) noexcept;

	initializer(const pp_tokens_range &tr) noexcept;

      protected:
	designation *_d;
      };

      class initializer_expr : public initializer
      {
      public:
	initializer_expr(expr* &&e) noexcept;

	virtual ~initializer_expr() noexcept;

	virtual void set_designation(designation* &&d) noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
      };

      class initializer_init_list : public initializer
      {
      public:
	initializer_init_list(const pp_tokens_range &tr,
			      initializer_list* &&il) noexcept;

	virtual ~initializer_init_list() noexcept;

	virtual void set_designation(designation* &&d) noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	initializer_list *_il;
      };

      class designator : public ast_entity<designator>
      {
      public:
	typedef type_set<designator_list> parent_types;

	designator(const pp_tokens_range &tr) noexcept;

	virtual ~designator() noexcept;
      };

      class designator_member : public designator
      {
      public:
	designator_member(const pp_tokens_range &tr,
			  const pp_token_index member_tok) noexcept;

	virtual ~designator_member() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _member_tok;
      };

      class designator_array : public designator
      {
      public:
	designator_array(const pp_tokens_range &tr,
			 expr* &&index_first, expr* &&index_last) noexcept;

	virtual ~designator_array() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_index_first;
	expr *_index_last;
      };

      class designator_list : public ast_entity<designator_list>
      {
      public:
	typedef type_set<designation> parent_types;

      public:
	designator_list(designator* &&d);

	virtual ~designator_list() noexcept;

	void extend(designator* &&d);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<designator> > _ds;
      };

      class designation : public ast_entity<designation>
      {
      public:
	typedef type_set<initializer_expr, initializer_init_list> parent_types;

      public:
	designation(const pp_tokens_range &tr,
		    designator_list* &&dl) noexcept;

	virtual ~designation() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	designator_list &_dl;
      };

      class initializer_list : public ast_entity<initializer_list>
      {
      public:
	typedef type_set<initializer_init_list,
			 expr_compound_literal> parent_types;

	initializer_list(initializer* &&i);

	virtual ~initializer_list() noexcept;

	void extend(initializer* &&i);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<initializer> > _is;
      };


      class asm_label : public ast_entity<asm_label>
      {
      public:
	typedef type_set<init_declarator> parent_types;

      public:
	asm_label(const pp_tokens_range &tr,
		  string_literal* &&label) noexcept;

	virtual ~asm_label() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	string_literal &_label;
      };

      class init_declarator : public ast_entity<init_declarator>
      {
      public:
	typedef type_set<init_declarator_list> parent_types;

	init_declarator(const pp_tokens_range &tr,
			declarator* &&d, initializer* &&i, asm_label* &&al,
			attribute_specifier_list* &&asl_middle,
			attribute_specifier_list* &&asl_after) noexcept;

	virtual ~init_declarator() noexcept;

	void set_asl_before(attribute_specifier_list* &&asl_before) noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declarator &_d;
	initializer *_i;
	asm_label *_al;
	attribute_specifier_list *_asl_before;
	attribute_specifier_list *_asl_middle;
	attribute_specifier_list *_asl_after;
      };

      class init_declarator_list : public ast_entity<init_declarator_list>
      {
      public:
	typedef type_set<declaration> parent_types;

	init_declarator_list(init_declarator* &&id);

	virtual ~init_declarator_list() noexcept;

	void extend(init_declarator* &&id);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<init_declarator> > _ids;
      };

      class declaration : public ast_entity<declaration>
      {
      public:
	typedef type_set<declaration_list,
			 block_item_decl,
			 stmt_for_init_decl,
			 external_declaration_decl> parent_types;

      public:
	declaration(const pp_tokens_range &tr, declaration_specifiers* &&ds,
		    init_declarator_list* &&idl) noexcept;

	virtual ~declaration() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declaration_specifiers &_ds;
	init_declarator_list *_idl;
      };

      class parameter_declaration : public ast_entity<parameter_declaration>
      {
      public:
	typedef type_set<parameter_declaration_list> parent_types;

	parameter_declaration(const pp_tokens_range &tr,
			      declaration_specifiers* &&ds) noexcept;

	virtual ~parameter_declaration() noexcept;

	declaration_specifiers& get_declaration_specifiers() noexcept
	{ return _ds; }

      protected:
	declaration_specifiers &_ds;
      };

      class parameter_declaration_declarator : public parameter_declaration
      {
      public:
	parameter_declaration_declarator(const pp_tokens_range &tr,
					 declaration_specifiers* &&ds,
					 declarator* &&d,
					 attribute_specifier_list* &&asl)
	  noexcept;

	virtual ~parameter_declaration_declarator() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declarator &_d;
	attribute_specifier_list *_asl;
      };

      class parameter_declaration_abstract
	: public parameter_declaration
      {
      public:
	parameter_declaration_abstract(const pp_tokens_range &tr,
				       declaration_specifiers* &&ds,
				       abstract_declarator* &&ad)
	  noexcept;

	virtual ~parameter_declaration_abstract() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	abstract_declarator *_ad;
      };

      class parameter_declaration_list
	: public ast_entity<parameter_declaration_list>
      {
      public:
	typedef type_set<direct_abstract_declarator_func,
			 direct_declarator_func> parent_types;

	parameter_declaration_list(parameter_declaration* &&pd);

	virtual ~parameter_declaration_list() noexcept;

	void extend(parameter_declaration* &&pd);

	void set_variadic(const pp_tokens_range &triple_dot_tr) noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<parameter_declaration> > _pds;
	bool _variadic;
      };

      class identifier_list : public ast_entity<identifier_list>
      {
      public:
	typedef type_set<direct_abstract_declarator_func,
			 direct_declarator_func,
			 local_label_declaration> parent_types;

	identifier_list(const pp_token_index id_tok);

	virtual ~identifier_list() noexcept;

	void extend(const pp_token_index id_tok);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<pp_token_index> _ids;
      };

      class declaration_list : public ast_entity<declaration_list>
      {
      public:
	typedef type_set<function_definition> parent_types;

	declaration_list(declaration* &&d);

	virtual ~declaration_list() noexcept;

	void extend(declaration* &&d);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

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

	virtual ~stmt() noexcept;

      protected:
	stmt(const pp_tokens_range &tr) noexcept;
      };

      class stmt_labeled : public stmt
      {
      public:
	stmt_labeled(const pp_tokens_range &tr,
		     const pp_token_index label_tok,
		     stmt* &&s, attribute_specifier_list* &&asl) noexcept;

	virtual ~stmt_labeled() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _label_tok;
	stmt &_s;
	attribute_specifier_list *_asl;
      };

      class stmt_case : public stmt
      {
      public:
	stmt_case(const pp_tokens_range &tr,
		  expr* &&e, stmt* &&s) noexcept;

	virtual ~stmt_case() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
	stmt &_s;
      };

      class stmt_case_range : public stmt
      {
      public:
	stmt_case_range(const pp_tokens_range &tr,
			expr* &&e_low, expr* &&e_high, stmt* &&s) noexcept;

	virtual ~stmt_case_range() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e_low;
	expr &_e_high;
	stmt &_s;
      };

      class stmt_default : public stmt
      {
      public:
	stmt_default(const pp_tokens_range &tr, stmt* &&s) noexcept;

	~stmt_default() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	stmt &_s;
      };


      class local_label_declaration : public ast_entity<local_label_declaration>
      {
      public:
	typedef type_set<local_label_declaration_list> parent_types;

	local_label_declaration(const pp_tokens_range &tr,
				identifier_list* &&idl) noexcept;

	virtual ~local_label_declaration() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	identifier_list &_idl;
      };

      class local_label_declaration_list
	: public ast_entity<local_label_declaration_list>
      {
      public:
	typedef type_set<stmt_compound> parent_types;

	local_label_declaration_list(local_label_declaration* &&lld);

	virtual ~local_label_declaration_list() noexcept;

	void extend(local_label_declaration* &&lld);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<local_label_declaration> > _llds;
      };

      class block_item : public ast_entity<block_item>
      {
      public:
	typedef type_set<block_item_list> parent_types;

	block_item(const pp_tokens_range &tr) noexcept;

	virtual ~block_item() noexcept;
      };

      class block_item_decl : public block_item
      {
      public:
	block_item_decl(declaration* &&d) noexcept;

	virtual ~block_item_decl() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declaration &_d;
      };

      class block_item_stmt : public block_item
      {
      public:
	block_item_stmt(stmt* &&s) noexcept;

	virtual ~block_item_stmt() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	stmt &_s;
      };

      class block_item_function_definition : public block_item
      {
      public:
	block_item_function_definition(function_definition* &&fd) noexcept;

	virtual ~block_item_function_definition() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	function_definition &_fd;
      };

      class block_item_list : public ast_entity<block_item_list>
      {
      public:
	typedef type_set<stmt_compound> parent_types;

	block_item_list(block_item* &&bi);

	virtual ~block_item_list() noexcept;

	void extend(block_item* &&bi);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<block_item> > _bis;
      };

      class stmt_compound : public stmt
      {
      public:
	stmt_compound(const pp_tokens_range &tr,
		      local_label_declaration_list* &&lldl,
		      block_item_list* &&bil) noexcept;

	virtual ~stmt_compound() noexcept;

	void register_label(stmt_labeled * const label);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	local_label_declaration_list *_lldl;
	block_item_list *_bil;

	std::vector<stmt_labeled*> _labels;
      };

      class stmt_expr : public stmt
      {
      public:
	stmt_expr(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~stmt_expr() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr *_e;
      };

      class stmt_if : public stmt
      {
      public:
	stmt_if(const pp_tokens_range &tr,
		expr* &&cond, stmt* &&s_true, stmt* &&s_false)
	  noexcept;

	virtual ~stmt_if() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_cond;
	stmt &_s_true;
	stmt *_s_false;
      };

      class stmt_switch : public stmt
      {
      public:
	stmt_switch(const pp_tokens_range &tr, expr* &&e, stmt* &&s) noexcept;

	virtual ~stmt_switch() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
	stmt &_s;
      };

      class stmt_while : public stmt
      {
      public:
	stmt_while(const pp_tokens_range &tr, expr* &&e, stmt* &&s) noexcept;

	virtual ~stmt_while() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
	stmt &_s;
      };

      class stmt_do : public stmt
      {
      public:
	stmt_do(const pp_tokens_range &tr, expr* &&e, stmt* &&s) noexcept;

	virtual ~stmt_do() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
	stmt &_s;
      };

      class stmt_for_init_expr : public stmt
      {
      public:
	stmt_for_init_expr(const pp_tokens_range &tr,
			   expr* &&init, expr* &&cond, expr* &&next,
			   stmt* &&s) noexcept;

	virtual ~stmt_for_init_expr() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr *_init;
	expr *_cond;
	expr *_next;
	stmt &_s;
      };

      class stmt_for_init_decl : public stmt
      {
      public:
	stmt_for_init_decl(const pp_tokens_range &tr,
			   declaration* &&d, expr* &&cond, expr* &&next,
			   stmt* &&s) noexcept;

	virtual ~stmt_for_init_decl() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declaration &_d;
	expr *_cond;
	expr *_next;
	stmt &_s;
      };

      class stmt_goto : public stmt
      {
      public:
	stmt_goto(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~stmt_goto() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr &_e;
      };

      class stmt_continue : public stmt
      {
      public:
	stmt_continue(const pp_tokens_range &tr) noexcept;

	virtual ~stmt_continue() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;
      };

      class stmt_break : public stmt
      {
      public:
	stmt_break(const pp_tokens_range &tr) noexcept;

	virtual ~stmt_break() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;
      };

      class stmt_return : public stmt
      {
      public:
	stmt_return(const pp_tokens_range &tr, expr* &&e) noexcept;

	virtual ~stmt_return() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	expr *_e;
      };

      class stmt_asm : public stmt
      {
      public:
	stmt_asm(asm_directive* &&ad) noexcept;

	virtual ~stmt_asm() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	asm_directive &_ad;
      };


      enum class asm_qualifier
      {
	aq_goto,
	aq_volatile,
      };

      class asm_qualifier_list : public ast_entity<asm_qualifier_list>
      {
      public:
	typedef type_set<asm_directive> parent_types;

	asm_qualifier_list(const pp_tokens_range &tr,
			   const asm_qualifier aq);

	virtual ~asm_qualifier_list() noexcept;

	void extend(const pp_tokens_range &tr,
		    const asm_qualifier aq);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<asm_qualifier> _aqs;
      };

      class asm_operand_name : public ast_entity<asm_operand_name>
      {
      public:
	typedef type_set<asm_operand> parent_types;

	asm_operand_name(const pp_tokens_range &tr,
			 const pp_token_index id_tok) noexcept;

	virtual ~asm_operand_name() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	pp_token_index _id_tok;
      };

      class asm_operand : public ast_entity<asm_operand>
      {
      public:
	typedef type_set<asm_operand_list> parent_types;

	asm_operand(const pp_tokens_range &tr, asm_operand_name* &&aon,
		    const pp_token_index constraint_tok, expr* &&e) noexcept;

	virtual ~asm_operand() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	asm_operand_name *_aon;
	pp_token_index _constraint_tok;
	expr &_e;
      };

      class asm_operand_list : public ast_entity<asm_operand_list>
      {
      public:
	typedef type_set<asm_directive> parent_types;

      public:
	asm_operand_list(asm_operand* &&ao);

	virtual ~asm_operand_list() noexcept;

	void extend(asm_operand* &&ao);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<asm_operand> > _aos;
      };

      class asm_clobber_list : public ast_entity<asm_clobber_list>
      {
      public:
	typedef type_set<asm_directive> parent_types;

	asm_clobber_list(const pp_token_index clobber_tok);

	virtual ~asm_clobber_list() noexcept;

	void extend(const pp_token_index clobber_tok);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<pp_token_index> _clobber_toks;
      };

      class asm_jump_to_label_list : public ast_entity<asm_jump_to_label_list>
      {
      public:
	typedef type_set<asm_directive> parent_types;

	asm_jump_to_label_list(const pp_token_index label_tok);

	virtual ~asm_jump_to_label_list() noexcept;

	void extend(const pp_token_index label_tok);

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<pp_token_index> _label_toks;
      };

      class asm_directive : public ast_entity<asm_directive>
      {
      public:
	typedef type_set<stmt_asm, external_declaration_asm> parent_types;

	asm_directive(const pp_tokens_range &tr,
		      asm_qualifier_list* &&aql, string_literal* &&asm_s,
		      asm_operand_list* &&output_aol,
		      asm_operand_list* &&input_aol, asm_clobber_list* &&acl,
		      asm_jump_to_label_list* &&ajtll)
	  noexcept;

	virtual ~asm_directive() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	asm_qualifier_list *_aql;
	string_literal &_asm_s;
	asm_operand_list *_output_aol;
	asm_operand_list *_input_aol;
	asm_clobber_list *_acl;
	asm_jump_to_label_list *_ajtll;
      };

      class function_definition : public ast_entity<function_definition>
      {
      public:
	typedef type_set<external_declaration_func,
			 block_item_function_definition> parent_types;

	function_definition(const pp_tokens_range &tr,
			    declaration_specifiers* &&ds,
			    declarator* &&d, attribute_specifier_list* &&asl,
			    declaration_list* &&dl, stmt_compound* &&sc)
	  noexcept;

	virtual ~function_definition() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declaration_specifiers &_ds;
	declarator &_d;
	attribute_specifier_list *_asl;
	declaration_list *_dl;
	stmt_compound &_sc;
      };

      class external_declaration : public ast_entity<external_declaration>
      {
      public:
	typedef type_set<translation_unit> parent_types;

	external_declaration(const pp_tokens_range &tr) noexcept;

	virtual ~external_declaration() noexcept;
      };

      class external_declaration_decl : public external_declaration
      {
      public:
	external_declaration_decl(declaration* &&d) noexcept;

	virtual ~external_declaration_decl() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	declaration &_d;
      };

      class external_declaration_func : public external_declaration
      {
      public:
	external_declaration_func(function_definition* &&fd) noexcept;

	virtual ~external_declaration_func() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	function_definition &_fd;
      };

      class external_declaration_asm : public external_declaration
      {
      public:
	external_declaration_asm(asm_directive* &&ad) noexcept;

	virtual ~external_declaration_asm() noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	asm_directive &_ad;
      };


      class translation_unit : public ast_entity<translation_unit>
      {
      public:
	typedef type_set<> parent_types;

	translation_unit(external_declaration* &&ed);

	virtual ~translation_unit() noexcept;

	void extend(external_declaration* &&ed);

	void extend_tokens_range(const pp_tokens_range &tr) noexcept;

      private:
	virtual _ast_entity* _get_child(const size_t i) noexcept;

	std::vector<std::reference_wrapper<external_declaration> > _eds;
      };


      class ast
      {
      public:
	typedef std::vector<std::unique_ptr<header_inclusion_root> >
	header_inclusion_roots_type;

	ast(header_inclusion_roots_type &&hirs, pp_tokens &&tokens,
	    std::unique_ptr<translation_unit> &&tu);

	template <typename callable_type>
	void for_each_dfs_po(callable_type &&c)
	{
	  if (_tu)
	    _tu->for_each_dfs_po(std::forward<callable_type>(c));
	}

	pp_tokens& get_pp_tokens() noexcept
	{ return _tokens; }

      private:
	header_inclusion_roots_type _hirs;
	pp_tokens _tokens;
	std::unique_ptr<translation_unit> _tu;
      };
    }
  }
}

#endif
