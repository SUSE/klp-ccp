#include <cassert>
#include "ast.hh"
#include "semantic_except.hh"

using namespace suse::cp;
using namespace suse::cp::ast;

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

void ast::ast::_register_labels()
{
  this->for_each_dfs_po([&](_ast_entity* ae) {
      stmt_labeled *sl = dynamic_cast<stmt_labeled*>(ae);
      if (sl) {
	_register_label_scope_finder rlsf(*this, sl->get_label_tok());
	sl->for_each_parent<type_set<expr_statement> >(rlsf);
	assert(rlsf.get_result());
	rlsf.get_result()->register_label(sl);
      }
    });
}

void ast::ast::resolve()
{
  _register_labels();
}
