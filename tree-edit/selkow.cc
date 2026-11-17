/*
 * Copyright (C) 2026  SUSE Software Solutions Germany GmbH
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

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include "selkow.hh"
#include "../string-edit/string_edit.hh"

using namespace klp::ted;

// The children sequences get aligned with the string edit distance
// machinery, which uses the same cost representation.
static_assert(std::is_same_v<cost_type, klp::sed::cost_type>);
static_assert(INFINITE_COST == klp::sed::INFINITE_COST);

namespace
{
  std::size_t checked_mul(const std::size_t a, const std::size_t b)
  {
    if (b && a > std::numeric_limits<std::size_t>::max() / b)
      throw std::overflow_error("selkow: size_t multiplication overflow");
    return a * b;
  }

  // Returns the DFS post-order indices of a node's children, from left
  // to right.
  std::vector<std::size_t> get_children(const tree &t, const std::size_t post)
  {
    const auto children = t.get_children(post);
    return std::vector<std::size_t>(children.begin(), children.end());
  }

  // Indexed by DFS post-order: the sum of node_cost() over each node's
  // subtree. Children precede their parent in DFS post-order.
  template <typename node_cost_type>
  std::vector<cost_type> subtree_costs(const tree &t,
				       node_cost_type &&node_cost)
  {
    std::vector<cost_type> r(t.size());
    for (std::size_t post = 0; post < t.size(); ++post) {
      cost_type c = node_cost(t.post_to_pre(post));
      for (const std::size_t child : t.get_children(post))
	c = cost_add(c, r[child]);
      r[post] = c;
    }
    return r;
  }

  // The costs for aligning the children sequences of a pair of nodes:
  // matching two children costs their subtrees' top-down distance,
  // deleting or inserting one costs deleting or inserting its whole
  // subtree. The children pairs' distances are taken from a table
  // indexed by the first tree's child's position times the number of
  // the second tree's children plus the latter's child's position. For
  // use with string_edit_distance_stepper, the table may also hold a
  // single row, in which case first_row is the row's position.
  class children_align_cost : public klp::sed::string_edit_cost
  {
  public:
    children_align_cost(const std::vector<std::size_t> &f_children,
			const std::vector<std::size_t> &g_children,
			const std::vector<cost_type> &subtree_del,
			const std::vector<cost_type> &subtree_insert,
			const std::vector<cost_type> &distances,
			const std::size_t first_row = 0) noexcept
      : _f_children(f_children), _g_children(g_children),
	_subtree_del(subtree_del), _subtree_insert(subtree_insert),
	_distances(distances), _first_row(first_row)
    {}

    virtual cost_type cost_match(const std::size_t i0,
				 const std::size_t i1) const override
    {
      assert(i0 >= _first_row);
      return _distances[(i0 - _first_row) * _g_children.size() + i1];
    }

    virtual cost_type cost_del(const std::size_t i0) const override
    {
      return _subtree_del[_f_children[i0]];
    }

    virtual cost_type cost_insert(const std::size_t i1) const override
    {
      return _subtree_insert[_g_children[i1]];
    }

  private:
    const std::vector<std::size_t> &_f_children;
    const std::vector<std::size_t> &_g_children;
    const std::vector<cost_type> &_subtree_del;
    const std::vector<cost_type> &_subtree_insert;
    const std::vector<cost_type> &_distances;
    const std::size_t _first_row;
  };

  // The top-down distance computation's state for a pair of trees.
  class selkow_context
  {
  public:
    selkow_context(const tree &f, const tree &g, const tree_edit_cost &cost);

    // Returns the top-down distance between the subtrees rooted at the
    // nodes with DFS post-order indices v and w, with v and w matched.
    cost_type top_down_distance(const std::size_t v, const std::size_t w);

    const tree &f;
    const tree &g;
    const tree_edit_cost &cost;

    // Indexed by DFS post-order: the costs of deleting each of the
    // first tree's nodes' subtrees and inserting each of the second
    // tree's ones.
    const std::vector<cost_type> subtree_del;
    const std::vector<cost_type> subtree_insert;

  private:
    // A pair of nodes whose children pairs' distances are still being
    // computed, one row of the first node's children after the other,
    // each row getting fed into the children sequences' string edit
    // distance computation once complete.
    struct _frame
    {
      cost_type match;
      std::vector<std::size_t> f_children;
      std::vector<std::size_t> g_children;
      // The current row's position and the distances between its
      // child and the second node's children computed so far.
      std::size_t i;
      std::vector<cost_type> row;
      klp::sed::string_edit_distance_stepper stepper;
    };

    // Returns the top-down distance between the subtrees rooted at v
    // and w right away if possible, i.e. if their matching cost is
    // infinite or either is a leaf. Otherwise pushes a frame.
    std::optional<cost_type> _start(const std::size_t v, const std::size_t w);

    std::vector<_frame> _stack;
  };
}

selkow_context::selkow_context(const tree &f_, const tree &g_,
			       const tree_edit_cost &cost_)
  : f(f_), g(g_), cost(cost_),
    subtree_del(subtree_costs(f, [&](const std::size_t pre) {
				       return cost.cost_del(pre);
				     })),
    subtree_insert(subtree_costs(g, [&](const std::size_t pre) {
					  return cost.cost_insert(pre);
					}))
{}

std::optional<cost_type>
selkow_context::_start(const std::size_t v, const std::size_t w)
{
  const cost_type match = cost.cost_match(f.post_to_pre(v), g.post_to_pre(w));
  if (match == INFINITE_COST)
    return INFINITE_COST;

  std::vector<std::size_t> f_children = get_children(f, v);
  std::vector<std::size_t> g_children = get_children(g, w);
  if (f_children.empty() || g_children.empty()) {
    // Nothing to align, all of the other node's children's subtrees
    // get deleted or inserted.
    cost_type d = match;
    for (const std::size_t c : f_children)
      d = cost_add(d, subtree_del[c]);
    for (const std::size_t c : g_children)
      d = cost_add(d, subtree_insert[c]);
    return d;
  }

  // Initializing the stepper only inserts the second node's children.
  const std::vector<cost_type> no_distances;
  const children_align_cost row0_cost{f_children, g_children, subtree_del,
				      subtree_insert, no_distances};
  klp::sed::string_edit_distance_stepper stepper{g_children.size(),
						 row0_cost};
  std::vector<cost_type> row;
  row.reserve(g_children.size());
  _stack.push_back(_frame{match, std::move(f_children), std::move(g_children),
			  0, std::move(row), std::move(stepper)});
  return std::nullopt;
}

cost_type selkow_context::top_down_distance(const std::size_t v,
					    const std::size_t w)
{
  // The recursion with an explicit stack. Each pair of nodes gets
  // evaluated at most once, because it's only ever needed by its
  // parents' pair, and the stack holds O(deg(v) + deg(w)) per pair on
  // the path from (v, w) down to the current one.
  assert(_stack.empty());
  std::optional<cost_type> d = _start(v, w);
  while (!_stack.empty()) {
    _frame &fr = _stack.back();
    if (d) {
      // A child pair's distance has become available.
      fr.row.push_back(*d);
      d.reset();
    }

    if (fr.row.size() < fr.g_children.size()) {
      // Might push another frame and invalidate fr.
      d = _start(fr.f_children[fr.i], fr.g_children[fr.row.size()]);
      continue;
    }

    // The current row is complete, feed it into the children
    // sequences' string edit distance computation.
    const children_align_cost row_cost{fr.f_children, fr.g_children,
				       subtree_del, subtree_insert, fr.row,
				       fr.i};
    fr.stepper.step(row_cost);
    fr.row.clear();
    if (++fr.i < fr.f_children.size())
      continue;

    d = cost_add(fr.match, fr.stepper.get_distance());
    _stack.pop_back();
  }

  return *d;
}


cost_type klp::ted::selkow_tree_edit_distance(const tree &f, const tree &g,
					      const tree_edit_cost &cost)
{
  selkow_context ctx{f, g, cost};
  if (!f.size() || !g.size()) {
    if (f.size())
      return ctx.subtree_del.back();
    else if (g.size())
      return ctx.subtree_insert.back();
    return 0;
  }

  return std::min(ctx.top_down_distance(f.size() - 1, g.size() - 1),
		  cost_add(ctx.subtree_del.back(), ctx.subtree_insert.back()));
}

std::vector<std::pair<std::size_t, std::size_t>>
klp::ted::selkow_mapping(const tree &f, const tree &g,
			 const tree_edit_cost &cost)
{
  // Starting with the roots, if matching them is optimal, compute the
  // top-down distances between all pairs of children of each matched
  // pair of nodes, align the children sequences with these as the
  // matching costs, and continue with the matched children pairs.
  // string_align_mapping() never matches children at an infinite cost
  // and is left-leaning.
  std::vector<std::pair<std::size_t, std::size_t>> mapping;
  if (!f.size() || !g.size())
    return mapping;

  selkow_context ctx{f, g, cost};
  std::vector<cost_type> distances;
  // Computes the top-down distances between all pairs of the given
  // children into distances.
  const auto compute_distances =
    [&](const std::vector<std::size_t> &f_children,
	const std::vector<std::size_t> &g_children) {
      distances.resize(checked_mul(f_children.size(), g_children.size()));
      for (std::size_t i = 0; i < f_children.size(); ++i) {
	for (std::size_t j = 0; j < g_children.size(); ++j) {
	  distances[i * g_children.size() + j] =
	    ctx.top_down_distance(f_children[i], g_children[j]);
	}
      }
    };

  // Whether to match the roots at all. The roots' top-down distance
  // is computed from their children pairs' distances, which are
  // needed for aligning their children sequences anyway.
  const std::size_t f_root = f.size() - 1;
  const std::size_t g_root = g.size() - 1;
  const std::vector<std::size_t> f_root_children = get_children(f, f_root);
  const std::vector<std::size_t> g_root_children = get_children(g, g_root);
  cost_type roots_distance = cost.cost_match(f.post_to_pre(f_root),
					     g.post_to_pre(g_root));
  if (roots_distance == INFINITE_COST) {
    return mapping;
  } else if (f_root_children.empty() || g_root_children.empty()) {
    for (const std::size_t c : f_root_children)
      roots_distance = cost_add(roots_distance, ctx.subtree_del[c]);
    for (const std::size_t c : g_root_children)
      roots_distance = cost_add(roots_distance, ctx.subtree_insert[c]);
  } else {
    compute_distances(f_root_children, g_root_children);
    const children_align_cost align_cost{f_root_children, g_root_children,
					 ctx.subtree_del, ctx.subtree_insert,
					 distances};
    roots_distance =
      cost_add(roots_distance,
	       klp::sed::string_edit_distance(f_root_children.size(),
					      g_root_children.size(),
					      align_cost));
  }
  if (roots_distance == INFINITE_COST ||
      roots_distance > cost_add(ctx.subtree_del.back(),
				ctx.subtree_insert.back())) {
    return mapping;
  }

  std::vector<std::pair<std::size_t, std::size_t>> worklist{{f_root, g_root}};
  while (!worklist.empty()) {
    const auto [v, w] = worklist.back();
    worklist.pop_back();
    mapping.emplace_back(f.post_to_pre(v), g.post_to_pre(w));

    const std::vector<std::size_t> f_children = get_children(f, v);
    const std::vector<std::size_t> g_children = get_children(g, w);
    if (f_children.empty() || g_children.empty())
      continue;
    // The roots' children pairs' distances are known already.
    if (v != f_root)
      compute_distances(f_children, g_children);

    const children_align_cost align_cost{f_children, g_children,
					 ctx.subtree_del, ctx.subtree_insert,
					 distances};
    for (const auto &m : klp::sed::string_align_mapping(f_children.size(),
							g_children.size(),
							align_cost)) {
      worklist.emplace_back(f_children[m.first], g_children[m.second]);
    }
  }

  std::sort(mapping.begin(), mapping.end());
  return mapping;
}
