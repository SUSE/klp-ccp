#ifndef PREPROCESSOR_HH
#define PREPROCESSOR_HH

#include <stack>
#include <map>
#include <memory>
#include <string>
#include <queue>
#include "pp_tokens.hh"
#include "code_remarks.hh"
#include "header_resolver.hh"
#include "macro.hh"
#include "pp_tokenizer.hh"
#include "header_inclusion_tree.hh"

namespace suse
{
  namespace cp
  {
    class pp_tokenizer;
    class header_inclusion_root;
    class header_inclusion_node;

    namespace _preprocessor_impl
    {
      struct _expansion_state
      {
	_expansion_state();

	std::vector<macro::instance> macro_instances;
	std::queue<pp_token> pending_tokens;
	bool last_ws;
      };
    }

    class preprocessor
    {
    public:
      typedef std::vector<std::unique_ptr<header_inclusion_root> >
	header_inclusion_roots_type;

      preprocessor(header_inclusion_roots_type &header_inclusion_roots,
		   const header_resolver &header_resolver);

      preprocessor(std::vector<header_inclusion_root*> &&roots);

      pp_token read_next_token();

      code_remarks& get_remarks() noexcept
      { return _remarks; }

      header_inclusion_roots_type
      grab_header_inclusion_roots()
      { return std::move(_header_inclusion_roots); }

    private:
      template<typename T>
      void _grab_remarks_from(T &from);

      pp_token _read_next_plain_token();

      void _handle_pp_directive(pp_token &&sharp_tok);

      pp_token
      _expand(_preprocessor_impl::_expansion_state &state,
	      const std::function<pp_token()> &token_reader);

      macro::instance
      _handle_object_macro_invocation(const std::shared_ptr<const macro> &macro,
				      pp_token &&id_tok);

      macro::instance
      _handle_func_macro_invocation(
			const std::shared_ptr<const macro> &macro,
			used_macros &&used_macros_base,
			used_macro_undefs &&used_macro_undefs_base,
			const file_range &id_tok_file_range,
			const std::function<pp_token()> &token_reader);

      std::tuple<pp_tokens, pp_tokens, pp_token>
      _create_macro_arg(const std::function<pp_token()> &token_reader,
			const bool expand, const bool variadic,
			used_macros &used_macros_base,
			used_macro_undefs &used_macro_undefs_base);

      void _handle_include(pp_tokens &&directive_toks);

      const header_resolver &_header_resolver;

      header_inclusion_roots_type &_header_inclusion_roots;
      header_inclusion_roots_type::iterator _cur_header_inclusion_root;
      header_inclusion_node *_cur_header_inclusion_node;

      std::stack<pp_tokenizer> _tokenizers;
      _preprocessor_impl::_expansion_state _root_expansion_state;
      std::map<std::string, std::shared_ptr<const macro> > _macros;
      std::map<std::string, std::shared_ptr<const macro_undef> > _macro_undefs;

      code_remarks _remarks;

      std::size_t __counter__;
      std::queue<pp_token> _pending_tokens;
      bool _maybe_pp_directive;
      bool _line_empty;
   };
  }
}

#endif
