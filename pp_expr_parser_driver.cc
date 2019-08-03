#include <map>
#include "pp_expr_parser_driver.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "raw_pp_token.hh"

using namespace klp::ccp::yy;
using namespace klp::ccp::ast;

struct _val_tok_map_entry {
  const char *value;
  pp_expr_parser::token_type tok_type;
};

static const _val_tok_map_entry punct_map_entries[] = {
  { "(", pp_expr_parser::token_type::TOK_LPAREN },
  { ")", pp_expr_parser::token_type::TOK_RPAREN },
  { "{", pp_expr_parser::token_type::TOK_LBRACE },
  { "}", pp_expr_parser::token_type::TOK_RBRACE },
  { "[", pp_expr_parser::token_type::TOK_LBRACKET },
  { "]", pp_expr_parser::token_type::TOK_RBRACKET },
  { "...", pp_expr_parser::token_type::TOK_TRIPLE_DOT },
  { ".", pp_expr_parser::token_type::TOK_DOT },
  { ":", pp_expr_parser::token_type::TOK_COLON },
  { ";", pp_expr_parser::token_type::TOK_SEMICOLON },
  { ",", pp_expr_parser::token_type::TOK_COMMA },
  { "*", pp_expr_parser::token_type::TOK_ASTERISK },
  { "=", pp_expr_parser::token_type::TOK_EQ },
  { "*=", pp_expr_parser::token_type::TOK_OP_ASSIGN_MUL },
  { "/=", pp_expr_parser::token_type::TOK_OP_ASSIGN_DIV },
  { "%=", pp_expr_parser::token_type::TOK_OP_ASSIGN_MOD },
  { "+=", pp_expr_parser::token_type::TOK_OP_ASSIGN_ADD },
  { "-=", pp_expr_parser::token_type::TOK_OP_ASSIGN_SUB },
  { "<<=", pp_expr_parser::token_type::TOK_OP_ASSIGN_LSHIFT },
  { ">>=", pp_expr_parser::token_type::TOK_OP_ASSIGN_RSHIFT },
  { "&=", pp_expr_parser::token_type::TOK_OP_ASSIGN_BIN_AND },
  { "^=", pp_expr_parser::token_type::TOK_OP_ASSIGN_BIN_XOR },
  { "|=", pp_expr_parser::token_type::TOK_OP_ASSIGN_BIN_OR },
  { "||", pp_expr_parser::token_type::TOK_OP_LOGICAL_OR },
  { "&&", pp_expr_parser::token_type::TOK_OP_DOUBLE_AMPERSAND },
  { "?", pp_expr_parser::token_type::TOK_OP_QUESTION_MARK },
  { "->", pp_expr_parser::token_type::TOK_OP_DEREF_MEMBER },
  { "++", pp_expr_parser::token_type::TOK_OP_INC },
  { "--", pp_expr_parser::token_type::TOK_OP_DEC },
  { "|", pp_expr_parser::token_type::TOK_OP_BIN_OR },
  { "^", pp_expr_parser::token_type::TOK_OP_BIN_XOR },
  { "<", pp_expr_parser::token_type::TOK_OP_REL_LT },
  { ">", pp_expr_parser::token_type::TOK_OP_REL_GT },
  { "<=", pp_expr_parser::token_type::TOK_OP_REL_LEQ },
  { ">=", pp_expr_parser::token_type::TOK_OP_REL_GEQ },
  { "==", pp_expr_parser::token_type::TOK_OP_EQ },
  { "!=", pp_expr_parser::token_type::TOK_OP_NEQ },
  { "+", pp_expr_parser::token_type::TOK_OP_PLUS },
  { "-", pp_expr_parser::token_type::TOK_OP_MINUS },
  { "&", pp_expr_parser::token_type::TOK_OP_AMPERSAND },
  { "/", pp_expr_parser::token_type::TOK_OP_DIV },
  { "%", pp_expr_parser::token_type::TOK_OP_MODULO },
  { "~", pp_expr_parser::token_type::TOK_OP_BIN_NEG },
  { "!", pp_expr_parser::token_type::TOK_OP_LOGICAL_NOT },
  { "<<", pp_expr_parser::token_type::TOK_OP_LSHIFT },
  { ">>", pp_expr_parser::token_type::TOK_OP_RSHIFT },
  { nullptr, static_cast<pp_expr_parser::token_type>(0) },
};

typedef std::map<std::string, pp_expr_parser::token_type> _val_tok_map_type;

static _val_tok_map_type _initialize_val_tok_map(const _val_tok_map_entry *e)
{
  _val_tok_map_type m;

  for (; e->value; ++e)
    m.insert(std::make_pair(std::string(e->value), e->tok_type));

  return m;
}


pp_expr_parser_driver::
pp_expr_parser_driver(const std::function<pp_token_index()> &token_reader,
		      pp_result &pp_result)
  : _token_reader(token_reader), _pp_result(pp_result),
    _result(nullptr), _parser(*this)
{}

pp_expr_parser_driver::~pp_expr_parser_driver() noexcept
{
  delete _result;
}

void pp_expr_parser_driver::parse()
{
  _parser.parse();
}

ast_pp_expr pp_expr_parser_driver::grab_result()
{
  std::unique_ptr<expr> e(_result);
  _result = nullptr;

  return ast_pp_expr(_pp_result, std::move(e));
}

klp::ccp::yy::pp_expr_parser::token_type
pp_expr_parser_driver::lex(pp_expr_parser::semantic_type *value,
			   pp_expr_parser::location_type *loc)
{
  pp_tokens &tokens = _pp_result.get_pp_tokens();
  pp_token_index last_index;
  do {
    try {
      last_index = _token_reader();
    } catch (const pp_except&) {
      throw;
    }
  } while (tokens.back().is_type_any_of<pp_token::type::ws,
					pp_token::type::empty,
					pp_token::type::newline>() ||
	   (tokens.back().is_id() &&
	    tokens.back().get_value() == "__extension__"));

  loc->end = last_index + 1;
  loc->begin = last_index;

  pp_token &tok = tokens.back();
  if (tok.is_eof()) {
    return static_cast<pp_expr_parser::token_type>(0);
  }

  static const _val_tok_map_type punct_map =
    _initialize_val_tok_map(punct_map_entries);
  _val_tok_map_type::const_iterator it_tok_type;
  switch (tok.get_type()) {
  case pp_token::type::pp_number:
    /* fall through */
  case pp_token::type::chr:
    /* fall through */
  case pp_token::type::wchr:
    /* fall through */
  case pp_token::type::uchr16:
    /* fall through */
  case pp_token::type::uchr32:
    value->token_index = loc->begin;
    return pp_expr_parser::token_type::TOK_CONSTANT;

  case pp_token::type::str:
    /* fall through */
  case pp_token::type::wstr:
    /* fall through */
  case pp_token::type::ustr8:
    /* fall through */
  case pp_token::type::ustr16:
    /* fall through */
  case pp_token::type::ustr32:
    value->token_index = loc->begin;
    return pp_expr_parser::token_type::TOK_STRING_LITERAL;

  case pp_token::type::id:
    tok.set_type_and_value(pp_token::type::pp_number, "0");
    value->token_index = loc->begin;
    return pp_expr_parser::token_type::TOK_CONSTANT;

  case pp_token::type::punctuator:
    it_tok_type = punct_map.find(tok.get_value());
    if (it_tok_type == punct_map.cend()) {
      const raw_pp_token_index tok_index = tok.get_token_source().begin;
      const raw_pp_token &raw_tok =
	_pp_result.get_raw_tokens()[tok_index];
      code_remark remark(code_remark::severity::fatal,
			 "unrecognized preprocessor token (punctuator)",
			 _pp_result.get_raw_token_source(tok_index),
			 raw_tok.get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }
    return it_tok_type->second;

  case pp_token::type::ws:
    /* fall through */
  case pp_token::type::newline:
    /* fall through */
  case pp_token::type::empty:
    /* fall through */
  case pp_token::type::eof:
    /* already handled above */
    assert(0);
    __builtin_unreachable();

  case pp_token::type::qstr:
    /* fall through */
  case pp_token::type::hstr:
    /* preprocessor won't return this */
    assert(0);
    __builtin_unreachable();

  case pp_token::type::non_ws_char:
    {
      const raw_pp_token_index tok_index = tok.get_token_source().begin;
      const raw_pp_token &raw_tok =
	_pp_result.get_raw_tokens()[tok_index];
      code_remark remark(code_remark::severity::fatal,
			 "unrecognized preprocessor token (non-ws char)",
			 _pp_result.get_raw_token_source(tok_index),
			 raw_tok.get_range_in_file());
      _remarks.add(remark);
      throw pp_except(remark);
    }
  }
  assert(0);
  __builtin_unreachable();
}

void pp_expr_parser_driver::error(const pp_expr_parser::location_type& loc,
				  const std::string& msg)
{
  code_remark remark(code_remark::severity::fatal, msg,
		     _pp_result, loc.begin);
  _remarks.add(remark);
  throw parse_except(remark);
}
