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

      pp_token _yield_pending_ws();
      pp_token _read_next_plain_token();
      pp_token _read_next_token();

      static macro::instance
      _handle_object_macro_invocation(const std::shared_ptr<const macro> &macro,
				      pp_token &&id_tok);

      template <typename token_reader_type>
      macro::instance
      _handle_func_macro_invocation(const std::shared_ptr<const macro> &macro,
	pp_token &&id_tok, pp_token &&lparen_tok,
	token_reader_type &&token_reader);

      pp_tokens
      _expand_macro_arg(const pp_tokens &arg);

      void _handle_pp_directive(pp_token &&sharp_tok);

      header_resolver _header_resolver;
      std::stack<pp_tokenizer> _tokenizers;

      std::vector <macro::instance> _macro_instances;
      std::queue<pp_token> _stashed_tokens;

      std::map<std::string, std::shared_ptr<const macro> > _macros;
      code_remarks _remarks;

      std::unique_ptr<pp_token> _pending_ws;

      bool _maybe_pp_directive;
      bool _line_empty;
   };
  }
}

#endif
