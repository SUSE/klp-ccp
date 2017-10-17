#include <map>
#include <cassert>
#include "gnuc_parser_driver.hh"
#include "pp_except.hh"
#include "parse_except.hh"


using namespace suse::cp::yy;
using namespace suse::cp::ast;

struct _val_tok_map_entry {
  const char *value;
  gnuc_parser::token_type tok_type;
};

static const _val_tok_map_entry kw_map_entries[] = {
  { "asm", gnuc_parser::token_type::TOK_KW_ASM },
  { "typedef", gnuc_parser::token_type::TOK_KW_TYPEDEF },
  { "extern", gnuc_parser::token_type::TOK_KW_EXTERN },
  { "static", gnuc_parser::token_type::TOK_KW_STATIC },
  { "auto", gnuc_parser::token_type::TOK_KW_AUTO },
  { "register", gnuc_parser::token_type::TOK_KW_REGISTER },
  { "void", gnuc_parser::token_type::TOK_KW_VOID },
  { "char", gnuc_parser::token_type::TOK_KW_CHAR },
  { "short", gnuc_parser::token_type::TOK_KW_SHORT },
  { "int", gnuc_parser::token_type::TOK_KW_INT },
  { "long", gnuc_parser::token_type::TOK_KW_LONG },
  { "float", gnuc_parser::token_type::TOK_KW_FLOAT },
  { "double", gnuc_parser::token_type::TOK_KW_DOUBLE },
  { "signed", gnuc_parser::token_type::TOK_KW_SIGNED },
  { "unsigned", gnuc_parser::token_type::TOK_KW_UNSIGNED },
  { "_Bool", gnuc_parser::token_type::TOK_KW_BOOL },
  { "complex", gnuc_parser::token_type::TOK_KW_COMPLEX },
  { "struct", gnuc_parser::token_type::TOK_KW_STRUCT },
  { "union", gnuc_parser::token_type::TOK_KW_UNION },
  { "enum", gnuc_parser::token_type::TOK_KW_ENUM },
  { "const", gnuc_parser::token_type::TOK_KW_CONST },
  { "restrict", gnuc_parser::token_type::TOK_KW_RESTRICT },
  { "volatile", gnuc_parser::token_type::TOK_KW_VOLATILE },
  { "inline", gnuc_parser::token_type::TOK_KW_INLINE },
  { "typeof", gnuc_parser::token_type::TOK_KW_TYPEOF },
  { "sizeof", gnuc_parser::token_type::TOK_KW_SIZEOF },
  { "__attribute__", gnuc_parser::token_type::TOK_KW_ATTRIBUTE },
  { "case", gnuc_parser::token_type::TOK_KW_CASE },
  { "default", gnuc_parser::token_type::TOK_KW_DEFAULT },
  { "label", gnuc_parser::token_type::TOK_KW_LABEL },
  { "if", gnuc_parser::token_type::TOK_KW_IF },
  { "else", gnuc_parser::token_type::TOK_KW_ELSE },
  { "switch", gnuc_parser::token_type::TOK_KW_SWITCH },
  { "while", gnuc_parser::token_type::TOK_KW_WHILE },
  { "do", gnuc_parser::token_type::TOK_KW_DO },
  { "for", gnuc_parser::token_type::TOK_KW_FOR },
  { "goto", gnuc_parser::token_type::TOK_KW_GOTO },
  { "continue", gnuc_parser::token_type::TOK_KW_CONTINUE },
  { "break", gnuc_parser::token_type::TOK_KW_BREAK },
  { "return", gnuc_parser::token_type::TOK_KW_RETURN },
  { nullptr, static_cast<gnuc_parser::token_type>(0) },
};

static const _val_tok_map_entry punct_map_entries[] = {
  { "(", gnuc_parser::token_type::TOK_LPAREN },
  { ")", gnuc_parser::token_type::TOK_RPAREN },
  { "{", gnuc_parser::token_type::TOK_LBRACE },
  { "}", gnuc_parser::token_type::TOK_RBRACE },
  { "[", gnuc_parser::token_type::TOK_LBRACKET },
  { "]", gnuc_parser::token_type::TOK_RBRACKET },
  { "...", gnuc_parser::token_type::TOK_TRIPLE_DOT },
  { ".", gnuc_parser::token_type::TOK_DOT },
  { ":", gnuc_parser::token_type::TOK_COLON },
  { ";", gnuc_parser::token_type::TOK_SEMICOLON },
  { ",", gnuc_parser::token_type::TOK_COMMA },
  { "*", gnuc_parser::token_type::TOK_ASTERISK },
  { "=", gnuc_parser::token_type::TOK_EQ },
  { "*=", gnuc_parser::token_type::TOK_OP_ASSIGN_MUL },
  { "/=", gnuc_parser::token_type::TOK_OP_ASSIGN_DIV },
  { "%=", gnuc_parser::token_type::TOK_OP_ASSIGN_MOD },
  { "+=", gnuc_parser::token_type::TOK_OP_ASSIGN_ADD },
  { "-=", gnuc_parser::token_type::TOK_OP_ASSIGN_SUB },
  { "<<=", gnuc_parser::token_type::TOK_OP_ASSIGN_LSHIFT },
  { ">>=", gnuc_parser::token_type::TOK_OP_ASSIGN_RSHIFT },
  { "&=", gnuc_parser::token_type::TOK_OP_ASSIGN_BIN_AND },
  { "^=", gnuc_parser::token_type::TOK_OP_ASSIGN_BIN_XOR },
  { "|=", gnuc_parser::token_type::TOK_OP_ASSIGN_BIN_OR },
  { "||", gnuc_parser::token_type::TOK_OP_LOGICAL_OR },
  { "&&", gnuc_parser::token_type::TOK_OP_DOUBLE_AMPERSAND },
  { "?", gnuc_parser::token_type::TOK_OP_QUESTION_MARK },
  { "->", gnuc_parser::token_type::TOK_OP_DEREF_MEMBER },
  { "++", gnuc_parser::token_type::TOK_OP_INC },
  { "--", gnuc_parser::token_type::TOK_OP_DEC },
  { "|", gnuc_parser::token_type::TOK_OP_BIN_OR },
  { "^", gnuc_parser::token_type::TOK_OP_BIN_XOR },
  { "<", gnuc_parser::token_type::TOK_OP_REL_LT },
  { ">", gnuc_parser::token_type::TOK_OP_REL_GT },
  { "<=", gnuc_parser::token_type::TOK_OP_REL_LEQ },
  { ">=", gnuc_parser::token_type::TOK_OP_REL_GEQ },
  { "==", gnuc_parser::token_type::TOK_OP_EQ },
  { "!=", gnuc_parser::token_type::TOK_OP_NEQ },
  { "+", gnuc_parser::token_type::TOK_OP_PLUS },
  { "-", gnuc_parser::token_type::TOK_OP_MINUS },
  { "&", gnuc_parser::token_type::TOK_OP_AMPERSAND },
  { "/", gnuc_parser::token_type::TOK_OP_DIV },
  { "%", gnuc_parser::token_type::TOK_OP_MODULO },
  { "~", gnuc_parser::token_type::TOK_OP_BIN_NEG },
  { "!", gnuc_parser::token_type::TOK_OP_LOGICAL_NOT },
  { "<<", gnuc_parser::token_type::TOK_OP_LSHIFT },
  { ">>", gnuc_parser::token_type::TOK_OP_RSHIFT },
  { nullptr, static_cast<gnuc_parser::token_type>(0) },
};

typedef std::map<std::string, gnuc_parser::token_type> _val_tok_map_type;

static _val_tok_map_type _initialize_val_tok_map(const _val_tok_map_entry *e)
{
  _val_tok_map_type m;

  for (; e->value; ++e)
    m.insert(std::make_pair(std::string(e->value), e->tok_type));

  return m;
}

void gnuc_parser_driver::parse()
{
  _parser.parse();
}

#ifdef DEBUG_PARSER
void gnuc_parser_driver::parse_debug()
{
  _parser.set_debug_level(1);
  parse();
}
#endif

gnuc_parser_driver::gnuc_parser_driver(const std::string &filename,
				       const header_resolver &header_resolver)
  : _pp(filename, header_resolver),
    _parser(*this), _ignore_td_spec(0), _in_typedef(false)
{
  _typedefs_scopes.emplace();
}

gnuc_parser::token_type
gnuc_parser_driver::lex(gnuc_parser::semantic_type *value,
			gnuc_parser::location_type *loc)
{
  do {
    try {
      _tokens.push_back(_pp.read_next_token());
    } catch (const pp_except&) {
      _grab_pp_remarks();
      throw;
    }
  } while (_tokens.back().is_ws() ||
	   _tokens.back().is_newline() ||
	   _tokens.back().is_empty());
  _grab_pp_remarks();

  loc->end = _tokens.size();
  loc->begin = loc->end - 1;

  const pp_token &tok = _tokens.back();
  if (tok.is_eof()) {
    leave_td_scope();
    return static_cast<gnuc_parser::token_type>(0);
  }

  static const _val_tok_map_type kw_map =
    _initialize_val_tok_map(kw_map_entries);
  static const _val_tok_map_type punct_map =
    _initialize_val_tok_map(punct_map_entries);
  _val_tok_map_type::const_iterator it_tok_type;
  switch (tok.get_type()) {
  case pp_token::type::pp_number:
    /* fall through */
  case pp_token::type::chr:
    /* fall through */
  case pp_token::type::wchr:
    value->token_index = loc->begin;
    return gnuc_parser::token_type::TOK_CONSTANT;

  case pp_token::type::str:
    /* fall through */
  case pp_token::type::wstr:
    value->token_index = loc->begin;
    return gnuc_parser::token_type::TOK_STRING_LITERAL;

  case pp_token::type::id:
    it_tok_type = kw_map.find(tok.get_value());
    if (it_tok_type != kw_map.cend())
      return it_tok_type->second;

    value->token_index = loc->begin;

    if (is_typedef_id(tok.get_value())) {
      return gnuc_parser::token_type::TOK_TYPEDEF_IDENTIFIER;
    }
    return gnuc_parser::token_type::TOK_IDENTIFIER;

  case pp_token::type::punctuator:
    it_tok_type = punct_map.find(tok.get_value());
    if (it_tok_type == punct_map.cend()) {
      code_remark remark(code_remark::severity::fatal,
			 "unrecognized preprocessor token (punctuator)",
			 tok.get_file_range());
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
      code_remark remark(code_remark::severity::fatal,
			 "unrecognized preprocessor token (non-ws char)",
			 tok.get_file_range());
      _remarks.add(remark);
      throw pp_except(remark);
    }
  }
  assert(0);
  __builtin_unreachable();
}

void gnuc_parser_driver::error(const gnuc_parser::location_type& loc,
			       const std::string& msg)
{
  code_remark remark(code_remark::severity::fatal, msg,
		     _tokens[loc.begin].get_file_range());
  _remarks.add(remark);
  throw parse_except(remark);
}

void gnuc_parser_driver::enter_td_scope()
{
  assert(!_typedefs_scopes.empty());
  _typedefs_scopes.push(_typedefs_scopes.top());
}

void gnuc_parser_driver::leave_td_scope()
{
  _typedefs_scopes.pop();
}

void gnuc_parser_driver::stash_td_scope()
{
  assert(_typedefs_scopes.size() > 1);
  _stashed_typedef_scope = std::move(_typedefs_scopes.top());
  _typedefs_scopes.pop();
}

void gnuc_parser_driver::restore_stashed_td_scope()
{
  assert(!_typedefs_scopes.empty());
  _typedefs_scopes.push(std::move(_stashed_typedef_scope));
}

void gnuc_parser_driver::set_in_typedef() noexcept
{
  // A typedef specifier in a parameter declaration is not
  // allowed. However we have to be prepared and ignore it for now.
  if (_ignore_td_spec)
    return;
  _in_typedef = true;
}

void gnuc_parser_driver::clear_in_typedef() noexcept
{
  assert(!_ignore_td_spec);
  _in_typedef = false;
}

void gnuc_parser_driver::begin_ignore_td_spec() noexcept
{
  ++_ignore_td_spec;
}

void gnuc_parser_driver::end_ignore_td_spec() noexcept
{
  --_ignore_td_spec;
}

void gnuc_parser_driver::handle_decl_id(const ast::pp_token_index tok)
{
  assert(!_typedefs_scopes.empty());
  assert(_tokens[tok].is_id());
  assert(!_ignore_td_spec);

  switch (_in_typedef)
  {
  case true:
    _typedefs_scopes.top().insert(_tokens[tok].get_value());
    break;

  case false:
    _typedefs_scopes.top().erase(_tokens[tok].get_value());
    break;
  };
}

void gnuc_parser_driver::handle_enumerator_id(const ast::pp_token_index tok)
{
  assert(!_typedefs_scopes.empty());
  assert(_tokens[tok].is_id());

  _typedefs_scopes.top().erase(_tokens[tok].get_value());
}

void gnuc_parser_driver::handle_param_id(const ast::pp_token_index tok)
{
  assert(!_typedefs_scopes.empty());
  assert(_tokens[tok].is_id());
  assert(_ignore_td_spec);
  _typedefs_scopes.top().erase(_tokens[tok].get_value());
}


bool gnuc_parser_driver::is_typedef_id(const std::string &id) const noexcept
{
  assert(!_typedefs_scopes.empty());
  return _typedefs_scopes.top().count(id);
}

void gnuc_parser_driver::_grab_pp_remarks()
{
  _remarks += _pp.get_remarks();
  _pp.get_remarks().clear();
}
