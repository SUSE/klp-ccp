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

#include "builtins.hh"
#include "ast.hh"
#include "ast_impl.hh"
#include "semantic_except.hh"
#include <map>

using namespace klp::ccp;
using namespace klp::ccp::ast;
using namespace klp::ccp::types;

builtin_func::~builtin_func() noexcept = default;


namespace
{
  class builtin_var__func__ final : public builtin_var
  {
  public:
    virtual ~builtin_var__func__() noexcept override;

    virtual evaluation_result_type
    evaluate(klp::ccp::ast::ast &a, const target &tgt,
	     const expr_id &eid) const override;

    static std::unique_ptr<builtin_var> create();
  };
}

builtin_var__func__::~builtin_var__func__() noexcept = default;

builtin_var::evaluation_result_type
builtin_var__func__::evaluate(klp::ccp::ast::ast &a, const target &tgt,
			      const expr_id &eid) const
{
  const function_definition *fd = nullptr;
  for (const _ast_entity *p = eid.get_parent(); p && !fd; p = p->get_parent()) {
    p->process<void, type_set<function_definition>>
      (wrap_callables<default_action_nop>
       ([&](const function_definition &_fd) {
	  fd = &_fd;
	}));
  }

  if (!fd) {
    code_remark remark(code_remark::severity::fatal,
		       ("usage of \"" +
			a.get_pp_tokens()[eid.get_id_tok()].get_value() +
			"\" outside of function body"),
		       a.get_pp_result(), eid.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const direct_declarator_id &ddid =
    fd->get_declarator().get_direct_declarator_id();

  const std::string &name = a.get_pp_tokens()[ddid.get_id_tok()].get_value();
  return evaluation_result_type{
		(plain_char_type::create(qualifiers{qualifier::q_const})
		 ->derive_array(mpa::limbs::from_size_type(name.size() + 1))),
		true
	 };
}

std::unique_ptr<builtin_var> klp::ccp::builtin_var__func__create()
{
  return std::unique_ptr<builtin_var>{new builtin_var__func__{}};
}


constexpr builtin_var::builtin_var() noexcept = default;

builtin_var::~builtin_var() noexcept = default;

builtin_var::factory builtin_var::lookup(const std::string &id)
{
  static std::map<const std::string,
		  const builtin_var::factory > m = {
    { "__func__", builtin_var__func__create },
    { "__FUNCTION__", builtin_var__func__create },
    { "__PRETTY_FUNCTION__", builtin_var__func__create },
  };

  auto it = m.find(id);
  if (it == m.end())
    return nullptr;

  return it->second;
}


builtin_typedef::factory::factory(const std::string &_name,
				  const create_type &_create)
  : name(_name), create(_create)
{}

builtin_typedef::~builtin_typedef() noexcept = default;
