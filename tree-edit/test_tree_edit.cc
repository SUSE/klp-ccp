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
#include <string>
#include <vector>
#include "tree_edit.hh"

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

int main()
{
  TestState s;

  test_single_node(s);
  test_nested(s);
  test_implicit_pop(s);
  test_wide(s);
  test_cost_add(s);

  return 0;
}
