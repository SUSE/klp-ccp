%language "C++"
%skeleton "lalr1.cc"
%defines
%define namespace "suse::cp::yy"
%define parser_class_name "gnuc_parser"
%name-prefix "pd."
%define api.location.type pp_tokens_range
%locations

%code requires {
#ifdef DEBUG_PARSER
#define YYDEBUG 1
#endif

#include "types.hh"
#include "ast.hh"

namespace suse {
  namespace cp
  {
    namespace yy
    {
      // Bison doesn't accept namespace specifications at
      // %define api.location.type. Pull the type in.
      typedef suse::cp::ast::pp_tokens_range pp_tokens_range;

      class gnuc_parser_driver;
    }
  }
}
}

%code {
using namespace suse::cp;
using namespace suse::cp::ast;

#define YYLLOC_DEFAULT(Cur, Rhs, N)				\
  do {								\
     if (N) {							\
       Cur = pp_tokens_range{YYRHSLOC(Rhs, 1).begin,		\
			     YYRHSLOC(Rhs, N).end};		\
     } else {							\
       Cur = YYRHSLOC(Rhs, 0);					\
     }								\
  } while (0)


#include "gnuc_parser_driver.hh"

template<typename T>
static T* mv_p(T* &&p)
{
	T *_p = nullptr;
	std::swap(p, _p);
	return _p;
}

#define MV_P(p) mv_p(std::move(p))

template <typename T>
static void empty(T* &value, pp_tokens_range &loc)
{
	value = nullptr;
	loc.begin = loc.end;
}

static void empty(pp_tokens_range &loc)
{
	loc.begin = loc.end;
}

}

%parse-param {suse::cp::yy::gnuc_parser_driver &pd}


%union {
  suse::cp::ast::pp_token_index token_index;
  suse::cp::ast::assign_op assign_op;
  suse::cp::ast::binary_op binary_op;
  suse::cp::ast::unary_op_pre unary_op_pre;
  suse::cp::types::struct_or_union_kind struct_or_union_kind;

  suse::cp::ast::expr *expr;
  suse::cp::ast::offset_member_designator *offset_member_designator;
  suse::cp::ast::string_literal *string_literal;
  suse::cp::ast::expr_list *expr_list;
  suse::cp::ast::attribute *attribute;
  suse::cp::ast::attribute_list *attribute_list;
  suse::cp::ast::attribute_specifier *attribute_specifier;
  suse::cp::ast::attribute_specifier_list *attribute_specifier_list;
  suse::cp::ast::pointer *pointer;
  suse::cp::ast::direct_abstract_declarator *direct_abstract_declarator;
  suse::cp::ast::abstract_declarator *abstract_declarator;
  suse::cp::ast::type_name *type_name;
  suse::cp::ast::direct_declarator *direct_declarator;
  suse::cp::ast::declarator *declarator;
  suse::cp::ast::storage_class_specifier *storage_class_specifier;
  suse::cp::ast::type_qualifier *type_qualifier;
  suse::cp::ast::type_qualifier_list *type_qualifier_list;
  suse::cp::ast::type_specifier *type_specifier;
  suse::cp::ast::struct_declaration *struct_declaration;
  suse::cp::ast::struct_declarator *struct_declarator;
  suse::cp::ast::struct_declarator_list *struct_declarator_list;
  suse::cp::ast::unnamed_struct_or_union *unnamed_struct_or_union;
  suse::cp::ast::struct_declaration_list *struct_declaration_list;
  suse::cp::ast::enumerator *enumerator;
  suse::cp::ast::enumerator_list *enumerator_list;
  suse::cp::ast::function_specifier *function_specifier;
  suse::cp::ast::specifier_qualifier_list *specifier_qualifier_list;
  suse::cp::ast::declaration_specifiers *declaration_specifiers;
  suse::cp::ast::initializer *initializer;
  suse::cp::ast::initializer_list *initializer_list;
  suse::cp::ast::designator *designator;
  suse::cp::ast::designator_list *designator_list;
  suse::cp::ast::designation *designation;
  suse::cp::ast::asm_label *asm_label;
  suse::cp::ast::init_declarator *init_declarator;
  suse::cp::ast::init_declarator_list *init_declarator_list;
  suse::cp::ast::declaration *declaration;
  suse::cp::ast::parameter_declaration *parameter_declaration;
  suse::cp::ast::parameter_declaration_list *parameter_declaration_list;
  suse::cp::ast::identifier_list *identifier_list;
  suse::cp::ast::declaration_list *declaration_list;
  suse::cp::ast::stmt *stmt;
  suse::cp::ast::stmt_compound *stmt_compound;
  suse::cp::ast::local_label_declaration *local_label_declaration;
  suse::cp::ast::local_label_declaration_list *local_label_declaration_list;
  suse::cp::ast::block_item *block_item;
  suse::cp::ast::block_item_list *block_item_list;
  suse::cp::ast::asm_directive *asm_directive;
  suse::cp::ast::asm_qualifier_list *asm_qualifier_list;
  suse::cp::ast::asm_operand_name *asm_operand_name;
  suse::cp::ast::asm_operand *asm_operand;
  suse::cp::ast::asm_operand_list *asm_operand_list;
  suse::cp::ast::asm_clobber_list *asm_clobber_list;
  suse::cp::ast::asm_jump_to_label_list *asm_jump_to_label_list;
  suse::cp::ast::function_definition *function_definition;
  suse::cp::ast::external_declaration *external_declaration;
  suse::cp::ast::translation_unit *translation_unit;
}

%destructor {} <token_index>
%destructor {} <assign_op>
%destructor {} <binary_op>
%destructor {} <unary_op_pre>
%destructor {} <struct_or_union_kind>
%destructor { delete $$; } <*>

/* keyword tokens */
%token TOK_KW_ASM

%token TOK_KW_TYPEDEF
%token TOK_KW_EXTERN
%token TOK_KW_STATIC
%token TOK_KW_AUTO
%token TOK_KW_REGISTER

%token TOK_KW_VOID
%token TOK_KW_CHAR
%token TOK_KW_SHORT
%token TOK_KW_INT
%token TOK_KW_INT128
%token TOK_KW_LONG
%token TOK_KW_FLOAT
%token TOK_KW_DOUBLE
%token TOK_KW_SIGNED
%token TOK_KW_UNSIGNED
%token TOK_KW_BOOL
%token TOK_KW_COMPLEX

%token TOK_KW_STRUCT
%token TOK_KW_UNION
%token TOK_KW_ENUM

%token TOK_KW_CONST
%token TOK_KW_RESTRICT
%token TOK_KW_VOLATILE

%token TOK_KW_INLINE

%token TOK_KW_TYPEOF

%token TOK_KW_SIZEOF
%token TOK_KW_ALIGNOF

%token TOK_KW_BUILTIN_OFFSETOF
%token TOK_KW_BUILTIN_TYPES_COMPATIBLE_P
%token TOK_KW_BUILTIN_VA_ARG


%token TOK_KW_ATTRIBUTE

%token TOK_KW_CASE
%token TOK_KW_DEFAULT

%token TOK_KW_LABEL

%token TOK_KW_IF
%token TOK_KW_ELSE
%token TOK_KW_SWITCH
%token TOK_KW_WHILE
%token TOK_KW_DO
%token TOK_KW_FOR
%token TOK_KW_GOTO
%token TOK_KW_CONTINUE
%token TOK_KW_BREAK
%token TOK_KW_RETURN

/* Preprocessor tokens. */
%token <token_index> TOK_STRING_LITERAL
%token <token_index> TOK_IDENTIFIER
%token <token_index> TOK_CONSTANT

/* Artificial token for an indentifier resolved to a typedef name. */
%token <token_index> TOK_TYPEDEF_IDENTIFIER

/* Punctuators which aren't necessarily operators. */
%token TOK_LPAREN
%token TOK_RPAREN
%token TOK_LBRACE
%token TOK_RBRACE
%token TOK_LBRACKET
%token TOK_RBRACKET

%token TOK_TRIPLE_DOT

%token TOK_DOT

%token TOK_COLON
%token TOK_SEMICOLON
%token TOK_COMMA

%token TOK_ASTERISK

%token TOK_EQ


/* Punctuators which are always operators. */
%token TOK_OP_ASSIGN_MUL
%token TOK_OP_ASSIGN_DIV
%token TOK_OP_ASSIGN_MOD
%token TOK_OP_ASSIGN_ADD
%token TOK_OP_ASSIGN_SUB
%token TOK_OP_ASSIGN_LSHIFT
%token TOK_OP_ASSIGN_RSHIFT
%token TOK_OP_ASSIGN_BIN_AND
%token TOK_OP_ASSIGN_BIN_XOR
%token TOK_OP_ASSIGN_BIN_OR

%token TOK_OP_LOGICAL_OR
%token TOK_OP_DOUBLE_AMPERSAND


%token TOK_OP_QUESTION_MARK

%token TOK_OP_DEREF_MEMBER

%token TOK_OP_INC
%token TOK_OP_DEC

%token TOK_OP_BIN_OR
%token TOK_OP_BIN_XOR

%token TOK_OP_REL_LT
%token TOK_OP_REL_GT
%token TOK_OP_REL_LEQ
%token TOK_OP_REL_GEQ
%token TOK_OP_EQ
%token TOK_OP_NEQ
%token TOK_OP_PLUS
%token TOK_OP_MINUS
%token TOK_OP_AMPERSAND
%token TOK_OP_DIV
%token TOK_OP_MODULO
%token TOK_OP_BIN_NEG
%token TOK_OP_LOGICAL_NOT

%token TOK_OP_LSHIFT
%token TOK_OP_RSHIFT


/* Define precedence in order to solve the dangling else problem */
%nonassoc TOK_KW_IF
%nonassoc TOK_KW_ELSE

/*
 * Define precedence for resolving a shift/reduce conflict where an
 * attribute follows the closing brace of a struct, union or enum
 * declaration: this clashes with the declaration_specifiers
 * production also allowing for attributes.
 */
%nonassoc TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
%nonassoc TOK_KW_ATTRIBUTE

%type <translation_unit>	translation_unit
%type <external_declaration>	external_declaration
%type <function_definition>	function_definition
%type <function_definition>	function_definition_ext
%type <stmt_compound>	function_definition_body
%type <declaration>	declaration
%type <declaration>	declaration_ext
%type <declaration_specifiers>	declaration_specifiers_no_ts_opt
%type <declaration_specifiers>	declaration_specifiers_no_ts
%type <declaration_specifiers>	declaration_specifiers_w_non_att_no_ts
%type <declaration_specifiers>	declaration_specifiers_ts_no_tdid
%type <declaration_specifiers>	declaration_specifiers_ts
%type <storage_class_specifier>	storage_class_specifier
%type <type_specifier>	type_specifier_no_tdid
%type <type_qualifier>	type_qualifier
%type <function_specifier>	function_specifier
%type <attribute_specifier_list>	attribute_specifier_list_opt
%type <attribute_specifier_list>	attribute_specifier_list
%type <attribute_specifier>	attribute_specifier
%type <attribute_list>	attribute_list
%type <attribute>	attribute_opt
%type <attribute>	attribute
%type <token_index>		attribute_name
%type <expr_list>	attribute_parameters
%type <expr_list>	expression_list_opt
%type <expr_list>	expression_list
%type <type_specifier>	struct_or_union_specifier
%type <struct_or_union_kind>	struct_or_union
%type <token_index>	id_or_tdid
%type <struct_declaration_list>	struct_declaration_list_opt
%type <struct_declaration_list>	struct_declaration_list
%type <struct_declaration>	struct_declaration
%type <specifier_qualifier_list>	specifier_qualifier_list_no_ts_opt
%type <specifier_qualifier_list>	specifier_qualifier_list_no_ts
%type <specifier_qualifier_list>	specifier_qualifier_list_ts_no_tdid
%type <specifier_qualifier_list>	specifier_qualifier_list_ts
%type <type_qualifier_list>	type_qualifier_list_opt
%type <type_qualifier_list>	type_qualifier_list
%type <struct_declarator_list>	struct_declarator_list_no_tdid
%type <struct_declarator>	struct_declarator_no_tdid
%type <struct_declarator_list>	struct_declarator_list_tdid
%type <struct_declarator>	struct_declarator_tdid
%type <unnamed_struct_or_union>	unnamed_struct_or_union
%type <type_specifier>	enum_specifier
%type <enumerator_list>	enumerator_list
%type <enumerator>	enumerator
%type <type_specifier>	typedef_name_no_tdid
%type <type_name>	type_name
%type <abstract_declarator>	abstract_declarator_opt
%type <abstract_declarator>	abstract_declarator
%type <abstract_declarator>	abstract_declarator_ptr
%type <abstract_declarator>	abstract_declarator_non_ptr
%type <pointer>	pointer_opt
%type <pointer>	pointer
%type <direct_abstract_declarator>	direct_abstract_declarator_opt
%type <direct_abstract_declarator>	direct_abstract_declarator
%type <parameter_declaration_list>	parameter_type_list_opt
%type <parameter_declaration_list>	parameter_type_list
%type <parameter_declaration_list>	parameter_list
%type <parameter_declaration>	parameter_declaration_first
%type <parameter_declaration>	parameter_declaration
%type <declarator>	param_declarator_tdid
%type <declarator>	param_declarator_tdid_in_parens
%type <direct_declarator>	direct_param_declarator_tdid
%type <direct_declarator>	direct_param_declarator_tdid_in_parens_tdid
%type <direct_declarator>	direct_param_declarator_tdid_in_parens_no_tdid
%type <declarator>	declarator_no_tdid_opt
%type <declarator>	declarator_no_tdid
%type <direct_declarator>	direct_declarator_no_tdid
%type <declarator>	declarator_tdid_opt
%type <declarator>	declarator_tdid
%type <direct_declarator>	direct_declarator_tdid
%type <identifier_list>	identifier_list_opt
%type <identifier_list>	identifier_list
%type <declaration_list>	declaration_list_opt
%type <declaration_list>	declaration_list
%type <declaration>	declaration_not_starting_with_att
%type <declaration_specifiers>	declaration_specifiers_not_starting_with_att_no_ts_opt
%type <declaration_specifiers>	declaration_specifiers_not_starting_with_att_no_ts
%type <declaration_specifiers>	declaration_specifiers_not_starting_with_att_ts_no_tdid
%type <declaration_specifiers>	declaration_specifiers_not_starting_with_att_ts
%type <init_declarator_list>	init_declarator_list_no_tdid_opt
%type <init_declarator_list>	init_declarator_list_no_tdid
%type <init_declarator>	init_declarator_no_tdid
%type <init_declarator_list>	init_declarator_list_tdid_opt
%type <init_declarator_list>	init_declarator_list_tdid
%type <init_declarator>	init_declarator_tdid
%type <asm_label>	asm_label
%type <initializer>	initializer
%type <initializer_list>	initializer_list
%type <designation>	designation_opt
%type <designation>	designation
%type <designator_list>	designator_list
%type <designator>	designator
%type <stmt>	statement
%type <stmt>	labeled_statement
%type <stmt_compound>	compound_statement
%type <local_label_declaration_list>	local_label_declaration_list_opt
%type <local_label_declaration_list>	local_label_declaration_list
%type <local_label_declaration>	local_label_declaration
%type <block_item_list>	block_item_list_opt
%type <block_item_list>	block_item_list
%type <block_item>	block_item
%type <stmt>	expression_statement
%type <stmt>	selection_statement
%type <stmt>	iteration_statement
%type <stmt>	jump_statement
%type <stmt>	asm_statement
%type <asm_directive>	asm_directive
%type <asm_qualifier_list>	asm_qualifier_list_opt
%type <asm_qualifier_list>	asm_qualifier_list
%type <asm_operand_list>	asm_operand_list_opt
%type <asm_operand_list>	asm_operand_list
%type <asm_operand>	asm_operand
%type <asm_operand_name>	asm_operand_name_opt
%type <asm_operand_name>	asm_operand_name
%type <asm_clobber_list>	asm_clobber_list_opt
%type <asm_clobber_list>	asm_clobber_list
%type <asm_jump_to_label_list>	asm_jump_to_label_list_opt
%type <asm_jump_to_label_list>	asm_jump_to_label_list
%type <expr>	constant_expression
%type <expr>	expression_opt
%type <expr>	expression
%type <expr>	assignment_expression_opt
%type <expr>	assignment_expression
%type <assign_op>	assignment_operator
%type <expr>	conditional_expression
%type <expr>	logical_or_expression
%type <expr>	logical_and_expression
%type <expr>	inclusive_or_expression
%type <expr>	exclusive_or_expression
%type <expr>	and_expression
%type <expr>	equality_expression
%type <binary_op>	equality_operator
%type <expr>	relational_expression
%type <binary_op>	relational_operator
%type <expr>	shift_expression
%type <binary_op>	shift_operator
%type <expr>	additive_expression
%type <expr>	multiplicative_expression
%type <expr>	cast_expression
%type <expr>	unary_expression
%type <unary_op_pre>	unary_operator
%type <offset_member_designator>	offset_member_designator
%type <expr>	postfix_expression
%type <expr>	statement_expression
%type <expr>	primary_expression
%type <string_literal>	string_literal

%%

start:
	/* empty */
	| translation_unit
	  { pd._result = MV_P($1); }
;

translation_unit:
	external_declaration
	  { $$ = new translation_unit(std::move($1)); }
	| translation_unit external_declaration
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| translation_unit TOK_SEMICOLON
	  { $$ = MV_P($1);}
;

external_declaration:
	function_definition_ext
	  { $$ = new external_declaration_func(std::move($1)); }
	| declaration_ext
	  { $$ = new external_declaration_decl(std::move($1)); }
	| asm_directive
	  { $$ = new external_declaration_asm(std::move($1)); }
;

function_definition_ext:
	function_definition
	  { $$ = MV_P($1); }
	| declarator_no_tdid
	  attribute_specifier_list_opt { pd.handle_decl_id($1->get_id_tok()); }
	  declaration_list_opt function_definition_body
	  {
	    declaration_specifiers *ds =
		new declaration_specifiers(pp_tokens_range(@1.begin, @1.begin));
	    try {
	      $$ = new function_definition(@$, std::move(ds), std::move($1),
					   std::move($2), std::move($4),
					   std::move($5));
	    } catch (...) {
	      delete ds;
	      throw;
	    }
	  }
;

function_definition:
	declaration_specifiers_no_ts declarator_no_tdid
	attribute_specifier_list_opt { pd.handle_decl_id($2->get_id_tok()); }
	declaration_list_opt function_definition_body
	  {
	    $$ = new function_definition(@$, std::move($1), std::move($2),
					 std::move($3), std::move($5),
					 std::move($6));
	  }
	| declaration_specifiers_ts declarator_tdid
	  attribute_specifier_list_opt { pd.handle_decl_id($2->get_id_tok()); }
	  declaration_list_opt function_definition_body
	  {
	    $$ = new function_definition(@$, std::move($1), std::move($2),
					 std::move($3), std::move($5),
					 std::move($6));
	  }
;

function_definition_body:
	TOK_LBRACE {
		     pd.clear_in_typedef();
		     pd.restore_stashed_td_scope();
		   }
	local_label_declaration_list_opt block_item_list_opt TOK_RBRACE
	  {
	    pd.leave_td_scope();
	    $$ = new stmt_compound(@$, std::move($3), std::move($4));
	  }
;

declaration_ext:
	init_declarator_list_no_tdid TOK_SEMICOLON
	  {
	    declaration_specifiers *ds =
		new declaration_specifiers(pp_tokens_range(@1.begin, @1.begin));
	    try {
	      $$ = new declaration(@$, std::move(ds), std::move($1));
	    } catch (...) {
	      delete ds;
	      throw;
	    }
	  }
	| declaration
	  {
	    $$ = MV_P($1);
	  }
;

declaration:
	declaration_specifiers_no_ts init_declarator_list_no_tdid_opt TOK_SEMICOLON
	  {
	    pd.clear_in_typedef();
	    $$ = new declaration(@$, std::move($1), std::move($2));
	  }
	| declaration_specifiers_ts init_declarator_list_tdid_opt TOK_SEMICOLON
	  {
	    pd.clear_in_typedef();
	    $$ = new declaration(@$, std::move($1), std::move($2));
	  }
;

declaration_specifiers_no_ts_opt:
	/* empty */
	  { empty($$, @$); }
	| declaration_specifiers_no_ts
	  { $$ = MV_P($1); }
;

declaration_specifiers_no_ts:
	declaration_specifiers_w_non_att_no_ts
	  { $$ = MV_P($1); }
	| attribute_specifier_list
	  { $$ = new declaration_specifiers(std::move($1)); }
;

declaration_specifiers_w_non_att_no_ts:
	storage_class_specifier
	  { $$ = new declaration_specifiers(std::move($1)); }
	| attribute_specifier_list storage_class_specifier
	  {
	    $$ = new declaration_specifiers(std::move($1));
	    $$->extend(std::move($2));
	  }
	| declaration_specifiers_w_non_att_no_ts storage_class_specifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| type_qualifier
	  { $$ = new declaration_specifiers(std::move($1)); }
	| attribute_specifier_list type_qualifier
	  {
	    $$ = new declaration_specifiers(std::move($1));
	    $$->extend(std::move($2));
	  }
	| declaration_specifiers_w_non_att_no_ts type_qualifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| function_specifier
	  { $$ = new declaration_specifiers(std::move($1)); }
	| attribute_specifier_list function_specifier
	  {
	    $$ = new declaration_specifiers(std::move($1));
	    $$->extend(std::move($2));
	  }
	| declaration_specifiers_w_non_att_no_ts function_specifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_w_non_att_no_ts attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

declaration_specifiers_ts_no_tdid:
	declaration_specifiers_no_ts_opt type_specifier_no_tdid
	  {
	    if (!$1) {
	      $$ = new declaration_specifiers(std::move($2));
	    } else {
	      $$ = MV_P($1);
	      $$->extend(std::move($2));
	    }
	  }
	| declaration_specifiers_ts_no_tdid type_specifier_no_tdid
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_ts_no_tdid storage_class_specifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_ts_no_tdid type_qualifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_ts_no_tdid function_specifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_ts_no_tdid attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

declaration_specifiers_ts:
	declaration_specifiers_ts_no_tdid
	  { $$ = MV_P($1); }
	| declaration_specifiers_no_ts TOK_TYPEDEF_IDENTIFIER declaration_specifiers_no_ts_opt
	  {
	    type_specifier_tdid *ts_tdid = new type_specifier_tdid($2);
	    $$ = MV_P($1);
	    $$->extend(std::move(ts_tdid));
	    $$->extend(std::move($3));
	  }
	| TOK_TYPEDEF_IDENTIFIER declaration_specifiers_no_ts_opt
	  {
	    type_specifier_tdid *ts_tdid = new type_specifier_tdid($1);
	    try {
	      $$ = new declaration_specifiers(std::move(ts_tdid));
	    } catch (...) {
	      delete ts_tdid;
	    }
	    $$->extend(std::move($2));
	  }
;

storage_class_specifier:
	TOK_KW_TYPEDEF
	  {
	    pd.set_in_typedef();
	    $$ = new storage_class_specifier(@$, storage_class::sc_typedef);
	  }
	| TOK_KW_EXTERN
	  { $$ = new storage_class_specifier(@$, storage_class::sc_extern); }
	| TOK_KW_STATIC
	  { $$ = new storage_class_specifier(@$, storage_class::sc_static); }
	| TOK_KW_AUTO
	  { $$ = new storage_class_specifier(@$, storage_class::sc_auto); }
	| TOK_KW_REGISTER
	  { $$ = new storage_class_specifier(@$, storage_class::sc_register); }
;

type_specifier_no_tdid:
	TOK_KW_VOID
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_void); }
	| TOK_KW_CHAR
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_char); }
	| TOK_KW_SHORT
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_short); }
	| TOK_KW_INT
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_int); }
	| TOK_KW_INT128
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_int128); }
	| TOK_KW_LONG
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_long); }
	| TOK_KW_FLOAT
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_float); }
	| TOK_KW_DOUBLE
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_double); }
	| TOK_KW_SIGNED
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_signed); }
	| TOK_KW_UNSIGNED
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_unsigned); }
	| TOK_KW_BOOL
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_bool); }
	| TOK_KW_COMPLEX
	  { $$ = new type_specifier_pod(@$, pod_spec::ps_complex); }
	| struct_or_union_specifier
	  { $$ =MV_P($1); }
	| enum_specifier
	  { $$ =MV_P($1); }
	| typedef_name_no_tdid
	  { $$ =MV_P($1); }
;

type_qualifier:
	TOK_KW_CONST
	  { $$ = new type_qualifier(@$, types::qualifier::q_const); }
	| TOK_KW_RESTRICT
	  { $$ = new type_qualifier(@$, types::qualifier::q_restrict); }
	| TOK_KW_VOLATILE
	  { $$ = new type_qualifier(@$, types::qualifier::q_volatile); }
;

function_specifier:
	TOK_KW_INLINE
	  { $$ = new function_specifier(@1.begin); }
;

attribute_specifier_list_opt:
	/* empty */ %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { empty($$, @$); };
	| attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = MV_P($1); }
;

attribute_specifier_list:
	attribute_specifier
	  { $$ = new attribute_specifier_list(std::move($1)); }
	| attribute_specifier_list attribute_specifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

attribute_specifier:
	TOK_KW_ATTRIBUTE TOK_LPAREN TOK_LPAREN attribute_list TOK_RPAREN TOK_RPAREN
	  { $$ = new attribute_specifier(@$, std::move($4)); }
;

attribute_list:
	attribute_opt
	  { $$ = new attribute_list(@$, std::move($1)); }
	| attribute_list TOK_COMMA attribute_opt
	  { $$ = MV_P($1); $$->extend(@3, std::move($3)); }
;

attribute_opt:
	/* empty */
	  { empty($$, @$); }
	| attribute
	  { $$ = MV_P($1); }
;

attribute:
	attribute_name
	  { $$ = new attribute($1); }
	| attribute_name TOK_LPAREN attribute_parameters TOK_RPAREN
	  { $$ = new attribute(@$, $1, std::move($3)); }
;

attribute_name:
	id_or_tdid
	  { $$ = $1; }
	| TOK_KW_CONST
	  { $$ = @$.begin; }
;

attribute_parameters:
	expression_list_opt
	  { $$ = MV_P($1); }
;

expression_list_opt:
	/* empty */
	  { empty($$, @$); }
	| expression_list
	  { $$ = MV_P($1); }
;

expression_list:
	assignment_expression
	  { $$ = new expr_list(std::move($1)); }
	| expression_list TOK_COMMA assignment_expression
	  { $$ = MV_P($1); $$->extend(std::move($3)); }
;

struct_or_union_specifier:
	struct_or_union attribute_specifier_list_opt id_or_tdid TOK_LBRACE struct_declaration_list_opt TOK_RBRACE attribute_specifier_list_opt
	  {
	    $$ = new struct_or_union_def(@$, $1, $3, std::move($5),
					 std::move($2), std::move($7));
	  }
	| struct_or_union attribute_specifier_list_opt TOK_LBRACE struct_declaration_list_opt TOK_RBRACE attribute_specifier_list_opt
	  {
	    $$ = new struct_or_union_def(@$, $1, std::move($4),
					 std::move($2), std::move($6));
	  }
	| struct_or_union attribute_specifier_list_opt id_or_tdid
	  { $$ = new struct_or_union_ref(@$, $1, $3, std::move($2)); }
;

struct_or_union:
	TOK_KW_STRUCT
	  { $$ = types::struct_or_union_kind::souk_struct; }
	| TOK_KW_UNION
	  { $$ = types::struct_or_union_kind::souk_union; }
;

id_or_tdid:
	TOK_IDENTIFIER
	  { $$ = $1; }
	| TOK_TYPEDEF_IDENTIFIER
	  { $$ = $1; }
;

struct_declaration_list_opt:
	/* empty */
	  { empty($$, @$); }
	| semicolons
	  { $$ = nullptr; }
	| struct_declaration_list
	  { $$ = MV_P($1); }
;

struct_declaration_list:
	semicolons_opt struct_declaration semicolons_opt
	  { $$ = new struct_declaration_list(std::move($2)); }
	| struct_declaration_list struct_declaration semicolons_opt
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

semicolons_opt:
	/* empty */
	  { empty(@$); }
	| semicolons
;

semicolons:
	TOK_SEMICOLON
	| semicolons TOK_SEMICOLON
;

/*
 * The specifier_qualifier_list in front of the unnamed_struct_or_union
 * below should really be a type_qualifier_list. However, this would result
 * in a reduce/reduce conflict. Hence, allow for the superset when parsing.
 */
struct_declaration:
	specifier_qualifier_list_no_ts struct_declarator_list_no_tdid TOK_SEMICOLON
	  { $$ = new struct_declaration_c99(@$, std::move($1), std::move($2)); }
	| specifier_qualifier_list_ts struct_declarator_list_tdid TOK_SEMICOLON
	  { $$ = new struct_declaration_c99(@$, std::move($1), std::move($2)); }
	|  specifier_qualifier_list_no_ts unnamed_struct_or_union TOK_SEMICOLON
	  {
	    $$ = new struct_declaration_unnamed_sou(@$, std::move($1),
						    std::move($2));
	  }
	|  unnamed_struct_or_union TOK_SEMICOLON
	  {
	    $$ = new struct_declaration_unnamed_sou(@$, nullptr,
						    std::move($1));
	  }
;

specifier_qualifier_list_no_ts_opt:
	/* empty */
	  { empty($$, @$); }
	| specifier_qualifier_list_no_ts
	  { $$ = MV_P($1); }
;

specifier_qualifier_list_no_ts:
	type_qualifier
	  { $$ = new specifier_qualifier_list(std::move($1)); }
	| specifier_qualifier_list_no_ts type_qualifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = new specifier_qualifier_list(std::move($1)); }
	| specifier_qualifier_list_no_ts attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = MV_P($1); $$->extend(std::move($2)); }

;

specifier_qualifier_list_ts_no_tdid:
	type_specifier_no_tdid
	  { $$ = new specifier_qualifier_list(std::move($1)); }
	| specifier_qualifier_list_no_ts type_specifier_no_tdid
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| specifier_qualifier_list_ts_no_tdid type_specifier_no_tdid
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| specifier_qualifier_list_ts_no_tdid type_qualifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| specifier_qualifier_list_ts_no_tdid attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

specifier_qualifier_list_ts:
	specifier_qualifier_list_ts_no_tdid
	 { $$ = MV_P($1); }
	| specifier_qualifier_list_no_ts_opt TOK_TYPEDEF_IDENTIFIER specifier_qualifier_list_no_ts_opt
	  {
	    type_specifier_tdid *ts_tdid = new type_specifier_tdid($2);
	    if (!$1) {
	      try {
		$$ = new specifier_qualifier_list(std::move(ts_tdid));
	      } catch (...) {
		delete ts_tdid;
	      }
	    } else {
	      $$ = MV_P($1);
	      $$->extend(std::move(ts_tdid));
	    }
	    $$->extend(std::move($3));
	  }
;

type_qualifier_list_opt:
	/* empty */
	  { empty($$, @$); }
	| type_qualifier_list
	  { $$ = MV_P(std::move($1)); }
;

type_qualifier_list:
	type_qualifier
	  { $$ = new type_qualifier_list(std::move($1)); }
	| type_qualifier_list type_qualifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = new type_qualifier_list(std::move($1)); }
	| type_qualifier_list attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

struct_declarator_list_no_tdid:
	struct_declarator_no_tdid
	  { $$ = new struct_declarator_list(std::move($1)); }
	| struct_declarator_list_no_tdid TOK_COMMA attribute_specifier_list_opt struct_declarator_no_tdid
	  {
	    $$ = MV_P($1);
	    $4->set_asl_before(std::move($3));
	    $$->extend(std::move($4));
	  }
;

struct_declarator_no_tdid:
	declarator_no_tdid attribute_specifier_list_opt
	  {
	    $$ = new struct_declarator(@$, std::move($1), nullptr,
				       std::move($2));
	  }
	| declarator_no_tdid_opt TOK_COLON constant_expression attribute_specifier_list_opt
	  {
	    $$ = new struct_declarator(@$, std::move($1), std::move($3),
				       std::move($4));
	  }
;

struct_declarator_list_tdid:
	struct_declarator_tdid
	  { $$ = new struct_declarator_list(std::move($1)); }
	| struct_declarator_list_tdid TOK_COMMA attribute_specifier_list_opt struct_declarator_tdid
	  {
	    $$ = MV_P($1);
	    $4->set_asl_before(std::move($3));
	    $$->extend(std::move($4));
	  }
;

struct_declarator_tdid:
	declarator_tdid attribute_specifier_list_opt
	  {
	    $$ = new struct_declarator(@$, std::move($1), nullptr,
				       std::move($2));
	  }
	| declarator_tdid_opt TOK_COLON constant_expression attribute_specifier_list_opt
	  {
	    $$ = new struct_declarator(@$, std::move($1), std::move($3),
				       std::move($4));
	  }
;

unnamed_struct_or_union:
	struct_or_union attribute_specifier_list_opt TOK_LBRACE struct_declaration_list_opt TOK_RBRACE attribute_specifier_list_opt
	  {
	    $$ = new unnamed_struct_or_union(@$, $1, std::move($4),
					     std::move($2), std::move($6));
	  }
;


enum_specifier:
	TOK_KW_ENUM attribute_specifier_list_opt id_or_tdid TOK_LBRACE enumerator_list TOK_RBRACE attribute_specifier_list_opt
	  {
	    $$ = new enum_def(@$, $3, std::move($5), std::move($2),
			      std::move($7));
	  }
	| TOK_KW_ENUM attribute_specifier_list_opt id_or_tdid TOK_LBRACE enumerator_list TOK_COMMA TOK_RBRACE attribute_specifier_list_opt
	  {
	    $$ = new enum_def(@$, $3, std::move($5), std::move($2),
			      std::move($8));
	  }
	| TOK_KW_ENUM attribute_specifier_list_opt TOK_LBRACE enumerator_list TOK_RBRACE attribute_specifier_list_opt
	  {
	    $$ = new enum_def(@$, std::move($4), std::move($2),
			      std::move($6));
	  }
	| TOK_KW_ENUM attribute_specifier_list_opt TOK_LBRACE enumerator_list TOK_COMMA TOK_RBRACE attribute_specifier_list_opt
	  {
	    $$ = new enum_def(@$, std::move($4), std::move($2),
			      std::move($7));
	  }
	| TOK_KW_ENUM attribute_specifier_list_opt id_or_tdid
	  { $$ = new enum_ref(@$, $3, std::move($2)); }
;

enumerator_list:
	enumerator
	  { $$ = new enumerator_list(std::move($1)); }
	| enumerator_list TOK_COMMA enumerator
	  { $$ = MV_P($1), $$->extend(std::move($3)); }
;

enumerator:
	id_or_tdid
	  {
	    pd.handle_enumerator_id($1);
	    $$ = new enumerator(@$, $1, nullptr);
	  }
	| id_or_tdid TOK_EQ constant_expression
	  {
	    pd.handle_enumerator_id($1);
	    $$ = new enumerator(@$, $1, std::move($3));
	  }
;

typedef_name_no_tdid:
	TOK_KW_TYPEOF TOK_LPAREN expression TOK_RPAREN
	  { $$ = new typeof_expr(@$, std::move($3)); }
	| TOK_KW_TYPEOF TOK_LPAREN type_name TOK_RPAREN
	  { $$ = new typeof_type_name(@$, std::move($3)); }
;

type_name:
	specifier_qualifier_list_no_ts abstract_declarator_opt
	  { $$ = new type_name(@$, std::move($1), std::move($2)); }
	| specifier_qualifier_list_ts abstract_declarator_opt
	  { $$ = new type_name(@$, std::move($1), std::move($2)); }
;

abstract_declarator_opt:
	/* empty */
	  { empty($$, @$); }
	| abstract_declarator
	  { $$ = MV_P($1); }
;

abstract_declarator:
	pointer
	  { $$ = new abstract_declarator(@$, std::move($1), nullptr); }
	| pointer direct_abstract_declarator
	  { $$ = new abstract_declarator(@$, std::move($1), std::move($2)); }
	| direct_abstract_declarator
	  { $$ = new abstract_declarator(@$, nullptr, std::move($1)); }
;

abstract_declarator_ptr:
	pointer
	  { $$ = new abstract_declarator(@$, std::move($1), nullptr); }
;

abstract_declarator_non_ptr:
	pointer direct_abstract_declarator
	  { $$ = new abstract_declarator(@$, std::move($1), std::move($2)); }
	| direct_abstract_declarator
	  { $$ = new abstract_declarator(@$, nullptr, std::move($1)); }
;

pointer_opt:
	/* empty */
	  { empty($$, @$); }
	| pointer
	  { $$ = MV_P($1); }
;

pointer:
	TOK_ASTERISK type_qualifier_list_opt
	  { $$ = new pointer(@$, std::move($2)); }
	| pointer TOK_ASTERISK type_qualifier_list_opt
	  { $$ = MV_P($1); $$->extend(@3, std::move($3)); }
;

direct_abstract_declarator_opt:
	/* empty */
	  { empty($$, @$); }
	| direct_abstract_declarator
	  { $$ = MV_P($1); }
;

lparen_ign_td_spec:
	TOK_LPAREN
	  { pd.begin_ignore_td_spec(); }
;

direct_abstract_declarator:
	lparen_ign_td_spec attribute_specifier_list_opt abstract_declarator TOK_RPAREN
	  {
	    pd.end_ignore_td_spec();
	    $$ = new direct_abstract_declarator_parenthesized(@$, std::move($2),
							      std::move($3));
	  }
	| direct_abstract_declarator_opt TOK_LBRACKET type_qualifier_list assignment_expression_opt TOK_RBRACKET
	  {
	    $$ = new direct_abstract_declarator_array(@$, std::move($1),
						      std::move($3),
						      std::move($4), false);
	  }
	| direct_abstract_declarator_opt TOK_LBRACKET assignment_expression_opt TOK_RBRACKET
	  {
	    $$ = new direct_abstract_declarator_array(@$, std::move($1),
						      nullptr,
						      std::move($3), false);
	  }
	| direct_abstract_declarator_opt TOK_LBRACKET TOK_KW_STATIC type_qualifier_list_opt assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_abstract_declarator_array(@$, std::move($1),
						      std::move($4),
						      std::move($5), true);
	  }
	| direct_abstract_declarator_opt TOK_LBRACKET type_qualifier_list TOK_KW_STATIC assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_abstract_declarator_array(@$, std::move($1),
						      std::move($3),
						      std::move($5), true);
	  }
	| direct_abstract_declarator_opt TOK_LBRACKET TOK_ASTERISK TOK_RBRACKET
	  {
		    $$ = new direct_abstract_declarator_array(@$, std::move($1),
		       direct_abstract_declarator_array::vla_unspec_size_tag{});
	  }
	| direct_abstract_declarator lparen_ign_td_spec parameter_type_list_opt TOK_RPAREN
	  {
	    if ($3)
	      pd.leave_td_scope();
	    pd.end_ignore_td_spec();
	    $$ = new direct_abstract_declarator_func(@$, std::move($1),
						     std::move($3));
	  }
	| lparen_ign_td_spec parameter_type_list TOK_RPAREN
	  {
	    pd.leave_td_scope();
	    pd.end_ignore_td_spec();
	    $$ = new direct_abstract_declarator_func(@$, nullptr,
						     std::move($2));
	  }
;

parameter_type_list_opt:
	/* empty */
	  { empty($$, @$); }
	| parameter_type_list
	  { $$ = MV_P($1); }
;

parameter_type_list:
	parameter_list
	  { $$ = MV_P($1); }
	| parameter_list TOK_COMMA TOK_TRIPLE_DOT
	  { $$ = MV_P($1); $$->set_variadic(@3); }
;

parameter_list:
	parameter_declaration_first
	  { $$ = new parameter_declaration_list(std::move($1)); }
	| parameter_list TOK_COMMA parameter_declaration
	  { $$ = MV_P($1); $$->extend(std::move($3)); }
;

parameter_declaration_first:
	declaration_specifiers_w_non_att_no_ts declarator_no_tdid attribute_specifier_list_opt
	  {
	    pd.enter_td_scope();
	    pd.handle_param_id($2->get_id_tok());
	    $$ = new parameter_declaration_declarator(@$, std::move($1),
						      std::move($2),
						      std::move($3));
	  }
	| declaration_specifiers_ts param_declarator_tdid attribute_specifier_list_opt
	  {
	    pd.enter_td_scope();
	    pd.handle_param_id($2->get_id_tok());
	    $$ = new parameter_declaration_declarator(@$, std::move($1),
						      std::move($2),
						      std::move($3));
	  }
	| declaration_specifiers_w_non_att_no_ts abstract_declarator_ptr
	  {
	    pd.enter_td_scope();
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    std::move($2), nullptr);
	  }
	| declaration_specifiers_w_non_att_no_ts abstract_declarator_non_ptr attribute_specifier_list_opt
	  {
	    pd.enter_td_scope();
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    std::move($2),
						    std::move($3));
	  }
	| declaration_specifiers_w_non_att_no_ts
	  {
	    pd.enter_td_scope();
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    nullptr, nullptr);
	  }
	| declaration_specifiers_ts abstract_declarator_ptr
	  {
	    pd.enter_td_scope();
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    std::move($2), nullptr);
	  }
	| declaration_specifiers_ts abstract_declarator_non_ptr attribute_specifier_list_opt
	  {
	    pd.enter_td_scope();
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    std::move($2),
						    std::move($3));
	  }
	| declaration_specifiers_ts
	  {
	    pd.enter_td_scope();
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    nullptr, nullptr);
	  }
;

parameter_declaration:
	declaration_specifiers_no_ts declarator_no_tdid attribute_specifier_list_opt
	  {
	    pd.handle_param_id($2->get_id_tok());
	    $$ = new parameter_declaration_declarator(@$, std::move($1),
						      std::move($2),
						      std::move($3));
	  }
	| declaration_specifiers_ts param_declarator_tdid attribute_specifier_list_opt
	  {
	    pd.handle_param_id($2->get_id_tok());
	    $$ = new parameter_declaration_declarator(@$, std::move($1),
						      std::move($2),
						      std::move($3));
	  }
	| declaration_specifiers_no_ts abstract_declarator_ptr
	  {
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    std::move($2), nullptr);
	  }
	| declaration_specifiers_no_ts abstract_declarator_non_ptr attribute_specifier_list_opt
	  {
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    std::move($2),
						    std::move($3));
	  }
	| declaration_specifiers_no_ts
	  {
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    nullptr, nullptr);
	  }

	| declaration_specifiers_ts abstract_declarator_ptr
	  {
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    std::move($2), nullptr);
	  }
	| declaration_specifiers_ts abstract_declarator_non_ptr attribute_specifier_list_opt
	  {
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    std::move($2),
						    std::move($3));
	  }
	| declaration_specifiers_ts
	  {
	    $$ = new parameter_declaration_abstract(@$, std::move($1),
						    nullptr, nullptr);
	  }
;

declarator_no_tdid_opt:
	/* empty */
	  { empty($$, @$); };
	| declarator_no_tdid
	  { $$ = MV_P($1); }
;

declarator_no_tdid:
	pointer direct_declarator_no_tdid
	  { $$ = new declarator(@$, std::move($1), std::move($2)); }
	| direct_declarator_no_tdid
	  { $$ = new declarator(@$, nullptr, std::move($1)); }
;

direct_declarator_no_tdid:
	TOK_IDENTIFIER
	  { $$ = new direct_declarator_id($1); }
	| lparen_ign_td_spec attribute_specifier_list_opt declarator_no_tdid TOK_RPAREN
	  {
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_parenthesized(@$, std::move($3),
						     std::move($2));
	  }
	| direct_declarator_no_tdid TOK_LBRACKET type_qualifier_list_opt assignment_expression_opt TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($4), false);
	  }
	| direct_declarator_no_tdid TOK_LBRACKET TOK_KW_STATIC type_qualifier_list_opt assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($4),
					     std::move($5), true);
	  }
	| direct_declarator_no_tdid TOK_LBRACKET type_qualifier_list TOK_KW_STATIC assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($5), true);
	  }
	| direct_declarator_no_tdid TOK_LBRACKET type_qualifier_list_opt TOK_ASTERISK TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
				direct_declarator_array::vla_unspec_size_tag{});
	  }
	| direct_declarator_no_tdid lparen_ign_td_spec parameter_type_list TOK_RPAREN
	  {
	    pd.stash_td_scope();
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_func(@$, std::move($1), std::move($3));
	  }
	| direct_declarator_no_tdid TOK_LPAREN identifier_list_opt TOK_RPAREN
	  {
	    pd.enter_td_scope();
	    pd.stash_td_scope();
	    $$ = new direct_declarator_func(@$, std::move($1), std::move($3));
	  }
;

declarator_tdid_opt:
	/* empty */
	  { empty($$, @$); }
	| declarator_tdid
	  { $$ = MV_P($1); }
;

declarator_tdid:
	pointer_opt direct_declarator_tdid
	  { $$ = new declarator(@$, std::move($1), std::move($2)); }
;

direct_declarator_tdid:
	id_or_tdid
	  { $$ = new direct_declarator_id($1); }
	| TOK_LPAREN attribute_specifier_list_opt declarator_tdid TOK_RPAREN
	  {
	    $$ = new direct_declarator_parenthesized(@$, std::move($3),
						     std::move($2));
	  }
	| direct_declarator_tdid TOK_LBRACKET type_qualifier_list_opt assignment_expression_opt TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($4), false);
	  }
	| direct_declarator_tdid TOK_LBRACKET TOK_KW_STATIC type_qualifier_list_opt assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($4),
					     std::move($5), true);
	  }
	| direct_declarator_tdid TOK_LBRACKET type_qualifier_list TOK_KW_STATIC assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($5), true);
	  }
	| direct_declarator_tdid TOK_LBRACKET type_qualifier_list_opt TOK_ASTERISK TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
				direct_declarator_array::vla_unspec_size_tag{});
	  }
	| direct_declarator_tdid lparen_ign_td_spec parameter_type_list TOK_RPAREN
	  {
	    pd.stash_td_scope();
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_func(@$, std::move($1), std::move($3));
	  }
	| direct_declarator_tdid TOK_LPAREN identifier_list_opt TOK_RPAREN
	  {
	    pd.enter_td_scope();
	    pd.stash_td_scope();
	    $$ = new direct_declarator_func(@$, std::move($1), std::move($3));
	  }
;


param_declarator_tdid:
	pointer direct_param_declarator_tdid
	  { $$ = new declarator(@$, std::move($1), std::move($2)); }
	| direct_param_declarator_tdid
	  { $$ = new declarator(@$, nullptr, std::move($1)); }
;

direct_param_declarator_tdid:
	id_or_tdid
	  { $$ = new direct_declarator_id($1); }
	| lparen_ign_td_spec attribute_specifier_list_opt param_declarator_tdid_in_parens TOK_RPAREN
	  {
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_parenthesized(@$, std::move($3),
						     std::move($2));
	  }
	| direct_param_declarator_tdid TOK_LBRACKET type_qualifier_list_opt assignment_expression_opt TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($4), false);
	  }
	| direct_param_declarator_tdid TOK_LBRACKET TOK_KW_STATIC type_qualifier_list_opt assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($4),
					     std::move($5), true);
	  }
	| direct_param_declarator_tdid TOK_LBRACKET type_qualifier_list TOK_KW_STATIC assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($5), true);
	  }
	| direct_param_declarator_tdid TOK_LBRACKET type_qualifier_list_opt TOK_ASTERISK TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
				direct_declarator_array::vla_unspec_size_tag{});
	  }
	| direct_param_declarator_tdid lparen_ign_td_spec parameter_type_list TOK_RPAREN
	  {
	    pd.leave_td_scope();
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_func(@$, std::move($1), std::move($3));
	  }
	| direct_param_declarator_tdid TOK_LPAREN identifier_list_opt TOK_RPAREN
	  { $$ = new direct_declarator_func(@$, std::move($1), std::move($3)); }
;

param_declarator_tdid_in_parens:
	pointer direct_param_declarator_tdid_in_parens_tdid
	  { $$ = new declarator(@$, std::move($1), std::move($2)); }
	| direct_param_declarator_tdid_in_parens_no_tdid
	  { $$ = new declarator(@$, nullptr, std::move($1)); }
;

direct_param_declarator_tdid_in_parens_tdid:
	id_or_tdid
	  { $$ = new direct_declarator_id($1); }
	| lparen_ign_td_spec attribute_specifier_list_opt param_declarator_tdid_in_parens TOK_RPAREN
	  {
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_parenthesized(@$, std::move($3),
						     std::move($2));
	  }
	| direct_param_declarator_tdid_in_parens_tdid TOK_LBRACKET type_qualifier_list_opt assignment_expression_opt TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($4), false);
	  }
	| direct_param_declarator_tdid_in_parens_tdid TOK_LBRACKET TOK_KW_STATIC type_qualifier_list_opt assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($4),
					     std::move($5), true);
	  }
	| direct_param_declarator_tdid_in_parens_tdid TOK_LBRACKET type_qualifier_list TOK_KW_STATIC assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($5), true);
	  }
	| direct_param_declarator_tdid_in_parens_tdid TOK_LBRACKET type_qualifier_list_opt TOK_ASTERISK TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
				direct_declarator_array::vla_unspec_size_tag{});
	  }
	| direct_param_declarator_tdid_in_parens_tdid lparen_ign_td_spec parameter_type_list TOK_RPAREN
	  {
	    pd.leave_td_scope();
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_func(@$, std::move($1), std::move($3));
	  }
	| direct_param_declarator_tdid_in_parens_tdid TOK_LPAREN identifier_list_opt TOK_RPAREN
	  { $$ = new direct_declarator_func(@$, std::move($1), std::move($3)); }
;


direct_param_declarator_tdid_in_parens_no_tdid:
	TOK_IDENTIFIER
	  { $$ = new direct_declarator_id($1); }
	| lparen_ign_td_spec attribute_specifier_list_opt param_declarator_tdid_in_parens TOK_RPAREN
	  {
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_parenthesized(@$, std::move($3),
						     std::move($2));
	  }
	| direct_param_declarator_tdid_in_parens_no_tdid TOK_LBRACKET type_qualifier_list_opt assignment_expression_opt TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($4), false);
	  }
	| direct_param_declarator_tdid_in_parens_no_tdid TOK_LBRACKET TOK_KW_STATIC type_qualifier_list_opt assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($4),
					     std::move($5), true);
	  }
	| direct_param_declarator_tdid_in_parens_no_tdid TOK_LBRACKET type_qualifier_list TOK_KW_STATIC assignment_expression TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
					     std::move($5), true);
	  }
	| direct_param_declarator_tdid_in_parens_no_tdid TOK_LBRACKET type_qualifier_list_opt TOK_ASTERISK TOK_RBRACKET
	  {
	    $$ = new direct_declarator_array(@$, std::move($1), std::move($3),
				direct_declarator_array::vla_unspec_size_tag{});
	  }
	| direct_param_declarator_tdid_in_parens_no_tdid lparen_ign_td_spec parameter_type_list TOK_RPAREN
	  {
	    pd.leave_td_scope();
	    pd.end_ignore_td_spec();
	    $$ = new direct_declarator_func(@$, std::move($1), std::move($3));
	  }
	| direct_param_declarator_tdid_in_parens_no_tdid TOK_LPAREN identifier_list_opt TOK_RPAREN
	  { $$ = new direct_declarator_func(@$, std::move($1), std::move($3)); }
;


identifier_list_opt:
	/* empty */
	  { empty($$, @$); }
	| identifier_list
	  { $$ = MV_P($1); }
;

identifier_list:
	TOK_IDENTIFIER
	  { $$ = new identifier_list(std::move($1)); }
	| identifier_list TOK_COMMA id_or_tdid
	  { $$ = MV_P($1); $$->extend(std::move($3)); }
;

declaration_list_opt:
	/* empty */
	  { empty($$, @$); }
	| declaration_list
	  { $$ = MV_P($1); }
;

declaration_list:
	declaration_not_starting_with_att
	  { $$ = new declaration_list(std::move($1)); }
	| declaration_list declaration
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

declaration_not_starting_with_att:
	declaration_specifiers_not_starting_with_att_no_ts init_declarator_list_no_tdid_opt TOK_SEMICOLON
	  {
	    pd.clear_in_typedef();
	    $$ = new declaration(@$, std::move($1), std::move($2));
	  }
	| declaration_specifiers_not_starting_with_att_ts init_declarator_list_tdid_opt TOK_SEMICOLON
	  {
	    pd.clear_in_typedef();
	    $$ = new declaration(@$, std::move($1), std::move($2));
	  }
;

declaration_specifiers_not_starting_with_att_no_ts_opt:
	/* empty */
	  { empty($$, @$); }
	| declaration_specifiers_not_starting_with_att_no_ts
	  { $$ = MV_P($1); }
;

declaration_specifiers_not_starting_with_att_no_ts:
	storage_class_specifier declaration_specifiers_no_ts_opt
	  {
	    $$ = new declaration_specifiers(std::move($1));
	    $$->extend(std::move($2));
	  }
	| type_qualifier declaration_specifiers_no_ts_opt
	  {
	    $$ = new declaration_specifiers(std::move($1));
	    $$->extend(std::move($2));
	  }
	| function_specifier declaration_specifiers_no_ts_opt
	  {
	    $$ = new declaration_specifiers(std::move($1));
	    $$->extend(std::move($2));
	  }
;

declaration_specifiers_not_starting_with_att_ts_no_tdid:
	declaration_specifiers_not_starting_with_att_no_ts_opt type_specifier_no_tdid
	  {
	    if (!$1) {
	      $$ = new declaration_specifiers(std::move($2));
	    } else {
	      $$ = MV_P($1); $$->extend(std::move($2));
	    }
	  }
	| declaration_specifiers_not_starting_with_att_ts_no_tdid type_specifier_no_tdid
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_not_starting_with_att_ts_no_tdid storage_class_specifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_not_starting_with_att_ts_no_tdid type_qualifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_not_starting_with_att_ts_no_tdid function_specifier
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
	| declaration_specifiers_not_starting_with_att_ts_no_tdid attribute_specifier_list %prec TOK_DUMMY_PRIO_GREEDY_ATTR_LIST
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

declaration_specifiers_not_starting_with_att_ts:
	declaration_specifiers_not_starting_with_att_ts_no_tdid
	  { $$ = MV_P($1); }
	| declaration_specifiers_not_starting_with_att_no_ts_opt TOK_TYPEDEF_IDENTIFIER declaration_specifiers_no_ts_opt
	  {
	    type_specifier_tdid *ts_tdid = new type_specifier_tdid($2);
	    if (!$1) {
	      try {
		$$ = new declaration_specifiers(std::move(ts_tdid));
	      } catch (...) {
		delete ts_tdid;
	      }
	    } else {
	      $$ = MV_P($1);
	      $$->extend(std::move(ts_tdid));
	    }
	    $$->extend(std::move($3));
	  }
;

init_declarator_list_no_tdid_opt:
	/* empty */
	  { empty($$, @$); }
	| init_declarator_list_no_tdid
	  { $$ = MV_P($1); }
;

init_declarator_list_no_tdid:
	init_declarator_no_tdid
	  { $$ = new init_declarator_list(std::move($1)); }
	| init_declarator_list_no_tdid TOK_COMMA attribute_specifier_list_opt init_declarator_no_tdid
	  {
	    $$ = MV_P($1);
	    $4->set_asl_before(std::move($3));
	    $$->extend(std::move($4));
	  }
;

init_declarator_no_tdid:
	declarator_no_tdid asm_label attribute_specifier_list_opt
	  {
	    pd.handle_decl_id($1->get_id_tok());
	    $$ = new init_declarator(@$, std::move($1), nullptr,
				     std::move($2), nullptr, std::move($3));
	  }
	| declarator_no_tdid attribute_specifier_list_opt
	  {
	    pd.handle_decl_id($1->get_id_tok());
	    $$ = new init_declarator(@$, std::move($1), nullptr,
				     nullptr, nullptr, std::move($2));
	  }
	| declarator_no_tdid asm_label attribute_specifier_list_opt
	  TOK_EQ { pd.handle_decl_id($1->get_id_tok()); }
	  initializer attribute_specifier_list_opt
	  {
	    $$ = new init_declarator(@$, std::move($1), std::move($6),
				     std::move($2), std::move($3),
				     std::move($7));
	  }
	| declarator_no_tdid attribute_specifier_list_opt
	  TOK_EQ { pd.handle_decl_id($1->get_id_tok()); }
	  initializer attribute_specifier_list_opt
	  {
	    $$ = new init_declarator(@$, std::move($1), std::move($5),
				     nullptr, std::move($2),
				     std::move($6));
	  }
;

init_declarator_list_tdid_opt:
	/* empty */
	  { empty($$, @$); }
	| init_declarator_list_tdid
	  { $$ = MV_P($1); }
;

init_declarator_list_tdid:
	init_declarator_tdid
	  { $$ = new init_declarator_list(std::move($1)); }
	| init_declarator_list_tdid TOK_COMMA attribute_specifier_list_opt init_declarator_tdid
	  {
	    $$ = MV_P($1);
	    $4->set_asl_before(std::move($3));
	    $$->extend(std::move($4));
	  }
;

init_declarator_tdid:
	declarator_tdid asm_label attribute_specifier_list_opt
	  {
	    pd.handle_decl_id($1->get_id_tok());
	    $$ = new init_declarator(@$, std::move($1), nullptr,
				     std::move($2), nullptr, std::move($3));
	  }
	| declarator_tdid attribute_specifier_list_opt
	  {
	    pd.handle_decl_id($1->get_id_tok());
	    $$ = new init_declarator(@$, std::move($1), nullptr,
				     nullptr, nullptr, std::move($2));
	  }
	| declarator_tdid asm_label attribute_specifier_list_opt
	  TOK_EQ { pd.handle_decl_id($1->get_id_tok()); }
	  initializer attribute_specifier_list_opt
	  {
	    $$ = new init_declarator(@$, std::move($1), std::move($6),
				     std::move($2), std::move($3),
				     std::move($7));
	  }
	| declarator_tdid attribute_specifier_list_opt
	  TOK_EQ { pd.handle_decl_id($1->get_id_tok()); }
	  initializer attribute_specifier_list_opt
	  {
	    $$ = new init_declarator(@$, std::move($1), std::move($5),
				     nullptr, std::move($2),
				     std::move($6));
	  }
;

asm_label:
	TOK_KW_ASM TOK_LPAREN string_literal TOK_RPAREN
	  { $$ = new asm_label(@$, std::move($3)); }
;

initializer:
	assignment_expression
	  { $$ = new initializer_expr(std::move($1)); }
	| TOK_LBRACE initializer_list TOK_RBRACE
	  { $$ = new initializer_init_list(@$, std::move($2)); }
	| TOK_LBRACE initializer_list TOK_COMMA TOK_RBRACE
	  { $$ = new initializer_init_list(@$, std::move($2)); }
	| TOK_LBRACE TOK_RBRACE
	  { $$ = new initializer_init_list(@$, nullptr); }
;

initializer_list:
	designation_opt initializer
	  {
	    $2->set_designation(std::move($1));
	    $$ = new initializer_list(std::move($2));
	  }
	| initializer_list TOK_COMMA designation_opt initializer
	  {
	    $$ = MV_P($1);
	    $4->set_designation(std::move($3));
	    $$->extend(std::move($4));
	  }
;

designation_opt:
	/* empty */
	  { empty($$, @$); }
	| designation
	  { $$ = MV_P($1); }
;

designation:
	designator_list TOK_EQ
	  { $$ = new designation(@$, std::move($1)); }
;

designator_list:
	designator
	  { $$ = new designator_list(std::move($1)); }
	| designator_list designator
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

designator:
	TOK_LBRACKET constant_expression TOK_RBRACKET
	  { $$ = new designator_array(@$, std::move($2), nullptr); }
	| TOK_LBRACKET constant_expression TOK_TRIPLE_DOT constant_expression TOK_RBRACKET
	  { $$ = new designator_array(@$, std::move($2), std::move($4)); }
	| TOK_DOT id_or_tdid
	  { $$ = new designator_member(@$, $2); }
;

statement:
	labeled_statement
	  { $$ = MV_P($1); }
	| compound_statement
	  { $$ = MV_P($1); }
	| expression_statement
	  { $$ = MV_P($1); }
	| selection_statement
	  { $$ = MV_P($1); }
	| iteration_statement
	  { $$ = MV_P($1); }
	| jump_statement
	  { $$ = MV_P($1); }
	| asm_statement
	  { $$ = MV_P($1); }
;

labeled_statement:
	id_or_tdid TOK_COLON attribute_specifier_list_opt statement
	  { $$ = new stmt_labeled(@$, $1, std::move($4), std::move($3)); }
	| TOK_KW_CASE constant_expression TOK_COLON statement
	  { $$ = new stmt_case(@$, std::move($2), std::move($4)); }
	| TOK_KW_CASE constant_expression TOK_TRIPLE_DOT constant_expression TOK_COLON statement
	  { $$ = new stmt_case_range(@$, std::move($2), std::move($4),
				     std::move($6));
	  }
	| TOK_KW_DEFAULT TOK_COLON statement
	  { $$ = new stmt_default(@$, std::move($3)); }
;

compound_statement:
	TOK_LBRACE local_label_declaration_list_opt block_item_list_opt TOK_RBRACE
	  { $$ = new stmt_compound(@$, std::move($2), std::move($3)); }
;

local_label_declaration_list_opt:
	/* empty */
	  { empty($$, @$); }
	| local_label_declaration_list
	  { $$ = MV_P($1); }
;

local_label_declaration_list:
	local_label_declaration
	  { $$ = new local_label_declaration_list(std::move($1)); }
	| local_label_declaration_list local_label_declaration
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

local_label_declaration:
	TOK_KW_LABEL identifier_list TOK_SEMICOLON
	  { $$ = new local_label_declaration(@$, std::move($2)); }
;

block_item_list_opt:
	/* empty */
	 { empty($$, @$); }
	| block_item_list
	  { $$ = MV_P($1); }
;

block_item_list:
	block_item
	  { $$ = new block_item_list(std::move($1)); }
	| block_item_list block_item
	  { $$ = MV_P($1); $$->extend(std::move($2)); }
;

block_item:
	declaration
	  { $$ = new block_item_decl(std::move($1)); }
	| statement
	  { $$ = new block_item_stmt(std::move($1)); }
	| function_definition
	  { $$ = new block_item_function_definition(std::move($1)); }
;

expression_statement:
	expression_opt TOK_SEMICOLON
	  { $$ = new stmt_expr(@$, std::move($1)); }
;


selection_statement:
	TOK_KW_IF TOK_LPAREN expression TOK_RPAREN statement %prec TOK_KW_IF
	  { $$ = new stmt_if(@$, std::move($3), std::move($5), nullptr); }
	| TOK_KW_IF TOK_LPAREN expression TOK_RPAREN statement TOK_KW_ELSE statement %prec TOK_KW_ELSE
	  { $$ = new stmt_if(@$, std::move($3), std::move($5), std::move($7)); }
	| TOK_KW_SWITCH TOK_LPAREN expression TOK_RPAREN statement
	  { $$ = new stmt_switch(@$, std::move($3), std::move($5)); }
;

iteration_statement:
	TOK_KW_WHILE TOK_LPAREN expression TOK_RPAREN statement
	  { $$ = new stmt_while(@$, std::move($3), std::move($5)); }
	| TOK_KW_DO statement TOK_KW_WHILE TOK_LPAREN expression TOK_RPAREN TOK_SEMICOLON
	  { $$ = new stmt_do(@$, std::move($5), std::move($2)); }
	| kw_for TOK_LPAREN expression_opt TOK_SEMICOLON expression_opt TOK_SEMICOLON expression_opt TOK_RPAREN statement
	  {
	    pd.leave_td_scope();
	    $$ = new stmt_for_init_expr(@$, std::move($3), std::move($5),
					std::move($7), std::move($9));
	  }
	| kw_for TOK_LPAREN declaration expression_opt TOK_SEMICOLON expression_opt TOK_RPAREN statement
	  {
	    pd.leave_td_scope();
	    /*
	     * Ugly hack: recheck whether the lookahead token is
	     * a typedef identifier.
	     */
	    if (yychar == token::TOK_IDENTIFIER) {
		if (pd.is_typedef_id(pd._tokens[yylval.token_index].get_value()))
			yychar = token::TOK_TYPEDEF_IDENTIFIER;
	    }
	    $$ = new stmt_for_init_decl(@$, std::move($3), std::move($4),
					std::move($6), std::move($8));
	  }
;

kw_for:
	TOK_KW_FOR
	  { pd.enter_td_scope(); }
;


jump_statement:
	TOK_KW_GOTO expression TOK_SEMICOLON
	  { $$ = new stmt_goto(@$, std::move($2)); }
	| TOK_KW_GOTO TOK_TYPEDEF_IDENTIFIER TOK_SEMICOLON
	  { $$ = new stmt_goto(@$, new expr_id($2)); }
	| TOK_KW_CONTINUE TOK_SEMICOLON
	  { $$ = new stmt_continue(@$); }
	| TOK_KW_BREAK TOK_SEMICOLON
	  { $$ = new stmt_break(@$); }
	| TOK_KW_RETURN expression_opt TOK_SEMICOLON
	  { $$ = new stmt_return(@$, std::move($2)); }
;


asm_statement:
	asm_directive
	  { $$ = new stmt_asm(std::move($1)); }
;

asm_directive:
	TOK_KW_ASM asm_qualifier_list_opt TOK_LPAREN string_literal TOK_RPAREN TOK_SEMICOLON
	  {
	    $$ = new asm_directive(@$, std::move($2), std::move($4),
				   nullptr, nullptr,
				   nullptr, nullptr);
	  }
	| TOK_KW_ASM asm_qualifier_list_opt TOK_LPAREN string_literal TOK_COLON asm_operand_list_opt TOK_RPAREN  TOK_SEMICOLON
	  {
	    $$ = new asm_directive(@$, std::move($2), std::move($4),
				   std::move($6), nullptr,
				   nullptr, nullptr);
	  }
	| TOK_KW_ASM asm_qualifier_list_opt TOK_LPAREN string_literal TOK_COLON asm_operand_list_opt TOK_COLON asm_operand_list_opt TOK_RPAREN  TOK_SEMICOLON
	  {
	    $$ = new asm_directive(@$, std::move($2), std::move($4),
				   std::move($6), std::move($8),
				   nullptr, nullptr);
	  }
	| TOK_KW_ASM asm_qualifier_list_opt TOK_LPAREN string_literal TOK_COLON asm_operand_list_opt TOK_COLON asm_operand_list_opt TOK_COLON asm_clobber_list_opt TOK_RPAREN TOK_SEMICOLON
	  {
	    $$ = new asm_directive(@$, std::move($2), std::move($4),
				   std::move($6), std::move($8),
				   std::move($10), nullptr);
	  }
	| TOK_KW_ASM asm_qualifier_list_opt TOK_LPAREN string_literal TOK_COLON asm_operand_list_opt TOK_COLON asm_operand_list_opt TOK_COLON asm_clobber_list_opt TOK_COLON asm_jump_to_label_list_opt TOK_RPAREN TOK_SEMICOLON
	  {
	    $$ = new asm_directive(@$, std::move($2), std::move($4),
				   std::move($6), std::move($8),
				   std::move($10), std::move($12));
	  }
;

asm_qualifier_list_opt:
	/* empty */
	  { empty($$, @$); }
	| asm_qualifier_list
	  { $$ = MV_P($1); }
;

asm_qualifier_list:
	TOK_KW_GOTO
	  { $$ = new asm_qualifier_list(@$, asm_qualifier::aq_goto); }
	| asm_qualifier_list TOK_KW_GOTO
	  { $$ = MV_P($1); $$->extend(@2, asm_qualifier::aq_goto); }
	| TOK_KW_VOLATILE
	  { $$ = new asm_qualifier_list(@$, asm_qualifier::aq_volatile); }
	| asm_qualifier_list TOK_KW_VOLATILE
	  { $$ = MV_P($1); $$->extend(@2, asm_qualifier::aq_volatile); }
;

asm_operand_list_opt:
	/* empty */
	  { empty($$, @$); }
	| asm_operand_list
	  { $$ =MV_P($1); }
;

asm_operand_list:
	asm_operand
	  { $$ = new asm_operand_list(std::move($1)); }
	| asm_operand_list TOK_COMMA asm_operand
	  { $$ = MV_P($1); $$->extend(std::move($3)); }
;

asm_operand:
	asm_operand_name_opt TOK_STRING_LITERAL TOK_LPAREN expression TOK_RPAREN
	  { $$ = new asm_operand(@$, std::move($1), $2, std::move($4)); }
;

asm_operand_name_opt:
	/* empty */
	  { empty($$, @$); }
	| asm_operand_name
	  { $$ = MV_P($1); }
;

asm_operand_name:
	TOK_LBRACKET id_or_tdid TOK_RBRACKET
	  { $$ = new asm_operand_name(@$, $2); }
;

asm_clobber_list_opt:
	/* empty */
	  { empty($$, @$); }
	| asm_clobber_list
	  { $$ = MV_P($1); }
;

asm_clobber_list:
	TOK_STRING_LITERAL
	  { $$ = new asm_clobber_list($1); }
	| asm_clobber_list TOK_COMMA TOK_STRING_LITERAL
	  { $$ = MV_P($1); $$->extend($3); }
;

asm_jump_to_label_list_opt:
	/* empty */
	  { empty($$, @$); }
	| asm_jump_to_label_list
	  { $$ = MV_P($1); }
;

asm_jump_to_label_list:
	id_or_tdid
	  { $$ = new asm_jump_to_label_list($1); }
	| asm_jump_to_label_list TOK_COMMA id_or_tdid
	  { $$ = MV_P($1); $$->extend($3); }
;

constant_expression:
	conditional_expression
	  { $$ = MV_P($1); }
;

expression_opt:
	/* empty */
	  { empty($$, @$); }
	| expression
	  { $$ = MV_P($1); }
;

expression:
	assignment_expression
	  { $$ = MV_P($1); }
	| expression TOK_COMMA assignment_expression
	  { $$ = new expr_comma(std::move($1), std::move($3)); }
;

assignment_expression_opt:
	/* empty */
	  { empty($$, @$); }
	| assignment_expression
	  { $$ = MV_P($1); }
;

assignment_expression:
	conditional_expression
	  { $$ = MV_P($1); }
	| unary_expression assignment_operator assignment_expression
	  { $$ = new expr_assignment($2, std::move($1), std::move($3)); }
;

assignment_operator:
	TOK_EQ { $$ = assign_op::set; }
	| TOK_OP_ASSIGN_MUL { $$ = assign_op::mul; }
	| TOK_OP_ASSIGN_DIV { $$ = assign_op::div; }
	| TOK_OP_ASSIGN_MOD { $$ = assign_op::mod; }
	| TOK_OP_ASSIGN_ADD { $$ = assign_op::add; }
	| TOK_OP_ASSIGN_SUB { $$ = assign_op::sub; }
	| TOK_OP_ASSIGN_LSHIFT { $$ = assign_op::lshift; }
	| TOK_OP_ASSIGN_RSHIFT { $$ = assign_op::rshift; }
	| TOK_OP_ASSIGN_BIN_AND { $$ = assign_op::bin_and; }
	| TOK_OP_ASSIGN_BIN_XOR { $$ = assign_op::bin_xor; }
	| TOK_OP_ASSIGN_BIN_OR { $$ = assign_op::bin_or; }
;

conditional_expression:
	logical_or_expression
	  { $$ = MV_P($1); }
	| logical_or_expression TOK_OP_QUESTION_MARK expression TOK_COLON conditional_expression
	  {
	    $$ = new expr_conditional(std::move($1), std::move($3),
				      std::move($5));
	  }
	| logical_or_expression TOK_OP_QUESTION_MARK TOK_COLON conditional_expression
	  { $$ = new expr_conditional(std::move($1), std::move($4)); }
;

logical_or_expression:
	logical_and_expression
	  { $$ = MV_P($1); }
	| logical_or_expression TOK_OP_LOGICAL_OR logical_and_expression
	  {
	    $$ = new expr_binop(binary_op::logical_or,
				std::move($1), std::move($3));
	  }
;

logical_and_expression:
	inclusive_or_expression
	  { $$ = MV_P($1); }
	| logical_and_expression TOK_OP_DOUBLE_AMPERSAND inclusive_or_expression
	  {
	    $$ = new expr_binop(binary_op::logical_and,
				std::move($1), std::move($3));
	  }
;

inclusive_or_expression:
	exclusive_or_expression
	  { $$ = MV_P($1); }
	| inclusive_or_expression TOK_OP_BIN_OR exclusive_or_expression
	  {
	    $$ = new expr_binop(binary_op::bin_or,
				std::move($1), std::move($3));
	  }
;

exclusive_or_expression:
	and_expression
	  { $$ = MV_P($1); }
	| exclusive_or_expression TOK_OP_BIN_XOR and_expression
	  {
	    $$ = new expr_binop(binary_op::bin_xor,
				std::move($1), std::move($3));
	  }
;

and_expression:
	equality_expression
	  { $$ = MV_P($1); }
	| and_expression TOK_OP_AMPERSAND equality_expression
	  {
	    $$ = new expr_binop(binary_op::bin_and,
				std::move($1), std::move($3));
	  }
;

equality_expression:
	relational_expression
	  { $$ = MV_P($1); }
	| equality_expression equality_operator relational_expression
	  { $$ = new expr_binop($2, std::move($1), std::move($3)); }
;

equality_operator:
	TOK_OP_EQ { $$ = binary_op::eq; }
	| TOK_OP_NEQ { $$ = binary_op::neq; }
;

relational_expression:
	shift_expression
	  { $$ = MV_P($1); }
	| relational_expression relational_operator shift_expression
	  { $$ = new expr_binop($2, std::move($1), std::move($3)); }
;

relational_operator:
	TOK_OP_REL_LT { $$ = binary_op::lt; }
	| TOK_OP_REL_GT { $$ = binary_op::gt; }
	| TOK_OP_REL_LEQ { $$ = binary_op::leq; }
	| TOK_OP_REL_GEQ { $$ = binary_op::geq; }
;

shift_expression:
	additive_expression
	  { $$ = MV_P($1); }
	| shift_expression shift_operator additive_expression
	  { $$ = new expr_binop($2, std::move($1), std::move($3)); }
;

shift_operator:
	TOK_OP_LSHIFT { $$ = binary_op::lshift; }
	| TOK_OP_RSHIFT { $$ = binary_op::rshift; }
;

additive_expression:
	multiplicative_expression
	  { $$ = MV_P($1); }
	| additive_expression TOK_OP_PLUS multiplicative_expression
	  {
	    $$ = new expr_binop(binary_op::add,
				std::move($1), std::move($3));
	  }
	| additive_expression TOK_OP_MINUS multiplicative_expression
	  {
	    $$ = new expr_binop(binary_op::sub,
				std::move($1), std::move($3));
	  }
;

multiplicative_expression:
	cast_expression
	  { $$ = MV_P($1); }
	| multiplicative_expression TOK_ASTERISK cast_expression
	  {
	    $$ = new expr_binop(binary_op::mul,
				std::move($1), std::move($3));
	  }
	| multiplicative_expression TOK_OP_DIV cast_expression
	  {
	    $$ = new expr_binop(binary_op::div,
				std::move($1), std::move($3));
	  }
	| multiplicative_expression TOK_OP_MODULO cast_expression
	  {
	    $$ = new expr_binop(binary_op::mod,
				std::move($1), std::move($3));
	  }
;

cast_expression:
	unary_expression
	  { $$ = MV_P($1); }
	| TOK_LPAREN type_name TOK_RPAREN cast_expression
	  { $$ = new expr_cast(@$, std::move($2), std::move($4)); }
;

unary_expression:
	TOK_OP_DOUBLE_AMPERSAND id_or_tdid
	  { $$ = new expr_label_addr(@$, $2); }
	| postfix_expression
	  { $$ = MV_P($1); }
	| TOK_OP_INC unary_expression
	  { $$ = new expr_unop_pre(@$, unary_op_pre::inc, std::move($2)); }
	| TOK_OP_DEC unary_expression
	  { $$ = new expr_unop_pre(@$, unary_op_pre::dec, std::move($2)); }
	| unary_operator cast_expression
	  { $$ = new expr_unop_pre(@$, $1, std::move($2)); }
	| TOK_KW_SIZEOF unary_expression
	  { $$ = new expr_sizeof_expr(@$, std::move($2)); }
	| TOK_KW_SIZEOF TOK_LPAREN type_name TOK_RPAREN
	  { $$ = new expr_sizeof_type_name(@$, std::move($3)); }
	| TOK_KW_ALIGNOF unary_expression
	  { $$ = new expr_alignof_expr(@$, std::move($2)); }
	| TOK_KW_ALIGNOF TOK_LPAREN type_name TOK_RPAREN
	  { $$ = new expr_alignof_type_name(@$, std::move($3)); }
	| TOK_KW_BUILTIN_OFFSETOF TOK_LPAREN type_name TOK_COMMA offset_member_designator TOK_RPAREN
	  { $$ = new expr_builtin_offsetof(@$, std::move($3), std::move($5)); }
	| TOK_KW_BUILTIN_TYPES_COMPATIBLE_P TOK_LPAREN type_name TOK_COMMA type_name TOK_RPAREN
	  { $$ = new expr_builtin_types_compatible_p(@$, std::move($3), std::move($5)); }
;

unary_operator:
	TOK_OP_AMPERSAND
	  { $$ = unary_op_pre::addr; }
	| TOK_ASTERISK
	  { $$ = unary_op_pre::deref; }
	| TOK_OP_PLUS
	  { $$ = unary_op_pre::plus; }
	| TOK_OP_MINUS
	  { $$ = unary_op_pre::neg; }
	| TOK_OP_BIN_NEG
	  { $$ = unary_op_pre::bin_neg; }
	| TOK_OP_LOGICAL_NOT
	  { $$ = unary_op_pre::logical_not; }
;

offset_member_designator:
	TOK_IDENTIFIER
	  { $$ = new offset_member_designator($1); }
	| offset_member_designator TOK_DOT TOK_IDENTIFIER
	  { $$ = MV_P($1); $$->extend($3, false); }
	| offset_member_designator TOK_OP_DEREF_MEMBER TOK_IDENTIFIER
	  { $$ = MV_P($1); $$->extend($3, true); }
	| offset_member_designator TOK_LBRACKET expression TOK_RBRACKET
	  { $$ = MV_P($1); $$->extend(std::move($3)); }
;

postfix_expression:
	statement_expression
	  { $$ = MV_P($1); }
	| postfix_expression TOK_LBRACKET expression TOK_RBRACKET
	  { $$ = new expr_array_subscript(@$, std::move($1), std::move($3)); }
	| postfix_expression TOK_LPAREN expression_list_opt TOK_RPAREN
	  {
	    $$ = $3 ? new expr_func_invocation(@$, std::move($1), std::move($3))
		    : new expr_func_invocation(@$, std::move($1));
	  }
	| postfix_expression TOK_DOT id_or_tdid
	  {
	    $$ = new expr_member_deref(@$, member_deref_type::non_ptr_base,
				       std::move($1), $3);
	  }
	| postfix_expression TOK_OP_DEREF_MEMBER id_or_tdid
	  {
	    $$ = new expr_member_deref(@$, member_deref_type::ptr_base,
				       std::move($1), $3);
	  }
	| postfix_expression TOK_OP_INC
	  { $$ = new expr_unop_post(@$, unary_op_post::inc, std::move($1)); }
	| postfix_expression TOK_OP_DEC
	  { $$ = new expr_unop_post(@$, unary_op_post::dec, std::move($1)); }
	| TOK_LPAREN type_name TOK_RPAREN TOK_LBRACE initializer_list TOK_RBRACE
	  { $$ = new expr_compound_literal(@$, std::move($2), std::move($5)); }
	| TOK_LPAREN type_name TOK_RPAREN TOK_LBRACE initializer_list TOK_COMMA TOK_RBRACE
	  { $$ = new expr_compound_literal(@$, std::move($2), std::move($5)); }
	| TOK_LPAREN type_name TOK_RPAREN TOK_LBRACE TOK_RBRACE
	  { $$ = new expr_compound_literal(@$, std::move($2), nullptr); }
	| TOK_KW_BUILTIN_VA_ARG TOK_LPAREN expression TOK_COMMA type_name TOK_RPAREN
	  { $$ = new expr_builtin_va_arg(@$, std::move($3), std::move($5)); }
;

statement_expression:
	primary_expression
	  { $$ = MV_P($1); }
	| TOK_LPAREN compound_statement TOK_RPAREN
	  { $$ = new expr_statement(@$, std::move($2)); }
;

primary_expression:
	TOK_IDENTIFIER
	  { $$ = new expr_id($1); }
	| TOK_CONSTANT
	  { $$ = new expr_constant($1); }
	| string_literal
	  { $$ = new expr_string_literal(MV_P($1)); }
	| TOK_LPAREN expression TOK_RPAREN
	  { $$ = new expr_parenthesized(@$, std::move($2)); }
;

string_literal:
	TOK_STRING_LITERAL
	  { $$ = new string_literal($1); }
	| string_literal TOK_STRING_LITERAL
	  { $$ = MV_P($1); $$->extend($2); }
;

%%

void suse::cp::yy::gnuc_parser::error(const location_type& loc,
				      const std::string& msg)
{
  pd.error(loc, msg);
}
