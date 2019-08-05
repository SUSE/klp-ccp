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

#include "lp_creation_policy_user_commands.hh"
#include "code_remarks.hh"
#include "lp_except.hh"
#include "ast.hh"
#include "raw_pp_token.hh"

using namespace klp::ccp;

lp_creation_policy_user_commands
::lp_creation_policy_user_commands
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
	 user_policy_command &&pol_cmd_rename_rewritten_fun)
  : _pp_result(pp_result), _envp(envp),
    _patched_functions(std::move(patched_functions)),
    _pol_cmd_is_patched(std::move(pol_cmd_is_patched)),
    _pol_cmd_may_include_header(std::move(pol_cmd_may_include_header)),
    _pol_cmd_can_externalize_fun(std::move(pol_cmd_can_externalize_fun)),
    _pol_cmd_shall_externalize_fun(std::move(pol_cmd_shall_externalize_fun)),
    _pol_cmd_shall_externalize_obj(std::move(pol_cmd_shall_externalize_obj)),
    _pol_cmd_mod_externalized_sym(std::move(pol_cmd_mod_externalized_sym)),
    _pol_cmd_mod_patched_fun_sym(std::move(pol_cmd_mod_patched_fun_sym)),
    _pol_cmd_rename_rewritten_fun(std::move(pol_cmd_rename_rewritten_fun))
{}

lp_creation_policy_user_commands::~lp_creation_policy_user_commands() noexcept
{}

bool
lp_creation_policy_user_commands::is_patched(const ast::function_definition &fd,
					     code_remarks &remarks) const
{

  const pp_token_index id_tok = _get_id_tok(fd);
  if (std::find(_patched_functions.cbegin(),
		_patched_functions.cend(),
		_pp_result.get_pp_tokens()[id_tok].get_value())
      != _patched_functions.cend()) {
    return true;
  }

  if (_pol_cmd_is_patched) {
    std::vector<std::string> args;
    args.reserve(1);
    args.push_back(_pp_result.get_pp_tokens()[id_tok].get_value());
    auto i = _pol_cmd_is_patched->execute(std::move(args), _envp,
					  std::regex("(YES|NO)"));
    i.wait();
    _handle_remarks(id_tok, i.get_warnings(), i.get_errors(), remarks);

    if (i.get_matched_result()[1].str() == "YES")
      return true;

    assert(i.get_matched_result()[1].str() == "NO");
    return false;
  }

  return false;
}

bool lp_creation_policy_user_commands::
is_header_eligible(const pp_result::header_inclusion_root &pre_include,
		   code_remarks &remarks) const
{
  std::vector<std::string> args;
  args.reserve(2);
  args.push_back(pre_include.get_filename());
  args.push_back("pre-include");

  auto i = _pol_cmd_may_include_header.execute(std::move(args), _envp,
					       std::regex("(YES|NO)"));
  i.wait();

  for (const auto &w : i.get_warnings()) {
    code_remark remark{
	code_remark::severity::warning,
	w,
	pre_include,
	range_in_file{0}
    };
    remarks.add(std::move(remark));
  }

  if (!i.get_errors().empty()) {
    for (const auto &e : i.get_errors()) {
      code_remark remark{
	code_remark::severity::fatal,
	e,
	pre_include,
	range_in_file{0}
      };
      remarks.add(std::move(remark));
    }

    code_remark remark{
	code_remark::severity::fatal,
	i.get_errors().front(),
	pre_include,
	range_in_file{0}
    };
    throw lp_except(std::move(remark));
  }

  if (i.get_matched_result()[1].str() == "YES")
    return true;

  assert(i.get_matched_result()[1].str() == "NO");
  return false;
}

bool lp_creation_policy_user_commands::
is_header_eligible(const pp_result::header_inclusion_child &h,
		   code_remarks &remarks) const
{
  std::vector<std::string> args;
  args.reserve(1);
  args.push_back(h.get_filename());

  auto i = _pol_cmd_may_include_header.execute(std::move(args), _envp,
					       std::regex("(YES|NO)"));
  i.wait();

  for (const auto &w : i.get_warnings()) {
    code_remark remark{
	code_remark::severity::warning,
	w,
	_pp_result,
	h.get_directive_range()
    };
    remarks.add(std::move(remark));
  }

  if (!i.get_errors().empty()) {
    for (const auto &e : i.get_errors()) {
      code_remark remark{
	code_remark::severity::fatal,
	e,
	_pp_result,
	h.get_directive_range()
      };
      remarks.add(remark);
    }

    code_remark remark{
	code_remark::severity::fatal,
	i.get_errors().front(),
	_pp_result,
	h.get_directive_range()
    };
    throw lp_except(std::move(remark));
  }

  if (i.get_matched_result()[1].str() == "YES")
    return true;

  assert(i.get_matched_result()[1].str() == "NO");
  return false;
}

bool lp_creation_policy_user_commands::
is_function_externalizable(const ast::function_definition &fd,
			   code_remarks &remarks) const
{
  std::vector<std::string> args;
  args.reserve(3);

  const pp_token_index id_tok = _get_id_tok(fd);
  args.push_back(_pp_result.get_pp_tokens()[id_tok].get_value());

  switch (fd.get_linkage().get_linkage_kind()) {
  case ast::linkage::linkage_kind::internal:
    args.push_back("internal");
    break;

  case ast::linkage::linkage_kind::external:
    args.push_back("external");
    break;

  case ast::linkage::linkage_kind::none:
    /* fall through */
  case ast::linkage::linkage_kind::nested_fun_auto:
    assert(0);
    __builtin_unreachable();
  }

  const raw_pp_token_index &raw_id_tok
    = (_pp_result.pp_tokens_range_to_raw(pp_tokens_range{id_tok, id_tok + 1})
       .begin);
  const auto id_tok_source =
    _pp_result.intersecting_sources_begin(raw_pp_tokens_range{raw_id_tok});
  assert(id_tok_source !=
	 (_pp_result.intersecting_sources_end
	  (raw_pp_tokens_range{raw_id_tok})));
  const auto id_tok_line_col
    = (id_tok_source->offset_to_line_col
       (_pp_result.get_raw_tokens()[raw_id_tok].get_range_in_file().begin));
  args.push_back(id_tok_source->get_filename()
		 + ":" + std::to_string(id_tok_line_col.first)
		 + ":" + std::to_string(id_tok_line_col.second));

  auto i = _pol_cmd_can_externalize_fun.execute(std::move(args), _envp,
						std::regex("(YES|NO)"));
  i.wait();
  _handle_remarks(id_tok, i.get_warnings(), i.get_errors(), remarks);

  if (i.get_matched_result()[1].str() == "YES")
    return true;

  assert(i.get_matched_result()[1].str() == "NO");
  return false;
}

bool lp_creation_policy_user_commands::
is_function_externalizable(const ast::init_declarator &id,
			   code_remarks &remarks) const
{
  std::vector<std::string> args;
  args.reserve(1);

  const pp_token_index id_tok = _get_id_tok(id);
  args.push_back(_pp_result.get_pp_tokens()[id_tok].get_value());

  auto i = _pol_cmd_can_externalize_fun.execute(std::move(args), _envp,
						std::regex("(YES|NO)"));
  i.wait();
  _handle_remarks(id_tok, i.get_warnings(), i.get_errors(), remarks);

  if (i.get_matched_result()[1].str() == "YES")
    return true;

  assert(i.get_matched_result()[1].str() == "NO");
  return false;
}

bool lp_creation_policy_user_commands::
is_function_externalization_preferred(const ast::function_definition &fd,
				      const bool in_eligible_headers,
				      code_remarks &remarks) const
{
  std::vector<std::string> args;
  args.reserve(2);

  const pp_token_index id_tok = _get_id_tok(fd);
  args.push_back(_pp_result.get_pp_tokens()[id_tok].get_value());

  if (in_eligible_headers)
    args.push_back("from-header");

  auto i = _pol_cmd_shall_externalize_fun.execute(std::move(args), _envp,
						  std::regex("(YES|NO)"));
  i.wait();
  _handle_remarks(id_tok, i.get_warnings(), i.get_errors(), remarks);

  if (i.get_matched_result()[1].str() == "YES")
    return true;

  assert(i.get_matched_result()[1].str() == "NO");
  return false;
}

bool lp_creation_policy_user_commands::
shall_externalize_object(const ast::init_declarator &id,
			 code_remarks &remarks) const
{
  std::vector<std::string> args;
  args.reserve(1);

  const pp_token_index id_tok = _get_id_tok(id);
  args.push_back(_pp_result.get_pp_tokens()[id_tok].get_value());

  auto i = _pol_cmd_shall_externalize_obj.execute(std::move(args), _envp,
						  std::regex("(YES|NO)"));
  i.wait();
  _handle_remarks(id_tok, i.get_warnings(), i.get_errors(), remarks);

  if (i.get_matched_result()[1].str() == "YES")
    return true;

  assert(i.get_matched_result()[1].str() == "NO");
  return false;
}

lp_creation_policy::externalized_symbol_modification
lp_creation_policy_user_commands::
get_sym_mod_for_externalized_fun(const ast::direct_declarator_id &ddid,
				 const allocated_ids_type &allocated_ids,
				 code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(ddid);
  return _get_sym_mod_for_externalized(id_tok, "function", allocated_ids,
				       remarks);
}

lp_creation_policy::symbol_modification lp_creation_policy_user_commands::
get_sym_mod_for_patched_fun(const ast::function_definition &fd,
			    const allocated_ids_type &allocated_ids,
			    code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(fd);

  std::vector<std::string> warnings;
  for (unsigned int j = 0; j < _MAX_RENAME_RETRIES; ++j) {
    std::vector<std::string> args;
    args.reserve(2);
    args.push_back(_pp_result.get_pp_tokens()[id_tok].get_value());
    args.push_back(std::to_string(j));

    auto i = (_pol_cmd_mod_patched_fun_sym.execute
	      (std::move(args), _envp,
	       std::regex(std::string("([a-zA-Z_]+[a-zA-Z_0-9]*)") +
			  "[[:blank:]]+" +
			  "(KEEP-LINKAGE|MAKE-INTERNAL|MAKE-EXTERNAL)")));
    i.wait();

    for (const auto &w : i.get_warnings()) {
      if (std::find(warnings.cbegin(), warnings.cend(), w) == warnings.cend())
	warnings.push_back(w);
      if (!i.get_errors().empty())
	_handle_remarks(id_tok, warnings, i.get_errors(), remarks);
    }

    const std::smatch &m = i.get_matched_result();

    std::string cmd_result_new_name = m[1].str();
    if (cmd_result_new_name == _pp_result.get_pp_tokens()[id_tok].get_value())
      cmd_result_new_name.clear();

    if (cmd_result_new_name.empty() ||
	!allocated_ids.count(cmd_result_new_name)) {
      _handle_remarks(id_tok, warnings, std::vector<std::string>{}, remarks);

      symbol_modification::linkage_change lc;
      const std::string cmd_result_lc = m[2].str();
      if (cmd_result_lc == "KEEP-LINKAGE")
	lc = symbol_modification::linkage_change::lc_none;
      else if (cmd_result_lc == "MAKE-INTERNAL")
	lc = symbol_modification::linkage_change::lc_make_static;
      else if (cmd_result_lc == "MAKE-EXTERNAL")
	lc = symbol_modification::linkage_change::lc_make_extern;

      return lp_creation_policy::symbol_modification{
		std::move(cmd_result_new_name), lc
	     };
    }
  }

  _handle_remarks(id_tok, warnings, std::vector<std::string>{}, remarks);

  code_remark remark{
	code_remark::severity::fatal,
	("exceeded maximum retries for renaming patched \"" +
	 _pp_result.get_pp_tokens()[id_tok].get_value() + "\""),
	_pp_result,
	id_tok
  };
  remarks.add(remark);
  throw lp_except(std::move(remark));
}

std::string lp_creation_policy_user_commands::
rename_rewritten_closure_fun(const ast::function_definition &fd,
			     const allocated_ids_type &allocated_ids,
			     code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(fd);

  std::vector<std::string> warnings;
  for (unsigned int j = 0; j < _MAX_RENAME_RETRIES; ++j) {
    std::vector<std::string> args;
    args.reserve(2);
    args.push_back(_pp_result.get_pp_tokens()[id_tok].get_value());
    args.push_back(std::to_string(j));

    auto i = (_pol_cmd_rename_rewritten_fun.execute
	      (std::move(args), _envp,
	       std::regex(std::string("([a-zA-Z_]+[a-zA-Z_0-9]*)"))));
    i.wait();

    for (const auto &w : i.get_warnings()) {
      if (std::find(warnings.cbegin(), warnings.cend(), w) == warnings.cend())
	warnings.push_back(w);
      if (!i.get_errors().empty())
	_handle_remarks(id_tok, warnings, i.get_errors(), remarks);
    }

    const std::smatch &m = i.get_matched_result();

    std::string cmd_result_new_name = m[1].str();
    if (cmd_result_new_name == _pp_result.get_pp_tokens()[id_tok].get_value())
      cmd_result_new_name.clear();

    if (cmd_result_new_name.empty() ||
	!allocated_ids.count(cmd_result_new_name)) {
      _handle_remarks(id_tok, warnings, std::vector<std::string>{}, remarks);

      return cmd_result_new_name;
    }
  }

  _handle_remarks(id_tok, warnings, std::vector<std::string>{}, remarks);

  code_remark remark{
	code_remark::severity::fatal,
	("exceeded maximum retries for renaming rewritten \"" +
	 _pp_result.get_pp_tokens()[id_tok].get_value() + "\""),
	_pp_result,
	id_tok
  };
  remarks.add(remark);
  throw lp_except(std::move(remark));
}

lp_creation_policy::externalized_symbol_modification
lp_creation_policy_user_commands::
get_sym_mod_for_externalized_obj(const ast::init_declarator &id,
				 const allocated_ids_type &allocated_ids,
				 code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(id);
  return _get_sym_mod_for_externalized(id_tok, "object", allocated_ids,
				       remarks);
}

lp_creation_policy::externalized_symbol_modification
lp_creation_policy_user_commands::
_get_sym_mod_for_externalized(const pp_token_index id_tok,
			      const char *what,
			      const allocated_ids_type &allocated_ids,
			      code_remarks &remarks) const
{
  std::vector<std::string> warnings;
  for (unsigned int j = 0; j < _MAX_RENAME_RETRIES; ++j) {
    std::vector<std::string> args;
    args.reserve(3);
    args.push_back(what);
    args.push_back(_pp_result.get_pp_tokens()[id_tok].get_value());
    args.push_back(std::to_string(j));

    auto i = (_pol_cmd_mod_externalized_sym.execute
	      (std::move(args), _envp,
	       std::regex(std::string("([a-zA-Z_]+[a-zA-Z_0-9]*)") +
			  "[[:blank:]]+" +
			  "(KEEP-LINKAGE|MAKE-INTERNAL|MAKE-EXTERNAL)" +
			  "([[:blank:]]+(MAKE-POINTER))?")));
    i.wait();

    for (const auto &w : i.get_warnings()) {
      if (std::find(warnings.cbegin(), warnings.cend(), w) == warnings.cend())
	warnings.push_back(w);
      if (!i.get_errors().empty())
	_handle_remarks(id_tok, warnings, i.get_errors(), remarks);
    }

    const std::smatch &m = i.get_matched_result();

    std::string cmd_result_new_name = m[1].str();
    if (cmd_result_new_name == _pp_result.get_pp_tokens()[id_tok].get_value())
      cmd_result_new_name.clear();

    if (cmd_result_new_name.empty() ||
	!allocated_ids.count(cmd_result_new_name)) {
      _handle_remarks(id_tok, warnings, std::vector<std::string>{}, remarks);

      symbol_modification::linkage_change lc;
      const std::string cmd_result_lc = m[2].str();
      if (cmd_result_lc == "KEEP-LINKAGE")
	lc = symbol_modification::linkage_change::lc_none;
      else if (cmd_result_lc == "MAKE-INTERNAL")
	lc = symbol_modification::linkage_change::lc_make_static;
      else if (cmd_result_lc == "MAKE-EXTERNAL")
	lc = symbol_modification::linkage_change::lc_make_extern;

      bool make_pointer = false;
      if (m[4].matched)
	make_pointer = true;

      return lp_creation_policy::externalized_symbol_modification{
		std::move(cmd_result_new_name), lc, make_pointer
	     };
    }
  }

  _handle_remarks(id_tok, warnings, std::vector<std::string>{}, remarks);

  code_remark remark{
	code_remark::severity::fatal,
	("exceeded maximum retries for renaming externalized \"" +
	 _pp_result.get_pp_tokens()[id_tok].get_value() + "\""),
	_pp_result,
	id_tok
  };
  remarks.add(remark);
  throw lp_except(std::move(remark));
}

pp_token_index lp_creation_policy_user_commands::
_get_id_tok(const ast::init_declarator &id) noexcept
{
  return _get_id_tok(id.get_declarator().get_direct_declarator_id());
}

pp_token_index lp_creation_policy_user_commands::
_get_id_tok(const ast::function_definition &fd) noexcept
{
  return _get_id_tok(fd.get_declarator().get_direct_declarator_id());
}

pp_token_index lp_creation_policy_user_commands::
_get_id_tok(const ast::direct_declarator_id &ddid) noexcept
{
  return ddid.get_id_tok();
}

void lp_creation_policy_user_commands::
_handle_remarks(const pp_token_index id_tok,
		const std::vector<std::string> &warnings,
		const std::vector<std::string> &errors,
		code_remarks &remarks) const
{
  for (const auto &w : warnings) {
    code_remark remark{code_remark::severity::warning, w, _pp_result, id_tok};
    remarks.add(std::move(remark));
  }

  if (!errors.empty()) {
    for (const auto &e : errors) {
      code_remark remark{code_remark::severity::fatal, e, _pp_result, id_tok};
      remarks.add(remark);
    }

    code_remark remark{
      code_remark::severity::fatal, errors.front(), _pp_result, id_tok
    };
    throw lp_except(std::move(remark));
  }
}
