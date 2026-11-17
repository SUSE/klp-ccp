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

#include <cassert>
#include <utility>
#include "tree_edit.hh"

using namespace klp::ted;

std::size_t tree_builder::push_child()
{
  // Only a single root is allowed.
  assert(!_open_leftmost_subtree_leaves.empty() || _t._pre_to_post.empty());
  _open_leftmost_subtree_leaves.push_back(_t._post_to_pre.size());
  // The node's DFS post-order index gets filled in once it's popped.
  const std::size_t pre = _t._pre_to_post.size();
  _t._pre_to_post.push_back(0);
  return pre;
}

void tree_builder::pop_node()
{
  assert(!_open_leftmost_subtree_leaves.empty());
  // The node is complete now: it's next in DFS post-order.
  const std::size_t post = _t._post_to_pre.size();
  const std::size_t leftmost_subtree_leaf =
    _open_leftmost_subtree_leaves.back();
  _open_leftmost_subtree_leaves.pop_back();
  const std::size_t depth = _open_leftmost_subtree_leaves.size();
  // The nodes preceding it in DFS pre-order are its ancestors and the
  // nodes to its left, the latter all precede its leftmost leaf
  // descendant in DFS post-order.
  const std::size_t pre = leftmost_subtree_leaf + depth;
  _t._leftmost_subtree_leaf.push_back(leftmost_subtree_leaf);
  _t._post_to_pre.push_back(pre);
  _t._pre_to_post[pre] = post;
}

tree tree_builder::grab_result()
{
  while (!_open_leftmost_subtree_leaves.empty())
    pop_node();
  return std::move(_t);
}
