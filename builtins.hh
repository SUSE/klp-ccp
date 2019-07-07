#ifndef BUILTINS_HH
#define BUILTINS_HH

#include <utility>
#include <memory>
#include <functional>
#include <vector>

namespace klp
{
  namespace ccp
  {
    class architecture;

    namespace ast
    {
      class constexpr_value;
      class ast;
      class expr_func_invocation;
      class expr_id;
      class type_specifier_tdid;
    }

    namespace types
    {
      class type;
      class addressable_type;
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

    std::unique_ptr<builtin_func> builtin_func_choose_expr_create();

    class builtin_var
    {
    public:
      struct evaluation_result_type
      {
	std::shared_ptr<const types::type> type;
	bool is_lvalue;
      };

      typedef std::unique_ptr<builtin_var> (*factory)();

      constexpr builtin_var() noexcept;
      virtual ~builtin_var() noexcept;

      virtual evaluation_result_type
      evaluate(ast::ast &a, const architecture &arch,
	       const ast::expr_id &eid) const = 0;

      static factory lookup(const std::string &id);
    };

    std::unique_ptr<builtin_var> builtin_var__func__create();


    class builtin_typedef
    {
    public:
      struct factory
      {
	typedef std::function<std::unique_ptr<builtin_typedef>()> create_type;

	factory(const std::string &_name, const create_type &_create);

	std::string name;
	create_type create;
      };

      typedef std::vector<factory> factories;

      virtual ~builtin_typedef() noexcept;

      virtual std::shared_ptr<const types::addressable_type>
      evaluate(ast::ast &a, const architecture &arch,
	       const ast::type_specifier_tdid &ts_tdid) const = 0;
    };
  }
}


#endif
