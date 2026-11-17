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

#ifndef TREE_EDIT_TREE_EDIT_HH
#define TREE_EDIT_TREE_EDIT_HH

#include <cstddef>
#include <iterator>
#include <limits>
#include <vector>

namespace klp::ted
{
  // Returned in place of a node index if there's no such node.
  constexpr std::size_t INVALID_NODE = std::numeric_limits<std::size_t>::max();

  // Edit operation costs.
  typedef std::size_t cost_type;

  // A match with that cost must never be made.
  constexpr cost_type INFINITE_COST = std::numeric_limits<cost_type>::max();

  // Adds costs, saturating at INFINITE_COST.
  constexpr cost_type cost_add(const cost_type a, const cost_type b) noexcept
  {
    return (a > INFINITE_COST - b) ? INFINITE_COST : a + b;
  }

  // The costs of the edit operations transforming the first tree into
  // the second one. The callbacks take the nodes' DFS pre-order indices:
  // n0 in the first tree, n1 in the second one.
  class tree_edit_cost
  {
  public:
    virtual ~tree_edit_cost() noexcept = default;

    // The cost of matching, i.e. relabeling n0 to n1. INFINITE_COST if
    // they must never be matched.
    virtual cost_type cost_match(const std::size_t n0,
				 const std::size_t n1) const = 0;

    // The cost of deleting n0 from the first tree.
    virtual cost_type cost_del(const std::size_t n0) const = 0;

    // The cost of inserting n1 from the second tree.
    virtual cost_type cost_insert(const std::size_t n1) const = 0;
  };

  class tree_builder;

  // Externally, nodes are always indexed in DFS pre-order. Internally,
  // DFS post-order indexing is used mostly.
  class tree
  {
  public:
    std::size_t size() const noexcept
    { return _pre_to_post.size(); }

    // Takes a DFS post-order index, returns the node's DFS pre-order
    // index.
    std::size_t post_to_pre(const std::size_t post) const noexcept
    { return _post_to_pre[post]; }

    // Takes a DFS pre-order index, returns the node's DFS post-order
    // index.
    std::size_t pre_to_post(const std::size_t pre) const noexcept
    { return _pre_to_post[pre]; }

    // Takes a DFS pre-order index, returns the node's reversed DFS
    // post-order index, i.e. in DFS post-order with children visited
    // from right to left. That's DFS pre-order read backwards.
    std::size_t pre_to_rpost(const std::size_t pre) const noexcept
    { return size() - 1 - pre; }

    // Takes a reversed DFS post-order index, returns the node's DFS
    // pre-order index.
    std::size_t rpost_to_pre(const std::size_t rpost) const noexcept
    { return size() - 1 - rpost; }

    // Takes a DFS post-order index, returns the node's reversed DFS
    // post-order index.
    std::size_t post_to_rpost(const std::size_t post) const noexcept
    { return pre_to_rpost(post_to_pre(post)); }

    // Takes a reversed DFS post-order index, returns the node's DFS
    // post-order index.
    std::size_t rpost_to_post(const std::size_t rpost) const noexcept
    { return pre_to_post(rpost_to_pre(rpost)); }

    // Takes a DFS post-order index, returns the number of nodes in the
    // subtree rooted at it, including the node itself. A subtree
    // occupies a contiguous DFS post-order range, starting at its
    // leftmost leaf and ending at its root.
    std::size_t subtree_size(const std::size_t post) const noexcept
    { return post - _leftmost_subtree_leaf[post] + 1; }

    // Takes a reversed DFS post-order index, returns the reversed DFS
    // post-order index of the node's rightmost leaf descendant (or of
    // the node itself for leaves). A subtree occupies a contiguous
    // reversed DFS post-order range, starting at its rightmost leaf
    // and ending at its root.
    std::size_t rightmost_subtree_leaf(const std::size_t rpost) const noexcept
    { return rpost - subtree_size(rpost_to_post(rpost)) + 1; }

    // Takes a DFS post-order index, returns whether the node is its
    // parent's leftmost child. False for the root. The node preceding
    // a leftmost child in DFS pre-order is its parent, while for any
    // other node it's a descendant of the left sibling, which precedes
    // the node in DFS post-order.
    bool is_leftmost_child(const std::size_t post) const noexcept
    {
      const std::size_t pre = post_to_pre(post);
      return pre && pre_to_post(pre - 1) > post;
    }

    // Takes a DFS post-order index, returns whether the node is its
    // parent's rightmost child. False for the root. The node following
    // a rightmost child in DFS post-order is its parent, while for any
    // other node it's the right sibling's leftmost leaf descendant,
    // which follows the node in DFS pre-order.
    bool is_rightmost_child(const std::size_t post) const noexcept
    {
      return (post + 1 < size() &&
	      post_to_pre(post + 1) < post_to_pre(post));
    }

    // Takes a DFS post-order index, returns the DFS post-order index
    // of the node's leftmost child, which immediately follows it in DFS
    // pre-order, or INVALID_NODE for leaves.
    std::size_t get_leftmost_child(const std::size_t post) const noexcept
    {
      if (_leftmost_subtree_leaf[post] == post)
	return INVALID_NODE;
      return pre_to_post(post_to_pre(post) + 1);
    }

    // Takes a DFS post-order index, returns the DFS post-order index
    // of the node's rightmost child, which immediately precedes it in
    // DFS post-order, or INVALID_NODE for leaves.
    std::size_t get_rightmost_child(const std::size_t post) const noexcept
    {
      if (_leftmost_subtree_leaf[post] == post)
	return INVALID_NODE;
      return post - 1;
    }

    // Iterates over a node's children from left to right, yielding
    // their DFS post-order indices. Dereferencing returns by value, so
    // that std::reverse_iterator works.
    class child_iterator
    {
    public:
      typedef std::bidirectional_iterator_tag iterator_category;
      typedef std::size_t value_type;
      typedef std::ptrdiff_t difference_type;
      typedef void pointer;
      typedef std::size_t reference;

      child_iterator() noexcept = default;

      reference operator*() const noexcept
      { return _child; }

      child_iterator& operator++() noexcept
      {
	// The rightmost child immediately precedes the parent in DFS
	// post-order. For any other child, the right sibling
	// immediately follows the child's subtree in DFS pre-order.
	if (_child + 1 == _parent) {
	  _child = INVALID_NODE;
	} else {
	  _child = _t->pre_to_post(_t->post_to_pre(_child) +
				   _t->subtree_size(_child));
	}
	return *this;
      }

      child_iterator operator++(int) noexcept
      {
	child_iterator old = *this;
	++*this;
	return old;
      }

      child_iterator& operator--() noexcept
      {
	// The left sibling immediately precedes the child's subtree in
	// DFS post-order.
	if (_child == INVALID_NODE)
	  _child = _parent - 1;
	else
	  _child = _t->_leftmost_subtree_leaf[_child] - 1;
	return *this;
      }

      child_iterator operator--(int) noexcept
      {
	child_iterator old = *this;
	--*this;
	return old;
      }

      bool operator==(const child_iterator &rhs) const noexcept
      { return _child == rhs._child; }

      bool operator!=(const child_iterator &rhs) const noexcept
      { return !(*this == rhs); }

    private:
      friend class tree;

      child_iterator(const tree &t, const std::size_t parent,
		     const std::size_t child) noexcept
	: _t(&t), _parent(parent), _child(child)
      {}

      const tree *_t = nullptr;

      // DFS post-order indices. _child is INVALID_NODE past the end.
      std::size_t _parent = INVALID_NODE;
      std::size_t _child = INVALID_NODE;
    };

    class children_range
    {
    public:
      child_iterator begin() const noexcept
      { return _begin; }

      child_iterator end() const noexcept
      { return _end; }

    private:
      friend class tree;

      children_range(const child_iterator &begin,
		     const child_iterator &end) noexcept
	: _begin(begin), _end(end)
      {}

      child_iterator _begin;
      child_iterator _end;
    };

    // Takes a DFS post-order index, returns the range of the node's
    // children, empty for leaves.
    children_range get_children(const std::size_t post) const noexcept
    {
      return children_range{
	       child_iterator{*this, post, get_leftmost_child(post)},
	       child_iterator{*this, post, INVALID_NODE}
	     };
    }

  private:
    friend class tree_builder;

    tree() = default;

    // Indexed by DFS post-order: each node's DFS pre-order index.
    std::vector<std::size_t> _post_to_pre;

    // Indexed by DFS post-order: the DFS post-order index of each
    // node's leftmost leaf descendant (or the node itself for leaves).
    std::vector<std::size_t> _leftmost_subtree_leaf;

    // Indexed by DFS pre-order: each node's DFS post-order index.
    std::vector<std::size_t> _pre_to_post;
  };

  // Builds up a tree in DFS pre-order. Initially there's no current
  // node and the first push_child() creates the root. Once the root
  // has been popped, no more nodes may be added.
  class tree_builder
  {
  public:
    // Add a child to the current node and make it the current
    // node. Returns the new node's DFS pre-order index.
    std::size_t push_child();

    // Make the current node's parent the current node.
    void pop_node();

    // Implicitly pops all nodes still pushed.
    tree grab_result();

  private:
    tree _t;

    // One entry for each node on the path from the root to the
    // current node, inclusive: the DFS post-order index its
    // leftmost leaf descendant will get, which is the number of nodes
    // completed (popped) at the time the node got pushed. The stack's
    // size minus one is the current node's depth.
    std::vector<std::size_t> _open_leftmost_subtree_leaves;
  };
}

#endif
