#ifndef LP_CREATION_POLICY_HH
#define LP_CREATION_POLICY_HH

#include <string>
#include <set>
#include "pp_result.hh"

namespace klp
{
  namespace ccp
  {
    namespace ast
    {
      class direct_declarator_id;
      class init_declarator;
      class function_definition;
    };

    class code_remarks;

    class lp_creation_policy
    {
    public:
      virtual ~lp_creation_policy() noexcept;

      virtual bool
      is_patched(const ast::function_definition &fd,
		 code_remarks &remarks) const = 0;

      virtual bool
      is_header_eligible(const pp_result::header_inclusion_root &pre_include,
			 code_remarks &remarks) const = 0;

      virtual bool
      is_header_eligible(const pp_result::header_inclusion_child &h,
			 code_remarks &remarks) const = 0;

      virtual bool
      is_function_externalizable(const ast::function_definition &fd,
				 code_remarks &remarks) const = 0;

      virtual bool
      is_function_externalizable(const ast::init_declarator &id,
				 code_remarks &remarks) const = 0;

      virtual bool
      is_function_externalization_preferred(const ast::function_definition &fd,
					    const bool in_eligible_headers,
					    code_remarks &remarks) const = 0;

      virtual bool
      shall_externalize_object(const ast::init_declarator &id,
			       code_remarks &remarks) const = 0;

      struct symbol_modification
      {
	enum class linkage_change
	{
	  lc_none,
	  lc_make_static,
	  lc_make_extern,
	};

	symbol_modification();

	symbol_modification(std::string &&_new_name,
			    const linkage_change _new_linkage);

	bool is_rename() const noexcept
	{ return !new_name.empty(); }

	std::string new_name;
	linkage_change new_linkage;
      };

      struct externalized_symbol_modification
      {
	externalized_symbol_modification();

	externalized_symbol_modification
		(std::string &&_new_name,
		 const symbol_modification::linkage_change _new_linkage,
		 const bool _make_pointer);


	symbol_modification sym_mod;
	bool make_pointer;
      };

      typedef std::set<std::string> allocated_ids_type;

      virtual externalized_symbol_modification
      get_sym_mod_for_externalized_fun(const ast::direct_declarator_id &ddid,
				       const allocated_ids_type &allocated_ids,
				       code_remarks &remarks) const = 0;

      virtual symbol_modification
      get_sym_mod_for_patched_fun(const ast::function_definition &fd,
				  const allocated_ids_type &allocated_ids,
				  code_remarks &remarks) const = 0;

      virtual std::string
      rename_rewritten_closure_fun(const ast::function_definition &fd,
				   const allocated_ids_type &allocated_ids,
				   code_remarks &remarks) const = 0;

      virtual externalized_symbol_modification
      get_sym_mod_for_externalized_obj(const ast::init_declarator &id,
				       const allocated_ids_type &allocated_ids,
				       code_remarks &remarks) const = 0;
    };
  }
}

#endif
