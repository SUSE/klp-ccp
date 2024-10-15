/*
 * Copyright (C) 2024  SUSE Software Solutions Germany GmbH
 *
 * This file is part of klp-ccp.
 *
 * klp-ccp is free software: you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * klp-ccp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _LP_CREATION_PYTHON_POLICY_HH
#define _LP_CREATION_PYTHON_POLICY_HH

#include <memory>
#include <stdexcept>
#include "lp_creation_policy.hh"
#include "ast.hh"

namespace klp
{
  namespace ccp
  {
    class python_except : public std::runtime_error
    {
    public:
      python_except(const char *what);
      virtual ~python_except() noexcept override;
    };

    void handle_python_except();

    class lp_creation_python_policy final : public lp_creation_policy
    {
    public:
      lp_creation_python_policy
	(const pp_result &pp_result,
	 std::vector<std::string> &&patched_functions,
	 const char * const python_policy_mod,
	 const char * const python_policy_cls);

      virtual ~lp_creation_python_policy() noexcept override;

      virtual bool
      is_patched(const ast::function_definition &fd,
		 code_remarks &remarks) const override;

      virtual bool
      is_header_eligible(const pp_result::header_inclusion_root &pre_include,
			 code_remarks &remarks) const override;

      virtual bool
      is_header_eligible(const pp_result::header_inclusion_child &h,
			 code_remarks &remarks) const override;

      virtual bool
      is_function_externalizable(const ast::function_definition &fd,
				 code_remarks &remarks) const override;

      virtual bool
      is_function_externalizable(const ast::init_declarator &id,
				 code_remarks &remarks) const override;

      virtual bool
      is_function_externalization_preferred(const ast::function_definition &fd,
					    const bool in_eligible_headers,
					    code_remarks &remarks)
	const override;

      virtual bool
      shall_externalize_object(const ast::init_declarator &id,
			       code_remarks &remarks) const override;

      virtual externalized_symbol_modification
      get_sym_mod_for_externalized_fun(const ast::direct_declarator_id &ddid,
				       const allocated_ids_type &allocated_ids,
				       code_remarks &remarks) const override;

      virtual symbol_modification
      get_sym_mod_for_patched_fun(const ast::function_definition &fd,
				  const allocated_ids_type &allocated_ids,
				  code_remarks &remarks) const override;

      virtual std::string
      rename_rewritten_closure_fun(const ast::function_definition &fd,
				   const allocated_ids_type &allocated_ids,
				   code_remarks &remarks) const override;

      virtual externalized_symbol_modification
      get_sym_mod_for_externalized_obj(const ast::init_declarator &id,
				       const allocated_ids_type &allocated_ids,
				       code_remarks &remarks) const override;

      static void teardown_python() noexcept;

    private:
      // Avoid pulling in declarations from Python.h here, keep all
      // related data encapuslated in a implementation internal
      // struct.
      struct _data;

      bool
      _is_header_eligible(const pp_result::header_inclusion_node &h,
			  const bool is_pre_include,
			  code_remarks &remarks) const;

      bool
      _is_function_externalizable(const pp_token_index id_tok,
				  const ast::linkage::linkage_kind linkage,
				  const bool is_definition,
				  code_remarks &remarks) const;

      static pp_token_index _get_id_tok(const ast::init_declarator &id)
	noexcept;
      static pp_token_index _get_id_tok(const ast::function_definition &fd)
	noexcept;
      static pp_token_index _get_id_tok(const ast::direct_declarator_id &ddid)
	noexcept;

      const pp_result &_pp_result;

      std::vector<std::string> _patched_functions;

      std::unique_ptr<_data> _d;
    };
  }
}

#endif
