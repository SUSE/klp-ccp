#ifndef CREATE_LP_HH
#define CREATE_LP_HH

#include <string>

namespace klp
{
  namespace ast
  {
    class ast_translation_unit;
  }

  class lp_creation_policy;
  class code_remarks;
  class output_remarks;

  namespace ccp
  {
    void create_lp(const std::string &outfile,
		   const ast::ast_translation_unit &atu,
		   const lp_creation_policy &pol,
		   code_remarks &remarks,
		   output_remarks &output_remarks);
  }
}

#endif
