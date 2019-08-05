/*
 * Copyright (C) 2019  SUSE Software Solutions Germany GmbH
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

#ifndef LP_CREATION_POLICY_USER_COMMANDS_HH
#define LP_CREATION_POLICY_USER_COMMANDS_HH

#include "lp_creation_policy.hh"
#include "user_policy_command.hh"

namespace klp
{
  namespace ccp
  {
    namespace ast
    {
      class function_definition;
      class init_declarator;
      class direct_declarator_id;
    }

    class lp_creation_policy_user_commands final : public lp_creation_policy
    {
    public:
      lp_creation_policy_user_commands
	(const pp_result &pp_result,
	 char ** const envp,
	 std::vector<std::string> &&patched_functions,
	 std::unique_ptr<const user_policy_command> &&pol_cmd_is_patched,
	 user_policy_command &&pol_cmd_may_include_header,
	 user_policy_command &&pol_cmd_can_externalize_fun,
	 user_policy_command &&pol_cmd_shall_externalize_fun,
	 user_policy_command &&pol_cmd_shall_externalize_obj,
	 user_policy_command &&pol_cmd_mod_externalized_sym,
	 user_policy_command &&pol_cmd_mod_patched_fun_sym,
	 user_policy_command &&pol_cmd_rename_rewritten_fun);

      virtual ~lp_creation_policy_user_commands() noexcept override;

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

    private:
      static constexpr unsigned int _MAX_RENAME_RETRIES = 128;

      externalized_symbol_modification
      _get_sym_mod_for_externalized(const pp_token_index id_tok,
				    const char *what,
				    const allocated_ids_type &allocated_ids,
				    code_remarks &remarks) const;

      static pp_token_index _get_id_tok(const ast::init_declarator &id)
	noexcept;
      static pp_token_index _get_id_tok(const ast::function_definition &fd)
	noexcept;
      static pp_token_index _get_id_tok(const ast::direct_declarator_id &ddid)
	noexcept;

      void _handle_remarks(const pp_token_index id_tok,
			   const std::vector<std::string> &warnings,
			   const std::vector<std::string> &errors,
			   code_remarks &remarks) const;

      const pp_result &_pp_result;

      char ** const _envp;

      std::vector<std::string> _patched_functions;
      std::unique_ptr<const user_policy_command> _pol_cmd_is_patched;
      const user_policy_command _pol_cmd_may_include_header;
      const user_policy_command _pol_cmd_can_externalize_fun;
      const user_policy_command _pol_cmd_shall_externalize_fun;
      const user_policy_command _pol_cmd_shall_externalize_obj;
      const user_policy_command _pol_cmd_mod_externalized_sym;
      const user_policy_command _pol_cmd_mod_patched_fun_sym;
      const user_policy_command _pol_cmd_rename_rewritten_fun;
    };
  }
}

#endif
