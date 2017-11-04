#ifndef GNUC_PARSER_DRIVER_HH
#define GNUC_PARSER_DRIVER_HH

#include <stack>
#include <string>
#include <set>

#include "gnuc_parser.hh"
#include "pp_tokens.hh"
#include "preprocessor.hh"
#include "code_remarks.hh"

namespace suse
{
  namespace cp
  {
    namespace yy
    {
      class gnuc_parser_driver
      {
      public:
	gnuc_parser_driver(preprocessor &&pp);

	gnuc_parser_driver(gnuc_parser_driver&&) = delete;

	void parse();

#ifdef DEBUG_PARSER
	void parse_debug();
#endif

	code_remarks& get_remarks() noexcept
	{ return _remarks; }

      private:
	suse::cp::yy::gnuc_parser::token_type
	lex(gnuc_parser::semantic_type *value,
	    gnuc_parser::location_type *loc);

	void error(const gnuc_parser::location_type& loc,
		   const std::string& msg);

	void enter_td_scope();
	void leave_td_scope();

	void stash_td_scope();
	void restore_stashed_td_scope();

	void set_in_typedef() noexcept;
	void clear_in_typedef() noexcept;

	void begin_ignore_td_spec() noexcept;
	void end_ignore_td_spec() noexcept;

	void handle_decl_id(const ast::pp_token_index tok);
	void handle_enumerator_id(const ast::pp_token_index tok);
	void handle_param_id(const ast::pp_token_index tok);

	bool is_typedef_id(const std::string &id) const noexcept;

	void _grab_pp_remarks();

	typedef std::set<std::string> _typedefs_type;
	typedef std::stack<_typedefs_type> _typedefs_scopes_type;

	_typedefs_scopes_type _typedefs_scopes;
	_typedefs_type _stashed_typedef_scope;

	pp_tokens _tokens;

	preprocessor _pp;
	gnuc_parser _parser;

	code_remarks _remarks;

	unsigned int _ignore_td_spec;
	bool _in_typedef;

	friend class gnuc_parser;
      };
    }
  }
}

#endif