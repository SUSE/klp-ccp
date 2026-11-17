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

#include <dejagnu.h>
#include <algorithm>
#include <iterator>
#include <map>
#include <random>
#include <string>
#include <tuple>
#include <vector>
#include "tree_edit.hh"
#include "selkow.hh"

using namespace klp::ted;

static void check(TestState &s, const std::string &name, const bool cond)
{
  if (cond)
    s.pass(name);
  else
    s.fail(name);
}

static std::vector<std::size_t> all_post_to_pre(const tree &t)
{
  std::vector<std::size_t> r;
  for (std::size_t post = 0; post < t.size(); ++post)
    r.push_back(t.post_to_pre(post));
  return r;
}

static std::vector<std::size_t> all_subtree_sizes(const tree &t)
{
  std::vector<std::size_t> r;
  for (std::size_t post = 0; post < t.size(); ++post)
    r.push_back(t.subtree_size(post));
  return r;
}

static std::vector<std::size_t> all_pre_to_rpost(const tree &t)
{
  std::vector<std::size_t> r;
  for (std::size_t pre = 0; pre < t.size(); ++pre)
    r.push_back(t.pre_to_rpost(pre));
  return r;
}

static std::vector<std::size_t> all_post_to_rpost(const tree &t)
{
  std::vector<std::size_t> r;
  for (std::size_t post = 0; post < t.size(); ++post)
    r.push_back(t.post_to_rpost(post));
  return r;
}

static std::vector<std::size_t> all_rpost_to_pre(const tree &t)
{
  std::vector<std::size_t> r;
  for (std::size_t rpost = 0; rpost < t.size(); ++rpost)
    r.push_back(t.rpost_to_pre(rpost));
  return r;
}

static std::vector<std::size_t> all_pre_to_post(const tree &t)
{
  std::vector<std::size_t> r;
  for (std::size_t pre = 0; pre < t.size(); ++pre)
    r.push_back(t.pre_to_post(pre));
  return r;
}

static std::vector<std::size_t> all_rpost_to_post(const tree &t)
{
  std::vector<std::size_t> r;
  for (std::size_t rpost = 0; rpost < t.size(); ++rpost)
    r.push_back(t.rpost_to_post(rpost));
  return r;
}

static std::vector<std::size_t> all_rightmost_subtree_leaves(const tree &t)
{
  std::vector<std::size_t> r;
  for (std::size_t rpost = 0; rpost < t.size(); ++rpost)
    r.push_back(t.rightmost_subtree_leaf(rpost));
  return r;
}

// Applies f to each DFS post-order index and collects the results.
template <typename result_type, typename f_type>
static std::vector<result_type> map_post(const tree &t, f_type &&f)
{
  std::vector<result_type> r;
  for (std::size_t post = 0; post < t.size(); ++post)
    r.push_back(f(post));
  return r;
}

static void check_children(TestState &s, const std::string &name,
			   const tree &t,
			   const std::vector<bool> &is_leftmost,
			   const std::vector<bool> &is_rightmost,
			   const std::vector<std::size_t> &leftmost,
			   const std::vector<std::size_t> &rightmost)
{
  check(s, name + ": is leftmost child",
	(map_post<bool>(t, [&](std::size_t post)
			    { return t.is_leftmost_child(post); }) ==
	 is_leftmost));
  check(s, name + ": is rightmost child",
	(map_post<bool>(t, [&](std::size_t post)
			    { return t.is_rightmost_child(post); }) ==
	 is_rightmost));
  check(s, name + ": leftmost children",
	(map_post<std::size_t>(t, [&](std::size_t post)
				   { return t.get_leftmost_child(post); }) ==
	 leftmost));
  check(s, name + ": rightmost children",
	(map_post<std::size_t>(t, [&](std::size_t post)
				   { return t.get_rightmost_child(post); }) ==
	 rightmost));
}

static void check_children_iteration
	(TestState &s, const std::string &name, const tree &t,
	 const std::vector<std::vector<std::size_t>> &expected)
{
  std::vector<std::vector<std::size_t>> forward, backward;
  for (std::size_t post = 0; post < t.size(); ++post) {
    const tree::children_range children = t.get_children(post);
    forward.emplace_back(children.begin(), children.end());
    backward.emplace_back(std::make_reverse_iterator(children.end()),
			  std::make_reverse_iterator(children.begin()));
    std::reverse(backward.back().begin(), backward.back().end());
  }
  check(s, name + ": children forward", forward == expected);
  check(s, name + ": children backward", backward == expected);
}

static void test_single_node(TestState &s)
{
  tree_builder tb;
  check(s, "single node: root pre index", tb.push_child() == 0);
  tb.pop_node();
  const tree t = tb.grab_result();
  check(s, "single node: post to pre",
	all_post_to_pre(t) == std::vector<std::size_t>{0});
  check(s, "single node: pre to post",
	all_pre_to_post(t) == std::vector<std::size_t>{0});
  check(s, "single node: rpost to post",
	all_rpost_to_post(t) == std::vector<std::size_t>{0});
  check(s, "single node: subtree sizes",
	all_subtree_sizes(t) == std::vector<std::size_t>{1});
  check(s, "single node: rightmost subtree leaves",
	(all_rightmost_subtree_leaves(t) ==
	 std::vector<std::size_t>{0}));
  check(s, "single node: pre to rpost",
	all_pre_to_rpost(t) == std::vector<std::size_t>{0});
  check(s, "single node: rpost to pre",
	all_rpost_to_pre(t) == std::vector<std::size_t>{0});
  check(s, "single node: post to rpost",
	all_post_to_rpost(t) == std::vector<std::size_t>{0});
  check_children(s, "single node", t, {false}, {false},
		 {INVALID_NODE}, {INVALID_NODE});
  check_children_iteration(s, "single node", t, {{}});
}

static void test_nested(TestState &s)
{
  // DFS pre-order:   a(b(c, d), e(f))
  // DFS post-order:  c, d, b, f, e, a
  // Reversed DFS post-order: f, e, d, c, b, a
  tree_builder tb;
  const std::vector<std::size_t> pre_indices{
    tb.push_child(),	// a
    tb.push_child(),	// b
    tb.push_child(),	// c
  };
  std::vector<std::size_t> more_pre_indices;
  tb.pop_node();
  more_pre_indices.push_back(tb.push_child()); // d
  tb.pop_node();
  tb.pop_node();
  more_pre_indices.push_back(tb.push_child()); // e
  more_pre_indices.push_back(tb.push_child()); // f
  tb.pop_node();
  tb.pop_node();
  tb.pop_node();
  const tree t = tb.grab_result();

  check(s, "nested: pre indices",
	(pre_indices == std::vector<std::size_t>{0, 1, 2} &&
	 more_pre_indices == std::vector<std::size_t>{3, 4, 5}));
  check(s, "nested: size", t.size() == 6);
  check(s, "nested: post to pre",
	all_post_to_pre(t) == std::vector<std::size_t>{2, 3, 1, 5, 4, 0});
  check(s, "nested: pre to post",
	all_pre_to_post(t) == std::vector<std::size_t>{5, 2, 0, 1, 4, 3});
  check(s, "nested: rpost to post",
	all_rpost_to_post(t) == std::vector<std::size_t>{3, 4, 1, 0, 2, 5});
  check(s, "nested: subtree sizes",
	all_subtree_sizes(t) == std::vector<std::size_t>{1, 1, 3, 1, 2, 6});
  check(s, "nested: rightmost subtree leaves",
	(all_rightmost_subtree_leaves(t) ==
	 std::vector<std::size_t>{0, 0, 2, 3, 2, 0}));
  check(s, "nested: pre to rpost",
	all_pre_to_rpost(t) == std::vector<std::size_t>{5, 4, 3, 2, 1, 0});
  check(s, "nested: rpost to pre",
	all_rpost_to_pre(t) == std::vector<std::size_t>{5, 4, 3, 2, 1, 0});
  check(s, "nested: post to rpost",
	all_post_to_rpost(t) == std::vector<std::size_t>{3, 2, 4, 0, 1, 5});
  check_children(s, "nested", t,
		 {true, false, true, true, false, false},
		 {false, true, false, true, true, false},
		 {INVALID_NODE, INVALID_NODE, 0, INVALID_NODE, 3, 2},
		 {INVALID_NODE, INVALID_NODE, 1, INVALID_NODE, 3, 4});
  check_children_iteration(s, "nested", t, {{}, {}, {0, 1}, {}, {3}, {2, 4}});
}

static void test_implicit_pop(TestState &s)
{
  // a(b(c)), with no explicit pops at all.
  tree_builder tb;
  tb.push_child();
  tb.push_child();
  tb.push_child();
  const tree t = tb.grab_result();
  check(s, "implicit pop: post to pre",
	all_post_to_pre(t) == std::vector<std::size_t>{2, 1, 0});
  check(s, "implicit pop: pre to post",
	all_pre_to_post(t) == std::vector<std::size_t>{2, 1, 0});
  check(s, "implicit pop: rpost to post",
	all_rpost_to_post(t) == std::vector<std::size_t>{0, 1, 2});
  check(s, "implicit pop: subtree sizes",
	all_subtree_sizes(t) == std::vector<std::size_t>{1, 2, 3});
  check(s, "implicit pop: rightmost subtree leaves",
	(all_rightmost_subtree_leaves(t) ==
	 std::vector<std::size_t>{0, 0, 0}));
  check(s, "implicit pop: pre to rpost",
	all_pre_to_rpost(t) == std::vector<std::size_t>{2, 1, 0});
  check(s, "implicit pop: rpost to pre",
	all_rpost_to_pre(t) == std::vector<std::size_t>{2, 1, 0});
  check(s, "implicit pop: post to rpost",
	all_post_to_rpost(t) == std::vector<std::size_t>{0, 1, 2});
  check_children(s, "implicit pop", t,
		 {true, true, false}, {true, true, false},
		 {INVALID_NODE, 0, 1}, {INVALID_NODE, 0, 1});
  check_children_iteration(s, "implicit pop", t, {{}, {0}, {1}});
}

static void test_wide(TestState &s)
{
  // DFS pre-order:   a(b, c(d, e), f, g(h))
  // DFS post-order:  b, d, e, c, f, h, g, a
  tree_builder tb;
  tb.push_child();			// a
  tb.push_child(); tb.pop_node();	// b
  tb.push_child();			// c
  tb.push_child(); tb.pop_node();	// d
  tb.push_child(); tb.pop_node();	// e
  tb.pop_node();
  tb.push_child(); tb.pop_node();	// f
  tb.push_child();			// g
  tb.push_child();			// h
  const tree t = tb.grab_result();

  check_children_iteration(s, "wide", t,
			   {{}, {}, {}, {1, 2}, {}, {}, {5}, {0, 3, 4, 6}});
}

static void test_cost_add(TestState &s)
{
  check(s, "cost add: plain", cost_add(2, 3) == 5);
  check(s, "cost add: up to infinite",
	cost_add(INFINITE_COST - 1, 1) == INFINITE_COST);
  check(s, "cost add: overflow",
	cost_add(INFINITE_COST - 1, 2) == INFINITE_COST);
  check(s, "cost add: infinite lhs",
	cost_add(INFINITE_COST, 0) == INFINITE_COST);
  check(s, "cost add: infinite rhs",
	cost_add(1, INFINITE_COST) == INFINITE_COST);
  check(s, "cost add: both infinite",
	cost_add(INFINITE_COST, INFINITE_COST) == INFINITE_COST);
}

// A tree with labels, given in DFS pre-order.
struct labeled_tree
{
  tree t;
  std::string labels;
};

// Parses a tree written as e.g. "{a{b}{c{d}}}": every node is an opening
// brace, followed by its label character, its children and a closing
// brace.
static labeled_tree parse_tree(const std::string &s)
{
  tree_builder tb;
  std::string labels;
  for (std::size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '{') {
      tb.push_child();
      labels.push_back(s[++i]);
    } else if (s[i] == '}') {
      tb.pop_node();
    }
  }
  return labeled_tree{tb.grab_result(), labels};
}

// Costs given by tables, indexed by DFS pre-order.
class table_cost : public tree_edit_cost
{
public:
  table_cost(std::vector<std::vector<cost_type>> &&match,
	     std::vector<cost_type> &&del, std::vector<cost_type> &&insert)
    : _match(std::move(match)), _del(std::move(del)),
      _insert(std::move(insert))
  {}

  cost_type cost_match(const std::size_t n0,
		       const std::size_t n1) const override
  { return _match[n0][n1]; }

  cost_type cost_del(const std::size_t n0) const override
  { return _del[n0]; }

  cost_type cost_insert(const std::size_t n1) const override
  { return _insert[n1]; }

private:
  std::vector<std::vector<cost_type>> _match;
  std::vector<cost_type> _del;
  std::vector<cost_type> _insert;
};

// Unit costs: matching nodes with equal labels is free, everything
// else costs one.
static table_cost unit_cost(const labeled_tree &f, const labeled_tree &g)
{
  std::vector<std::vector<cost_type>> match(f.labels.size());
  for (std::size_t n0 = 0; n0 < f.labels.size(); ++n0) {
    for (std::size_t n1 = 0; n1 < g.labels.size(); ++n1)
      match[n0].push_back(f.labels[n0] == g.labels[n1] ? 0 : 1);
  }
  return table_cost{std::move(match),
		    std::vector<cost_type>(f.labels.size(), 1),
		    std::vector<cost_type>(g.labels.size(), 1)};
}

// Builds a random tree with n nodes, each with at most max_degree
// children, unless there's no other choice.
static tree random_tree(std::mt19937 &rng, const std::size_t n,
			const std::size_t max_degree)
{
  std::vector<std::vector<std::size_t>> children(n);
  for (std::size_t i = 1; i < n; ++i) {
    std::vector<std::size_t> candidates;
    for (std::size_t j = 0; j < i; ++j) {
      if (children[j].size() < max_degree)
	candidates.push_back(j);
    }
    if (candidates.empty()) {
      for (std::size_t j = 0; j < i; ++j)
	candidates.push_back(j);
    }
    children[candidates[rng() % candidates.size()]].push_back(i);
  }

  // Node i's children all have larger indices, emit in DFS pre-order.
  tree_builder tb;
  std::vector<std::pair<std::size_t, std::size_t>> stack{{0, 0}};
  tb.push_child();
  while (!stack.empty()) {
    auto &top = stack.back();
    if (top.second == children[top.first].size()) {
      tb.pop_node();
      stack.pop_back();
      continue;
    }
    const std::size_t child = children[top.first][top.second++];
    tb.push_child();
    stack.emplace_back(child, 0);
  }
  return tb.grab_result();
}

// Brute force Selkow distance: for each pair of matched nodes, try
// all order preserving partial matchings between their children
// explicitly, rather than aligning them by dynamic programming. The
// subtrees' distances get memoized.
class brute_force_selkow
{
public:
  brute_force_selkow(const tree &f, const tree &g, const tree_edit_cost &cost)
    : _f(f), _g(g), _cost(cost)
  {}

  cost_type distance()
  {
    if (!_f.size() && !_g.size())
      return 0;
    cost_type all_del_ins = 0;
    if (_f.size())
      all_del_ins = cost_add(all_del_ins, subtree_del(_f.size() - 1));
    if (_g.size())
      all_del_ins = cost_add(all_del_ins, subtree_insert(_g.size() - 1));
    if (!_f.size() || !_g.size())
      return all_del_ins;
    return std::min(top_down(_f.size() - 1, _g.size() - 1), all_del_ins);
  }

private:
  cost_type subtree_del(const std::size_t v) const
  {
    cost_type c = _cost.cost_del(_f.post_to_pre(v));
    for (const std::size_t child : _f.get_children(v))
      c = cost_add(c, subtree_del(child));
    return c;
  }

  cost_type subtree_insert(const std::size_t w) const
  {
    cost_type c = _cost.cost_insert(_g.post_to_pre(w));
    for (const std::size_t child : _g.get_children(w))
      c = cost_add(c, subtree_insert(child));
    return c;
  }

  // The distance between the subtrees rooted at v and w, matched.
  cost_type top_down(const std::size_t v, const std::size_t w)
  {
    const auto it = _memo.find({v, w});
    if (it != _memo.end())
      return it->second;
    const auto fc = _f.get_children(v), gc = _g.get_children(w);
    const std::vector<std::size_t> f_children(fc.begin(), fc.end());
    const std::vector<std::size_t> g_children(gc.begin(), gc.end());
    const cost_type d = cost_add(_cost.cost_match(_f.post_to_pre(v),
						  _g.post_to_pre(w)),
				 best_matching(f_children, 0,
					       g_children, 0));
    _memo[{v, w}] = d;
    return d;
  }

  // The cheapest order preserving partial matching between the
  // children f_children[i...] and g_children[j...].
  cost_type best_matching(const std::vector<std::size_t> &f_children,
			  const std::size_t i,
			  const std::vector<std::size_t> &g_children,
			  const std::size_t j)
  {
    if (i == f_children.size()) {
      cost_type c = 0;
      for (std::size_t k = j; k < g_children.size(); ++k)
	c = cost_add(c, subtree_insert(g_children[k]));
      return c;
    }

    // Either f_children[i] remains unmatched, or it gets matched to
    // g_children[k], with those in between remaining unmatched.
    cost_type best = cost_add(subtree_del(f_children[i]),
			      best_matching(f_children, i + 1, g_children, j));
    cost_type skipped = 0;
    for (std::size_t k = j; k < g_children.size(); ++k) {
      best = std::min(best,
		      cost_add(cost_add(skipped,
					top_down(f_children[i], g_children[k])),
			       best_matching(f_children, i + 1,
					     g_children, k + 1)));
      skipped = cost_add(skipped, subtree_insert(g_children[k]));
    }
    return best;
  }

  const tree &_f;
  const tree &_g;
  const tree_edit_cost &_cost;
  std::map<std::pair<std::size_t, std::size_t>, cost_type> _memo;
};

// Indexed by DFS pre-order: each node's parent's DFS pre-order index,
// INVALID_NODE for the root.
static std::vector<std::size_t> pre_parents(const tree &t)
{
  std::vector<std::size_t> r(t.size(), INVALID_NODE);
  for (std::size_t post = 0; post < t.size(); ++post) {
    for (const std::size_t child : t.get_children(post))
      r[t.post_to_pre(child)] = t.post_to_pre(post);
  }
  return r;
}

// Checks that the Selkow mapping is valid, i.e. one-to-one and
// preserving both, the ancestor relation and the sibling order, which
// is the case if and only if it preserves DFS pre-order and DFS
// post-order, that it doesn't match any nodes with an infinite
// matching cost, that it's top-down, i.e. that the parents of matched
// non-root nodes are matched to each other, and that its cost equals
// the distance.
static bool check_selkow_mapping(const tree &f, const tree &g,
				 const tree_edit_cost &cost)
{
  const cost_type distance = selkow_tree_edit_distance(f, g, cost);
  const auto mapping = selkow_mapping(f, g, cost);

  if (!std::is_sorted(mapping.begin(), mapping.end()))
    return false;
  std::vector<std::size_t> f_mapped_to(f.size(), INVALID_NODE);
  std::vector<bool> g_mapped(g.size());
  cost_type c = 0;
  for (const auto &m : mapping) {
    if (m.first >= f.size() || m.second >= g.size() ||
	f_mapped_to[m.first] != INVALID_NODE || g_mapped[m.second] ||
	cost.cost_match(m.first, m.second) == INFINITE_COST)
      return false;
    f_mapped_to[m.first] = m.second;
    g_mapped[m.second] = true;
    c = cost_add(c, cost.cost_match(m.first, m.second));
  }
  for (std::size_t n0 = 0; n0 < f.size(); ++n0) {
    if (f_mapped_to[n0] == INVALID_NODE)
      c = cost_add(c, cost.cost_del(n0));
  }
  for (std::size_t n1 = 0; n1 < g.size(); ++n1) {
    if (!g_mapped[n1])
      c = cost_add(c, cost.cost_insert(n1));
  }
  if (c != distance)
    return false;

  for (const auto &a : mapping) {
    for (const auto &b : mapping) {
      if ((a.first < b.first) != (a.second < b.second))
	return false;
      if ((f.pre_to_post(a.first) < f.pre_to_post(b.first)) !=
	  (g.pre_to_post(a.second) < g.pre_to_post(b.second)))
	return false;
    }
  }

  const std::vector<std::size_t> f_parents = pre_parents(f);
  const std::vector<std::size_t> g_parents = pre_parents(g);
  for (const auto &m : mapping) {
    const std::size_t fp = f_parents[m.first], gp = g_parents[m.second];
    if ((fp == INVALID_NODE) != (gp == INVALID_NODE))
      return false;
    if (fp != INVALID_NODE && f_mapped_to[fp] != gp)
      return false;
  }
  return true;
}

static void test_selkow_known(TestState &s)
{
  typedef std::vector<std::pair<std::size_t, std::size_t>> mapping_type;

  // Zhang and Shasha's example: the general tree edit distance moves b
  // up by deleting c and inserting another c above d, which a top-down
  // mapping can't do. Instead, d gets relabeled to c, its child a
  // deleted, and its child c relabeled to d, getting the a inserted
  // below.
  {
    const labeled_tree f = parse_tree("{f{d{a}{c{b}}}{e}}");
    const labeled_tree g = parse_tree("{f{c{d{a}{b}}}{e}}");
    const table_cost cost = unit_cost(f, g);
    const cost_type r_distance = selkow_tree_edit_distance(f.t, g.t, cost);
    const auto r_mapping = selkow_mapping(f.t, g.t, cost);
    check(s, "selkow: Zhang and Shasha's example", r_distance == 4);
    check(s, "selkow: Zhang and Shasha's example, mapping",
	  r_mapping ==
	  mapping_type{{0, 0}, {1, 1}, {3, 2}, {4, 4}, {5, 5}});
    check(s, "selkow: Zhang and Shasha's example, mapping valid",
	  check_selkow_mapping(f.t, g.t, cost));
  }

  // Inserting an inner node costs inserting all of its subtree.
  {
    const labeled_tree f = parse_tree("{r{a}{b}}");
    const labeled_tree g = parse_tree("{r{x{a}{b}}}");
    const table_cost cost = unit_cost(f, g);
    const cost_type r_distance = selkow_tree_edit_distance(f.t, g.t, cost);
    const auto r_mapping = selkow_mapping(f.t, g.t, cost);
    check(s, "selkow: inner node inserted", r_distance == 4);
    check(s, "selkow: inner node inserted, mapping",
	  r_mapping == mapping_type{{0, 0}, {1, 1}});
  }

  // Identical trees.
  {
    const labeled_tree h = parse_tree("{a{b}{c{d}{e}}{f}}");
    const table_cost cost = unit_cost(h, h);
    const cost_type r_distance = selkow_tree_edit_distance(h.t, h.t, cost);
    const auto r_mapping = selkow_mapping(h.t, h.t, cost);
    check(s, "selkow: identical trees",
	  (r_distance == 0 &&
	   r_mapping ==
	   mapping_type{{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}}));
  }

  // Among equally good mappings, the left-leaning one gets chosen.
  const auto check_left_leaning =
    [&](const std::string &name, const std::string &f_str,
	const std::string &g_str, const mapping_type &expected) {
      const labeled_tree f = parse_tree(f_str);
      const labeled_tree g = parse_tree(g_str);
      check(s, "selkow: left-leaning, " + name,
	    selkow_mapping(f.t, g.t, unit_cost(f, g)) == expected);
    };
  check_left_leaning("one of two", "{r{a}}", "{r{a}{a}}", {{0, 0}, {1, 1}});
  check_left_leaning("two to one", "{r{a}{a}}", "{r{a}}", {{0, 0}, {1, 1}});
  check_left_leaning("before sibling", "{r{a}{b}}", "{r{a}{a}{b}}",
		     {{0, 0}, {1, 1}, {2, 3}});
  check_left_leaning("subtree", "{r{x{a}}}", "{r{x{a}}{x{a}}}",
		     {{0, 0}, {1, 1}, {2, 2}});
  check_left_leaning("nested", "{r{a{c}}}", "{r{a}{a{c}{c}}}",
		     {{0, 0}, {1, 2}, {2, 3}});
}

static void test_selkow_special(TestState &s)
{
  const labeled_tree empty = parse_tree("");
  const labeled_tree f = parse_tree("{a{b}{c}}");
  {
    const table_cost cost{
      std::vector<std::vector<cost_type>>(3, std::vector<cost_type>(3, 0)),
      {1, 2, 4}, {8, 16, 32}};
    const cost_type r0_distance = selkow_tree_edit_distance(empty.t, f.t, cost);
    const auto r0_mapping = selkow_mapping(empty.t, f.t, cost);
    check(s, "selkow: empty first tree",
	  r0_distance == 56 && r0_mapping.empty());
    const cost_type r1_distance = selkow_tree_edit_distance(f.t, empty.t, cost);
    const auto r1_mapping = selkow_mapping(f.t, empty.t, cost);
    check(s, "selkow: empty second tree",
	  r1_distance == 7 && r1_mapping.empty());
    const cost_type r2_distance = selkow_tree_edit_distance(empty.t, empty.t, cost);
    const auto r2_mapping = selkow_mapping(empty.t, empty.t, cost);
    check(s, "selkow: both trees empty",
	  r2_distance == 0 && r2_mapping.empty());
  }

  // The roots can't be matched, everything gets deleted and inserted.
  {
    const table_cost cost{
      {{INFINITE_COST, 0, 0}, {0, 0, 0}, {0, 0, 0}},
      {1, 2, 4}, {8, 16, 32}};
    const cost_type r_distance = selkow_tree_edit_distance(f.t, f.t, cost);
    const auto r_mapping = selkow_mapping(f.t, f.t, cost);
    check(s, "selkow: roots not matchable",
	  r_distance == 63 && r_mapping.empty());
  }

  // Matching the roots is possible, but too expensive.
  {
    const table_cost cost{
      {{100, 0, 0}, {0, 0, 0}, {0, 0, 0}},
      {1, 2, 4}, {8, 16, 32}};
    const cost_type r_distance = selkow_tree_edit_distance(f.t, f.t, cost);
    const auto r_mapping = selkow_mapping(f.t, f.t, cost);
    check(s, "selkow: roots too expensive",
	  r_distance == 63 && r_mapping.empty());
  }

  // Equal costs for matching the roots and deleting and inserting
  // everything: matches are preferred.
  {
    const table_cost cost{
      {{63, 0, 0}, {0, 0, 0}, {0, 0, 0}},
      {1, 2, 4}, {8, 16, 32}};
    const cost_type r_distance = selkow_tree_edit_distance(f.t, f.t, cost);
    const auto r_mapping = selkow_mapping(f.t, f.t, cost);
    check(s, "selkow: roots tie",
	  (r_distance == 63 &&
	   r_mapping ==
	   std::vector<std::pair<std::size_t, std::size_t>>{
	     {0, 0}, {1, 1}, {2, 2}}));
  }

  // Everything infinite but matching the a's at the roots: the
  // distance is infinite, and the b's must not get matched.
  {
    const labeled_tree h = parse_tree("{a{b}}");
    const table_cost cost{
      {{0, INFINITE_COST}, {INFINITE_COST, INFINITE_COST}},
      {INFINITE_COST, INFINITE_COST}, {INFINITE_COST, INFINITE_COST}};
    const cost_type r_distance = selkow_tree_edit_distance(h.t, h.t, cost);
    check(s, "selkow: infinite distance",
	  r_distance == INFINITE_COST);
    check(s, "selkow: infinite distance, no infinite matches",
	  check_selkow_mapping(h.t, h.t, cost));
  }
}

static void test_selkow_random(TestState &s, const bool infinite_del_ins)
{
  // Random trees of various shapes and random costs, compared against
  // the brute force Selkow distance. The seed is fixed, for
  // reproducibility.
  std::mt19937 rng(infinite_del_ins ? 4 : 3);
  const std::string name = (infinite_del_ins
			    ? "selkow: random, infinite del/ins "
			    : "selkow: random ");
  for (int round = 0; round < 300; ++round) {
    const tree f = random_tree(rng, 1 + rng() % 12, 1 + rng() % 4);
    const tree g = random_tree(rng, 1 + rng() % 12, 1 + rng() % 4);

    // Few distinct costs, for lots of ties.
    std::vector<std::vector<cost_type>> match(f.size());
    for (auto &row : match) {
      for (std::size_t n1 = 0; n1 < g.size(); ++n1)
	row.push_back(rng() % 8 == 0 ? INFINITE_COST : rng() % 3);
    }
    std::vector<cost_type> del(f.size()), insert(g.size());
    for (auto &c : del)
      c = infinite_del_ins && rng() % 6 == 0 ? INFINITE_COST : 1 + rng() % 2;
    for (auto &c : insert)
      c = infinite_del_ins && rng() % 6 == 0 ? INFINITE_COST : 1 + rng() % 2;
    const table_cost cost{std::move(match), std::move(del), std::move(insert)};

    const cost_type d = selkow_tree_edit_distance(f, g, cost);
    check(s, name + std::to_string(round),
	  d == brute_force_selkow{f, g, cost}.distance());
    check(s, name + std::to_string(round) + ", mapping",
	  check_selkow_mapping(f, g, cost));
  }
}

int main()
{
  TestState s;

  test_single_node(s);
  test_nested(s);
  test_implicit_pop(s);
  test_wide(s);
  test_cost_add(s);
  test_selkow_known(s);
  test_selkow_special(s);
  test_selkow_random(s, false);
  test_selkow_random(s, true);

  return 0;
}
