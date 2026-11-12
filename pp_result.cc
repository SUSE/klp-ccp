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
#include "pp_result.hh"
#include "pp_result_impl.hh"
#include "raw_pp_token.hh"
#include "source_reader.hh"
#include <limits>
#include <algorithm>
#include <iterator>

using namespace klp::ccp;


pp_result::directive::directive(const raw_pp_tokens_range &directive_range)
  noexcept
  : _directive_range(directive_range)
{}


pp_result::macro::macro(const std::string &name,
			const bool func_like,
			const bool variadic,
			std::vector<std::string> &&arg_names,
			raw_pp_tokens &&repl,
			const raw_pp_tokens_range &repl_range,
			const raw_pp_tokens_range &directive_range)
  : _name(name), _arg_names(std::move(arg_names)),
    _repl(std::move(repl)),
    _repl_range(repl_range),
    _o(_origin::o_source),
    _directive_range(directive_range),
    _func_like(func_like),
    _variadic(variadic)
{
  _init_do_expand_args();
}

pp_result::macro::macro(const std::string &name,
			const unsigned long predefinition_pos,
			const builtin_special_tag&)
  : _name(name),
    _o(_origin::o_builtin_special),
    _predefinition_pos(predefinition_pos),
    _func_like(false),
    _variadic(false)
{}

pp_result::macro::macro(const std::string &name,
			std::vector<std::string> &&arg_names,
			const unsigned long predefinition_pos,
			const builtin_special_tag&)
  : _name(name), _arg_names(std::move(arg_names)),
    _o(_origin::o_builtin_special),
    _predefinition_pos(predefinition_pos),
    _func_like(true),
    _variadic(false)
{}

pp_result::macro::macro(const std::string &name,
			const bool func_like,
			const bool variadic,
			std::vector<std::string> &&arg_names,
			raw_pp_tokens &&repl,
			const unsigned long predefinition_pos,
			const builtin_tag&)
  : macro(name, func_like, variadic, std::move(arg_names), std::move(repl),
	  predefinition_pos, _origin::o_builtin)
{}

pp_result::macro::macro(const std::string &name,
	      const bool func_like,
	      const bool variadic,
	      std::vector<std::string> &&arg_names,
	      raw_pp_tokens &&repl,
	      const unsigned long predefinition_pos,
	      const predefined_user_tag&)
  : macro(name, func_like, variadic, std::move(arg_names), std::move(repl),
	  predefinition_pos, _origin::o_predefined_user)
{}

pp_result::macro::macro(const std::string &name,
			const bool func_like,
			const bool variadic,
			std::vector<std::string> &&arg_names,
			raw_pp_tokens &&repl,
			const unsigned long predefinition_pos,
			const _origin o)
  : _name(name), _arg_names(std::move(arg_names)), _repl(std::move(repl)),
    _o(o), _predefinition_pos(predefinition_pos),
    _func_like(func_like), _variadic(variadic)
{
  assert(this->is_predefined());
  _init_do_expand_args();
}

bool pp_result::macro::operator==(const macro &rhs) const noexcept
{
  if (!this->is_builtin_special() && !rhs.is_builtin_special()) {
    return (_name == rhs._name &&
	    _func_like == rhs._func_like &&
	    _arg_names == rhs._arg_names &&
	    _variadic == rhs._variadic &&
	    _repl == rhs._repl);
  } else {
    return (this->is_builtin_special() && rhs.is_builtin_special() &&
	    _name == rhs._name);
  }
}

const std::vector<std::string>& pp_result::macro::get_arg_names() const noexcept
{
  return _arg_names;
}

size_t pp_result::macro::non_va_arg_count() const noexcept
{
  assert(_func_like);
  if (_variadic)
    return _do_expand_args.size() - 1;

  return _do_expand_args.size();
}

bool pp_result::macro::shall_expand_arg(const size_t pos) const noexcept
{
  assert(_func_like);
  assert(pos <= _do_expand_args.size());
  return _do_expand_args[pos];
}

void pp_result::macro::_init_do_expand_args()
{
  // Due to the restriction that a ## concatenated token must again
  // yield a valid preprocessing token, macro evaluation can fail and
  // thus yield an error. Hence macro arguments must not get macro
  // expanded if not needed. An argument needs to get expanded if it
  // appears in the replacement list, ignoring operands of ## and
  // #. Determine those.
  std::set<std::string> do_expand;

  auto &&is_stringification =
    [&](const raw_pp_tokens::const_iterator &it_repl) {
      return _func_like && it_repl->is_punctuator("#");
    };

  auto &&skip_ws =
    [&](raw_pp_tokens::const_iterator it_repl) {
      // Continuation lines are represented as empty tokens in the
      // replacement list and may be surrounded by whitespace tokens
      // on both sides, so skip over sequences thereof.
      while (it_repl != _repl.cend() &&
	     (it_repl->is_ws() || it_repl->is_empty())) {
	++it_repl;
      }
      return it_repl;
    };

  auto &&advance_it_repl =
    [&](const raw_pp_tokens::const_iterator &it_repl) {
      assert(it_repl != _repl.cend());
      if (is_stringification(it_repl)) {
	assert(skip_ws(it_repl + 1) != _repl.cend());
	assert(skip_ws(it_repl + 1)->is_id());
	return skip_ws(skip_ws(it_repl + 1) + 1);
      }

      return skip_ws(it_repl + 1);
    };

  auto &&is_vaargs =
    [&](const raw_pp_tokens::const_iterator &it_repl) {
      return (_variadic && it_repl->is_id() &&
	      it_repl->get_value() == _arg_names.back());
    };

  for (raw_pp_tokens::const_iterator it = _repl.cbegin(), it_next_no_ws;
       it != _repl.cend(); it = it_next_no_ws) {
    it_next_no_ws = advance_it_repl(it);

    if (it_next_no_ws != _repl.cend() && it_next_no_ws->is_punctuator("##")) {
      do {
	auto it_next_concat_tok = skip_ws(it_next_no_ws + 1);
	assert(it_next_concat_tok != _repl.cend());

	// Take care of the special
	//  , ## __VA_ARGS__
	// construct.
	if (it->is_punctuator(",") && is_vaargs(it_next_concat_tok)) {
	  const auto it_repl_lookahead = skip_ws(it_next_concat_tok + 1);

	  if (it_repl_lookahead == _repl.cend() ||
	      !it_repl_lookahead->is_punctuator("##")) {
	    it_next_no_ws = it_next_concat_tok; // Continue at __VA_ARGS__
	    break;
	  }
	}

	it = it_next_concat_tok;
	if (it->is_punctuator("##")) {
	  // Multiple consecutive ## tokens.
	  it_next_no_ws = it;
	} else {
	  it_next_no_ws = advance_it_repl(it);
	}
      } while (it_next_no_ws != _repl.cend() &&
	       it_next_no_ws->is_punctuator("##"));

    } else if (it->is_id()) {
      if (std::find(_arg_names.cbegin(), _arg_names.cend(), it->get_value())
	  != _arg_names.cend()) {
	do_expand.insert(it->get_value());
      }

    }
  }

  _do_expand_args.resize(_arg_names.size(), false);
  for (size_t i = 0; i < _arg_names.size(); ++i) {
    if (do_expand.count(_arg_names[i]))
      _do_expand_args[i] = true;
  }
}


pp_result::macro_undef::macro_undef(const std::string &name,
				    const raw_pp_tokens_range &directive_range)
  : _name(name), _o(_origin::o_source), _directive_range(directive_range)
{}

pp_result::macro_undef::macro_undef(const std::string &name,
				    const unsigned long predefinition_pos,
				    const predefined_user_tag&)
  : _name(name), _o(_origin::o_predefined_user),
    _predefinition_pos(predefinition_pos)
{}

pp_result::used_macros::used_macros(_used_macros_type &&um)
  : _used_macros(std::move(um))
{}

void pp_result::used_macros::clear() noexcept
{
  _used_macros.clear();
}

std::size_t pp_result::used_macros::count(const macro &m) const noexcept
{
  return _used_macros.count(std::ref(m));
}

pp_result::used_macros&
pp_result::used_macros::operator+=(const used_macros &rhs)
{
  _used_macros.insert(rhs._used_macros.cbegin(),
		      rhs._used_macros.cend());
  return *this;
}

pp_result::used_macros&
pp_result::used_macros::operator+=(const macro &rhs)
{
  _used_macros.insert(std::ref(rhs));
  return *this;
}

bool pp_result::used_macros::_compare::
operator()(const std::reference_wrapper<const macro> a,
	   const std::reference_wrapper<const macro> b) const noexcept
{
  return &a.get() < &b.get();
}


pp_result::macro_nondef_constraint::
macro_nondef_constraint(const std::string &id, bool func_like_allowed)
  : _id(id), _func_like_allowed(func_like_allowed)
{}

pp_result::macro_nondef_constraints::
macro_nondef_constraints(macro_nondef_constraints &&mnc)
  : _macro_nondef_constraints(std::move(mnc._macro_nondef_constraints))
{}

bool pp_result::macro_nondef_constraint::
operator<(const macro_nondef_constraint &rhs)
  const
{
  return this->_id < rhs._id;
}

pp_result::macro_nondef_constraints&
pp_result::macro_nondef_constraints::operator=(macro_nondef_constraints &&rhs)
{
  if (this == &rhs)
    return *this;

  this->_macro_nondef_constraints = std::move(rhs._macro_nondef_constraints);
  return *this;
}

pp_result::macro_nondef_constraints& pp_result::macro_nondef_constraints::
operator+=(const macro_nondef_constraints &rhs)
{
  for (auto &mnc : rhs._macro_nondef_constraints)
    *this += mnc;
  return *this;
}

pp_result::macro_nondef_constraints& pp_result::macro_nondef_constraints::
operator+=(const macro_nondef_constraint &rhs)
{
  auto it = _macro_nondef_constraints.find(rhs);

  if (it != _macro_nondef_constraints.end()) {
    if (it->is_func_like_allowed() && !rhs.is_func_like_allowed()) {
      it = _macro_nondef_constraints.erase(it);
      _macro_nondef_constraints.insert(it, rhs);
    }
  } else {
      _macro_nondef_constraints.insert(rhs);
  }

  return *this;
}

pp_result::macro_nondef_constraints::const_iterator
pp_result::macro_nondef_constraints::find(const std::string &id) const noexcept
{
  return const_iterator{
    _macro_nondef_constraints.find(id)
  };
}


pp_result::macro_invocation::
macro_invocation
(const macro &m,
 expansion_forest::invocation_source &&initial_invocation_source)
  : _expansion_forest{m, std::move(initial_invocation_source)}
{
  _used_macros += m;
}

const pp_tokens_range pp_result::macro_invocation::
get_output_range(const pp_result &pp_result) const noexcept
{
  // This reverse lookup method works also for macro invocations not
  // represented in the final preprocessing output, because
  // e.g. they're invoked from some preprocessor directive. In this
  // case a "collapsed" point within the pp_tokens sequence resembling
  // their relative position within the raw_pp_tokens is getting
  // returned.
  return pp_result.raw_pp_tokens_range_to_nonraw(get_source_range());
}

const raw_pp_tokens_range pp_result::macro_invocation::
get_source_range() const noexcept {
  return
    _expansion_forest.get_invocation_source().get_total_spanned_source_range();
}

using expansion_forest=
  pp_result::macro_invocation::expansion_forest;

expansion_forest::_forest_nodes::
_forest_nodes(const macro &m,
	      const std::size_t initial_src_length)
{
  // Add initial root node.
  add_rescan_invocation
    (_rescan_macro_invocation{
       size_type::invalid(), m,
       rescan_macro_invocation::output::tokens_range{
	 0, initial_src_length
       }
     });
}

expansion_forest::_rescan_macro_invocation&
expansion_forest::_forest_nodes::operator[](const size_type n) noexcept
{
  assert(n._n <= _nodes.size());
  return _nodes[n._n];
}

const expansion_forest::_rescan_macro_invocation&
expansion_forest::_forest_nodes::operator[](const size_type n)
  const noexcept
{
  assert(n._n <= _nodes.size());
  return _nodes[n._n];
}


expansion_forest::_forest_nodes::size_type expansion_forest::_forest_nodes::
add_rescan_invocation(_rescan_macro_invocation &&rescan_invocation)
{
  assert(_nodes.empty() ||
	 (rescan_invocation._rescan_parent != size_type::invalid() &&
	  rescan_invocation._rescan_parent._n < _nodes.size()));
  assert(_nodes.empty() ||
	 ((*this)[rescan_invocation._rescan_parent].out_length() >
	  rescan_invocation._src_range.begin));
  _nodes.emplace_back(std::move(rescan_invocation));

  return size_type{_nodes.size() - 1};
}

expansion_forest::_forest_nodes::size_type expansion_forest::_forest_nodes::
add_macro_arg_tree(const _forest_nodes::size_type owner,
		   _rescan_macro_invocation &&arg_root_node)
{
  assert(arg_root_node.is_arg_tree_root());
  assert(owner != size_type::invalid() && owner._n < _nodes.size());
  assert((*this)[owner]._src_range.begin <= arg_root_node._src_range.begin);
  assert((*this)[owner]._src_range.end >= arg_root_node._src_range.end);
  assert(!(*this)[owner].out_length());
  _nodes.emplace_back(std::move(arg_root_node));
  ++(*this)[owner]._nargs;
  return size_type{_nodes.size() - 1};
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::first_arg_of(const size_type node)
  const noexcept
{
  assert((*this)[node].nargs() > 0);
  assert(node._n != _nodes.size());
  const size_type arg_root{node._n + 1};
  assert((*this)[arg_root].is_rescan_tree_root());

  return arg_root;
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::next_arg(const size_type prev_arg_root)
  const noexcept
{
  assert(prev_arg_root != size_type::invalid() &&
	 prev_arg_root._n > 0 && prev_arg_root._n < _nodes.size());
  assert((*this)[prev_arg_root].is_rescan_tree_root());
  const size_type next_arg_root = _rescan_subtree_end(prev_arg_root);
  assert(next_arg_root._n != _nodes.size());
  assert((*this)[next_arg_root].is_rescan_tree_root());

  return next_arg_root;
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::arg_owner(const size_type arg_root)
  const noexcept
{
  assert(arg_root != size_type::invalid() &&
	 arg_root._n > 0 && arg_root._n < _nodes.size());
  assert((*this)[arg_root].is_rescan_tree_root());
  _container_type::size_type pending_arg_roots = 1;
  _container_type::const_iterator it;

  it = _nodes.cbegin() + arg_root._n;
  while (it != _nodes.cbegin()) {
    const _container_type::const_iterator nit = it - 1;

    if (nit->nargs() >= pending_arg_roots)
      break;

    pending_arg_roots -= nit->nargs();
    pending_arg_roots += nit->is_rescan_tree_root();

    it = nit;
  }
  assert(it != _nodes.cbegin());

  return size_type{
    static_cast<_container_type::size_type>(it - 1 - _nodes.cbegin())
  };
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
rescan_tree_root(const size_type descendant_node) const noexcept
{
  assert(descendant_node != size_type::invalid());
  size_type node = descendant_node;

  while (!(*this)[node].is_rescan_tree_root())
    node = (*this)[node].get_rescan_parent();

  return node;
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
next_rescan_subtree_dfs_pre_node(const size_type subtree_root,
				 const size_type prev_node) const noexcept
{
  const size_type next_node =
    _next_rescan_subtree_dfs_pre_node(subtree_root, prev_node);

  assert(next_node == size_type::invalid() ||
	 _prev_rescan_subtree_dfs_pre_node(next_node) == prev_node);
  return next_node;
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
prev_rescan_subtree_dfs_pre_node(const size_type subtree_root,
				 const size_type next_node) const noexcept
{
  const size_type prev_node = _prev_rescan_subtree_dfs_pre_node(next_node);

  assert(prev_node == size_type::invalid() ||
	 (_next_rescan_subtree_dfs_pre_node(subtree_root, prev_node) ==
	  next_node));
  return prev_node;
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
last_rescan_subtree_dfs_pre_node(const size_type root_node)
  const noexcept
{
  return _last_rescan_subtree_dfs_pre_node(root_node);
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
_next_rescan_subtree_dfs_pre_node(const size_type subtree_root,
				  const size_type prev_node) const noexcept
{
  assert(subtree_root._n < _nodes.size());
  assert(prev_node != size_type::invalid());

  _container_type::const_iterator it;
  for (it = _nodes.cbegin() + prev_node._n + 1; it != _nodes.cend(); ++it) {
    const auto node_parent = it->get_rescan_parent();
    if (node_parent != size_type::invalid() && node_parent._n <= prev_node._n) {
      if (node_parent._n < subtree_root._n)
	return size_type::invalid();
      break;
    }
  }
  if (it == _nodes.cend())
    return size_type::invalid();

  return size_type{
    static_cast<_container_type::size_type>(it - _nodes.cbegin())
  };
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
_prev_rescan_subtree_dfs_pre_node(const size_type next_node) const noexcept
{
  const size_type next_node_parent = (*this)[next_node].get_rescan_parent();
  return __last_rescan_subtree_dfs_pre_node(next_node_parent, next_node);
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
_last_rescan_subtree_dfs_pre_node(const size_type subtree_root)
  const noexcept
{
  return __last_rescan_subtree_dfs_pre_node(subtree_root,
					    _rescan_subtree_end(subtree_root));
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
__last_rescan_subtree_dfs_pre_node(const size_type subtree_root,
				   const size_type subtree_end)
  const noexcept
{
  assert(subtree_end._n <= _nodes.size());
  assert(subtree_end._n > subtree_root._n);
  size_type node = size_type{subtree_end._n - 1};
  size_type node_ancestor_or_self = node;
  while (node_ancestor_or_self._n != 0 &&
	 node_ancestor_or_self != subtree_root) {
    if ((*this)[node_ancestor_or_self].is_rescan_tree_root()) {
      assert(node_ancestor_or_self._n > 0);
      node = size_type{node_ancestor_or_self._n - 1};
      node_ancestor_or_self = node;
    } else {
      node_ancestor_or_self =
	(*this)[node_ancestor_or_self].get_rescan_parent();
    }
  }

  return node;
}

const expansion_forest::_forest_nodes::size_type
expansion_forest::_forest_nodes::
_rescan_subtree_end(const size_type subtree_root) const noexcept
{
  assert(subtree_root._n < _nodes.size());
  // Find the position right after the last node from the subtree
  // located at subtree_root or from any of the argument trees
  // (recursively) referenced therefrom.
  // Optimization for the case subtree_root refer's to the initial
  // rescan_tree's root.
  if (!subtree_root._n)
    return size_type{_nodes.size()};

  _container_type::size_type skip_arg_roots = (*this)[subtree_root].nargs();
  _container_type::const_iterator it;
  for (it = _nodes.cbegin() + subtree_root._n + 1; it != _nodes.cend(); ++it) {
    const bool is_arg_root = it->is_rescan_tree_root();

    // Per the DFS pre serialization order, subtree_root's (rescan
    // tree) next sibling, if any, will have its parent located before
    // subtree_root.
    if (!skip_arg_roots &&
	(is_arg_root || it->get_rescan_parent()._n < subtree_root._n)) {
      break;
    }
    assert(is_arg_root || it->get_rescan_parent()._n >= subtree_root._n);

    skip_arg_roots -= is_arg_root;
    skip_arg_roots += it->nargs();
  }

  return size_type{
    static_cast<_container_type::size_type>(it - _nodes.cbegin())
  };
}


expansion_forest::_rescan_macro_invocation::
_rescan_macro_invocation
(const _forest_nodes::size_type rescan_parent, const macro &m,
 const rescan_macro_invocation::output::tokens_range &src_range) noexcept
  : _rescan_parent{rescan_parent}, _nargs{0}, _src_range{src_range}, _m(&m)
{}

expansion_forest::_rescan_macro_invocation::
_rescan_macro_invocation
  (const rescan_macro_invocation::output::tokens_range &src_range) noexcept
  : _rescan_parent(_forest_nodes::size_type::invalid()), _nargs(0),
    _src_range(src_range), _m(nullptr)
{}

std::size_t expansion_forest::_rescan_macro_invocation::out_length()
  const noexcept
{
  // The roots for argument expansions are dummy identity mappings.
  if (is_arg_tree_root())
    return _src_range.length();

  return _output.out_length();
}

bool expansion_forest::_rescan_macro_invocation::
is_rescan_tree_root() const noexcept
{
  return _rescan_parent == _forest_nodes::size_type::invalid();
}

bool expansion_forest::_rescan_macro_invocation::is_arg_tree_root()
  const noexcept
{
  assert(_m || is_rescan_tree_root());
  return !_m;
}

expansion_forest::_forest_nodes::size_type
expansion_forest::_rescan_macro_invocation::get_rescan_parent() const noexcept
{
  return _rescan_parent;
}

const pp_result::macro& expansion_forest::_rescan_macro_invocation::
get_macro() const noexcept
{
  assert(_m);
  return *_m;
}


expansion_forest::_rescan_macro_invocation_ref::
_rescan_macro_invocation_ref(const _forest_nodes &forest,
			     const _forest_nodes::size_type node) noexcept
  : _forest(forest), _node(node)
{}

expansion_forest::_rescan_macro_invocation_ref::
operator _rescan_macro_invocation_ptr() const noexcept
{
  return _rescan_macro_invocation_ptr{_forest, _node};
}

bool expansion_forest::_rescan_macro_invocation_ref::
operator==(const _rescan_macro_invocation_ref &rhs)
  const noexcept
{
  return &_forest.get() == &rhs._forest.get() && _node == rhs._node;
}

const expansion_forest::_rescan_macro_invocation&
expansion_forest::_rescan_macro_invocation_ref::deref() const noexcept
{
  assert(!is_nil_node());
  return _forest.get()[_node];
}

bool expansion_forest::_rescan_macro_invocation_ref::
is_nil_node() const noexcept
{
  return _node == _forest_nodes::size_type::invalid();
}

expansion_forest::_rescan_macro_invocation_ptr::
_rescan_macro_invocation_ptr() noexcept
  : _forest{nullptr}, _node{_forest_nodes::size_type::invalid()}
{}

expansion_forest::_rescan_macro_invocation_ptr::
_rescan_macro_invocation_ptr(const _forest_nodes &forest,
			     const _forest_nodes::size_type node) noexcept
  : _forest{&forest}, _node{node}
{}

expansion_forest::_rescan_macro_invocation_ptr::
operator _rescan_macro_invocation_ref() const noexcept
{
  assert(_forest);
  return _rescan_macro_invocation_ref{*_forest, _node};
}

expansion_forest::_rescan_macro_invocation_ptr::
operator bool() const noexcept
{
  return !!_forest;
}

bool expansion_forest::_rescan_macro_invocation_ptr::
operator==(const _rescan_macro_invocation_ptr &rhs) const noexcept
{
  return this->_forest == rhs._forest && this->_node == rhs._node;
}


expansion_forest::expansion_forest
(const macro &m,
   expansion_forest::invocation_source &&initial_invocation_source)
  : _forest{
      m,
      initial_invocation_source._get_total_invocation_range().length()
    },
    _invocation_src{std::move(initial_invocation_source)}
{}

const expansion_forest::rescan_macro_invocation expansion_forest::
operator[](const nodes_size_type node_index) const noexcept
{
  return rescan_macro_invocation{
    _rescan_macro_invocation_ref{_forest, node_index}
  };
}

const expansion_forest::rescan_subtree expansion_forest::get_main_tree()
  const noexcept
{
  return rescan_subtree{
    _rescan_macro_invocation_ref{_forest, _forest_nodes::size_type{0}}
  };
}

expansion_forest::const_all_nodes_iterator
expansion_forest::all_nodes_begin() const noexcept
{
  return const_all_nodes_iterator{
    _forest, const_all_nodes_iterator::begin_tag{}
  };
}

expansion_forest::const_all_nodes_iterator
expansion_forest::all_nodes_end() const noexcept
{
  return const_all_nodes_iterator{
    _forest, const_all_nodes_iterator::end_tag{}
  };
}


expansion_forest::rescan_output_segment::rescan_output_segment
  (const _rescan_macro_invocation_ref &emitting_node,
   const rescan_macro_invocation::output::tokens_range range_in_emitting_node)
    noexcept
    : _emitting_node{emitting_node},
      _range_in_emitting_node{range_in_emitting_node}
{}

bool expansion_forest::rescan_output_segment::
operator==(const rescan_output_segment &rhs) const noexcept
{
  return (_emitting_node == rhs._emitting_node &&
	  _range_in_emitting_node == rhs._range_in_emitting_node);
}

std::pair<expansion_forest::rescan_output_segment::const_fragment_iterator,
	  expansion_forest::rescan_output_segment::const_fragment_iterator>
expansion_forest::rescan_output_segment::
fragments() const noexcept
{
  if (_emitting_node.is_nil_node()) {
    return std::make_pair(const_fragment_iterator{},
			  const_fragment_iterator{});
  }

  const auto overlapping_fragments =
    _emitting_node.deref()._output._overlapping_fragments
      (_range_in_emitting_node);
  return
    (std::make_pair
     (const_fragment_iterator{*this, overlapping_fragments.first},
      const_fragment_iterator{*this, overlapping_fragments.second}));
}

const expansion_forest::rescan_output_segment
expansion_forest::rescan_output_segment::
crop(const rescan_macro_invocation::output::tokens_range &limit)
  const noexcept
{
  // Only cropping of overlapping ranges is supported,
  // otherwise we might end up with end < begin below.
  assert(limit.begin <= _range_in_emitting_node.end &&
	 limit.end >= _range_in_emitting_node.begin);
  return rescan_output_segment{
    _emitting_node,
    rescan_macro_invocation::output::tokens_range{
      std::max(_range_in_emitting_node.begin, limit.begin),
      std::min(_range_in_emitting_node.end, limit.end),
    }
  };
}

const expansion_forest::rescan_output_segment
expansion_forest::rescan_output_segment::
crop_relative(const rescan_macro_invocation::output::tokens_range &limit)
  const noexcept
{
  return crop(rescan_macro_invocation::output::tokens_range{
		_range_in_emitting_node.begin + limit.begin,
		_range_in_emitting_node.begin + limit.end
	      });
}

const expansion_forest::rescan_macro_invocation
expansion_forest::rescan_output_segment::get_emitting_node() const noexcept
{
  return rescan_macro_invocation{_emitting_node};
}


expansion_forest::rescan_output_segment::const_fragment_iterator::
const_fragment_iterator() noexcept
  : _seg_emitting_node{}, _seg_out_range_in_emitting_node{}, _it_frag{}
{}

expansion_forest::rescan_output_segment::const_fragment_iterator::
const_fragment_iterator(const rescan_output_segment &seg,
			const _fragments_container_type::const_iterator it_frag)
  noexcept
  : const_fragment_iterator{
      seg.get_emitting_node()._ref,
      seg.get_out_range_in_emitting_node(),
      it_frag
    }
{}

expansion_forest::rescan_output_segment::const_fragment_iterator::
const_fragment_iterator
  (const _rescan_macro_invocation_ref &seg_emitting_node,
   const rescan_macro_invocation::output::tokens_range
     &seg_out_range_in_emitting_node,
   const _fragments_container_type::const_iterator it_frag)
  noexcept
  : _seg_emitting_node{
      static_cast<_rescan_macro_invocation_ptr>(seg_emitting_node)
    },
    _seg_out_range_in_emitting_node{seg_out_range_in_emitting_node},
    _it_frag{it_frag}
{}

bool expansion_forest::rescan_output_segment::const_fragment_iterator::
operator==(const const_fragment_iterator &rhs) const noexcept
{
  return (_seg_emitting_node == rhs._seg_emitting_node &&
	  (_seg_out_range_in_emitting_node ==
	   rhs._seg_out_range_in_emitting_node) &&
	  _it_frag == rhs._it_frag);
}

expansion_forest::rescan_output_segment::const_fragment_iterator::reference
expansion_forest::rescan_output_segment::const_fragment_iterator::operator*()
  const noexcept
{
  using rescan_macro_invocation_output =
    expansion_forest::rescan_macro_invocation::output;
  using rescan_macro_invocation_output_range =
    rescan_macro_invocation_output::tokens_range;

  assert(static_cast<bool>(_seg_emitting_node));
  assert(_it_frag !=
	 (static_cast<_rescan_macro_invocation_ref>(_seg_emitting_node).deref()
	  ._output._fragments.end()));

  const auto frag_out_range = _it_frag->get_out_range();
  const rescan_macro_invocation_output_range overlapping_out_range{
    std::max(_seg_out_range_in_emitting_node.begin, frag_out_range.begin),
    std::min(_seg_out_range_in_emitting_node.end, frag_out_range.end)
  };
  const rescan_macro_invocation_output_range range_in_frag{
    overlapping_out_range.begin - frag_out_range.begin,
    overlapping_out_range.end - frag_out_range.begin,
  };

  return value_type{*_it_frag, range_in_frag};
}

expansion_forest::rescan_output_segment::const_fragment_iterator&
expansion_forest::rescan_output_segment::const_fragment_iterator::operator++()
noexcept
{
  ++_it_frag;
  return *this;
}

expansion_forest::rescan_output_segment::const_fragment_iterator
expansion_forest::rescan_output_segment::const_fragment_iterator::
operator++(int) noexcept
{
  return const_fragment_iterator{
    static_cast<_rescan_macro_invocation_ref>(_seg_emitting_node),
    _seg_out_range_in_emitting_node,
    _it_frag++
  };
}

expansion_forest::rescan_output_segment::const_fragment_iterator&
expansion_forest::rescan_output_segment::const_fragment_iterator::operator--()
  noexcept
{
  --_it_frag;
  return *this;
}

expansion_forest::rescan_output_segment::const_fragment_iterator
expansion_forest::rescan_output_segment::const_fragment_iterator::
operator--(int) noexcept
{
  return const_fragment_iterator{
    static_cast<_rescan_macro_invocation_ref>(_seg_emitting_node),
    _seg_out_range_in_emitting_node,
    _it_frag--
  };
}


expansion_forest::rescan_macro_invocation::const_src_iterator::
const_src_iterator() noexcept
  : _forest(nullptr), _node(_forest_nodes::size_type::invalid()),
    _remaining_src_length(0), _src_begin(0),
    _rescan_tree_distance{std::numeric_limits<std::size_t>::max()},
    _cross_arg_tree_roots(false)
{}

expansion_forest::rescan_macro_invocation::const_src_iterator::
const_src_iterator(const _rescan_macro_invocation_ref &node,
		   const bool cross_arg_tree_roots,
		   const _begin_tag&) noexcept
  : _forest{&node.get_forest()},
    _node{!node.deref().is_arg_tree_root() ?
	  node.deref().get_rescan_parent() :
	  node.get_node()},
    _remaining_src_length{node.deref().get_source_range().length()},
    _src_begin{
      (!node.deref().is_arg_tree_root() ?
       node.deref().get_source_range().begin :
       0)
    },
    _rescan_tree_distance{
      !node.deref().is_arg_tree_root() ? static_cast<std::size_t>(1) : 0
    },
    _cross_arg_tree_roots{cross_arg_tree_roots}
{
  _advance_node_to_emitting();
}

expansion_forest::rescan_macro_invocation::const_src_iterator::
const_src_iterator(const _rescan_macro_invocation_ref &node,
		   const bool cross_arg_tree_roots,
		   const _end_tag&) noexcept
  : _forest{&node.get_forest()}, _node{_forest_nodes::size_type::invalid()},
    _remaining_src_length{0}, _src_begin{0},
    _rescan_tree_distance{std::numeric_limits<std::size_t>::max()},
    _cross_arg_tree_roots{cross_arg_tree_roots}
{}

bool expansion_forest::rescan_macro_invocation::const_src_iterator::
operator==(const const_src_iterator &rhs) const noexcept
{
  assert(this->_forest == rhs._forest);

  const bool equal = (this->_remaining_src_length == rhs._remaining_src_length);
  assert(!equal || (this->_node == rhs._node));
  // When reading into the raw invocation source, _node will be
  // invalid(), yet there might be some remaining src length to
  // process.
  assert(equal || this->_node != rhs._node ||
	 this->_node == _forest_nodes::size_type::invalid());
  assert(!equal || (this->_src_begin == rhs._src_begin));

  return equal;
}

expansion_forest::rescan_macro_invocation::const_src_iterator::reference
expansion_forest::rescan_macro_invocation::const_src_iterator::
operator*() const noexcept
{
  const rescan_macro_invocation::output::tokens_range
    out_range_in_emitting_node{
      _src_begin,
      _src_begin + _cur_src_segment_length(),
    };
  return rescan_output_segment{
	  _rescan_macro_invocation_ref{*_forest, _node},
	  out_range_in_emitting_node,
  };
}

expansion_forest::rescan_macro_invocation::const_src_iterator&
expansion_forest::rescan_macro_invocation::const_src_iterator::operator++()
  noexcept
{
  if (_node == _forest_nodes::size_type::invalid()) {
    // Reading from the raw invocation source, which is the last
    // segment. Make the result compare equal to the end iterator.
    _remaining_src_length = 0;
    _src_begin = 0;
    return *this;
  }

  const auto cur_src_segment_length = _cur_src_segment_length();
  _src_begin += cur_src_segment_length;
  _remaining_src_length -= cur_src_segment_length;
  if (!_remaining_src_length) {
    _node = _forest_nodes::size_type::invalid();
    _src_begin = 0;
    return *this;
  }

  _advance_node_to_emitting();

  return *this;
}

expansion_forest::rescan_macro_invocation::const_src_iterator
expansion_forest::rescan_macro_invocation::const_src_iterator::
operator++(int) noexcept
{
  const const_src_iterator i = *this;

  ++*this;
  return i;
}

std::size_t expansion_forest::rescan_macro_invocation::const_src_iterator::
skip_src_distance(const std::size_t distance) noexcept
{
  std::size_t skipped = 0;
  while (_node != _forest_nodes::size_type::invalid()) {
    const auto cur_src_segment_length =
      (**this).get_out_range_in_emitting_node().length();
    if (skipped + cur_src_segment_length > distance)
      break;
    skipped += cur_src_segment_length;
    ++(*this);
  }
  return distance - skipped;
}

void expansion_forest::rescan_macro_invocation::const_src_iterator::
_advance_node_to_emitting() noexcept
{
  while (_node != _forest_nodes::size_type::invalid()) {
    const _rescan_macro_invocation &node = (*_forest)[_node];
    if (!node.is_arg_tree_root()) {
      if (node.out_length() > _src_begin)
	break;

      _src_begin -= node.out_length();
      _src_begin += node.get_source_range().end;
      _node = node.get_rescan_parent();
      ++_rescan_tree_distance;
    } else if (!_cross_arg_tree_roots) {
      assert(node.out_length() > _src_begin);
      assert(node.out_length() >= _src_begin + _remaining_src_length);
      break;
    } else {
      // The offset of the (unexpanded) argument's beginning is relative
      // to the owner's rescan parent's output.
      _src_begin += node.get_source_range().begin;
      _node = _forest->arg_owner(_node);
      _node = (*_forest)[_node].get_rescan_parent();
    }
  }
}

std::size_t expansion_forest::rescan_macro_invocation::const_src_iterator::
_cur_src_segment_length() const noexcept
{
  if (_node == _forest_nodes::size_type::invalid())
    return _remaining_src_length;

  assert(_src_begin < (*_forest)[_node].out_length());
  return std::min((*_forest)[_node].out_length() - _src_begin,
		  _remaining_src_length);
}


expansion_forest::rescan_macro_invocation::
rescan_macro_invocation(const _rescan_macro_invocation_ref &ref) noexcept
  : _ref(ref)
{}

expansion_forest::rescan_macro_invocation::
operator rescan_subtree() const noexcept
{
  return rescan_subtree{_ref};
}

expansion_forest::rescan_macro_invocation::
operator nodes_size_type() const noexcept
{
  return _ref.get_node();
}

expansion_forest::rescan_macro_invocation::
operator rescan_output_segment() const noexcept
{
  const output::tokens_range all_range{
    0,
    _ref.deref().out_length(),
  };

  return rescan_output_segment{_ref, all_range};
}


expansion_forest::rescan_macro_invocation::const_src_iterator
expansion_forest::rescan_macro_invocation::
src_begin(const bool cross_arg_tree_roots) const noexcept
{
  return const_src_iterator{
    _ref, cross_arg_tree_roots, const_src_iterator::_begin_tag{}
  };
}

expansion_forest::rescan_macro_invocation::const_src_iterator
expansion_forest::
rescan_macro_invocation::src_end(const bool cross_arg_tree_roots) const noexcept
{
  return const_src_iterator{
    _ref, cross_arg_tree_roots, const_src_iterator::_end_tag{}
  };
}

bool expansion_forest::rescan_macro_invocation::
operator==(const rescan_macro_invocation &rhs) const noexcept
{
  return _ref == rhs._ref;
}

const bool expansion_forest::rescan_macro_invocation::is_nil_node()
  const noexcept
{
  return _ref.is_nil_node();
}

const bool expansion_forest::rescan_macro_invocation::is_rescan_tree_root()
  const noexcept
{
  return _ref.deref().is_rescan_tree_root();
}

const expansion_forest::rescan_macro_invocation
expansion_forest::rescan_macro_invocation::get_rescan_parent() const noexcept
{
  return rescan_macro_invocation{
	   _rescan_macro_invocation_ref{
	      _ref.get_forest(),
	      _ref.deref().get_rescan_parent()
	   }
	 };
}

const bool expansion_forest::rescan_macro_invocation::is_arg_tree_root()
  const noexcept
{
  return _ref.deref().is_arg_tree_root();
}

const expansion_forest::rescan_macro_invocation
expansion_forest::rescan_macro_invocation::get_arg_tree_owner() const noexcept
{
  assert(is_arg_tree_root());
  const auto &forest_nodes = _ref.get_forest();
  return rescan_macro_invocation{
	   _rescan_macro_invocation_ref{
	      forest_nodes,
	      forest_nodes.arg_owner(_ref.get_node())
	   }
	 };
}

const pp_result::macro& expansion_forest::
rescan_macro_invocation::get_macro() const noexcept
{
  assert(!is_arg_tree_root());
  return _ref.deref().get_macro();
}

const expansion_forest::rescan_subtree
expansion_forest::rescan_macro_invocation::
get_arg_tree(const std::size_t arg_index) const noexcept {
  auto arg_tree_root_node = _ref.get_forest().first_arg_of(_ref.get_node());
  for (std::size_t i = 0; i < arg_index; ++i) {
    arg_tree_root_node = _ref.get_forest().next_arg(arg_tree_root_node);
  }
  return rescan_subtree{
	   _rescan_macro_invocation_ref{
	     _ref.get_forest(),
	     arg_tree_root_node
	   }
  };
}

const expansion_forest::rescan_macro_invocation::output&
expansion_forest::rescan_macro_invocation::get_output() const noexcept
{
  return _ref.deref().get_output();
}

std::size_t expansion_forest::rescan_macro_invocation::out_length()
  const noexcept
{
  return _ref.deref().out_length();
}

const expansion_forest::rescan_macro_invocation::output::tokens_range&
expansion_forest::rescan_macro_invocation::get_src_range() const noexcept
{
  return _ref.deref().get_source_range();
}

std::size_t expansion_forest::rescan_macro_invocation::get_nargs()
  const noexcept
{
  return _ref.deref().nargs();
}

expansion_forest::rescan_macro_invocation::output::fragment::repl_ref::
repl_ref() noexcept
  : _begin(std::numeric_limits<std::size_t>::max()), _length(0)
{}

expansion_forest::rescan_macro_invocation::output::fragment::repl_ref::
repl_ref(const std::size_t begin) noexcept
  : _begin(begin), _length(1)
{}

expansion_forest::rescan_macro_invocation::output::fragment::repl_ref::
repl_ref(const std::size_t begin, const std::size_t length) noexcept
  : _begin(begin), _length(length)
{}

void expansion_forest::rescan_macro_invocation::output::fragment::repl_ref::
_extend(const std::size_t tail_begin) noexcept
{
  assert(_begin + _length == tail_begin);
  ++_length;
}


expansion_forest::rescan_macro_invocation::output::fragment::arg_ref::
arg_ref(const std::size_t arg_index) noexcept
  : _arg_index(arg_index)
{}


expansion_forest::rescan_macro_invocation::output::fragment::stringification::
stringification(const arg_ref &arg) noexcept
  : _arg(arg)
{}


expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
component::component(repl_ref &&repl, const repl_tag&) noexcept
  : _op(op::repl), _repl(std::move(repl))
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
component::component(repl_ref &&repl, arg_ref &&arg, const arg_tag&) noexcept
  : _op(op::arg), _repl(std::move(repl)), _arg(std::move(arg))
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
component::component(repl_ref &&repl, stringification &&s,
		     const stringification_tag&) noexcept
  : _op(op::stringification), _repl(std::move(repl)),
    _stringification(std::move(s))
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
component::component(repl_ref &&repl, const bool skipped,
		     const conditional_comma_tag&) noexcept
  : _op(op::conditional_comma), _repl(std::move(repl)),
    _conditional_comma_skipped(skipped)
{}


expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::concatenated_token_origin::
concatenated_token_origin(const repl_token &repl) noexcept
  : _origin{origin::repl},
    _repl{repl}
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::concatenated_token_origin::
concatenated_token_origin(const arg_token &arg) noexcept
  : _origin{origin::arg},
    _arg{arg}
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::concatenated_token_origin::
concatenated_token_origin(const concatenated_token_origin &rhs) noexcept
  : _origin{rhs._origin}
{
  switch (_origin) {
  case origin::repl:
    new (&_repl) repl_token{rhs._repl};
    break;
  case origin::arg:
    new (&_arg) arg_token{rhs._arg};
    break;
  }
}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::concatenated_token_origin::
~concatenated_token_origin() noexcept
{
  switch (_origin) {
  case origin::repl:
    _repl.~repl_token();
    break;
  case origin::arg:
    _arg.~arg_token();
    break;
  }
}

const
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::concatenated_token_origin::repl_token&
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::concatenated_token_origin::
get_repl_origin() const noexcept
{
  assert(_origin == origin::repl);
  return _repl;
}

const
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::concatenated_token_origin::arg_token&
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::concatenated_token_origin::
get_arg_origin() const noexcept
{
  assert(_origin == origin::arg);
  return _arg;
}


expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::
concatenated_tokens(const concatenated_tokens&) = default;

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::
concatenated_tokens(concatenated_tokens&&) noexcept = default;

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::
concatenated_tokens
  (std::vector<concatenated_token_origin> &&concatenated_tokens_origins)
  noexcept
: _concatenated_tokens_origins{std::move(concatenated_tokens_origins)}
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens::
~concatenated_tokens() noexcept = default;


expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::output_fragment(repl_ref &&repl,
				 const std::size_t out_begin,
				 concatenated_tokens &&concatenated_tokens)
  noexcept
  : _kind{kind::concatenated_tokens},
    _concatenated_tokens{std::move(concatenated_tokens)},
    _repl{std::move(repl)},
    _out_begin{out_begin}
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::output_fragment(repl_ref &&repl,
				 const std::size_t out_begin,
				 arg_tokens &&arg_tokens) noexcept
  :_kind{kind::arg_tokens},
   _arg_tokens{std::move(arg_tokens)},
    _repl{std::move(repl)},
    _out_begin{out_begin}
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::output_fragment(repl_ref &&repl,
				 const std::size_t out_begin,
				 stringification &&stringification) noexcept
  : _kind{kind::stringification},
    _stringification{std::move(stringification)},
    _repl{std::move(repl)},
    _out_begin{out_begin}
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::output_fragment(repl_ref &&repl,
		    const std::size_t out_begin,
		    conditional_comma &&conditional_comma) noexcept
  : _kind{kind::conditional_comma},
    _conditional_comma{std::move(conditional_comma)},
    _repl{std::move(repl)},
    _out_begin{out_begin}
{}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::output_fragment(output_fragment &&frag) noexcept
  : _kind{frag._kind}, _repl{frag._repl}, _out_begin{frag._out_begin}
{
  switch (_kind) {
  case kind::concatenated_tokens:
    new (&_concatenated_tokens)
      concatenated_tokens{std::move(frag._concatenated_tokens)};
    break;
  case kind::arg_tokens:
    new (&_arg_tokens) arg_tokens{std::move(frag._arg_tokens)};
    break;
  case kind::stringification:
    new (&_stringification) stringification{std::move(frag._stringification)};
    break;
  case kind::conditional_comma:
    new (&_conditional_comma)
      conditional_comma{std::move(frag._conditional_comma)};
    break;
  }
}

expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::~output_fragment() noexcept
{
  switch (_kind) {
  case kind::concatenated_tokens:
    _concatenated_tokens.~concatenated_tokens();
    break;
  case kind::arg_tokens:
    _arg_tokens.~arg_tokens();
    break;
  case kind::stringification:
    _stringification.~stringification();
    break;
  case kind::conditional_comma:
    _conditional_comma.~conditional_comma();
    break;
  }
}

const
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::concatenated_tokens&
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::get_concatenated_tokens_fragment() const noexcept
{
  assert(_kind == kind::concatenated_tokens);
  return _concatenated_tokens;
}

const
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::arg_tokens&
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::get_arg_tokens_fragment() const noexcept
{
  assert(_kind == kind::arg_tokens);
  return _arg_tokens;
}

const
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::stringification&
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::get_stringification_fragment() const noexcept
{
  assert(_kind == kind::stringification);
  return _stringification;
}

const
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::conditional_comma&
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
output_fragment::get_conditional_comma_fragment() const noexcept
{
  assert(_kind == kind::conditional_comma);
  return _conditional_comma;
}

const expansion_forest::rescan_macro_invocation::output::fragment::
  concat_sequence::_components_container_type::size_type
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
size() const noexcept
{
  return _components.size();
}

const expansion_forest::rescan_macro_invocation::output::fragment::
  concat_sequence::component&
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
operator[](const _components_container_type::size_type i)
  const noexcept
{
  return _components[i];
}

bool
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
is_conditional_comma() const noexcept
{
  // The special conditional comma construct , ## __VA_ARGS__ is
  // recognized only at the end of a concatenation sequence.
  // Otherwise it's handled just like a regular concatenation.
  // Furthermore, a comma cannot concatenate with anything other token
  // to form a new valid preprocessor token. That means that if
  // there's a conditional comma construct, and the comma is emitted,
  // then all other components (prior to it), if any, must evaluate to empties.
  assert(!_components.empty());
  assert(!std::any_of(_components.cbegin(), _components.cend() - 1,
		      [](const auto &c) {
			return c._op == component::op::conditional_comma;
		      }));
  return (_components.back()._op == component::op::conditional_comma &&
	  !_components.back()._conditional_comma_skipped);
}

bool
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
is_stringfication() const noexcept
{
  // A string literal cannot concatenate with anything else, including
  // other string literals. Thus, if there is any stringification component,
  // all others must evalutate to empties.
  assert(std::count_if(_components.cbegin(), _components.cend(),
		       [](const auto &c) {
			 return c._op == component::op::stringification;
		       }) <= 1);
  return std::any_of(_components.cbegin(), _components.cend(),
		     [](const auto &c) {
		       return c._op == component::op::stringification;
		     });
}

std::vector<
  expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
  output_fragment
  >
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
get_output_fragments
  (const rescan_macro_invocation &emitting_rmi) const
{
  using concatenated_tokens = output_fragment::concatenated_tokens;
  using concatenated_token_origin =
    concatenated_tokens::concatenated_token_origin;
  using arg_tokens = output_fragment::arg_tokens;

  std::vector<output_fragment> fragments;
  std::size_t next_output_token_index = 0;

  for (_components_container_type::const_iterator
	 it_component = _components.begin();
       it_component != _components.end();) {
    std::vector<concatenated_token_origin> concatenated_tokens_origins;
    switch (it_component->_op) {
    case component::op::repl:
      // There cannot be any stringification or conditional comma tokens.
      assert(std::all_of
	     (fragments.cbegin(), fragments.cend(),
	      [](const auto &frag) {
		return ((frag._kind ==
			 output_fragment::kind::concatenated_tokens) ||
			frag._kind == output_fragment::kind::arg_tokens);
	      }));
	  // First in a series of concatenated tokens.
	  concatenated_tokens_origins.push_back
	    (concatenated_token_origin{
	       concatenated_token_origin::repl_token {
		 .concatenated_repl_token_index =
		   it_component->_repl.begin(),
	       }
	     });
      break;

    case component::op::arg:
      {
	const auto arg_tokens_length =
	  ((static_cast<rescan_macro_invocation>
	    (emitting_rmi.get_arg_tree(it_component->_arg.get_arg_index())))
	   .out_length());
	if (arg_tokens_length != 0) {
	  // There cannot be any stringification or conditional comma tokens.
	  assert(std::all_of
		 (fragments.cbegin(), fragments.cend(),
		  [](const auto &frag) {
		    return ((frag._kind ==
			     output_fragment::kind::concatenated_tokens) ||
			    frag._kind == output_fragment::kind::arg_tokens);
		  }));

	  std::size_t consumed_arg_tokens = 0;
	  // If the current argument has more than one token, and a
	  // non-trivial number of concatentaion components before the
	  // current one all evaluated to empties, then emit a
	  // concatenated_tokens fragment wrapping only the current
	  // argument's first token.
	  if (arg_tokens_length != 1 && fragments.empty() &&
	      it_component != _components.cbegin()) {
	    fragments.push_back
	      (output_fragment{
		 repl_ref{it_component->_repl},
		 next_output_token_index,
		 concatenated_tokens {
		   std::vector<concatenated_token_origin>{{
		       concatenated_token_origin{
			 concatenated_token_origin::arg_token {
			   .arg = it_component->_arg,
			   .arg_ref_repl_token_index =
			     it_component->_repl.begin(),
			   .concatenated_arg_token_index = 0,
			 }
		       }
		   }}
		 }
	       });
	    consumed_arg_tokens = 1;
	    next_output_token_index += 1;
	  } else if (!fragments.empty()) {
	    // The first token got consumed in the previous iteration.
	    consumed_arg_tokens = 1;
	  }

	  assert(consumed_arg_tokens == 1 ||
		 (consumed_arg_tokens == 0 &&
		  fragments.empty() &&
		  (arg_tokens_length == 1 ||
		   it_component == _components.cbegin())));

	  if (arg_tokens_length != 1 &&
	      it_component + 1 == _components.cend()) {
	    assert(arg_tokens_length - consumed_arg_tokens > 0);
	    // At the end of the concat sequence (which has at least
	    // two components). The current argument's first token has
	    // been consumed for the tail of a concatenated_tokens
	    // fragment, either above or in the previous
	    // iteration. The remainder gets output verbatim.
	    assert(it_component != _components.cbegin());
	    assert(consumed_arg_tokens == 1);
	    fragments.push_back
	      (output_fragment{
		 repl_ref{it_component->_repl},
		 next_output_token_index,
		 arg_tokens {
		   .arg = it_component->_arg,
		   .arg_tokens = tokens_range{
		     consumed_arg_tokens,
		     arg_tokens_length,
		   },
		 },
	      });
	    next_output_token_index += arg_tokens_length - consumed_arg_tokens;
	  } else {
	    if (arg_tokens_length - consumed_arg_tokens > 1) {
	      // Trivial.
	      assert(arg_tokens_length != 1);
	      // Otherwise the previous branch would have been taken.
	      assert(it_component + 1 != _components.cend());
	      // Also, from the invariant above the if-statement.
	      assert(consumed_arg_tokens == 1 ||
		     it_component == _components.cbegin());
	      fragments.push_back
		(output_fragment{
		  repl_ref{it_component->_repl},
		  next_output_token_index,
		  arg_tokens {
		    .arg = it_component->_arg,
		    .arg_tokens = tokens_range{
		      consumed_arg_tokens,
		      arg_tokens_length - 1,
		    },
		  },
		});
	      next_output_token_index +=
		arg_tokens_length - 1 - consumed_arg_tokens;
	      consumed_arg_tokens = arg_tokens_length - 1;
	    }

	    assert(consumed_arg_tokens == arg_tokens_length - 1);
	    // First in a series of concatenated tokens to collect.
	    concatenated_tokens_origins.push_back
	      (concatenated_token_origin{
		 concatenated_token_origin::arg_token {
		   .arg = it_component->_arg,
		   .arg_ref_repl_token_index =
		   it_component->_repl.begin(),
		   .concatenated_arg_token_index = consumed_arg_tokens,
		 }
	       });
	  }
	}
      }
      break;

    case component::op::stringification:
      // A string cannot combine with anything else (including other
      // strings) to form a valid preprocessor token.
      assert(fragments.empty());
      fragments.push_back
	(output_fragment{
	   repl_ref{it_component->_repl},
	   next_output_token_index,
	   output_fragment::stringification{
	     .arg = it_component->_stringification._arg
	   }
	 });
	next_output_token_index += 1;
      break;

    case component::op::conditional_comma:
      // The special conditional comma construct is recognized only at
      // the end of a concatenation sequence.
      assert(it_component + 1 == _components.end());
      // A comma cannot combine with anything else to form a valid
      // preprocessor token.
      assert(it_component->_conditional_comma_skipped ||
	     fragments.empty());
      if (!it_component->_conditional_comma_skipped) {
	fragments.push_back
	  (output_fragment{
	     repl_ref{it_component->_repl},
	     next_output_token_index,
	     output_fragment::conditional_comma{}
	   });
	next_output_token_index += 1;
      }
      break;
    }
    ++it_component;

    // If a series of concatenated tokens has been started (either by
    // a replacement token component or an argument reference
    // component for a non-empty argument), collect the remainder.
    if (concatenated_tokens_origins.empty())
      continue;

    while (it_component != _components.cend()) {
      bool stop = false;
      switch (it_component->_op) {
      case component::op::repl:
	concatenated_tokens_origins.push_back
	  (concatenated_token_origin{
	    concatenated_token_origin::repl_token {
	      .concatenated_repl_token_index =
	      it_component->_repl.begin(),
	    }
	  });
	break;

      case component::op::arg:
	{
	  const auto arg_tokens_length =
	    ((static_cast<rescan_macro_invocation>
	      (emitting_rmi.get_arg_tree(it_component->_arg.get_arg_index())))
	     .out_length());
	  if (arg_tokens_length != 0) {
	    concatenated_tokens_origins.push_back
	      (concatenated_token_origin{
		 concatenated_token_origin::arg_token {
		   .arg = it_component->_arg,
		   .arg_ref_repl_token_index =
		   it_component->_repl.begin(),
		   .concatenated_arg_token_index = 0,
		 }
	       });
	  }
	  stop = (arg_tokens_length > 1);
	}
      break;

      case component::op::stringification:
	assert(0);
	stop = true;
	break;

      case component::op::conditional_comma:
	assert(0);
	stop = true;
	break;
      }

      if (stop) {
	break;
      }
      it_component += 1;
    }

    std::size_t repl_begin;
    switch (concatenated_tokens_origins.front().get_origin()) {
    case concatenated_token_origin::origin::repl:
      repl_begin = (concatenated_tokens_origins.front().get_repl_origin()
		    .concatenated_repl_token_index);
      break;
    case concatenated_token_origin::origin::arg:
      repl_begin = (concatenated_tokens_origins.front().get_arg_origin()
		    .arg_ref_repl_token_index);
      break;
    }
    std::size_t repl_end;
    switch (concatenated_tokens_origins.back().get_origin()) {
    case concatenated_token_origin::origin::repl:
      repl_end = (concatenated_tokens_origins.back().get_repl_origin()
		    .concatenated_repl_token_index) + 1;
      break;
    case concatenated_token_origin::origin::arg:
      repl_end = (concatenated_tokens_origins.back().get_arg_origin()
		    .arg_ref_repl_token_index) + 1;
      break;
    }

    fragments.push_back
      (output_fragment{
	 repl_ref{repl_begin, repl_end - repl_begin},
	 next_output_token_index,
	 concatenated_tokens {std::move(concatenated_tokens_origins)}
       });
    next_output_token_index += 1;
  }

  return fragments;
}

void
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence::
_extend(component &&c)
{
  _components.emplace_back(std::move(c));
}


expansion_forest::rescan_macro_invocation::output::fragment::_concat_builder::
_concat_builder(fragment &frag) noexcept
  : _frag(frag)
{
  assert(_frag.get()._op == op::concat);
}

void
expansion_forest::rescan_macro_invocation::output::fragment::_concat_builder::
add_component(const std::size_t repl_begin, const repl_tag&)
{
  auto &frag = _frag.get();
  auto &c = frag._concat_sequence;

  c._extend(concat_sequence::component{
      repl_ref{repl_begin}, repl_tag{}
    });

  if (!frag._out_range.length())
    ++frag._out_range.end;

  frag._repl._length = repl_begin + 1 - frag._repl._begin;
}

void
expansion_forest::rescan_macro_invocation::output::fragment::_concat_builder::
add_component(const std::size_t repl_begin, const std::size_t arg_index,
	      const std::size_t arg_length, const arg_tag&)
{
  auto &frag = _frag.get();
  auto &c = frag._concat_sequence;

  c._extend(concat_sequence::component{
      repl_ref{repl_begin}, arg_ref{arg_index}, arg_tag{}
    });

  if (arg_length) {
    if (!frag._out_range.length())
      ++frag._out_range.end;

    frag._out_range.end += arg_length - 1;
  }

  frag._repl._length = repl_begin + 1 - frag._repl._begin;
}

void
expansion_forest::rescan_macro_invocation::output::fragment::_concat_builder::
add_component(const std::size_t repl_begin, const std::size_t repl_end,
	      const std::size_t arg_index, const stringification_tag&) noexcept
{
  auto &frag = _frag.get();
  auto &c = frag._concat_sequence;

  c._extend(concat_sequence::component{
      repl_ref{repl_begin, repl_end - repl_begin},
      stringification{arg_ref{arg_index}}, stringification_tag{}
    });

  if (!frag._out_range.length())
    ++frag._out_range.end;

  frag._repl._length = repl_end - frag._repl._begin;
}

void
expansion_forest::rescan_macro_invocation::output::fragment::_concat_builder::
add_component(const std::size_t repl_begin, const std::size_t repl_end,
	      const bool skipped, const conditional_comma_tag&) noexcept
{
  auto &frag = _frag.get();
  auto &c = frag._concat_sequence;

  c._extend(concat_sequence::component{
      repl_ref{repl_begin, repl_end - repl_begin}, skipped,
      conditional_comma_tag{}
    });

  if (!frag._out_range.length() && !skipped)
    ++frag._out_range.end;

  frag._repl._length = repl_end - frag._repl._begin;
}


expansion_forest::rescan_macro_invocation::output::tokens_range::
tokens_range() noexcept
  : begin{std::numeric_limits<std::size_t>::max()}, end{0}
{}

expansion_forest::rescan_macro_invocation::output::tokens_range::
tokens_range(const std::size_t _begin, const std::size_t _end)
  noexcept
  : begin{_begin}, end{_end}
{
  assert(begin <= end);
}

bool
expansion_forest::rescan_macro_invocation::output::tokens_range::
operator==(const tokens_range &rhs) const noexcept
{
  return this->begin == rhs.begin && this->end == rhs.end;
}

bool
expansion_forest::rescan_macro_invocation::output::tokens_range::
operator<(const tokens_range &rhs) const noexcept
{
  return this->end <= rhs.begin;
}

const std::size_t
expansion_forest::rescan_macro_invocation::output::tokens_range::
length() const noexcept
{
  return end - begin;
}


expansion_forest::rescan_macro_invocation::output::fragment::
fragment(const std::size_t repl_begin, const std::size_t out_begin,
	 const repl_tag&)
  : _op{op::repl}, _out_range{out_begin, out_begin + 1},
    _repl{repl_begin}
{}

expansion_forest::rescan_macro_invocation::output::fragment::
fragment(const std::size_t arg_ref_repl_pos,
	 const std::size_t out_begin, const std::size_t out_length,
	 const std::size_t arg_index, const arg_tag&)
  : _op{op::arg}, _out_range{out_begin, out_begin + out_length},
    _repl{arg_ref_repl_pos}, _arg{arg_index}
{}

expansion_forest::rescan_macro_invocation::output::fragment::
fragment(const std::size_t repl_begin, const std::size_t repl_length,
	 const std::size_t out_begin, const std::size_t arg_index,
	 const stringification_tag&)
  : _op{op::stringification}, _out_range{out_begin, out_begin + 1},
    _repl{repl_begin, repl_length}, _stringification{arg_ref{arg_index}}
{}

expansion_forest::rescan_macro_invocation::output::fragment::
fragment(const std::size_t repl_begin, const std::size_t out_begin,
	 const concat_tag&)
  : _op{op::concat}, _out_range{out_begin, out_begin},
    _repl{repl_begin}, _concat_sequence{}
{}

expansion_forest::rescan_macro_invocation::output::fragment::
fragment(const std::size_t repl_begin, const std::size_t repl_length,
	 const std::size_t out_begin, const bool skipped,
	 const conditional_comma_tag&)
  : _op{op::conditional_comma}, _out_range{out_begin, out_begin + !skipped},
    _repl{repl_begin, repl_length}
{}

expansion_forest::rescan_macro_invocation::output::fragment::
fragment(const std::size_t out_begin, const empty_tag&)
  : _op{op::empty}, _out_range{out_begin, out_begin + 1}
{}

expansion_forest::rescan_macro_invocation::output::fragment::
fragment(const std::size_t out_begin, const std::size_t out_length,
	 const builtin_special_tag&)
  : _op{op::builtin_special}, _out_range{out_begin, out_begin + out_length}
{}

expansion_forest::rescan_macro_invocation::output::fragment::
fragment(fragment &&frag)
  : _op{frag._op}, _out_range{frag._out_range}, _repl{frag._repl}
{
  switch (_op) {
  case op::repl:
    break;

  case op::arg:
    new (&_arg) arg_ref(std::move(frag._arg));
    break;

  case op::stringification:
    new (&_stringification) stringification(std::move(frag._stringification));
    break;

  case op::concat:
    new (&_concat_sequence) concat_sequence(std::move(frag._concat_sequence));
    break;

  case op::conditional_comma:
    break;

  case op::empty:
    break;

  case op::builtin_special:
    break;
  }
}

expansion_forest::rescan_macro_invocation::output::fragment::
~fragment() noexcept
{
  switch (_op) {
  case op::repl:
    break;

  case op::arg:
    _arg.~arg_ref();
    break;

  case op::stringification:
    _stringification.~stringification();
    break;

  case op::concat:
    _concat_sequence.~concat_sequence();
    break;

  case op::conditional_comma:
    break;

  case op::empty:
    break;

  case op::builtin_special:
    break;
  }
}

const expansion_forest::rescan_macro_invocation::output::fragment::arg_ref&
expansion_forest::rescan_macro_invocation::output::fragment::
get_arg_ref_fragment() const noexcept
{
  assert(_op == op::arg);
  return _arg;
}

const
expansion_forest::rescan_macro_invocation::output::fragment::stringification&
expansion_forest::rescan_macro_invocation::output::fragment::
get_stringification_fragment() const noexcept
{
  assert(_op == op::stringification);
  return _stringification;
}

const
expansion_forest::rescan_macro_invocation::output::fragment::concat_sequence&
expansion_forest::rescan_macro_invocation::output::fragment::
get_concat_sequence_fragment() const noexcept
{
  assert(_op == op::concat);
  return _concat_sequence;
}


expansion_forest::rescan_macro_invocation::output::output() noexcept
  = default;

expansion_forest::rescan_macro_invocation::output::output(output &&) noexcept
  = default;

expansion_forest::rescan_macro_invocation::output::~output() noexcept
  = default;

std::size_t expansion_forest::rescan_macro_invocation::output::out_length()
  const noexcept
{
  return _next_fragment_out_begin();
}

std::size_t
expansion_forest::rescan_macro_invocation::output::_next_fragment_out_begin()
  const noexcept
{
  if (_fragments.empty())
    return 0;

  return _fragments.back()._out_range.end;
}

void expansion_forest::rescan_macro_invocation::output::
_add_fragment(const std::size_t repl_begin, const fragment::repl_tag&)
{
  // If the last fragment added had been of type op::repl already, extend it.
  if (!_fragments.empty() && _fragments.back()._op == fragment::op::repl) {
    auto &last = _fragments.back();
    assert(last._repl.begin() + last._repl.length() == repl_begin);
    ++last._repl._length;
    ++last._out_range.end;
    return;
  }

  _fragments.emplace_back(fragment{
      repl_begin, _next_fragment_out_begin(), fragment::repl_tag{}
    });
}

void expansion_forest::rescan_macro_invocation::output::
_add_fragment(const std::size_t arg_ref_repl_pos, const std::size_t arg_index,
	      const std::size_t out_length, const fragment::arg_tag&)
{
  _fragments.emplace_back(fragment{
      arg_ref_repl_pos, _next_fragment_out_begin(), out_length, arg_index,
      fragment::arg_tag{}
    });
}

void expansion_forest::rescan_macro_invocation::output::
_add_fragment(const std::size_t repl_begin, const std::size_t repl_end,
	      const std::size_t arg_index, const fragment::stringification_tag&)
{
  _fragments.emplace_back(fragment{
      repl_begin, repl_end - repl_begin, _next_fragment_out_begin(), arg_index,
      fragment::stringification_tag{}
    });
}

expansion_forest::rescan_macro_invocation::output::fragment::_concat_builder
expansion_forest::rescan_macro_invocation::output::
_add_fragment(const std::size_t repl_begin, const fragment::concat_tag&)
{
  _fragments.emplace_back(fragment{
      repl_begin, _next_fragment_out_begin(), fragment::concat_tag{}
    });
  return fragment::_concat_builder{_fragments.back()};
}

void expansion_forest::rescan_macro_invocation::output::
_add_fragment(const std::size_t repl_begin, const std::size_t repl_end,
	     const bool skipped,
	     const fragment::conditional_comma_tag&)
{
  _fragments.emplace_back(fragment{
      repl_begin, repl_end - repl_begin, _next_fragment_out_begin(), skipped,
      fragment::conditional_comma_tag{}
    });
}

void expansion_forest::rescan_macro_invocation::output::
_add_fragment(const fragment::empty_tag&)
{
  _fragments.emplace_back(fragment{
      _next_fragment_out_begin(), fragment::empty_tag{}
    });
}

void expansion_forest::rescan_macro_invocation::output::
_add_fragment(const std::size_t out_length,
	      const fragment::builtin_special_tag&)
{
  _fragments.emplace_back(fragment{
      _next_fragment_out_begin(), out_length, fragment::builtin_special_tag{}
    });
}

std::pair<expansion_forest::rescan_macro_invocation::output::
	    _fragments_container_type::const_iterator,
	  expansion_forest::rescan_macro_invocation::output::
	    _fragments_container_type::const_iterator>
expansion_forest::rescan_macro_invocation::output::
_overlapping_fragments(const tokens_range &range) const noexcept
{
  struct cmp {
    bool operator()(const tokens_range &range, const fragment &frag)
      const noexcept
    {
      return range < frag.get_out_range();
    }

    bool operator()(const fragment &frag, const tokens_range &range)
      const noexcept {
      return frag.get_out_range() < range;
    }
  };

  return std::equal_range(_fragments.cbegin(), _fragments.cend(),
			  range, cmp{});
}



expansion_forest::rescan_subtree::const_dfs_pre_iterator::
const_dfs_pre_iterator() noexcept
  : _forest(nullptr), _subtree_root(_forest_nodes::size_type::invalid()),
    _node(_forest_nodes::size_type::invalid()), _at_end(false)
{}

expansion_forest::rescan_subtree::const_dfs_pre_iterator::
const_dfs_pre_iterator(const _rescan_macro_invocation_ref &root,
		       const _begin_tag&) noexcept
  : _forest(&root.get_forest()), _subtree_root(root.get_node()),
    _node(root.get_node()), _at_end(false)
{}

expansion_forest::rescan_subtree::const_dfs_pre_iterator::
const_dfs_pre_iterator(const _rescan_macro_invocation_ref &root,
		       const _end_tag&) noexcept
  : _forest(&root.get_forest()), _subtree_root(root.get_node()),
    _node(_forest_nodes::size_type::invalid()), _at_end(true)
{}

bool
expansion_forest::rescan_subtree::const_dfs_pre_iterator::
operator==(const const_dfs_pre_iterator &rhs) const noexcept
{
  assert(this->_forest);
  assert(this->_forest == rhs._forest);
  assert(this->_subtree_root == rhs._subtree_root);

  if (this->_at_end != rhs._at_end)
    return false;

  if (this->_at_end)
    return true;

  return this->_node == rhs._node;
}

expansion_forest::rescan_subtree::const_dfs_pre_iterator::reference
expansion_forest::rescan_subtree::const_dfs_pre_iterator::operator*()
  const noexcept
{
  assert(_forest);
  assert(!_at_end);
  return rescan_macro_invocation{
	   _rescan_macro_invocation_ref{*_forest, _node}
	 };
}

expansion_forest::rescan_subtree::const_dfs_pre_iterator&
expansion_forest::rescan_subtree::const_dfs_pre_iterator::operator++() noexcept
{
  const _forest_nodes::size_type next =
    _forest->next_rescan_subtree_dfs_pre_node(_subtree_root, _node);

  if (next == _forest_nodes::size_type::invalid()) {
    _at_end = true;
    return *this;
  }

  _node = next;
  return *this;
}

expansion_forest::rescan_subtree::const_dfs_pre_iterator
expansion_forest::rescan_subtree::const_dfs_pre_iterator::operator++(int)
  noexcept
{
  const const_dfs_pre_iterator i = *this;

  ++*this;
  return i;
}

expansion_forest::rescan_subtree::const_dfs_pre_iterator&
expansion_forest::rescan_subtree::const_dfs_pre_iterator::operator--() noexcept
{
  if (_at_end) {
    // A begin() iterator incremented past the tree end will have
    // _node set to the last DFS pre node and _at_end true. end()
    // iterators' _node OTOH gets initialized lazily, find that node
    // now.
    if (_node == _forest_nodes::size_type::invalid())
      _node = _forest->last_rescan_subtree_dfs_pre_node(_subtree_root);
    _at_end = false;
    return *this;
  }

  _node = _forest->prev_rescan_subtree_dfs_pre_node(_subtree_root, _node);
  return *this;
}

expansion_forest::rescan_subtree::const_dfs_pre_iterator
expansion_forest::rescan_subtree::const_dfs_pre_iterator::operator--(int)
  noexcept
{
  const const_dfs_pre_iterator i = *this;

  --*this;
  return i;
}


expansion_forest::rescan_subtree::const_dfs_pre_post_iterator::
const_dfs_pre_post_iterator() noexcept
  : _forest{nullptr},
    _subtree_root{_forest_nodes::size_type::invalid()},
    _node{_forest_nodes::size_type::invalid()},
    _next_dfs_pre_node{_forest_nodes::size_type::invalid()},
    _at_dfs_pre{false}
{}

expansion_forest::rescan_subtree::const_dfs_pre_post_iterator::
const_dfs_pre_post_iterator(const _rescan_macro_invocation_ref &root,
			    const _begin_tag&) noexcept
  : _forest{&root.get_forest()},
    _subtree_root{root.get_node()},
    _node{root.get_node()},
    _next_dfs_pre_node{
      _forest->next_rescan_subtree_dfs_pre_node(_subtree_root, _node)
    },
    _at_dfs_pre{true}
{}

expansion_forest::rescan_subtree::const_dfs_pre_post_iterator::
const_dfs_pre_post_iterator(const _rescan_macro_invocation_ref &root,
			    const _end_tag&) noexcept
  : _forest{&root.get_forest()},
    _subtree_root{root.get_node()},
    _node{_forest_nodes::size_type::invalid()},
    _next_dfs_pre_node{_forest_nodes::size_type::invalid()},
    _at_dfs_pre{false}
{}

bool expansion_forest::rescan_subtree::const_dfs_pre_post_iterator::
operator==(const const_dfs_pre_post_iterator &rhs) const noexcept
{
  assert(this->_forest == rhs._forest);
  assert(this->_subtree_root == rhs._subtree_root);

  return _node == rhs._node && _next_dfs_pre_node == rhs._next_dfs_pre_node;
}

expansion_forest::rescan_subtree::const_dfs_pre_post_iterator::reference
expansion_forest::rescan_subtree::const_dfs_pre_post_iterator::
operator*() const noexcept
{
  assert(_node != _forest_nodes::size_type::invalid());
  const bool at_dfs_post =
    (_next_dfs_pre_node == _forest_nodes::size_type::invalid() ||
     (*_forest)[_next_dfs_pre_node].get_rescan_parent() != _node);
  return (std::make_tuple
	  (_at_dfs_pre, at_dfs_post,
	   rescan_macro_invocation{
	     _rescan_macro_invocation_ref{*_forest, _node}
	   }));
}

expansion_forest::rescan_subtree::const_dfs_pre_post_iterator&
expansion_forest::rescan_subtree::const_dfs_pre_post_iterator::
operator++() noexcept
{
  assert(_node != _forest_nodes::size_type::invalid());

  if (_next_dfs_pre_node == _forest_nodes::size_type::invalid()) {
    // No next DFS pre node in the tree. Unwind for DFS post visit.
    _at_dfs_pre = false;
    if (_node != _subtree_root)
      _node = (*_forest)[_node].get_rescan_parent();
    else
      _node = _forest_nodes::size_type::invalid();
    return *this;
  }

  const auto node_parent = (*_forest)[_node].get_rescan_parent();
  const auto next_dfs_pre_node_parent =
    (*_forest)[_next_dfs_pre_node].get_rescan_parent();
  // _next_dfs_pre_node is always a proper descendant of the
  // _subtree_root.
  assert(next_dfs_pre_node_parent != _forest_nodes::size_type::invalid() &&
	 next_dfs_pre_node_parent._n >= _subtree_root._n);
  // If node_parent is the invalid() node, encoded as ::max(), then
  // this will not evaluate to true.
  if (next_dfs_pre_node_parent._n < node_parent._n) {
    // The next DFS pre node's parent is a proper ancestor of the
    // current node's parent. Go to the parent for the DFS post visit.
    _at_dfs_pre = false;
    _node = node_parent;
  } else {
    _at_dfs_pre = true;
    _node = _next_dfs_pre_node;
    _next_dfs_pre_node =
      _forest->next_rescan_subtree_dfs_pre_node(_subtree_root, _node);
  }

  return *this;
}

expansion_forest::rescan_subtree::const_dfs_pre_post_iterator
expansion_forest::rescan_subtree::const_dfs_pre_post_iterator::
operator++(int) noexcept
{
  const const_dfs_pre_post_iterator i = *this;

  ++*this;
  return i;
}


expansion_forest::rescan_subtree::const_final_out_iterator::
const_final_out_iterator() noexcept
  : _forest(nullptr),
    _subtree_root(_forest_nodes::size_type::invalid()),
    _stack_top_node(_forest_nodes::size_type::invalid()),
    _next_dfs_pre_node(_forest_nodes::size_type::invalid())
{}

expansion_forest::rescan_subtree::const_final_out_iterator::
const_final_out_iterator(const _rescan_macro_invocation_ref &subtree_root,
			 const _begin_tag&)
  : _forest(&subtree_root.get_forest()), _subtree_root(subtree_root.get_node()),
    _stack_top_node(_subtree_root),
    _next_dfs_pre_node(
       _forest->next_rescan_subtree_dfs_pre_node(_subtree_root, _stack_top_node)
    )
{
  _consumed_out_lengths_stack.push(0);
  _enter_branch_at_pos();
}

expansion_forest::rescan_subtree::const_final_out_iterator::
const_final_out_iterator(const _rescan_macro_invocation_ref &subtree_root,
			 const _end_tag&)
  : _forest(&subtree_root.get_forest()), _subtree_root(subtree_root.get_node()),
    _stack_top_node(_forest_nodes::size_type::invalid()),
    _next_dfs_pre_node(_forest_nodes::size_type::invalid())
{}

bool expansion_forest::rescan_subtree::const_final_out_iterator::
operator==(const const_final_out_iterator &rhs) const noexcept
{
  assert(this->_forest == rhs._forest);
  assert(this->_subtree_root == rhs._subtree_root);

  const bool equal = (this->_stack_top_node == rhs._stack_top_node &&
		      this->_next_dfs_pre_node == rhs._next_dfs_pre_node);
  assert(!equal ||
	 this->_consumed_out_lengths_stack == rhs._consumed_out_lengths_stack);

  return equal;
}

expansion_forest::rescan_subtree::const_final_out_iterator::reference
expansion_forest::rescan_subtree::const_final_out_iterator::operator*()
  const noexcept
{
  const auto out_begin_in_node = _consumed_out_lengths_stack.top();
  const rescan_macro_invocation::output::tokens_range out_range_in_node{
    out_begin_in_node,
    out_begin_in_node + _cur_out_segment_length()
  };
  return rescan_output_segment{
    _rescan_macro_invocation_ref{*_forest, _stack_top_node},
    out_range_in_node
  };
}

expansion_forest::rescan_subtree::const_final_out_iterator&
expansion_forest::rescan_subtree::const_final_out_iterator::operator++()
{
  std::size_t accumulated_src_length = 0;

  assert(!_consumed_out_lengths_stack.empty());
  _consumed_out_lengths_stack.top() += _cur_out_segment_length();
  while(((*_forest)[_stack_top_node].out_length() ==
	 _consumed_out_lengths_stack.top())) {
    const _rescan_macro_invocation &child = (*_forest)[_stack_top_node];
    accumulated_src_length += child.get_source_range().length();
    _consumed_out_lengths_stack.pop();
    _stack_top_node = child.get_rescan_parent();
    assert(_consumed_out_lengths_stack.empty() ==
	   (_stack_top_node == _forest_nodes::size_type::invalid()));
    if (_consumed_out_lengths_stack.empty()) {
      assert(_next_dfs_pre_node == _forest_nodes::size_type::invalid());
      return *this;
    }

    const _rescan_macro_invocation &parent = (*_forest)[_stack_top_node];
    const std::size_t consumed_from_parent =
      std::min(accumulated_src_length,
	       parent.out_length() - _consumed_out_lengths_stack.top());
    accumulated_src_length -= consumed_from_parent;
    _consumed_out_lengths_stack.top() += consumed_from_parent;
  }

  assert(!_consumed_out_lengths_stack.empty());
  assert(!accumulated_src_length);

  _enter_branch_at_pos();

  return *this;
}

std::size_t expansion_forest::rescan_subtree::const_final_out_iterator::
skip_out_distance(const std::size_t distance)
{
  std::size_t skipped = 0;
  while (_stack_top_node != _forest_nodes::size_type::invalid()) {
    const auto cur_out_segment_length =
      (**this).get_out_range_in_emitting_node().length();
    if (skipped + cur_out_segment_length > distance)
      break;
    skipped += cur_out_segment_length;
    ++(*this);
  }
  return distance - skipped;
}

std::size_t expansion_forest::rescan_subtree::const_final_out_iterator::
depth() const noexcept
{
  // The depth of the current node in the expansion tree is happens to
  // equal the stack size (minus one to make it zero based).
  assert(!_consumed_out_lengths_stack.empty());
  return _consumed_out_lengths_stack.size() - 1;
}

std::size_t expansion_forest::rescan_subtree::const_final_out_iterator::
_cur_out_segment_length() const noexcept
{
  std::size_t out_segment_end;
  if (_next_dfs_pre_node == _forest_nodes::size_type::invalid() ||
      (*_forest)[_next_dfs_pre_node].get_rescan_parent() != _stack_top_node) {
    out_segment_end = (*_forest)[_stack_top_node].out_length();
  } else {
    out_segment_end = (*_forest)[_next_dfs_pre_node].get_source_range().begin;
  }

  assert(out_segment_end > _consumed_out_lengths_stack.top()
	 || (out_segment_end == 0 && _consumed_out_lengths_stack.top() == 0));
  return out_segment_end - _consumed_out_lengths_stack.top();
}

void expansion_forest::rescan_subtree::const_final_out_iterator::
_enter_branch_at_pos()
{
  while(_next_dfs_pre_node != _forest_nodes::size_type::invalid()) {
    const _rescan_macro_invocation &next = (*_forest)[_next_dfs_pre_node];

    if (next.get_rescan_parent() != _stack_top_node ||
	next.get_source_range().begin != _consumed_out_lengths_stack.top()) {
      break;
    }

    _consumed_out_lengths_stack.push(0);
    _stack_top_node = _next_dfs_pre_node;
    _next_dfs_pre_node =
      _forest->next_rescan_subtree_dfs_pre_node(_subtree_root, _stack_top_node);
  }
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::
output_fragment_origin(replacement &&replacement) noexcept
  : _origin{origin::replacement},
    _replacement{std::move(replacement)}
{}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::
output_fragment_origin(verbatim_invocation_arg &&verbatim_invocation_arg)
  noexcept
  : _origin{origin::verbatim_invocation_arg},
    _verbatim_invocation_arg{std::move(verbatim_invocation_arg)}
{}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::
output_fragment_origin(concatenated_tokens &&concatenated_tokens) noexcept
  : _origin{origin::concatenated_tokens},
    _concatenated_tokens{std::move(concatenated_tokens)}
{}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::
output_fragment_origin(stringification &&stringification) noexcept
  : _origin{origin::stringification},
    _stringification{std::move(stringification)}
{}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::
output_fragment_origin(builtin_special &&builtin_special) noexcept
  : _origin{origin::builtin_special},
    _builtin_special{std::move(builtin_special)}
{}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::
output_fragment_origin(empty &&empty) noexcept
  : _origin{origin::empty},
    _empty{std::move(empty)}
{}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::
output_fragment_origin(output_fragment_origin &&rhs) noexcept
  : _origin{rhs._origin}
{
  switch (_origin) {
  case origin::replacement:
    new (&_replacement) replacement{std::move(rhs._replacement)};
    break;
  case origin::verbatim_invocation_arg:
    new (&_verbatim_invocation_arg) verbatim_invocation_arg{
      std::move(rhs._verbatim_invocation_arg)
    };
    break;
  case origin::concatenated_tokens:
    new (&_concatenated_tokens) concatenated_tokens{
      std::move(rhs._concatenated_tokens)
    };
    break;
  case origin::stringification:
    new (&_stringification) stringification{std::move(rhs._stringification)};
    break;
  case origin::builtin_special:
    new (&_builtin_special) builtin_special{std::move(rhs._builtin_special)};
    break;
  case origin::empty:
    new (&_empty) empty{std::move(rhs._empty)};
    break;
  }
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::
~output_fragment_origin() noexcept
{
  switch (_origin) {
  case origin::replacement:
    _replacement.~replacement();
    break;
  case origin::verbatim_invocation_arg:
    _verbatim_invocation_arg.~verbatim_invocation_arg();
    break;
  case origin::concatenated_tokens:
    _concatenated_tokens.~concatenated_tokens();
    break;
  case origin::stringification:
    _stringification.~stringification();
    break;
  case origin::builtin_special:
    _builtin_special.~builtin_special();
    break;
  case origin::empty:
    _empty.~empty();
    break;
  }
}

const expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::replacement&
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::get_replacement_origin() const noexcept
{
  assert(_origin == origin::replacement);
  return _replacement;
}

const expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::verbatim_invocation_arg&
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::get_verbatim_invocation_arg_origin() const noexcept
{
  assert(_origin == origin::verbatim_invocation_arg);
  return _verbatim_invocation_arg;
}

const expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::concatenated_tokens&
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::get_concatenated_tokens_origin() const noexcept
{
  assert(_origin == origin::concatenated_tokens);
  return _concatenated_tokens;
}

const expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::stringification&
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::get_stringification_origin() const noexcept
{
  assert(_origin == origin::stringification);
  return _stringification;
}

const expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::builtin_special&
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin::get_builtin_special_origin() const noexcept
{
  assert(_origin == origin::builtin_special);
  return _builtin_special;
}


expansion_forest::rescan_subtree::const_final_out_origin_iterator::
const_final_out_origin_iterator() noexcept = default;

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
const_final_out_origin_iterator
  (const rescan_subtree &subtree,
   const rescan_macro_invocation::output::tokens_range &out_range)
{
  auto out_segs = _stack_entry_rmi_out_segments_iterator{
    subtree.final_out_begin(),
    subtree.final_out_end(),
    out_range,
  };

  if (out_segs.at_end())
    return;

  _stack.emplace(std::move(out_segs));
  _enter_arg_fragment();
}

bool expansion_forest::rescan_subtree::const_final_out_origin_iterator
::operator==(const const_final_out_origin_iterator &rhs) const noexcept
{
  return _stack == rhs._stack;
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator&
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
operator++()
{
  while (!_stack.empty()) {
    auto &stack_top = _stack.top();

    if (stack_top.cursor.fragments.first == stack_top.cursor.fragments.second) {
      // A segment corresponding to some region within an expanded
      // argument that had not been subject to any expansion. Note
      // that for argument ranges copied from the outermost invocation
      // source, the emitting rescan_macro_invocation would be a NIL
      // node.
      assert((stack_top.cursor.out_segment
	      .get_emitting_node().is_arg_tree_root()) ||
	     (stack_top.cursor.out_segment
	      .get_emitting_node().is_nil_node()));
      _stack.pop();
      continue;
    }

    if (stack_top.cursor.it_cur_concat_sequence_fragments !=
	stack_top.cursor.cur_concat_sequence_fragments.end()) {
      ++stack_top.cursor.it_cur_concat_sequence_fragments;
      if (stack_top.cursor.it_cur_concat_sequence_fragments !=
	  stack_top.cursor.cur_concat_sequence_fragments.end()) {
	const auto &cur_fragment = *stack_top.cursor.fragments.first;
	const auto &cur_concat_sequence_fragment =
	  *stack_top.cursor.it_cur_concat_sequence_fragments;
	if (cur_concat_sequence_fragment.get_out_begin() <
	    cur_fragment.range_in_fragment.end) {
	  // Still in range.
	  break;
	}
      }
    }

    do {
      ++stack_top.cursor.fragments.first;
    }
    while ((stack_top.cursor.fragments.first !=
	    stack_top.cursor.fragments.second) &&
	   ((*stack_top.cursor.fragments.first).range_in_fragment.length()
	    == 0));
    if (stack_top.cursor.fragments.first != stack_top.cursor.fragments.second) {
      stack_top.cursor._enter_fragment();
      break;
    }

    // The current segment had been exhausted.
    ++stack_top.it_segment;
    if (stack_top.it_segment.at_end()) {
      _stack.pop();
      continue;
    }

    stack_top.cursor = _stack_entry_rmi_out_segment_cursor{
      *stack_top.it_segment,
      stack_top.it_segment.get_out_range(),
    };
    break;
  }

  if (!_stack.empty())
    _enter_arg_fragment();

  return *this;
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
output_fragment_origin
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
operator*() const
{
  assert(!_stack.empty());

  const auto &stack_top = _stack.top();
  if (stack_top.cursor.fragments.first == stack_top.cursor.fragments.second) {
    // Is a segment from the original outer invocation's (unpexpanded)
    // argument tokens.
    assert(stack_top.cursor.out_segment
	   .get_emitting_node().is_nil_node());
    return output_fragment_origin{
      output_fragment_origin::verbatim_invocation_arg{
	.src_tokens =
	  stack_top.cursor.out_segment.get_out_range_in_emitting_node()
      }
    };
  }

  const auto &cur_fragment = *stack_top.cursor.fragments.first;
  const auto emitting_rmi = stack_top.cursor.out_segment.get_emitting_node();
  if (stack_top.cursor.it_cur_concat_sequence_fragments !=
      stack_top.cursor.cur_concat_sequence_fragments.end()) {
    const auto &cur_concat_sequence_fragment =
      *stack_top.cursor.it_cur_concat_sequence_fragments;

    switch (cur_concat_sequence_fragment.get_kind()) {
    case concat_sequence_out_fragment::kind::concatenated_tokens:
      {
	const auto emitting_rmi_out_token_index =
	  (cur_fragment.fragment.get_out_range().begin +
	   cur_concat_sequence_fragment.get_out_begin());
	return output_fragment_origin{
	  output_fragment_origin::concatenated_tokens{
	    .emitting_rmi = emitting_rmi,
	    .out_token_index = emitting_rmi_out_token_index,
	    .concatenated_tokens = (cur_concat_sequence_fragment
				    .get_concatenated_tokens_fragment()),
	  }
	};
      }
    case concat_sequence_out_fragment::kind::arg_tokens:
      // Impossible, the argument tokens' origins would have been
      // followed recurisvely to their origin.
      assert(0);
      __builtin_unreachable();
      break;
    case concat_sequence_out_fragment::kind::stringification:
      {
	const auto emitting_rmi_out_token_index =
	  (cur_fragment.fragment.get_out_range().begin +
	   cur_concat_sequence_fragment.get_out_begin());
	return output_fragment_origin{
	  output_fragment_origin::stringification{
	    .emitting_rmi = emitting_rmi,
	    .out_token_index = emitting_rmi_out_token_index,
	    .arg = (cur_concat_sequence_fragment
		    .get_stringification_fragment().arg),
	  }
	};
      }
    case concat_sequence_out_fragment::kind::conditional_comma:
      return output_fragment_origin{
	output_fragment_origin::replacement{
	  .m = std::ref(emitting_rmi.get_macro()),
	  .repl = rmi_out_fragment::repl_ref{
	    cur_concat_sequence_fragment.get_repl().begin(),
	  },
	}
      };
    }
  }

  switch (cur_fragment.fragment.get_op()) {
  case rmi_out_fragment::op::repl:
    {
      const auto fragment_repl_begin =
	cur_fragment.fragment.get_repl().begin();
      const auto &range_in_fragment = cur_fragment.range_in_fragment;
      return output_fragment_origin{
	output_fragment_origin::replacement{
	  .m = std::ref(emitting_rmi.get_macro()),
	  .repl = rmi_out_fragment::repl_ref{
	    fragment_repl_begin + range_in_fragment.begin,
	    range_in_fragment.length(),
	  },
	}
      };
    }
  case rmi_out_fragment::op::arg:
    // Impossible, the argument tokens' origins would have been
    // followed recurisvely to their origin.
    assert(0);
    __builtin_unreachable();
    break;
  case rmi_out_fragment::op::stringification:
    {
      const auto emitting_rmi_out_token_index =
	cur_fragment.fragment.get_out_range().begin;
      return output_fragment_origin{
	output_fragment_origin::stringification{
	  .emitting_rmi = emitting_rmi,
	  .out_token_index = emitting_rmi_out_token_index,
	  .arg = cur_fragment.fragment.get_stringification_fragment().get_arg(),
	}
      };
    }
    break;
  case rmi_out_fragment::op::concat:
    // Would have been entered.
    assert(0);
    __builtin_unreachable();
    break;
  case rmi_out_fragment::op::conditional_comma:
    // Because empty fragments are being skipped, the conditional
    // comma must have been emitted.
    assert(cur_fragment.fragment.get_out_range().length() == 1);
    assert(cur_fragment.range_in_fragment.length() == 1);
    return output_fragment_origin{
      output_fragment_origin::replacement{
	.m = std::ref(emitting_rmi.get_macro()),
	.repl = rmi_out_fragment::repl_ref{
	  cur_fragment.fragment.get_repl().begin(),
	},
      }
    };
  case rmi_out_fragment::op::builtin_special:
    return output_fragment_origin{
      output_fragment_origin::builtin_special{
	.builtin_special_rmi = emitting_rmi,
	.out_tokens = cur_fragment.range_in_fragment,
      }
    };
  case rmi_out_fragment::op::empty:
    return output_fragment_origin{
      output_fragment_origin::empty{}
    };
  }
}

void expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_enter_arg_fragment()
{
  if (_stack.empty())
    return;

  while (true) {
    const auto &stack_top = _stack.top();

    if (stack_top.cursor.out_segment.get_emitting_node().is_arg_tree_root()) {
      // A region within an expanded argument that had not been
      // subject to any expansion. Translate from the identity
      // argument root node to the source.
      assert(stack_top.cursor.fragments.first ==
	     stack_top.cursor.fragments.second);
      const auto &arg_tokens_range =
	stack_top.cursor.out_segment.get_out_range_in_emitting_node();
      const auto arg_root_rmi =
	stack_top.cursor.out_segment.get_emitting_node();
      auto src_segs = _stack_entry_rmi_out_segments_iterator{
	arg_root_rmi.src_begin(true),
	arg_root_rmi.src_end(true),
	arg_tokens_range,
      };
      assert(!src_segs.at_end());
      _stack.emplace(std::move(src_segs));
    } else {
      if (stack_top.cursor.fragments.first ==
	  stack_top.cursor.fragments.second) {
	break;
      }

      const auto &cur_fragment = *stack_top.cursor.fragments.first;
      if (cur_fragment.range_in_fragment.length() == 0)
	break;

      if (stack_top.cursor.it_cur_concat_sequence_fragments !=
	  stack_top.cursor.cur_concat_sequence_fragments.end()) {
	assert(cur_fragment.fragment.get_op() == rmi_out_fragment::op::concat);
	const auto &cur_concat_sequence_fragment
	  = *stack_top.cursor.it_cur_concat_sequence_fragments;
	if (cur_concat_sequence_fragment.get_kind() !=
	    concat_sequence_out_fragment::kind::arg_tokens) {
	  break;
	}

	const auto &concat_sequence_fragment_arg_tokens
	  = cur_concat_sequence_fragment.get_arg_tokens_fragment();

	// Determine the range of argument tokens to copy verbatim.
	// It's a subrange of the range originally emitted verbatim by
	// the concat sequence fragment, trimmed (relatively) to
	// cur_fragment.range_in_fragment.
	auto arg_tokens_range =
	  concat_sequence_fragment_arg_tokens.arg_tokens;
	if ((cur_concat_sequence_fragment.get_out_begin() +
	     arg_tokens_range.length()) >
	    cur_fragment.range_in_fragment.end) {
	  arg_tokens_range.end -=
	    ((cur_concat_sequence_fragment.get_out_begin() +
	      arg_tokens_range.length()) -
	     cur_fragment.range_in_fragment.end);
	}
	if (cur_concat_sequence_fragment.get_out_begin() <
	    cur_fragment.range_in_fragment.begin) {
	  arg_tokens_range.begin +=
	    (cur_fragment.range_in_fragment.begin -
	     cur_concat_sequence_fragment.get_out_begin());
	}

	// The cast takes it from the expanded subtree to the unexpanded
	// identity root.
	const auto arg_root_rmi =
	  (static_cast<rescan_macro_invocation>
	   (stack_top.cursor.out_segment.get_emitting_node().get_arg_tree
	    (concat_sequence_fragment_arg_tokens.arg.get_arg_index())));
	auto src_segs = _stack_entry_rmi_out_segments_iterator{
	  arg_root_rmi.src_begin(true),
	  arg_root_rmi.src_end(true),
	  arg_tokens_range,
	};
	assert(!src_segs.at_end());
	_stack.emplace(std::move(src_segs));

      } else {
	if (cur_fragment.fragment.get_op() != rmi_out_fragment::op::arg)
	  break;

	auto arg_tokens_range = cur_fragment.range_in_fragment;
	const auto arg_tree =
	  (stack_top.cursor.out_segment.get_emitting_node().get_arg_tree
	   (cur_fragment.fragment.get_arg_ref_fragment().get_arg_index()));
	auto out_segs = _stack_entry_rmi_out_segments_iterator{
	  arg_tree.final_out_begin(),
	  arg_tree.final_out_end(),
	  arg_tokens_range,
	};
	assert(!out_segs.at_end());
	_stack.emplace(std::move(out_segs));
      }
    }
  }
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator::_stack_entry_rmi_out_segments_iterator
  (const_final_out_iterator &&it,
   const_final_out_iterator &&end,
   const rescan_macro_invocation::output::tokens_range &out_range) noexcept

  : _kind{_kind::final_out},
    _out_segs {
      .it = std::move(it),
      .end = std::move(end),
    },
    _out_range{out_range}
{
  if (_out_range.begin == _out_range.end) {
    _out_range.begin = 0;
    _out_range.end = 0;
    _out_segs.it = _out_segs.end;
    return;
  }

  const auto out_begin =
    _out_segs.it.skip_out_distance(_out_range.begin);
  assert(_out_range.begin >= out_begin);
  const auto skipped = _out_range.begin - out_begin;
  _out_range.end -= skipped;
  _out_range.begin = out_begin;

  // Skip over empty segments at the head.
  while (_out_segs.it != _out_segs.end &&
	 (*_out_segs.it).get_out_range_in_emitting_node().length() == 0) {
    ++_out_segs.it;
  }
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator::_stack_entry_rmi_out_segments_iterator
  (rescan_macro_invocation::const_src_iterator &&it,
   rescan_macro_invocation::const_src_iterator &&end,
   const rescan_macro_invocation::output::tokens_range &out_range)
  noexcept
  : _kind{_kind::src},
    _src_segs {
      .it = std::move(it),
      .end = std::move(end),
    },
    _out_range{out_range}
{
  if (_out_range.begin == _out_range.end) {
    _out_range.begin = 0;
    _out_range.end = 0;
    _src_segs.it = _src_segs.end;
    return;
  }

  const auto out_begin =
    _src_segs.it.skip_src_distance(_out_range.begin);
  assert(_out_range.begin >= out_begin);
  const auto skipped = _out_range.begin - out_begin;
  _out_range.end -= skipped;
  _out_range.begin = out_begin;

  // Skip over empty segments at the head.
  while (_src_segs.it != _src_segs.end &&
	 (*_src_segs.it).get_out_range_in_emitting_node().length() == 0) {
    ++_src_segs.it;
  }
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator::_stack_entry_rmi_out_segments_iterator
  (_stack_entry_rmi_out_segments_iterator &&rhs) noexcept
  : _kind{rhs._kind}, _out_range{rhs._out_range}
{
  switch (_kind) {
  case _kind::final_out:
    new (&_out_segs) _final_out_segments{
      std::move(rhs._out_segs)
    };
    break;
  case _kind::src:
    new (&_src_segs) _src_segments{
      std::move(rhs._src_segs)
    };
    break;
  }
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator::
~_stack_entry_rmi_out_segments_iterator() noexcept
{
  switch (_kind) {
  case _kind::final_out:
    _out_segs.~_final_out_segments();
    break;
  case _kind::src:
    _src_segs.~_src_segments();
    break;
  }
}

bool expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator::
operator==(const _stack_entry_rmi_out_segments_iterator &rhs) const noexcept
{
  if (_kind != rhs._kind || _out_range != rhs._out_range)
    return false;

  switch (_kind) {
  case _kind::final_out:
    return _out_segs.it == rhs._out_segs.it;
  case _kind::src:
    return _src_segs.it == rhs._src_segs.it;
  }
}

const expansion_forest::rescan_output_segment
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator::
operator*() const noexcept
{
  switch (_kind) {
  case _kind::final_out:
    return *_out_segs.it;
  case _kind::src:
    return *_src_segs.it;
  }
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator&
expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator::operator++()
{
  switch (_kind) {
  case _kind::final_out:
    {
      assert(_out_segs.it != _out_segs.end);
      const auto cur_seg_length =
	(*_out_segs.it).get_out_range_in_emitting_node().length();
      assert(_out_range.begin < cur_seg_length);
      _out_range.begin = 0;
      if (_out_range.end > cur_seg_length) {
	_out_range.end -= cur_seg_length;
	do {
	  ++_out_segs.it;
	} while ((_out_segs.it != _out_segs.end) &&
		 ((*_out_segs.it).get_out_range_in_emitting_node().length()
		  == 0));
	// As long as _out_range.end != 0, there should be some
	// non-trivial segments left.
	assert(_out_segs.it != _out_segs.end);
      } else {
	_out_range.end = 0;
	_out_segs.it = _out_segs.end;
      }
    }
    break;
  case _kind::src:
    {
      assert(_src_segs.it != _src_segs.end);
      const auto cur_seg_length =
	(*_src_segs.it).get_out_range_in_emitting_node().length();
      assert(_out_range.begin < cur_seg_length);
      _out_range.begin = 0;
      if (_out_range.end > cur_seg_length) {
	_out_range.end -= cur_seg_length;
	do {
	  ++_src_segs.it;
	} while ((_src_segs.it != _src_segs.end) &&
		 ((*_src_segs.it).get_out_range_in_emitting_node().length()
		  == 0));
	// As long as _out_range.end != 0, there should be some
	// non-trivial segments left.
	assert(_src_segs.it != _src_segs.end);
      } else {
	_out_range.end = 0;
	_src_segs.it = _src_segs.end;
      }
    }
    break;
  }

  return *this;
}

bool expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segments_iterator::at_end() const noexcept
{
  switch (_kind) {
  case _kind::final_out:
    return _out_segs.it == _out_segs.end;
  case _kind::src:
    return _src_segs.it == _src_segs.end;
  }
}


expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segment_cursor::
_stack_entry_rmi_out_segment_cursor
  (const rescan_output_segment &_out_segment,
   const rescan_macro_invocation::output::tokens_range &out_range)
  : out_segment{_out_segment.crop_relative(out_range)},
    fragments{out_segment.fragments()}
{
  // Skip over empty fragments at the head
  while (fragments.first != fragments.second &&
	 (*fragments.first).range_in_fragment.length() == 0) {
    ++fragments.first;
  }

  _enter_fragment();
}

bool expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segment_cursor::
operator==(const _stack_entry_rmi_out_segment_cursor &rhs) const noexcept
{
  if (out_segment != rhs.out_segment || fragments != rhs.fragments)
    return false;

  return ((it_cur_concat_sequence_fragments -
	   cur_concat_sequence_fragments.begin()) ==
	  (rhs.it_cur_concat_sequence_fragments -
	   rhs.cur_concat_sequence_fragments.begin()));
}

void expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry_rmi_out_segment_cursor::_enter_fragment()
{
  // Reset.
  cur_concat_sequence_fragments = std::vector<concat_sequence_out_fragment>{};
  it_cur_concat_sequence_fragments = cur_concat_sequence_fragments.end();

  if (fragments.first == fragments.second)
    return;

  // If the current fragment is a concat sequence, enter that.
  const auto &cur_fragment = *fragments.first;
  if (cur_fragment.fragment.get_op() != rmi_out_fragment::op::concat)
    return;

  cur_concat_sequence_fragments =
    (cur_fragment.fragment.get_concat_sequence_fragment().get_output_fragments
     (out_segment.get_emitting_node()));
  it_cur_concat_sequence_fragments = cur_concat_sequence_fragments.begin();
  // Skip over concat sequence fragments out of range at the head.
  while ((it_cur_concat_sequence_fragments !=
	  cur_concat_sequence_fragments.end()) &&
	 (it_cur_concat_sequence_fragments->get_out_begin() <
	  cur_fragment.range_in_fragment.begin)) {
    ++it_cur_concat_sequence_fragments;
  }
}

expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry::_stack_entry(_stack_entry_rmi_out_segments_iterator &&_it_segment)
  : it_segment{std::move(_it_segment)},
    cursor{*it_segment, it_segment.get_out_range()}
{}

bool expansion_forest::rescan_subtree::const_final_out_origin_iterator::
_stack_entry::operator==(const _stack_entry &rhs) const noexcept
{
  return it_segment == rhs.it_segment && cursor == rhs.cursor;
}


expansion_forest::rescan_subtree::
rescan_subtree(const _rescan_macro_invocation_ref &subtree_root) noexcept
  : _subtree_root(subtree_root)
{}

expansion_forest::rescan_subtree::
operator rescan_macro_invocation() const noexcept
{
  return rescan_macro_invocation{_subtree_root};
}

expansion_forest::rescan_subtree::const_dfs_pre_iterator
expansion_forest::rescan_subtree::dfs_pre_begin() const noexcept
{
  return const_dfs_pre_iterator{
    _subtree_root, const_dfs_pre_iterator::_begin_tag{}
  };
}

expansion_forest::rescan_subtree::const_dfs_pre_iterator
expansion_forest::rescan_subtree::dfs_pre_end() const noexcept
{
  return const_dfs_pre_iterator{
    _subtree_root, const_dfs_pre_iterator::_end_tag{}
  };
}

expansion_forest::rescan_subtree::const_dfs_pre_post_iterator
expansion_forest::rescan_subtree::dfs_pre_post_begin() const noexcept
{
  return const_dfs_pre_post_iterator{
    _subtree_root, const_dfs_pre_post_iterator::_begin_tag{}
  };
}

expansion_forest::rescan_subtree::const_dfs_pre_post_iterator
expansion_forest::rescan_subtree::dfs_pre_post_end() const noexcept
{
  return const_dfs_pre_post_iterator{
    _subtree_root, const_dfs_pre_post_iterator::_end_tag{}
  };
}

expansion_forest::rescan_subtree::const_final_out_iterator
expansion_forest::rescan_subtree::final_out_begin() const
{
  return const_final_out_iterator{
    _subtree_root, const_final_out_iterator::_begin_tag{}
  };
}

expansion_forest::rescan_subtree::const_final_out_iterator
expansion_forest::rescan_subtree::final_out_end() const
{
  return const_final_out_iterator{
    _subtree_root, const_final_out_iterator::_end_tag{}
  };
}

std::size_t expansion_forest::rescan_subtree::total_src_length() const
{
  std::size_t total_src_length = 0;
  const _forest_nodes &forest = _subtree_root.get_forest();
  const _forest_nodes::size_type subtree_root = _subtree_root.get_node();

  _forest_nodes::size_type rightmost =
    forest.last_rescan_subtree_dfs_pre_node(subtree_root);
  while (rightmost != subtree_root) {
    const _rescan_macro_invocation &child = forest[rightmost];
    const _forest_nodes::size_type parent = child.get_rescan_parent();
    assert(parent != _forest_nodes::size_type::invalid());
    assert(child.get_source_range().begin < forest[parent].out_length());

    total_src_length += child.get_source_range().length();
    total_src_length -=
      std::min(total_src_length,
	       forest[parent].out_length() - child.get_source_range().begin);

    rightmost = parent;
  }

  total_src_length += forest[rightmost].get_source_range().length();

  return total_src_length;
}

std::size_t expansion_forest::rescan_subtree::total_out_length() const
{
  std::size_t total_out_length = 0;
  const _forest_nodes &forest = _subtree_root.get_forest();
  const _forest_nodes::size_type subtree_root = _subtree_root.get_node();
  for (auto node = subtree_root;
       node != _forest_nodes::size_type::invalid();
       node = forest.next_rescan_subtree_dfs_pre_node(subtree_root, node)) {
    auto src_begin = forest[node].get_source_range().begin;
    auto remaining_src_len = forest[node].get_source_range().length();
    auto ancestor_node = node;
    while (remaining_src_len != 0 && ancestor_node != subtree_root) {
      ancestor_node = forest[ancestor_node].get_rescan_parent();
      const auto &ancestor = forest[ancestor_node];
      if (ancestor.out_length() > src_begin) {
	const auto consumed_from_ancestor =
	  std::min(ancestor.out_length() - src_begin, remaining_src_len);
	total_out_length -= consumed_from_ancestor;
	remaining_src_len -= consumed_from_ancestor;
	src_begin += consumed_from_ancestor;
      }
      assert(src_begin >= ancestor.out_length() || remaining_src_len == 0);
      src_begin -= ancestor.out_length();
      src_begin += ancestor.get_source_range().end;
    }
    total_out_length += forest[node].out_length();
  }
  return total_out_length;
}


expansion_forest::invocation_source::segment::
operator raw_pp_tokens_range() const noexcept
{
  return raw_pp_tokens_range{
    raw_pp_tokens_begin,
    raw_pp_tokens_begin + invocation_range.length()
  };
}


expansion_forest::invocation_source::const_segment_iterator::
const_segment_iterator() noexcept
  : _it_seg{},
    _invocation_range{
      std::numeric_limits<std::size_t>::max(),
      std::numeric_limits<std::size_t>::max()
    }
{}

expansion_forest::invocation_source::const_segment_iterator::
const_segment_iterator
(const _segments_container_type::const_iterator it_seg,
 const rescan_macro_invocation::output::tokens_range invocation_range)
noexcept
  : _it_seg{it_seg}, _invocation_range{invocation_range}
{}

bool expansion_forest::invocation_source::const_segment_iterator::
operator==(const const_segment_iterator &rhs) const noexcept
{
  assert(_invocation_range == rhs._invocation_range);
  return _it_seg == rhs._it_seg;
}

expansion_forest::invocation_source::const_segment_iterator::reference
expansion_forest::invocation_source::const_segment_iterator::
operator*() const noexcept
{
  auto s = *_it_seg;

  if (_invocation_range.begin > s.invocation_range.begin) {
    s.raw_pp_tokens_begin +=
       _invocation_range.begin - s.invocation_range.begin;
    s.invocation_range.begin = _invocation_range.begin;
  }
  s.invocation_range.end = std::min(s.invocation_range.end,
				    _invocation_range.end);

  return s;
}

expansion_forest::invocation_source::const_segment_iterator&
expansion_forest::invocation_source::const_segment_iterator::
operator++() noexcept
{
  ++_it_seg;
  return *this;
}

expansion_forest::invocation_source::const_segment_iterator
expansion_forest::invocation_source::const_segment_iterator::
operator++(int) noexcept
{
  return const_segment_iterator{_it_seg++, _invocation_range};
}

expansion_forest::invocation_source::const_segment_iterator&
expansion_forest::invocation_source::const_segment_iterator::
operator--() noexcept
{
  --_it_seg;
  return *this;
}

expansion_forest::invocation_source::const_segment_iterator
expansion_forest::invocation_source::const_segment_iterator::
operator--(int) noexcept
{
  return const_segment_iterator{_it_seg--, _invocation_range};
}


expansion_forest::invocation_source::const_segment_iterator
expansion_forest::invocation_source::
segments_begin() const noexcept
{
  return const_segment_iterator{
    _segments.cbegin(),
    _get_total_invocation_range()
  };
}

expansion_forest::invocation_source::const_segment_iterator
expansion_forest::invocation_source::
segments_end() const noexcept
{
    return const_segment_iterator{
    _segments.cend(),
    _get_total_invocation_range()
  };
}

std::pair<expansion_forest::invocation_source::const_segment_iterator,
	  expansion_forest::invocation_source::const_segment_iterator>
expansion_forest::invocation_source::
overlapping_segments
(const rescan_macro_invocation::output::tokens_range &range)
  const noexcept
{
  struct cmp {
    bool operator()(const segment &seg,
		    const rescan_macro_invocation::output::tokens_range &range)
      const noexcept
    {
      return seg.invocation_range < range;
    }

    bool operator()(const rescan_macro_invocation::output::tokens_range &range,
		    const segment &seg)
      const noexcept
    {
      return range < seg.invocation_range;
    }
  };

  const auto r = std::equal_range(_segments.cbegin(),
				  _segments.cend(),
				  range, cmp{});
  return std::make_pair(const_segment_iterator{r.first, range},
			const_segment_iterator{r.second, range});
}

const raw_pp_tokens_range expansion_forest::invocation_source::
get_total_spanned_source_range() const noexcept
{
  assert(!_segments.empty());
  return raw_pp_tokens_range{
    static_cast<raw_pp_tokens_range>(_segments.front()).begin,
    static_cast<raw_pp_tokens_range>(_segments.back()).end,
  };
}

void expansion_forest::invocation_source::
_append_raw_pp_token(const raw_pp_token_index raw_tok_index) {
  if (!_segments.empty() &&
      static_cast<raw_pp_tokens_range>(_segments.back()).end == raw_tok_index) {
    ++_segments.back().invocation_range.end;
  } else {
    const auto invocation_range_begin =
      !_segments.empty() ? _segments.back().invocation_range.end : 0;
    _segments.emplace_back
      (segment{
	  raw_tok_index,
	  rescan_macro_invocation::output::tokens_range{
	    invocation_range_begin,
	    invocation_range_begin + 1,
	  }
       });
  }
}

const expansion_forest::rescan_macro_invocation::output::tokens_range
expansion_forest::invocation_source::
_get_total_invocation_range() const noexcept
{
  assert(!_segments.empty());
  return rescan_macro_invocation::output::tokens_range{
    _segments.front().invocation_range.begin,
    _segments.back().invocation_range.end,
  };
}


expansion_forest::const_all_nodes_iterator::
const_all_nodes_iterator() noexcept
  : _forest{nullptr},
    _node_index{
      std::numeric_limits<_forest_nodes::_container_type::size_type>::max()
    }
{}

expansion_forest::const_all_nodes_iterator::
const_all_nodes_iterator(const _forest_nodes &forest,
			 const begin_tag&) noexcept
  : _forest{&forest}, _node_index{0}
{}

expansion_forest::const_all_nodes_iterator::
const_all_nodes_iterator(const _forest_nodes &forest,
			 const end_tag&) noexcept
  : _forest{&forest}, _node_index{forest._nodes.size()}
{}

expansion_forest::const_all_nodes_iterator::
const_all_nodes_iterator(const _forest_nodes &forest,
			 const _forest_nodes::size_type node) noexcept
  : _forest{&forest}, _node_index{node._n}
{}

expansion_forest::const_all_nodes_iterator
expansion_forest::rescan_subtree::all_nodes_begin() const noexcept
{
  return const_all_nodes_iterator{_subtree_root.get_forest(),
				  _subtree_root.get_node()};
}

expansion_forest::const_all_nodes_iterator
expansion_forest::rescan_subtree::all_nodes_end() const noexcept
{
  return const_all_nodes_iterator{
    _subtree_root.get_forest(),
    _subtree_root.get_forest()._rescan_subtree_end(_subtree_root.get_node())
  };
}

bool expansion_forest::const_all_nodes_iterator::
operator==(const const_all_nodes_iterator &rhs)
  const noexcept
{
  assert(_forest == rhs._forest);
  return _node_index == rhs._node_index;
}

expansion_forest::const_all_nodes_iterator::reference
expansion_forest::const_all_nodes_iterator::operator*() const noexcept
{
  return rescan_macro_invocation{
    _rescan_macro_invocation_ref{
      *_forest, _forest_nodes::size_type{_node_index}
    }
  };
}

expansion_forest::const_all_nodes_iterator&
expansion_forest::const_all_nodes_iterator::operator++() noexcept
{
  ++_node_index;
  return *this;
}

expansion_forest::const_all_nodes_iterator
expansion_forest::const_all_nodes_iterator::operator++(int) noexcept
{
  const auto it = *this;
  ++_node_index;
  return it;
}

expansion_forest::const_all_nodes_iterator&
expansion_forest::const_all_nodes_iterator::operator--() noexcept
{
  --_node_index;
  return *this;
}

expansion_forest::const_all_nodes_iterator
expansion_forest::const_all_nodes_iterator::operator--(int) noexcept
{
  const auto it = *this;
  --_node_index;
  return it;
}


pp_result::_pp_tokens_origins::_pp_tokens_origins() noexcept = default;

pp_result::_pp_tokens_origins::_pp_tokens_origins(_pp_tokens_origins&&) noexcept
  = default;

pp_result::_pp_tokens_origins::~_pp_tokens_origins() noexcept = default;

void pp_result::_pp_tokens_origins::
append(raw_pp_token_index raw_index,
       const _macro_invocations_container_type &macro_invocations,
       const from_raw_tag)
{
  if (!_pp_tokens_seqs_origins.empty()) {
    auto &back = _pp_tokens_seqs_origins.back();
    switch (back._kind)
    {
    case _pp_tokens_seq_origin::_kind::from_raw:
      if (back._source.raw_source.range.end == raw_index) {
	++back._source.raw_source.range.end;
	return;
      }
      break;
    case _pp_tokens_seq_origin::_kind::from_macro_expansion:
      break;
    }
  }

  const pp_token_index out_begin = _total_out_end(macro_invocations);
  _pp_tokens_seqs_origins.emplace_back(out_begin, raw_index,
				       from_raw_tag{});
}

void pp_result::_pp_tokens_origins::
append(const _macro_invocations_index_type mi_index,
       const _macro_invocations_container_type &macro_invocations,
       const from_macro_expansion_tag)
{
  const pp_token_index out_begin = _total_out_end(macro_invocations);
  _pp_tokens_seqs_origins.emplace_back(out_begin, mi_index,
				       from_macro_expansion_tag{});
}

void pp_result::_pp_tokens_origins::
drop_pp_tokens_tail(const pp_tokens::size_type new_end,
		    const _macro_invocations_container_type &macro_invocations)
  noexcept
{
  auto e = _lookup(new_end, macro_invocations);
  switch (_pp_tokens_seqs_origins[e.first]._kind) {
  case _pp_tokens_seq_origin::_kind::from_raw:
    if (e.second != 0) {
      _pp_tokens_seqs_origins[e.first]._source.raw_source.range.end =
	(_pp_tokens_seqs_origins[e.first]._source.raw_source.range.begin +
	 e.second);
      e.first += 1;
    }
    break;
  case _pp_tokens_seq_origin::_kind::from_macro_expansion:
    // Dropping tails starting in the middle of a macro expansion
    // is unsupported.
    assert(e.second == 0);
    break;
  }
  _pp_tokens_seqs_origins.erase(_pp_tokens_seqs_origins.cbegin() + e.first,
				_pp_tokens_seqs_origins.cend());
}


raw_pp_tokens_range pp_result::_pp_tokens_origins::
pp_tokens_range_to_raw
(const pp_tokens_range &range,
 const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  auto r = _lookup(range, macro_invocations);
  if (r.first.first == _pp_tokens_seqs_origins.size()) {
    assert(range.begin == range.end);
    const auto raw_pp_tokens_end = _total_raw_end(macro_invocations);
    return raw_pp_tokens_range{raw_pp_tokens_end, raw_pp_tokens_end};
  }
  if (r.second.second == 0 && range.begin != range.end) {
    r.second.first -= 1;
    r.second.second =
      _pp_tokens_seqs_origins[r.second.first].out_length(macro_invocations);
  }
  assert(r.second.first != _pp_tokens_seqs_origins.size());

  const auto begin = [&]{
    const auto &head_seq_origin = _pp_tokens_seqs_origins[r.first.first];
    switch (head_seq_origin._kind) {
    case _pp_tokens_seq_origin::_kind::from_raw:
      return (head_seq_origin.get_raw_pp_tokens_range(macro_invocations).begin +
	      r.first.second);

    case _pp_tokens_seq_origin::_kind::from_macro_expansion:
      return head_seq_origin.get_raw_pp_tokens_range(macro_invocations).begin;
    }
  }();

  const auto end = [&]{
    const auto &tail_seq_origin = _pp_tokens_seqs_origins[r.second.first];
    switch (tail_seq_origin._kind) {
    case _pp_tokens_seq_origin::_kind::from_raw:
      return (tail_seq_origin.get_raw_pp_tokens_range(macro_invocations).begin +
	      r.second.second);

    case _pp_tokens_seq_origin::_kind::from_macro_expansion:
      if (range.begin == range.end && r.second.second == 0) {
	return begin;
      } else {
	return tail_seq_origin.get_raw_pp_tokens_range(macro_invocations).end;
      }
    }
  }();

  return raw_pp_tokens_range{begin, end};
}

raw_pp_tokens_range pp_result::_pp_tokens_origins::
pp_token_index_to_raw
(const pp_token_index tok_index,
 const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  const auto b = _lookup(tok_index, macro_invocations);
  if (b.first == _pp_tokens_seqs_origins.size()) {
    const auto raw_pp_tokens_end = _total_raw_end(macro_invocations);
    return raw_pp_tokens_range{raw_pp_tokens_end, raw_pp_tokens_end};
  }
  const auto head_seq_origin = _pp_tokens_seqs_origins[b.first];
  switch (head_seq_origin._kind) {
  case _pp_tokens_seq_origin::_kind::from_raw:
    return raw_pp_tokens_range{
      (head_seq_origin.get_raw_pp_tokens_range(macro_invocations).begin +
       b.second)
    };
  case _pp_tokens_seq_origin::_kind::from_macro_expansion:
    return head_seq_origin.get_raw_pp_tokens_range(macro_invocations);
  }
}

pp_tokens_range pp_result::_pp_tokens_origins::
raw_pp_tokens_range_to_nonraw
(const raw_pp_tokens_range &raw_range,
 const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  struct cmp
  {
    bool operator()(const _pp_tokens_seq_origin &e,
		    const raw_pp_tokens_range &raw_range) const noexcept {
      // For the lower bound.
      const auto e_raw_range = e.get_raw_pp_tokens_range(macro_invocations);
      if (e_raw_range.begin >= raw_range.begin)
	return false;
      return e_raw_range.end <= raw_range.begin;
    }

    bool operator()(const raw_pp_tokens_range &raw_range,
		    const _pp_tokens_seq_origin &e) const noexcept {
      // For the upper bound.
      const auto e_raw_range = e.get_raw_pp_tokens_range(macro_invocations);
      return raw_range.end <= e_raw_range.begin;
    }

    const _macro_invocations_container_type &macro_invocations;
  };

  auto r = std::equal_range(_pp_tokens_seqs_origins.cbegin(),
			    _pp_tokens_seqs_origins.cend(),
			    raw_range,
			    cmp {macro_invocations});
  assert(r.second != r.first ||
	 (r.second == _pp_tokens_seqs_origins.cend() ||
	  (raw_range.end <=
	   r.first->get_raw_pp_tokens_range(macro_invocations).begin)));
  if (r.second == r.first) {
    if (r.first == _pp_tokens_seqs_origins.cend()) {
      const auto pp_tokens_end = _total_out_end(macro_invocations);
      return pp_tokens_range{pp_tokens_end, pp_tokens_end};
    } else {
      return pp_tokens_range{r.first->_out_begin, r.first->_out_begin};
    }
  }
  assert(r.first != _pp_tokens_seqs_origins.cend());
  r.second -= 1;

  const auto begin = [&]{
    switch (r.first->_kind) {
    case _pp_tokens_seq_origin::_kind::from_raw:
      if (raw_range.begin >=
	  r.first->get_raw_pp_tokens_range(macro_invocations).begin) {
	return (r.first->_out_begin +
		(raw_range.begin -
		 r.first->get_raw_pp_tokens_range(macro_invocations).begin));
      } else {
	return r.first->_out_begin;
      }

    case _pp_tokens_seq_origin::_kind::from_macro_expansion:
      // For raw_pp_tokens_range queries starting in the middle of a macro
      // invocation, return the full macro expansion output range.
      return r.first->_out_begin;
    }
  }();

  const auto end = [&]{
    switch (r.first->_kind) {
    case _pp_tokens_seq_origin::_kind::from_raw:
      if (raw_range.end <=
	  r.second->get_raw_pp_tokens_range(macro_invocations).end) {
	return (r.second->_out_begin +
		(raw_range.end -
		 r.second->get_raw_pp_tokens_range(macro_invocations).begin));
      } else {
	return r.second->out_end(macro_invocations);
      }

    case _pp_tokens_seq_origin::_kind::from_macro_expansion:
      // For raw_pp_tokens_range queries starting in the middle of a macro
      // invocation, return the full macro expansion output range.
      return r.second->out_end(macro_invocations);
    }
  }();

  return pp_tokens_range{begin, end};
}

std::pair<std::pair<pp_result::const_macro_invocation_iterator, pp_token_index>,
	  std::pair<pp_result::const_macro_invocation_iterator, pp_token_index>>
pp_result::_pp_tokens_origins::find_overlapping_macro_invocations
(const pp_tokens_range &range,
 const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  auto r = _lookup(range, macro_invocations);
  if (r.second.second != 0) {
    r.second.first += 1;
  }

  const auto it_first_mi = std::find_if
    (_pp_tokens_seqs_origins.cbegin() + r.first.first,
     _pp_tokens_seqs_origins.cbegin() + r.second.first,
     [](const _pp_tokens_seq_origin  &e) {
       return e._kind == _pp_tokens_seq_origin::_kind::from_macro_expansion;
     });
  if (it_first_mi == _pp_tokens_seqs_origins.cbegin() + r.second.first) {
    return
      std::make_pair
      (std::make_pair(const_macro_invocation_iterator{macro_invocations.cend()},
		      std::numeric_limits<pp_token_index>::max()),
       std::make_pair(const_macro_invocation_iterator{macro_invocations.cend()},
		      std::numeric_limits<pp_token_index>::max()));
  }

  const auto it_last_mi = std::find_if
    (std::make_reverse_iterator(_pp_tokens_seqs_origins.cbegin() +
				r.second.first),
     std::make_reverse_iterator(it_first_mi + 1),
     [](const _pp_tokens_seq_origin  &e) {
       return e._kind == _pp_tokens_seq_origin::_kind::from_macro_expansion;
     });

  return
    std::make_pair
    (std::make_pair(const_macro_invocation_iterator{
		      macro_invocations.cbegin() +
		       it_first_mi->_source.macro_expansion.mi_index
		    },
		    it_first_mi->_out_begin),
     std::make_pair(const_macro_invocation_iterator{
		      macro_invocations.cbegin() +
		       it_last_mi->_source.macro_expansion.mi_index + 1
		    },
		    it_last_mi->out_end(macro_invocations)));
}



pp_token_index pp_result::_pp_tokens_origins::_total_out_end
(const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  if (!_pp_tokens_seqs_origins.empty()) {
    return _pp_tokens_seqs_origins.back().out_end(macro_invocations);
  } else {
    return 0;
  }
}

raw_pp_token_index pp_result::_pp_tokens_origins::_total_raw_end
(const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  if (!_pp_tokens_seqs_origins.empty()) {
    return (_pp_tokens_seqs_origins.back()
	    .get_raw_pp_tokens_range(macro_invocations).end);
  } else {
    return 0;
  }
}


std::pair<pp_result::_pp_tokens_origins::_pp_tokens_seqs_origins_index_type,
	  std::size_t>
pp_result::_pp_tokens_origins::_lookup
(const pp_token_index tok_index,
 const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  const auto b = std::lower_bound
    (_pp_tokens_seqs_origins.cbegin(),
     _pp_tokens_seqs_origins.cend(),
     tok_index,
     [&](const _pp_tokens_seq_origin &e, const pp_token_index tok_index) {
       // For the lower bound.
       if (e._out_begin >= tok_index)
	 return false;
       return e.out_end(macro_invocations) <= tok_index;
     });
  assert(b != _pp_tokens_seqs_origins.cend());
  assert(b->_out_begin <= tok_index);
  assert(tok_index < b->out_end(macro_invocations));
  return std::make_pair(b - _pp_tokens_seqs_origins.cbegin(),
			tok_index - b->_out_begin);
}

std::pair<
  std::pair<pp_result::_pp_tokens_origins::_pp_tokens_seqs_origins_index_type,
	    std::size_t>,
  std::pair<pp_result::_pp_tokens_origins::_pp_tokens_seqs_origins_index_type,
	    std::size_t>
>
pp_result::_pp_tokens_origins::_lookup
(const pp_tokens_range &range,
 const _macro_invocations_container_type &macro_invocations)
  const noexcept {
  struct cmp
  {
    bool operator()(const _pp_tokens_seq_origin &e,
		    const pp_tokens_range &range) const noexcept {
      // For the lower bound.
      if (e._out_begin >= range.begin)
	return false;
      return e.out_end(macro_invocations) <= range.begin;
    }

    bool operator()(const pp_tokens_range &range,
		    const _pp_tokens_seq_origin &e) const noexcept {
      // For the upper bound.
      return range.end <= e._out_begin;
    }

    const _macro_invocations_container_type &macro_invocations;
  };

  auto r = std::equal_range(_pp_tokens_seqs_origins.cbegin(),
			    _pp_tokens_seqs_origins.cend(),
			    range,
			    cmp {macro_invocations});
  assert(r.second != r.first ||
	 (range.begin == range.end &&
	  (r.second == _pp_tokens_seqs_origins.cend() ||
	   range.end == r.second->_out_begin)));
  if (r.second == r.first) {
    return std::make_pair
      (std::make_pair(r.second - _pp_tokens_seqs_origins.cbegin(), 0),
       std::make_pair(r.second - _pp_tokens_seqs_origins.cbegin(), 0));
  }
  assert(r.first != _pp_tokens_seqs_origins.cend());
  assert(r.first->_out_begin <= range.begin);

  const auto begin =
    std::make_pair(r.first - _pp_tokens_seqs_origins.cbegin(),
		   range.begin - r.first->_out_begin);
  auto offset_in_end =
    range.end - (r.second - 1)->_out_begin;
  if (offset_in_end >= (r.second - 1)->out_length(macro_invocations)) {
    // All but the very last special EOF token should be associated
    // with some raw_pp_token.
    assert(offset_in_end <= (r.second - 1)->out_length(macro_invocations) + 1);
    offset_in_end = 0;
  } else {
    r.second -= 1;
    assert(r.second->out_end(macro_invocations) > range.end);
  }
  const auto end =
    std::make_pair(r.second - _pp_tokens_seqs_origins.cbegin(),
		   offset_in_end);
  return std::make_pair(begin, end);
}

pp_result::_pp_tokens_origins::_pp_tokens_seq_origin::
_pp_tokens_seq_origin(const pp_token_index out_begin,
		      const raw_pp_token_index raw_begin,
		      const from_raw_tag) noexcept
  : _out_begin(out_begin), _kind(_kind::from_raw),
    _source({.raw_source = {{raw_begin}}})
{}

pp_result::_pp_tokens_origins::_pp_tokens_seq_origin::
_pp_tokens_seq_origin
(const pp_token_index out_begin,
 const _macro_invocations_index_type mi_index,
 const from_macro_expansion_tag) noexcept
  : _out_begin(out_begin), _kind(_kind::from_macro_expansion),
    _source({.macro_expansion = {mi_index}})
{}


std::size_t pp_result::_pp_tokens_origins::_pp_tokens_seq_origin::
out_length(const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  switch (_kind) {
  case _kind::from_raw:
    return _source.raw_source.range.end - _source.raw_source.range.begin;

  case _kind::from_macro_expansion:
    return (macro_invocations[_source.macro_expansion.mi_index]
	    ->get_expansion_forest().get_main_tree().total_out_length());
  }
}

pp_token_index pp_result::_pp_tokens_origins::_pp_tokens_seq_origin::
out_end(const _macro_invocations_container_type &macro_invocations)
  const noexcept
{
  return _out_begin + out_length(macro_invocations);
}

raw_pp_tokens_range pp_result::_pp_tokens_origins::_pp_tokens_seq_origin::
get_raw_pp_tokens_range
(const _macro_invocations_container_type &macro_invocations) const noexcept
{
  switch (_kind) {
  case _kind::from_raw:
    return _source.raw_source.range;

  case _kind::from_macro_expansion:
    return (macro_invocations[_source.macro_expansion.mi_index]
	    ->get_source_range());
  }
}


pp_result::inclusion_node::inclusion_node()
  : _parent(nullptr)
{}

pp_result::inclusion_node::inclusion_node(inclusion_node * const parent,
					  const raw_pp_token_index range_begin)
  : _parent(parent), _range(range_begin, range_begin)
{}

pp_result::inclusion_node::~inclusion_node() noexcept = default;

pp_result::inclusion_node::recursive_header_inclusion_iterator
pp_result::inclusion_node::header_inclusions_recursive_begin() const
{
  return recursive_header_inclusion_iterator{_recursive_child_iterator{*this}};
}

pp_result::inclusion_node::recursive_header_inclusion_iterator
pp_result::inclusion_node::header_inclusions_recursive_end() const
{
  return recursive_header_inclusion_iterator{};
}


pp_result::header_inclusion_child& pp_result::inclusion_node::
_add_header_inclusion(const std::string &filename,
		      const raw_pp_tokens_range &directive_range,
		      used_macros &&used_macros,
		      macro_nondef_constraints &&macro_nondef_constraints,
		      pp_result &container)
{
  std::unique_ptr<header_inclusion_child> new_child
     (new header_inclusion_child(*this, filename,
				 directive_range,
				 std::move(used_macros),
				 std::move(macro_nondef_constraints),
				 container._next_header_node_id++));
  header_inclusion_child &_new_child = *new_child;
  _children.emplace_back(std::move(new_child));
  return _new_child;
}

pp_result::conditional_inclusion_node& pp_result::inclusion_node::
_add_conditional_inclusion(const raw_pp_token_index range_begin)
{
  std::unique_ptr<conditional_inclusion_node> new_child
     (new conditional_inclusion_node(*this, range_begin));
  conditional_inclusion_node &_new_child = *new_child;
  _children.emplace_back(std::move(new_child));
  return _new_child;
}

void pp_result::inclusion_node::
_set_range_begin(const raw_pp_token_index range_begin) noexcept
{
  _range.begin = range_begin;
}

void pp_result::inclusion_node::
_set_range_end(const raw_pp_token_index range_end) noexcept
{
  _range.end = range_end;
}

pp_result::inclusion_node::_child::
_child(std::unique_ptr<header_inclusion_child> &&_h) noexcept
  : k(kind::header), h(std::move(_h))
{}

pp_result::inclusion_node::_child::
_child(std::unique_ptr<conditional_inclusion_node> &&_c) noexcept
  : k(kind::conditional), c(std::move(_c))
{}

pp_result::inclusion_node::_child::_child(_child &&chld) noexcept
  : k(chld.k), h(nullptr)
{
  switch(k) {
  case kind::header:
    h = std::move(chld.h);
    break;

  case kind::conditional:
    h.~unique_ptr<header_inclusion_child>();
    new (&c) std::unique_ptr<conditional_inclusion_node>{std::move(chld.c)};
    break;
  }
}

pp_result::inclusion_node::_child::~_child() noexcept
{
  switch (k) {
  case kind::header:
    h.~unique_ptr<header_inclusion_child>();
    break;

  case kind::conditional:
    c.~unique_ptr<conditional_inclusion_node>();
    break;
  }
}

const pp_result::inclusion_node&
pp_result::inclusion_node::_child::get_inclusion_node() const noexcept
{
  switch (k) {
  case kind::header:
    return *h;

  case kind::conditional:
    return *c;
  }
}


pp_result::inclusion_node::_recursive_child_iterator::
_recursive_child_iterator() = default;

pp_result::inclusion_node::_recursive_child_iterator::
_recursive_child_iterator(const inclusion_node &n)
{
  if (!n._children.empty()) {
    _walk_stack.push_back(std::make_pair(n._children.begin(),
					 n._children.end()));
  }
}

pp_result::inclusion_node::_recursive_child_iterator&
pp_result::inclusion_node::_recursive_child_iterator::operator++()
{
  // DFS in preorder
  const _children_container_type &children
    = (_walk_stack.back().first->k == _child::kind::header ?
       _walk_stack.back().first->h->_children :
       _walk_stack.back().first->c->_children);

  if (!children.empty()) {
    _walk_stack.push_back(std::make_pair(children.begin(), children.end()));
    return *this;
  }

  ++_walk_stack.back().first;
  while (_walk_stack.back().first == _walk_stack.back().second) {
    _walk_stack.pop_back();
    if (_walk_stack.empty())
      return *this;
    ++_walk_stack.back().first;
  }

  return *this;
}

const pp_result::inclusion_node::_recursive_child_iterator
pp_result::inclusion_node::_recursive_child_iterator::operator++(int)
{
  _recursive_child_iterator it = *this;
  ++*this;
  return it;
}


pp_result::inclusion_node::recursive_header_inclusion_iterator::
recursive_header_inclusion_iterator() = default;

pp_result::inclusion_node::recursive_header_inclusion_iterator::
recursive_header_inclusion_iterator(_recursive_child_iterator &&it)
  : _it(std::move(it))
{
  _skip_non_headers();
}

pp_result::inclusion_node::recursive_header_inclusion_iterator&
pp_result::inclusion_node::recursive_header_inclusion_iterator::operator++()
{
  ++_it;
  _skip_non_headers();
  return *this;
}

const pp_result::inclusion_node::recursive_header_inclusion_iterator
pp_result::inclusion_node::recursive_header_inclusion_iterator::operator++(int)
{
  recursive_header_inclusion_iterator it = *this;
  ++*this;
  return it;
}

void pp_result::inclusion_node::recursive_header_inclusion_iterator::
_skip_non_headers()
{
  while(!_it.is_end() && _it->k != _child::kind::header)
    ++_it;
}


pp_result::header_inclusion_node::
header_inclusion_node(const std::string &filename)
  : _filename(filename), _id(std::numeric_limits<unsigned long>::max())
{}

pp_result::header_inclusion_node::
header_inclusion_node(inclusion_node &parent,
		      const raw_pp_token_index range_begin,
		      const std::string &filename,
		      const unsigned long id)
  : inclusion_node(&parent, range_begin), _filename(filename), _id(id)
{}

pp_result::header_inclusion_node::~header_inclusion_node() noexcept = default;

const pp_result::header_inclusion_node&
pp_result::header_inclusion_node::get_containing_header() const noexcept
{
  return *this;
}

void pp_result::header_inclusion_node::add_line(const std::streamoff length)
{
  _offset_to_line_col_map.add_line(length);
}

std::pair<std::streamoff, std::streamoff>
pp_result::header_inclusion_node::offset_to_line_col(const std::streamoff off)
  const noexcept
{
  return _offset_to_line_col_map.offset_to_line_col(off);
}

std::unique_ptr<source_reader>
pp_result::header_inclusion_node::create_source_reader() const
{
  return std::unique_ptr<source_reader>(new file_source_reader(_filename));
}

unsigned long pp_result::header_inclusion_node::get_id() const noexcept
{
  assert(_id != std::numeric_limits<unsigned long>::max());
  return _id;
}

void pp_result::header_inclusion_node::_set_id(const unsigned long id) noexcept
{
  assert(_id == std::numeric_limits<unsigned long>::max());
  _id = id;
}


pp_result::header_inclusion_root::
header_inclusion_root(const std::string &filename, const bool is_preinclude)
  : header_inclusion_node(filename), _is_preinclude(is_preinclude)
{}

pp_result::header_inclusion_root::~header_inclusion_root() noexcept = default;

const pp_result::conditional_inclusion_node*
pp_result::header_inclusion_root::get_containing_conditional_inclusion()
  const noexcept
{
  return nullptr;
}


pp_result::header_inclusion_child::
header_inclusion_child(inclusion_node &parent,
		       const std::string &filename,
		       const raw_pp_tokens_range &directive_range,
		       used_macros &&used_macros,
		       macro_nondef_constraints &&macro_nondef_constraints,
		       const unsigned long id)
  : header_inclusion_node(parent, directive_range.end, filename, id),
    _directive_range(directive_range),
    _used_macros(std::move(used_macros)),
    _macro_nondef_constraints(std::move(macro_nondef_constraints))
{}

pp_result::header_inclusion_child::~header_inclusion_child() noexcept = default;

const pp_result::conditional_inclusion_node*
pp_result::header_inclusion_child::get_containing_conditional_inclusion()
  const noexcept
{
  return this->get_parent()->get_containing_conditional_inclusion();
}


pp_result::conditional_inclusion_node::
conditional_inclusion_node(inclusion_node &parent,
			   const raw_pp_token_index range_begin)
  : inclusion_node(&parent, range_begin), _taken_branch(0)
{}

pp_result::conditional_inclusion_node::~conditional_inclusion_node() = default;

const pp_result::header_inclusion_node&
pp_result::conditional_inclusion_node::get_containing_header() const noexcept
{
  return _parent->get_containing_header();
}

const pp_result::conditional_inclusion_node*
pp_result::conditional_inclusion_node::get_containing_conditional_inclusion()
  const noexcept
{
  return this;
}

pp_result::conditional_inclusion_node::directive_ranges_type::size_type
pp_result::conditional_inclusion_node::nbranches() const noexcept
{
  return _directive_ranges.size() - 1;
}

const raw_pp_tokens_range& pp_result::conditional_inclusion_node::
get_branch_directive_range(const directive_ranges_type::size_type i)
  const noexcept
{
  assert(i < _directive_ranges.size());
  return _directive_ranges[i];
}

bool pp_result::conditional_inclusion_node::has_taken_branch() const noexcept
{
  return _taken_branch;
}

pp_result::conditional_inclusion_node::directive_ranges_type::size_type
pp_result::conditional_inclusion_node::get_taken_branch() const noexcept
{
  assert(_taken_branch);
  assert(_taken_branch < _directive_ranges.size());
  return _taken_branch - 1;
}

void pp_result::conditional_inclusion_node::
_finalize(const raw_pp_token_index range_end,
	  used_macros &&used_macros,
	  macro_nondef_constraints &&macro_nondef_constraints,
	  directive_ranges_type &&directive_ranges,
	  const directive_ranges_type::size_type taken_branch,
	  const bool has_else_branch)
{
  _set_range_end(range_end);
  _used_macros = std::move(used_macros);
  _macro_nondef_constraints = std::move(macro_nondef_constraints);
  _directive_ranges = std::move(directive_ranges);
  _taken_branch = taken_branch;
  _has_else_branch = has_else_branch;
}


pp_result::header_inclusion_roots::header_inclusion_roots() = default;

pp_result::header_inclusion_roots::
header_inclusion_roots(header_inclusion_roots &&hirs)
  : _roots(std::move(hirs._roots))
{}

pp_result::header_inclusion_roots::~header_inclusion_roots() noexcept = default;

pp_result::header_inclusion_root&
pp_result::header_inclusion_roots::add(const std::string &filename,
				       const bool is_preinclude)
{
  std::unique_ptr<header_inclusion_root> n{
    new header_inclusion_root{filename, is_preinclude}
  };

  _roots.emplace_back(std::move(n));
  return *_roots.back();
}


template <typename node_type>
pp_result::_const_intersecting_child_node_iterator<node_type>::
_const_intersecting_child_node_iterator(const header_inclusion_root &root,
					const raw_pp_tokens_range &range)
  : _range(range), _pos(_range.begin), _cur_val(nullptr)
{
  auto chlds_sr = _intersecting_chlds_subrange(root._children.cbegin(),
					       root._children.end());
  if (chlds_sr.first != chlds_sr.second) {
    // Descend
    _walk_stack.emplace_back(_walk_stack_entry{chlds_sr.first,
			     chlds_sr.first,
			     chlds_sr.second});
    _descend_walk_stack();
    _fwd_walk_stack_to_node_kind();
  }

  if (!_walk_stack.empty()) {
    _cur_val = _find_deepest_covering_pos();
    if (!_cur_val)
      _cur_val = _find_node_after_pos();
    assert(_cur_val);
    _pos = _cur_val->get_range().begin;
  } else {
    _pos = _range.end;
  }
}

template <typename node_type>
pp_result::_const_intersecting_child_node_iterator<node_type>::
_const_intersecting_child_node_iterator(const raw_pp_tokens_range &range)
  noexcept
  : _range(range), _pos(_range.end), _cur_val(nullptr)
{}

template <typename node_type>
bool
pp_result::_const_intersecting_child_node_iterator<node_type>::
operator==(const _const_intersecting_child_node_iterator &rhs) const noexcept
{
  assert(this->_range == rhs._range);
  return (this->_pos == rhs._pos);
}

template <typename node_type>
void pp_result::_const_intersecting_child_node_iterator<node_type>::
advance()
{
  assert(!_walk_stack.empty());
  assert(_walk_stack.back().chlds_it->k == _get_node_kind());

  if (_pos < _access_chld(*_walk_stack.back().chlds_it).get_range().begin) {
    // The current _pos is somewhere to the left of the current
    // leaf node. Find the deepest node of the desired type coming next.
    _cur_val = _find_node_after_pos();
    assert(_cur_val);
    _pos = _cur_val->get_range().begin;

  } else {
    assert(_access_chld(*_walk_stack.back().chlds_it).get_range().begin <=
	   _pos);
    assert(_pos <=
	   _access_chld(*_walk_stack.back().chlds_it).get_range().end);
    const raw_pp_token_index end_pos =
      _walk_stack.back().chlds_it->h->get_range().end;
    if (_range.end <= end_pos) {
      // We're done.
      _pos = _range.end;
      _walk_stack.clear();
      _cur_val = nullptr;
      return;
    }

    _pos = end_pos;

    // Move the walk stack to the next DFS post node of the desired
    // kind not ending at end_pos.
    do {
      if (_walk_stack.back().chlds_it + 1 != _walk_stack.back().chlds_end) {
	++_walk_stack.back().chlds_it;
	_descend_walk_stack();
	_fwd_walk_stack_to_node_kind();
      } else {
	_walk_stack.pop_back();
	if (!_walk_stack.empty())
	  _fwd_walk_stack_to_node_kind();
      }
    } while (!_walk_stack.empty() &&
	     (_access_chld(*_walk_stack.back().chlds_it).get_range().end ==
	      end_pos));

    if (_walk_stack.empty()) {
      _pos = _range.end;
      _walk_stack.clear();
      _cur_val = nullptr;
      return;
    }

    // There are two possible cases now: some of the nodes of the
    // desired kind on the walk stack either overlap with the current
    // _pos or there is a gap at the top level.
    _cur_val = _find_deepest_covering_pos();
    if (!_cur_val) {
      // There aren't any. Advance past the top level gap.
      assert(_pos <
	     _access_chld(*_walk_stack.back().chlds_it).get_range().begin);
      advance();
    }
  }
}

template <>
pp_result::inclusion_node::_child::kind
pp_result::_const_intersecting_child_node_iterator
				<pp_result::header_inclusion_child>::
_get_node_kind() noexcept
{
  return inclusion_node::_child::kind::header;
}

template <>
pp_result::inclusion_node::_child::kind
pp_result::_const_intersecting_child_node_iterator
				<pp_result::conditional_inclusion_node>::
_get_node_kind() noexcept
{
  return inclusion_node::_child::kind::conditional;
}

template <>
const pp_result::header_inclusion_child&
pp_result::_const_intersecting_child_node_iterator
				<pp_result::header_inclusion_child>::
_access_chld(const inclusion_node::_child &c) noexcept
{
  assert(c.k == _get_node_kind());
  return *c.h;
}

template <>
const pp_result::conditional_inclusion_node&
pp_result::_const_intersecting_child_node_iterator
				<pp_result::conditional_inclusion_node>::
_access_chld(const inclusion_node::_child &c) noexcept
{
  assert(c.k == _get_node_kind());
  return *c.c;
}

template <typename node_type>
std::pair<pp_result::inclusion_node::_children_container_type::const_iterator,
	  pp_result::inclusion_node::_children_container_type::const_iterator>
pp_result::_const_intersecting_child_node_iterator<node_type>::
_intersecting_chlds_subrange
(const inclusion_node::_children_container_type::const_iterator &b,
 const inclusion_node::_children_container_type::const_iterator &e) noexcept
{
  const auto srb =
    std::lower_bound(b, e, _range,
		     [](const inclusion_node::_child &c,
			const raw_pp_tokens_range &r) {
		       return (c.get_inclusion_node().get_range() < r);
		     });
  const auto sre =
    std::upper_bound(srb, e, _range,
		     [](const raw_pp_tokens_range &r,
			const inclusion_node::_child &c) {
		       return r < c.get_inclusion_node().get_range();
		     });
  return std::make_pair(srb, sre);
}

template <typename node_type>
void pp_result::_const_intersecting_child_node_iterator<node_type>::
_descend_walk_stack()
{
  assert(!_walk_stack.empty());
  const inclusion_node::_child *cur = &*_walk_stack.back().chlds_it;
  const inclusion_node *n = &cur->get_inclusion_node();
  auto chlds_sr = _intersecting_chlds_subrange(n->_children.cbegin(),
					       n->_children.end());
  while (chlds_sr.first != chlds_sr.second) {
    // Descend
    _walk_stack.emplace_back(_walk_stack_entry{chlds_sr.first,
			     chlds_sr.first,
			     chlds_sr.second});
    cur = &*_walk_stack.back().chlds_it;
    n = &cur->get_inclusion_node();
    chlds_sr = _intersecting_chlds_subrange(n->_children.cbegin(),
					    n->_children.end());
  }
}

template <typename node_type>
void pp_result::_const_intersecting_child_node_iterator<node_type>::
_fwd_walk_stack_to_node_kind()
{
  // Move the walk stack to the next (in DFS post order) node of the
  // desired kind.
  assert(!_walk_stack.empty());
  assert(_walk_stack.back().chlds_it != _walk_stack.back().chlds_end);
  while (!_walk_stack.empty() &&
	 _walk_stack.back().chlds_it->k != _get_node_kind()) {
    if (_walk_stack.back().chlds_it + 1 != _walk_stack.back().chlds_end) {
      ++_walk_stack.back().chlds_it;
      _descend_walk_stack();
    } else {
      _walk_stack.pop_back();
    }
  }
}

template <typename node_type>
const node_type *
pp_result::_const_intersecting_child_node_iterator<node_type>::
_find_deepest_covering_pos() noexcept
{
  // Find the deepest node of the desired kind on the walk stack
  // which begins before or at the current _pos.
  auto deepest_covering_pos =
    std::find_if(_walk_stack.crbegin(), _walk_stack.crend(),
		 [&](const _walk_stack_entry &e) {
		   return (e.chlds_it->k == _get_node_kind() &&
			   (_access_chld(*e.chlds_it).get_range().begin
			    <= _pos));
		 });

  if (deepest_covering_pos != _walk_stack.crend())
    return &_access_chld(*deepest_covering_pos->chlds_it);
  else
    return nullptr;
}

template <typename node_type>
const node_type *
pp_result::_const_intersecting_child_node_iterator<node_type>::
_find_node_after_pos() noexcept
{
  // Find the first node of the desired kind on the walk stack
  // which begins after the current _pos. If there are multiple
  // (nested) candidates, take the innermost one.
  const auto topmost_gt_pos =
    std::find_if(_walk_stack.cbegin(), _walk_stack.cend(),
		 [&](const _walk_stack_entry &e) {
		   return (e.chlds_it->k == _get_node_kind() &&
			   (_access_chld(*e.chlds_it).get_range().begin
			    > _pos));
		 });
  assert(topmost_gt_pos != _walk_stack.cend());
  const auto next_pos =
    _access_chld(*topmost_gt_pos->chlds_it).get_range().begin;
  const auto deepest_at_pos =
    std::find_if(_walk_stack.crbegin(), _walk_stack.crend(),
		 [&](const _walk_stack_entry &e) {
		   return (e.chlds_it->k == _get_node_kind() &&
			   (_access_chld(*e.chlds_it).get_range().begin
			    == next_pos));
		 });
  assert(deepest_at_pos != _walk_stack.crend());
  return &_access_chld(*deepest_at_pos->chlds_it);
}


pp_result::const_intersecting_source_iterator::
const_intersecting_source_iterator(const header_inclusion_roots &roots,
				   const raw_pp_tokens_range &range,
				   const init_begin_iterator_tag&)
  : _range(range),
    _roots(_intersecting_roots_subrange(roots.begin(), roots.end())),
    _roots_it(_roots.first),
    _child_it(_range), _max_pos_seen_from_chlds(_range.begin)
{

  if (_roots.first != _roots.second) {
    _child_it =
      _const_intersecting_child_node_iterator<header_inclusion_child> {
	*_roots_it, _range
      };
  } else {
    // Range is an empty range at EOF, special case it.
    --_roots.first;
    _roots_it = _roots.first;
    assert(range.begin == range.end);
    assert(range.begin == _roots.first->get_range().end);
  }
}

pp_result::const_intersecting_source_iterator::
const_intersecting_source_iterator(const header_inclusion_roots &roots,
				   const raw_pp_tokens_range &range,
				   const init_end_iterator_tag&) noexcept
  : _range(range),
    _roots(_intersecting_roots_subrange(roots.begin(), roots.end())),
    _roots_it(_roots.second),
    _child_it(_range), _max_pos_seen_from_chlds(_range.end)
{
  if (_roots.first == roots.end()) {
    // C.f. begin iterator construction
    --_roots.first;
  }
}

bool pp_result::const_intersecting_source_iterator::
operator==(const const_intersecting_source_iterator &rhs) const noexcept
{
  assert(this->_range == rhs._range);
  return (this->_roots_it == rhs._roots_it &&
	  this->_child_it == rhs._child_it);
}

pp_result::const_intersecting_source_iterator::reference
pp_result::const_intersecting_source_iterator::operator*() const noexcept
{
  assert(_roots_it != _roots.second);

  if (_child_it.get_value() &&
      _max_pos_seen_from_chlds >= _child_it.get_value()->get_range().begin) {
    return *_child_it.get_value();
  } else {
    // Current position is within some gap not covered by any child
    // header.
    return *_roots_it;
  }
}

pp_result::const_intersecting_source_iterator&
pp_result::const_intersecting_source_iterator::operator++()
{
  assert(_roots_it != _roots.second);
  if (_child_it.get_value()) {
    if (_max_pos_seen_from_chlds < _child_it.get_value()->get_range().begin) {
      // We're currently within some range not covered by any
      // children. Move on to the next child.
      _max_pos_seen_from_chlds = _child_it.get_value()->get_range().begin;
    } else {
      _max_pos_seen_from_chlds =
	std::max(_child_it.get_value()->get_range().end,
		 _max_pos_seen_from_chlds);
      _child_it.advance();

      if (!_child_it.get_value() &&
	  (_max_pos_seen_from_chlds >= _roots_it->get_range().end ||
	   _max_pos_seen_from_chlds >= _range.end)) {
	  _next_root();
      }
    }
  } else {
    // Current position is within the current root header, after all
    // of its children, if any.
    _next_root();
  }
  return *this;
}

const pp_result::const_intersecting_source_iterator
pp_result::const_intersecting_source_iterator::operator++(int)
{
  const_intersecting_source_iterator it = *this;
  ++*this;
  return it;
}

std::pair<pp_result::header_inclusion_roots::const_iterator,
	  pp_result::header_inclusion_roots::const_iterator>
pp_result::const_intersecting_source_iterator::_intersecting_roots_subrange
			(const header_inclusion_roots::const_iterator &b,
			 const header_inclusion_roots::const_iterator &e)
  noexcept
{
  const auto srb =
    std::lower_bound(b, e, _range,
		     [&](const header_inclusion_root &root,
			 const raw_pp_tokens_range &range) {
		       return root.get_range() < range;
		     });
  const auto sre =
    std::upper_bound(srb, e, _range,
		     [&](const raw_pp_tokens_range &range,
			 const header_inclusion_root &root) {
		       return range < root.get_range();
		     });

  return std::make_pair(srb, sre);
}

void pp_result::const_intersecting_source_iterator::_next_root()
{
  assert(_roots_it != _roots.second);
  ++_roots_it;

  if (_roots_it != _roots.second) {
    _child_it =
      _const_intersecting_child_node_iterator<header_inclusion_child> {
	*_roots_it, _range
      };
    _max_pos_seen_from_chlds = _roots_it->get_range().begin;

  } else {
    _child_it =
      _const_intersecting_child_node_iterator<header_inclusion_child> {
	_range
      };
    _max_pos_seen_from_chlds = _range.end;

  }
}


pp_result::const_intersecting_conditional_iterator::
const_intersecting_conditional_iterator(const header_inclusion_roots &roots,
					const raw_pp_tokens_range &range,
					const init_begin_iterator_tag&)
  : _range(range),
    _roots(_intersecting_roots_subrange(roots.begin(), roots.end())),
    _roots_it(_roots.first),
    _child_it(_range)
{
  _enter_root();
}

pp_result::const_intersecting_conditional_iterator::
const_intersecting_conditional_iterator(const header_inclusion_roots &roots,
					const raw_pp_tokens_range &range,
					const init_end_iterator_tag&)
  noexcept
  : _range(range),
    _roots(_intersecting_roots_subrange(roots.begin(), roots.end())),
    _roots_it(_roots.second),
    _child_it(_range)
{}

bool pp_result::const_intersecting_conditional_iterator::
operator==(const const_intersecting_conditional_iterator &rhs) const noexcept
{
  assert(this->_range == rhs._range);
  return (this->_roots_it == rhs._roots_it &&
	  this->_child_it == rhs._child_it);
}

pp_result::const_intersecting_conditional_iterator::reference
pp_result::const_intersecting_conditional_iterator::operator*() const noexcept
{
  assert(_child_it.get_value());
  return *_child_it.get_value();
}

pp_result::const_intersecting_conditional_iterator&
pp_result::const_intersecting_conditional_iterator::operator++()
{
  assert(_child_it.get_value());
  _child_it.advance();
  if (!_child_it.get_value()) {
    ++_roots_it;
    _enter_root();
  }
  return *this;
}

const pp_result::const_intersecting_conditional_iterator
pp_result::const_intersecting_conditional_iterator::operator++(int)
{
  const_intersecting_conditional_iterator it = *this;
  ++*this;
  return it;
}

std::pair<pp_result::header_inclusion_roots::const_iterator,
	  pp_result::header_inclusion_roots::const_iterator>
pp_result::const_intersecting_conditional_iterator::
_intersecting_roots_subrange(const header_inclusion_roots::const_iterator &b,
			     const header_inclusion_roots::const_iterator &e)
  noexcept
{
  const auto srb =
    std::lower_bound(b, e, _range,
		     [&](const header_inclusion_root &root,
			 const raw_pp_tokens_range &range) {
		       return root.get_range() < range;
		     });
  const auto sre =
    std::upper_bound(srb, e, _range,
		     [&](const raw_pp_tokens_range &range,
			 const header_inclusion_root &root) {
		       return range < root.get_range();
		     });

  return std::make_pair(srb, sre);
}

void pp_result::const_intersecting_conditional_iterator::_enter_root()
{
  if (_roots_it == _roots.second) {
    assert(!_child_it.get_value());
    return;
  }

  // Skip roots without any condition inclusion descendants.
  do {
    _child_it =
      _const_intersecting_child_node_iterator<conditional_inclusion_node> {
	*_roots_it, _range
      };
  } while (!_child_it.get_value() && ++_roots_it != _roots.second);
}


pp_result::pp_result()
  : _next_header_node_id(0), _next_predefinition_pos(0)
{}

pp_result::pp_result(header_inclusion_roots &&header_inclusion_roots)
  : _header_inclusion_roots(std::move(header_inclusion_roots)),
    _next_header_node_id(0), _next_predefinition_pos(0)
{}

std::pair<pp_result::const_directive_iterator,
	  pp_result::const_directive_iterator>
pp_result::find_overlapping_directives(const raw_pp_tokens_range &r)
  const noexcept
{
  return std::equal_range(directives_begin(), directives_end(), r);
}

std::pair<pp_result::const_macro_iterator, pp_result::const_macro_iterator>
pp_result::find_overlapping_macros(const raw_pp_tokens_range &r) const noexcept
{
  return std::equal_range(macros_begin(), macros_end(), r);
}

std::pair<pp_result::const_macro_undef_iterator,
	  pp_result::const_macro_undef_iterator>
pp_result::find_overlapping_macro_undefs(const raw_pp_tokens_range &r)
  const noexcept
{
  return std::equal_range(macro_undefs_begin(), macro_undefs_end(), r);
}


std::pair<pp_result::const_macro_invocation_iterator,
	  pp_result::const_macro_invocation_iterator>
pp_result::find_overlapping_macro_invocations(const raw_pp_tokens_range &r)
  const noexcept
{
  return std::equal_range(macro_invocations_begin(), macro_invocations_end(),
			  r);
}

std::pair<std::pair<pp_result::const_macro_invocation_iterator, pp_token_index>,
	  std::pair<pp_result::const_macro_invocation_iterator, pp_token_index>>
pp_result::find_overlapping_macro_invocations(const pp_tokens_range &r)
  const noexcept
{
  return
    _pp_tokens_origins.find_overlapping_macro_invocations(r,
							  _macro_invocations);
}

pp_result::const_intersecting_source_iterator
pp_result::intersecting_sources_begin(const raw_pp_tokens_range &range) const
{
  return (const_intersecting_source_iterator
	  {_header_inclusion_roots, range,
	   const_intersecting_source_iterator::init_begin_iterator_tag{}});
}

pp_result::const_intersecting_source_iterator
pp_result::intersecting_sources_end(const raw_pp_tokens_range &range)
  const noexcept
{
  return (const_intersecting_source_iterator
	  {_header_inclusion_roots, range,
	   const_intersecting_source_iterator::init_end_iterator_tag{}});
}

pp_result::const_intersecting_conditional_iterator
pp_result::intersecting_conditionals_begin(const raw_pp_tokens_range &range)
  const
{
  return (const_intersecting_conditional_iterator
	  {_header_inclusion_roots, range,
	   const_intersecting_conditional_iterator::init_begin_iterator_tag{}});
}

pp_result::const_intersecting_conditional_iterator
pp_result::intersecting_conditionals_end(const raw_pp_tokens_range &range)
  const noexcept
{
  return (const_intersecting_conditional_iterator
	  {_header_inclusion_roots, range,
	   const_intersecting_conditional_iterator::init_end_iterator_tag{}});
}

const pp_result::header_inclusion_node&
pp_result::get_raw_token_source(const raw_pp_token_index tok_index) const
{
  const auto it =
    this->intersecting_sources_begin(raw_pp_tokens_range{tok_index});
  assert(it != this->intersecting_sources_end(raw_pp_tokens_range{tok_index}));
  return *it;
}

const raw_pp_tokens_range
pp_result::pp_tokens_range_to_raw(const pp_tokens_range &r) const noexcept
{
  return _pp_tokens_origins.pp_tokens_range_to_raw(r, _macro_invocations);
}
const raw_pp_tokens_range
pp_result::pp_token_index_to_raw(const pp_token_index tok_index) const noexcept
{
  return _pp_tokens_origins.pp_token_index_to_raw(tok_index,
						  _macro_invocations);
}

const pp_tokens_range
pp_result::raw_pp_tokens_range_to_nonraw(const raw_pp_tokens_range &r)
  const noexcept
{
  return _pp_tokens_origins.raw_pp_tokens_range_to_nonraw(r,
							  _macro_invocations);
}

void pp_result::_enter_root(header_inclusion_root &hir,
			    const raw_pp_token_index range_begin) noexcept
{
  hir._set_range_begin(range_begin);
  hir._set_id(_next_header_node_id++);
}

void pp_result::_append_token(const raw_pp_token &tok)
{
  _raw_tokens.push_back(tok);
}

void pp_result::_append_token(raw_pp_token &&tok)
{
  _raw_tokens.push_back(std::move(tok));
}

void pp_result::_append_token(pp_token &&tok)
{
  _pp_tokens.push_back(std::move(tok));
}

raw_pp_tokens::size_type pp_result::_get_last_raw_index() const noexcept
{
  assert(!_raw_tokens.empty());
  return _raw_tokens.size() - 1;
}

pp_tokens::size_type pp_result::_get_last_pp_index() const noexcept
{
  assert(!_pp_tokens.empty());
  return _pp_tokens.size() - 1;
}

void pp_result::_add_directive(const raw_pp_tokens_range &directive_range)
{
  _directives.emplace_back(directive_range);
}

const pp_result::macro& pp_result::
_add_macro(const std::string &name,
	   const bool func_like,
	   const bool variadic,
	   std::vector<std::string> &&arg_names,
	   raw_pp_tokens &&repl,
	   const raw_pp_tokens_range &repl_range,
	   const raw_pp_tokens_range &directive_range)
{
  _macros.push_back(std::unique_ptr<const macro>{
			new macro{name, func_like, variadic,
				  std::move(arg_names), std::move(repl),
				  repl_range, directive_range}});
  return *_macros.back();
}

const pp_result::macro& pp_result::
_add_macro(const std::string &name, const macro::builtin_special_tag &tag)
{
  _macros.push_back(std::unique_ptr<const macro>{
			new macro{name, _next_predefinition_pos++, tag}});
  return *_macros.back();
}

const pp_result::macro& pp_result::
_add_macro(const std::string &name, std::vector<std::string> &&arg_names,
	   const macro::builtin_special_tag &tag)
{
  _macros.push_back(std::unique_ptr<const macro>{
			new macro{name, std::move(arg_names),
				  _next_predefinition_pos++, tag}});
  return *_macros.back();
}

const pp_result::macro&
pp_result::_add_macro(const std::string &name,
		      const bool func_like,
		      const bool variadic,
		      std::vector<std::string> &&arg_names,
		      raw_pp_tokens &&repl,
		      const macro::builtin_tag &tag)
{
  _macros.push_back(std::unique_ptr<const macro>{
			new macro{name, func_like, variadic,
				  std::move(arg_names), std::move(repl),
				  _next_predefinition_pos++, tag}});
  return *_macros.back();
}

const pp_result::macro&
pp_result::_add_macro(const std::string &name,
		      const bool func_like,
		      const bool variadic,
		      std::vector<std::string> &&arg_names,
		      raw_pp_tokens &&repl,
		      const macro::predefined_user_tag &tag)
{
  _macros.push_back(std::unique_ptr<const macro>{
			new macro{name, func_like, variadic,
				  std::move(arg_names), std::move(repl),
				  _next_predefinition_pos++, tag}});
  return *_macros.back();
}

void pp_result::
_add_macro_undef(const std::string &name,
		 const raw_pp_tokens_range &directive_range)
{
  _macro_undefs.emplace_back(name, directive_range);
}

void pp_result::_add_macro_undef(const std::string &name,
				 const macro_undef::predefined_user_tag &tag)
{
  _macro_undefs.emplace_back(name, _next_predefinition_pos++, tag);
}

pp_result::macro_invocation& pp_result::
_add_macro_invocation(const macro &m,
		      macro_invocation::expansion_forest::invocation_source
			&&initial_invocation_source)
{
  _macro_invocations.push_back
    (std::unique_ptr<macro_invocation>
     {new macro_invocation{m, std::move(initial_invocation_source)}});
  return *_macro_invocations.back();
}

std::pair<pp_result::used_macros, pp_result::macro_nondef_constraints>
pp_result::_drop_pp_tokens_tail(const pp_tokens::size_type new_end)
{
  // The preprocessor temporarily pushed some pp_token instances when
  // doing macro expansion for #include directives or evaluating
  // conditional #if expressions. Purge these alongside with any
  // associated macro_invocations.
  // Collect any used_macros and macro_nondef_constraints of this
  // tail sequence and return it.
  used_macros um;
  macro_nondef_constraints mnc;

  assert(new_end <= _pp_tokens.size());

  if (new_end == _pp_tokens.size())
    return std::make_pair(std::move(um), std::move(mnc));

  // Each pp_token is either the result of a macro_invocation or not.
  // If yes, then all used_macros and macro_nondef_constraints can be
  // found at this macro_invocation. Otherwise, the
  // macro_nondef_constraints for identifier tokens can be derived
  // from the pp_token_value itself.
  const auto mis_range = [&]() {
    const auto mis_range
      = (this->find_overlapping_macro_invocations
	 (pp_tokens_range{new_end, _pp_tokens.size()}));
    return std::make_pair(mis_range.first.first, mis_range.second.first);
  }();
  auto it_mi = mis_range.first;
  auto tok_index = new_end;
  while (tok_index != _pp_tokens.size()) {
    if (it_mi != mis_range.second) {
      const auto mi_output_range = it_mi->get_output_range(*this);
      if (tok_index >= mi_output_range.begin) {
	um += it_mi->get_used_macros();
	mnc += it_mi->get_macro_nondef_constraints();

	// Advance the current token iterator past the tokens from the
	// macro_invocation just processed.
	tok_index = std::min(_pp_tokens.size(), mi_output_range.end);
	++it_mi;
	continue;
      }
    }

    if (_pp_tokens[tok_index].is_id()) {
      // Look ahead to check whether the identifier token is not
      // followed by an opening parenthesis and thus, a function style
      // definition, even if present, would not alter the result. If
      // the next token is a parenthesis, but it itself or any
      // whitespace before it is the result of some macro invocation,
      // then it can be safely ignored.
      auto next_tok_index = tok_index + 1;
      while (next_tok_index != _pp_tokens.size() &&
	     (_pp_tokens[next_tok_index]
	      .is_type_any_of<pp_token::type::ws,
			      pp_token::type::empty,
			      pp_token::type::newline>())) {
	++next_tok_index;
      }

      bool func_like_allowed = false;
      if (next_tok_index == _pp_tokens.size() ||
	  !_pp_tokens[next_tok_index].is_punctuator("(") ||
	  (it_mi != mis_range.second &&
	   next_tok_index >= it_mi->get_output_range(*this).begin)) {
	func_like_allowed = true;
      }

      mnc += macro_nondef_constraint{
	       _pp_tokens[tok_index].get_value(),
	       func_like_allowed
	     };
      ++tok_index;

    } else {
      ++tok_index;

    }
  }

  _pp_tokens_origins.drop_pp_tokens_tail(new_end, _macro_invocations);
  _macro_invocations.erase((_macro_invocations.begin() +
			    (mis_range.first._it -
			     _macro_invocations.cbegin())),
			   (_macro_invocations.begin() +
			    (mis_range.second._it -
			     _macro_invocations.cbegin())));
  _pp_tokens.shrink(new_end);

  return std::make_pair(std::move(um), std::move(mnc));
}
