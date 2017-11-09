#include <cassert>
#include <set>
#include <string>
#include "ast.hh"
#include "semantic_except.hh"

using namespace suse::cp;
using namespace suse::cp::ast;

static const char * const _builtin_ids[] = {
	"__ATOMIC_ACQ_REL",
	"__ATOMIC_ACQUIRE",
	"__ATOMIC_CONSUME",
	"__ATOMIC_RELAXED",
	"__ATOMIC_RELEASE",
	"__ATOMIC_SEQ_CST",
	"__GCC_ATOMIC_TEST_AND_SET_TRUEVAL",
	"__atomic_add",
	"__atomic_add_fetch",
	"__atomic_and_fetch",
	"__atomic_clear",
	"__atomic_compare_exchange",
	"__atomic_compare_exchange_n",
	"__atomic_exchange",
	"__atomic_exchange_n",
	"__atomic_fetch_add",
	"__atomic_fetch_and",
	"__atomic_fetch_nand",
	"__atomic_fetch_or",
	"__atomic_fetch_sub",
	"__atomic_fetch_xor",
	"__atomic_load",
	"__atomic_load_n",
	"__atomic_nand_fetch",
	"__atomic_or_fetch",
	"__atomic_signal_fence",
	"__atomic_store",
	"__atomic_store_",
	"__atomic_store_n",
	"__atomic_sub_fetch",
	"__atomic_test_and_set",
	"__atomic_thread_fence",
	"__atomic_xor_fetch",
	"__builtin_abort",
	"__builtin_abs",
	"__builtin_add_overflow",
	"__builtin_aligned_alloc",
	"__builtin_alloca",
	"__builtin_alloca_with_align",
	"__builtin_apply",
	"__builtin_apply_args",
	"__builtin_assume_aligned",
	"__builtin_atan",
	"__builtin_bcmp",
	"__builtin_bcopy",
	"__builtin_bswap",
	"__builtin_bzero",
	"__builtin_cabs",
	"__builtin_cabsf",
	"__builtin_cabsl",
	"__builtin_calloc",
	"__builtin_cexpi",
	"__builtin_choose_expr",
	"__builtin___clear_cache",
	"__builtin_clrsb",
	"__builtin_clrsbl",
	"__builtin_clrsbll",
	"__builtin_clz",
	"__builtin_clzl",
	"__builtin_clzll",
	"__builtin_complex",
	"__builtin_constant_p",
	"__builtin_copysign",
	"__builtin_copysignf",
	"__builtin_cos",
	"__builtin_ctz",
	"__builtin_ctzl",
	"__builtin_ctzll",
	"__builtin_exit",
	"__builtin_exp",
	"__builtin_expect",
	"__builtin_fabs",
	"__builtin_ffs",
	"__builtin_ffsl",
	"__builtin_ffsll",
	"__builtin_finite",
	"__builtin_finitef",
	"__builtin_finitel",
	"__builtin_fma",
	"__builtin_fpclassify",
	"__builtin_frame_address",
	"__builtin_free",
	"__builtin_inf",
	"__builtin_infl",
	"__builtin_irint",
	"__builtin_iround",
	"__builtin_isfinite",
	"__builtin_isgreater",
	"__builtin_isgreaterequal",
	"__builtin_isinf",
	"__builtin_isinff",
	"__builtin_isinfl",
	"__builtin_isinf_sign",
	"__builtin_isless",
	"__builtin_islessequal",
	"__builtin_islessgreater",
	"__builtin_isnan",
	"__builtin_isnanf",
	"__builtin_isnanl",
	"__builtin_isnormal",
	"__builtin_isunordered",
	"__builtin_labs",
	"__builtin_lceil",
	"__builtin_lfloor",
	"__builtin_llabs",
	"__builtin_llceil",
	"__builtin_llfloor",
	"__builtin_llround",
	"__builtin_lround",
	"__builtin_lroundf",
	"__builtin_malloc",
	"__builtin_memcmp",
	"__builtin_memcpy",
	"__builtin___memcpy_chk",
	"__builtin_memmove",
	"__builtin___memmove_chk",
	"__builtin_mempcpy",
	"__builtin___mempcpy_chk",
	"__builtin_memset",
	"__builtin___memset_chk",
	"__builtin_mul_overflow",
	"__builtin_nan",
	"__builtin_nanl",
	"__builtin_next_arg",
	"__builtin_object_size",
	"__builtin_offsetof",
	"__builtin_parity",
	"__builtin_parityl",
	"__builtin_parityll",
	"__builtin_popcount",
	"__builtin_popcountl",
	"__builtin_popcountll",
	"__builtin_pow",
	"__builtin_powi",
	"__builtin_powif",
	"__builtin_powil",
	"__builtin_prefetch",
	"__builtin_printf",
	"__builtin_puts",
	"__builtin_realloc",
	"__builtin_return",
	"__builtin_return_address",
	"__builtin_setjmp",
	"__builtin_shuffle",
	"__builtin_signbit",
	"__builtin_signbitf",
	"__builtin_signbitl",
	"__builtin_significandl",
	"__builtin_sin",
	"__builtin_sincos",
	"__builtin_sincosl",
	"__builtin_snprintf",
	"__builtin___snprintf_chk",
	"__builtin_sprintf",
	"__builtin_sqrt",
	"__builtin_sqrtf",
	"__builtin_sqrtl",
	"__builtin_stack_restore",
	"__builtin_stack_save",
	"__builtin_stpcpy",
	"__builtin___stpcpy_chk",
	"__builtin_stpncpy",
	"__builtin_strcat",
	"__builtin___strcat_chk",
	"__builtin_strchr",
	"__builtin_strcmp",
	"__builtin_strcpy",
	"__builtin___strcpy_chk",
	"__builtin_strdup",
	"__builtin_strlen",
	"__builtin_strncat",
	"__builtin_strncmp",
	"__builtin_strncpy",
	"__builtin_strstr",
	"__builtin_trap",
	"__builtin_types_compatible_p",
	"__builtin_unreachable",
	"__builtin_va_arg_pack",
	nullptr
};

static std::set<std::string> _init_builtin_ids_set()
{
  std::set<std::string> s;

  for (const char * const *b = _builtin_ids; *b; ++b)
    s.insert(*b);

  return s;
}

void stmt_compound::register_label(stmt_labeled * const label)
{
  _labels.push_back(std::ref(*label));
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


namespace
{
  class _register_label_scope_finder
  {
  public:
    _register_label_scope_finder(ast::ast &ast,
				 const pp_token_index label_tok) noexcept
      : _ast(ast), _label_tok(label_tok), _registrar(nullptr)
    {}

    stmt_compound* get_result() noexcept
    {
      return _registrar;
    }

    bool operator()(stmt_compound &sc)
    {
      _registrar = &sc;

      if (sc.lookup_label(_ast, _label_tok)) {
	code_remark remark(code_remark::severity::fatal,
			   "label redefined",
			   _ast.get_pp_tokens()[_label_tok].get_file_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      // If this label is declared local to this block, finish the
      // search here.
      if (sc.is_local_label(_ast, _label_tok))
	return false;

      return true;
    }

    bool operator()(const expr_statement&)
    {
      // Don't let labels escape statement expressions.
      return false;
    }

    bool operator()(const function_definition&)
    {
      // Don't let labels escape function definitions.
      return false;
    }

    bool operator()(const _ast_entity&)
    {
      // Default: continue search upwards the tree.
      return true;
    }

  private:
    ast::ast &_ast;
    const pp_token_index _label_tok;
    stmt_compound *_registrar;
  };
}

void ast::ast::_register_labels()
{
  this->for_each_dfs_po([&](_ast_entity &ae) {
      stmt_labeled *sl = dynamic_cast<stmt_labeled*>(&ae);
      if (sl) {
	_register_label_scope_finder rlsf(*this, sl->get_label_tok());
	sl->for_each_parent<type_set<expr_statement> >(rlsf);
	assert(rlsf.get_result());
	rlsf.get_result()->register_label(sl);
      }
    });
}

namespace
{
  class _id_resolver
  {
  public:
    _id_resolver(ast::ast &ast);

    bool operator()(_ast_entity &ae, const pre_traversal_tag&);

    void operator()(_ast_entity &ae, const post_traversal_tag&);

  private:
    void _enter_scope();
    void _leave_scope();

    void _handle_direct_declarator_id(direct_declarator_id &ddid);
    void _resolve_id(expr_id &ei);
    void _resolve_id(type_specifier_tdid &ts_tdid);

    struct _declarator_ctx_finder
    {
      _declarator_ctx_finder() noexcept
	: type(ctx_type::none)
      {}

      bool operator()(const declarator&) noexcept
      { return true; }

      bool operator()(const direct_declarator&) noexcept
      { return true; }

      bool operator()(const struct_declarator&) noexcept;
      bool operator()(const parameter_declaration_declarator &pdd) noexcept;
      bool operator()(const init_declarator&) noexcept;
      bool operator()(const function_definition&) noexcept;

      enum class ctx_type
      {
	none,
	struct_decl,
	parameter_decl,
	init_decl,
	function_def,
      } type;
    };

    ast::ast &_ast;

    typedef std::vector<expr_id::resolved> _scope_type;
    typedef std::vector<_scope_type> _scopes_type;

    _scopes_type _scopes;
  };
}

_id_resolver::_id_resolver(ast::ast &ast)
  : _ast(ast)
{
  _enter_scope();
}

static bool _is_fundef_ddf(direct_declarator_func const *ddf)
{
  // Check that the first non-declarator/-direct_declarator parent is
  // a function_definition instance.
  for (const _ast_entity *p = ddf->get_parent(); ; p = p->get_parent()) {
    assert(p);
    if (dynamic_cast<const function_definition*>(p)) {
      return true;
    } else if (!dynamic_cast<const declarator*>(p) &&
	       !dynamic_cast<const direct_declarator*>(p)) {
      return false;
    }
  }

  return false;
}

static bool _is_fundef_ddf_pdl(const parameter_declaration_list *pdl)
  noexcept
{
  const _ast_entity * const p = pdl->get_parent();

  if (dynamic_cast<const direct_abstract_declarator_func*>(p)) {
    // A parameter in an abstract func declarator cannot be a
    // parameter in a function definition.
    return false;
  }

  const direct_declarator_func *ddf =
    dynamic_cast<const direct_declarator_func*>(p);
  assert(ddf);

  return _is_fundef_ddf(ddf);
}

static bool _is_fundef_ddf_pil(const identifier_list *pil) noexcept
{
  const direct_declarator_func *ddf =
    dynamic_cast<const direct_declarator_func*>(pil->get_parent());
  if (!ddf)
    return false;

  return _is_fundef_ddf(ddf);
}


bool _id_resolver::operator()(_ast_entity &ae, const pre_traversal_tag&)
{
  if (dynamic_cast<function_definition*>(&ae) ||
      dynamic_cast<stmt_compound*>(&ae) ||
      dynamic_cast<stmt_for_init_decl*>(&ae)) {
    _enter_scope();
    return true;
  }

  parameter_declaration_list *pdl =
    dynamic_cast<parameter_declaration_list*>(&ae);
  if (pdl) {
    if (!_is_fundef_ddf_pdl(pdl)) {
	// Open a new scope if this paramter declaration list is not part
	// of a function definition's prototype.
	_enter_scope();
	return true;
    }
    return false;
  }

  direct_declarator_id *ddid = dynamic_cast<direct_declarator_id*>(&ae);
  if (ddid) {
    _handle_direct_declarator_id(*ddid);
    return false;
  }

  enumerator *e = dynamic_cast<enumerator*>(&ae);
  if (e) {
    _scopes.back().push_back(expr_id::resolved(*e));
    return false;
  }

  identifier_list *pil = dynamic_cast<identifier_list*>(&ae);
  if (pil) {
    if (_is_fundef_ddf_pil(pil))
      _scopes.back().push_back(expr_id::resolved(*pil));

    return false;
  }


  expr_id *ei = dynamic_cast<expr_id*>(&ae);
  if (ei) {
    _resolve_id(*ei);
    return false;
  }

  type_specifier_tdid *ts_tdid = dynamic_cast<type_specifier_tdid*>(&ae);
  if (ts_tdid) {
    _resolve_id(*ts_tdid);
    return false;
  }

  return false;
}

void _id_resolver::operator()(_ast_entity &ae, const post_traversal_tag&)
{
  _leave_scope();
}

void _id_resolver::_enter_scope()
{
  _scopes.emplace_back();
}

void _id_resolver::_leave_scope()
{
  _scopes.pop_back();
}

bool _id_resolver::_declarator_ctx_finder::operator()(const struct_declarator&)
  noexcept
{
  type = ctx_type::struct_decl;
  return false;
}

bool _id_resolver::_declarator_ctx_finder::
operator()(const parameter_declaration_declarator &pdd) noexcept
{
  type = ctx_type::parameter_decl;
  return false;
}

bool _id_resolver::_declarator_ctx_finder::operator()(const init_declarator&)
  noexcept
{
  type = ctx_type::init_decl;
  return false;
}

bool
_id_resolver::_declarator_ctx_finder::operator()(const function_definition&)
  noexcept
{
  type = ctx_type::function_def;
  return false;
}


void _id_resolver::_handle_direct_declarator_id(direct_declarator_id &ddid)
{
  _declarator_ctx_finder dcf;

  ddid.for_each_parent<type_set<struct_declarator,
				 init_declarator,
				 function_definition,
				 parameter_declaration_declarator> >(dcf);
  assert(dcf.type != _declarator_ctx_finder::ctx_type::none);

  switch (dcf.type)
  {
  case _declarator_ctx_finder::ctx_type::none:
    assert(0);
    __builtin_unreachable();

  case _declarator_ctx_finder::ctx_type::struct_decl:
    return;

  case _declarator_ctx_finder::ctx_type::parameter_decl:
    /* fall through */
  case _declarator_ctx_finder::ctx_type::init_decl:
    _scopes.back().push_back(expr_id::resolved(ddid));
    return;

  case _declarator_ctx_finder::ctx_type::function_def:
    // A function definition's id declarator must get added to parent
    // scope.
    assert(_scopes.size() >= 2);
    (_scopes.end() - 2)->push_back(expr_id::resolved(ddid));
    return;
  }
}

void _id_resolver::_resolve_id(expr_id &ei)
{
  assert(ei.get_parent());

  // In case of the expr_id representing the identifier following a
  // goto keyword, try to resolve to a label first.
  if (dynamic_cast<const stmt_goto*>(ei.get_parent())) {
    for (_ast_entity *p = ei.get_parent()->get_parent();
	 p; p = p->get_parent()) {
      stmt_compound *sc = dynamic_cast<stmt_compound*>(p);
      if (sc) {
	stmt_labeled *sl = sl = sc->lookup_label(_ast, ei.get_id_tok());
	if (sl) {
	  ei.set_resolved(expr_id::resolved(*sl));
	  return;
	}
      }
    }
  }

  // Otherwise, search "ordinary" identifiers.
  const pp_token &id_tok = _ast.get_pp_tokens()[ei.get_id_tok()];
  for (auto scope_it = _scopes.rbegin(); scope_it != _scopes.rend();
       ++scope_it) {
    for (auto r_it = scope_it->rbegin(); r_it != scope_it->rend(); ++r_it) {
      if (r_it->get_type() ==
	  expr_id::resolved::resolved_type::direct_declarator_id) {
	if (id_tok.get_value() ==
	    (_ast.get_pp_tokens()[r_it->get_direct_declarator().get_id_tok()]
	     .get_value())) {
	  ei.set_resolved(*r_it);
	  return;
	}
      } else if (r_it->get_type() ==
		 expr_id::resolved::resolved_type::enumerator) {
	if (id_tok.get_value() ==
	    (_ast.get_pp_tokens()[r_it->get_enumerator().get_id_tok()]
	     .get_value())) {
	  ei.set_resolved(*r_it);
	  return;
	}
      } else if (r_it->get_type() ==
		 expr_id::resolved::resolved_type::in_param_id_list) {
	const identifier_list &pil = r_it->get_param_id_list();
	for (auto pi_tok : pil.get_identifiers()) {
	  if (id_tok.get_value() == _ast.get_pp_tokens()[pi_tok].get_value()) {
	    ei.set_resolved(*r_it);
	    return;
	  }
	}
      }
    }
  }


  // Otherwise check whether the identifier refers to a builtin
  // and if so, report that fact.
  static std::set<std::string> builtin_ids = _init_builtin_ids_set();
  if (builtin_ids.count(id_tok.get_value())) {
    ei.set_resolved(expr_id::resolved(expr_id::resolved::builtin_tag{}));
    return;
  }


  // Accept resolving failure for the second slot of
  // __builtin_offsetof(), i.e. the member designator.
  for (const _ast_entity *p = ei.get_parent(); dynamic_cast<const expr*>(p);
       p = p->get_parent()) {
    if (dynamic_cast<const expr_builtin_offsetof*>(p))
      return;
  }

  // Resolving the identifier failed. Silently accept that for expr_id's
  // being part of attributes.
  for (const _ast_entity *p = ei.get_parent(); p; p = p->get_parent()) {
    if (dynamic_cast<const attribute*>(p))
      return;
  }


  if (dynamic_cast<expr_func_invocation*>(ei.get_parent())) {
    // warn only about calls to undeclared functions
    code_remark remark(code_remark::severity::warning,
		       "identifier not declared",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    return;
  }

  code_remark remark(code_remark::severity::fatal,
		     "identifier not declared",
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
    for (auto r_it = scope_it->rbegin(); r_it != scope_it->rend(); ++r_it) {
      if (r_it->get_type() ==
	  expr_id::resolved::resolved_type::direct_declarator_id) {
	if (id_tok.get_value() ==
	    (_ast.get_pp_tokens()[r_it->get_direct_declarator().get_id_tok()]
	     .get_value())) {
	  ts_tdid.set_resolved(r_it->get_direct_declarator());
	  return;
	}
      }
    }
  }

  code_remark remark(code_remark::severity::fatal,
		     "typedef identifier not declared",
		     id_tok.get_file_range());
  _ast.get_remarks().add(remark);
  throw semantic_except(remark);
}


void ast::ast::_resolve_ids()
{
  _id_resolver ir(*this);
  this->for_each_dfs_pre_and_po(ir);
}



void ast::ast::resolve()
{
  _register_labels();
  _resolve_ids();
}
