#include <cassert>
#include <set>
#include <string>
#include "ast_impl.hh"
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
    _register_label_scope_finder(suse::cp::ast::ast &ast,
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

    void operator()(const expr_statement&)
    {
      // The ancestor search stops here.
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
    suse::cp::ast::ast &_ast;
    const pp_token_index _label_tok;
    stmt_compound *_registrar;
  };
}

void suse::cp::ast::ast::_register_labels()
{
  this->for_each_dfs_po([&](_ast_entity &ae) {
      stmt_labeled *sl = dynamic_cast<stmt_labeled*>(&ae);
      if (sl) {
	_register_label_scope_finder rlsf(*this, sl->get_label_tok());
	sl->for_each_ancestor<type_set<expr_statement> >(rlsf);
	assert(rlsf.get_result());
	rlsf.get_result()->register_label(sl);
      }
    });
}


const declaration& init_declarator::get_containing_declaration() const noexcept
{
  assert(dynamic_cast<const init_declarator_list*>(get_parent()));
  const declaration *d
    = dynamic_cast<const declaration*>(get_parent()->get_parent());
  assert(d);
  return *d;
}


const _ast_entity& direct_declarator::get_first_non_declarator_parent()
  const noexcept
{
  const _ast_entity *p;
  for (p = get_parent(); p; p = p->get_parent()) {
    if (!dynamic_cast<const declarator*>(p) &&
	!dynamic_cast<const direct_declarator*>(p)) {
      break;
    }
  }

  assert(p);
  return *p;
}


namespace
{
  class _id_resolver
  {
  public:
    _id_resolver(suse::cp::ast::ast &ast);

    bool operator()(_ast_entity &ae, const pre_traversal_tag&);

    void operator()(_ast_entity&, const post_traversal_tag&);

  private:
    void _enter_scope();
    void _leave_scope();

    const expr_id::resolved* _lookup_id(const pp_token_index id_tok,
					bool *in_cur_scope = nullptr,
					bool skip_newest_scope = false)
      const noexcept;

    template <typename predicate_type>
    const expr_id::resolved* _lookup_id(const pp_token_index id_tok,
					predicate_type &&pred,
					bool *in_cur_scope = nullptr,
					bool skip_newest_scope = false)
      const noexcept;

    const sou_decl_link* _lookup_sou_decl(const pp_token_index id_tok,
					  const bool only_in_cur_scope)
      const noexcept;

    enum_def* _lookup_enum_def(const pp_token_index id_tok,
			       const bool only_in_cur_scope)
      const noexcept;

    void _handle_direct_declarator_id(direct_declarator_id &ddid);
    void _handle_init_decl(direct_declarator_id &ddid);
    void _handle_fun_def(direct_declarator_id &ddid);
    void _handle_sou_ref(struct_or_union_ref &sour);
    void _handle_sou_def(struct_or_union_def &soud);
    void _handle_enum_def(enum_def &ed);
    void _handle_enum_ref(enum_ref &er);

    static linkage::linkage_type
    _get_linkage_type(const direct_declarator_id::context &ctx) noexcept;
    static void _link_to(linkage &l,
			 const direct_declarator_id::context &target_ctx)
      noexcept;
    void _add_pending_linkage(direct_declarator_id &ddid,
			      const linkage::linkage_type type);
    bool _try_resolve_pending_linkages(direct_declarator_id &ddid,
				       const linkage::linkage_type type);

    void _resolve_id(expr_id &ei);
    void _resolve_id(type_specifier_tdid &ts_tdid);

    class _declarator_id_ctx_finder
    {
    public:
      _declarator_id_ctx_finder(direct_declarator_id &ddid) noexcept
	: _ddid(ddid)
      {}

      bool operator()(const declarator&) const noexcept
      { return true; }

      bool operator()(const direct_declarator&) const noexcept
      { return true; }

      void operator()(struct_declarator &sd) const noexcept;
      void operator()(parameter_declaration_declarator &pdd) const noexcept;
      void operator()(init_declarator &id) const noexcept;
      void operator()(function_definition &fd) const noexcept;

    private:
      direct_declarator_id &_ddid;
    };

    suse::cp::ast::ast &_ast;

    struct _scope
    {
      std::vector<expr_id::resolved> _declared_ids;
      std::vector<sou_decl_link> _declared_sous;
      std::vector<std::reference_wrapper<enum_def> > _defined_enums;
    };
    typedef std::vector<expr_id::resolved> _scope_type;
    typedef std::vector<_scope> _scopes_type;
    _scopes_type _scopes;

    std::vector<std::reference_wrapper<direct_declarator_id> >
    _pending_linkages;
  };

}

_id_resolver::_id_resolver(suse::cp::ast::ast &ast)
  : _ast(ast)
{
  _enter_scope();
}

static bool _is_fundef_ddf(direct_declarator_func const *ddf)
{
  // Check that the first non-declarator/-direct_declarator parent is
  // a function_definition instance.
  if (dynamic_cast<const function_definition*>
      (&ddf->get_first_non_declarator_parent())) {
    return true;
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
      dynamic_cast<stmt_if*>(&ae) ||
      dynamic_cast<stmt_switch*>(&ae) ||
      dynamic_cast<stmt_for_init_decl*>(&ae) ||
      dynamic_cast<stmt_for_init_expr*>(&ae) ||
      dynamic_cast<stmt_while*>(&ae) ||
      dynamic_cast<stmt_do*>(&ae)) {
    _enter_scope();
    return true;
  }

  if (dynamic_cast<stmt*>(&ae)) {
    _ast_entity * const p = ae.get_parent();
    assert(p);
    if (dynamic_cast<stmt_if*>(p) ||
	dynamic_cast<stmt_switch*>(p) ||
	dynamic_cast<stmt_for_init_decl*>(p) ||
	dynamic_cast<stmt_for_init_expr*>(p) ||
	dynamic_cast<stmt_while*>(p) ||
	dynamic_cast<stmt_do*>(p)) {
      _enter_scope();
      return true;
    }
    return false;
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
    _scopes.back()._declared_ids.push_back(expr_id::resolved(*e));
    return false;
  }

  identifier_list *pil = dynamic_cast<identifier_list*>(&ae);
  if (pil) {
    if (_is_fundef_ddf_pil(pil))
      _scopes.back()._declared_ids.push_back(expr_id::resolved(*pil));

    return false;
  }

  struct_or_union_ref *sour = dynamic_cast<struct_or_union_ref*>(&ae);
  if (sour) {
    _handle_sou_ref(*sour);
    return false;
  }

  struct_or_union_def *soud = dynamic_cast<struct_or_union_def*>(&ae);
  if (soud) {
    _handle_sou_def(*soud);
    return false;
  }

  enum_def *ed = dynamic_cast<enum_def*>(&ae);
  if (ed) {
    _handle_enum_def(*ed);
    return false;
  }

  enum_ref *er = dynamic_cast<enum_ref*>(&ae);
  if (er) {
    _handle_enum_ref(*er);
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

void _id_resolver::operator()(_ast_entity&, const post_traversal_tag&)
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

const expr_id::resolved* _id_resolver::_lookup_id(const pp_token_index id_tok,
						  bool *in_cur_scope,
						  bool skip_newest_scope)
  const noexcept
{
  return _lookup_id(id_tok, [](const expr_id::resolved&) { return true; },
		    in_cur_scope, skip_newest_scope);
}

template <typename predicate_type>
const expr_id::resolved* _id_resolver::_lookup_id(const pp_token_index id_tok,
						  predicate_type &&pred,
						  bool *in_cur_scope,
						  bool skip_newest_scope)
  const noexcept
{
  const expr_id::resolved *result = nullptr;

  if (in_cur_scope)
    *in_cur_scope = false;

  if (skip_newest_scope)
    assert(_scopes.size() > 1);

  const pp_token &id = _ast.get_pp_tokens()[id_tok];
  const auto &scopes_begin =
    !skip_newest_scope ? _scopes.rbegin() : _scopes.rbegin() + 1;
  for (auto scope_it = scopes_begin; scope_it != _scopes.rend(); ++scope_it) {
    for (auto r_it = scope_it->_declared_ids.rbegin();
	 !result && r_it != scope_it->_declared_ids.rend();
	 ++r_it) {
      if (r_it->get_type() ==
	  expr_id::resolved::resolved_type::direct_declarator_id) {
	if (id.get_value() ==
	    (_ast.get_pp_tokens()[r_it->get_direct_declarator_id().get_id_tok()]
	     .get_value())) {
	  result = &*r_it;
	}
      } else if (r_it->get_type() ==
		 expr_id::resolved::resolved_type::enumerator) {
	if (id.get_value() ==
	    (_ast.get_pp_tokens()[r_it->get_enumerator().get_id_tok()]
	     .get_value())) {
	  result = &*r_it;
	}
      } else if (r_it->get_type() ==
		 expr_id::resolved::resolved_type::in_param_id_list) {
	const identifier_list &pil = r_it->get_param_id_list();
	for (auto pi_tok : pil.get_identifiers()) {
	  if (id.get_value() == _ast.get_pp_tokens()[pi_tok].get_value()) {
	    result = &*r_it;
	    break;
	  }
	}
      } else {
	assert(0);
	__builtin_unreachable();
      }

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
_lookup_sou_decl(const pp_token_index id_tok, const bool only_in_cur_scope)
  const noexcept
{
  const pp_token &_id_tok = _ast.get_pp_tokens()[id_tok];
  assert(_scopes.size() > 0);
  const auto scopes_end =
    (!only_in_cur_scope ?
     _scopes.rend() :
     _scopes.rbegin() + 1);
  for (auto scope_it = _scopes.rbegin(); scope_it != scopes_end; ++scope_it) {
    for (auto d_it = scope_it->_declared_sous.begin();
	 d_it != scope_it->_declared_sous.end(); ++d_it) {
      pp_token_index d_id_tok;
      switch (d_it->get_target_type()) {
      case sou_decl_link::target_type::ref:
	d_id_tok = d_it->get_target_sou_ref().get_id_tok();
	break;

      case sou_decl_link::target_type::def:
	d_id_tok = d_it->get_target_sou_def().get_id_tok();
	break;

      case sou_decl_link::target_type::unlinked:
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

void _id_resolver::_handle_direct_declarator_id(direct_declarator_id &ddid)
{
  const _declarator_id_ctx_finder dcf(ddid);
  ddid.for_each_ancestor<type_set<struct_declarator,
				  init_declarator,
				  function_definition,
				  parameter_declaration_declarator> >(dcf);

  switch (ddid.get_context().get_type())
  {
  case direct_declarator_id::context::context_type::unknown:
    assert(0);
    __builtin_unreachable();

  case direct_declarator_id::context::context_type::struct_decl:
    return;

  case direct_declarator_id::context::context_type::parameter_decl:
    _scopes.back()._declared_ids.push_back(expr_id::resolved(ddid));
    return;

  case direct_declarator_id::context::context_type::init_decl:
    _handle_init_decl(ddid);
    return;

  case direct_declarator_id::context::context_type::function_def:
    _handle_fun_def(ddid);
    return;
  }
}

void _id_resolver::_handle_init_decl(direct_declarator_id &ddid)
{
  init_declarator &id = ddid.get_context().get_init_declarator();
  const declaration &d = id.get_containing_declaration();
  const storage_class sc
    = d.get_declaration_specifiers().get_storage_class(_ast);
  const bool is_fun = ddid.is_function();
  const bool is_at_file_scope = d.is_at_file_scope();


  // Find a previous declaration visible in the current scope, if any.
  bool prev_is_in_cur_scope;
  const expr_id::resolved *prev = _lookup_id(ddid.get_id_tok(),
					     &prev_is_in_cur_scope);

  // Let's be good citizens and check for forbidden redeclarations in
  // the same scope.
  const bool is_local_nonfun =
    !is_fun && !is_at_file_scope && sc != storage_class::sc_extern;
  const bool no_linkage =
    ((sc != storage_class::sc_none &&
      sc != storage_class::sc_static &&
      sc != storage_class::sc_extern &&
      !(is_fun && !is_at_file_scope && sc == storage_class::sc_auto)) ||
     is_local_nonfun);
  if (prev && prev_is_in_cur_scope &&
      (no_linkage ||
       prev->get_type() == expr_id::resolved::resolved_type::enumerator)) {

    // typedef redeclarations are possible if they yield the same result.
    const bool prev_is_typedef
      = ((prev->get_type() ==
	  expr_id::resolved::resolved_type::direct_declarator_id) &&
	 (prev->get_direct_declarator_id().get_context().get_type() ==
	  direct_declarator_id::context::context_type::init_decl) &&
	 ((prev->get_direct_declarator_id().get_context().get_init_declarator()
	   .get_containing_declaration().get_declaration_specifiers()
	   .get_storage_class(_ast)) ==
	  storage_class::sc_typedef));

    // In old-style parameter declarations, the declaration obviously
    // has to declare some id of the function prototype's identifier
    // list. So that's Ok.
    const bool is_oldstyle_param_decl
      = (is_local_nonfun &&
	 (prev->get_type() ==
	  expr_id::resolved::resolved_type::in_param_id_list) &&
	 dynamic_cast<const declaration_list*>(d.get_parent()));

    if (!(is_oldstyle_param_decl ||
	  (sc == storage_class::sc_typedef && prev_is_typedef))) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "invalid redeclaration",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
    }
  }

  if (no_linkage) {
    _scopes.back()._declared_ids.push_back(expr_id::resolved(ddid));
    return;
  }


  assert((sc == storage_class::sc_none ||
	  sc == storage_class::sc_static ||
	  sc == storage_class::sc_extern) ||
	 (is_fun && !is_at_file_scope && sc == storage_class::sc_auto));
  assert(is_fun || is_at_file_scope || sc == storage_class::sc_extern);

  if (is_at_file_scope && sc == storage_class::sc_static) {
    // Internal linkage.
    if (prev) {
      assert(prev_is_in_cur_scope &&
	     (prev->get_type() ==
	      expr_id::resolved::resolved_type::direct_declarator_id));

      direct_declarator_id &prev_ddid = prev->get_direct_declarator_id();
      const linkage::linkage_type prev_linkage =
	_get_linkage_type(prev_ddid.get_context());
      if (prev_linkage != linkage::linkage_type::internal) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark(code_remark::severity::fatal,
			   "static declaration follows non-static one",
			   id_tok.get_file_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(id.get_linkage(), prev_ddid.get_context());

    } else {
      _try_resolve_pending_linkages(ddid, linkage::linkage_type::internal);
    }

  } else if (sc == storage_class::sc_extern ||
	     (sc == storage_class::sc_none && is_fun)) {
    // Linkage of previous declaration, if any. Otherwise external.
    direct_declarator_id* prev_ddid = nullptr;
    if (prev) {
      auto get_ddid_if_linkage =
	[](const expr_id::resolved &r) {
	switch(r.get_type()) {
	case expr_id::resolved::resolved_type::direct_declarator_id:
	  {
	    direct_declarator_id &_prev_ddid = r.get_direct_declarator_id();
	    const linkage::linkage_type prev_linkage =
	      _get_linkage_type(_prev_ddid.get_context());
	    if (prev_linkage != linkage::linkage_type::none)
	      return &_prev_ddid;
	  }
	  return static_cast<direct_declarator_id*>(nullptr);

	case expr_id::resolved::resolved_type::enumerator:
	  return static_cast<direct_declarator_id*>(nullptr);

	case expr_id::resolved::resolved_type::in_param_id_list:
	  return static_cast<direct_declarator_id*>(nullptr);

	case expr_id::resolved::resolved_type::none:
	/* fall through */
	case expr_id::resolved::resolved_type::builtin:
	/* fall through */
	case expr_id::resolved::resolved_type::stmt_labeled:
	  assert(0);
	  __builtin_unreachable();
	}
      };

      prev_ddid = get_ddid_if_linkage(*prev);
      if (!prev_ddid) {
	// Try to find a visible declaration which actually has a linkage.
	prev = _lookup_id(ddid.get_id_tok(), get_ddid_if_linkage);
	if (prev) {
	  prev_ddid = &prev->get_direct_declarator_id();
	  const linkage::linkage_type prev_linkage =
	    _get_linkage_type(prev_ddid->get_context());
	  if (prev_linkage != linkage::linkage_type::external) {
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

    if (prev_ddid) {
      _link_to(id.get_linkage(), prev_ddid->get_context());
    } else {
      _add_pending_linkage(ddid, linkage::linkage_type::external);
    }

  } else if (is_at_file_scope) {
    assert(sc == storage_class::sc_none && !is_fun);
    // Object at file scope with no storage class specifier:
    // external linkage.
    if (prev) {
      assert(prev_is_in_cur_scope &&
	     (prev->get_type() ==
	      expr_id::resolved::resolved_type::direct_declarator_id));

      direct_declarator_id &prev_ddid = prev->get_direct_declarator_id();
      const linkage::linkage_type prev_linkage =
	_get_linkage_type(prev_ddid.get_context());
      if (prev_linkage != linkage::linkage_type::external) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark(code_remark::severity::fatal,
			   "non-static declaration follows static one",
			   id_tok.get_file_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(id.get_linkage(), prev_ddid.get_context());

    } else {
      _try_resolve_pending_linkages(ddid, linkage::linkage_type::external);
    }

  } else if (is_fun && !is_at_file_scope && sc == storage_class::sc_auto) {
    // This branch deals with the gcc extension of having an 'auto' specifier
    // for linking to nested functions in the same scope.
    if (prev && prev_is_in_cur_scope) {
      assert(prev->get_type() ==
	     expr_id::resolved::resolved_type::direct_declarator_id);

      direct_declarator_id &prev_ddid = prev->get_direct_declarator_id();
      const linkage::linkage_type prev_linkage =
	_get_linkage_type(prev_ddid.get_context());
      if (prev_linkage != linkage::linkage_type::nested_fun_auto) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark{
	  code_remark::severity::fatal,
	  "auto nested function declaration follows an external one",
	  id_tok.get_file_range()
	};
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(id.get_linkage(), prev_ddid.get_context());

    } else {
      id.get_linkage()
	.set_linkage_type(linkage::linkage_type::nested_fun_auto);
    }

  } else {
    assert(sc == storage_class::sc_static && is_fun && !is_at_file_scope);
    // That's not allowed.
    const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "static specifier at non-local function declaration",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _scopes.back()._declared_ids.push_back(expr_id::resolved(ddid));
}

void _id_resolver::_handle_fun_def(direct_declarator_id &ddid)
{
  if (!ddid.is_function()) {
      const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
      code_remark remark(code_remark::severity::fatal,
			 "no function declarator in definition's prototype",
			 id_tok.get_file_range());
      _ast.get_remarks().add(remark);
      throw semantic_except(remark);
  }

  function_definition &fd = ddid.get_context().get_function_definition();
  storage_class sc
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
						    &prev_is_in_cur_scope,
						    true);

  // Check for forbidden redeclarations in the same scope.
  if (prev && prev_is_in_cur_scope &&
      (prev->get_type() == expr_id::resolved::resolved_type::enumerator ||
       prev->get_type() == expr_id::resolved::resolved_type::in_param_id_list))
    {
    const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
    code_remark remark(code_remark::severity::fatal,
		       "invalid redeclaration",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  if (!is_at_file_scope) {
    // Nested functions have 'auto' linkage.
    if (prev && prev_is_in_cur_scope) {
      assert(prev->get_type() ==
	     expr_id::resolved::resolved_type::direct_declarator_id);

      direct_declarator_id &prev_ddid = prev->get_direct_declarator_id();
      const linkage::linkage_type prev_linkage =
	_get_linkage_type(prev_ddid.get_context());
      if (prev_linkage != linkage::linkage_type::nested_fun_auto) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark{
	  code_remark::severity::fatal,
	  "nested function definition follows an external declaration",
	  id_tok.get_file_range()
	};
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(fd.get_linkage(), prev_ddid.get_context());

    } else {
      fd.get_linkage().set_linkage_type(linkage::linkage_type::nested_fun_auto);
    }

  } else if (sc == storage_class::sc_static) {
    assert(is_at_file_scope);
    // Internal linkage.
    if (prev) {
      assert(prev->get_type() ==
	     expr_id::resolved::resolved_type::direct_declarator_id);

      direct_declarator_id &prev_ddid = prev->get_direct_declarator_id();
      const linkage::linkage_type prev_linkage =
	_get_linkage_type(prev_ddid.get_context());
      if (prev_linkage != linkage::linkage_type::internal) {
	const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
	code_remark remark(code_remark::severity::fatal,
			   "static declaration follows non-static one",
			   id_tok.get_file_range());
	_ast.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      _link_to(fd.get_linkage(), prev_ddid.get_context());

    } else {
      _try_resolve_pending_linkages(ddid, linkage::linkage_type::internal);
    }

  } else {
    assert(sc == storage_class::sc_extern || sc == storage_class::sc_none);
    // Linkage of previous declaration, if any. Otherwise external.
    if (prev) {
      assert(prev->get_type() ==
	     expr_id::resolved::resolved_type::direct_declarator_id);
      direct_declarator_id &prev_ddid = prev->get_direct_declarator_id();
      const linkage::linkage_type prev_linkage =
	_get_linkage_type(prev_ddid.get_context());
      if (prev_linkage == linkage::linkage_type::none) {
	fd.get_linkage().set_linkage_type(linkage::linkage_type::external);
      }

      _link_to(fd.get_linkage(), prev_ddid.get_context());

    } else {
      _try_resolve_pending_linkages(ddid, linkage::linkage_type::external);
    }
  }

  // A function definition's id declarator must get added to parent
  // scope.
  assert(_scopes.size() >= 2);
  (_scopes.end() - 2)->_declared_ids.push_back(expr_id::resolved(ddid));
}

void _id_resolver::_handle_sou_ref(struct_or_union_ref &sour)
{
  bool is_standalone_decl;
  auto &&standalone_decl_checker
    = (wrap_callables<no_default_action>
       ([](const specifier_qualifier_list&) {
	 // Move on searching upwards the tree.
	 return true;
	},
	[&is_standalone_decl](const struct_declaration_c99 &sd) {
	  is_standalone_decl = sd.get_struct_declarator_list().empty();
	},
	[&is_standalone_decl](const struct_declaration_unnamed_sou&) {
	  // struct_declaration_unnamed_sou can be a parent of a
	  // specifier_qualifier_list. A specifier_qualifier_list can
	  // in turn be a parent of our struct_or_union_ref.
	  // However, these can't be both true for the *same*
	  // specifier_qualifier_list instance.
	  assert(0);
	  __builtin_unreachable();
	},
	[&is_standalone_decl](const type_name&) {
	  is_standalone_decl = false;
	},
	[&is_standalone_decl](const declaration &d) {
	  is_standalone_decl = !d.get_init_declarator_list();
	},
	[&is_standalone_decl](const parameter_declaration_declarator&) {
	  is_standalone_decl = false;
	},
	[&is_standalone_decl](const parameter_declaration_abstract&) {
	  is_standalone_decl = false;
	},
	[&is_standalone_decl](const function_definition&) {
	  is_standalone_decl = false;
	}));
  sour.for_each_ancestor<type_set<struct_declaration_c99,
				  struct_declaration_unnamed_sou,
				  type_name, declaration,
				  parameter_declaration_declarator,
				  parameter_declaration_abstract,
				  function_definition> >
    (standalone_decl_checker);

  // If the 'struct foo' construct is standalone, i.e. is a
  // declaration of that tag, then search only the current scope.
  const sou_decl_link *prev_decl = _lookup_sou_decl(sour.get_id_tok(),
						    is_standalone_decl);

  struct_or_union prev_tag_kind;
  if (is_standalone_decl && prev_decl) {
    // It's a redeclaration, link it to the previous one.
    switch (prev_decl->get_target_type()) {
    case sou_decl_link::target_type::ref:
      prev_tag_kind = prev_decl->get_target_sou_ref().get_tag_kind();
      sour.get_decl_list_node().link_to(prev_decl->get_target_sou_ref());
      break;

    case sou_decl_link::target_type::def:
      prev_tag_kind = prev_decl->get_target_sou_def().get_tag_kind();
      sour.get_decl_list_node().link_to(prev_decl->get_target_sou_def());
      break;

    case sou_decl_link::target_type::unlinked:
      assert(0);
      __builtin_unreachable();
    }

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
    assert (!is_standalone_decl && prev_decl);
    // It isnt't a declaration for that tag, but a real usage.
    switch (prev_decl->get_target_type()) {
    case sou_decl_link::target_type::ref:
      prev_tag_kind = prev_decl->get_target_sou_ref().get_tag_kind();
      sour.link_to_declaration(prev_decl->get_target_sou_ref());
      break;

    case sou_decl_link::target_type::def:
      prev_tag_kind = prev_decl->get_target_sou_def().get_tag_kind();
      sour.link_to_declaration(prev_decl->get_target_sou_def());
      break;

    case sou_decl_link::target_type::unlinked:
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
						    true);
  if (prev_decl) {
    // It's a redeclaration, link it to the previous one.
    struct_or_union prev_tag_kind;
    struct_or_union_def *prev_def = nullptr;
    switch (prev_decl->get_target_type()) {
    case sou_decl_link::target_type::ref:
      {
	struct_or_union_ref &prev_sou_ref = prev_decl->get_target_sou_ref();
	prev_def = prev_sou_ref.get_decl_list_node().find_definition();
	prev_tag_kind = prev_sou_ref.get_tag_kind();
	soud.get_decl_list_node().link_to(prev_sou_ref);
      }
      break;

    case sou_decl_link::target_type::def:
      {
	struct_or_union_def &prev_sou_def = prev_decl->get_target_sou_def();
	prev_def = &prev_sou_def;
	prev_tag_kind = prev_sou_def.get_tag_kind();
	soud.get_decl_list_node().link_to(prev_sou_def);
      }
      break;

    case sou_decl_link::target_type::unlinked:
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

    _scopes.back()._declared_sous.push_back(sou_decl_link(soud));
  }
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
  } else if (_lookup_sou_decl(ed.get_id_tok(), true)) {
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

linkage::linkage_type
_id_resolver::_get_linkage_type(const direct_declarator_id::context &ctx)
  noexcept
{
  switch (ctx.get_type()) {
  case direct_declarator_id::context::context_type::init_decl:
    return ctx.get_init_declarator().get_linkage().get_linkage_type();

  case direct_declarator_id::context::context_type::function_def:
    return ctx.get_function_definition().get_linkage().get_linkage_type();

  case direct_declarator_id::context::context_type::unknown:
    /* fall through */
  case direct_declarator_id::context::context_type::struct_decl:
    /* fall through */
  case direct_declarator_id::context::context_type::parameter_decl:
    assert(0);
    __builtin_unreachable();
  }
}

void _id_resolver::_link_to(linkage &l,
			    const direct_declarator_id::context &target_ctx)
  noexcept
{
  const linkage::linkage_type type = _get_linkage_type(target_ctx);

  switch (target_ctx.get_type()) {
  case direct_declarator_id::context::context_type::init_decl:
    l.link_to(target_ctx.get_init_declarator(), type);
    break;

  case direct_declarator_id::context::context_type::function_def:
    l.link_to(target_ctx.get_function_definition(), type);
    break;

  case direct_declarator_id::context::context_type::unknown:
    /* fall through */
  case direct_declarator_id::context::context_type::struct_decl:
    /* fall through */
  case direct_declarator_id::context::context_type::parameter_decl:
    assert(0);
    __builtin_unreachable();
  }
}

void _id_resolver::_add_pending_linkage(direct_declarator_id &ddid,
					const linkage::linkage_type type)
{
  if (_try_resolve_pending_linkages(ddid, type))
    return;
  _pending_linkages.push_back(std::ref(ddid));
}

bool
_id_resolver::_try_resolve_pending_linkages(direct_declarator_id &ddid,
					    const linkage::linkage_type type)
{
  const pp_token &id_tok = _ast.get_pp_tokens()[ddid.get_id_tok()];
  auto it_pl = _pending_linkages.begin();
  for (; it_pl != _pending_linkages.end(); ++it_pl) {
    const pp_token &pl_id_tok =
      _ast.get_pp_tokens()[it_pl->get().get_id_tok()];
    if (id_tok.get_value() == pl_id_tok.get_value())
      break;
  }

  bool is_at_file_scope;
  linkage *l;
  const direct_declarator_id::context &ctx = ddid.get_context();
  switch(ctx.get_type()) {
  case direct_declarator_id::context::context_type::init_decl:
    l = &ctx.get_init_declarator().get_linkage();
    is_at_file_scope =
      ctx.get_init_declarator().get_containing_declaration().is_at_file_scope();
    break;

  case direct_declarator_id::context::context_type::function_def:
    l = &ctx.get_function_definition().get_linkage();
    is_at_file_scope =
      ctx.get_function_definition().is_at_file_scope();
    break;

  case direct_declarator_id::context::context_type::unknown:
    /* fall through */
  case direct_declarator_id::context::context_type::struct_decl:
    /* fall through */
  case direct_declarator_id::context::context_type::parameter_decl:
    assert(0);
    __builtin_unreachable();
  }

  if (it_pl == _pending_linkages.end()) {
    l->set_linkage_type(type);
    return is_at_file_scope;
  }

  const direct_declarator_id::context &pl_ctx = it_pl->get().get_context();
  const linkage::linkage_type pl_linkage_type
    = _get_linkage_type(pl_ctx);
  assert(pl_linkage_type == linkage::linkage_type::external);

  if (type != pl_linkage_type) {
    code_remark remark(code_remark::severity::fatal,
		       "static declaration follows external one",
		       id_tok.get_file_range());
    _ast.get_remarks().add(remark);
    throw semantic_except(remark);
  }

  _link_to(*l, pl_ctx);
  if (is_at_file_scope) {
    // Subsequent extern declarations will be able to discover this.
    _pending_linkages.erase(it_pl);
    return true;
  }

  return false;
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
  const expr_id::resolved *resolved_id = _lookup_id(ei.get_id_tok());
  if (resolved_id) {
    ei.set_resolved(*resolved_id);
    return;
  }


  // Otherwise check whether the identifier refers to a builtin
  // and if so, report that fact.
  const pp_token &id_tok = _ast.get_pp_tokens()[ei.get_id_tok()];
  static std::set<std::string> builtin_ids =
    _init_builtin_ids_set(_builtin_ids);
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
    for (auto r_it = scope_it->_declared_ids.rbegin();
	 r_it != scope_it->_declared_ids.rend(); ++r_it) {
      if (r_it->get_type() ==
	  expr_id::resolved::resolved_type::direct_declarator_id) {
	if (id_tok.get_value() ==
	    (_ast.get_pp_tokens()[r_it->get_direct_declarator_id().get_id_tok()]
	     .get_value())) {
	  ts_tdid.set_resolved(r_it->get_direct_declarator_id());
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
    return;
  }

  code_remark remark(code_remark::severity::fatal,
		     "typedef identifier not declared",
		     id_tok.get_file_range());
  _ast.get_remarks().add(remark);
  throw semantic_except(remark);
}


void
_id_resolver::_declarator_id_ctx_finder::operator()(struct_declarator &sd)
  const noexcept
{
  _ddid.set_context(direct_declarator_id::context(sd));
  // The ancestor search stops here.
}

void _id_resolver::_declarator_id_ctx_finder::
operator()(parameter_declaration_declarator &pdd) const noexcept
{
  _ddid.set_context(direct_declarator_id::context(pdd));
  // The ancestor search stops here.
}

void _id_resolver::_declarator_id_ctx_finder::operator()(init_declarator& id)
  const noexcept
{
  _ddid.set_context(direct_declarator_id::context(id));
  // The ancestor search stops here.
}

void
_id_resolver::_declarator_id_ctx_finder::operator()(function_definition& fd)
  const noexcept
{
  _ddid.set_context(direct_declarator_id::context(fd));
  // The ancestor search stops here.
}



void suse::cp::ast::ast::_resolve_ids()
{
  _id_resolver ir(*this);
  this->for_each_dfs_pre_and_po(ir);
}

void suse::cp::ast::ast::resolve()
{
  _register_labels();
  _resolve_ids();
}
