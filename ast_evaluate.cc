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

#include <cassert>
#include <algorithm>
#include <stack>
#include <stdexcept>
#include "ast_impl.hh"
#include "semantic_except.hh"
#include "types_impl.hh"
#include "callables_wrapper.hh"
#include "type_set.hh"
#include "target.hh"
#include "mp_arithmetic.hh"
#include "target_float.hh"
#include "execution_charset_encoder.hh"
#include "ast_evaluate.hh"
#include "pp_token.hh"

using namespace klp::ccp;
using namespace klp::ccp::ast;
using namespace klp::ccp::types;

using constness = constexpr_value::constness;


static void _complete_type_from_init(klp::ccp::ast::ast &a,
				     const target &tgt,
				     init_declarator &i);

static void
_check_type_completeness_local(klp::ccp::ast::ast &a,
			       const klp::ccp::ast::init_declarator &i);

namespace
{
  class _evaluator
  {
  public:
    _evaluator(klp::ccp::ast::ast &ast,
	       klp::ccp::ast::_ast_entity &ae,
	       const target &tgt) noexcept;

    void operator()();

    static std::function<void(klp::ccp::ast::expr&)>
    make_expr_evaluator(klp::ccp::ast::ast &a,
			const target &tgt) noexcept;

  private:
    void _handle_sou_member(struct_declarator &sd);
    void _handle_sou_member(unnamed_struct_or_union &unnamed_sou);

    void _check_return_stmt(const klp::ccp::ast::stmt_return &ret_stmt) const;

    void
    _check_function_definition(const klp::ccp::ast::function_definition &fd)
      const;

    static void _evaluate_expr(klp::ccp::ast::ast &a, const target &tgt,
			       klp::ccp::ast::expr &e);

    klp::ccp::ast::ast &_ast;
    klp::ccp::ast::_ast_entity &_ae;
    const target &_tgt;
    std::stack<std::unique_ptr<target::sou_layouter>> _sou_layouters;
  };
}

_evaluator::_evaluator(klp::ccp::ast::ast &ast,
		       klp::ccp::ast::_ast_entity &ae,
		       const target &tgt) noexcept
  : _ast(ast), _ae(ae), _tgt(tgt)
{}

void _evaluator::operator()()
{
  auto &&pre =
    (wrap_callables<default_action_return_value<bool, false>::type>
     ([this](abstract_declarator &ad) {
       if (ad.is_evaluated())
	  return false;
	ad.evaluate_type(_ast, _tgt);
	return false;
      },
      [this](direct_abstract_declarator &dad) {
	if (dad.is_evaluated())
	  return false;
	dad.evaluate_type(_ast, _tgt);
	return false;
      },
      [this](declarator &d) {
	if (d.is_evaluated())
	  return false;
	d.evaluate_type(_ast, _tgt);
	return false;
      },
      [this](direct_declarator &dd) {
	if (dd.is_evaluated())
	  return false;
	dd.evaluate_type(_ast, _tgt);
	return false;
      },
      [](enumerator&) {
	return true;
      },
      [](const struct_declarator&) {
	return true;
      },
      [this](unnamed_struct_or_union &unnamed_sou) {
	_sou_layouters.push((_tgt.create_sou_layouter
			     (unnamed_sou.get_tag_kind(),
			      unnamed_sou.get_asl_before(),
			      unnamed_sou.get_asl_after(),
			      _ast,
			      make_expr_evaluator(_ast, _tgt))));
	return true;
      },
      [](struct_declaration_list &sdl) {
	// A struct's last member may be of array type of unspecified
	// length. Mark the last member so that the type evaluation
	// code can handle this.
	sdl.mark_last_member();
	return false;
      },
      [this](struct_or_union_def &soud) {
	_sou_layouters.push((_tgt.create_sou_layouter
			     (soud.get_tag_kind(),
			      soud.get_asl_before(),
			      soud.get_asl_after(),
			      _ast,
			      make_expr_evaluator(_ast, _tgt))));
	return true;
      },
      [](init_declarator&) {
	return true;
      },
      [](parameter_declaration_abstract&) {
	return true;
      },
      [](const function_definition&) {
	return true;
      },
      [](const static_assertion&) {
	return true;
      },
      [](const stmt_case&) {
	return true;
      },
      [](const stmt_case_range&) {
	return true;
      },
      [](const stmt_expr&) {
	return true;
      },
      [](const stmt_if&) {
	return true;
      },
      [](const stmt_switch&) {
	return true;
      },
      [](const stmt_while&) {
	return true;
      },
      [](const stmt_do&) {
	return true;
      },
      [](const stmt_for_init_expr&) {
	return true;
      },
      [](const stmt_for_init_decl&) {
	return true;
      },
      [](const stmt_for_init_static_assert&) {
	return true;
      },
      [](const stmt_goto&) {
	return true;
      },
      [](const stmt_return&) {
	return true;
      },
      [](const asm_operand&) {
	return true;
      },
      [](_typed &t) {
	if (t.is_evaluated())
	  return false;
	return true;
      }));

  auto &&post =
    (wrap_callables<default_action_nop>
     ([this](enumerator &e) {
	 expr * const value_expr = e.get_value_expr();

	 if (value_expr)
	   value_expr->apply_lvalue_conversion(false);

	 e.register_at_parent(_ast, _tgt);
      },
      [this](struct_declarator &sd) {
	expr * const width_expr = sd.get_width();

	if (width_expr)
	  width_expr->apply_lvalue_conversion(false);

	_handle_sou_member(sd);
      },
      [this](unnamed_struct_or_union &unnamed_sou) {
	assert(!_sou_layouters.empty());
	const auto souk = _sou_layouters.top()->get_tag_kind();
	auto &&c = _sou_layouters.top()->grab_result();
	_sou_layouters.pop();
	_tgt.apply_sou_attributes(_ast, make_expr_evaluator(_ast, _tgt),
				  souk,
				  unnamed_sou.get_asl_before(),
				  unnamed_sou.get_asl_after(),
				  *c);
	unnamed_sou.set_content(std::move(c));
	_handle_sou_member(unnamed_sou);
      },
      [&](struct_or_union_def &soud) {
	assert(!_sou_layouters.empty());
	// Don't evaluate twice, e.g. if the struct or union
	// definition is evaluated as part of a function parameter
	// list. Throw the reconstructed struct_or_union_content away
	// in this case.
	if (soud.get_content()) {
	  _sou_layouters.pop();
	  return;
	}

	const auto souk = _sou_layouters.top()->get_tag_kind();
	auto &&c = _sou_layouters.top()->grab_result();
	_sou_layouters.pop();
	_tgt.apply_sou_attributes(_ast, make_expr_evaluator(_ast, _tgt),
				  souk,
				  soud.get_asl_before(),
				  soud.get_asl_after(),
				  *c);
	soud.set_content(std::move(c));
      },
      [this](init_declarator &i) {
	_complete_type_from_init(_ast, _tgt, i);
	_check_type_completeness_local(_ast, i);
      },
      [this](parameter_declaration_abstract &pda) {
	pda.evaluate_type(_ast, _tgt);
      },
      [this](const function_definition &fd) {
	_check_function_definition(fd);
      },
      [&](const static_assertion &sa) {
	sa.test_assertion(_ast, _tgt);
      },
      [](stmt_case &s) {
	s.get_expr().apply_lvalue_conversion(false);
      },
      [](stmt_case_range &s) {
	s.get_expr_lo().apply_lvalue_conversion(false);
	s.get_expr_hi().apply_lvalue_conversion(false);
      },
      [](stmt_expr &s) {
	expr * const e = s.get_expr();

	if (e)
	  e->apply_lvalue_conversion(false);
      },
      [](stmt_if &s) {
	s.get_cond().apply_lvalue_conversion(false);
      },
      [](stmt_switch &s) {
	s.get_expr().apply_lvalue_conversion(false);
      },
      [](stmt_while &s) {
	s.get_cond().apply_lvalue_conversion(false);
      },
      [](stmt_do &s) {
	s.get_cond().apply_lvalue_conversion(false);
      },
      [](stmt_for_init_expr &s) {
	expr * const init = s.get_init_expr();
	expr * const cond = s.get_cond();
	expr * const next = s.get_next_expr();

	if (init)
	  init->apply_lvalue_conversion(false);

	if (cond)
	  cond->apply_lvalue_conversion(false);

	if (next)
	  next->apply_lvalue_conversion(false);
      },
      [](stmt_for_init_decl &s) {
	expr * const cond = s.get_cond();
	expr * const next = s.get_next_expr();

	if (cond)
	  cond->apply_lvalue_conversion(false);

	if (next)
	  next->apply_lvalue_conversion(false);
      },
      [](stmt_for_init_static_assert &s) {
	expr * const cond = s.get_cond();
	expr * const next = s.get_next_expr();

	if (cond)
	  cond->apply_lvalue_conversion(false);

	if (next)
	  next->apply_lvalue_conversion(false);
      },
      [](stmt_goto &s) {
	s.get_expr().apply_lvalue_conversion(false);
      },
      [this](stmt_return &s) {
	expr * const e = s.get_expr();

	if (e)
	  e->apply_lvalue_conversion(false);

	_check_return_stmt(s);
      },
      [](asm_operand &ao) {
	ao.get_expr().apply_lvalue_conversion(false);
      },
      [this](_typed &t) {
	t.evaluate_type(_ast, _tgt);
     }));

  _ae.for_each_dfs_pre_and_po<type_set<abstract_declarator,
				       direct_abstract_declarator,
				       declarator,
				       direct_declarator,
				       enumerator,
				       struct_declarator,
				       unnamed_struct_or_union,
				       struct_declaration_list,
				       struct_or_union_def,
				       init_declarator,
				       parameter_declaration_abstract,
				       function_definition,
				       static_assertion,
				       stmt_case, stmt_case_range,
				       stmt_expr, stmt_if, stmt_switch,
				       stmt_while, stmt_do,
				       stmt_for_init_expr,
				       stmt_for_init_decl,
				       stmt_for_init_static_assert,
				       stmt_goto, stmt_return,
				       asm_operand,
				       _typed>,
			      type_set<enumerator,
				       struct_declarator,
				       unnamed_struct_or_union,
				       struct_or_union_def,
				       init_declarator,
				       parameter_declaration_abstract,
				       function_definition,
				       static_assertion,
				       stmt_case, stmt_case_range,
				       stmt_expr, stmt_if, stmt_switch,
				       stmt_while, stmt_do,
				       stmt_for_init_expr,
				       stmt_for_init_decl,
				       stmt_for_init_static_assert,
				       stmt_goto, stmt_return,
				       asm_operand,
				       _typed> >
    (std::move(pre), std::move(post));
}

std::function<void(klp::ccp::ast::expr&)>
_evaluator::make_expr_evaluator(klp::ccp::ast::ast &a,
				const target &tgt) noexcept
{
  return std::bind(_evaluate_expr,
		   std::ref(a), std::cref(tgt),
		   std::placeholders::_1);
}

void _evaluator::_handle_sou_member(struct_declarator &sd)
{
  assert(!_sou_layouters.empty());
  target::sou_layouter &l = *_sou_layouters.top();

  std::string id;
  const declarator * const d = sd.get_declarator();
  if (d) {
    const pp_token &id_tok = _ast.get_pp_tokens()[d->get_id_tok()];
    id = id_tok.get_value();
    if (id.length() && l.lookup(id)) {
      code_remark remark(code_remark::severity::fatal,
			 "member already declared",
			 _ast.get_pp_result(), d->get_id_tok());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    }
  }

  struct_declaration_c99 &enclosing_decl =
    (sd.get_unique_parent<struct_declarator_list>()
     .get_unique_parent<struct_declaration_c99>());
  specifier_qualifier_list * const sql =
    enclosing_decl.get_specifier_qualifier_list();
  assert(sql);

  std::shared_ptr<const addressable_type> t;
  if (d)
    t = d->get_innermost_type();
  else
    t = sql->get_type();

  // Find the attributes associated with the containing struct/union
  // definition as a whole.
  attribute_specifier_list *soud_asl_before = nullptr;
  attribute_specifier_list *soud_asl_after = nullptr;
  struct_declaration_list &enclosing_decl_list
    = enclosing_decl.get_unique_parent<struct_declaration_list>();
  enclosing_decl_list.process_parent
    (wrap_callables<no_default_action>
     ([&](struct_or_union_def &soud) {
       soud_asl_before = soud.get_asl_before();
       soud_asl_after = soud.get_asl_after();
      },
      [&](unnamed_struct_or_union &unnamed_sou) {
       soud_asl_before = unnamed_sou.get_asl_before();
       soud_asl_after = unnamed_sou.get_asl_after();
      }));

  if (!sd.get_width()) {
    // Not a bitfield.
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](std::shared_ptr<const object_type> &&ot) {
	  if (!ot->is_complete()) {
	    // A struct's last member is allowed to be an
	    // incomplete array
	    if (!(l.get_tag_kind() == struct_or_union_kind::souk_struct &&
		  sd.is_last() &&
		  is_type<array_type>(*ot))) {
	      code_remark remark(code_remark::severity::fatal,
				 "incomplete type for member",
				 _ast.get_pp_result(),
				 sd.get_tokens_range());
	      _ast.get_remarks().add(remark);
	      throw semantic_except(remark);
	    }
	  }
	  l.add_member(std::move(id), std::move(ot),
		       *sql, sd.get_asl_before(), sd.get_asl_after());
	 },
	 [&](const std::shared_ptr<const type>&) {
	   code_remark remark(code_remark::severity::fatal,
			      "invalid type for member",
			      _ast.get_pp_result(),
			      sd.get_tokens_range());
	   _ast.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
       std::move(t));
    return;
  }

  // Bitfield
  std::shared_ptr<const int_type> it
    = (handle_types<std::shared_ptr<const int_type>>
       ((wrap_callables<no_default_action>
	 ([&](const std::shared_ptr<const enum_type> &et) {
	    // Bitfield's base type is an enum type specifier.
	    if (!et->is_complete()) {
	      code_remark remark
		(code_remark::severity::fatal,
		 "bitfield base type is an incomplete enum type",
		 _ast.get_pp_result(), sd.get_tokens_range());
	      _ast.get_remarks().add(remark);
	      throw semantic_except(remark);
	    }
	    return et->get_underlying_type();
	  },
	  [&](std::shared_ptr<const int_type> &&_it) {
	    if (_it->is_signed(_tgt) && _tgt.is_bitfield_default_signed()) {
	      if (!sql->is_signed_explicit()) {
		// Whether or not a bitfield without an explicit
		// 'signed' specifier is actually signed or not is
		// implementation specific, i.e. determined by the
		// target/ABI. In the evaluation of the specifier
		// qualifier list, we chose the wrong default. Correct
		// this.
		return _it->to_unsigned();
	      }
	    }
	    return std::move(_it);
	  },
	  [&](const std::shared_ptr<const type>&)
		-> std::shared_ptr<const int_type> {
	   code_remark remark(code_remark::severity::fatal,
			      "invalid bitfield base type",
			      _ast.get_pp_result(), sd.get_tokens_range());
	   _ast.get_remarks().add(remark);
	    throw semantic_except(remark);
	  })),
	std::move(t)));

  const expr &width = *sd.get_width();
  assert(width.is_evaluated());

  if (!width.is_constexpr()) {
    code_remark remark(code_remark::severity::fatal,
		       "bit-field width is not a constant expression",
		       _ast.get_pp_result(), width.get_tokens_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const constexpr_value &cv = width.get_constexpr_value();
  if (!cv.has_constness(constness::c_integer_constant_expr)) {
    code_remark remark
      (code_remark::severity::fatal,
       "bit-field width is not an integer constant expression",
       _ast.get_pp_result(), width.get_tokens_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  assert(cv.get_value_kind() == constexpr_value::value_kind::vk_int);
  const target_int &ti_width = cv.get_int_value();
  if (ti_width.is_negative()) {
    code_remark remark(code_remark::severity::fatal,
		       "negative bit-field width",
		       _ast.get_pp_result(), width.get_tokens_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const mpa::limbs &ls_width = ti_width.get_limbs();
  mpa::limbs::size_type w;
  if (!ls_width.fits_into_type<mpa::limbs::size_type>() ||
      ((w = ls_width.to_type<mpa::limbs::size_type>()) >
       it->get_width(_tgt))) {
    code_remark remark(code_remark::severity::fatal,
		       "bit-field width exceeds underlying type's width",
		       _ast.get_pp_result(), width.get_tokens_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (d && !w) {
    // 6.7.2.1(3)
    code_remark remark(code_remark::severity::fatal,
		       "zero width bit-field shall not have a declarator",
		       _ast.get_pp_result(), width.get_tokens_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  std::shared_ptr<const bitfield_type> bft =
    bitfield_type::create(std::move(it), w);
  l.add_member(std::move(id), std::move(bft),
	       *sql, sd.get_asl_before(), sd.get_asl_after());
}

void _evaluator::_handle_sou_member(unnamed_struct_or_union &unnamed_sou)
{
  assert(!_sou_layouters.empty());
  target::sou_layouter &l = *_sou_layouters.top();

  unnamed_sou.get_content().for_each_member_flat
    ([&](const struct_or_union_content::member &m) {
      if (m.get_name().length() && l.lookup(m.get_name())) {
	code_remark remark(code_remark::severity::fatal,
			   "unnamed struct/union redeclares member",
			   _ast.get_pp_result(),
			   unnamed_sou.get_tokens_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }
    });

  auto t = struct_or_union_type::create(unnamed_sou.get_tag_kind(),
					unnamed_sou.get_content());
  const struct_declaration_unnamed_sou &p =
    unnamed_sou.get_unique_parent<struct_declaration_unnamed_sou>();
  const specifier_qualifier_list * const sql = p.get_specifier_qualifier_list();
  if (sql) {
    const qualifiers &qs = sql->get_qualifiers();
    t = (std::dynamic_pointer_cast<const struct_or_union_type>
	 (t->amend_qualifiers(qs)));
  }
  l.add_member(std::move(t));
}

void _evaluator::_check_return_stmt(const klp::ccp::ast::stmt_return &ret_stmt)
  const
{
  const function_definition *fd = nullptr;

  for (const _ast_entity *p = ret_stmt.get_parent(); p;
       p && (p = p->get_parent())) {
    p->process<void, type_set<function_definition>>
      (wrap_callables<default_action_nop>
       ([&](const function_definition &_fd) {
	  fd = &_fd;
	  p = nullptr;
	}));
  }

  if (!fd) {
    code_remark remark(code_remark::severity::fatal,
		       "return statement outside of function definition",
		       _ast.get_pp_result(), ret_stmt.get_tokens_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const function_type * const ft =
    (handle_types<const function_type *>
     ((wrap_callables<default_action_unreachable<const function_type*,
						 type_set<> >::type>
       ([&](const function_type &_ft) -> const function_type* {
	  return &_ft;
	},
	[&](const type&) -> const function_type* {
	  // Ok, this function definition has a declarator which does
	  // not yield function type. This will be reported when the
	  // function_definition as a whole gets checked, i.e.
	  // from _check_function_definition().
	  return nullptr;
	})),
      *fd->get_declarator().get_innermost_type()));
  if (!ft)
    return;

  const std::shared_ptr<const returnable_type> &ret_type
    = ft->get_return_type();

  const expr * const ret_e = ret_stmt.get_expr();
  const bool ret_type_is_void = is_type<void_type>(*ret_type);
  if (ret_e) {
    if (!ret_type_is_void) {
      check_types_assignment(_ast, _tgt, *ret_type, *ret_e, false);
    } else if (!is_type<void_type>(*ret_e->get_type())) {
      // GCC still accepts this.
      code_remark remark(code_remark::severity::warning,
			 "return with value in function returning void",
			 _ast.get_pp_result(), ret_stmt.get_tokens_range());
      _ast.get_remarks().add(remark);
    }
  } else {
    if (!ret_type_is_void) {
      // GCC still accepts this.
      code_remark remark
	(code_remark::severity::warning,
	 "return without value in function returning non-void",
	 _ast.get_pp_result(), ret_stmt.get_tokens_range());
      _ast.get_remarks().add(remark);
    }
  }
}

void _evaluator::
_check_function_definition(const klp::ccp::ast::function_definition &fd) const
{
  const declarator &d = fd.get_declarator();
  const direct_declarator_id &ddid = d.get_direct_declarator_id();

  const function_type &ft =
    (handle_types<const function_type&>
     ((wrap_callables<default_action_unreachable<const function_type&,
       type_set<> >::type>
       ([&](const function_type &_ft) -> const function_type& {
	  return _ft;
	},
	 [&](const type&) -> const function_type& {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "non-function type declarator at function definition",
	      _ast.get_pp_result(), fd.get_declarator().get_tokens_range());
	   _ast.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
      *ddid.get_type()));

  const std::shared_ptr<const returnable_type> &ret_type
    = ft.get_return_type();

  if (!ret_type->is_complete() && !is_type<void_type>(*ret_type)) {
    code_remark remark
      (code_remark::severity::warning,
       "function definition's return value has incomplete type",
       _ast.get_pp_result(), fd.get_declarator().get_tokens_range());
    _ast.get_remarks().add(remark);
  }

  const direct_declarator_func &ddf = *ddid.is_function();
  const parameter_declaration_list * const pdl =
    ddf.get_parameter_declaration_list();
  if (!pdl)
    return;

  if (pdl->is_single_void(_ast))
    return;

  pdl->for_each
    ([&](const parameter_declaration &pd) {
       if (!pd.get_type()->is_complete()) {
	 code_remark remark
	   (code_remark::severity::warning,
	    "parameter in function definition has incomplete type",
	    _ast.get_pp_result(), pd.get_tokens_range());
	 _ast.get_remarks().add(remark);
       }
     });
}

void _evaluator::_evaluate_expr(klp::ccp::ast::ast &a, const target &tgt,
				klp::ccp::ast::expr &e)
{
  _evaluator ev{a, e, tgt};
  ev();
}


static bool _is_string_literal_expr(const expr &e)
{
  if (!e.is_dereferenced_const_addr())
    return false;

  const constexpr_value::address_constant &ac
    = e.get_constexpr_value().get_address_value();

  return (ac.get_base_kind() ==
	  constexpr_value::address_constant::base_kind::bk_string_literal);
}

static bool _is_compound_literal_expr(const expr &e)
{
  // This check does not work if the compound literal is the
  // result of a compile-time constant __builtin_choose_expr().
  // However, GCC can't handle this case either.
  if (e.skip_parens_down().is_any_of<expr_compound_literal>())
    return true;

  // This fails for compound literals in function context because they
  // don't have static storage duration.
  if (!e.is_dereferenced_const_addr())
    return false;

  const constexpr_value::address_constant &ac
    = e.get_constexpr_value().get_address_value();

  return (ac.get_base_kind() ==
	  constexpr_value::address_constant::base_kind::bk_compound_literal);
}

static std::shared_ptr<const array_type>
_evaluate_init(klp::ccp::ast::ast &a, const target &tgt,
	       const type &t_target, initializer_expr &ie);

static std::shared_ptr<const array_type>
_evaluate_init(klp::ccp::ast::ast &a, const target &tgt,
	       const type &t_target, initializer_list * const il);

namespace
{
  class _initializer_list_evaluator
  {
  public:
    _initializer_list_evaluator(klp::ccp::ast::ast &a,
				const target &tgt,
				const array_type &at,
				initializer_list &il);

    _initializer_list_evaluator(klp::ccp::ast::ast &a,
				const target &tgt,
				const struct_or_union_type &sout,
				initializer_list &il);

    void operator()();

    mpa::limbs grab_array_length();

  private:
    const type& _get_current_target_type() const noexcept;

    void _update_max_array_index();

    void _set_cursor(const designator_list &dl);
    void _advance_cursor();
    bool _descend_cursor(const expr &e_init);


    klp::ccp::ast::ast &_a;
    const target &_tgt;
    const array_type * const _at_start;
    const struct_or_union_type * const _sout_start;
    initializer_list &_il;

    struct _cur_obj
    {
      _cur_obj(const array_type &_at, mpa::limbs &&_index);
      _cur_obj(const struct_or_union_type &_sout,
	       const struct_or_union_content::const_member_iterator &it_m);

      const array_type *at;
      mpa::limbs cur_index;

      const struct_or_union_type *sout;
      struct_or_union_content::const_member_iterator cur_member;
    };

    std::stack<_cur_obj> _cursor;
    mpa::limbs _max_array_index;
  };
}

_initializer_list_evaluator::
_initializer_list_evaluator(klp::ccp::ast::ast &a, const target &tgt,
			    const array_type &at, initializer_list &il)
  : _a(a), _tgt(tgt), _at_start(&at), _sout_start(nullptr), _il(il)
{}

_initializer_list_evaluator::
_initializer_list_evaluator(klp::ccp::ast::ast &a, const target &tgt,
			    const struct_or_union_type &sout,
			    initializer_list &il)
  : _a(a), _tgt(tgt), _at_start(nullptr), _sout_start(&sout), _il(il)
{}

void _initializer_list_evaluator::operator()()
{
  bool excess_elements = false;
  _il.for_each
    ([&](initializer &i) {
       if (i.get_designation()) {
	 excess_elements = false;
	 _set_cursor(i.get_designation()->get_designator_list());
	 assert(!_cursor.empty());
       } else {
	 if (excess_elements)
	   return;

	 _advance_cursor();
	 if (_cursor.empty()) {
	   code_remark remark(code_remark::severity::warning,
			      "excess elements in intializer",
			      _a.get_pp_result(), _il.get_tokens_range());
	   _a.get_remarks().add(remark);
	   excess_elements = true;
	   return;
	 }
       }

       i.process<void, type_set<initializer_init_list, initializer_expr> >
	 ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
	   ([&](initializer_init_list &iil) {
	      _evaluate_init(_a, _tgt, _get_current_target_type(),
			     iil.get_initializer_list());
	    },
	    [&](initializer_expr &ie) {
	      if (!_descend_cursor(ie.get_expr())) {
		excess_elements = true;
		return;
	      }
	      _evaluate_init(_a, _tgt, _get_current_target_type(), ie);
	    })));
     });
}

mpa::limbs _initializer_list_evaluator::grab_array_length()
{
  assert(_at_start);
  // If _update_max_array_index() hasn't ever been called,
  // the length is zero.
  if (_max_array_index.empty())
    return mpa::limbs::from_size_type(0);

  mpa::limbs l = std::move(_max_array_index);
  l = l + mpa::limbs::from_size_type(1);
  return l;
}


const type& _initializer_list_evaluator::_get_current_target_type()
  const noexcept
{
  assert(!_cursor.empty());
  if (_cursor.top().at) {
    return *_cursor.top().at->get_element_type();
  } else {
    assert(_cursor.top().sout);
    return *_cursor.top().cur_member->get_type().get();
  }
}

void _initializer_list_evaluator::_update_max_array_index()
{
  if (_at_start && _cursor.size() == 1) {
    if (_max_array_index.empty() ||
	_max_array_index < _cursor.top().cur_index) {
      assert(!_cursor.top().cur_index.empty());
      _max_array_index = _cursor.top().cur_index;
    }
  }
}

void _initializer_list_evaluator::_set_cursor(const designator_list &dl)
{
  _cursor = std::stack<_cur_obj>{};
  dl.for_each
    ([&](const designator &d) {
       d.process<void, type_set<designator_array, designator_member> >
	 ((wrap_callables<default_action_unreachable<void, type_set<>>::type>
	   ([&](const designator_member &dm) {
	      const struct_or_union_type *sout = nullptr;
	      if (_cursor.empty()) {
		sout = _sout_start;
	      } else {
		handle_types<void>
		  ((wrap_callables<default_action_nop>
		    ([&](const struct_or_union_type &_sout) {
		       sout = &_sout;
		     })),
		   _get_current_target_type());
	      }
	      if (!sout) {
		code_remark remark
		  (code_remark::severity::fatal,
		   "member designator in non-struct initialization",
		   _a.get_pp_result(), dm.get_tokens_range());
		_a.get_remarks().add(remark);
		throw semantic_except(remark);
	      }

	      const std::string &name =
		_a.get_pp_tokens()[dm.get_member_tok()].get_value();

	      const struct_or_union_content::lookup_result lr
		= sout->get_content()->lookup(name);
	      if (lr.empty()) {
		code_remark remark(code_remark::severity::fatal,
				   "member lookup failed",
				   _a.get_pp_result(),
				   dm.get_tokens_range());
		_a.get_remarks().add(remark);
		throw semantic_except(remark);
	      }

	      _cursor.emplace(*sout, lr.front());
	      for (auto it = lr.begin() + 1; it != lr.end(); ++it) {
		assert((*(it - 1))->is_unnamed_sou());
		_cursor.emplace(*(*(it - 1))->get_sou_type(), *it);
	      }

	    },
	    [&](const designator_array &da) {
	      const array_type *at = nullptr;
	      if (_cursor.empty()) {
		at = _at_start;
	      } else {
		handle_types<void>
		  ((wrap_callables<default_action_nop>
		    ([&](const array_type &_at) {
		       at = &_at;
		     })),
		   _get_current_target_type());
	      }
	      if (!at) {
		code_remark remark
		  (code_remark::severity::fatal,
		   "array index designator in non-array initialization",
		   _a.get_pp_result(), da.get_tokens_range());
		_a.get_remarks().add(remark);
		throw semantic_except(remark);
	      }

	      assert(at->get_element_type()->is_complete());
	      if (at->is_complete() && !at->is_size_constant()) {
		code_remark remark
		  (code_remark::severity::fatal,
		   "initialization of variable-length array",
		   _a.get_pp_result(), da.get_tokens_range());
		_a.get_remarks().add(remark);
		throw semantic_except(remark);
	      }

	      // Set the cursor to the last index as we're interested
	      // in the max.
	      const expr &e_index_last = da.get_index_last();
	      assert(e_index_last.is_evaluated());
	      if (!e_index_last.is_constexpr()) {
		code_remark remark
		  (code_remark::severity::fatal,
		   "non-constant array index designator in initialization",
		   _a.get_pp_result(), e_index_last.get_tokens_range());
		_a.get_remarks().add(remark);
		throw semantic_except(remark);
	      }

	      const constexpr_value &last_index_cv =
		e_index_last.get_constexpr_value();
	      if (!(last_index_cv.has_constness
		    (constexpr_value::constness::c_integer_constant_expr))) {
		code_remark remark
		  (code_remark::severity::fatal,
		   "array index designator is not an integer constant",
		   _a.get_pp_result(), e_index_last.get_tokens_range());
		_a.get_remarks().add(remark);
		throw semantic_except(remark);
	      }

	      assert(last_index_cv.get_value_kind() ==
		     constexpr_value::value_kind::vk_int);
	      const target_int &last_index_ti = last_index_cv.get_int_value();
	      if (last_index_ti.is_negative()) {
		code_remark remark
		  (code_remark::severity::fatal,
		   "negative array index designator in initializer",
		   _a.get_pp_result(), e_index_last.get_tokens_range());
		_a.get_remarks().add(remark);
		throw semantic_except(remark);
	      }

	      if (at->is_complete() &&
		  at->get_length() <= last_index_ti.get_limbs()) {
		code_remark remark
		  (code_remark::severity::fatal,
		   "array index designator exceeds array bounds",
		   _a.get_pp_result(), e_index_last.get_tokens_range());
		_a.get_remarks().add(remark);
		throw semantic_except(remark);
	      }

	      _cursor.emplace(*at, mpa::limbs{last_index_ti.get_limbs()});
	      _update_max_array_index();

	    })));
     });
}

void _initializer_list_evaluator::_advance_cursor()
{
  if (_cursor.empty()) {
    // First time _advance_cursor gets called, move the cursor
    // to the first subobject.
    if (_at_start) {
      if (_at_start->is_complete()) {
	assert(_at_start->is_size_constant());
	if (!static_cast<bool>(_at_start->get_length())) {
	  // Zero length array, leave _cursor empty.
	  return;
	}
      }

      _cursor.emplace(*_at_start, mpa::limbs::from_size_type(0));
      _update_max_array_index();

    } else {
      assert(_sout_start);
      // Move the member iterator to the first named member resp.
      // anonymous struct/union.
      const struct_or_union_content &souc = *_sout_start->get_content();
      for (auto it = souc.members_begin(); it != souc.members_end(); ++it) {
	if (it->is_unnamed_sou() || !it->get_name().empty()) {
	  _cursor.emplace(*_sout_start, it);
	  break;
	}
      }
    }

    return;
  }

  // Advance the cursor to the next, most outer subobject
  while (!_cursor.empty()) {
    _cur_obj &t = _cursor.top();
    if (t.at) {
      // Advance the array index, go up and retry to advance the
      // cursor if the end has been reached.
      t.cur_index = t.cur_index + mpa::limbs::from_size_type(1);
      if (t.at->is_complete() && t.at->get_length() == t.cur_index) {
	_cursor.pop();
	continue;
      }
      assert(!_cursor.empty());
      _update_max_array_index();
      return;

    } else {
      assert(t.sout);

      // Always break out of unions.
      if (t.sout->get_kind() == struct_or_union_kind::souk_union) {
	_cursor.pop();
	continue;
      }

      // For structs, search for the next member which is either named
      // or is an anonymous struct or union. When the end is reached,
      // go up and retry.
      const struct_or_union_content &souc = *t.sout->get_content();
      while (++t.cur_member != souc.members_end()) {
	if (t.cur_member->is_unnamed_sou() ||
	    !t.cur_member->get_name().empty()) {
	  return;
	}
      }
      _cursor.pop();
    }
  }
}

bool _initializer_list_evaluator::_descend_cursor(const expr &e_init)
{
  // Walk down the subobject hierarchy rooted at the current cursor until
  // either a struct, union or array compatible with the initializer
  // expression's type t has been found or the leaf has been reached.
  bool stop = false;
  bool found = false;
  assert(!_cursor.empty());
  while (!stop) {
    const type &next_t = _get_current_target_type();

    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const array_type &next_at) {
	   if (e_init.get_type()->is_compatible_with(_tgt, next_at, true)) {
	     stop = true;
	     found = true;
	     return;
	   }

	   if (_is_string_literal_expr(e_init) &&
	       is_type<int_type>(*next_at.get_element_type())) {
	     stop = true;
	     found = true;
	     return;
	   }

	   // Descend into the array element type
	   if (next_at.is_complete() && !next_at.get_length()) {
	     // Trying to initialize an array of zero length with some
	     // expression.
	     code_remark remark
	       (code_remark::severity::warning,
		"excess elements in initialization of zero length array",
		_a.get_pp_result(), e_init.get_tokens_range());
	     _a.get_remarks().add(remark);
	     stop = true;
	     found =  false;
	     return;
	   }

	   _cursor.emplace(next_at, mpa::limbs::from_size_type(0));

	 },
	 [&](const struct_or_union_type &next_sout) {
	   if (e_init.get_type()->is_compatible_with(_tgt, next_sout, true)) {
	     stop = true;
	     found = true;
	     return;
	   }

	   // Ok, descend into the struct or union. Find the first member which
	   // is either an anonymous struct/union or has a name.
	   assert(next_sout.is_complete());
	   const struct_or_union_content &souc = *next_sout.get_content();
	   auto it_m = souc.members_begin();
	   while (it_m != souc.members_end() &&
		  !it_m->is_unnamed_sou() && it_m->get_name().empty()) {
	     ++it_m;
	   }
	   if (it_m == souc.members_end()) {
	     // Now, that's bad, the struct/union subobject is essentially
	     // empty, yet there's an attempt to initialize it with an
	     // expression.
	     code_remark remark
	       (code_remark::severity::warning,
		"excess elements initialization of empty struct/union",
		_a.get_pp_result(), e_init.get_tokens_range());
	     _a.get_remarks().add(remark);
	     stop = true;
	     found = false;
	     return;
	   }

	   _cursor.emplace(next_sout, it_m);

	 },
	 [&](const type&) {
	   stop = true;
	   found = true;
	 })),
       next_t);
  }

  return found;
}

_initializer_list_evaluator::
_cur_obj::_cur_obj(const array_type &_at, mpa::limbs &&_index)
  :  at(&_at), cur_index(std::move(_index)), sout(nullptr)
{}

_initializer_list_evaluator::
_cur_obj::_cur_obj(const struct_or_union_type &_sout,
		   const struct_or_union_content::const_member_iterator &it_m)
  : at(nullptr), sout(&_sout), cur_member(it_m)
{}


static std::shared_ptr<const array_type>
_evaluate_array_init_from_string_literal (klp::ccp::ast::ast &a,
					  const target &tgt,
					  const array_type &at_target,
					  const expr &e)
{
  const array_type * const at_source
    = dynamic_cast<const array_type*>(e.get_type().get());
  assert(at_source);

  bool compatible = false;
  if (at_source->get_element_type()->is_compatible_with
      (tgt, *at_target.get_element_type(), true)) {
    compatible = true;
  } else {
    handle_types<void>
      ((wrap_callables<default_action_nop>
	([&](const std_int_type &it, const plain_char_type&) {
	   if (it.get_kind() != std_int_type::kind::k_char)
	     return;

	   compatible = true;
	 })),
       *at_target.get_element_type(), *at_source->get_element_type());
  }

  if (!compatible) {
    code_remark remark
      (code_remark::severity::fatal,
       "initialization of empty struct/union with expression",
       a.get_pp_result(), e.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  assert(at_source->is_size_constant());
  assert(!at_target.is_complete() || at_target.is_size_constant());
  if (at_target.is_complete()) {
    if (at_target.get_length() < at_source->get_length()) {
      code_remark remark
	(code_remark::severity::warning,
	 "excess characters in array initialization",
	 a.get_pp_result(), e.get_tokens_range());
      a.get_remarks().add(remark);
    }
  } else {
    return (at_target.set_length_from_initializer
	    (mpa::limbs{at_source->get_length()}));
  }
  return nullptr;
}

static std::shared_ptr<const array_type>
_evaluate_array_init(klp::ccp::ast::ast &a, const target &tgt,
		     const array_type &at_target,
		     initializer_expr &ie)
{
  // First, deal with the special case of the initializer expression
  // being a string literal.
  if (_is_string_literal_expr(ie.get_expr()) &&
      is_type<int_type>(*at_target.get_element_type())) {
    return _evaluate_array_init_from_string_literal(a, tgt, at_target,
						    ie.get_expr());
  }

  // The to be initialized array must not be a VLA.
  if (at_target.is_complete() && !at_target.is_size_constant()) {
    code_remark remark
      (code_remark::severity::fatal,
       "initialization of variable size array",
       a.get_pp_result(), ie.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  // The source array must be the result of a compound literal
  // expression.
  const expr &e = ie.get_expr();
  if (!_is_compound_literal_expr(e)) {
    code_remark remark
      (code_remark::severity::fatal,
       "initialization of array with non-compound literal",
       a.get_pp_result(), ie.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  // The initializer expression shall have compatible array type.
  std::shared_ptr<const array_type> at_source =
    handle_types<std::shared_ptr<const array_type> >
    ((wrap_callables<no_default_action>
      ([](const std::shared_ptr<const array_type> &at) {
	 return at;
       },
       [&](const std::shared_ptr<const type>&)
		-> std::shared_ptr<const array_type> {
	 code_remark remark
	   (code_remark::severity::fatal,
	    "initialization of array with non-array expression",
	    a.get_pp_result(), ie.get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
      })),
     e.get_type());

  assert(at_source->is_complete() && at_source->is_size_constant());

  if (!at_source->is_compatible_with(tgt, at_target, true)) {
    code_remark remark
      (code_remark::severity::fatal,
       "incompatible array types at initialization",
       a.get_pp_result(), ie.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (!at_target.is_complete()) {
    return (at_target.set_length_from_initializer
	    (mpa::limbs{at_source->get_length()}));
  }

  return nullptr;
}

static initializer_expr*
_try_unwrap_initializer_list(initializer_list &il)
{
  // If the initializer list consists of a single initializer_expr
  // w/o any associated designator, return that.
  if (il.get_size() != 1) {
    return nullptr;
  }

  initializer_expr *ie = nullptr;
  il.for_each
    ([&](initializer &i) {
       if (i.get_designation())
	 return;

       i.process<void, type_set<initializer_expr> >
	 ((wrap_callables<default_action_nop>
	   ([&](initializer_expr &_ie) {
	      ie = &_ie;
	    })));
     });

  return ie;
}

static std::shared_ptr<const array_type>
_evaluate_array_init(klp::ccp::ast::ast &a, const target &tgt,
		     const array_type &at_target,
		     initializer_list &il)
{
  // First check, if the array type is possibly an array of characters and
  // the initializer_list contains a single string literal.
  if (is_type<int_type>(*at_target.get_element_type())) {
    const initializer_expr * const unwrapped_ie
      = _try_unwrap_initializer_list(il);
    if (unwrapped_ie) {
      if (_is_string_literal_expr(unwrapped_ie->get_expr()))
	return (_evaluate_array_init_from_string_literal
		(a, tgt, at_target, unwrapped_ie->get_expr()));
      }
  }

  // The to be initialized array must not be a VLA.
  if (at_target.is_complete() && !at_target.is_size_constant()) {
    code_remark remark
      (code_remark::severity::fatal,
       "braced initialization of variable size array",
       a.get_pp_result(), il.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _initializer_list_evaluator ev(a, tgt, at_target, il);
  ev();
  if (!at_target.is_complete())
    return at_target.set_length_from_initializer(ev.grab_array_length());
  else
    return nullptr;
}

static void
_evaluate_sou_init(klp::ccp::ast::ast &a, const target &tgt,
		   const struct_or_union_type &sout_target,
		   initializer_list &il)
{
  if (!sout_target.is_complete()) {
    code_remark remark
      (code_remark::severity::fatal,
       "initialization of incomplete struct/union",
       a.get_pp_result(), il.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _initializer_list_evaluator ev(a, tgt, sout_target, il);
  ev();
}

static std::shared_ptr<const array_type>
_evaluate_init(klp::ccp::ast::ast &a, const target &tgt,
	       const type &t_target, initializer_expr &ie)
{
  return
    (handle_types<std::shared_ptr<const array_type>>
     ((wrap_callables<no_default_action>
       ([&](const array_type &at) {
	  return _evaluate_array_init(a, tgt, at, ie);
	},
	[&](const type &t) {
	  expr &e_source = ie.get_expr();
	  e_source.apply_lvalue_conversion(false);
	  check_types_assignment(a, tgt, t, e_source, false);
	  return nullptr;
	})),
      t_target));
}

static std::shared_ptr<const array_type>
_evaluate_init(klp::ccp::ast::ast &a, const target &tgt,
	       const type &t_target, initializer_list * const il)
{
  std::shared_ptr<const array_type> completed_at;

  handle_types<void>
    ((wrap_callables<no_default_action>
      ([&](const array_type &at_target) {
	 if (il) {
	   completed_at = _evaluate_array_init(a, tgt, at_target, *il);
	 } else {
	   // Empty initializer list. Complete the array to zero size,
	   // if needed.
	   if (at_target.is_complete())
	     return;
	   completed_at = (at_target.set_length_from_initializer
			   (mpa::limbs::from_size_type(0)));
	 }
       },
       [&](const struct_or_union_type &sout_target) {
	 if (il)
	   _evaluate_sou_init(a, tgt, sout_target, *il);
       },
       [&](const type &t) {
	 // Scalar initialization with single value wrapped in
	 // braces. Empty list means zero.
	 if (!il || !il->get_size())
	   return;

	 initializer_expr * const unwrapped_ie
	   = _try_unwrap_initializer_list(*il);
	 if (!unwrapped_ie) {
	   code_remark remark
	     (code_remark::severity::warning,
	      "invalid initialization of scalar type",
	      a.get_pp_result(), il->get_tokens_range());
	   a.get_remarks().add(remark);
	   return;
	 }
	 expr &e_source = unwrapped_ie->get_expr();
	 e_source.apply_lvalue_conversion(false);
	 check_types_assignment(a, tgt, t_target, e_source, false);
       })),
     t_target);

  return completed_at;
}

static std::shared_ptr<const array_type>
_evaluate_init(klp::ccp::ast::ast &a, const target &tgt,
	       const type &t_target, initializer &i)
{
  std::shared_ptr<const array_type> completed_at;

  i.process<void, type_set<initializer_expr, initializer_init_list> >
    (wrap_callables<default_action_unreachable<void, type_set<> >::type>
     ([&](initializer_expr &ie) {
	completed_at = _evaluate_init(a, tgt, t_target, ie);
      },
      [&](initializer_init_list &iil) {
	completed_at = _evaluate_init(a, tgt, t_target,
				      iil.get_initializer_list());
      }));

  return completed_at;
}

static void _complete_type_from_init(klp::ccp::ast::ast &a,
				     const target &tgt,
				     init_declarator &id)

{
  direct_declarator_id &ddid = id.get_declarator().get_direct_declarator_id();
  assert(ddid.is_evaluated() || id.get_initializer());

  if (!id.get_initializer())
    return;

  if (ddid.is_evaluated()) {
    // The declarator might declare an array of unknown size which
    // gets completed by its initializer.
    std::shared_ptr<const array_type> completed_at =
      _evaluate_init(a, tgt, *ddid.get_type(), *id.get_initializer());
    if (static_cast<bool>(completed_at)) {
      ddid.complete_type(std::move(completed_at));
    }
  } else {
    // Deduce the __auto_type from the initializer.
    assert(id.get_containing_declaration()
	   .get_declaration_specifiers().is_auto_type());
    ddid.deduce_auto_type(a, tgt, id);
  }
}


static
void _check_type_completeness_global(klp::ccp::ast::ast &a,
				     const klp::ccp::ast::declaration &d,
				     const klp::ccp::ast::init_declarator &i)
{
  const direct_declarator_id &ddid =
    i.get_declarator().get_direct_declarator_id();
  const addressable_type &t = *ddid.get_type();

  if (t.is_complete())
    return;
  assert(!ddid.is_function());

  const storage_class sc = d.get_declaration_specifiers().get_storage_class(a);
  if (sc == storage_class::sc_typedef)
    return;

  assert(d.is_at_file_scope());
  // Declaration is at file scope and we're asked not to ignore
  // incomplete types here.
  const linkage &l = i.get_linkage();
  if (!l.is_last_in_file_scope_chain()) {
    // The last declaration of this identifier will have its type to
    // be the composite type from all prior declarations.
    // We'll check type completeness on that one, if required.
    return;
  }

  // We'll need type completeness if any of the current identifier's
  // declarations are definitions, i.e. either have no or 'static'
  // storage class.
  bool need_completeness = false;
  if (sc == storage_class::sc_none || sc == storage_class::sc_static) {
    need_completeness = true;
  } else {
    l.for_each_visible
      (wrap_callables<default_action_unreachable<bool, type_set<>>::type>
       ([&](const init_declarator &prev_id) {
	  const declaration &prev_d = i.get_containing_declaration();
	  const storage_class prev_sc =
	    prev_d.get_declaration_specifiers().get_storage_class(a);
	  if (prev_sc == storage_class::sc_none ||
	      prev_sc == storage_class::sc_static) {
	    need_completeness = true;
	    return false;
	  }
	  return true;
	}));
  }

  if (!need_completeness)
    return;

  code_remark remark(code_remark::severity::warning,
		     "init declarator has incomplete type",
		     a.get_pp_result(), i.get_tokens_range());
  a.get_remarks().add(remark);
}

void
_check_type_completeness_local(klp::ccp::ast::ast &a,
			       const klp::ccp::ast::init_declarator &i)
{
  const direct_declarator_id &ddid =
    i.get_declarator().get_direct_declarator_id();
  const addressable_type &t = *ddid.get_type();

  if (t.is_complete())
    return;
  assert(!ddid.is_function());

  const declaration &d = i.get_containing_declaration();
  const storage_class sc = d.get_declaration_specifiers().get_storage_class(a);
  if (sc == storage_class::sc_typedef)
    return;

  // At file scope, GCC supports completion by later declarations:
  //  struct A foo; struct A { int a; };
  // This can only be checked after the whole translation unit has
  // been processed -- ignore it for now.
  if (d.is_at_file_scope())
    return;

  // Non-file scope extern declarations don't need to be complete.
  if (sc == storage_class::sc_extern)
    return;

  // Arrays w/o a length specifier are Ok for an old-style function
  // definition's parameter definition.
  if (is_type<array_type>(t) && d.get_parent()->is_any_of<declaration_list>())
    return;

  code_remark remark(code_remark::severity::warning,
		     "init declarator has incomplete type",
		     a.get_pp_result(), i.get_tokens_range());
  a.get_remarks().add(remark);
}


void ast_translation_unit::evaluate(const target &tgt)
{
  _evaluator ev{*this, *_tu, tgt};
  ev();

  // Finally, sweep over all global objects defined in this
  // translation unit and check that their types are complete.
  _tu->for_each_external_declaration
    (wrap_callables<default_action_nop>
     ([&](const external_declaration_decl &edd) {
	const declaration &d = edd.get_declaration();
	const init_declarator_list * const il = d.get_init_declarator_list();
	if (!il)
	  return;

	il->for_each
	  ([&](const init_declarator &id) {
	     _check_type_completeness_global(*this, d, id);
	   });
      }));
}


bool ast_pp_expr::evaluate(const target &tgt)
{
  _evaluator ev{*this, *_e, tgt};
  ev();

  if (!_e->is_constexpr() ||
      !(_e->get_constexpr_value().has_constness
	(constexpr_value::constness::c_integer_constant_expr))) {
    code_remark remark
      (code_remark::severity::fatal,
       "preprocesser conditional is not an integer constant expression",
       get_pp_result(), _e->get_tokens_range());
    get_remarks().add(remark);
    throw semantic_except(remark);
  }

  return !_e->get_constexpr_value().is_zero();
}


void klp::ccp::ast::specifier_qualifier_list::
evaluate_type(ast &a, const target &tgt)
{
  types::qualifiers qs;

  for (auto q : _tqs)
    qs.add(q.get().get_kind());

  if (_tss.empty()) {
    _set_type(std_int_type::create(std_int_type::kind::k_int, true, qs));
    return;
  }

  std::shared_ptr<const addressable_type> result;
  bool is_auto_type = false;
  _tss[0].get().process<void, type_set<type_specifier_pod,
				       type_specifier_ext_int,
				       type_specifier_ext_float,
				       type_specifier_tdid,
				       type_specifier_auto_type,
				       struct_or_union_def,
				       struct_or_union_ref,
				       enum_def, enum_ref,
				       typeof_expr, typeof_type_name> >
    (wrap_callables<default_action_unreachable<void, type_set<> >::type>
     ([](const type_specifier_pod&) {
       // Will be processed below
       return;
      },
      [](const type_specifier_ext_int&) {
       // Will be processed below
       return;
      },
      [](const type_specifier_ext_float&) {
       // Will be processed below
       return;
      },
      [&](const type_specifier_tdid &ts_tdid) {
	const type_specifier_tdid::resolved &r = ts_tdid.get_resolved();
	switch (r.get_kind()) {
	case type_specifier_tdid::resolved::resolved_kind::builtin_typedef:
	  result = (r.get_builtin_typedef_factory().create()
		    ->evaluate(a, tgt, ts_tdid)->amend_qualifiers(qs));
	  break;

	case type_specifier_tdid::resolved::resolved_kind::init_declarator:
	  result = (r.get_init_declarator().get_declarator()
		    .get_direct_declarator_id()
		    .get_type()->amend_qualifiers(qs));
	  break;

	case type_specifier_tdid::resolved::resolved_kind::none:
	  assert(0);
	  __builtin_unreachable();
	};
      },
      [&](const type_specifier_auto_type&) {
	is_auto_type = true;
      },
      [&](struct_or_union_def &soud) {
	result = struct_or_union_type::create(soud.get_tag_kind(),
					      soud.get_decl_list_node(), qs);
      },
      [&](const struct_or_union_ref &sour) {
	result = struct_or_union_type::create(sour.get_tag_kind(),
					      sour.get_decl_list_node(), qs);
      },
      [&](enum_def &ed) {
	enum_content &ec = ed.get_enumerator_list().get_content();
	tgt.evaluate_enum_type(a, _evaluator::make_expr_evaluator(a, tgt),
			       ed.get_asl_before(), ed.get_asl_after(), ec);
	result = enum_type::create(ed.get_decl_list_node(), qs);
      },
      [&](const enum_ref &er) {
	result = enum_type::create(er.get_decl_list_node(), qs);
      },
      [&](const typeof_expr &to_e) {
	handle_types<void>
	  ((wrap_callables<no_default_action>
	    ([&](const addressable_type &a_t) {
	       result = a_t.amend_qualifiers(qs);
	     },
	     [&](const type&) {
	       code_remark remark(code_remark::severity::fatal,
				  "invalid expression type in typeof()",
				  a.get_pp_result(),
				  to_e.get_expr().get_tokens_range());
	       a.get_remarks().add(remark);
	       throw semantic_except(remark);
	     })),
	   *to_e.get_expr().get_type());
      },
      [&](const typeof_type_name &to_tn) {
	result = to_tn.get_type_name().get_type()->amend_qualifiers(qs);
      }));

  if (result.get() || is_auto_type) {
    if (_tss.size() > 1) {
      const type_specifier &extra_ts = _tss[1].get();
      code_remark remark(code_remark::severity::fatal,
			 "conflicting type specifier",
			 a.get_pp_result(), extra_ts.get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    _set_type(std::move(result));
    return;
  }

  // Handle PODs.
  enum {
    cls_unknown,
    cls_void,
    cls_bool,
    cls_int,
    cls_float,
  } cls = cls_unknown;

  bool is_signed = false;
  bool is_unsigned = false;

  bool is_short = false;
  unsigned int is_long = 0;

  bool is_complex = false;

  enum {
    ibt_unknown,
    ibt_char,
    ibt_int,
    ibt_ext_int,
  } int_base_type = ibt_unknown;

  types::ext_int_type::kind ext_int_kind{-1};

  enum {
    fbt_unknown,
    fbt_float,
    fbt_double,
    fbt_ext_float,
  } float_base_type = fbt_unknown;

  types::ext_float_type::kind ext_float_kind{-1};

  for (auto ts : _tss) {
    ts.get().process<void, type_set<type_specifier_pod,
				    type_specifier_ext_int,
				    type_specifier_ext_float,
				    type_specifier> >
      (wrap_callables<default_action_unreachable<void, type_set<> >
		      ::template type>
       ([&](const type_specifier_pod &ts_pod) {
	 bool conflict = false;

	 switch (ts_pod.get_pod_spec()) {
	 case pod_spec::ps_void:
	   conflict = (cls != cls_unknown || is_long);
	   cls = cls_void;
	   break;

	 case pod_spec::ps_bool:
	   conflict = (cls != cls_unknown || is_long);
	   cls = cls_bool;
	   break;

	 case pod_spec::ps_signed:
	   conflict = ((cls != cls_unknown && cls != cls_int) ||
		       is_signed || is_unsigned);
	   is_signed = true;
	   cls = cls_int;
	   break;

	 case pod_spec::ps_unsigned:
	   conflict = ((cls != cls_unknown && cls != cls_int) ||
		       is_signed || is_unsigned);
	   is_unsigned = true;
	   cls = cls_int;
	   break;

	 case pod_spec::ps_short:
	   conflict = ((cls != cls_unknown && cls != cls_int) ||
		       is_short || is_long ||
		       (int_base_type != ibt_unknown &&
			int_base_type != ibt_int));
	   is_short = true;
	   cls = cls_int;
	   break;

	 case pod_spec::ps_long:
	   conflict = ((cls != cls_unknown && cls != cls_int
			&& cls != cls_float) ||
		       is_short ||
		       (cls == cls_float &&
			(is_long ||
			 (float_base_type != fbt_unknown &&
			  float_base_type != fbt_double))) ||
		       is_long > 1 ||
		       (cls == cls_int && (int_base_type != ibt_unknown &&
					   int_base_type != ibt_int)));
	   ++is_long;
	   if (is_long > 1)
	     cls = cls_int;
	   break;

	 case pod_spec::ps_complex:
	   conflict = ((cls != cls_unknown && cls != cls_float) ||
		       is_complex);
	   is_complex = true;
	   cls = cls_float;
	   break;

	 case pod_spec::ps_char:
	   conflict = ((cls != cls_unknown && cls != cls_int) ||
		       is_short || is_long ||
		       int_base_type != ibt_unknown);
	   int_base_type = ibt_char;
	   cls = cls_int;
	   break;

	 case pod_spec::ps_int:
	   conflict = ((cls != cls_unknown && cls != cls_int) ||
		       int_base_type != ibt_unknown);
	   int_base_type = ibt_int;
	   cls = cls_int;
	   break;

	 case pod_spec::ps_float:
	   conflict = ((cls != cls_unknown && cls != cls_float) ||
		       is_long ||
		       float_base_type != fbt_unknown);
	   float_base_type = fbt_float;
	   cls = cls_float;
	   break;

	 case pod_spec::ps_double:
	   conflict = ((cls != cls_unknown && cls != cls_float) ||
		       float_base_type != fbt_unknown ||
		       is_long > 1);
	   float_base_type = fbt_double;
	   cls = cls_float;
	   break;
	 }

	 if (conflict) {
	   code_remark remark(code_remark::severity::fatal,
			      "conflicting type specifier",
			      a.get_pp_result(), ts_pod.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }
       },
       [&](const type_specifier_ext_int &ts_ext_int) {
	 if ((cls != cls_unknown && cls != cls_int) ||
	     is_short || is_long ||
	     int_base_type != ibt_unknown) {
	   code_remark remark(code_remark::severity::fatal,
			      "conflicting type specifier",
			      a.get_pp_result(), ts_ext_int.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }

	 cls = cls_int;
	 int_base_type = ibt_ext_int;
	 ext_int_kind = ts_ext_int.get_ext_int_kind();
       },
       [&](const type_specifier_ext_float &ts_ext_float) {
	 if ((cls != cls_unknown && cls != cls_float) ||
	     is_long ||
	     float_base_type != fbt_unknown) {
	   code_remark remark(code_remark::severity::fatal,
			      "conflicting type specifier",
			      a.get_pp_result(),
			      ts_ext_float.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }

	 cls = cls_float;
	 float_base_type = fbt_ext_float;
	 ext_float_kind = ts_ext_float.get_ext_float_kind();
       },
       [&](const type_specifier &_ts) {
	 code_remark remark(code_remark::severity::fatal,
			    "conflicting type specifier",
			    a.get_pp_result(), _ts.get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       }));
  }

  switch (cls) {
  case cls_void:
    _set_type(void_type::create(qs));
    return;

  case cls_bool:
    _set_type(bool_type::create(qs));
    return;

  case cls_unknown:
    assert(is_long);
    /* fall through */
  case cls_int:
    if (int_base_type == ibt_char && !is_signed && !is_unsigned) {
      _set_type(plain_char_type::create(qs));
      return;
    } else if (int_base_type == ibt_ext_int) {
      _set_type(ext_int_type::create(ext_int_kind, !is_unsigned, qs));
      return;
    }

    assert(!(is_signed && is_unsigned));
    assert(!(is_short && is_long));
    assert(is_long <= 2);
    {
      std_int_type::kind k;
      switch (int_base_type) {
      case ibt_char:
	k = std_int_type::kind::k_char;
	break;

      case ibt_unknown:
	assert(is_short || is_long || is_signed || is_unsigned);
	/* fall through */
      case ibt_int:
	if (is_short) {
	  k = std_int_type::kind::k_short;
	} else if (is_long == 1) {
	  k = std_int_type::kind::k_long;
	} else if (is_long == 2) {
	  k = std_int_type::kind::k_long_long;
	} else {
	  k = std_int_type::kind::k_int;
	}
	break;
      }

      _set_type(std_int_type::create(k, !is_unsigned, qs));
      return;
    }

  case cls_float:
    {
      std::shared_ptr<const real_float_type> rft;
      const types::qualifiers &rt_qs = !is_complex ? qs : types::qualifiers{};

      switch (float_base_type) {
      case fbt_float:
	assert(!is_long);
	rft = std_float_type::create(std_float_type::kind::k_float, rt_qs);
	break;

      case fbt_unknown:
	assert(is_complex);
	/* fall through */
      case fbt_double:
	assert(is_long <= 1);
	if (!is_long)
	  rft = std_float_type::create(std_float_type::kind::k_double, rt_qs);
	else
	  rft = std_float_type::create(std_float_type::kind::k_long_double,
				       rt_qs);
	break;

      case fbt_ext_float:
	rft = ext_float_type::create(ext_float_kind, rt_qs);
	break;
      }

      if (!is_complex)
	_set_type(std::move(rft));
      else
	_set_type(complex_float_type::create(std::move(rft), qs));

      return;
    }
  }
}


std::shared_ptr<const klp::ccp::types::addressable_type>
parameter_declaration_declarator::get_type() const
{
  return _d.get_innermost_type();
}


void parameter_declaration_abstract::
evaluate_type(ast &a, const target &tgt)
{
  if (static_cast<bool>(_type))
    return;

  if (_ad) {
    // The type has already been determined in the abstract declarator.
    _type = _ad->get_innermost_type();
    return;
  }

  // There's no abstract_declarator.
  // It can happen that the current type is of array or function
  // type: namely with typedefs passing the
  // direct_abstract_declarator_array and
  // direct_abstract_declarator_func logic. Decay the type to
  // pointers in this case.
  _type =
    handle_types<std::shared_ptr<const addressable_type>>
    ((wrap_callables<default_action_unreachable
		     <std::shared_ptr<const addressable_type>,
		      type_set<> >::type>
      ([&](const std::shared_ptr<const array_type> &at) {
	 return (at->get_element_type()
		 ->amend_qualifiers(at->get_qualifiers())
		 ->derive_pointer());
       },
       [&](const std::shared_ptr<const function_type> &ft) {
	 return ft->derive_pointer();
       },
       [&](const std::shared_ptr<const addressable_type> &_t) {
	 return _t;
       })),
     _ds.get_type());

  _type = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				  std::move(_type), _ds, _asl);
}

std::shared_ptr<const klp::ccp::types::addressable_type>
parameter_declaration_abstract::get_type()const
{
  assert(static_cast<bool>(_type));
  return _type;
}


void direct_abstract_declarator_parenthesized::
evaluate_type(ast &a, const target &tgt)
{
  std::shared_ptr<const addressable_type> t = _get_enclosing_type();

  if (_asl) {
    t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				std::move(t), _asl);
  }

  _set_type(std::move(t));
}

static void evaluate_array_size_expr(expr &size, klp::ccp::ast::ast &a,
				     const target &tgt)
{
  // Array declarators are evaluated in DFS pre, evaluate the size
  // expr.
  _evaluator e(a, size, tgt);
  e();

  size.apply_lvalue_conversion(false);

  if (!is_type<int_type>(*size.get_type()) &&
      !is_type<bitfield_type>(*size.get_type())) {
    code_remark remark(code_remark::severity::fatal,
		       "array size expression is not of integer type",
		       a.get_pp_result(), size.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (!size.is_constexpr())
    return;

  const constexpr_value &size_cv = size.get_constexpr_value();
  assert(size_cv.get_value_kind() ==
	 klp::ccp::ast::constexpr_value::value_kind::vk_int);
  if (size_cv.get_int_value().is_negative()) {
    code_remark remark(code_remark::severity::fatal,
		       "array size expression is negative",
		       a.get_pp_result(), size.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }
}

void direct_abstract_declarator_array::
evaluate_type(ast &a, const target &tgt)
{
  const object_type &o_et =
    handle_types<const object_type &>
    ((wrap_callables<no_default_action>
      ([](const object_type &_o_et) -> const object_type& {
	 return _o_et;
       },
       [&](const type&) -> const object_type& {
	 code_remark remark(code_remark::severity::fatal,
			    "invalid array element type",
			    a.get_pp_result(), get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       })),
     *_get_enclosing_type());

  if (!o_et.is_complete()) {
    code_remark remark(code_remark::severity::fatal,
		       "incomplete array element type",
		       a.get_pp_result(), get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  // An outermost array in function parameter declaration context,
  // i.e. the innermost declarator derivation, needs special
  // treatment: if the array is a function parameter's type, it must
  // get adjusted to pointer type.
  type_name *tn = nullptr;
  if (!_dad) {
    parameter_declaration_abstract *pda = nullptr;
    this->for_each_ancestor<type_set<type_name,
				     parameter_declaration_abstract>>
      (wrap_callables<default_action_return_value<bool, true>::type>
       ([&](parameter_declaration_abstract &_pda) {
	  pda = &_pda;
	  // The ancestor search stops here/
	},
	[&](type_name &_tn) {
	  tn = &_tn;
	  // The ancestor search stops here.
	}));

    if (pda) {
      // Outermost array in a function parameter declaration, adjust the
      // type to "pointer to element type".
      qualifiers qs;
      if (_tql)
	qs = _tql->get_qualifiers();

      std::shared_ptr<const addressable_type> t = o_et.derive_pointer(qs);
      t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				  std::move(t),
				  pda->get_declaration_specifiers(),
				  pda->get_asl());
      _set_type(std::move(t));
      return;

    } else {
      assert(tn);

    }
  }

  // Not the outermost array or outermost array in a type name --
  // the type will be an array type.
  std::shared_ptr<const addressable_type> t;
  if (_size) {
    evaluate_array_size_expr(*_size, a, tgt);
    t = o_et.derive_array(_size);
  } else {
    t = o_et.derive_array(_vla_unspec_size);
  }

  if (tn) {
    // Outermost array in a type name, apply attributes.
    t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				std::move(t),
				tn->get_specifier_qualifier_list());
  }

  _set_type(std::move(t));
}

void direct_abstract_declarator_func::
evaluate_type(ast &a, const target &tgt)
{
  const returnable_type &r_t =
    handle_types<const returnable_type &>
    ((wrap_callables<no_default_action>
      ([](const returnable_type &_r_et) -> const returnable_type& {
	 return _r_et;
       },
       [&](const type&) -> const returnable_type& {
	 code_remark remark(code_remark::severity::fatal,
			    "invalid function return type",
			    a.get_pp_result(), get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       })),
     *_get_enclosing_type());

  std::shared_ptr<const addressable_type> t;
  if (_ptl) {
    // direct_declarator_func is evaluated in DFS PRE.
    // Evaluate parameter types first.
    _evaluator e(a, *_ptl, tgt);
    e();

    types::parameter_type_list ptl;
    if (!_ptl->is_single_void(a)) {
      _ptl->for_each([&](const parameter_declaration &pd) {
		       ptl.emplace_back(pd.get_type());
		     });
    }

    t = r_t.derive_function(std::move(ptl), _ptl->is_variadic());

  } else {
    t = r_t.derive_function(false);

  }

  // An outermost function type in function parameter declaration
  // context, i.e. the innermost declarator derivation, needs special
  // treatment: if the function type is a function parameter's type,
  // it must get adjusted to "pointer to function".
  if (!_dad) {
    parameter_declaration_abstract *pda = nullptr;
    type_name *tn = nullptr;
    this->for_each_ancestor<type_set<type_name,
				     parameter_declaration_abstract>>
      (wrap_callables<default_action_return_value<bool, true>::type>
       ([&](parameter_declaration_abstract &_pda) {
	  pda = &_pda;
	  // The ancestor search stops here/
	},
	[&](type_name &_tn) {
	  tn = &_tn;
	  // The ancestor search stops here.
	}));

    if (pda) {
      // Outermost function in a function parameter declaration, adjust
      // the type to "pointer to function type".
      t = t->derive_pointer();
      t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				  std::move(t),
				  pda->get_declaration_specifiers(),
				  pda->get_asl());
    } else {
      // Outermost function derivation (innermost declarator) in a
      // type name.  Apply type name's attributes.
      assert(tn);
      t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				  std::move(t),
				  tn->get_specifier_qualifier_list());
    }
  }

  _set_type(std::move(t));
}

void abstract_declarator::
evaluate_type(ast &a, const target &tgt)
{
  std::shared_ptr<const addressable_type> t = _get_enclosing_type();
  if (_pt) {
    for (auto tql : _pt->get_type_qualifier_lists()) {
      if (!tql) {
	t = t->derive_pointer(qualifiers{});
      } else {
	const qualifiers qs = tql->get_qualifiers();
	std::shared_ptr<const pointer_type> pt = t->derive_pointer(qs);
	t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				    std::move(pt), *tql);
      }
    }
  }

  if (!_dad) {
    // We're at a leaf, this can happen only with pointers.
    assert(is_type<pointer_type>(*t));

    // Take into account any 'aligned' or 'mode'
    // attributes at the enclosing context (which take precedence
    // over any such attribute from the base type or attached to any
    // declarator).
    for_each_ancestor<type_set<type_name, parameter_declaration_abstract> >
      (wrap_callables<no_default_action>
       ([&](type_name &tn) {
	  t = tgt.evaluate_attributes(a,
				      _evaluator::make_expr_evaluator(a, tgt),
				      std::move(t),
				      tn.get_specifier_qualifier_list());
	  // The ancestor search stops here
	},
	[&](parameter_declaration_abstract &pda) {
	  t = tgt.evaluate_attributes(a,
				      _evaluator::make_expr_evaluator(a, tgt),
				      std::move(t),
				      pda.get_declaration_specifiers(),
				      pda.get_asl());
	  // The ancestor search stops here
	},
	[](const direct_abstract_declarator&) {
	  // Proceed the search upwards
	  return true;
	},
	[](const abstract_declarator&) {
	  // Proceed the search upwards
	  return true;
	}));
  }

  _set_type(std::move(t));
}

void direct_declarator_id::
evaluate_type(ast &a, const target &tgt)
{
  std::shared_ptr<const addressable_type> a_t = _get_enclosing_type();

  // Check whether there are any 'aligned' oder 'mode' attributes
  // in the enclosing context -- these take precedence
  // over anything we've got so far (base type alignment and
  // 'aligned' attributes at declarators).
  // Also, extract the linkage information, if any.
  linkage *l = nullptr;
  this->process_context<void>
    (wrap_callables<default_action_unreachable<void, type_set<> >::type>
     ([](const struct_declarator&) {
	// For struct_declarator's, the alignment attributes are
	// examined along with any bitfield width in the course of
	// struct/union member handling.
      },
      [&](init_declarator &id) {
	l = &id.get_linkage();

	declaration &enclosing_decl =
	  (id.get_unique_parent<init_declarator_list>()
	   .get_unique_parent<declaration>());

	assert(a_t ||
	       enclosing_decl.get_declaration_specifiers().is_auto_type());
	if (!a_t)
	  return;

	a_t =
	  tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				  std::move(a_t),
				  enclosing_decl.get_declaration_specifiers(),
				  id.get_asl_before(), id.get_asl_middle(),
				  id.get_asl_after());

      },
      [&](parameter_declaration_declarator &pdd) {
	// Decay parameters of array or function type to pointer types
	// as appropriate.
	handle_types<void>
	  ((wrap_callables<default_action_nop>
	    ([&](const array_type &at) {
	       a_t = (at.get_element_type()
		      ->amend_qualifiers(at.get_qualifiers())
		      ->derive_pointer());
	     },
	     [&](const function_type &ft) {
	       a_t = ft.derive_pointer();
	     })),
	   *a_t);

	a_t =
	  tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				  std::move(a_t),
				  pdd.get_declaration_specifiers(),
				  pdd.get_asl());
      },
      [&](function_definition &fd) {
	// Ignore alignment of functions, it's not an attribute of their
	// type.
	l = &fd.get_linkage();
      }));

  // For __auto_type declarations, the type will not be available
  // until after the initializer has been processed.
  if (!a_t)
    return;

  // Examine any previous declaration relate by linkage.
  if (l && !l->is_first_in_chain()) {
    const direct_declarator_id *prev_ddid = nullptr;
    switch (l->get_link_target_kind()) {
    case linkage::link_target_kind::init_decl:
      prev_ddid = &(l->get_target_init_declarator().get_declarator()
		    .get_direct_declarator_id());
      break;

    case linkage::link_target_kind::function_def:
      prev_ddid = &(l->get_target_function_definition().get_declarator()
		    .get_direct_declarator_id());
      break;

    case linkage::link_target_kind::unlinked:
      assert(0);
      __builtin_unreachable();
    }

    if (!a_t->is_compatible_with(tgt, *prev_ddid->get_type(), false)) {
      code_remark remark(code_remark::severity::warning,
			 "incompatible redeclaration",
			 a.get_pp_result(), get_tokens_range());
      a.get_remarks().add(remark);

    } else if (l->get_linkage_kind() != linkage::linkage_kind::none &&
	       l->is_target_visible()) {
      // See whether composite type construction gives something new.
      std::shared_ptr<const addressable_type> comp_type
	= a_t->construct_composite(tgt, *prev_ddid->get_type());
      if (comp_type) {
	// It did.
	l->set_type_modified_through_linkage();
	a_t = std::move(comp_type);
      }
    }
  }

  _set_type(std::move(a_t));
}

void direct_declarator_id::deduce_auto_type(ast &a, const target &tgt,
					    init_declarator &id)
{
  assert(id.get_initializer());
  const initializer_expr *ie = nullptr;
  id.get_initializer()->process<void, type_set<const initializer_expr>>
    (wrap_callables<default_action_nop>
     ([&](const initializer_expr &_ie) {
	ie = &_ie;
      }));
  if (!ie) {
    code_remark remark(code_remark::severity::fatal,
		       "__auto_type initializer not an expression",
		       a.get_pp_result(), id.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  std::shared_ptr<const object_type> o_t;
  handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const std::shared_ptr<const object_type> &&_o_t) {
	 o_t = std::move(_o_t);
       })),
     ie->get_expr().get_type());
  if (!o_t) {
    code_remark remark
      (code_remark::severity::fatal,
       "__auto_type initializer expression not of object type",
       a.get_pp_result(), id.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  } else if (!o_t->is_complete()) {
    code_remark remark
      (code_remark::severity::fatal,
       "__auto_type initializer expression of incompete type",
       a.get_pp_result(), id.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  o_t = o_t->strip_qualifiers();

  // Apply attributes from the enclosing context.
  declaration &containing_decl = id.get_containing_declaration();
  std::shared_ptr<const addressable_type> a_t =
    tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
			    std::move(o_t),
			    containing_decl.get_declaration_specifiers(),
			    id.get_asl_before(), id.get_asl_middle(),
			    id.get_asl_after());

  // Now that the __auto_type has been deduced, verify compatibility
  // with previous declarations, if any.
  const linkage &l = id.get_linkage();
  if (!l.is_first_in_chain() &&
      (l.get_link_target_kind() != linkage::link_target_kind::init_decl ||
       !(a_t->is_compatible_with
	 (tgt,
	  (*l.get_target_init_declarator().get_declarator()
	   .get_direct_declarator_id().get_type()),
	  false)))) {
    code_remark remark(code_remark::severity::warning,
		       "incompatible redeclaration",
		       a.get_pp_result(), id.get_tokens_range());
    a.get_remarks().add(remark);
  }

  // Finally, assign the deduced type to the resp. AST entities.
  // For consistency with how non-__auto_type declarations are
  // handled, d receives the type without attributes from the
  // surrounding context applied, and ddid the type with attributes
  // taking effect.
  id.get_declarator()._set_deduced_auto_type(std::move(o_t));
  _set_type(std::move(a_t));
}


void direct_declarator_parenthesized::
evaluate_type(ast &a, const target &tgt)
{
  std::shared_ptr<const addressable_type> t = _get_enclosing_type();

  if (_asl) {
    t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				std::move(t), _asl);
  }

  _set_type(std::move(t));
}

void direct_declarator_array::
evaluate_type(ast &a, const target &tgt)
{
  const object_type &o_et =
    handle_types<const object_type&>
    ((wrap_callables<no_default_action>
      ([](const object_type &_o_et) -> const object_type& {
	 return _o_et;
       },
       [&](const type&) -> const object_type& {
	 code_remark remark(code_remark::severity::fatal,
			    "invalid array element type",
			    a.get_pp_result(), get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       })),
     *_get_enclosing_type());


  if (!o_et.is_complete()) {
    code_remark remark(code_remark::severity::fatal,
		       "incomplete array element type",
		       a.get_pp_result(), get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (_size) {
    evaluate_array_size_expr(*_size, a, tgt);
    _set_type(o_et.derive_array(_size));
  } else {
    // No size given. In the context of init declarators, the type
    // might get completed later in _complete_type_from_init().
    _set_type(o_et.derive_array(_vla_unspec_size));
  }
}

void direct_declarator_func::
evaluate_type(ast &a, const target &tgt)
{
  const returnable_type &r_t =
    handle_types<const returnable_type&>
    ((wrap_callables<no_default_action>
      ([](const returnable_type &_r_et) -> const returnable_type& {
	 return _r_et;
       },
       [&](const type&) -> const returnable_type& {
	 code_remark remark(code_remark::severity::fatal,
			    "invalid function return type",
			    a.get_pp_result(), get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       })),
     *_get_enclosing_type());

  std::shared_ptr<const addressable_type> t;
  if (_ptl) {
    // direct_declarator_func is evaluated in DFS PRE.
    // Evaluate parameter types first.
    _evaluator e(a, *_ptl, tgt);
    e();

    types::parameter_type_list ptl;
    if (!_ptl->is_single_void(a)) {
      _ptl->for_each([&](const parameter_declaration &pd) {
		       ptl.emplace_back(pd.get_type());
		     });
    }

    t = r_t.derive_function(std::move(ptl), _ptl->is_variadic());

  } else {
    const bool is_definition =
      (this->get_parent()->process<bool, type_set<declarator>>
       (wrap_callables<default_action_return_value<bool, false>::type>
	([](const declarator &d) {
	  return d.get_parent()->is_any_of<function_definition>();
	 })));

    if (_il) {
      assert(_il->get_identifiers().size() > 0);
      if (is_definition) {
	t = r_t.derive_function(_il->get_identifiers().size());
      } else {
	code_remark remark(code_remark::severity::warning,
			   "initializer list not part of definition",
			   a.get_pp_result(), _il->get_tokens_range());
	a.get_remarks().add(remark);
	t = r_t.derive_function(false);
      }
    } else {
      t = r_t.derive_function(is_definition);
    }
  }

  _set_type(std::move(t));
}

void declarator::evaluate_type(ast &a, const target &tgt)
{
  std::shared_ptr<const addressable_type> t = _get_enclosing_type();
  // If an __auto_type declaration, then the type will not be available
  // until the initializer has been processed.
  if (!t)
    return;
  if (_pt) {
    for (auto tql : _pt->get_type_qualifier_lists()) {
      if (!tql) {
	t = t->derive_pointer(qualifiers{});
      } else {
	const qualifiers qs = tql->get_qualifiers();
	std::shared_ptr<const pointer_type> pt = t->derive_pointer(qs);
	t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				    std::move(pt), *tql);
      }
    }
  }

  _set_type(std::move(t));
}

void type_name::evaluate_type(ast &a, const target &tgt)
{
  if (_ad) {
    _set_type(_ad->get_innermost_type());
  } else {
    // There's no abstract_declarator, apply any attributes here.
    std::shared_ptr<const addressable_type> t = _sql.get_type();
    t = tgt.evaluate_attributes(a, _evaluator::make_expr_evaluator(a, tgt),
				std::move(t), _sql);
    _set_type(std::move(t));
  }
}

void enumerator::register_at_parent(ast &a, const target &tgt)
{
  const std::string& name = a.get_pp_tokens()[_id_tok].get_value();

  enumerator_list &el = get_unique_parent<enumerator_list>();
  if (el.get_content().lookup(name)) {
    code_remark remark(code_remark::severity::fatal,
		       "enumerator redefined",
		       a.get_pp_result(), _id_tok);
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  enum_content &ec = el.get_content();
  if (_value) {
    if (!_value->is_constexpr()) {
      code_remark remark(code_remark::severity::fatal,
			 "enumerator's value is not a constant expression",
			 a.get_pp_result(), _value->get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    const constexpr_value &cv = _value->get_constexpr_value();
    if (!cv.has_constness(constness::c_integer_constant_expr)) {
      code_remark remark
	(code_remark::severity::fatal,
	 "enumerator's value is not an integer constant expression",
	 a.get_pp_result(), _value->get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }
    assert(cv.get_value_kind() == constexpr_value::value_kind::vk_int);

    const target_int &value = cv.get_int_value();
    handle_types<void>
      ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
	([&](const std::shared_ptr<const int_type> &it) {
	  ec.add_member(*this, name, it, value);
	 })),
       _value->get_type());

  } else {
    try {
      ec.add_member(*this, name, tgt);

    } catch (const std::overflow_error&) {
      code_remark remark(code_remark::severity::fatal,
			 "overflow in enumerator value",
			 a.get_pp_result(), get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }
  }
}


void expr_list::apply_lvalue_conversions(const bool only_decay)
{
  for (auto &e : _exprs)
    e.get().apply_lvalue_conversion(only_decay);
}


void expr::apply_lvalue_conversion(const bool only_decay)
{
  // Expression type conversions as specified in C99 6.3.2.1
  handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const array_type &a_t) {
	 if (this->is_dereferenced_const_addr())
	   _value->get_address_value().undereference();
	 _reset_type(a_t.get_element_type()->derive_pointer());
	 _set_lvalue(false);
       },
       [&](const function_type &f_t) {
	 if (this->is_dereferenced_const_addr())
	   _value->get_address_value().undereference();
	 _reset_type(f_t.derive_pointer());
	 _set_lvalue(false);
       },
       [&](const type &t) {
	 if (only_decay)
	   return;

	 _reset_type(t.strip_qualifiers());
	 _set_lvalue(false);
       })),
     *this->get_type());
}


void expr_comma::evaluate_type(ast&, const target&)
{
  _left.apply_lvalue_conversion(false);
  _right.apply_lvalue_conversion(false);

  _set_type(_right.get_type());
  if (_right.is_constexpr()) {
    auto cv = _right.get_constexpr_value().clone();
    cv->drop_constness();
    _set_value(std::move(cv));
  }
}


static mpa::limbs _check_pointer_arithmetic(klp::ccp::ast::ast &a,
					    const pointer_type &pt,
					    const expr &e,
					    const target &tgt)
{
  return (handle_types<mpa::limbs>
	  ((wrap_callables<default_action_unreachable<mpa::limbs, type_set<>>
			   ::type>
	    ([&](const object_type &o_t) {
	       if (!o_t.is_complete()) {
		 code_remark remark(code_remark::severity::fatal,
				    "pointer arithmetic on incomplete type",
				    a.get_pp_result(), e.get_tokens_range());
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       } else if (o_t.is_size_constant()) {
		 return o_t.get_size(tgt);
	       } else {
		 return mpa::limbs{};
	       }
	     },
	     [&](const void_type&) {
	       // As an extension, GCC allows addition and subtraction to/from
	       // pointers to void.
	       return mpa::limbs::from_size_type(1);
	     },
	     [&](const function_type&) {
	       // As an extension, GCC allows addition and subtraction to/from
	       // function pointers.
	       return mpa::limbs::from_size_type(1);
	     })),
	   *pt.get_pointed_to_type()));
}

static std::unique_ptr<constexpr_value>
_do_pointer_arithmetic(const constexpr_value &cv_pointer,
		       const constexpr_value &cv_offset,
		       const mpa::limbs &pointed_to_size,
		       const bool sub)
{
  assert(cv_pointer.get_value_kind() ==
	 constexpr_value::value_kind::vk_address);
  assert(cv_offset.get_value_kind() ==
	 constexpr_value::value_kind::vk_int);

  constexpr_value::address_constant ac = cv_pointer.get_address_value();
  ac.add_to_offset(sub, cv_offset.get_int_value(),
		   pointed_to_size);
  if ((cv_pointer.has_constness
       (constexpr_value::constness::c_address_constant)) &&
      (cv_offset.has_constness
       (constexpr_value::constness::c_integer_constant_expr))) {
    return (std::unique_ptr<constexpr_value>
	    (new constexpr_value(constexpr_value::address_constant_tag{},
				 std::move(ac))));

  } else {
    return (std::unique_ptr<constexpr_value>
	    (new constexpr_value(std::move(ac))));

  }
}

static bool _check_transparent_union_func_arg_assignment
  (const target &tgt,
   const struct_or_union_type &sou,
   const expr &e_source) noexcept
{
  if (sou.get_kind() != struct_or_union_kind::souk_union)
    return false;

  const auto * union_content = sou.get_content();
  if (!union_content)
    return false;
  for (auto it_member = union_content->members_begin();
       it_member != union_content->members_end();
       ++it_member) {
    if (handle_types<bool>
	((wrap_callables<default_action_return_value<bool, false>::type>
	  ([&](const pointer_type &target, const pointer_type &source) {
	     const addressable_type &pointed_to_type_target =
	       *target.get_pointed_to_type();
	     const addressable_type  &pointed_to_type_source =
	       *source.get_pointed_to_type();

	     if (!(pointed_to_type_source.get_qualifiers().is_subset_of
		   (pointed_to_type_target.get_qualifiers()))) {
	       return false;
	     }

	     return (is_type<void_type>(pointed_to_type_target) ||
		     is_type<void_type>(pointed_to_type_source) ||
		     (pointed_to_type_target
		      .is_compatible_with(tgt, pointed_to_type_source,
					  true)));
	   },
	   [&](const pointer_type &, const int_type &source) {
	     // Source expression shall be a zero integer constant
	     // expression.
	     return (e_source.is_constexpr() &&
		     e_source.get_constexpr_value().is_zero());
	   },

	   [&](const object_type &target, const object_type &source) {
	     return (target.is_complete() && source.is_complete() &&
		     target.is_compatible_with(tgt, source, true));

	  })),
	 *it_member->get_type(), *e_source.get_type())) {
      return true;
    }
  }
  return false;
}

void
klp::ccp::check_types_assignment(klp::ccp::ast::ast &a,
				 const target &tgt,
				 const type &t_target,
				 const expr &e_source,
				 const bool is_func_arg)
{
  auto &&check_enum_completeness_lhs =
    [&](const type &t_lhs) {
	 handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_lhs) {
		if (!et_lhs.is_complete()) {
		  code_remark remark(code_remark::severity::fatal,
				     "assignment to incomplete enum type",
				     a.get_pp_result(),
				     e_source.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    t_lhs);
    };

  auto &&check_enum_completeness_rhs =
    [&](const type &t_rhs) {
	 handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_rhs) {
		if (!et_rhs.is_complete()) {
		  code_remark remark(code_remark::severity::fatal,
				     "assignment from incomplete enum type",
				     a.get_pp_result(),
				     e_source.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    t_rhs);
    };

  handle_types<void>
    ((wrap_callables<no_default_action>
      ([&](const bool_type&, const arithmetic_type &source) {
	 check_enum_completeness_rhs(source);

	 // Always Ok

       },
       [](const bool_type&, const pointer_type&) {
	 // Always Ok

       },
       [&](const arithmetic_type &target, const arithmetic_type &source) {
	 check_enum_completeness_lhs(target);
	 check_enum_completeness_rhs(source);

	 // Always Ok

       },
       [&](const arithmetic_type &target, const bitfield_type&) {
	 check_enum_completeness_lhs(target);

	 // Always Ok

       },
       [&](const bitfield_type&, const arithmetic_type &source) {
	 check_enum_completeness_rhs(source);

	 // Always Ok

       },
       [&](const bitfield_type&, const bitfield_type&) {
	 // Always Ok

       },
       [&](const bitfield_type &bft, const pointer_type&) {
	 // If the bitfield's underlying type is a bool,
	 // then the assignment is Ok and has the same semantics
	 // as an assignment to plain bool.
	 if (!is_type<bool_type>(*bft.get_base_type())) {
	   code_remark remark
	     (code_remark::severity::warning,
	      "assignment from pointer to non-boolean bitfield type",
	      a.get_pp_result(), e_source.get_tokens_range());
	   a.get_remarks().add(remark);
	 }

       },
       [&](const int_type &target, const pointer_type&) {
	 code_remark remark
	   (code_remark::severity::warning,
	    "assignment to integer from pointer",
	    a.get_pp_result(), e_source.get_tokens_range());
	 a.get_remarks().add(remark);

	 check_enum_completeness_lhs(target);
       },
       [&](const pointer_type&, const int_type &source) {
	 // Source expression shall be a zero integer constant
	 // expression.
	 if (!e_source.is_constexpr() ||
	     !e_source.get_constexpr_value().is_zero()) {
	   // Not a null pointer constant.
	   code_remark remark
	     (code_remark::severity::warning,
	      "assignment to pointer from integer which is not NULL",
	      a.get_pp_result(), e_source.get_tokens_range());
	   a.get_remarks().add(remark);
	 }

	 check_enum_completeness_lhs(source);

       },
       [&](const pointer_type&, const bitfield_type&) {
	 // Source expression shall be a zero integer constant
	 // expression, but bitfields never qualify as such one.
	 code_remark remark
	   (code_remark::severity::warning,
	    "assignment to pointer from integer which is not NULL",
	    a.get_pp_result(), e_source.get_tokens_range());
	 a.get_remarks().add(remark);

       },
       [&](const pointer_type &target, const pointer_type &source) {
	 const addressable_type &pointed_to_type_target =
	   *target.get_pointed_to_type();
	 const addressable_type  &pointed_to_type_source =
	   *source.get_pointed_to_type();

	 if (!(pointed_to_type_source.get_qualifiers().is_subset_of
	       (pointed_to_type_target.get_qualifiers()))) {
	   code_remark remark
	     (code_remark::severity::warning,
	      "assignment's source qualifiers are not a subset of target ones",
	      a.get_pp_result(), e_source.get_tokens_range());
	   a.get_remarks().add(remark);
	 }

	 if (!is_type<void_type>(pointed_to_type_target) &&
	     !is_type<void_type>(pointed_to_type_source) &&
	     !(pointed_to_type_target
	       .is_compatible_with(tgt, pointed_to_type_source,
				   true))) {
	   // Ugh, oh. But gcc still only emits a warning though.
	   handle_types<void>
	     ((wrap_callables<default_action_unreachable<void, type_set<> >
				::type>
	       ([&](const int_type &ptt_it_target,
		    const int_type &ptt_it_source) {
		  if (pointed_to_type_target.is_complete() &&
		      pointed_to_type_source.is_complete() &&
		      ptt_it_target.get_width(tgt) !=
		      ptt_it_source.get_width(tgt)) {
		    code_remark remark
		      (code_remark::severity::warning,
		       "assigning pointers to integer types of different width",
		       a.get_pp_result(), e_source.get_tokens_range());
		    a.get_remarks().add(remark);
		  } else {
		    code_remark remark
		      (code_remark::severity::warning,
		       "pointers to different integer types in assignment",
		       a.get_pp_result(), e_source.get_tokens_range());
		    a.get_remarks().add(remark);
		  }
		},
		[&](const addressable_type&, const addressable_type&) {
		    code_remark remark
		      (code_remark::severity::warning,
		       "incompatible pointer types in assignment",
		       a.get_pp_result(), e_source.get_tokens_range());
		    a.get_remarks().add(remark);
		})),
	      pointed_to_type_target, pointed_to_type_source);
	 }

       },
       [&](const struct_or_union_type &target,
	   const struct_or_union_type &source) {
	 if (!target.is_complete()) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "assignment to incomplete struct or union type",
	      a.get_pp_result(), e_source.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }

	 if (!target.is_compatible_with(tgt, source, true) &&
	     !(is_func_arg &&
	       target.is_transparent_union() &&
	       _check_transparent_union_func_arg_assignment(tgt,
							    target,
							    e_source))) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "assignment from incompatible struct or union type",
	      a.get_pp_result(), e_source.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }
       },
       [&](const struct_or_union_type &target,
	   const type &source) {
	 if (!(is_func_arg &&
	       target.is_transparent_union() &&
	       _check_transparent_union_func_arg_assignment(tgt,
							    target,
							    e_source))) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "invalid type for assignment target",
	      a.get_pp_result(), e_source.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }
       },
       [&](const type&, const type&) {
	 code_remark remark
	   (code_remark::severity::fatal,
	    "invalid type for assignment target",
	    a.get_pp_result(), e_source.get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);

       })),
     t_target, *e_source.get_type());
}

void expr_assignment::evaluate_type(ast &a, const target &tgt)
{

  _lhs.apply_lvalue_conversion(true);
  _rhs.apply_lvalue_conversion(false);

  if (!_lhs.is_lvalue()) {
    code_remark remark(code_remark::severity::fatal,
		       "assigment to something which is not a lvalue",
		       a.get_pp_result(), _lhs.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const std::shared_ptr<const type> &t_lhs = _lhs.get_type();
  const std::shared_ptr<const type> &t_rhs = _rhs.get_type();

  auto &&check_enum_completeness_lhs =
    [&]() {
      handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_lhs) {
		if (!et_lhs.is_complete()) {
		  code_remark remark(code_remark::severity::fatal,
				     "assignment to incomplete enum type",
				     a.get_pp_result(),
				     _lhs.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *t_lhs);
    };

  auto &&check_enum_completeness_rhs =
    [&]() {
	 handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_rhs) {
		if (!et_rhs.is_complete()) {
		  code_remark remark(code_remark::severity::fatal,
				     "assignment from incomplete enum type",
				     a.get_pp_result(),
				     _rhs.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *t_rhs);
    };

  switch (_op) {
  case assign_op::set:
    {
      check_types_assignment(a, tgt, *t_lhs, _rhs, false);
      _set_type(t_lhs);
    }
    break;

  case assign_op::add:
  case assign_op::sub:
    // Check for the pointer +- integer case and fall through to the
    // mul/div case for the handling of arithmetic operands otherwise.
    if ((handle_types<bool>
	 ((wrap_callables<default_action_return_value<bool, false>::type>
	   ([&](const pointer_type &pt_lhs, const int_type&) {
	      _check_pointer_arithmetic(a, pt_lhs, _lhs, tgt);
	      check_enum_completeness_rhs();
	      _set_type(pt_lhs.strip_qualifiers());
	      return true;
	    },
	    [&](const pointer_type &pt_lhs, const bitfield_type&) {
	      _check_pointer_arithmetic(a, pt_lhs, _lhs, tgt);
	      _set_type(pt_lhs.strip_qualifiers());
	      return true;
	    })),
	  *t_lhs, *t_rhs))) {
      break;
    }

    /* fall through */
  case assign_op::mul:
  case assign_op::div:
    // Arithmetic operands wanted.
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const arithmetic_type&, const arithmetic_type&) {
	   check_enum_completeness_lhs();
	   check_enum_completeness_rhs();

	   // Ok
	   _set_type(t_lhs);
	 },
	 [&](const arithmetic_type&, const bitfield_type&) {
	   check_enum_completeness_lhs();

	   // Ok
	   _set_type(t_lhs);
	 },
	 [&](const bitfield_type&, const arithmetic_type&) {
	   check_enum_completeness_rhs();

	   // Ok
	   _set_type(t_lhs);
	 },
	 [&](const bitfield_type&, const bitfield_type&) {
	   // Ok
	   _set_type(t_lhs);
	 },
	 [&](const type&, const type&) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "non-arithmetic operand in arithmetic assignment",
	      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
       *t_lhs, *t_rhs);
    break;

  case assign_op::mod:
  case assign_op::lshift:
  case assign_op::rshift:
  case assign_op::bin_and:
  case assign_op::bin_xor:
  case assign_op::bin_or:
    // Integer operands wanted.
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const int_type&, const int_type&) {
	   check_enum_completeness_lhs();
	   check_enum_completeness_rhs();

	   _set_type(t_lhs);
	 },
	 [&](const int_type&, const bitfield_type&) {
	   check_enum_completeness_lhs();

	   _set_type(t_lhs);
	 },
	 [&](const bitfield_type&, const int_type&) {
	   check_enum_completeness_rhs();

	   _set_type(t_lhs);
	 },
	 [&](const bitfield_type&, const bitfield_type&) {
	   _set_type(t_lhs);
	 },
	 [&](const type&, const type&) {
	   code_remark remark (code_remark::severity::fatal,
			       "non-integer operand in integer assignment",
			       a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
       *t_lhs, *t_rhs);
    break;
  };
}


void expr_conditional::evaluate_type(ast &a, const target &tgt)
{
  _cond.apply_lvalue_conversion(false);
  if (_expr_true)
    _expr_true->apply_lvalue_conversion(false);
  _expr_false.apply_lvalue_conversion(false);

  if (!is_scalar_type(*_cond.get_type())) {
    code_remark remark
      (code_remark::severity::fatal,
       "ternary operator's condition evaluates to non-scalar type",
       a.get_pp_result(), _cond.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const expr &expr_true = _expr_true ? *_expr_true : _cond;

  const constexpr_value * const cv_cond =
    _cond.is_constexpr() ? &_cond.get_constexpr_value() : nullptr;
  const constexpr_value * const cv_true =
    expr_true.is_constexpr() ? &expr_true.get_constexpr_value() : nullptr;
  const constexpr_value * const cv_false =
    _expr_false.is_constexpr() ? &_expr_false.get_constexpr_value() : nullptr;
  const constexpr_value * const cv_value =
    cv_cond ? (!cv_cond->is_zero() ? cv_true : cv_false) : nullptr;

  const std::shared_ptr<const type> &t_true = expr_true.get_type();
  const std::shared_ptr<const type> &t_false = _expr_false.get_type();

  auto &&check_enum_completeness_t_true =
    [&]() {
      handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_true) {
		if (!et_true.is_complete()) {
		  code_remark remark
		    (code_remark::severity::fatal,
		     "ternary operator's operand has incomplete enum type",
		     a.get_pp_result(), expr_true.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *t_true);
    };

  auto &&check_enum_completeness_t_false =
    [&]() {
      handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_false) {
		if (!et_false.is_complete()) {
		  code_remark remark
		    (code_remark::severity::fatal,
		     "ternary operator's operand has incomplete enum type",
		     a.get_pp_result(), _expr_false.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *t_true);
    };

  handle_types<void>
    ((wrap_callables<no_default_action>
      ([&](const arithmetic_type &at_true, const arithmetic_type &at_false) {
	 check_enum_completeness_t_true();
	 check_enum_completeness_t_false();

	 const std::shared_ptr<const arithmetic_type> at_result
	   = [&]() {
	       try {
		 return at_true.arithmetic_conversion(tgt, at_false);
	       } catch (const std::range_error &re) {
		 code_remark remark
		 (code_remark::severity::fatal,
		  (std::string{
		      "could not determine common type for ternary operator: "
		   } +
		   re.what()),
		  a.get_pp_result(), this->get_tokens_range());
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       }
	     }();
	 _set_type(at_result);

	 if (cv_value) {
	   handle_types<void>
	     ((wrap_callables<default_action_nop>
	       ([&](const int_type &it_result) {
		  target_int i_result = cv_value->convert_to(tgt, it_result);
		  // Strictly speaking, the result is an integer
		  // constant expression if and only if all
		  // constituent subexpressions are. However, gcc relaxes this
		  // rule by ignoring the unusued one.
		  if (cv_cond->has_constness(constness::
						c_integer_constant_expr) &&
		      cv_value->has_constness(constness::
						c_integer_constant_expr)) {
		    _set_value(constexpr_value::integer_constant_expr_tag{},
			       std::move(i_result));
		  } else if
		      (cv_cond->has_constness(constness::
						c_arithmetic_constant_expr) &&
		       cv_value->has_constness(constness::
						c_arithmetic_constant_expr)) {
		    _set_value(constexpr_value::arithmetic_constant_expr_tag{},
			       std::move(i_result));
		  } else {
		    _set_value(std::move(i_result));
		  }
		},
		[&](const real_float_type &rft_result) {
		  target_float f_result = cv_value->convert_to(tgt,
							       rft_result);
		  if (cv_cond->has_constness(constness::
						c_arithmetic_constant_expr) &&
		      cv_true &&
		      cv_true->has_constness(constness::
						c_arithmetic_constant_expr) &&
		      cv_false &&
		      cv_false->has_constness(constness::
						c_arithmetic_constant_expr)) {
		    _set_value(constexpr_value::arithmetic_constant_expr_tag{},
			       std::move(f_result));
		  } else {
		    _set_value(std::move(f_result));
		  }
		})),
	      *at_result);
	 }

       },
       [&](const arithmetic_type &at_true, const bitfield_type &bft_false) {
	 check_enum_completeness_t_true();

	 std::shared_ptr<const arithmetic_type> at_result
	   = [&]() {
	       try {
		 return at_true.arithmetic_conversion(tgt,
						      *bft_false.promote(tgt));
	       } catch (const std::range_error &re) {
		 code_remark remark
		 (code_remark::severity::fatal,
		  (std::string{
		      "could not determine common type for ternary operator: "
		   } +
		   re.what()),
		  a.get_pp_result(), this->get_tokens_range());
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       }
	     }();
	 _set_type(std::move(at_result));

       },
       [&](const bitfield_type &bft_true, const arithmetic_type &at_false) {
	 check_enum_completeness_t_false();

	 std::shared_ptr<const arithmetic_type> at_result
	   = [&]() {
	       try {
		 return at_false.arithmetic_conversion(tgt,
						       *bft_true.promote(tgt));
	       } catch (const std::range_error &re) {
		 code_remark remark
		 (code_remark::severity::fatal,
		  (std::string{
		      "could not determine common type for ternary operator: "
		   } +
		   re.what()),
		  a.get_pp_result(), this->get_tokens_range());
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       }
	     }();
	 _set_type(std::move(at_result));

       },
       [&](const bitfield_type &bft_true, const bitfield_type &bft_false) {
	 check_enum_completeness_t_false();

	 std::shared_ptr<const arithmetic_type> at_result
	   = [&]() {
	       try {
		 return (bft_true.promote(tgt)
			 ->arithmetic_conversion(tgt, *bft_false.promote(tgt)));
	       } catch (const std::range_error &re) {
		 code_remark remark
		 (code_remark::severity::fatal,
		  (std::string{
		      "could not determine common type for ternary operator: "
		   } +
		   re.what()),
		  a.get_pp_result(), this->get_tokens_range());
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       }
	     }();
	 _set_type(std::move(at_result));

       },
       [&](const pointer_type &pt_true, const pointer_type &pt_false) {
	 const addressable_type &pointed_to_type_true
	   = *pt_true.get_pointed_to_type();
	 const addressable_type &pointed_to_type_false
	   = *pt_false.get_pointed_to_type();

	 // Check for the (void *)0 form of a null pointer constant.
	 if (is_type<void_type>(pointed_to_type_true) &&
	     !pointed_to_type_true.get_qualifiers().any() &&
	     expr_true.is_constexpr() &&
	     (expr_true.get_constexpr_value().has_constness
	      (constexpr_value::constness::c_address_constant)) &&
	     expr_true.get_constexpr_value().is_zero()) {
	  _set_type(t_false);

	 } else if (is_type<void_type>(pointed_to_type_false) &&
		    !pointed_to_type_false.get_qualifiers().any() &&
		    _expr_false.is_constexpr() &&
		    (_expr_false.get_constexpr_value().has_constness
		     (constexpr_value::constness::c_address_constant)) &&
		    _expr_false.get_constexpr_value().is_zero()) {
	  _set_type(t_true);

	 } else if (is_type<void_type>(pointed_to_type_true) ||
		    is_type<void_type>(pointed_to_type_false)) {
	   // The resulting type is pointer to void, qualified with the
	   // union of the operands' pointer to types' qualifiers.
	   qualifiers qs = pointed_to_type_true.get_qualifiers();
	   qs.add(pointed_to_type_false.get_qualifiers());
	   _set_type(void_type::create(qs)->derive_pointer());

	 } else {
	  if (!(pointed_to_type_true.is_compatible_with
		(tgt, pointed_to_type_false, true))) {
	    code_remark remark
	      (code_remark::severity::fatal,
	       "ternary operator's operands have incompatible pointer types",
	       a.get_pp_result(), get_tokens_range());
	    a.get_remarks().add(remark);
	    throw semantic_except(remark);
	  }

	  // Resulting type is a pointer to the composite pointed-to type
	  // with the qualifiers merged.
	  // In the case of an array type, the composite type is
	  // the type with known length, if any.
	  const addressable_type *comp_type = &pointed_to_type_true;
	  handle_types<void>
	    ((wrap_callables<default_action_nop>
	      ([&](const array_type &at_true) {
		 // If the array type is not complete, just pick the
		 // other one.
		 if (!at_true.is_complete())
		   comp_type = &pointed_to_type_false;
	       })),
	     pointed_to_type_true);

	  qualifiers qs = pointed_to_type_true.get_qualifiers();
	  qs.add(pointed_to_type_false.get_qualifiers());
	  _set_type(comp_type->amend_qualifiers(qs)->derive_pointer());
	 }

	 if (cv_value)
	   _set_value(cv_value->clone());

       },
       [&](const pointer_type&, const int_type&) {
	 // The false expression shall be a zero integer constant
	 // (NULL).
	 if (!_expr_false.is_constexpr() ||
	     !_expr_false.get_constexpr_value().is_zero()) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "expected null pointer constant for ternary operator's operand",
	      a.get_pp_result(), _expr_false.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }

	 check_enum_completeness_t_false();

	 _set_type(t_true);

	 if (cv_value) {
	   if (cv_value == &_expr_false.get_constexpr_value()) {
	     // Transform the null pointer constant in integer constant
	     // expression form to an address constant.
	     _set_value(constexpr_value::address_constant
			{target_int::create_zero(1, false)});
	   } else {
	     _set_value(cv_value->clone());
	   }
	 }

       },
       [&](const pointer_type&, const bitfield_type&) {
	 // The false expression shall be a zero integer constant
	 // (NULL), a bit-field obviously doesn't qualify
	 code_remark remark
	   (code_remark::severity::fatal,
	    "expected null pointer constant for ternary operator's operand",
	    a.get_pp_result(), _expr_false.get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       },
       [&](const int_type&, const pointer_type&) {
	 // The true expression shall be a zero integer constant
	 // (NULL).
	 if (!expr_true.is_constexpr() ||
	     !expr_true.get_constexpr_value().is_zero()) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "expected null pointer constant for ternary operator's operand",
	      a.get_pp_result(), expr_true.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }

	 check_enum_completeness_t_true();

	 _set_type(t_false);

	 if (cv_value) {
	   if (cv_value == &expr_true.get_constexpr_value()) {
	     // Transform the null pointer constant in integer constant
	     // expression form to an address constant.
	     _set_value(constexpr_value::address_constant
			{target_int::create_zero(1, false)});
	   } else {
	     _set_value(cv_value->clone());
	   }
	 }

       },
       [&](const bitfield_type&, const pointer_type&) {
	 // The true expression shall be a zero integer constant
	 // (NULL), a bit-field obviously doesn't qualify
	 code_remark remark
	   (code_remark::severity::fatal,
	    "expected null pointer constant for ternary operator's operand",
	    a.get_pp_result(), _expr_false.get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       },
       [&](const struct_or_union_type &sout_true,
	   const struct_or_union_type &sout_false) {
	 if (!sout_true.is_compatible_with(tgt, sout_false, true)) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "ternary operator's operands have incompatible types",
	      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 } else if (!sout_true.is_complete()) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "ternary operator's operand has incomplete struct/union type",
	      a.get_pp_result(), expr_true.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 }

	 _set_type(t_true);

       },
       [&](const void_type&, const void_type&) {
	_set_type(void_type::create());

       },
       [&](const void_type&, const type&) {
	 // As an extension, GCC allows this.
	_set_type(void_type::create());

       },
       [&](const type&, const void_type&) {
	 // As an extension, GCC allows this.
	_set_type(void_type::create());

       },
       [&](const type&, const type&) {
	 code_remark remark
	   (code_remark::severity::fatal,
	    "ternary operator's operands have invalid types",
	    a.get_pp_result(), get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       })),
     *t_true, *t_false);
}


void expr_binop::_evaluate_arith_binop(const arithmetic_type &at_left,
				       const arithmetic_type &at_right,
				       ast &a, const target &tgt)
{
  const std::shared_ptr<const arithmetic_type> at
    = [&]() {
	try {
	  return at_left.arithmetic_conversion(tgt, at_right);
	} catch (const std::range_error &re) {
	  code_remark remark
	  (code_remark::severity::fatal,
	   (std::string{
	       "could not determine common type for binary operation: "
	    } +
	    re.what()),
	   a.get_pp_result(), this->get_tokens_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}
      }();

  _set_type(at);

  if (_left.is_constexpr() && _right.is_constexpr()) {
    const constexpr_value &cv_left = _left.get_constexpr_value();
    const constexpr_value &cv_right = _right.get_constexpr_value();

    handle_types<void>
      ((wrap_callables<default_action_nop>
	([&](const int_type &it) {
	   const target_int i_left = cv_left.convert_to(tgt, it);
	   const target_int i_right = cv_right.convert_to(tgt, it);
	   target_int i_result;
	   switch (_op) {
	   case binary_op::mul:
	     try {
	       i_result = i_left * i_right;
	     } catch (const std::overflow_error&) {
	       code_remark remark(code_remark::severity::warning,
				  "integer overflow in multiplication",
				  a.get_pp_result(), get_tokens_range());
	       a.get_remarks().add(remark);
	       return;
	     }
	     break;

	   case binary_op::div:
	     if (!i_right) {
	       code_remark remark(code_remark::severity::warning,
				  "division by zero",
				  a.get_pp_result(), get_tokens_range());
	       a.get_remarks().add(remark);
	       return;
	     }

	     i_result = i_left / i_right;
	     break;

	   case binary_op::add:
	     try {
	       i_result = i_left + i_right;
	     } catch (const std::overflow_error&) {
	       code_remark remark(code_remark::severity::warning,
				  "integer overflow in addition",
				  a.get_pp_result(), get_tokens_range());
	       a.get_remarks().add(remark);
	       return;
	     }
	     break;

	   case binary_op::sub:
	     try {
	       i_result = i_left - i_right;
	     } catch (const std::overflow_error&) {
	       code_remark remark(code_remark::severity::warning,
				  "integer overflow in subtraction",
				  a.get_pp_result(), get_tokens_range());
	       a.get_remarks().add(remark);
	       return;
	     }
	     break;
	   };

	   if (cv_left.has_constness(constness::c_integer_constant_expr) &&
	       cv_right.has_constness(constness::c_integer_constant_expr)) {
	     _set_value(constexpr_value::integer_constant_expr_tag{},
			std::move(i_result));
	   } else if ((cv_left.has_constness
		       (constness::c_arithmetic_constant_expr)) &&
		      (cv_right.has_constness
		       (constness::c_arithmetic_constant_expr))) {
	     _set_value(constexpr_value::arithmetic_constant_expr_tag{},
			std::move(i_result));
	   } else {
	     _set_value(std::move(i_result));
	   }

	 },
	 [&](const real_float_type &rft) {
	   const target_float f_left = cv_left.convert_to(tgt, rft);
	   const target_float f_right = cv_right.convert_to(tgt, rft);
	   target_float f_result;
	   switch (_op) {
	   case binary_op::mul:
	     f_result = f_left * f_right;
	     break;

	   case binary_op::div:
	     f_result = f_left / f_right;
	     break;

	   case binary_op::add:
	     f_result = f_left + f_right;
	     break;

	   case binary_op::sub:
	     f_result = f_left - f_right;
	     break;
	   };

	   if ((cv_left.has_constness
		(constness::c_arithmetic_constant_expr)) &&
	       (cv_right.has_constness
		(constness::c_arithmetic_constant_expr))) {
	     _set_value(constexpr_value::arithmetic_constant_expr_tag{},
			std::move(f_result));
	   } else {
	     _set_value(std::move(f_result));
	   }

	 })),
       *at);
  }
}

void expr_binop::_evaluate_ptr_sub(const pointer_type &pt_left,
				   const pointer_type &pt_right,
				   ast &a, const target &tgt)
{
  if (!pt_left.is_compatible_with(tgt, pt_right, true)) {
    code_remark remark(code_remark::severity::fatal,
			  "subtraction of pointers to incompatible types",
			  a.get_pp_result(), get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const mpa::limbs pointed_to_size =
    _check_pointer_arithmetic(a, pt_left, _left, tgt);

  _set_type(tgt.create_ptrdiff_type(true));

  if (pointed_to_size && _left.is_constexpr() && _right.is_constexpr()) {
    const constexpr_value::address_constant &ac_left
      = _left.get_constexpr_value().get_address_value();
    const constexpr_value::address_constant &ac_right
      = _right.get_constexpr_value().get_address_value();

    if (ac_left.has_same_base_as(ac_right)) {
      mpa::limbs off_right = ac_right.get_offset();
      // If off_right is of the form 0b100..000, enlarge to make
      // the complement below positive.
      if (off_right && off_right.test_bit(off_right.width() - 1) &&
	  !off_right.is_any_set_below(off_right.width() - 1)) {
	const mpa::limbs::size_type w = off_right.width();
	off_right.resize(mpa::limbs::width_to_size(w) + 1);
	off_right.set_bits_at_and_above(w, true);
      }

      off_right = off_right.complement();
      mpa::limbs distance = ac_left.get_offset().add_signed(off_right);
      // Divide by element size
      bool negate = false;
      if (distance && distance.test_bit(distance.width() - 1)) {
	negate = true;
	distance = distance.complement();
      }

      auto div_result = distance / pointed_to_size;
      if (div_result.second) {
	code_remark remark
	  (code_remark::severity::warning,
	   "pointer difference is not a multiple of element size",
	   a.get_pp_result(), get_tokens_range());
	a.get_remarks().add(remark);
	return;
      }
      distance = std::move(div_result.first);

      // distance is taken to be positive. Make room for the
      // sign bit, if needed.
      if (distance && distance.test_bit(distance.width() - 1)) {
	distance.resize(mpa::limbs::width_to_size(distance.width()) + 1);
      }

      assert(!negate || distance.width());
      if (negate)
	distance = distance.complement();

      const mpa::limbs::size_type ptrdiff_width = tgt.get_ptrdiff_width();
      if (ptrdiff_width < (distance.width() - distance.clrsb())) {
	code_remark remark
	  (code_remark::severity::warning,
	   "pointer difference overflows ptrdiff_t",
	   a.get_pp_result(), get_tokens_range());
	a.get_remarks().add(remark);
	return;
      }

      const mpa::limbs::size_type w = distance.width();
      distance.resize(mpa::limbs::width_to_size(ptrdiff_width));
      if (w && ptrdiff_width > w) {
	// Propagate sign bit.
	distance.set_bits_at_and_above(w, distance.test_bit(w - 1));
      }

      const mpa::limbs::size_type ptrdiff_prec = ptrdiff_width - 1;
      _set_value(target_int{ptrdiff_prec, true, std::move(distance)});
    }
  }
}

void expr_binop::_evaluate_shift(const types::int_type &it_left,
				 const types::int_type &it_right,
				 ast &a, const target &tgt)
{
  const std::shared_ptr<const int_type> it_result = it_left.promote(tgt);

  _set_type(it_result);

  if (_left.is_constexpr() && _right.is_constexpr()) {
    const constexpr_value &cv_left = _left.get_constexpr_value();
    const constexpr_value &cv_right = _right.get_constexpr_value();
    assert(cv_left.get_value_kind() == constexpr_value::value_kind::vk_int);
    assert(cv_right.get_value_kind() == constexpr_value::value_kind::vk_int);
    const target_int i_left = cv_left.convert_to(tgt, *it_result);
    const target_int &i_right = cv_right.get_int_value();

    if (i_right.is_negative()) {
      code_remark remark(code_remark::severity::fatal,
			 "negative shift distance",
			 a.get_pp_result(), _right.get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    if (!i_right.get_limbs().fits_into_type<mpa::limbs::size_type>() ||
	(i_right.get_limbs().to_type<mpa::limbs::size_type>() >=
	 i_left.width())) {
      code_remark remark(code_remark::severity::warning,
			 "shift distance exceeds integer width",
			 a.get_pp_result(), get_tokens_range());
      a.get_remarks().add(remark);
      return;
    }

    target_int i_result;
    if (_op == binary_op::lshift) {
      try {
	i_result = i_left << i_right;
      } catch(const std::overflow_error&) {
	code_remark remark(code_remark::severity::warning,
			   "integer overflow in shift operation",
			   a.get_pp_result(), get_tokens_range());
	a.get_remarks().add(remark);

	// We're in undefined behaviour land. However, gcc still
	// accepts these as constant expressions and thus, we'll have
	// to provide something here as well.
	mpa::limbs ls = i_left.get_limbs();
	const mpa::limbs::size_type distance =
	  i_right.get_limbs().to_type<mpa::limbs::size_type>();
	ls.resize(mpa::limbs::width_to_size(ls.fls() + distance));
	ls = ls.lshift(distance);
	ls.resize(mpa::limbs::width_to_size(i_left.width()));
	if (i_left.is_signed()) {
	  // Propagate sign bit to high.
	  if (i_left.width() < ls.width()) {
	    ls.set_bits_at_and_above(i_left.width(),
				     ls.test_bit(i_left.width() - 1));
	  }
	  i_result = target_int(i_left.width() - 1, true, std::move(ls));
	} else {
	  i_result = target_int(i_left.width(), false, std::move(ls));
	}
      }
    } else {
      assert(_op == binary_op::rshift);
      i_result = i_left >> i_right;
    }

    if (cv_left.has_constness(constness::c_integer_constant_expr) &&
	cv_right.has_constness(constness::c_integer_constant_expr)) {
      _set_value(constexpr_value::integer_constant_expr_tag{},
		 std::move(i_result));
    } else if ((cv_left.has_constness
		(constness::c_arithmetic_constant_expr)) &&
	       (cv_right.has_constness
		(constness::c_arithmetic_constant_expr))) {
      _set_value(constexpr_value::arithmetic_constant_expr_tag{},
		 std::move(i_result));
    } else {
      _set_value(std::move(i_result));
    }
  }
}

void expr_binop::_evaluate_bin_binop(const types::int_type &it_left,
				     const types::int_type &it_right,
				     ast &a, const target &tgt)
{
  const std::shared_ptr<const int_type> it_result
    = it_left.integer_conversion(tgt, it_right);
  _set_type(it_result);

  if (_left.is_constexpr() && _right.is_constexpr()) {
    const constexpr_value &cv_left = _left.get_constexpr_value();
    const constexpr_value &cv_right = _right.get_constexpr_value();
    assert(cv_left.get_value_kind() ==
	   constexpr_value::value_kind::vk_int);
    assert(cv_right.get_value_kind() ==
	   constexpr_value::value_kind::vk_int);
    const target_int i_left = cv_left.convert_to(tgt, *it_result);
    const target_int i_right = cv_right.convert_to(tgt, *it_result);

    target_int i_result;
    switch (_op) {
    case binary_op::mod:
      if (!i_right) {
	code_remark remark(code_remark::severity::warning,
			   "division by zero",
			   a.get_pp_result(), get_tokens_range());
	a.get_remarks().add(remark);
	return;
      }

      i_result = i_left % i_right;
      break;

    case binary_op::bin_and:
      i_result = i_left & i_right;
      break;

    case binary_op::bin_xor:
      i_result = i_left ^ i_right;
      break;

    case binary_op::bin_or:
      i_result = i_left | i_right;
      break;
    };

    if (cv_left.has_constness(constness::c_integer_constant_expr) &&
	cv_right.has_constness(constness::c_integer_constant_expr)) {
      _set_value(constexpr_value::integer_constant_expr_tag{},
		 std::move(i_result));
    } else if ((cv_left.has_constness
		(constness::c_arithmetic_constant_expr)) &&
	       (cv_right.has_constness
		(constness::c_arithmetic_constant_expr))) {
      _set_value(constexpr_value::arithmetic_constant_expr_tag{},
		 std::move(i_result));
    } else {
      _set_value(std::move(i_result));
    }
  }
}

void expr_binop::_evaluate_logical_binop(const ast &a, const target &tgt)
{
  const std::shared_ptr<const int_type> it_result
    = (!a.is_pp_expr() ?
       std_int_type::create(std_int_type::kind::k_int, true) :
       tgt.create_int_max_type(true));

  if (_left.is_constexpr()) {
    const constexpr_value &cv_left = _left.get_constexpr_value();
    const bool is_signed = it_result->is_signed(tgt);
    const mpa::limbs::size_type prec =
      it_result->get_width(tgt) - is_signed;

    if (_right.is_constexpr()) {
      const constexpr_value &cv_right = _right.get_constexpr_value();

      target_int i_result;
      switch (_op) {
      case binary_op::logical_and:
	i_result = ((!cv_left.is_zero() && !cv_right.is_zero()) ?
		    target_int::create_one(prec, is_signed) :
		    target_int::create_zero(prec, is_signed));
	break;

      case binary_op::logical_or:
	i_result = ((!cv_left.is_zero() || !cv_right.is_zero()) ?
		    target_int::create_one(prec, is_signed) :
		    target_int::create_zero(prec, is_signed));
      };

      if (cv_left.has_constness(constness::c_integer_constant_expr) &&
	  cv_right.has_constness(constness::c_integer_constant_expr)) {
	_set_value(constexpr_value::integer_constant_expr_tag{},
		   std::move(i_result));
      } else if ((cv_left.has_constness
		  (constness::c_arithmetic_constant_expr)) &&
		 (cv_right.has_constness
		  (constness::c_arithmetic_constant_expr))) {
	_set_value(constexpr_value::arithmetic_constant_expr_tag{},
		   std::move(i_result));
      } else {
	_set_value(std::move(i_result));
      }

    } else if (_op == binary_op::logical_and && cv_left.is_zero()) {
      _set_value(target_int::create_zero(prec, is_signed));

    } else if (_op == binary_op::logical_or && !cv_left.is_zero()) {
      _set_value(target_int::create_one(prec, is_signed));
    }
  }

  _set_type(std::move(it_result));
}

void expr_binop::_evaluate_cmp(const types::pointer_type &pt_left,
			       const types::pointer_type &pt_right,
			       ast &a, const target &tgt)
{
  const addressable_type &pointed_to_type_left =
    *pt_left.get_pointed_to_type();
  const addressable_type  &pointed_to_type_right =
    *pt_right.get_pointed_to_type();

  handle_types<void>
    ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
      ([](const void_type&, const void_type&) {
	 // Always Ok.
       },
       [&](const void_type&, const addressable_type&) {
	 if (_op != binary_op::eq && _op != binary_op::neq) {
	    code_remark remark
	      (code_remark::severity::warning,
	       "comparison between incompatible pointers without cast",
	       a.get_pp_result(), get_tokens_range());
	    a.get_remarks().add(remark);
	 }
       },
       [&](const addressable_type&, const void_type&) {
	 if (_op != binary_op::eq && _op != binary_op::neq) {
	    code_remark remark
	      (code_remark::severity::warning,
	       "comparison between incompatible pointers without cast",
	       a.get_pp_result(), get_tokens_range());
	    a.get_remarks().add(remark);
	 }
       },
       [&](const addressable_type &at_left, const addressable_type &at_right) {
	 if (!at_left.is_compatible_with(tgt, at_right, true)) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "comparison between incompatible pointer types",
	      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 }
       })),
     pointed_to_type_left, pointed_to_type_right);

  const std::shared_ptr<const std_int_type> it_result
    = std_int_type::create(std_int_type::kind::k_int, true);
  _set_type(it_result);

  if (!(_left.is_constexpr() && _right.is_constexpr()))
    return;

  const bool is_signed = it_result->is_signed(tgt);
  const mpa::limbs::size_type prec = it_result->get_width(tgt) - is_signed;

  const constexpr_value &cv_left = _left.get_constexpr_value();
  const constexpr_value &cv_right = _right.get_constexpr_value();

  if (cv_left.is_zero() && cv_right.is_zero()) {
    switch (_op) {
    case binary_op::eq:
      /* fall through */
    case binary_op::leq:
      /* fall through */
    case binary_op::geq:
      _set_value(target_int::create_one(prec, is_signed));
      break;

    case binary_op::neq:
      /* fall through */
    case binary_op::lt:
      /* fall through */
    case binary_op::gt:
      _set_value(target_int::create_zero(prec, is_signed));
      break;
    };

  } else if (cv_left.is_zero()) {
    // Comparison of NULL against something which is not NULL.
    switch (_op) {
    case binary_op::neq:
      /* fall through */
    case binary_op::lt:
      /* fall through */
    case binary_op::leq:
      _set_value(target_int::create_one(prec, is_signed));
      break;

    case binary_op::eq:
      /* fall through */
    case binary_op::gt:
      /* fall through */
    case binary_op::geq:
      /* fall through */
      _set_value(target_int::create_zero(prec, is_signed));
      break;
    };

  } else if (cv_right.is_zero()) {
    // Comparison of something which is not NULL against NULL.
    switch (_op) {
    case binary_op::neq:
      /* fall through */
    case binary_op::gt:
      /* fall through */
    case binary_op::geq:
      _set_value(target_int::create_one(prec, is_signed));
      break;

    case binary_op::eq:
      /* fall through */
    case binary_op::lt:
      /* fall through */
    case binary_op::leq:
      /* fall through */
      _set_value(target_int::create_zero(prec, is_signed));
      break;
    };

  } else {
    // Comparison of two non-NULL pointer values.
    const constexpr_value::address_constant &ac_left =
      cv_left.get_address_value();
    const constexpr_value::address_constant &ac_right =
      cv_right.get_address_value();

    if (ac_left.has_same_base_as(ac_right)) {
      int diff_sign = 0;
      {
	const mpa::limbs &off_left = ac_left.get_offset();
	mpa::limbs off_right = ac_right.get_offset();
	// If off_right is of the form 0b100..000, enlarge to make
	// the complement below positive.
	if (off_right && off_right.test_bit(off_right.width() - 1) &&
	    !off_right.is_any_set_below(off_right.width() - 1)) {
	  const mpa::limbs::size_type w = off_right.width();
	  off_right.resize(mpa::limbs::width_to_size(w) + 1);
	  off_right.set_bits_at_and_above(w, true);

	  const mpa::limbs diff = off_left - off_right;
	  if (diff.test_bit(diff.width() - 1))
	    diff_sign = -1;
	  else if (diff)
	    diff_sign = 1;
	}

	switch (_op) {
	case binary_op::eq:
	  if (!diff_sign)
	    _set_value(target_int::create_one(prec, is_signed));
	  else
	    _set_value(target_int::create_zero(prec, is_signed));
	  break;

	case binary_op::neq:
	  if (diff_sign)
	    _set_value(target_int::create_one(prec, is_signed));
	  else
	    _set_value(target_int::create_zero(prec, is_signed));
	  break;

	case binary_op::lt:
	  if (diff_sign < 0)
	    _set_value(target_int::create_one(prec, is_signed));
	  else
	    _set_value(target_int::create_zero(prec, is_signed));
	  break;

	case binary_op::gt:
	  if (diff_sign > 0)
	    _set_value(target_int::create_one(prec, is_signed));
	  else
	    _set_value(target_int::create_zero(prec, is_signed));
	  break;

	case binary_op::leq:
	  if (diff_sign <= 0)
	    _set_value(target_int::create_one(prec, is_signed));
	  else
	    _set_value(target_int::create_zero(prec, is_signed));
	  break;

	case binary_op::geq:
	  if (diff_sign >= 0)
	    _set_value(target_int::create_one(prec, is_signed));
	  else
	    _set_value(target_int::create_zero(prec, is_signed));
	  break;
	};
      }
    }
  }
}

void expr_binop::_evaluate_cmp(const types::pointer_type &pt_left,
			       const types::int_type &it_right,
			       ast &a, const target &tgt)
{
  const std::shared_ptr<const std_int_type> it_result
    = std_int_type::create(std_int_type::kind::k_int, true);
  _set_type(it_result);

  // The right operand must be a NULL pointer constant in integer
  // constant expression form.
  if (!_right.is_constexpr() || !_right.get_constexpr_value().is_zero()) {
    code_remark remark
      (code_remark::severity::warning,
       "pointer comparison against non-NULL integer",
       a.get_pp_result(), _right.get_tokens_range());
    a.get_remarks().add(remark);
    return;
  }

  if (!_left.is_constexpr())
    return;

  const bool is_signed = it_result->is_signed(tgt);
  const mpa::limbs::size_type prec = it_result->get_width(tgt) - is_signed;
  const constexpr_value &cv_left = _left.get_constexpr_value();

  if (cv_left.is_zero()) {
    switch (_op) {
    case binary_op::eq:
      /* fall through */
    case binary_op::leq:
      /* fall through */
    case binary_op::geq:
      _set_value(target_int::create_one(prec, is_signed));
      break;

    case binary_op::neq:
      /* fall through */
    case binary_op::lt:
      /* fall through */
    case binary_op::gt:
      _set_value(target_int::create_zero(prec, is_signed));
      break;
    };

  } else {
    // Comparison of something which is not NULL against NULL.
    switch (_op) {
    case binary_op::neq:
      /* fall through */
    case binary_op::gt:
      /* fall through */
    case binary_op::geq:
      _set_value(target_int::create_one(prec, is_signed));
      break;

    case binary_op::eq:
      /* fall through */
    case binary_op::lt:
      /* fall through */
    case binary_op::leq:
      /* fall through */
      _set_value(target_int::create_zero(prec, is_signed));
      break;
    };

  }
}

void expr_binop::_evaluate_cmp(const types::int_type &it_left,
			       const types::pointer_type &pt_right,
			       ast &a, const target &tgt)
{
  const std::shared_ptr<const std_int_type> it_result
    = std_int_type::create(std_int_type::kind::k_int, true);
  _set_type(it_result);

  // The left operand must be a NULL pointer constant in integer
  // constant expression form.
  if (!_left.is_constexpr() || !_left.get_constexpr_value().is_zero()) {
    code_remark remark
      (code_remark::severity::warning,
       "pointer comparison against non-NULL integer",
       a.get_pp_result(), _left.get_tokens_range());
    a.get_remarks().add(remark);
    return;
  }

  if (!_right.is_constexpr())
    return;

  const bool is_signed = it_result->is_signed(tgt);
  const mpa::limbs::size_type prec = it_result->get_width(tgt) - is_signed;
  const constexpr_value &cv_right = _right.get_constexpr_value();

  if (cv_right.is_zero()) {
    switch (_op) {
    case binary_op::eq:
      /* fall through */
    case binary_op::leq:
      /* fall through */
    case binary_op::geq:
      _set_value(target_int::create_one(prec, is_signed));
      break;

    case binary_op::neq:
      /* fall through */
    case binary_op::lt:
      /* fall through */
    case binary_op::gt:
      _set_value(target_int::create_zero(prec, is_signed));
      break;
    };

  } else {
    // Comparison of NULL against something which is not NULL.
    switch (_op) {
    case binary_op::neq:
      /* fall through */
    case binary_op::lt:
      /* fall through */
    case binary_op::leq:
      _set_value(target_int::create_one(prec, is_signed));
      break;

    case binary_op::eq:
      /* fall through */
    case binary_op::gt:
      /* fall through */
    case binary_op::geq:
      /* fall through */
      _set_value(target_int::create_zero(prec, is_signed));
      break;
    };
  }
}

void expr_binop::_evaluate_cmp(const types::arithmetic_type &at_left,
			       const types::arithmetic_type &at_right,
			       ast &a, const target &tgt)
{
  const std::shared_ptr<const int_type> it_result
    = (!a.is_pp_expr() ?
       std_int_type::create(std_int_type::kind::k_int, true) :
       tgt.create_int_max_type(true));
  _set_type(it_result);

  if (!(_left.is_constexpr() && _right.is_constexpr()))
    return;

  const bool is_signed = it_result->is_signed(tgt);
  const mpa::limbs::size_type prec = it_result->get_width(tgt) - is_signed;

  const constexpr_value &cv_left = _left.get_constexpr_value();
  const constexpr_value &cv_right = _right.get_constexpr_value();

  const std::shared_ptr<const arithmetic_type> at
    = [&]() {
	try {
	  return at_left.arithmetic_conversion(tgt, at_right);
	} catch (const std::range_error &re) {
	  code_remark remark
	  (code_remark::severity::fatal,
	   (std::string{
	       "could not determine common type for comparison: "
	    } +
	    re.what()),
	   a.get_pp_result(), this->get_tokens_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}
      }();

  handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const real_float_type &rft) {
	 const target_float f_left = cv_left.convert_to(tgt, rft);
	 const target_float f_right = cv_right.convert_to(tgt, rft);

	 bool result;
	 switch (_op) {
	 case binary_op::eq:
	      result = (f_left == f_right);
	      break;

	    case binary_op::neq:
	      result = (f_left != f_right);
	      break;

	    case binary_op::lt:
	      result = (f_left < f_right);
	      break;

	    case binary_op::gt:
	      result = (f_left > f_right);
	      break;

	    case binary_op::leq:
	      result = (f_left <= f_right);
	      break;

	    case binary_op::geq:
	      result = (f_left >= f_right);
	      break;
	    };

	 if (result) {
	   _set_value(target_int::create_one(prec, is_signed));

	 } else {
	   _set_value(target_int::create_zero(prec, is_signed));

	 }
       },
       [&](const int_type &it) {
	    // These don't overflow because the result type comes from
	    // an arithmetic conversion.
	    const target_int i_left = cv_left.convert_to(tgt, it);
	    const target_int i_right = cv_right.convert_to(tgt, it);

	    bool result;
	    switch (_op) {
	    case binary_op::eq:
	      result = (i_left == i_right);
	      break;

	    case binary_op::neq:
	      result = (i_left != i_right);
	      break;

	    case binary_op::lt:
	      result = (i_left < i_right);
	      break;

	    case binary_op::gt:
	      result = (i_left > i_right);
	      break;

	    case binary_op::leq:
	      result = (i_left <= i_right);
	      break;

	    case binary_op::geq:
	      result = (i_left >= i_right);
	      break;
	    };

	    target_int i_result =
	      (result ?
	       target_int::create_one(prec, is_signed) :
	       target_int::create_zero(prec, is_signed));
	    if (cv_left.has_constness(constness::c_integer_constant_expr) &&
		cv_right.has_constness(constness::c_integer_constant_expr)) {
	      _set_value(constexpr_value::integer_constant_expr_tag{},
			 std::move(i_result));
	    } else if ((cv_left.has_constness
			(constness::c_arithmetic_constant_expr)) &&
		       (cv_right.has_constness
			(constness::c_arithmetic_constant_expr))) {
	      _set_value(constexpr_value::arithmetic_constant_expr_tag{},
			 std::move(i_result));
	    } else {
	      _set_value(std::move(i_result));
	    }
       })),
     *at);
}

void expr_binop::evaluate_type(ast &a, const target &tgt)
{
  auto &&check_enum_completeness_op =
    [&](const expr &e_op) {
	 handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_op) {
		if (!et_op.is_complete()) {
		  code_remark remark
		    (code_remark::severity::fatal,
		     "binary expression's operand has incomplete enum type",
		     a.get_pp_result(), e_op.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *e_op.get_type());
    };

  _left.apply_lvalue_conversion(false);
  _right.apply_lvalue_conversion(false);

  switch(_op) {
  case binary_op::sub:
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const pointer_type &pt_left, const pointer_type &pt_right) {
	   _evaluate_ptr_sub(pt_left, pt_right, a, tgt);

	 },
	 [&](const pointer_type &pt_left, const int_type &it_right) {
	   const mpa::limbs pointed_to_size =
	     _check_pointer_arithmetic(a, pt_left, *this, tgt);

	   check_enum_completeness_op(_right);

	   _set_type(_left.get_type());

	   if (_left.is_constexpr() && _right.is_constexpr()) {
	     _set_value(_do_pointer_arithmetic(_left.get_constexpr_value(),
					       _right.get_constexpr_value(),
					       pointed_to_size, true));
	   }

	 },
	 [&](const pointer_type &pt_left, const bitfield_type&) {
	   const mpa::limbs pointed_to_size =
	     _check_pointer_arithmetic(a, pt_left, *this, tgt);

	   _set_type(_left.get_type());
	 },
	 [&](const arithmetic_type &at_left, const arithmetic_type &at_right) {
	   check_enum_completeness_op(_left);
	   check_enum_completeness_op(_right);
	   _evaluate_arith_binop(at_left, at_right, a, tgt);

	 },
	 [&](const arithmetic_type &at_left, const bitfield_type &bft_right) {
	   check_enum_completeness_op(_left);
	   _evaluate_arith_binop(at_left, *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const arithmetic_type &at_right) {
	   check_enum_completeness_op(_right);
	   _evaluate_arith_binop(*bft_left.promote(tgt), at_right, a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const bitfield_type &bft_right) {
	   _evaluate_arith_binop(*bft_left.promote(tgt),
				 *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const type&, const type&) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "invalid operand types for subtraction operator",
	      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       *_left.get_type(), *_right.get_type());
    break;

  case binary_op::add:
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const pointer_type &pt_left, const int_type &it_right) {
	   const mpa::limbs pointed_to_size =
	     _check_pointer_arithmetic(a, pt_left, *this, tgt);

	   check_enum_completeness_op(_right);

	   _set_type(_left.get_type());

	   if (_left.is_constexpr() && _right.is_constexpr()) {
	     _set_value(_do_pointer_arithmetic(_left.get_constexpr_value(),
					       _right.get_constexpr_value(),
					       pointed_to_size, false));
	   }

	 },
	 [&](const pointer_type &pt_left, const bitfield_type&) {
	   const mpa::limbs pointed_to_size =
	     _check_pointer_arithmetic(a, pt_left, *this, tgt);

	   _set_type(_left.get_type());

	 },
	 [&](const int_type &it_left, const pointer_type &pt_right) {
	   const mpa::limbs pointed_to_size =
	     _check_pointer_arithmetic(a, pt_right, *this, tgt);

	   check_enum_completeness_op(_left);

	   _set_type(_right.get_type());

	   if (_left.is_constexpr() && _right.is_constexpr()) {
	     _set_value(_do_pointer_arithmetic(_right.get_constexpr_value(),
					       _left.get_constexpr_value(),
					       pointed_to_size, false));
	   }

	 },
	 [&](const bitfield_type&, const pointer_type &pt_right) {
	   const mpa::limbs pointed_to_size =
	     _check_pointer_arithmetic(a, pt_right, *this, tgt);

	   _set_type(_right.get_type());

	 },
	 [&](const arithmetic_type &at_left, const arithmetic_type &at_right) {
	   check_enum_completeness_op(_left);
	   check_enum_completeness_op(_right);
	   _evaluate_arith_binop(at_left, at_right, a, tgt);

	 },
	 [&](const arithmetic_type &at_left, const bitfield_type &bft_right) {
	   check_enum_completeness_op(_left);
	   _evaluate_arith_binop(at_left, *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const arithmetic_type &at_right) {
	   check_enum_completeness_op(_right);
	   _evaluate_arith_binop(*bft_left.promote(tgt), at_right, a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const bitfield_type &bft_right) {
	   _evaluate_arith_binop(*bft_left.promote(tgt),
				 *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const type&, const type&) {
	   code_remark remark(code_remark::severity::fatal,
			      "invalid operand types for addition operator",
			      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       *_left.get_type(), *_right.get_type());
    break;

  case binary_op::mul:
    /* fall through */
  case binary_op::div:
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const arithmetic_type &at_left, const arithmetic_type &at_right) {
	   check_enum_completeness_op(_left);
	   check_enum_completeness_op(_right);
	   _evaluate_arith_binop(at_left, at_right, a, tgt);

	 },
	 [&](const arithmetic_type &at_left, const bitfield_type &bft_right) {
	   check_enum_completeness_op(_left);
	   _evaluate_arith_binop(at_left, *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const arithmetic_type &at_right) {
	   check_enum_completeness_op(_right);
	   _evaluate_arith_binop(*bft_left.promote(tgt), at_right, a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const bitfield_type &bft_right) {
	   _evaluate_arith_binop(*bft_left.promote(tgt),
				 *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const type&, const type&) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "invalid operand types for multiplicative operator",
	      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       *_left.get_type(), *_right.get_type());
    break;

  case binary_op::lshift:
    /* fall through */
  case binary_op::rshift:
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const int_type &it_left, const int_type &it_right) {
	   check_enum_completeness_op(_left);
	   check_enum_completeness_op(_right);
	   _evaluate_shift(it_left, it_right, a, tgt);

	 },
	 [&](const int_type &it_left, const bitfield_type &bft_right) {
	   check_enum_completeness_op(_left);
	   _evaluate_shift(it_left, *bft_right.promote(tgt), a, tgt);

	 },
	[&](const bitfield_type &bft_left, const int_type &it_right) {
	   check_enum_completeness_op(_right);
	   _evaluate_shift(*bft_left.promote(tgt), it_right, a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const bitfield_type &bft_right) {
	   _evaluate_shift(*bft_left.promote(tgt),
			   *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const type&, const type&) {
	   code_remark remark(code_remark::severity::fatal,
			      "invalid operand types for shift operator",
			      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       *_left.get_type(), *_right.get_type());
    break;

  case binary_op::mod:
    /* fall through */
  case binary_op::bin_and:
    /* fall through */
  case binary_op::bin_xor:
    /* fall through */
  case binary_op::bin_or:
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const int_type &it_left, const int_type &it_right) {
	   check_enum_completeness_op(_left);
	   check_enum_completeness_op(_right);
	   _evaluate_bin_binop(it_left, it_right, a, tgt);

	 },
	 [&](const int_type &it_left, const bitfield_type &bft_right) {
	   check_enum_completeness_op(_left);
	   _evaluate_bin_binop(it_left, *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const int_type &it_right) {
	   check_enum_completeness_op(_right);
	   _evaluate_bin_binop(*bft_left.promote(tgt), it_right, a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const bitfield_type &bft_right) {
	   _evaluate_bin_binop(*bft_left.promote(tgt),
			       *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const type&, const type&) {
	   code_remark remark(code_remark::severity::fatal,
			      "invalid operand types for binary operator",
			      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       *_left.get_type(), *_right.get_type());
    break;

  case binary_op::logical_and:
    /* fall through */
  case binary_op::logical_or:
    if (!is_scalar_type(*_left.get_type()) ||
	!is_scalar_type(*_right.get_type())) {
      code_remark remark(code_remark::severity::fatal,
			 "non-scalar operand to logical or/and",
			 a.get_pp_result(),
			 (!is_scalar_type(*_left.get_type()) ?
			  _left.get_tokens_range() :
			  _right.get_tokens_range()));
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    check_enum_completeness_op(_left);
    check_enum_completeness_op(_right);
    _evaluate_logical_binop(a, tgt);

    break;

  case binary_op::eq:
    /* fall through */
  case binary_op::neq:
    /* fall through */
  case binary_op::lt:
    /* fall through */
  case binary_op::gt:
    /* fall through */
  case binary_op::leq:
    /* fall through */
  case binary_op::geq:
    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const pointer_type &pt_left, const pointer_type &pt_right) {
	   _evaluate_cmp(pt_left, pt_right, a, tgt);

	 },
	 [&](const pointer_type &pt_left, const int_type &it_right) {
	   check_enum_completeness_op(_right);
	   _evaluate_cmp(pt_left, it_right, a, tgt);

	 },
	 [&](const pointer_type &pt_left, const bitfield_type &bft_right) {
	   _evaluate_cmp(pt_left, *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const int_type &it_left, const pointer_type &pt_right) {
	   check_enum_completeness_op(_left);
	   _evaluate_cmp(it_left, pt_right, a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const pointer_type &pt_right) {
	   _evaluate_cmp(*bft_left.promote(tgt), pt_right, a, tgt);

	 },
	 [&](const arithmetic_type &at_left, const arithmetic_type &at_right) {
	   check_enum_completeness_op(_left);
	   check_enum_completeness_op(_right);
	   _evaluate_cmp(at_left, at_right, a, tgt);

	 },
	 [&](const arithmetic_type &at_left, const bitfield_type &bft_right) {
	   check_enum_completeness_op(_left);
	   _evaluate_cmp(at_left, *bft_right.promote(tgt), a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const arithmetic_type &at_right) {
	   check_enum_completeness_op(_right);
	   _evaluate_cmp(*bft_left.promote(tgt), at_right, a, tgt);

	 },
	 [&](const bitfield_type &bft_left, const bitfield_type &bft_right) {
	   _evaluate_cmp(*bft_left.promote(tgt), *bft_right.promote(tgt),
			 a, tgt);

	 },
	 [&](const type&, const type&) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "invalid operand types for comparison operator",
	      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       *_left.get_type(), *_right.get_type());
    break;
  }
}

void expr_cast::evaluate_type(ast &a, const target &tgt)
{
  const std::shared_ptr<const addressable_type> &t_target = _tn.get_type();
  const std::shared_ptr<const type> &t_source = _e.get_type();

  auto &&check_enum_completeness_target =
    [&]() {
      handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_target) {
		if (!et_target.is_complete()) {
		  code_remark remark(code_remark::severity::fatal,
				     "cast to incomplete enum type",
				     a.get_pp_result(),
				     _tn.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *t_target);
    };

  auto &&check_enum_completeness_source =
    [&]() {
      handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_source) {
		if (!et_source.is_complete()) {
		  code_remark remark(code_remark::severity::fatal,
				     "cast from incomplete enum type",
				     a.get_pp_result(),
				     _e.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *t_source);
    };

  _e.apply_lvalue_conversion(false);

  handle_types<void>
    ((wrap_callables<no_default_action>
      ([&](const std::shared_ptr<const void_type> &vt_target,
	   const std::shared_ptr<const type>&) {
	 _set_type(vt_target);

       },
       [&](const std::shared_ptr<const int_type> &it_target,
	   const std::shared_ptr<const int_type> &it_source) {

	 check_enum_completeness_source();
	 check_enum_completeness_target();

	 _set_type(it_target);

	 if (!_e.is_constexpr())
	   return;

	 const constexpr_value &cv = _e.get_constexpr_value();
	 assert(cv.get_value_kind() == constexpr_value::value_kind::vk_int);

	 target_int i_result;
	 try {
	   i_result = cv.convert_to(tgt, *it_target);
	 } catch (const std::overflow_error&) {
	   code_remark remark(code_remark::severity::warning,
			      "integer overflow in cast",
			      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);

	   // GCC still accepts this. Do the same, interpreting the
	   // limbs as is.
	   assert(it_target->is_signed(tgt) &&
		  it_target->get_width(tgt) <= it_source->get_width(tgt));
	   mpa::limbs ls = cv.get_int_value().get_limbs();
	   const mpa::limbs::size_type width = it_target->get_width(tgt);
	   ls.resize(mpa::limbs::width_to_size(width));
	   // Propagate sign bit to high.
	   ls.set_bits_at_and_above(width, ls.test_bit(width - 1));
	   i_result = target_int(width - 1, true, std::move(ls));
	 }

	 if (cv.has_constness(constness::c_integer_constant_expr)) {
	   _set_value(constexpr_value::integer_constant_expr_tag{},
		      std::move(i_result));
	 } else if (cv.has_constness(constness::c_arithmetic_constant_expr)) {
	   _set_value(constexpr_value::arithmetic_constant_expr_tag{},
		      std::move(i_result));
	 } else {
	   _set_value(std::move(i_result));
	 }

       },
       [&](const std::shared_ptr<const int_type> &it_target,
	   const std::shared_ptr<const real_float_type> &ft_source) {
	 check_enum_completeness_target();

	 _set_type(it_target);

	 if (!_e.is_constexpr())
	   return;

	 const constexpr_value &cv = _e.get_constexpr_value();
	 assert(cv.get_value_kind() == constexpr_value::value_kind::vk_float);

	 target_int i_result;
	 try {
	   i_result = cv.convert_to(tgt, *it_target);
	 } catch (const std::overflow_error&) {
	   code_remark remark(code_remark::severity::warning,
			      "integer overflow in cast",
			      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	   return;
	 }

	 if (cv.has_constness(constness::c_literal_fp_constant)) {
	   _set_value(constexpr_value::integer_constant_expr_tag{},
		      std::move(i_result));
	 } else if (cv.has_constness(constness::c_arithmetic_constant_expr)) {
	   _set_value(constexpr_value::arithmetic_constant_expr_tag{},
		      std::move(i_result));
	 } else {
	   _set_value(std::move(i_result));
	 }

       },
       [&](const std::shared_ptr<const int_type> &it_target,
	   const std::shared_ptr<const arithmetic_type>&) {
	 check_enum_completeness_target();

	 _set_type(it_target);

       },
       [&](const std::shared_ptr<const int_type> &it_target,
	   const std::shared_ptr<const pointer_type> &pt_source) {
	 check_enum_completeness_target();

	 _set_type(it_target);

	 if (!_e.is_constexpr())
	   return;

	 const constexpr_value &cv = _e.get_constexpr_value();
	 assert(cv.get_value_kind() == constexpr_value::value_kind::vk_address);
	 const constexpr_value::address_constant &ac = cv.get_address_value();

	 assert(!ac.is_dereferenced());
	 if (ac.get_base_kind() !=
	     constexpr_value::address_constant::base_kind::bk_constant) {
	   return;
	 }

	 const mpa::limbs::size_type width = it_target->get_width(tgt);
	 const bool is_signed = it_target->is_signed(tgt);
	 mpa::limbs offset = ac.get_offset();

	 if (is_signed && (offset.width() - offset.clrsb()) > width) {
	   code_remark remark(code_remark::severity::warning,
			      "integer overflow in cast",
			      a.get_pp_result(), get_tokens_range());
	   a.get_remarks().add(remark);
	 } else {
	   offset.resize(mpa::limbs::width_to_size(width));
	   if (is_signed)
	     offset.set_bits_at_and_above(width, offset.test_bit(width - 1));
	   else
	     offset.set_bits_at_and_above(width, false);

	   _set_value(target_int(width - is_signed, is_signed,
				 std::move(offset)));
	 }

       },
       [&](const std::shared_ptr<const real_float_type> &rft_target,
	   const std::shared_ptr<const arithmetic_type> &at_source) {
	 check_enum_completeness_source();

	 _set_type(rft_target);

	 if (!_e.is_constexpr())
	   return;

	 const constexpr_value &cv = _e.get_constexpr_value();
	 assert(cv.get_value_kind() == constexpr_value::value_kind::vk_int ||
		cv.get_value_kind() == constexpr_value::value_kind::vk_float);

	 target_float f_result = cv.convert_to(tgt, *rft_target);
	 if (cv.has_constness(constness::c_arithmetic_constant_expr)) {
	   _set_value(constexpr_value::arithmetic_constant_expr_tag{},
		      std::move(f_result));
	 } else {
	   _set_value(std::move(f_result));
	 }

       },
       [&](const std::shared_ptr<const complex_float_type> &cft_target,
	   const std::shared_ptr<const arithmetic_type> &at_source) {
	 check_enum_completeness_source();

	 _set_type(cft_target);

       },
       [&](const std::shared_ptr<const arithmetic_type> &at_target,
	   const std::shared_ptr<const bitfield_type>&) {

	 check_enum_completeness_source();

	 _set_type(at_target);

       },
       [&](const std::shared_ptr<const pointer_type> &pt_target,
	   const std::shared_ptr<const int_type> &it_source) {
	 check_enum_completeness_source();

	 _set_type(pt_target);

	 if (!_e.is_constexpr())
	   return;

	 const constexpr_value &cv = _e.get_constexpr_value();
	 assert(cv.get_value_kind() == constexpr_value::value_kind::vk_int);

	if (cv.has_constness(constness::c_integer_constant_expr)) {
	  _set_value(constexpr_value::address_constant_tag{},
		     constexpr_value::address_constant{cv.get_int_value()});
	} else {
	  _set_value(constexpr_value::address_constant{cv.get_int_value()});
	}

       },
       [&](const std::shared_ptr<const pointer_type> &pt_target,
	   const std::shared_ptr<const bitfield_type>&) {
	 check_enum_completeness_source();

	 _set_type(pt_target);

       },
       [&](const std::shared_ptr<const pointer_type> &pt_target,
	   const std::shared_ptr<const pointer_type> &pt_source) {
	 _set_type(pt_target);

	 if (!_e.is_constexpr())
	   return;

	 const constexpr_value &cv = _e.get_constexpr_value();
	 assert(cv.get_value_kind() == constexpr_value::value_kind::vk_address);

	 _set_value(cv.clone());

       },
       [&](const std::shared_ptr<const type>&,
	   const std::shared_ptr<const type>&) {
	 code_remark remark(code_remark::severity::warning,
			    "invalid types in cast",
			    a.get_pp_result(), get_tokens_range());
	 a.get_remarks().add(remark);
	 _set_type(t_target);
       })),
     t_target, t_source);
}


void expr_label_addr::evaluate_type(ast&, const target&)
{
  assert(_resolved);
  _set_type(void_type::create()->derive_pointer());
  _set_value(constexpr_value::address_constant_tag{},
	     constexpr_value::address_constant{*_resolved, false});
}


void expr_unop_pre::evaluate_type(ast &a, const target &tgt)
{
  const std::shared_ptr<const type> &t = _e.get_type();

  auto &&check_enum_completeness_op =
    [&]() {
	 handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_op) {
		if (!et_op.is_complete()) {
		  code_remark remark
		    (code_remark::severity::fatal,
		     "unary expression's operand has incomplete enum type",
		     a.get_pp_result(), _e.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *t);
    };

  switch(_op) {
  case unary_op_pre::inc:
    /* fall through */
  case unary_op_pre::dec:
    _e.apply_lvalue_conversion(true);

    if (!_e.is_lvalue()) {
      code_remark remark
	(code_remark::severity::fatal,
	 "non-lvalue operand to increment/decrement operator",
	 a.get_pp_result(), _e.get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const arithmetic_type&) {
	   check_enum_completeness_op();
	   _set_type(t);

	 },
	 [&](const bitfield_type &bft) {
	   _set_type(t);

	 },
	 [&](const pointer_type &pt) {
	   _check_pointer_arithmetic(a, pt, _e, tgt);
	   _set_type(t->strip_qualifiers());

	 },
	 [&](const type&) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "invalid operand to increment/decrement operator",
	      a.get_pp_result(), _e.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       *t);

    break;

  case unary_op_pre::addr:
    if (!_e.is_lvalue()) {
      code_remark remark(code_remark::severity::fatal,
			 "non-lvalue operand to address operator",
			 a.get_pp_result(), _e.get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const addressable_type &at) {
	   _set_type(at.derive_pointer());

	   if (_e.is_dereferenced_const_addr()) {
	     const constexpr_value &cv = _e.get_constexpr_value();
	     auto v = cv.clone();
	     v->get_address_value().undereference();
	     _set_value(std::move(v));
	   }

	 },
	 [&](const type&) {
	   code_remark remark(code_remark::severity::fatal,
			      "operand's type is not addressable",
			      a.get_pp_result(), _e.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       *t);

    break;

  case unary_op_pre::deref:
    _e.apply_lvalue_conversion(false);

    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const pointer_type &pt) {
	   std::shared_ptr<const addressable_type> pointed_to_type =
	     pt.get_pointed_to_type();

	   bool is_local_label_ptr_deref = false;
	   if (!pointed_to_type->is_complete() &&
	       !is_type<array_type>(*pointed_to_type)) {
	     // Could still be a valid
	     //   goto *&&local_label
	     // statement
	     is_local_label_ptr_deref =
	       (is_type<void_type>(*pointed_to_type) &&
		this->get_parent()->is_any_of<stmt_goto>());

	     if (!is_local_label_ptr_deref) {
	       code_remark remark(code_remark::severity::warning,
				  "dereferencing pointer to incomplete type",
				  a.get_pp_result(), _e.get_tokens_range());
	       a.get_remarks().add(remark);
	     }
	   }

	   if (_e.is_constexpr()) {
	     const constexpr_value &cv = _e.get_constexpr_value();
	     assert(!cv.get_address_value().is_dereferenced());
	     auto v = cv.clone();
	     v->get_address_value().dereference();
	     _set_value(std::move(v));
	   }

	   _set_type(std::move(pointed_to_type));
	   _set_lvalue(!is_local_label_ptr_deref);
	 },
	 [&](const type &t) {
	   // As an (undocumented!) extension, GCC allows computed gotos
	   // with integer targets. Accept that too, but warn.
	   if ((is_type<int_type>(t) ||
		is_type<bitfield_type>(t)) &&
	       this->get_parent()->is_any_of<stmt_goto>()) {
	     code_remark remark
	       (code_remark::severity::warning,
		"dereferencing integer at computed goto",
		a.get_pp_result(), _e.get_tokens_range());
	     a.get_remarks().add(remark);

	     check_enum_completeness_op();

	     _set_type(void_type::create());

	   } else {
	     code_remark remark
	       (code_remark::severity::fatal,
		"dereferencing something which is not a pointer",
		a.get_pp_result(), _e.get_tokens_range());
	     a.get_remarks().add(remark);
	     throw semantic_except(remark);
	   }
	 })),
       *t);

    break;

  case unary_op_pre::plus:
    /* fall through */
  case unary_op_pre::neg:
    _e.apply_lvalue_conversion(false);

    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const std::shared_ptr<const int_type> &it) {
	   check_enum_completeness_op();

	   std::shared_ptr<const int_type> it_result = it->promote(tgt);

	   if (_e.is_constexpr()) {
	     const constexpr_value &cv = _e.get_constexpr_value();
	     assert(cv.get_value_kind() ==
		    constexpr_value::value_kind::vk_int);
	     target_int i_result = cv.convert_to(tgt, *it_result);
	     if (_op == unary_op_pre::neg)
	       i_result = -i_result;

	     if (cv.has_constness(constness::c_integer_constant_expr)) {
	       _set_value(constexpr_value::integer_constant_expr_tag{},
			  std::move(i_result));
	     } else if ((cv.has_constness
			 (constness::c_arithmetic_constant_expr))) {
	       _set_value(constexpr_value::arithmetic_constant_expr_tag{},
			  std::move(i_result));
	     } else {
	       _set_value(std::move(i_result));
	     }
	   }

	   _set_type(std::move(it_result));

	 },
	 [&](const std::shared_ptr<const bitfield_type> &bft) {
	   _set_type(bft->promote(tgt));

	 },
	 [&](std::shared_ptr<const real_float_type> &&ft) {
	   if (_e.is_constexpr()) {
	     const constexpr_value &cv = _e.get_constexpr_value();
	     assert(cv.get_value_kind() ==
		    constexpr_value::value_kind::vk_float);
	     target_float f_result = cv.get_float_value();
	     if (_op == unary_op_pre::neg)
	       f_result = -f_result;

	     if (cv.has_constness(constness::c_arithmetic_constant_expr)) {
	       _set_value(constexpr_value::arithmetic_constant_expr_tag{},
			  std::move(f_result));
	     } else {
	       _set_value(std::move(f_result));
	     }
	   }

	   _set_type(std::move(ft));

	 },
	 [&](std::shared_ptr<const arithmetic_type> &&at) {
	   _set_type(std::move(at));

	 },
	 [&](const std::shared_ptr<const type>&) {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "unary +/- operand applied to non-arithmetic type",
	      a.get_pp_result(), _e.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       t);

    break;

  case unary_op_pre::bin_neg:
    _e.apply_lvalue_conversion(false);

    handle_types<void>
      ((wrap_callables<no_default_action>
	([&](const std::shared_ptr<const int_type> &it) {
	   check_enum_completeness_op();

	   std::shared_ptr<const int_type> it_result = it->promote(tgt);

	   if (_e.is_constexpr()) {
	     const constexpr_value &cv = _e.get_constexpr_value();
	     assert(cv.get_value_kind() ==
		    constexpr_value::value_kind::vk_int);
	     target_int i_result = cv.convert_to(tgt, *it_result);
	     i_result = ~i_result;

	     if (cv.has_constness(constness::c_integer_constant_expr)) {
	       _set_value(constexpr_value::integer_constant_expr_tag{},
			  std::move(i_result));
	     } else if ((cv.has_constness
			 (constness::c_arithmetic_constant_expr))) {
	       _set_value(constexpr_value::arithmetic_constant_expr_tag{},
			  std::move(i_result));
	     } else {
	       _set_value(std::move(i_result));
	     }
	   }

	   _set_type(std::move(it_result));

	 },
	 [&](const std::shared_ptr<const bitfield_type> &bft) {
	   _set_type(bft->promote(tgt));

	 },
	 [&](const std::shared_ptr<const complex_float_type> &cft) {
	   // As an extension, GCC allows application of the '~' on
	   // complex floats which results in their complex
	   // conjugation.
	   _set_type(cft);

	 },
	 [&](const std::shared_ptr<const type>&) {
	   code_remark remark(code_remark::severity::fatal,
			      "binary negation applied to non-integer type",
			      a.get_pp_result(), _e.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       t);

    break;

  case unary_op_pre::logical_not:
    _e.apply_lvalue_conversion(false);

    if (!is_scalar_type(*t)) {
      code_remark remark(code_remark::severity::fatal,
			 "logical not applied to non-integer type",
			 a.get_pp_result(), _e.get_tokens_range());
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    check_enum_completeness_op();

    {
      const std::shared_ptr<const int_type> it_result
	= (!a.is_pp_expr() ?
	   std_int_type::create(std_int_type::kind::k_int, true) :
	   tgt.create_int_max_type(true));

      if (_e.is_constexpr()) {
	const constexpr_value &cv = _e.get_constexpr_value();
	const mpa::limbs::size_type prec = it_result->get_width(tgt) - 1;
	target_int i_result;
	if (cv.is_zero())
	  i_result = target_int::create_one(prec, true);
	else
	  i_result = target_int::create_zero(prec, true);

	if (cv.has_constness(constness::c_integer_constant_expr)) {
	  _set_value(constexpr_value::integer_constant_expr_tag{},
		     std::move(i_result));
	} else if (cv.has_constness(constness::c_arithmetic_constant_expr)) {
	  _set_value(constexpr_value::arithmetic_constant_expr_tag{},
		     std::move(i_result));
	} else {
	  _set_value(std::move(i_result));
	}
      }

      _set_type(std::move(it_result));
    }
    break;

  }
}


static target_int _limbs_to_size_t(const target &tgt, mpa::limbs &&ls)
{
  const mpa::limbs::size_type width = tgt.get_ptrdiff_width();
  ls.resize(mpa::limbs::width_to_size(width));
  ls.set_bits_at_and_above(width, false);
  return target_int{width, false, std::move(ls)};
}


void expr_sizeof_expr::evaluate_type(ast &a, const target &tgt)
{
    handle_types<void>
      ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
	([&](const object_type &ot) {
	   if (!ot.is_complete()) {
	     code_remark remark(code_remark::severity::fatal,
				"sizeof applied to incomplete type",
				a.get_pp_result(), _e.get_tokens_range());
	     a.get_remarks().add(remark);
	     throw semantic_except(remark);
	   }

	   _set_type(tgt.create_ptrdiff_type(false));

	   if (ot.is_size_constant()) {
	     _set_value(constexpr_value::integer_constant_expr_tag{},
			_limbs_to_size_t(tgt, ot.get_size(tgt)));
	   }

	 },
	 [&](const function_type &ft) {
	   // GCC extension: sizeof(function type) == 1
	   _set_type(tgt.create_ptrdiff_type(false));
	   _set_value(constexpr_value::integer_constant_expr_tag{},
		      target_int::create_one(tgt.get_ptrdiff_width(), false));

	 },
	 [&](const void_type&) {
	   // GCC extension: sizeof(void) == 1
	   _set_type(tgt.create_ptrdiff_type(false));
	   _set_value(constexpr_value::integer_constant_expr_tag{},
		      target_int::create_one(tgt.get_ptrdiff_width(), false));

	 },
	 [&](const type&) {
	   code_remark remark(code_remark::severity::fatal,
			      "sizeof applied to invalid type",
			      a.get_pp_result(), _e.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
       *_e.get_type());
}


void expr_sizeof_type_name::evaluate_type(ast &a, const target &tgt)
{
    handle_types<void>
      ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
	([&](const object_type &ot) {
	   if (!ot.is_complete()) {
	     code_remark remark(code_remark::severity::fatal,
				"sizeof applied to incomplete type",
				a.get_pp_result(), _tn.get_tokens_range());
	     a.get_remarks().add(remark);
	     throw semantic_except(remark);
	   }

	   _set_type(tgt.create_ptrdiff_type(false));

	   if (ot.is_size_constant()) {
	     _set_value(constexpr_value::integer_constant_expr_tag{},
			_limbs_to_size_t(tgt, ot.get_size(tgt)));
	   }

	 },
	 [&](const function_type &ft) {
	   // GCC extension: sizeof(function type) == 1
	   _set_type(tgt.create_ptrdiff_type(false));
	   _set_value(constexpr_value::integer_constant_expr_tag{},
		      target_int::create_one(tgt.get_ptrdiff_width(), false));

	 },
	 [&](const void_type&) {
	   // GCC extension: sizeof(void) == 1
	   _set_type(tgt.create_ptrdiff_type(false));
	   _set_value(constexpr_value::integer_constant_expr_tag{},
		      target_int::create_one(tgt.get_ptrdiff_width(), false));

	 },
	 [&](const type&) {
	   code_remark remark(code_remark::severity::fatal,
			      "sizeof applied to invalid type",
			      a.get_pp_result(), _tn.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
       *_tn.get_type());
}


void expr_alignof_expr::evaluate_type(ast &a, const target &tgt)
{
  const mpa::limbs::size_type align_log2 =
    handle_types<mpa::limbs::size_type>
      ((wrap_callables<default_action_unreachable<mpa::limbs::size_type,
						  type_set<> >::type>
	([&](const array_type &at) {
	   if (!at.is_complete() && !at.get_element_type()->is_complete()) {
	     code_remark remark(code_remark::severity::fatal,
				"alignof applied to incomplete type",
				a.get_pp_result(), _e.get_tokens_range());
	     a.get_remarks().add(remark);
	     throw semantic_except(remark);
	   }

	   return at.get_effective_alignment(tgt);

	 },
	 [&](const object_type &ot) {
	   if (!ot.is_complete()) {
	     code_remark remark(code_remark::severity::fatal,
				"alignof applied to incomplete type",
				a.get_pp_result(), _e.get_tokens_range());
	     a.get_remarks().add(remark);
	     throw semantic_except(remark);
	   }

	   return ot.get_effective_alignment(tgt);

	 },
	 [&](const function_type &ft) {
	   // GCC extension: __alignof__(function type) == 1
	   return 0;

	 },
	 [&](const void_type&) {
	   // GCC extension: __alignof__(void) == 1
	   return 0;

	 },
	 [&](const type&) -> mpa::limbs::size_type {
	   code_remark remark(code_remark::severity::fatal,
			      "alignof applied to invalid type",
			      a.get_pp_result(), _e.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
       *_e.get_type());

  _set_type(tgt.create_ptrdiff_type(false));

  mpa::limbs align;
  align.resize(mpa::limbs::width_to_size(align_log2 + 1));
  align.set_bit(align_log2, true);
  _set_value(constexpr_value::integer_constant_expr_tag{},
	     _limbs_to_size_t(tgt, std::move(align)));
}


void expr_alignof_type_name::evaluate_type(ast &a, const target &tgt)
{
  const mpa::limbs::size_type align_log2 =
    handle_types<mpa::limbs::size_type>
      ((wrap_callables<default_action_unreachable<mpa::limbs::size_type,
						  type_set<> >::type>
	([&](const array_type &at) {
	   if (!at.is_complete() && !at.get_element_type()->is_complete()) {
	     code_remark remark(code_remark::severity::fatal,
				"alignof applied to incomplete type",
				a.get_pp_result(), _tn.get_tokens_range());
	     a.get_remarks().add(remark);
	     throw semantic_except(remark);
	   }

	   return at.get_effective_alignment(tgt);

	 },
	 [&](const object_type &ot) {
	   if (!ot.is_complete()) {
	     code_remark remark(code_remark::severity::fatal,
				"alignof applied to incomplete type",
				a.get_pp_result(), _tn.get_tokens_range());
	     a.get_remarks().add(remark);
	     throw semantic_except(remark);
	   }

	   return ot.get_effective_alignment(tgt);

	 },
	 [&](const function_type &ft) {
	   // GCC extension: __alignof__(function type) == 1
	   return 0;

	 },
	 [&](const void_type&) {
	   // GCC extension: __alignof__(void) == 1
	   return 0;

	 },
	 [&](const type&) -> mpa::limbs::size_type {
	   code_remark remark(code_remark::severity::fatal,
			      "alignof applied to invalid type",
			      a.get_pp_result(), _tn.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);
	 })),
       *_tn.get_type());

  _set_type(tgt.create_ptrdiff_type(false));

  mpa::limbs align;
  align.resize(mpa::limbs::width_to_size(align_log2 + 1));
  align.set_bit(align_log2, true);
  _set_value(constexpr_value::integer_constant_expr_tag{},
	     _limbs_to_size_t(tgt, std::move(align)));
}


void expr_generic::evaluate_type(ast &a, const target &tgt)
{
  _ctrl_e.apply_lvalue_conversion(false);

  const type &ctrl_t = *_ctrl_e.get_type();
  const generic_association *match = nullptr;
  const generic_association *def = nullptr;
  _gal.for_each
    ([&](const generic_association &ga) {
      const type_name * const tn = ga.get_type_name();

      if (!tn) {
	if (def) {
	  code_remark remark
	    (code_remark::severity::fatal,
	     "multiple defaults in _Generic()",
	     a.get_pp_result(), ga.get_tokens_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	def = &ga;
	return;
      }

      if (!tn->get_type()->is_complete()) {
	  code_remark remark
	    (code_remark::severity::fatal,
	     "incomplete type in _Generic() selection",
	     a.get_pp_result(), tn->get_tokens_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
      }

      if (!ctrl_t.is_compatible_with(tgt, *tn->get_type(), false))
	return;

      if (match) {
	  code_remark remark
	    (code_remark::severity::fatal,
	     "multiple matches in _Generic()",
	     a.get_pp_result(), ga.get_tokens_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
      }

      match = &ga;
    });

  if (!match && def) {
    match = def;
  } else if (!match) {
    code_remark remark
      (code_remark::severity::fatal, "no matches in _Generic()",
       a.get_pp_result(), this->get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }


  const expr &e = match->get_expr();
  _match = &e;
  if (e.is_constexpr() || e.is_dereferenced_const_addr())
    _set_value(e.get_constexpr_value().clone());
  _set_type(e.get_type());
  _set_lvalue(e.is_lvalue());
}


void expr_builtin_choose_expr::evaluate_type(ast &a, const target &tgt)
{
  // Note: GCC really requires an integer constant expression
  // for the first argument.
  if (!_cond.is_constexpr() ||
      !(_cond.get_constexpr_value().has_constness
	(constexpr_value::constness::c_integer_constant_expr))) {
    code_remark remark
      (code_remark::severity::warning,
       "first argument to __builtin_choose_expr() is not a constant",
       a.get_pp_result(), _cond.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const constexpr_value &cv_cond = _cond.get_constexpr_value();
  const expr &e_result = cv_cond.is_zero() ? _expr_false : _expr_true;
  _set_type(e_result.get_type());
  _set_lvalue(e_result.is_lvalue());
  if (e_result.is_constexpr())
    _set_value(e_result.get_constexpr_value().clone());
}


void expr_builtin_offsetof::evaluate_type(ast &a, const target &tgt)
{
  const addressable_type *t_base = _tn.get_type().get();
  constexpr_value::address_constant ac;
  bool offset_is_const = true;
  bool offset_is_integer_constant_expr = true;

  _member_designator.for_each
    (wrap_callables<default_action_nop>
     ([&](const offset_member_designator::member &m) {
	if (m.ptr_base) {
	  t_base =
	    handle_types<const addressable_type *>
	    ((wrap_callables<default_action_nop>
	      ([&](const array_type &at_base) {
		 return at_base.get_element_type().get();
	       },
	       [&](const type&) -> const addressable_type* {
		 code_remark remark
		   (code_remark::severity::fatal,
		    "'->' style member designator applied to non-array type",
		    a.get_pp_result(), m.member_tok);
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       })),
	     *t_base);
	}

	handle_types<void>
	  ((wrap_callables<default_action_nop>
	    ([&](const struct_or_union_type &sout_base) {
	       if (!sout_base.is_complete()) {
		 code_remark remark
		   (code_remark::severity::fatal,
		    "member dereference in incomplete struct or union",
		    a.get_pp_result(), m.member_tok);
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       }

	       const struct_or_union_content::lookup_result lr =
		 (sout_base.get_content()
		  ->lookup(a.get_pp_tokens()[m.member_tok].get_value()));
	       if (lr.empty()) {
		 code_remark remark(code_remark::severity::fatal,
				       "member lookup failed",
				       a.get_pp_result(), m.member_tok);
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       }

	       const struct_or_union_content::member &sou_m = *lr.back();
	       t_base =
		 handle_types<const addressable_type *>
		   ((wrap_callables<default_action_nop>
		     ([&](const addressable_type &at) {
			return &at;
		      },
		      [&](const type&) -> const addressable_type* {
			code_remark remark
			  (code_remark::severity::fatal,
			   "taking address of non-addressable member",
			   a.get_pp_result(), m.member_tok);
			a.get_remarks().add(remark);
			throw semantic_except(remark);

		      })),
		    *sou_m.get_type());

	       if (offset_is_const && sou_m.has_constant_offset()) {
		 for (auto c : lr) {
		   const mpa::limbs &offset = c->get_offset();
		   const mpa::limbs &bitpos = c->get_bitpos();
		   assert(!bitpos || !bitpos.is_any_set_below(3));
		   ac.add_to_offset(offset + bitpos.rshift(3, false));
		 }
	       } else {
		 offset_is_const = false;
		 offset_is_integer_constant_expr = false;
	       }
	     },
	     [&](const type&) {
	       code_remark remark
		 (code_remark::severity::fatal,
		  "member dereference in something which is not a struct/union",
		  a.get_pp_result(), m.member_tok);
	       a.get_remarks().add(remark);
	       throw semantic_except(remark);

	     })),
	   *t_base);
      },
      [&](const expr &e_index) {
	const object_type &ot_element =
	  handle_types<const object_type&>
	    ((wrap_callables<default_action_nop>
	      ([&](const array_type &at_base) -> const object_type& {
		 return *at_base.get_element_type();
	       },
	       [&](const type&) -> const object_type& {
		 code_remark remark
		   (code_remark::severity::fatal,
		    "array dereference designator applied to non-array type",
		    a.get_pp_result(), e_index.get_tokens_range());
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       })),
	     *t_base);

	if (!is_type<int_type>(*e_index.get_type()) &&
	    !is_type<bitfield_type>(*e_index.get_type())) {
	  code_remark remark
	    (code_remark::severity::fatal,
	     "array index expression doesn't have integer type",
	     a.get_pp_result(), e_index.get_tokens_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	handle_types<void>
	  ((wrap_callables<default_action_nop>
	    ([&](const enum_type &et_index) {
	       if (!et_index.is_complete()) {
		 code_remark remark
		   (code_remark::severity::fatal,
		    "array index expression has incomplete enum type",
		    a.get_pp_result(), e_index.get_tokens_range());
		 a.get_remarks().add(remark);
		 throw semantic_except(remark);
	       }
	     })),
	   *e_index.get_type());

	t_base = &ot_element;

	if (offset_is_const && e_index.is_constexpr()) {
	  const constexpr_value &cv_index = e_index.get_constexpr_value();
	  ac.add_to_offset(false, cv_index.get_int_value(),
			   ot_element.get_size(tgt));
	  if (!cv_index.has_constness(constness::c_integer_constant_expr))
	    offset_is_integer_constant_expr = false;

	} else {
	  offset_is_const = false;
	  offset_is_integer_constant_expr = false;

	}
      }));

  _set_type(tgt.create_ptrdiff_type(false));

  if (offset_is_const) {
    mpa::limbs offset = ac.get_offset();
    const mpa::limbs::size_type width = tgt.get_ptrdiff_width();
    offset.resize(mpa::limbs::width_to_size(width));
    offset.set_bits_at_and_above(width, false);
    target_int result{width, false, std::move(offset)};
    if (offset_is_integer_constant_expr) {
      _set_value(constexpr_value::integer_constant_expr_tag{},
		 std::move(result));
    } else {
      _set_value(std::move(result));
    }
  }
}


void expr_builtin_types_compatible_p::evaluate_type(ast&,
						    const target &tgt)
{
  _set_type(std_int_type::create(std_int_type::kind::k_int, false));

  const mpa::limbs::size_type prec
    = tgt.get_std_int_width(std_int_type::kind::k_int) - 1;
  target_int i_result;
  if (_tn1.get_type()->is_compatible_with(tgt, *_tn2.get_type(), true))
    i_result = target_int::create_one(prec, true);
  else
    i_result = target_int::create_zero(prec, true);
  _set_value(constexpr_value::integer_constant_expr_tag{},
	     std::move(i_result));
}


void expr_builtin_va_arg::evaluate_type(ast&, const target&)
{
  _set_type(_tn.get_type());
}


void expr_array_subscript::_evaluate_type(ast &a, const target &tgt,
					  const pointer_type &pt_base,
					  const expr &e_base,
					  const expr &e_index)
{
  const mpa::limbs pointed_to_size =
    _check_pointer_arithmetic(a, pt_base, e_base, tgt);

  if (pointed_to_size && e_base.is_constexpr() && e_index.is_constexpr()) {
    const constexpr_value &cv_base = e_base.get_constexpr_value();
    assert(!cv_base.get_address_value().is_dereferenced());
    const constexpr_value &cv_index = e_index.get_constexpr_value();

    constexpr_value::address_constant ac = cv_base.get_address_value();
    ac.add_to_offset(false, cv_index.get_int_value(), pointed_to_size);
    ac.dereference();

    if (cv_base.has_constness(constness::c_address_constant) &&
	cv_index.has_constness(constness::c_integer_constant_expr)) {
      _set_value(constexpr_value::address_constant_tag{},
		 std::move(ac));
    } else {
      _set_value(std::move(ac));
    }
  }

  _set_lvalue(true);
  _set_type(pt_base.get_pointed_to_type());
}

void expr_array_subscript::evaluate_type(ast &a, const target &tgt)
{
  auto &&check_enum_completeness_index =
    [&](const expr &e_index) {
      handle_types<void>
	((wrap_callables<default_action_nop>
	  ([&](const enum_type &et_index) {
	     if (!et_index.is_complete()) {
	       code_remark remark
		 (code_remark::severity::fatal,
		  "array index expression has incomplete enum type",
		  a.get_pp_result(), e_index.get_tokens_range());
	       a.get_remarks().add(remark);
	       throw semantic_except(remark);
	     }
	   })),
	 *e_index.get_type());
    };

  _base.apply_lvalue_conversion(false);
  _index.apply_lvalue_conversion(false);

  handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const pointer_type &pt_base, const int_type&) {
	 check_enum_completeness_index(_index);
	 _evaluate_type(a, tgt, pt_base, _base, _index);
       },
       [&](const pointer_type &pt_base, const bitfield_type&) {
	 check_enum_completeness_index(_index);
	 _evaluate_type(a, tgt, pt_base, _base, _index);
       },
       [&](const int_type&, const pointer_type &pt_base) {
	 check_enum_completeness_index(_base);
	 _evaluate_type(a, tgt, pt_base, _index, _base);
       },
       [&](const bitfield_type&, const pointer_type &pt_base) {
	 check_enum_completeness_index(_base);
	 _evaluate_type(a, tgt, pt_base, _index, _base);
       },
       [&](const type&, const type&) {
	 code_remark remark(code_remark::severity::fatal,
			    "invalid types for array subscript operator",
			    a.get_pp_result(), get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       })),
     *_base.get_type(), *_index.get_type());

}

void expr_func_invocation::evaluate_type(ast &a, const target &tgt)
{
  _func.apply_lvalue_conversion(false);

  handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const pointer_type &pt_func) {
	 const std::size_t n_args = !_args ? 0 : _args->size();

	 if (_args)
	   _args->apply_lvalue_conversions(false);

	 handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const prototyped_function_type &pft) {
		const parameter_type_list &ptl = pft.get_parameter_type_list();

		if (n_args < ptl.size()) {
		  code_remark remark
		    (code_remark::severity::fatal,
		     "too few arguments in function invocation",
		     a.get_pp_result(), get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);

		} else if (!pft.is_variadic() && n_args > ptl.size()) {
		  code_remark remark
		    (code_remark::severity::fatal,
		     "too many arguments in function invocation",
		     a.get_pp_result(), get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
		for(std::size_t i = 0; i < ptl.size(); ++i) {
		  check_types_assignment(a, tgt, *ptl[i], (*_args)[i], true);
		}

		_set_type(pft.get_return_type());

	      },
	      [&](const unprototyped_function_type &upft) {
		if (upft.is_from_definition()) {
		  // I don't see a reason why, but gcc silently
		  // accepts passing a different number of arguments
		  // than indicated by the function definition's
		  // parameter identifier list.
		  if (n_args != upft.get_n_args()) {
		    code_remark remark
		      (code_remark::severity::warning,
		       "wrong number of arguments to function invocation",
		       a.get_pp_result(), get_tokens_range());
		    a.get_remarks().add(remark);
		  }
		}

		_set_type(upft.get_return_type());

	      },
	      [&](const type&) {
		code_remark remark
		  (code_remark::severity::fatal,
		   "function call on something which is not a function pointer",
		   a.get_pp_result(), _func.get_tokens_range());
		a.get_remarks().add(remark);
		throw semantic_except(remark);
	      })),
	    *pt_func.get_pointed_to_type());

       },
       [&](const builtin_func_type &bft) {
	 auto bf = bft.get_builtin_func_factory()();
	 auto result = bf->evaluate(a, tgt, *this);
	 _set_type(std::move(result.type));
	 if (result.value)
	   _set_value(std::move(result.value));
	 if (result.is_lvalue)
	   _set_lvalue(true);

       },
       [&](const type&) {
	 code_remark remark
	   (code_remark::severity::fatal,
	    "function call on something which is not a function",
	    a.get_pp_result(), _func.get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);

       })),
     *_func.get_type());
}


void expr_member_deref::evaluate_type(ast &a, const target&)
{
  const type &t_base
    = [&]() -> const type& {
	switch (_deref_type) {
	case member_deref_type::ptr_base:
	  _base.apply_lvalue_conversion(false);

	  return
	    handle_types<const addressable_type &>
	      ((wrap_callables<default_action_nop>
		([&](const pointer_type &pt) -> const addressable_type& {
		   return *pt.get_pointed_to_type();
		 },
		 [&](const type&) -> const addressable_type& {
		   code_remark remark
		     (code_remark::severity::fatal,
		      "application of -> member deref operator to non-pointer",
		      a.get_pp_result(), _base.get_tokens_range());
		   a.get_remarks().add(remark);
		   throw semantic_except(remark);
		 })),
	       *_base.get_type());

	case member_deref_type::non_ptr_base:
	  _base.apply_lvalue_conversion(true);

	  return *_base.get_type();
	}
      }();

  const struct_or_union_type &sout_base =
    handle_types<const struct_or_union_type&>
      ((wrap_callables<default_action_nop>
	([&](const struct_or_union_type &_sout_base)
				-> const struct_or_union_type& {
	   return _sout_base;
	 },
	 [&](const type&) -> const struct_or_union_type& {
	   code_remark remark
	     (code_remark::severity::fatal,
	      "application of member dereference operator to non-struct/union",
	      a.get_pp_result(), _base.get_tokens_range());
	   a.get_remarks().add(remark);
	   throw semantic_except(remark);

	 })),
       t_base);

  if (!sout_base.is_complete()) {
    code_remark remark
      (code_remark::severity::fatal,
       "application of member dereference operator to incomplete type",
       a.get_pp_result(), _base.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  // Look it up.
  const struct_or_union_content::lookup_result lr =
    (sout_base.get_content()->lookup
     (a.get_pp_tokens()[_member_tok].get_value()));
  if (lr.empty()) {
    code_remark remark(code_remark::severity::fatal,
		       "member not found in struct or union",
		       a.get_pp_result(), _member_tok);
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const struct_or_union_content::member &m = *lr.back();
  const std::shared_ptr<const type> &t = m.get_type();

  if (m.has_constant_offset() &&
      ((_deref_type == member_deref_type::ptr_base && _base.is_constexpr()) ||
       _base.is_dereferenced_const_addr())) {
  handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const addressable_type &) {
	 // Careful: with
	 //   static struct { char a; } b[1];
	 // &b->a is an address constant, but with
	 //   static struct { char a; } * b;
	 // it is not. Disambiguate by the address_constant's
	 // ->is_dereferenced(), c.f. expr::apply_lvalue_conversion().
	 const constexpr_value &cv_base = _base.get_constexpr_value();
	 constexpr_value::address_constant ac = cv_base.get_address_value();
	 if (_deref_type != member_deref_type::ptr_base ||
	     !ac.is_dereferenced()) {

	   for (auto c : lr) {
	     const mpa::limbs &offset = c->get_offset();
	     const mpa::limbs &bitpos = c->get_bitpos();
	     assert(!bitpos || !bitpos.is_any_set_below(3));
	     ac.add_to_offset(offset + bitpos.rshift(3, false));
	   }

	   switch (_deref_type) {
	   case member_deref_type::ptr_base:
	     ac.dereference();
	     break;

	   case member_deref_type::non_ptr_base:
	     assert(ac.is_dereferenced());
	     break;
	   }

	   if (cv_base.has_constness(constness::c_address_constant)) {
	     _set_value(constexpr_value::address_constant_tag{},
			std::move(ac));
	   } else {
	     _set_value(std::move(ac));
	   }
	 }
       })),
     *t);
  }

  _set_type(t->amend_qualifiers(sout_base.get_qualifiers()));
  switch (_deref_type) {
  case member_deref_type::ptr_base:
    _set_lvalue(true);
    break;

  case member_deref_type::non_ptr_base:
    _set_lvalue(_base.is_lvalue());
    break;
  }
}


void expr_unop_post::evaluate_type(ast &a, const target &tgt)
{
  _e.apply_lvalue_conversion(true);

  const std::shared_ptr<const type> &t = _e.get_type();
  if (!_e.is_lvalue()) {
    code_remark remark(code_remark::severity::fatal,
		       "non-lvalue operand to increment/decrement operator",
		       a.get_pp_result(), _e.get_tokens_range());
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](std::shared_ptr<const arithmetic_type> &&at) {
	 handle_types<void>
	   ((wrap_callables<default_action_nop>
	     ([&](const enum_type &et_op) {
		if (!et_op.is_complete()) {
		  code_remark remark
		    (code_remark::severity::fatal,
		     "unary expression's operand has incomplete enum type",
		     a.get_pp_result(), _e.get_tokens_range());
		  a.get_remarks().add(remark);
		  throw semantic_except(remark);
		}
	      })),
	    *at);

	 _set_type(std::move(at));
       },
       [&](const std::shared_ptr<const bitfield_type> &bft) {
	 _set_type(bft);
       },
       [&](const std::shared_ptr<const pointer_type> &pt) {
	 _check_pointer_arithmetic(a, *pt, _e, tgt);
	 _set_type(pt->strip_qualifiers());
       },
       [&](const std::shared_ptr<const type>&) {
	 code_remark remark
	   (code_remark::severity::fatal,
	    "invalid operand to increment/decrement operator",
	    a.get_pp_result(), _e.get_tokens_range());
	 a.get_remarks().add(remark);
	 throw semantic_except(remark);
       })),
     t);
}


void expr_compound_literal::evaluate_type(ast &a, const target &tgt)
{
  // Only compound literals which are at file scope have static
  // storage duration and thus, can be used in the creation of address
  // constants.
  bool is_at_file_scope = true;
  for (const _ast_entity *p = this->get_parent(); p;
       p && (p = p->get_parent())) {
    p->process<void, type_set<function_definition>>
      (wrap_callables<default_action_nop>
       ([&](const function_definition&) {
	 is_at_file_scope = false;
	 p = nullptr;
	}));
  }

  if (is_at_file_scope) {
    _set_value(constexpr_value::address_constant_tag{},
	       constexpr_value::address_constant(*this, true));
  }

  std::shared_ptr<const array_type> t =
    _evaluate_init(a, tgt, *_tn.get_type(), _il);
  if (static_cast<bool>(t)) {
    _set_type(std::move(t));
  } else {
    _set_type(_tn.get_type());
  }

  _set_lvalue(true);
}


void expr_statement::evaluate_type(ast&, const target&)
{
  const block_item_list * const bil = _s.get_block_item_list();
  if (!bil) {
    _set_type(void_type::create());
    return;
  }

  // Find the last statement in the block item list
  const block_item_stmt *bis = nullptr;
  bil->for_each_reverse
    (wrap_callables<default_action_return_value<bool, true>::type>
     ([&bis](const block_item_stmt &bi) {
	bis = &bi;
	return false;
     }));

  if (!bis) {
    _set_type(void_type::create());
    return;
  }

  // And get its type, if any.
  const stmt *s = &bis->get_stmt();
  bool is_stmt_labeled;
  do {
    is_stmt_labeled =
      (s->process<bool, type_set<stmt_expr, stmt_labeled>>
       (wrap_callables<default_action_return_value<bool, false>::type>
	([&](const stmt_labeled &sl) {
	   s = &sl.get_stmt();
	   return true;
	 },
	 [this](const stmt_expr &se) {
	   const expr * const e = se.get_expr();
	   if (!e)
	     return false;
	   _set_type(e->get_type());
	   if (e->is_constexpr()) {
	     auto cv = e->get_constexpr_value().clone();
	     cv->drop_constness();
	     _set_value(std::move(cv));
	   }
	   return false;
	 })));
  } while (is_stmt_labeled);

  if (!this->is_evaluated())
    _set_type(void_type::create());
}


void expr_id::evaluate_type(ast &a, const target &tgt)
{
  switch(_resolved.get_kind()) {
  case resolved::resolved_kind::none:
    // This can only happen if the identifier is an operand in an
    // attribute's expression or it's a function call's function
    // designator to an undeclared function.
    // Ignore the former case and handle the latter.
    // Strictly speaking, this is invalid in C99, but gcc accepts it.
    {
      const _ast_entity * const non_parens_parent =
	this->get_non_parens_parent();
      if (non_parens_parent &&
	  non_parens_parent->is_any_of<expr_func_invocation>()) {
	_set_type(std_int_type::create(std_int_type::kind::k_int, true)
		  ->derive_function(false)
		  ->derive_pointer());
      }
    }
    break;

  case resolved::resolved_kind::builtin_func:
    _set_type(builtin_func_type::create(_resolved.get_builtin_func_factory()));
    break;

  case resolved::resolved_kind::builtin_var:
    {
      builtin_var::evaluation_result_type er
	= _resolved.get_builtin_var_factory()()->evaluate(a, tgt, *this);
      _set_type(std::move(er.type));
      _set_lvalue(er.is_lvalue);
    }
    break;

  case resolved::resolved_kind::init_declarator:
    {
      const init_declarator &id = _resolved.get_init_declarator();
      const direct_declarator_id &ddid =
	id.get_declarator().get_direct_declarator_id();

      // Static storage duration objects can become
      // address constants (by taking their address).
      const declaration &d = id.get_containing_declaration();
      const linkage::linkage_kind linkage_kind
	= id.get_linkage().get_linkage_kind();
      if (linkage_kind == linkage::linkage_kind::internal ||
	  linkage_kind == linkage::linkage_kind::external ||
	  linkage_kind == linkage::linkage_kind::nested_fun_auto ||
	  (d.get_declaration_specifiers().get_storage_class(a) ==
	   storage_class::sc_static)) {
	_set_value(constexpr_value::address_constant_tag{},
		   constexpr_value::address_constant{id, true});
      }

      _set_type(ddid.get_type());
      _set_lvalue(true);
    }
    break;

  case resolved::resolved_kind::parameter_declaration_declarator:
    {
      const parameter_declaration_declarator &pdd
	= _resolved.get_parameter_declaration_declarator();
      const direct_declarator_id &ddid =
	pdd.get_declarator().get_direct_declarator_id();
      _set_type(ddid.get_type());
      _set_lvalue(true);
    }
    break;

  case resolved::resolved_kind::function_definition:
    {
      const function_definition &fd = _resolved.get_function_definition();
      const direct_declarator_id &ddid =
	fd.get_declarator().get_direct_declarator_id();

      // Function definitions always imply static storage duration
      _set_value(constexpr_value::address_constant_tag{},
		 constexpr_value::address_constant{fd, true});

      _set_type(ddid.get_type());
      _set_lvalue(true);
    }
    break;

  case resolved::resolved_kind::stmt_labeled:
    _set_value(constexpr_value::address_constant_tag{},
	       constexpr_value::address_constant{_resolved.get_stmt_labeled(),
						 true});
    _set_type(void_type::create());
    break;

  case resolved::resolved_kind::enumerator:
    {
      const enum_content::member &em = _resolved.get_enumerator().to_member(a);
      _set_value(constexpr_value::integer_constant_expr_tag{}, em.get_value());
      _set_type(em.get_type());
    }
    break;

  case resolved::resolved_kind::in_param_id_list:
    // An identifier can get resolved to an parameter id list member
    // only if there is no corresponding declaration in the declarator
    // id list. This is invalid in C99, but gcc accepts it and
    // defaults to int.
    _set_type(std_int_type::create(std_int_type::kind::k_int, true));
    _set_lvalue(true);
    break;
  };
}


void expr_constant::evaluate_type(ast &a, const target &tgt)
{
  const pp_token &val_tok = a.get_pp_tokens()[_const_tok];
  const std::string &val = val_tok.get_value();


  if (val_tok.get_type() != pp_token::type::pp_number) {
    const pp_token::type tok_type = val_tok.get_type();
    assert(tok_type == pp_token::type::chr ||
	   tok_type == pp_token::type::wchr ||
	   tok_type == pp_token::type::uchr16 ||
	   tok_type == pp_token::type::uchr32);

    target::execution_charset_encoding encoding;
    switch(tok_type) {
    case pp_token::type::chr:
      encoding = target::execution_charset_encoding::ecse_char;
      break;

    case pp_token::type::wchr:
      encoding = target::execution_charset_encoding::ecse_wchar;
      break;

    case pp_token::type::uchr16:
      encoding = target::execution_charset_encoding::ecse_char16;
      break;

    case pp_token::type::uchr32:
      encoding = target::execution_charset_encoding::ecse_char32;
      break;

    default:
      assert(0);
      __builtin_unreachable();
    }

    const std::unique_ptr<execution_charset_encoder> encoder =
      tgt.get_execution_charset_encoder(encoding);

    const std::vector<mpa::limbs> encoded = encoder->encode_string(a, _const_tok);
    if (!encoded.size()) {
      code_remark remark(code_remark::severity::fatal,
			 "empty character constant",
			 a.get_pp_result(), _const_tok);
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    // target (wide) character type signedness:
    // - char: implementation defined
    // - wchar_t: implementation defined
    // - char16_t: uint_least16_t -- unsigned
    // - char32_t: uint_least32_t -- unsigned
    bool target_char_is_signed = false;
    if (encoding == target::execution_charset_encoding::ecse_char)
      target_char_is_signed = tgt.is_char_signed();
    else if (encoding == target::execution_charset_encoding::ecse_wchar)
      target_char_is_signed = tgt.is_wchar_signed();

    const types::std_int_type::kind target_char_kind =
      encoder->get_target_char_kind();
    const mpa::limbs::size_type target_char_width =
      tgt.get_std_int_width(target_char_kind);

    if (encoding == target::execution_charset_encoding::ecse_char) {
      // non-wide char: the resulting type is int. Furthemore, the
      // value of multi-character character constants is
      // implementation defined, c.f. the GNU cpp manual, sec. 11.1
      // "Implementation defined behavior.
      mpa::limbs value;
      const std::shared_ptr<const int_type> it_result
	= (!a.is_pp_expr() ?
	   std_int_type::create(std_int_type::kind::k_int, true) :
	   tgt.create_int_max_type(true));
      const mpa::limbs::size_type target_int_width = it_result->get_width(tgt);
      if (encoded.size() == 1) {
	value = encoded[0];
	value.resize(mpa::limbs::width_to_size(target_int_width));
	if (target_char_is_signed) {
	  // Propagate sign bit to high.
	  assert(target_char_width <= target_int_width);
	  value.set_bits_at_and_above(target_char_width,
				      value.test_bit(target_char_width - 1));
	}

	_set_value(constexpr_value::integer_constant_expr_tag{},
		   target_int(target_int_width - 1, true, std::move(value)));

      } else {
	// Multi-character constant: we're in implementation defined
	// behavior land, do what GNU cpp does.
	value.resize(encoded.size() * target_char_width);
	for (mpa::limbs::size_type i = 0; i < encoded.size(); ++i) {
	  value = value.lshift(target_char_width);
	  assert(!encoded[i].is_any_set_at_or_above(target_char_width));
	  value = value | encoded[i];
	}

	if (value.is_any_set_at_or_above(target_int_width)) {
	  code_remark remark(code_remark::severity::fatal,
			     "integer overflow in multi-character constant",
			     a.get_pp_result(), _const_tok);
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	value.resize(mpa::limbs::width_to_size(target_int_width));
	_set_value(constexpr_value::integer_constant_expr_tag{},
		   target_int(target_int_width - 1, true, std::move(value)));
      }

      _set_type(std::move(it_result));
      return;

    } else {
      // Wide-character
      if (encoded.size() != 1) {
	code_remark remark(code_remark::severity::fatal,
			   "wide character constant too long for its type",
			   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      std::shared_ptr<const int_type> it_result
	= (!a.is_pp_expr() ?
	   std_int_type::create(target_char_kind, target_char_is_signed) :
	   tgt.create_int_max_type(target_char_is_signed));
      const mpa::limbs::size_type target_int_width = it_result->get_width(tgt);

      mpa::limbs value = encoded[0];
      value.resize(mpa::limbs::width_to_size(target_int_width));
      if (target_char_is_signed) {
	// Propagate sign to high.
	assert(target_char_width <= target_int_width);
	value.set_bits_at_and_above(target_char_width,
				    value.test_bit(target_char_width - 1));
      }

      _set_value(constexpr_value::integer_constant_expr_tag{},
		 target_int(target_int_width - target_char_is_signed,
			    target_char_is_signed, std::move(value)));
      _set_type(std::move(it_result));
      return;
    }

    assert(0);
    __builtin_unreachable();
  }


  // Not a character constant => either a floating or an integer constant.
  enum class base
  {
    dec = 10,
    hex = 16,
    oct = 8,
    bin = 2,
  } b = base::dec;

  std::string::const_iterator val_begin = val.begin();
  if (val.size() >= 2 && val[0] == '0') {
    switch (val[1]) {
    case 'x':
      /* fall through */
    case 'X':
      b = base::hex;
      val_begin += 2;
      break;

    case 'b':
      /* fall through */
    case 'B':
      b = base::bin;
      val_begin += 2;
      break;

    default:
      if (val[1] != 'u' && val[1] != 'U' && val[1] != 'l' && val[1] != 'L' &&
	  val.find('.') == std::string::npos) {
	b = base::oct;
	val_begin += 1;
      }
      break;
    }
  }

  std::string::const_iterator it_dot =
    std::find(val_begin, val.end(), '.');

  std::string::const_iterator it_flt_exp = val.end();
  if (it_dot != val.end() || val_begin != val.end()) {
    if (b == base::dec) {
      it_flt_exp =
	std::find_if(it_dot != val.end() ? it_dot + 1 : val_begin + 1,
		     val.end(),
		     [](const char c) {
		       return c == 'e' || c == 'E';
		     });
    } else if (b == base::hex) {
      it_flt_exp =
	std::find_if(it_dot != val.end() ? it_dot + 1 : val_begin + 1,
		     val.end(),
		     [](const char c) {
		       return c == 'p' || c == 'P';
		     });
    }
  }

  if (it_dot != val.end() || it_flt_exp != val.end()) {
    // Floating point constant
    if (a.is_pp_expr()) {
      code_remark remark
	(code_remark::severity::fatal,
	 "floating point constant in preprocessor expression",
	 a.get_pp_result(), _const_tok);
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    if (b == base::hex && it_flt_exp == val.end()) {
      code_remark remark(code_remark::severity::fatal,
			 "hexadecimal floating constant requires exponent",
			 a.get_pp_result(), _const_tok);
      a.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    std_float_type::kind k = std_float_type::kind::k_double;
    std::string::const_iterator it_flt_suffix = val.end();
    switch (val.back()) {
    case 'f':
      /* fall through */
    case 'F':
      k = std_float_type::kind::k_float;
      --it_flt_suffix;
      break;

    // As an extension, GCC accepts 'd' and 'D' for doubles.
    case 'd':
      /* fall through */
    case 'D':
      k = std_float_type::kind::k_double;
      --it_flt_suffix;
      break;

    case 'l':
      /* fall through */
    case 'L':
      k = std_float_type::kind::k_long_double;
      --it_flt_suffix;
      break;
    }

    // Parse the exponent
    mpa::limbs e;
    if (it_flt_exp != val.end()) {
      std::string::const_iterator it = it_flt_exp + 1;

      if (it == it_flt_suffix) {
	code_remark remark(code_remark::severity::fatal,
			   "invalid floating constant exponent format",
			   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      bool exp_is_negative = false;
      switch (*it) {
      case '+':
	++it;
	break;

      case '-':
	exp_is_negative = true;
	++it;
	break;
      }

      if (it == it_flt_suffix) {
	code_remark remark(code_remark::severity::fatal,
			   "invalid floating constant exponent format",
			   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      try {
	e = mpa::limbs::from_string(it, it_flt_suffix, mpa::limb{10});
      } catch(const std::range_error&) {
	code_remark remark
	  (code_remark::severity::fatal,
	   "unrecognized digit in floating constant's exponent",
	   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      // Enlarge e to make the sign bit zero, if necessary.
      if (e.test_bit(e.width() - 1))
	e.resize(e.size() + 1);

      if (exp_is_negative)
	e = e.complement();

    } else {
      // For convenience below
      it_flt_exp = it_flt_suffix;

    }

    if (it_dot != val.end()) {
      assert(it_dot < it_flt_exp);

      if (it_dot == val_begin && it_dot + 1 == it_flt_exp) {
	// Just a single dot with no integer and no fractional part
	code_remark remark(code_remark::severity::fatal,
			   "invalid floating constant format",
			   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }
    } else {
      // For convenience below
      it_dot = it_flt_exp;
    }

    // Parse the significand, integer part first.
    mpa::limbs m;
    if (it_dot != val_begin) {
      try {
	m = (mpa::limbs::from_string
	     (val_begin, it_dot,
	      mpa::limb{static_cast<mpa::limb::limb_type>(b)}));
      } catch(const std::range_error&) {
	code_remark remark
	  (code_remark::severity::fatal,
	   "unrecognized digit in floating constant's integer part",
	   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }
    }

    assert(it_dot <= it_flt_exp);
    if (it_flt_exp - it_dot > 1) {
      const std::string::const_iterator it_frac = it_dot + 1;
      const mpa::limbs frac_len =
	(mpa::limbs::from_size_type
	 (static_cast<std::string::size_type>(it_flt_exp - it_frac)));

      assert(frac_len);
      // Multiply m, if any, by b^frac_len and subtract frac_len from e in
      // order to account for that.
      e = e - frac_len;
      if (m) {
	// Calculate v = b^frac_len
	const mpa::limbs _b{static_cast<mpa::limb::limb_type>(b)};
	mpa::limbs v = _b;
	for (mpa::limbs::size_type i = frac_len.fls() - 1; i > 0; --i) {
	  v = v * v;
	  if (frac_len.test_bit(i - 1))
	    v = v * _b;
	}

	m = m * v;
      }

      // Parse the fractional part and add it to m.
      mpa::limbs frac;
      try {
	frac = (mpa::limbs::from_string
		(it_frac, it_flt_exp,
		 mpa::limb{static_cast<mpa::limb::limb_type>(b)}));
      } catch(const std::range_error&) {
	code_remark remark
	  (code_remark::severity::fatal,
	   "unrecognized digit in floating constant's fractional part",
	   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      m = m + frac;
    }


    const real_float_type::format &fmt = tgt.get_std_float_format(k);
    assert(b == base::hex || b == base::dec);
    target_float flt =
      (b == base::hex ?
       (target_float::from_base2_exp(fmt.significand_width, fmt.exponent_width,
				     std::move(m), std::move(e))) :
       (target_float::from_base10_exp(fmt.significand_width, fmt.exponent_width,
				      std::move(m), std::move(e))));

    _set_value(constexpr_value::literal_fp_constant_tag{}, std::move(flt));
    _set_type(std_float_type::create(k));
    return;
  }

  // it's an integer constant
  bool is_unsigned = false;
  std_int_type::kind k = std_int_type::std_int_type::kind::k_int;
  std::string::const_iterator it_int_suffix;
  for (it_int_suffix = val.end(); it_int_suffix != val_begin; --it_int_suffix) {
    bool last = false;

    switch(*(it_int_suffix - 1)) {
    case 'l':
      if (k != std_int_type::kind::k_int) {
	code_remark remark
	  (code_remark::severity::fatal, "invalid integer constant suffix",
	   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      if (it_int_suffix - 1 != val_begin && *(it_int_suffix - 2) == 'l') {
	--it_int_suffix;
	k = std_int_type::kind::k_long_long;
      } else {
	k = std_int_type::kind::k_long;
      }

      break;

    case 'L':
      if (k != std_int_type::kind::k_int) {
	code_remark remark
	  (code_remark::severity::fatal, "invalid integer constant suffix",
	   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      if (it_int_suffix - 1 != val_begin && *(it_int_suffix - 2) == 'L') {
	--it_int_suffix;
	k = std_int_type::kind::k_long_long;
      } else {
	k = std_int_type::kind::k_long;
      }

      break;

    case 'u':
      /* fall through */
    case 'U':
      if (is_unsigned) {
	code_remark remark
	  (code_remark::severity::fatal, "invalid integer constant suffix",
	   a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }
      is_unsigned = true;

      break;

    default:
      last = true;
      break;
    }

    if (last)
      break;
  }

  if (val_begin == it_int_suffix) {
    code_remark remark
      (code_remark::severity::fatal, "invalid integer constant format",
       a.get_pp_result(), _const_tok);
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  mpa::limbs m;
  try {
    m = (mpa::limbs::from_string(val_begin, it_int_suffix,
			 mpa::limb{static_cast<mpa::limb::limb_type>(b)}));
  } catch (const std::range_error&) {
    code_remark remark (code_remark::severity::fatal,
			"unrecognized digit in integer constant",
			a.get_pp_result(), _const_tok);
    a.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  // Determine the resulting type, c.f. C99, 6.4.4.1(5).
  const mpa::limbs::size_type req_width =
    (!is_unsigned && b == base::dec) ? m.fls() + 1 : m.fls();
  mpa::limbs::size_type width = tgt.get_std_int_width(k);
  std::shared_ptr<const types::int_type> t;
  if (!a.is_pp_expr() && width >= req_width) {
    if (!is_unsigned && b != base::dec && m.is_any_set_at_or_above(width - 1))
      is_unsigned = true;
    t = types::std_int_type::create(k, !is_unsigned);

  } else {
    if (!a.is_pp_expr()) {
      try {
	t = tgt.width_to_int_type(req_width, !is_unsigned, true);
      } catch (const std::overflow_error &) {
	code_remark remark (code_remark::severity::fatal,
			    "integer constant overflow",
			    a.get_pp_result(), _const_tok);
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }
    } else {
      t = tgt.create_int_max_type(true);
    }

    width = t->get_width(tgt);
    if (!is_unsigned && b != base::dec &&
	m.is_any_set_at_or_above(width - 1)) {
      is_unsigned = true;
      t = t->to_unsigned();
    }
  }

  assert(is_unsigned || !m.is_any_set_at_or_above(width - 1));
  m.resize(mpa::limbs::width_to_size(width));
  target_int i(width - !is_unsigned, !is_unsigned, std::move(m));
  _set_value(constexpr_value::integer_constant_expr_tag{}, std::move(i));
  _set_type(std::move(t));
  return;
}


void expr_string_literal::evaluate_type(ast &a, const target &tgt)
{
  auto &&string_tok_type_to_ecse =
    [](const pp_token::type tok_type) {
      switch (tok_type) {
      case pp_token::type::str:
	return target::execution_charset_encoding::ecse_char;

      case pp_token::type::wstr:
	return target::execution_charset_encoding::ecse_wchar;

      case pp_token::type::ustr8:
	return target::execution_charset_encoding::ecse_char8;

      case pp_token::type::ustr16:
	return target::execution_charset_encoding::ecse_char16;

      case pp_token::type::ustr32:
	return target::execution_charset_encoding::ecse_char32;
      }
      assert(0);
      __builtin_unreachable();
    };

  // First check that all the string tokens' encodings are the same.
  // If there is any encoding prefix, all string literals w/o such one
  // are treated as if they had that prefix.
  target::execution_charset_encoding ecse =
    target::execution_charset_encoding::ecse_char;
  for (auto tok_index : _sl.get_strings()) {
    const pp_token &tok = a.get_pp_tokens()[tok_index];
    const target::execution_charset_encoding cur_ecse =
      string_tok_type_to_ecse(tok.get_type());

    if (cur_ecse == target::execution_charset_encoding::ecse_char) {
      continue;

    } else if (ecse == target::execution_charset_encoding::ecse_char) {
      ecse = cur_ecse;

    } else if (ecse != cur_ecse) {
      code_remark remark(code_remark::severity::fatal,
			 "incompatible encodings in string concatenation",
			 a.get_pp_result(), tok_index);
      a.get_remarks().add(remark);
      throw semantic_except(remark);

    }
  }

  const std::unique_ptr<execution_charset_encoder> encoder =
    tgt.get_execution_charset_encoder(ecse);
  std::shared_ptr<const object_type> element_type;
  if (ecse == target::execution_charset_encoding::ecse_char ||
      ecse == target::execution_charset_encoding::ecse_char8) {
    element_type = plain_char_type::create();

  } else {
    bool target_char_is_signed = false;
    if (ecse == target::execution_charset_encoding::ecse_wchar)
      target_char_is_signed = tgt.is_wchar_signed();

    const types::std_int_type::kind target_char_kind =
      encoder->get_target_char_kind();

    element_type = std_int_type::create(target_char_kind,
					target_char_is_signed);

  }

  // Finally, count the number of encoded characters
  mpa::limbs length{1};
  for (auto tok_index : _sl.get_strings()) {
    const auto cur_length = encoder->encode_string(a, tok_index).size();
    length = length + mpa::limbs::from_size_type(cur_length);
  }

  _set_type(element_type->derive_array(std::move(length)));
  _set_value(constexpr_value::address_constant_tag{},
	     constexpr_value::address_constant{_sl, true});
  _set_lvalue(true);
}


void expr_parenthesized::evaluate_type(ast&, const target&)
{
  if (_e.is_constexpr() || _e.is_dereferenced_const_addr())
    _set_value(_e.get_constexpr_value().clone());
  _set_type(_e.get_type());
  _set_lvalue(_e.is_lvalue());
}

void static_assertion::test_assertion(ast &a, const target &tgt) const
{
  if (!_cond.is_constexpr()) {
    code_remark remark
      (code_remark::severity::warning,
       "_Static_assert() condition does't evaluate to constant expression",
       a.get_pp_result(), get_tokens_range());
    a.get_remarks().add(remark);
    return;
  }

  const constexpr_value &cv = _cond.get_constexpr_value();
  if (cv.is_zero()) {
    code_remark remark
      (code_remark::severity::warning,
       "_Static_assert() condition failure",
       a.get_pp_result(), get_tokens_range());
    a.get_remarks().add(remark);
    return;
  }
}
