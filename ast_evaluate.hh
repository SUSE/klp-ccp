#ifndef AST_EVALUATE_HH
#define AST_EVALUATE_HH

#include <memory>

namespace klp
{
  namespace ccp
  {
    class arch;

    namespace ast
    {
      class ast;
      class constexpr_value;
      class expr;
    }

    namespace types
    {
      class type;
    }

    void
    check_types_assignment(klp::ccp::ast::ast &a, const architecture &arch,
			   const types::type &t_target,
			   const ast::expr &e_source);
  }
}

#endif
