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

namespace suse
{
  namespace cp
  {
    class pp_tokenizer;

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
      preprocessor(const std::string &filename,
		   const header_resolver &header_resolver);

      pp_token read_next_token();

      code_remarks& get_remarks() noexcept
      { return _remarks; }

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
			const file_range &id_tok_file_range,
			const std::function<pp_token()> &token_reader);

      std::tuple<pp_tokens, pp_tokens, pp_token>
      _create_macro_arg(const std::function<pp_token()> &token_reader,
			const bool expand, const bool variadic);

      header_resolver _header_resolver;

      std::stack<pp_tokenizer> _tokenizers;
      _preprocessor_impl::_expansion_state _root_expansion_state;
      std::map<std::string, std::shared_ptr<const macro> > _macros;

      code_remarks _remarks;

      bool _maybe_pp_directive;
      bool _line_empty;
   };
  }
}

#endif
