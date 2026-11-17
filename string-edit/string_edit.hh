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

#ifndef STRING_EDIT_STRING_EDIT_HH
#define STRING_EDIT_STRING_EDIT_HH

#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace klp::sed
{
  // Edit operation costs.
  typedef std::size_t cost_type;

  // A match with that cost must never be made.
  constexpr cost_type INFINITE_COST = std::numeric_limits<cost_type>::max();

  // Adds costs, saturating at INFINITE_COST.
  constexpr cost_type cost_add(const cost_type a, const cost_type b) noexcept
  {
    return (a > INFINITE_COST - b) ? INFINITE_COST : a + b;
  }

  // The costs of the edit operations transforming the first string
  // into the second one. The callbacks take the characters' positions:
  // i0 in the first string, i1 in the second one.
  class string_edit_cost
  {
  public:
    virtual ~string_edit_cost() noexcept = default;

    // The cost of matching, i.e. substituting i0 by i1. INFINITE_COST
    // if they must never be matched.
    virtual cost_type cost_match(const std::size_t i0,
				 const std::size_t i1) const = 0;

    // The cost of deleting i0 from the first string.
    virtual cost_type cost_del(const std::size_t i0) const = 0;

    // The cost of inserting i1 from the second string.
    virtual cost_type cost_insert(const std::size_t i1) const = 0;
  };

  // Computes the edit distance between a first string and a second
  // one of fixed length n1 incrementally, one character of the first
  // string at a time, i.e. one row of the dynamic programming table
  // after the other. This allows for providing the matching costs
  // involving a given character of the first string only right before
  // they're needed, and for discarding them afterwards, e.g. when they
  // are expensive to compute and too many to keep around all at once.
  //
  // Initially, no characters of the first string have been processed.
  // Each step() processes the next one, at position i say, and calls
  // the cost's cost_del(i), and cost_match(i, i1) and cost_insert(i1)
  // for all positions i1 in the second string, each exactly once. The
  // cost's callbacks get passed the position i, so a string_edit_cost
  // providing costs only for the current row is fine.
  // get_distance() returns the distance between the processed prefix
  // of the first string and all of the second one.
  //
  // Each step takes O(n1) time, the stepper takes O(n1) space.
  class string_edit_distance_stepper
  {
  public:
    // Throws std::length_error if n1 + 1 doesn't fit into a
    // std::size_t. The cost's cost_insert() gets called for all
    // positions in the second string, each exactly once.
    string_edit_distance_stepper(const std::size_t n1,
				 const string_edit_cost &cost);

    // Processes the next character of the first string, with the
    // edit operations' costs taken from cost.
    void step(const string_edit_cost &cost);

    // The edit distance between the first string's prefix processed so
    // far and the whole second string.
    cost_type get_distance() const noexcept
    { return _row.back(); }

  private:
    // The number of characters of the first string processed so far,
    // i.e. the position of the next one.
    std::size_t _i;

    // _row[j] holds the edit distance between the first string's
    // prefix processed so far and the second string's prefix of
    // length j.
    std::vector<cost_type> _row;
  };

  // Computes the edit distance between a first string of length n0
  // and a second one of length n1, with the edit operations' costs
  // taken from cost. Takes O(n0 n1) time and O(n1) space. Throws
  // std::length_error if n1 + 1 doesn't fit into a std::size_t.
  cost_type string_edit_distance(const std::size_t n0, const std::size_t n1,
				 const string_edit_cost &cost);

  // Computes an optimal mapping between a first string of length n0
  // and a second one of length n1, with the edit operations' costs
  // taken from cost: the pairs of matched positions in the first and
  // second string, in increasing order of both. All characters of the
  // first string not in the mapping get deleted, all of the second
  // string's ones not in the mapping get inserted. Optimal mappings
  // aren't unique in general: among equally good choices, matches are
  // preferred over deletions and deletions over insertions, going from
  // the strings' beginnings towards their ends, i.e. the mapping is
  // left-leaning. Characters with an infinite matching cost never get
  // matched, not even if the distance is infinite. Uses Hirschberg's
  // divide and conquer technique and takes O(n0 n1) time and
  // O(n0 + n1) space. Throws std::length_error if n1 + 1 doesn't fit
  // into a std::size_t.
  std::vector<std::pair<std::size_t, std::size_t>>
  string_align_mapping(const std::size_t n0, const std::size_t n1,
		       const string_edit_cost &cost);
}

#endif
