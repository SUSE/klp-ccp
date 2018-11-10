#include <cassert>
#include <set>
#include <string>
#include "ast_impl.hh"
#include "semantic_except.hh"

using namespace suse::cp;
using namespace suse::cp::ast;

using resolved_kind = expr_id::resolved::resolved_kind;

static const char * const _builtin_tdids[] = {
	"__builtin_va_list",
	nullptr
};

static std::set<std::string>
_init_builtin_ids_set(const char * const builtin_ids[])
{
  std::set<std::string> s;

  for (const char * const *b = builtin_ids; *b; ++b)
    s.insert(*b);

  return s;
}

void stmt_compound::register_label(stmt_labeled &label)
{
  _labels.push_back(std::ref(label));
}

stmt_labeled* stmt_compound::lookup_label(const ast &ast,
					  const pp_token_index &label_tok)
  noexcept
{
  const pp_token &label = ast.get_pp_tokens()[label_tok];

  for (auto l : _labels) {
    if (ast.get_pp_tokens()[l.get().get_label_tok()].get_value() ==
	label.get_value()) {
      return &l.get();
    }
  }

  return nullptr;
}

bool stmt_compound::is_local_label(const ast &ast,
				   const pp_token_index &label_tok) noexcept
{
  if (!_lldl)
    return false;

  const pp_token &label = ast.get_pp_tokens()[label_tok];
  for (auto &lld : (_lldl-> get_local_label_declarations())) {
    for (auto id_tok : lld.get().get_identifier_list().get_identifiers()) {
      if (ast.get_pp_tokens()[id_tok].get_value() == label.get_value())
	    return true;
    }
  }

  return false;
}


void ast_translation_unit::_register_labels()
{
  auto &&stmt_labeled_handler
    = (wrap_callables<default_action_nop>
       ([this](stmt_labeled &sl) {
	 const pp_token_index label_tok = sl.get_label_tok();
	 stmt_compound *registrar = nullptr;

	 for (_ast_entity *p = sl.get_parent(); p;
	      p && (p = p->get_parent())) {
	   p->process<void, type_set<stmt_compound,
				     function_definition,
				     translation_unit>>
	     (wrap_callables<default_action_nop>
	      ([&](stmt_compound &sc) {
		registrar = &sc;

		if (sc.lookup_label(*this, label_tok)) {
		  code_remark remark
		    (code_remark::severity::fatal,
		     "label redefined",
		     get_pp_tokens()[label_tok].get_file_range());
		  get_remarks().add(remark);
		  throw semantic_except(remark);
		}

		// If this label is declared local to this block, finish the
		// search here.
		if (sc.is_local_label(*this, label_tok))
		  p = nullptr;
	      },
	      [&](const function_definition&) {
		// Don't let labels escape function definitions.
		p = nullptr;
	      },
	      [&](const translation_unit&) {
		// Label is from an expr_statement which is not part
		// of a function definition.
		p = nullptr;
		registrar = nullptr;
	      }));
	 }

	 if (registrar)
	   registrar->register_label(sl);
	 }));

  this->for_each_dfs_po<type_set<stmt_labeled> >(stmt_labeled_handler);
}


const declaration& init_declarator::get_containing_declaration() const noexcept
{
  return (get_unique_parent<init_declarator_list>()
	  .get_unique_parent<declaration>());
}


const _ast_entity& direct_declarator::get_first_non_declarator_parent()
  const noexcept
{
  const _ast_entity *p;
  for (p = get_parent(); p; p = p->get_parent()) {
    if (!p->is_any_of<declarator, direct_declarator>())
      break;
  }

  assert(p);
  return *p;
}


namespace
{
  class _id_resolver
  {
  public:
    _id_resolver(ast_translation_unit &ast);

    void operator()();

  private:
    void _enter_scope();
    void _leave_scope();

    const expr_id::resolved* _lookup_id(const pp_token_index id_tok,
					bool *in_cur_scope = nullptr)
      const noexcept;

    template <typename predicate_type>
    const expr_id::resolved* _lookup_id(const pp_token_index id_tok,
					predicate_type &&pred,
					bool *in_cur_scope = nullptr)
      const noexcept;

    const sou_decl_link* _lookup_sou_decl(const pp_token_index id_tok,
					  const bool only_in_cur_scope,
					  const bool skip_cur_scope)
      const noexcept;

    enum_def* _lookup_enum_def(const pp_token_index id_tok,
			       const bool only_in_cur_scope)
      const noexcept;

    void _handle_init_decl(init_declarator &id);
    void _handle_param_decl(parameter_declaration_declarator &pdd);
    void _handle_fun_def(function_definition &fd);
    void _handle_sou_ref(struct_or_union_ref &sour);
    void _handle_sou_def(struct_or_union_def &soud);
    void _handle_enum_def(enum_def &ed);
    void _handle_enum_ref(enum_ref &er);

    static linkage::linkage_kind
    _get_linkage_kind(const expr_id::resolved &resolved) noexcept;
    static void _link_to(linkage &l, const expr_id::resolved &resolved)
      noexcept;
    void _add_pending_linkage(init_declarator &id);
    void _try_resolve_pending_linkages(init_declarator &id,
				       const linkage::linkage_kind kind);
    void _try_resolve_pending_linkages(function_definition &fd,
				       const linkage::linkage_kind kind);

    void _resolve_id(expr_id &ei);
    void _resolve_id(expr_label_addr &ela);
    void _resolve_id(type_specifier_tdid &ts_tdid);

    ast_translation_unit &_ast;

    struct _scope
    {
      std::vector<expr_id::resolved> _declared_ids;
      std::vector<sou_decl_link> _declared_sous;
      std::vector<std::reference_wrapper<enum_def> > _defined_enums;
    };
    typedef std::vector<expr_id::resolved> _scope_type;
    typedef std::vector<_scope> _scopes_type;
    _scopes_type _scopes;

    std::vector<std::reference_wrapper<init_declarator> >
      _pending_linkages;
  };
}

_id_resolver::_id_resolver(ast_translation_unit &ast)
  : _ast(ast)
{
  _enter_scope();
}

static bool _is_fundef_ddf(const direct_declarator_func &ddf)
{
  // Check that the first non-declarator/-direct_declarator parent is
  // a function_definition instance.
  return
    ddf.get_first_non_declarator_parent().is_any_of<function_definition>();
}

static bool _is_fundef_ddf_pdl(const parameter_declaration_list &pdl)
  noexcept
{
  const _ast_entity * const p = pdl.get_parent();

  if (p->is_any_of<direct_abstract_declarator_func>()) {
    // A parameter in an abstract func declarator cannot be a
    // parameter in a function definition.
    return false;
  }

  const direct_declarator_func * const ddf =
    dynamic_cast<const direct_declarator_func*>(p);
  assert(ddf);

  return _is_fundef_ddf(*ddf);
}

static bool _is_fundef_ddf_pil(const identifier_list &pil) noexcept
{
  const direct_declarator_func *ddf =
    dynamic_cast<const direct_declarator_func*>(pil.get_parent());
  if (!ddf)
    return false;

  return _is_fundef_ddf(*ddf);
}

void _id_resolver::operator()()
{
  auto &&pre =
    (wrap_callables<default_action_return_value<bool, false>::type>
     ([this](function_definition &fd) {
	_handle_fun_def(fd);
	// The function's scope gets entered only when the
	// direct_declarator_id gets processed below.
	return true;
      },
      [this](const direct_declarator_id &ddid) {
	ddid.process_context<void>
	  (wrap_callables<default_action_nop>
	   ([&](const function_definition &fd) {
	      // It is important to enter a new scope only after the
	      // function definition's declaration specififers have
	      // been processed within the containing scope.
	      _enter_scope();
	    }));
	return false;
      },
      [this](const stmt_compound&) {
	_enter_scope();
	return true;
      },
      [this](const stmt_if&) {
	_enter_scope();
	return true;
      },
      [this](const stmt_switch&) {
	_enter_scope();
	return true;
      },
      [this](const stmt_for_init_decl&) {
	_enter_scope();
	return true;
      },
      [this](const stmt_for_init_expr&) {
	_enter_scope();
	return true;
      },
      [this](const stmt_while&) {
	_enter_scope();
	return true;
      },
      [this](const stmt_do&) {
	_enter_scope();
	return true;
      },
      [this](const stmt &s) {
	const _ast_entity * const p = s.get_parent();
	assert(p);
	if (p->is_any_of<stmt_if, stmt_switch,
			 stmt_for_init_decl, stmt_for_init_expr,
			 stmt_while, stmt_do>()) {
	  _enter_scope();
	  return true;
	}
	return false;
      },
      [this](const parameter_declaration_list &pdl) {
	if (!_is_fundef_ddf_pdl(pdl)) {
	  // Open a new scope if this paramter declaration list is not part
	  // of a function definition's prototype.
	  _enter_scope();
	  return true;
	}
	return false;
      },
      [this](init_declarator &id) {
	    _handle_init_decl(id);
	    return false;
      },
      [this](parameter_declaration_declarator &pdd) {
	_handle_param_decl(pdd);
	return false;
      },
      [this](enumerator &e) {
	_scopes.back()._declared_ids.push_back(expr_id::resolved(e));
	return false;
      },
      [this](identifier_list &pil) {
	if (_is_fundef_ddf_pil(pil))
	  _scopes.back()._declared_ids.push_back(expr_id::resolved(pil));
	return false;
      },
      [this](struct_or_union_ref &sour) {
	_handle_sou_ref(sour);
	return false;
      },
      [this](struct_or_union_def &soud) {
	_handle_sou_def(soud);
	return false;
      },
      [this](enum_ref &er) {
	_handle_enum_ref(er);
	return false;
      },
      [this](enum_def &ed) {
	_handle_enum_def(ed);
	return false;
      },
      [this](expr_id &ei) {
	_resolve_id(ei);
	return false;
      },
      [this](type_specifier_tdid &ts_tdid) {
	_resolve_id(ts_tdid);
	return false;
      },
      [this](expr_label_addr &ela) {
	_resolve_id(ela);
	return false;
      }));

  auto &&post =
    (wrap_callables<no_default_action>
     ([this](const _ast_entity&) {
	_leave_scope();
      }));

  _ast.for_each_dfs_pre_and_po<
    type_set<function_definition,
	     const direct_declarator_id,
	     const stmt_compound,
	     const stmt_if,
	     const stmt_switch,
	     const stmt_for_init_decl,
	     const stmt_for_init_expr,
	     const stmt_while,
	     const stmt_do,
	     const stmt,
	     const parameter_declaration_list,
	     init_declarator,
	     parameter_declaration_declarator,
	     enumerator,
	     identifier_list,
	     struct_or_union_ref,
	     struct_or_union_def,
	     enum_ref,
	     enum_def,
	     expr_id,
	     expr_label_addr,
	     type_specifier_tdid>,
    type_set<const _ast_entity> >(std::move(pre), std::move(post));
}

void _id_resolver::_enter_scope()
{
  _scopes.emplace_back();
}

void _id_resolver::_leave_scope()
{
  _scopes.pop_back();
}

const expr_id::resolved* _id_resolver::_lookup_id(const pp_token_index id_tok,
						  bool *in_cur_scope)
  const noexcept
{
  return _lookup_id(id_tok, [](const expr_id::resolved&) { return true; },
		    in_cur_scope);
}

template <typename predicate_type>
const expr_id::resolved* _id_resolver::_lookup_id(const pp_token_index id_tok,
						  predicate_type &&pred,
						  bool *in_cur_scope)
  const noexcept
{
  const expr_id::resolved *result = nullptr;

  if (in_cur_scope)
    *in_cur_scope = false;

  const pp_token &id = _ast.get_pp_tokens()[id_tok];
  const auto &scopes_begin =  _scopes.rbegin();
  for (auto scope_it = scopes_begin; scope_it != _scopes.rend(); ++scope_it) {
    for (auto r_it = scope_it->_declared_ids.rbegin();
	 !result && r_it != scope_it->_declared_ids.rend();
	 ++r_it) {
      switch (r_it->get_kind()) {
	case resolved_kind::init_declarator:
	  {
	    const pp_token_index r_id_tok
	      = (r_it->get_init_declarator().get_declarator().
		 get_direct_declarator_id().get_id_tok());
	    if (id.get_value() ==_ast.get_pp_tokens()[r_id_tok].get_value())
	      result = &*r_it;
	  }
	  break;

      case resolved_kind::parameter_declaration_declarator:
	  {
	    const pp_token_index r_id_tok
	      = (r_it->get_parameter_declaration_declarator()
		 .get_declarator().get_direct_declarator_id().get_id_tok());
	    if (id.get_value() ==_ast.get_pp_tokens()[r_id_tok].get_value())
	      result = &*r_it;
	  }
	  break;

      case resolved_kind::function_definition:
	  {
	    const pp_token_index r_id_tok
	      = (r_it->get_function_definition().get_declarator()
		 .get_direct_declarator_id().get_id_tok());
	    if (id.get_value() ==_ast.get_pp_tokens()[r_id_tok].get_value())
	      result = &*r_it;
	  }
	  break;

      case resolved_kind::enumerator:
	{
	  const pp_token_index r_id_tok = r_it->get_enumerator().get_id_tok();
	  if (id.get_value() ==_ast.get_pp_tokens()[r_id_tok].get_value())
	    result = &*r_it;
	}
	break;

      case resolved_kind::in_param_id_list:
	{
	  const identifier_list &pil = r_it->get_param_id_list();
	  for (auto pi_tok : pil.get_identifiers()) {
	    if (id.get_value() == _ast.get_pp_tokens()[pi_tok].get_value()) {
	      result = &*r_it;
	      break;
	    }
	  }
	}
	break;

      case resolved_kind::none:
	/* fall through */
      case resolved_kind::builtin_func:
	/* fall through */
      case resolved_kind::stmt_labeled:
	// These are never elements of _declared_ids
	assert(0);
	__builtin_unreachable();
      };

      if (result && !pred(*result)) {
	result = nullptr;
	break;
      }
    }

    if (result) {
      if (in_cur_scope && scope_it == scopes_begin)
	*in_cur_scope = true;
      return result;
    }
  }

  return nullptr;
}

const sou_decl_link* _id_resolver::
_lookup_sou_decl(const pp_token_index id_tok, const bool only_in_cur_scope,
		 const bool skip_cur_scope)
  const noexcept
{
  const pp_token &_id_tok = _ast.get_pp_tokens()[id_tok];
  assert(_scopes.size() > 0);
  const auto scopes_end =
    (!only_in_cur_scope ?
     _scopes.rend() :
     _scopes.rbegin() + 1);
  const auto scopes_begin =
    (!skip_cur_scope ?
     _scopes.rbegin() :
     _scopes.rbegin() + 1);
  for (auto scope_it = scopes_begin; scope_it != scopes_end; ++scope_it) {
    for (auto d_it = scope_it->_declared_sous.rbegin();
	 d_it != scope_it->_declared_sous.rend(); ++d_it) {
      pp_token_index d_id_tok;
      switch (d_it->get_target_kind()) {
      case sou_decl_link::target_kind::ref:
	d_id_tok = d_it->get_target_sou_ref().get_id_tok();
	break;

      case sou_decl_link::target_kind::def:
	d_id_tok = d_it->get_target_sou_def().get_id_tok();
	break;

      case sou_decl_link::target_kind::unlinked:
	assert(0);
	__builtin_unreachable();
      }

      if (_id_tok.get_value() == _ast.get_pp_tokens()[d_id_tok].get_value()) {
	return &*d_it;
      }
    }
  }

  return nullptr;
}

enum_def* _id_resolver::_lookup_enum_def(const pp_token_index id_tok,
					 const bool only_in_cur_scope)
  const noexcept

{
 const pp_token &_id_tok = _ast.get_pp_tokens()[id_tok];
  assert(_scopes.size() > 0);
  const auto scopes_end =
    (!only_in_cur_scope ?
     _scopes.rend() :
     _scopes.rbegin() + 1);
  for (auto scope_it = _scopes.rbegin(); scope_it != scopes_end; ++scope_it) {
    for (auto d_it = scope_it->_defined_enums.begin();
	 d_it != scope_it->_defined_enums.end(); ++d_it) {
      const pp_token_index d_id_tok = d_it->get().get_id_tok();

      if (_id_tok.get_value() == _ast.get_pp_tokens()[d_id_tok].get_value()) {
	return &d_it->get();
      }
    }
  }

  return nullptr;
}

void _id_resolver::_handle_init_decl(init_declarator &id)
{
  const direct_declarator_id &ddid =
    id.get_declarator().get_direct_declarator_id();
  const declaration &d = id.get_containing_declaration();
  const storage_class sc
    = d.get_declaration_specifiers().get_storage_class(_ast);
  const bool is_at_file_scope = d.is_at_file_scope();

  // Check for invalid 'auto' storage class at file scope.
  // In violation of C99 6.9(2), GCC accepts the 'register'
  // storage class at file scope.
  if (is_at_file_scope && sc == storage_class::sc_auto) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "invalid storage class at file scope",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
  }

  // Find a previous declaration visible in the current scope, if any.
  bool prev_is_in_cur_scope;
  const expr_id::resolved *prev = _lookup_id(ddid.get_id_tok(),
					     &prev_is_in_cur_scope);

  // Handle old-style parameter declarations. These have no linkage
  // and must correspond to some identifier in the function defintion's
  // identifier list.
  if (d.get_parent()->is_any_of<declaration_list>()) {
    if (sc != storage_class::sc_none && sc != storage_class::sc_register) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "invalid storage class at parameter declaration",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    } else if (!prev || !prev_is_in_cur_scope) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "old-style parameter not in identifier list",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    } else if (prev->get_kind() != resolved_kind::in_param_id_list) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "invalid redeclaration",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    _scopes.back()._declared_ids.push_back(expr_id::resolved(id));
    return;
  }

  // Handle typedefs
  const bool prev_is_td_in_cur_scope=
    (prev && prev_is_in_cur_scope &&
     (prev->get_kind() == resolved_kind::init_declarator) &&
     ((prev->get_init_declarator().get_containing_declaration()
       .get_declaration_specifiers().get_storage_class(_ast)) ==
      storage_class::sc_typedef));

  if (sc == storage_class::sc_typedef) {
    if (prev) {
      if (prev_is_in_cur_scope && !prev_is_td_in_cur_scope) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark(code_remark::severity::fatal,
			   "redeclaration of non-typedef symbol as typedef",
			   id_tok.get_file_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      id.get_linkage().link_to(prev->get_init_declarator(),
			       linkage::linkage_kind::none, true);
    }

    _scopes.back()._declared_ids.push_back(expr_id::resolved(id));
    return;

  } else if (prev_is_td_in_cur_scope) {
    const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "redeclaration of typedef symbol as non-typedef",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const bool is_fun = ddid.is_function();
  // The 'register' storage class is never allowed at function
  // declarations and function declarations at block scope
  // must be sc_none, sc_extern or, by GCC extension, sc_auto.
  if (is_fun &&
      (sc == storage_class::sc_register ||
       (!is_at_file_scope && sc == storage_class::sc_static))) {
    const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "invalid storage class at function declaration",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  const bool no_linkage =
    (sc == storage_class::sc_register ||
     (!is_fun && !is_at_file_scope && sc != storage_class::sc_extern));
  // Let's be good citizens and check for forbidden redeclarations in
  // the same scope.
  if (prev && prev_is_in_cur_scope &&
      (no_linkage || _get_linkage_kind(*prev) == linkage::linkage_kind::none)) {
    const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "invalid redeclaration",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (no_linkage) {
    _scopes.back()._declared_ids.push_back(expr_id::resolved(id));
    return;
  }


  assert((sc == storage_class::sc_static && is_at_file_scope) ||
	 (sc == storage_class::sc_none && (is_at_file_scope || is_fun)) ||
	 (sc == storage_class::sc_extern) ||
	 (is_fun && !is_at_file_scope && sc == storage_class::sc_auto));
  if (sc == storage_class::sc_static) {
    // Internal linkage.
    assert(is_at_file_scope);
    if (prev) {
      assert(prev_is_in_cur_scope &&
	     (prev->get_kind() == resolved_kind::init_declarator ||
	      prev->get_kind() == resolved_kind::function_definition));

      const linkage::linkage_kind prev_linkage = _get_linkage_kind(*prev);
      if (prev_linkage != linkage::linkage_kind::internal) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark(code_remark::severity::fatal,
			   "static declaration follows non-static one",
			   id_tok.get_file_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(id.get_linkage(), *prev);

    } else {
      _try_resolve_pending_linkages(id, linkage::linkage_kind::internal);

    }

  } else if (sc == storage_class::sc_extern ||
	     (sc == storage_class::sc_none && is_fun)) {
    // Linkage of previous declaration, if any. Otherwise external.
    if (prev) {
      auto has_linkage =
	[](const expr_id::resolved &resolved) {
	  switch (resolved.get_kind()) {
	  case resolved_kind::init_declarator:
	    /* fall through */
	  case resolved_kind::function_definition:
	    return (_get_linkage_kind(resolved) != linkage::linkage_kind::none);

	  case resolved_kind::parameter_declaration_declarator:
	    /* fall through */
	  case resolved_kind::enumerator:
	    /* fall through */
	  case resolved_kind::in_param_id_list:
	    return false;

	  case resolved_kind::none:
	    /* fall through */
	  case resolved_kind::builtin_func:
	    /* fall through */
	  case resolved_kind::stmt_labeled:
	    /* fall through */
	  assert(0);
	  __builtin_unreachable();
	};
      };

      if (!has_linkage(*prev)) {
	// C99, 6.2.2(4): if the prior declaration doesn't specify a
	// linkage, the linkage is extern. Check that this is
	// compatible with any declaration in turn preceeding the
	// prior one.
	prev = _lookup_id(ddid.get_id_tok(), has_linkage);
	if (prev) {
	  const linkage::linkage_kind prev_linkage = _get_linkage_kind(*prev);
	  if (prev_linkage != linkage::linkage_kind::external) {
	    const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	    code_remark remark(code_remark::severity::fatal,
			       "extern declaration follows static one",
			       id_tok.get_file_range());
	    _ast.get_remarks().add(remark);
	    throw semantic_except(remark);
	  }
	}
      }
    }

    if (prev) {
      _link_to(id.get_linkage(), *prev);
    } else {
      _add_pending_linkage(id);
    }

  } else if (sc == storage_class::sc_none) {
    assert(is_at_file_scope && !is_fun);
    // Object at file scope with no storage class specifier: external
    // linkage.
    if (prev) {
      assert(prev_is_in_cur_scope &&
	     (prev->get_kind() == resolved_kind::init_declarator ||
	      prev->get_kind() == resolved_kind::function_definition));

      const linkage::linkage_kind prev_linkage = _get_linkage_kind(*prev);
      if (prev_linkage != linkage::linkage_kind::external) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark(code_remark::severity::fatal,
			   "non-static declaration follows static one",
			   id_tok.get_file_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(id.get_linkage(), *prev);

    } else {
      _try_resolve_pending_linkages(id, linkage::linkage_kind::external);
    }

  } else if (is_fun && !is_at_file_scope && sc == storage_class::sc_auto) {
    // This branch deals with the gcc extension of having an 'auto' specifier
    // for linking to nested functions in the same scope.
    if (prev && prev_is_in_cur_scope) {
      assert(prev->get_kind() == resolved_kind::init_declarator ||
	     prev->get_kind() == resolved_kind::function_definition);

      const linkage::linkage_kind prev_linkage = _get_linkage_kind(*prev);
      if (prev_linkage != linkage::linkage_kind::nested_fun_auto) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark{
	  code_remark::severity::fatal,
	  "auto nested function declaration follows an external one",
	  id_tok.get_file_range()
	};
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(id.get_linkage(), *prev);

    } else {
      id.get_linkage()
	.set_linkage_kind(linkage::linkage_kind::nested_fun_auto);
    }

  } else {
    assert(0);
    __builtin_unreachable();
  }

  _scopes.back()._declared_ids.push_back(expr_id::resolved(id));
}

void _id_resolver::_handle_param_decl(parameter_declaration_declarator &pdd)
{
  const storage_class sc =
    pdd.get_declaration_specifiers().get_storage_class(_ast);
  if (sc != storage_class::sc_none && sc != storage_class::sc_register) {
    const direct_declarator_id &ddid =
      pdd.get_declarator().get_direct_declarator_id();
    const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "invalid storage class at parameter declaration",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _scopes.back()._declared_ids.push_back(expr_id::resolved(pdd));
}

void _id_resolver::_handle_fun_def(function_definition &fd)
{
  const direct_declarator_id &ddid =
    fd.get_declarator().get_direct_declarator_id();
  if (!ddid.is_function()) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "no function declarator in definition's prototype",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
  }

  const storage_class sc
    = fd.get_declaration_specifiers().get_storage_class(_ast);

  const bool is_at_file_scope = fd.is_at_file_scope();
  if (sc != storage_class::sc_none &&
      !(is_at_file_scope && (sc == storage_class::sc_static ||
			     sc == storage_class::sc_extern)) &&
      !(!is_at_file_scope && sc == storage_class::sc_auto)) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "invalid storage class at function definition",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
  }

  // Find a previous declaration visible in the current scope, if any.
  bool prev_is_in_cur_scope;
  const expr_id::resolved * const prev = _lookup_id(ddid.get_id_tok(),
						    &prev_is_in_cur_scope);

  // Check for forbidden redeclarations in the same scope.
  if (prev && prev_is_in_cur_scope) {
    if (prev->get_kind() == resolved_kind::enumerator ||
	prev->get_kind() == resolved_kind::parameter_declaration_declarator ||
	prev->get_kind() == resolved_kind::in_param_id_list) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "invalid redeclaration",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    } else if (prev->get_kind() == resolved_kind::function_definition) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "function redefined",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    }
  }

  if (!is_at_file_scope) {
    // Nested functions have 'auto' linkage.
    if (prev && prev_is_in_cur_scope) {
      assert(prev->get_kind() == resolved_kind::init_declarator);

      const linkage::linkage_kind prev_linkage = _get_linkage_kind(*prev);
      if (prev_linkage != linkage::linkage_kind::nested_fun_auto) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark{
	  code_remark::severity::fatal,
	  "nested function definition follows an external declaration",
	  id_tok.get_file_range()
	};
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(fd.get_linkage(), *prev);

    } else {
      fd.get_linkage().set_linkage_kind(linkage::linkage_kind::nested_fun_auto);
    }

  } else if (sc == storage_class::sc_static) {
    assert(is_at_file_scope);
    // Internal linkage.
    if (prev) {
      assert(prev_is_in_cur_scope);
      assert(prev->get_kind() == resolved_kind::init_declarator);

      const linkage::linkage_kind prev_linkage = _get_linkage_kind(*prev);
      if (prev_linkage != linkage::linkage_kind::internal) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark(code_remark::severity::fatal,
			   "static declaration follows non-static one",
			   id_tok.get_file_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(fd.get_linkage(), *prev);

    } else {
      _try_resolve_pending_linkages(fd, linkage::linkage_kind::internal);
    }

  } else {
    assert(sc == storage_class::sc_extern || sc == storage_class::sc_none);
    assert(is_at_file_scope);
    // Linkage of previous declaration, if any. Otherwise external.
    if (prev) {
      assert(prev->get_kind() == resolved_kind::init_declarator);
      const linkage::linkage_kind prev_linkage = _get_linkage_kind(*prev);
      assert(prev_linkage != linkage::linkage_kind::none);

      _link_to(fd.get_linkage(), *prev);

    } else {
      _try_resolve_pending_linkages(fd, linkage::linkage_kind::external);
    }
  }

  _scopes.back()._declared_ids.push_back(expr_id::resolved(fd));
}

void _id_resolver::_handle_sou_ref(struct_or_union_ref &sour)
{
  bool is_standalone_decl = sour.is_standalone_decl();
  // If the 'struct foo' construct is standalone, i.e. is a
  // declaration of that tag, then search only the current scope.
  const sou_decl_link *prev_decl = _lookup_sou_decl(sour.get_id_tok(),
						    is_standalone_decl, false);

  // Non-standalone struct or union usages can potentially serve as
  // declarations, even with all prior same-scope declarations
  // removed. Check for this case and link the current usage into the
  // list of declararions, if so.
  const sou_decl_link *prev_outer_scope_decl =
    _lookup_sou_decl(sour.get_id_tok(), false, true);
  types::struct_or_union_kind prev_tag_kind;
  if ((is_standalone_decl || !prev_outer_scope_decl) && prev_decl) {
    // It's a redeclaration in the same scope, link it to the previous one.
    switch (prev_decl->get_target_kind()) {
    case sou_decl_link::target_kind::ref:
      prev_tag_kind = prev_decl->get_target_sou_ref().get_tag_kind();
      sour.get_decl_list_node().link_to(prev_decl->get_target_sou_ref());
      break;

    case sou_decl_link::target_kind::def:
      prev_tag_kind = prev_decl->get_target_sou_def().get_tag_kind();
      sour.get_decl_list_node().link_to(prev_decl->get_target_sou_def());
      break;

    case sou_decl_link::target_kind::unlinked:
      assert(0);
      __builtin_unreachable();
    }

    _scopes.back()._declared_sous.push_back(sou_decl_link(sour));

  } else if (!prev_decl) {
    // It's the first occurence and thus a declaration.
    if (_lookup_enum_def(sour.get_id_tok(), true)) {
      const pp_token &id_tok = _ast.get_pp_tokens()[sour.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "tag redeclared as a different kind",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    }

    _scopes.back()._declared_sous.push_back(sou_decl_link(sour));
    return;

  } else {
    assert (!is_standalone_decl && prev_outer_scope_decl && prev_decl);
    assert(prev_decl == prev_outer_scope_decl);
    // It isnt't a declaration for that tag, but a real usage.
    switch (prev_decl->get_target_kind()) {
    case sou_decl_link::target_kind::ref:
      prev_tag_kind = prev_decl->get_target_sou_ref().get_tag_kind();
      sour.link_to_declaration(prev_decl->get_target_sou_ref());
      break;

    case sou_decl_link::target_kind::def:
      prev_tag_kind = prev_decl->get_target_sou_def().get_tag_kind();
      sour.link_to_declaration(prev_decl->get_target_sou_def());
      break;

    case sou_decl_link::target_kind::unlinked:
      assert(0);
      __builtin_unreachable();
    }
  }

  assert(prev_decl);
  if (prev_tag_kind != sour.get_tag_kind()) {
    const pp_token &id_tok = _ast.get_pp_tokens()[sour.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "tag redeclared as a different kind",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }
}

void _id_resolver::_handle_sou_def(struct_or_union_def &soud)
{
  if (!soud.has_id())
    return;

  const sou_decl_link *prev_decl = _lookup_sou_decl(soud.get_id_tok(),
						    true, false);
  if (prev_decl) {
    // It's a redeclaration, link it to the previous one.
    types::struct_or_union_kind  prev_tag_kind;
    struct_or_union_def *prev_def = nullptr;
    switch (prev_decl->get_target_kind()) {
    case sou_decl_link::target_kind::ref:
      {
	struct_or_union_ref &prev_sou_ref = prev_decl->get_target_sou_ref();
	prev_def = prev_sou_ref.get_decl_list_node().find_definition();
	prev_tag_kind = prev_sou_ref.get_tag_kind();
	soud.get_decl_list_node().link_to(prev_sou_ref);
      }
      break;

    case sou_decl_link::target_kind::def:
      {
	struct_or_union_def &prev_sou_def = prev_decl->get_target_sou_def();
	prev_def = &prev_sou_def;
	prev_tag_kind = prev_sou_def.get_tag_kind();
	soud.get_decl_list_node().link_to(prev_sou_def);
      }
      break;

    case sou_decl_link::target_kind::unlinked:
	assert(0);
	__builtin_unreachable();
    }

    if (prev_tag_kind != soud.get_tag_kind()) {
      const pp_token &id_tok = _ast.get_pp_tokens()[soud.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "tag redeclared as a different kind",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    } else if (prev_def) {
      const pp_token &id_tok = _ast.get_pp_tokens()[soud.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "struct or union redefined",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    }

  } else {
    // It's the first declaration.
    if (_lookup_enum_def(soud.get_id_tok(), true)) {
      const pp_token &id_tok = _ast.get_pp_tokens()[soud.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "tag redeclared as a different kind",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    }
  }

  _scopes.back()._declared_sous.push_back(sou_decl_link(soud));
}

void _id_resolver::_handle_enum_def(enum_def &ed)
{
  if (!ed.has_id())
    return;

  if (_lookup_enum_def(ed.get_id_tok(), true)) {
    const pp_token &id_tok = _ast.get_pp_tokens()[ed.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "enum redeclared",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  } else if (_lookup_sou_decl(ed.get_id_tok(), true, false)) {
    const pp_token &id_tok = _ast.get_pp_tokens()[ed.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "tag redeclared as a different kind",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _scopes.back()._defined_enums.push_back(std::ref(ed));
}

void _id_resolver::_handle_enum_ref(enum_ref &er)
{
  enum_def * const ed = _lookup_enum_def(er.get_id_tok(), false);
  if (!ed) {
    const pp_token &id_tok = _ast.get_pp_tokens()[er.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "enum undeclared",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  er.link_to_definition(*ed);
}

linkage::linkage_kind
_id_resolver::_get_linkage_kind(const expr_id::resolved &resolved) noexcept
{
  switch (resolved.get_kind()) {
  case resolved_kind::init_declarator:
    return resolved.get_init_declarator().get_linkage().get_linkage_kind();

  case resolved_kind::function_definition:
    return resolved.get_function_definition().get_linkage().get_linkage_kind();

  case resolved_kind::parameter_declaration_declarator:
    /* fall through */
  case resolved_kind::enumerator:
    /* fall through */
  case resolved_kind::in_param_id_list:
    return linkage::linkage_kind::none;

  case resolved_kind::none:
    /* fall through */
  case resolved_kind::builtin_func:
    /* fall through */
  case resolved_kind::stmt_labeled:
    assert(0);
  __builtin_unreachable();
  };
}

void _id_resolver::_link_to(linkage &l, const expr_id::resolved &resolved)
  noexcept
{
  switch (resolved.get_kind()) {
  case resolved_kind::init_declarator:
    {
      init_declarator &id = resolved.get_init_declarator();
      const linkage::linkage_kind kind = id.get_linkage().get_linkage_kind();
      l.link_to(id, kind, true);
    }
    break;

  case resolved_kind::function_definition:
    {
      function_definition &fd = resolved.get_function_definition();
      const linkage::linkage_kind kind = fd.get_linkage().get_linkage_kind();
      l.link_to(fd, kind, true);
    }
    break;

  case resolved_kind::none:
    /* fall through */
  case resolved_kind::builtin_func:
    /* fall through */
  case resolved_kind::parameter_declaration_declarator:
    /* fall through */
  case resolved_kind::stmt_labeled:
    /* fall through */
  case resolved_kind::enumerator:
    /* fall through */
  case resolved_kind::in_param_id_list:
    assert(0);
    __builtin_unreachable();
  };
}

void _id_resolver::_add_pending_linkage(init_declarator &id)
{
  _try_resolve_pending_linkages(id, linkage::linkage_kind::external);
  const bool is_at_file_scope =
    id.get_containing_declaration().is_at_file_scope();

  if (!is_at_file_scope) {
    // Subsequent declarations might not be able to discover this.
    _pending_linkages.push_back(std::ref(id));
  }
}

void
_id_resolver::_try_resolve_pending_linkages(init_declarator &id,
					    const linkage::linkage_kind kind)
{
  const direct_declarator_id &ddid =
    id.get_declarator().get_direct_declarator_id();
  const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
  auto it_pl = _pending_linkages.begin();
  for (; it_pl != _pending_linkages.end(); ++it_pl) {
    const direct_declarator_id &pl_ddid =
      it_pl->get().get_declarator().get_direct_declarator_id();
    const pp_token &pl_id_tok = _ast.get_pp_tokens()[pl_ddid.get_id_tok()];
    if (id_tok.get_value() == pl_id_tok.get_value())
      break;
  }

  if (it_pl == _pending_linkages.end()) {
    id.get_linkage().set_linkage_kind(kind);
    return;
  }

  init_declarator &pl_id = it_pl->get();
  const linkage::linkage_kind pl_linkage_kind
    = pl_id.get_linkage().get_linkage_kind();
  assert(pl_linkage_kind == linkage::linkage_kind::external);
  if (kind != pl_linkage_kind) {
    code_remark remark(code_remark::severity::fatal,
		       "static declaration follows external one",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _pending_linkages.erase(it_pl);
  id.get_linkage().link_to(pl_id, kind, false);
}

void
_id_resolver::_try_resolve_pending_linkages(function_definition &fd,
					   const linkage::linkage_kind kind)
{
  const direct_declarator_id &ddid =
    fd.get_declarator().get_direct_declarator_id();
  const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
  auto it_pl = _pending_linkages.begin();
  for (; it_pl != _pending_linkages.end(); ++it_pl) {
    const direct_declarator_id &pl_ddid =
      it_pl->get().get_declarator().get_direct_declarator_id();
    const pp_token &pl_id_tok = _ast.get_pp_tokens()[pl_ddid.get_id_tok()];
    if (id_tok.get_value() == pl_id_tok.get_value())
      break;
  }

  if (it_pl == _pending_linkages.end()) {
    fd.get_linkage().set_linkage_kind(kind);
    return;
  }

  init_declarator &pl_id = it_pl->get();
  const linkage::linkage_kind pl_linkage_kind
    = pl_id.get_linkage().get_linkage_kind();
  assert(pl_linkage_kind == linkage::linkage_kind::external);
  if (kind != pl_linkage_kind) {
    code_remark remark(code_remark::severity::fatal,
		       "static declaration follows external one",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _pending_linkages.erase(it_pl);
  fd.get_linkage().link_to(pl_id, kind, false);
}

void _id_resolver::_resolve_id(expr_id &ei)
{
  // In case of the expr_id representing the identifier following a
  // goto keyword, try to resolve to a label first.
  _ast_entity * const non_parens_parent = ei.get_non_parens_parent();
  assert(non_parens_parent);
  if (non_parens_parent->is_any_of<stmt_goto>()) {
    for (_ast_entity *p = non_parens_parent; p; p = p->get_parent()) {
      stmt_compound *sc = dynamic_cast<stmt_compound*>(p);
      if (sc) {
	stmt_labeled *sl = sc->lookup_label(_ast, ei.get_id_tok());
	if (sl) {
	  ei.set_resolved(expr_id::resolved(*sl));
	  return;
	}
      }
    }
  }

  // Otherwise, search "ordinary" identifiers.
  const expr_id::resolved *resolved_id = _lookup_id(ei.get_id_tok());
  if (resolved_id) {
    ei.set_resolved(*resolved_id);
    return;
  }

  // Otherwise check whether the identifier refers to a builtin
  // and if so, report that fact.
  const pp_token &id_tok = _ast.get_pp_tokens()[ei.get_id_tok()];
  const builtin_func::factory builtin_func_fac =
    builtin_func::lookup(id_tok.get_value());
  if (builtin_func_fac) {
    ei.set_resolved(expr_id::resolved(builtin_func_fac));
    return;
  }

  // Resolving the identifier failed. Silently accept that for expr_id's
  // being part of attributes.
  for (const _ast_entity *p = ei.get_parent(); p; p = p->get_parent()) {
    if (p->is_any_of<attribute>())
      return;
  }

  if (non_parens_parent->is_any_of<expr_func_invocation>()) {
    // warn only about calls to undeclared functions
    code_remark remark(code_remark::severity::warning,
		       "identifier \"" + id_tok.get_value() + "\" not declared",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    return;
  }

  code_remark remark(code_remark::severity::fatal,
		     "identifier \"" + id_tok.get_value() + "\" not declared",
		     id_tok.get_file_range());
  _ast.get_remarks().add(remark);
  throw semantic_except(remark);
}

void _id_resolver::_resolve_id(expr_label_addr &ela)
{
  for ( _ast_entity *p = ela.get_parent(); p; p = p->get_parent()) {
    stmt_compound *sc = dynamic_cast<stmt_compound *>(p);
    if (sc) {
      const stmt_labeled * const sl =
	sc->lookup_label(_ast, ela.get_label_tok());
      if (sl) {
	ela.set_resolved(*sl);
	return;
      }
    }
  }

  const pp_token &id_tok = _ast.get_pp_tokens()[ela.get_label_tok()];
  code_remark remark(code_remark::severity::fatal,
		     "label not found",
		     id_tok.get_file_range());
  _ast.get_remarks().add(remark);
  throw semantic_except(remark);
}

void _id_resolver::_resolve_id(type_specifier_tdid &ts_tdid)
{
  // Search "ordinary" identifiers.
  const pp_token &id_tok = _ast.get_pp_tokens()[ts_tdid.get_id_tok()];
  for (auto scope_it = _scopes.rbegin(); scope_it != _scopes.rend();
       ++scope_it) {
    for (auto r_it = scope_it->_declared_ids.rbegin();
	 r_it != scope_it->_declared_ids.rend(); ++r_it) {
      if (r_it->get_kind() == resolved_kind::init_declarator) {
	init_declarator &id = r_it->get_init_declarator();
	const direct_declarator_id &ddid =
	  id.get_declarator().get_direct_declarator_id();
	if (id_tok.get_value() ==
	    _ast.get_pp_tokens()[ddid.get_id_tok()].get_value()) {
	  ts_tdid.set_resolved(ddid);
	  return;
	}
      }
    }
  }

  // Typedef id not found. Check whether the identifier refers to a
  // builtin and if so, silently accept it.
  static std::set<std::string> builtin_tdids =
    _init_builtin_ids_set(_builtin_tdids);
  if (builtin_tdids.count(id_tok.get_value())) {
    ts_tdid.set_builtin();
    return;
  }

  code_remark remark(code_remark::severity::fatal,
		     "typedef identifier not declared",
		     id_tok.get_file_range());
  _ast.get_remarks().add(remark);
  throw semantic_except(remark);
}



void ast_translation_unit::_resolve_ids()
{
  _id_resolver ir(*this);
  ir();
}

void ast_translation_unit::resolve()
{
  _register_labels();
  _resolve_ids();
}
