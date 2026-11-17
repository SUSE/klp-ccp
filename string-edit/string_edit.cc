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
#include <stdexcept>
#include "string_edit.hh"

using namespace klp::sed;

// Throws std::length_error if a table row for a string of length n
// can't be indexed by a std::size_t.
static std::size_t row_size(const std::size_t n)
{
  if (n == std::numeric_limits<std::size_t>::max())
    throw std::length_error("string length too large");
  return n + 1;
}

string_edit_distance_stepper::
string_edit_distance_stepper(const std::size_t n1, const string_edit_cost &cost)
  : _i{0}, _row(row_size(n1))
{
  // The classic Wagner-Fischer dynamic programming scheme, keeping
  // only a single row of the table. Row 0: insert the second string's
  // prefixes.
  _row[0] = 0;
  for (std::size_t j = 1; j <= n1; ++j)
    _row[j] = cost_add(_row[j - 1], cost.cost_insert(j - 1));
}

void string_edit_distance_stepper::step(const string_edit_cost &cost)
{
  // Before it gets overwritten, _row[j] still holds the previous row's
  // value, which gets remembered in diag for the next column's match
  // option.
  const std::size_t n1 = _row.size() - 1;
  const cost_type del = cost.cost_del(_i);
  cost_type diag = _row[0];
  _row[0] = cost_add(_row[0], del);
  for (std::size_t j = 1; j <= n1; ++j) {
    const cost_type prev_row = _row[j];
    _row[j] = std::min({cost_add(prev_row, del),
			cost_add(_row[j - 1], cost.cost_insert(j - 1)),
			cost_add(diag, cost.cost_match(_i, j - 1))});
    diag = prev_row;
  }
  ++_i;
}

cost_type klp::sed::string_edit_distance(const std::size_t n0,
					const std::size_t n1,
					const string_edit_cost &cost)
{
  string_edit_distance_stepper stepper{n1, cost};
  for (std::size_t i = 0; i < n0; ++i)
    stepper.step(cost);
  return stepper.get_distance();
}


std::vector<std::pair<std::size_t, std::size_t>>
klp::sed::string_align_mapping(const std::size_t n0, const std::size_t n1,
			       const string_edit_cost &cost)
{
  // Think of the edit graph: a grid of vertices (i, j) with
  // 0 <= i <= n0 and 0 <= j <= n1, with edges from (i, j) to
  // (i + 1, j + 1) for matching i with j, to (i + 1, j) for deleting i
  // and to (i, j + 1) for inserting j. An optimal mapping corresponds
  // to a cheapest path from (0, 0) to (n0, n1). Let S(i, j) be the
  // cost of a cheapest path from (i, j) to the end, i.e. the distance
  // between the suffixes. The mapping returned is the one from walking
  // greedily from (0, 0) on: at each vertex take the first optimal
  // edge, trying a match first, then a deletion and an insertion last.
  // That's what makes it left-leaning: matches are made as early as
  // possible.
  //
  // Hirschberg's technique finds the walk in linear space: split the
  // first string at some row mid, find a vertex (mid, j*) on the walk,
  // and recurse into the two parts. The walk's portion after (mid, j*)
  // is the greedy walk for the bottom right subproblem, because S
  // restricted to it is the subproblem's own suffix distance. For the
  // portion before (mid, j*), the subproblem's suffix distances S'
  // towards (mid, j*) differ from S, but at each of the walk's vertices
  // (i, j), S(i, j) = S'(i, j) + S(mid, j*), because both are on an
  // optimal path. Thus any edge optimal with respect to S' is optimal
  // with respect to S as well, and the walk's edge, staying on a path
  // via (mid, j*), is optimal with respect to S'. So the greedy choices
  // agree, and the walk's first portion is the greedy walk for the top
  // left subproblem.
  //
  // What remains is finding a vertex on the walk in row mid. Plain
  // Hirschberg picks any vertex on some optimal path, which doesn't
  // necessarily lie on the greedy walk. Instead, compute S backwards
  // from the subproblem's end up to row a0 and, for each vertex in the
  // rows above mid, record the column where the greedy walk from that
  // vertex first reaches row mid: that's the label of the vertex the
  // greedy edge leads to, with the vertices in row mid labeled by
  // their own column. The label of the subproblem's start vertex is
  // j*. Each subproblem takes time proportional to its area, and the
  // areas halve from one level to the next, so the overall time is
  // O(n0 n1).
  //
  // If the distance is infinite, the argument above breaks down, as
  // all options saturate and compare equal, but the result is still a
  // valid mapping, and thus an optimal one. Characters with an
  // infinite matching cost are checked for explicitly in order to
  // never get matched.
  const std::size_t row_len = row_size(n1);
  std::vector<std::pair<std::size_t, std::size_t>> mapping;
  if (!n0 || !n1)
    return mapping;

  // A subproblem, given by the half-open position ranges [a0, b0) and
  // [a1, b1) in the first and second string respectively.
  struct subproblem
  {
    std::size_t a0;
    std::size_t b0;
    std::size_t a1;
    std::size_t b1;
  };

  // The top left subproblems get processed before the bottom right
  // ones, so the matches get found in increasing order.
  std::vector<subproblem> worklist{subproblem{0, n0, 0, n1}};
  std::vector<cost_type> s_next(row_len);
  std::vector<cost_type> s_cur(row_len);
  std::vector<std::size_t> label_next(row_len);
  std::vector<std::size_t> label_cur(row_len);
  while (!worklist.empty()) {
    const subproblem sp = worklist.back();
    worklist.pop_back();
    if (sp.a0 == sp.b0 || sp.a1 == sp.b1)
      continue;

    // Rows are indexed by the column relative to a1.
    const std::size_t q = sp.b1 - sp.a1;

    // Row b0 of S: insert all of the remaining second string.
    s_next[q] = 0;
    for (std::size_t j = q; j > 0; --j)
      s_next[j - 1] = cost_add(s_next[j], cost.cost_insert(sp.a1 + j - 1));

    // The costs of the three edges leaving (i, a1 + j), with s_next
    // holding row i + 1 of S and s_cur row i from column j + 1 on.
    const auto match =
      [&](const std::size_t i, const std::size_t j) {
	return cost_add(cost.cost_match(i, sp.a1 + j), s_next[j + 1]);
      };
    const auto may_match =
      [&](const std::size_t i, const std::size_t j) {
	return cost.cost_match(i, sp.a1 + j) != INFINITE_COST;
      };
    const auto del =
      [&](const std::size_t i, const std::size_t j) {
	return cost_add(cost.cost_del(i), s_next[j]);
      };
    const auto ins =
      [&](const std::size_t j) {
	return cost_add(cost.cost_insert(sp.a1 + j), s_cur[j + 1]);
      };
    // Computes row i of S into s_cur from row i + 1 in s_next.
    const auto compute_row =
      [&](const std::size_t i) {
	s_cur[q] = del(i, q);
	for (std::size_t j = q; j > 0; --j)
	  s_cur[j - 1] = std::min({match(i, j - 1), del(i, j - 1), ins(j - 1)});
      };

    if (sp.b0 - sp.a0 == 1) {
      // A single row: walk it directly. The walk either matches a0
      // with some position, deletes it or, at the end, has to.
      const std::size_t i = sp.a0;
      compute_row(i);
      for (std::size_t j = 0; j < q; ++j) {
	if (may_match(i, j) && s_cur[j] == match(i, j)) {
	  mapping.emplace_back(i, sp.a1 + j);
	  break;
	} else if (s_cur[j] == del(i, j)) {
	  break;
	}
	assert(s_cur[j] == ins(j));
      }
      continue;
    }

    const std::size_t mid = sp.a0 + (sp.b0 - sp.a0) / 2;
    for (std::size_t i = sp.b0; i > mid; --i) {
      compute_row(i - 1);
      std::swap(s_cur, s_next);
    }

    // s_next holds row mid now. Label its vertices with their columns,
    // then propagate the labels upwards along the greedy edges.
    for (std::size_t j = 0; j <= q; ++j)
      label_next[j] = j;
    for (std::size_t i = mid; i > sp.a0; --i) {
      compute_row(i - 1);
      // An insertion leads to the right neighbour in the same row,
      // whose label is known already when going backwards.
      label_cur[q] = label_next[q];
      for (std::size_t j = q; j > 0; --j) {
	if (may_match(i - 1, j - 1) && s_cur[j - 1] == match(i - 1, j - 1)) {
	  label_cur[j - 1] = label_next[j];
	} else if (s_cur[j - 1] == del(i - 1, j - 1)) {
	  label_cur[j - 1] = label_next[j - 1];
	} else {
	  assert(s_cur[j - 1] == ins(j - 1));
	  label_cur[j - 1] = label_cur[j];
	}
      }
      std::swap(s_cur, s_next);
      std::swap(label_cur, label_next);
    }

    const std::size_t j_star = sp.a1 + label_next[0];
    worklist.push_back(subproblem{mid, sp.b0, j_star, sp.b1});
    worklist.push_back(subproblem{sp.a0, mid, sp.a1, j_star});
  }

  return mapping;
}
