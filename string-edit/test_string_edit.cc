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
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "string_edit.hh"

using namespace klp::sed;

typedef std::vector<std::pair<std::size_t, std::size_t>> mapping_type;

static void check(TestState &s, const std::string &name, const bool cond)
{
  if (cond)
    s.pass(name);
  else
    s.fail(name);
}

// Unit costs for deletions and insertions, matching equal characters
// is free, unequal ones cost 1, or never match if mismatch_infinite is
// set.
class unit_cost : public string_edit_cost
{
public:
  unit_cost(const std::string &s0, const std::string &s1,
	    const bool mismatch_infinite = false)
    : _s0{s0}, _s1{s1}, _mismatch_infinite{mismatch_infinite}
  {}

  virtual cost_type cost_match(const std::size_t i0,
			       const std::size_t i1) const override
  {
    if (_s0.at(i0) == _s1.at(i1))
      return 0;
    return _mismatch_infinite ? INFINITE_COST : 1;
  }

  virtual cost_type cost_del(const std::size_t i0) const override
  {
    static_cast<void>(_s0.at(i0)); // Bounds check.
    return 1;
  }

  virtual cost_type cost_insert(const std::size_t i1) const override
  {
    static_cast<void>(_s1.at(i1)); // Bounds check.
    return 1;
  }

private:
  const std::string _s0;
  const std::string _s1;
  const bool _mismatch_infinite;
};

// Arbitrary costs from tables.
class table_cost : public string_edit_cost
{
public:
  table_cost(const std::size_t n0, const std::size_t n1)
    : _n1{n1}, _match(n0 * n1), _del(n0), _ins(n1)
  {}

  virtual cost_type cost_match(const std::size_t i0,
			       const std::size_t i1) const override
  {
    return _match.at(i0 * _n1 + i1);
  }

  virtual cost_type cost_del(const std::size_t i0) const override
  {
    return _del.at(i0);
  }

  virtual cost_type cost_insert(const std::size_t i1) const override
  {
    return _ins.at(i1);
  }

  cost_type& match(const std::size_t i0, const std::size_t i1)
  { return _match.at(i0 * _n1 + i1); }

  cost_type& del(const std::size_t i0)
  { return _del.at(i0); }

  cost_type& ins(const std::size_t i1)
  { return _ins.at(i1); }

private:
  std::size_t _n1;
  std::vector<cost_type> _match;
  std::vector<cost_type> _del;
  std::vector<cost_type> _ins;
};

// The reference: the full table of suffix distances S(i, j), and the
// greedy walk from (0, 0) along it, preferring matches over deletions
// over insertions.
class full_table_reference
{
public:
  full_table_reference(const std::size_t n0, const std::size_t n1,
		       const string_edit_cost &cost)
    : _n0{n0}, _n1{n1}, _cost{cost}, _s((n0 + 1) * (n1 + 1))
  {
    for (std::size_t i = n0 + 1; i > 0; --i) {
      for (std::size_t j = n1 + 1; j > 0; --j) {
	const std::size_t ii = i - 1, jj = j - 1;
	if (ii == n0 && jj == n1) {
	  s(ii, jj) = 0;
	  continue;
	}
	cost_type v = INFINITE_COST;
	if (ii < n0 && jj < n1)
	  v = std::min(v, match(ii, jj));
	if (ii < n0)
	  v = std::min(v, del(ii, jj));
	if (jj < n1)
	  v = std::min(v, ins(ii, jj));
	s(ii, jj) = v;
      }
    }
  }

  cost_type distance() const
  { return _s[0]; }

  mapping_type mapping() const
  {
    mapping_type m;
    std::size_t i = 0, j = 0;
    while (i < _n0 || j < _n1) {
      if (i < _n0 && j < _n1 &&
	  _cost.cost_match(i, j) != INFINITE_COST &&
	  s(i, j) == match(i, j)) {
	m.emplace_back(i, j);
	++i;
	++j;
      } else if (i < _n0 && s(i, j) == del(i, j)) {
	++i;
      } else {
	++j;
      }
    }
    return m;
  }

private:
  cost_type& s(const std::size_t i, const std::size_t j)
  { return _s[i * (_n1 + 1) + j]; }

  cost_type s(const std::size_t i, const std::size_t j) const
  { return _s[i * (_n1 + 1) + j]; }

  cost_type match(const std::size_t i, const std::size_t j) const
  { return cost_add(_cost.cost_match(i, j), s(i + 1, j + 1)); }

  cost_type del(const std::size_t i, const std::size_t j) const
  { return cost_add(_cost.cost_del(i), s(i + 1, j)); }

  cost_type ins(const std::size_t i, const std::size_t j) const
  { return cost_add(_cost.cost_insert(j), s(i, j + 1)); }

  std::size_t _n0;
  std::size_t _n1;
  const string_edit_cost &_cost;
  std::vector<cost_type> _s;
};

// Exhaustive recursion over all alignments, for small strings only.
static cost_type brute_force_distance(const std::size_t i,
				      const std::size_t j,
				      const std::size_t n0,
				      const std::size_t n1,
				      const string_edit_cost &cost)
{
  if (i == n0 && j == n1)
    return 0;
  cost_type v = INFINITE_COST;
  if (i < n0 && j < n1) {
    v = std::min(v, cost_add(cost.cost_match(i, j),
			     brute_force_distance(i + 1, j + 1, n0, n1, cost)));
  }
  if (i < n0) {
    v = std::min(v, cost_add(cost.cost_del(i),
			     brute_force_distance(i + 1, j, n0, n1, cost)));
  }
  if (j < n1) {
    v = std::min(v, cost_add(cost.cost_insert(j),
			     brute_force_distance(i, j + 1, n0, n1, cost)));
  }
  return v;
}

// Checks that m is strictly increasing in both components, within
// bounds, never matches at infinite cost, and returns its total cost.
static bool check_mapping(const mapping_type &m,
			  const std::size_t n0, const std::size_t n1,
			  const string_edit_cost &cost,
			  cost_type &total)
{
  total = 0;
  std::vector<bool> matched0(n0), matched1(n1);
  for (std::size_t k = 0; k < m.size(); ++k) {
    if (m[k].first >= n0 || m[k].second >= n1)
      return false;
    if (k && (m[k].first <= m[k - 1].first ||
	      m[k].second <= m[k - 1].second)) {
      return false;
    }
    const cost_type c = cost.cost_match(m[k].first, m[k].second);
    if (c == INFINITE_COST)
      return false;
    total = cost_add(total, c);
    matched0[m[k].first] = true;
    matched1[m[k].second] = true;
  }
  for (std::size_t i = 0; i < n0; ++i) {
    if (!matched0[i])
      total = cost_add(total, cost.cost_del(i));
  }
  for (std::size_t j = 0; j < n1; ++j) {
    if (!matched1[j])
      total = cost_add(total, cost.cost_insert(j));
  }
  return true;
}

static void test_cost_add(TestState &s)
{
  check(s, "cost_add: plain", cost_add(2, 3) == 5);
  check(s, "cost_add: saturating",
	cost_add(INFINITE_COST - 1, 2) == INFINITE_COST);
  check(s, "cost_add: infinite", cost_add(INFINITE_COST, 0) == INFINITE_COST);
}

static void test_known(TestState &s)
{
  {
    const unit_cost cost{"kitten", "sitting"};
    const cost_type d = string_edit_distance(6, 7, cost);
    const mapping_type m = string_align_mapping(6, 7, cost);
    check(s, "known: kitten/sitting distance", d == 3);
    cost_type total;
    check(s, "known: kitten/sitting mapping",
	  check_mapping(m, 6, 7, cost, total) && total == 3 &&
	  m == (mapping_type{{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}}));
  }

  {
    // Pure insertion of a duplicate: the earlier one gets matched.
    const unit_cost cost{"a", "aa"};
    const cost_type d = string_edit_distance(1, 2, cost);
    const mapping_type m = string_align_mapping(1, 2, cost);
    check(s, "known: a/aa distance", d == 1);
    check(s, "known: a/aa left-leaning",
	  m == (mapping_type{{0, 0}}));
  }

  {
    const unit_cost cost{"aa", "a"};
    const cost_type d = string_edit_distance(2, 1, cost);
    const mapping_type m = string_align_mapping(2, 1, cost);
    check(s, "known: aa/a distance", d == 1);
    check(s, "known: aa/a left-leaning",
	  m == (mapping_type{{0, 0}}));
  }

  {
    // Neither smallest nor largest split columns in Hirschberg would
    // get both this and the previous case right.
    const unit_cost cost{"qa", "aa", true};
    const cost_type d = string_edit_distance(2, 2, cost);
    const mapping_type m = string_align_mapping(2, 2, cost);
    check(s, "known: qa/aa distance", d == 2);
    check(s, "known: qa/aa left-leaning",
	  m == (mapping_type{{1, 0}}));
  }

  {
    // A run of equal characters in a longer string.
    const unit_cost cost{"abab", "abxxxxab", true};
    const cost_type d = string_edit_distance(4, 8, cost);
    const mapping_type m = string_align_mapping(4, 8, cost);
    check(s, "known: abab/abxxxxab distance", d == 4);
    check(s, "known: abab/abxxxxab left-leaning",
	  m ==
	  (mapping_type{{0, 0}, {1, 1}, {2, 6}, {3, 7}}));
  }

  {
    const unit_cost cost{"aaaa", "aa"};
    const cost_type d = string_edit_distance(4, 2, cost);
    const mapping_type m = string_align_mapping(4, 2, cost);
    check(s, "known: aaaa/aa distance", d == 2);
    check(s, "known: aaaa/aa left-leaning",
	  m == (mapping_type{{0, 0}, {1, 1}}));
  }

  {
    const unit_cost cost{"aa", "aaaa"};
    const cost_type d = string_edit_distance(2, 4, cost);
    const mapping_type m = string_align_mapping(2, 4, cost);
    check(s, "known: aa/aaaa distance", d == 2);
    check(s, "known: aa/aaaa left-leaning",
	  m == (mapping_type{{0, 0}, {1, 1}}));
  }
}

static void test_empty(TestState &s)
{
  {
    const unit_cost cost{"", ""};
    const cost_type d = string_edit_distance(0, 0, cost);
    const mapping_type m = string_align_mapping(0, 0, cost);
    check(s, "empty: both", d == 0 && m.empty());
  }

  {
    const unit_cost cost{"abc", ""};
    const cost_type d = string_edit_distance(3, 0, cost);
    const mapping_type m = string_align_mapping(3, 0, cost);
    check(s, "empty: second", d == 3 && m.empty());
  }

  {
    const unit_cost cost{"", "abcd"};
    const cost_type d = string_edit_distance(0, 4, cost);
    const mapping_type m = string_align_mapping(0, 4, cost);
    check(s, "empty: first", d == 4 && m.empty());
  }

  {
    const unit_cost cost{"", ""};
    bool thrown = false;
    try {
      string_edit_distance(0, std::numeric_limits<std::size_t>::max(), cost);
    } catch (const std::length_error&) {
      thrown = true;
    }
    check(s, "empty: distance length_error", thrown);
    thrown = false;
    try {
      string_align_mapping(0, std::numeric_limits<std::size_t>::max(), cost);
    } catch (const std::length_error&) {
      thrown = true;
    }
    check(s, "empty: mapping length_error", thrown);
  }
}

static void test_infinite(TestState &s)
{
  {
    // Nothing may match.
    const unit_cost cost{"abc", "xyz", true};
    const cost_type d = string_edit_distance(3, 3, cost);
    const mapping_type m = string_align_mapping(3, 3, cost);
    check(s, "infinite: no matches possible",
	  d == 6 && m.empty());
  }

  {
    // Infinite distance: deletions and insertions are impossible and
    // a mismatch is in the way. The possible matches still must not
    // include any infinite cost one.
    table_cost cost{3, 3};
    for (std::size_t i = 0; i < 3; ++i) {
      cost.del(i) = INFINITE_COST;
      cost.ins(i) = INFINITE_COST;
      for (std::size_t j = 0; j < 3; ++j)
	cost.match(i, j) = (i == j && i != 1) ? 0 : INFINITE_COST;
    }
    const cost_type d = string_edit_distance(3, 3, cost);
    const mapping_type m = string_align_mapping(3, 3, cost);
    check(s, "infinite: distance", d == INFINITE_COST);
    cost_type total;
    check(s, "infinite: mapping valid",
	  check_mapping(m, 3, 3, cost, total) && total == INFINITE_COST);
  }

  {
    // Infinite deletions force all of the first string to be matched.
    table_cost cost{2, 4};
    for (std::size_t i = 0; i < 2; ++i) {
      cost.del(i) = INFINITE_COST;
      for (std::size_t j = 0; j < 4; ++j)
	cost.match(i, j) = 5;
    }
    for (std::size_t j = 0; j < 4; ++j)
      cost.ins(j) = 1;
    const cost_type d = string_edit_distance(2, 4, cost);
    const mapping_type m = string_align_mapping(2, 4, cost);
    check(s, "infinite: forced matches distance", d == 12);
    check(s, "infinite: forced matches left-leaning",
	  m == (mapping_type{{0, 0}, {1, 1}}));
  }
}

static void test_random(TestState &s)
{
  std::mt19937 rng{4711};
  for (unsigned int round = 0; round < 600; ++round) {
    const std::size_t max_len = round < 400 ? 7 : 60;
    const std::size_t n0 =
      std::uniform_int_distribution<std::size_t>{0, max_len}(rng);
    const std::size_t n1 =
      std::uniform_int_distribution<std::size_t>{0, max_len}(rng);
    // Small cost ranges make for lots of ties, which exercise the
    // left-leaning tie breaking.
    const cost_type max_cost = (round % 3) ? 2 : 10;
    const unsigned int inf_percent = (round % 4) * 15;
    const bool del_ins_inf = round % 17 == 0;
    std::uniform_int_distribution<cost_type> cost_dist{0, max_cost};
    std::uniform_int_distribution<unsigned int> percent_dist{0, 99};

    table_cost cost{n0, n1};
    for (std::size_t i = 0; i < n0; ++i) {
      cost.del(i) = (del_ins_inf && percent_dist(rng) < 30) ?
	INFINITE_COST : cost_dist(rng);
      for (std::size_t j = 0; j < n1; ++j) {
	cost.match(i, j) = percent_dist(rng) < inf_percent ?
	  INFINITE_COST : cost_dist(rng);
      }
    }
    for (std::size_t j = 0; j < n1; ++j) {
      cost.ins(j) = (del_ins_inf && percent_dist(rng) < 30) ?
	INFINITE_COST : cost_dist(rng);
    }

    const std::string name = ("random " + std::to_string(round) + " (" +
			      std::to_string(n0) + "x" + std::to_string(n1) +
			      ")");
    const full_table_reference ref{n0, n1, cost};
    const cost_type d = string_edit_distance(n0, n1, cost);
    const mapping_type m = string_align_mapping(n0, n1, cost);
    check(s, name + ": distance", d == ref.distance());
    if (n0 <= 7 && n1 <= 7) {
      check(s, name + ": brute force",
	    d == brute_force_distance(0, 0, n0, n1, cost));
    }

    cost_type total;
    check(s, name + ": mapping cost",
	  check_mapping(m, n0, n1, cost, total) && total == ref.distance());
    // With an infinite distance, everything ties and Hirschberg's
    // splits aren't guaranteed to follow the greedy walk.
    if (ref.distance() != INFINITE_COST)
      check(s, name + ": mapping left-leaning", m == ref.mapping());
  }
}

// Forwards to another cost, counting the calls, and failing for match
// costs outside of the current row.
class counting_cost : public string_edit_cost
{
public:
  counting_cost(const string_edit_cost &cost, const std::size_t n0,
		const std::size_t n1)
    : _cost(cost), _n1(n1), _match_calls(n0 * n1), _del_calls(n0),
      _insert_calls(n1)
  {}

  virtual cost_type cost_match(const std::size_t i0,
			       const std::size_t i1) const override
  {
    if (i0 != row)
      wrong_row = true;
    ++_match_calls.at(i0 * _n1 + i1);
    return _cost.cost_match(i0, i1);
  }

  virtual cost_type cost_del(const std::size_t i0) const override
  {
    if (i0 != row)
      wrong_row = true;
    ++_del_calls.at(i0);
    return _cost.cost_del(i0);
  }

  virtual cost_type cost_insert(const std::size_t i1) const override
  {
    ++_insert_calls.at(i1);
    return _cost.cost_insert(i1);
  }

  // Whether each match and deletion cost got requested exactly once,
  // and each insertion cost exactly the given number of times.
  bool calls_ok(const std::size_t inserts) const
  {
    const auto is = [](const std::size_t n) {
		      return [n](const std::size_t c) { return c == n; };
		    };
    return (std::all_of(_match_calls.begin(), _match_calls.end(), is(1)) &&
	    std::all_of(_del_calls.begin(), _del_calls.end(), is(1)) &&
	    std::all_of(_insert_calls.begin(), _insert_calls.end(),
			is(inserts)));
  }

  std::size_t row = 0;
  mutable bool wrong_row = false;

private:
  const string_edit_cost &_cost;
  std::size_t _n1;
  mutable std::vector<std::size_t> _match_calls;
  mutable std::vector<std::size_t> _del_calls;
  mutable std::vector<std::size_t> _insert_calls;
};

static void test_stepper(TestState &s)
{
  // After each step, the stepper's distance must be the one between
  // the processed prefix and the whole second string. Each step must
  // only ask for its own row's costs, each exactly once.
  std::mt19937 rng{815};
  for (unsigned int round = 0; round < 50; ++round) {
    const std::size_t n0 = rng() % 15, n1 = rng() % 15;
    table_cost cost{n0, n1};
    for (std::size_t i = 0; i < n0; ++i) {
      cost.del(i) = rng() % 4;
      for (std::size_t j = 0; j < n1; ++j)
	cost.match(i, j) = rng() % 5 == 0 ? INFINITE_COST : rng() % 4;
    }
    for (std::size_t j = 0; j < n1; ++j)
      cost.ins(j) = rng() % 4;

    counting_cost counting{cost, n0, n1};
    string_edit_distance_stepper stepper{n1, counting};
    bool ok = stepper.get_distance() == string_edit_distance(0, n1, cost);
    for (std::size_t i = 0; i < n0; ++i) {
      counting.row = i;
      stepper.step(counting);
      ok = ok && (stepper.get_distance() ==
		  string_edit_distance(i + 1, n1, cost));
    }
    const std::string name = "stepper " + std::to_string(round);
    check(s, name + ": prefix distances", ok);
    check(s, name + ": cost calls",
	  !counting.wrong_row && counting.calls_ok(n0 + 1));
  }
}

int main()
{
  TestState s;

  test_cost_add(s);
  test_known(s);
  test_empty(s);
  test_infinite(s);
  test_stepper(s);
  test_random(s);

  return 0;
}
