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

#ifndef TREE_EDIT_SELKOW_HH
#define TREE_EDIT_SELKOW_HH

#include <cstddef>
#include <utility>
#include <vector>
#include "tree_edit.hh"

// Selkow's top-down tree edit distance, see S. M. Selkow, "The
// tree-to-tree editing problem", Information Processing Letters 6(6),
// 1977. Nodes may get inserted and deleted only as leaves, so deleting
// an inner node amounts to deleting its whole subtree, and likewise
// for insertions. Equivalently, whenever two non-root nodes are
// matched, so are their parents. The distance is an upper bound on
// the general tree edit distance as computed by RTED.
//
// The top-down distance d(v, w) between the subtrees rooted at a pair
// of matched nodes v and w is cost_match(v, w) plus the string edit
// distance between their children sequences, where matching two
// children costs the top-down distance between their subtrees, and
// deleting or inserting a child costs deleting or inserting its whole
// subtree. The distance between the trees is the smaller one of the
// roots' top-down distance and the cost of deleting all of the first
// tree and inserting all of the second one. Pairs of nodes with an
// infinite matching cost don't get descended into.
//
// In what follows, T denotes the number of pairs of children
// considered, i.e. the sum of deg(v) deg(w) over all pairs of nodes
// (v, w) with a finite matching cost whose parents got compared, or
// which are the roots. It's at most |f||g|, but typically much less,
// because only pairs of nodes at the same depth contribute.
namespace klp::ted
{
  // Computes Selkow's top-down edit distance between the trees f and
  // g, with the edit operations' costs taken from cost. Takes O(T)
  // time and O(|f| + |g|) space: the children pairs' distances get
  // computed one row at a time, right before the children sequences'
  // string edit distance computation needs them.
  cost_type selkow_tree_edit_distance(const tree &f, const tree &g,
				      const tree_edit_cost &cost);

  // Computes an optimal top-down mapping between the trees f and g,
  // with the edit operations' costs taken from cost: the pairs of
  // matched nodes, given by their DFS pre-order indices in the first
  // and second tree, sorted by the former. All nodes of the first tree
  // not in the mapping get deleted, all of the second tree's ones not
  // in the mapping get inserted. Optimal mappings aren't unique in
  // general: among equally good choices, matches are preferred over
  // deletions and insertions, and children further to the left over
  // children further to the right, i.e. the mapping is
  // left-leaning. Pairs of nodes whose subtrees' top-down distance is
  // infinite never get matched, in particular not those with an
  // infinite matching cost, not even if the distance is infinite.
  //
  // For each matched pair of nodes, the top-down distances between all
  // pairs of their children get recomputed and kept for aligning the
  // children sequences. With h being the length of the longest chain
  // of matched nodes, this takes O(h T) time, and O(|f| + |g|) space
  // plus deg(v) deg(w) for the largest pair of matched nodes (v, w).
  // Throws std::overflow_error if the latter doesn't fit into a
  // std::size_t.
  std::vector<std::pair<std::size_t, std::size_t>>
  selkow_mapping(const tree &f, const tree &g, const tree_edit_cost &cost);
}

#endif
