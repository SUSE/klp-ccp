%language "C++"
%skeleton "lalr1.cc"
%defines
%define namespace "suse::cp::yy"
%define parser_class_name "pp_expr_parser"
%name-prefix "pd."
%define api.location.type pp_tokens_range
%locations

%code requires {
#ifdef DEBUG_PARSER
#define YYDEBUG 1
#endif

#include "ast.hh"

namespace suse {
  namespace cp
  {
    namespace yy
    {
      // Bison doesn't accept namespace specifications at
      // %define api.location.type. Pull the type in.
      typedef suse::cp::ast::pp_tokens_range pp_tokens_range;

      class pp_expr_parser_driver;
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


#include "pp_expr_parser_driver.hh"

template<typename T>
static T* mv_p(T* &&p)
{
	T *_p = nullptr;
	std::swap(p, _p);
	return _p;
}

#define MV_P(p) mv_p(std::move(p))

}

%parse-param {suse::cp::yy::pp_expr_parser_driver &pd}


%union {
  suse::cp::ast::pp_token_index token_index;
  suse::cp::ast::binary_op binary_op;
  suse::cp::ast::unary_op_pre unary_op_pre;

  suse::cp::ast::expr *expr;
}

%destructor {} <token_index>
%destructor {} <binary_op>
%destructor {} <unary_op_pre>
%destructor { delete $$; } <*>


/* Preprocessor tokens. */
%token <token_index> TOK_STRING_LITERAL
%token <token_index> TOK_CONSTANT

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


%type <expr>	pp_expr
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
%type <expr>	unary_expression
%type <unary_op_pre>	unary_operator
%type <expr>	primary_expression

%%

start:
	pp_expr
	  { pd._result = MV_P($1); }
;

pp_expr:
	conditional_expression
	  { $$ = MV_P($1); }
	| pp_expr TOK_COMMA conditional_expression
	  { $$ = new expr_comma(std::move($1), std::move($3)); }
;

conditional_expression:
	logical_or_expression
	  { $$ = MV_P($1); }
	| logical_or_expression TOK_OP_QUESTION_MARK pp_expr TOK_COLON conditional_expression
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
	unary_expression
	  { $$ = MV_P($1); }
	| multiplicative_expression TOK_ASTERISK unary_expression
	  {
	    $$ = new expr_binop(binary_op::mul,
				std::move($1), std::move($3));
	  }
	| multiplicative_expression TOK_OP_DIV unary_expression
	  {
	    $$ = new expr_binop(binary_op::div,
				std::move($1), std::move($3));
	  }
	| multiplicative_expression TOK_OP_MODULO unary_expression
	  {
	    $$ = new expr_binop(binary_op::mod,
				std::move($1), std::move($3));
	  }
;

unary_expression:
	primary_expression
	  { $$ = MV_P($1); }
	| unary_operator unary_expression
	  { $$ = new expr_unop_pre(@$, $1, std::move($2)); }
;

unary_operator:
	TOK_OP_PLUS
	  { $$ = unary_op_pre::plus; }
	| TOK_OP_MINUS
	  { $$ = unary_op_pre::neg; }
	| TOK_OP_BIN_NEG
	  { $$ = unary_op_pre::bin_neg; }
	| TOK_OP_LOGICAL_NOT
	  { $$ = unary_op_pre::logical_not; }
;

primary_expression:
	TOK_CONSTANT
	  { $$ = new expr_constant($1); }
	| TOK_LPAREN pp_expr TOK_RPAREN
	  { $$ = new expr_parenthesized(@$, std::move($2)); }
;

%%

void suse::cp::yy::pp_expr_parser::error(const location_type& loc,
					 const std::string& msg)
{
  pd.error(loc, msg);
}
