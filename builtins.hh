#ifndef BUILTINS_HH
#define BUILTINS_HH

#include <utility>
#include <memory>

namespace suse
{
  namespace cp
  {
    class architecture;

    namespace ast
    {
      class constexpr_value;
      class ast;
      class expr_func_invocation;
    }

    namespace types
    {
      class type;
    }

    class builtin_func
    {
    public:
      struct evaluation_result_type
      {
	std::shared_ptr<const types::type> type;
	std::unique_ptr<ast::constexpr_value> value;
	bool is_lvalue;
      };

      typedef std::unique_ptr<builtin_func> (*factory)();

      constexpr builtin_func() noexcept;
      virtual ~builtin_func() noexcept;

      virtual evaluation_result_type
      evaluate(ast::ast &a, const architecture &arch,
	       const ast::expr_func_invocation &efi) const = 0;

      static factory lookup(const std::string &id);
    };
  }
}


#endif
