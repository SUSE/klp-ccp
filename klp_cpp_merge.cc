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

// klp-cpp-merge: merges C source files, typically klp-ccp outputs for
// different architectures, each associated with a preprocessor
// condition, into a single one, guarding the differences by
// conditional inclusions. Preprocessing the result with one of the
// conditions true yields the same as preprocessing the associated
// input.
//
// Each input gets modeled as a tree: the logical lines other than
// conditional inclusion directives are leaves, each conditional
// inclusion construct #if ... #endif is a node with one child per
// branch, and a branch's children are the logical lines and
// conditional inclusion constructs within it. Selkow's top-down tree
// edit distance yields a mapping between each pair of inputs. As its
// mappings are top-down, any differences are sequences of siblings,
// each a leaf or a complete conditional inclusion construct, so
// guarding them nests properly with the inputs' conditional
// inclusions.
//
// For more than two inputs, the nodes matched consistently by all
// pairwise mappings serve as anchors common to all inputs. The
// sequences of siblings inbetween get emitted for groups of inputs
// formed greedily, each guarded by the disjunction of its members'
// conditions, and recursively merged within the group the same way.
//
// klp-ccp emits conditional inclusions with exactly one branch taken
// and an #error in each of the others. A branch containing only a
// single #error doesn't take part in merging the branch's contents,
// unless that's the case for all inputs. Whitespace-only lines never
// get guarded. klp-ccp also adds a final
// #else with an #error to constructs with a branch taken, but not to
// those without, so a construct lacking an #else matches one having
// it with only a single #error, which then gets dropped.

#include <getopt.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "raw_pp_directive.hh"
#include "pp_except.hh"
#include "code_remarks.hh"
#include "tree-edit/selkow.hh"

using namespace klp::ccp;
namespace ted = klp::ted;

static const char prog_name[] = "klp-cpp-merge";

static const char optstr[] = ":ho:";

static const option longopts[] {
	{ "help", 0, nullptr, 'h' },
	{ "outfile", 1, nullptr, 'o' },
	{ nullptr, 0, nullptr, 0 }
};

static void show_usage(const char * const prog)
{
  std::cerr << "See '" << prog << " --help' for usage." << std::endl;
}

static void show_help(const char * const prog)
{
  std::cout
    << "Usage: " << prog
    << " OPTIONS [--] CONDITION FILE [CONDITION FILE...]"
    << std::endl;

  std::cout << "Options:" << std::endl;
  std::cout
    << " -o, --outfile=FILE"
    << "\t\t\t\tOutput file."
    << std::endl
    << std::endl;

  std::cout
    << "CONDITION FILE"
    << "\t\t\t\tdenotes an input C file and the preprocessor" << std::endl
    << "\t\t\t\t\t\tcondition, e.g. 'defined(CONFIG_X86_64)', under"
    << std::endl
    << "\t\t\t\t\t\twhich it applies. The conditions must be mutually"
    << std::endl
    << "\t\t\t\t\t\texclusive. The files get merged into a single"
    << std::endl
    << "\t\t\t\t\t\tone, with differences guarded by conditional"
    << std::endl
    << "\t\t\t\t\t\tinclusions."
    << std::endl << std::endl;
  std::cout
    << "For further information, please see"
    << " <https://github.com/suse/klp-ccp>."
    << std::endl;
}

static void show_opt_duplicate(const char * const prog,
			       const char * const o)
{
  std::cerr << "command line error: "
	    << " option '" << o << "' specified more than once"
	    << std::endl;
  show_usage(prog);
}

static void show_opt_missing(const char * const prog,
			       const char * const o)
{
  std::cerr << "command line error: "
	    << " option '" << o << "' is required"
	    << std::endl;
  show_usage(prog);
}


namespace
{
  // An error in the input, with the message ready for printing.
  class merge_error : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

  constexpr std::size_t INVALID = std::numeric_limits<std::size_t>::max();

  enum class node_kind
  {
    root,
    // A logical line other than a conditional inclusion directive.
    line,
    // A whole #if ... #endif construct.
    cond_incl,
    // One branch of a conditional inclusion construct.
    branch,
  };

  // A node of an input's tree, indexed by DFS pre-order.
  struct node_info
  {
    node_kind kind;
    // For lines: the logical line. For branches: the logical line of
    // the directive starting the branch.
    std::size_t line_index = INVALID;
    // For conditional inclusions: the logical lines of all of the
    // directives, including the final #endif.
    std::vector<std::size_t> directives;
    // For branches: the branch's index within its construct.
    std::size_t branch_index = INVALID;
    // For lines and conditional inclusions: an identifier for the
    // text, equal if and only if the texts are, modulo leading and
    // trailing whitespace on each physical line.
    std::size_t text_id = INVALID;
    // For conditional inclusions: like text_id, but with a final #else
    // whose branch contains only a single #error removed, if any.
    std::size_t stripped_text_id = INVALID;
    // The cost of deleting or inserting the node: the number of
    // physical lines of its logical line or directives.
    std::size_t cost = 0;
    // For lines: whether the line consists of whitespace only.
    bool blank = false;
    std::size_t parent = INVALID;
    // The node's position among its parent's children.
    std::size_t child_pos = INVALID;
    std::vector<std::size_t> children;
  };

  // Hands out identifiers for strings, equal for equal strings.
  class text_ids
  {
  public:
    std::size_t get(const std::string &s)
    {
      return _ids.emplace(s, _ids.size()).first->second;
    }

  private:
    std::unordered_map<std::string, std::size_t> _ids;
  };

  struct input_file
  {
    std::string cond;
    std::string filename;
    std::vector<std::string> phys_lines;
    std::vector<raw_pp_logical_line> logical_lines;
    std::optional<ted::tree> tree;
    std::vector<node_info> nodes;
  };
}

static std::string strip(const std::string &s)
{
  static const char ws[] = " \t\v\f\r";
  const std::size_t b = s.find_first_not_of(ws);
  if (b == std::string::npos)
    return std::string{};
  const std::size_t e = s.find_last_not_of(ws);
  return s.substr(b, e + 1 - b);
}

static std::vector<std::string> read_phys_lines(const std::string &filename)
{
  std::ifstream in{filename, std::ios::binary};
  if (!in)
    throw merge_error{"error: failed to open '" + filename + "'"};
  std::stringstream ss;
  ss << in.rdbuf();
  if (in.bad())
    throw merge_error{"error: failed to read '" + filename + "'"};
  const std::string text = ss.str();

  std::vector<std::string> lines;
  std::size_t b = 0;
  while (b < text.size()) {
    std::size_t e = text.find('\n', b);
    if (e == std::string::npos)
      e = text.size();
    lines.push_back(text.substr(b, e - b));
    b = e + 1;
  }
  return lines;
}

static merge_error input_error(const input_file &f,
			       const raw_pp_logical_line &l,
			       const std::string &msg)
{
  return merge_error{f.filename + ":" + std::to_string(l.first_line + 1) +
		     ": error: " + msg};
}

// The text of a logical line, with leading and trailing whitespace
// stripped from each of its physical lines.
static std::string normalized_text(const input_file &f,
				   const raw_pp_logical_line &l)
{
  std::string s;
  for (std::size_t i = l.first_line; i < l.end_line; ++i) {
    if (i != l.first_line)
      s.push_back('\n');
    s += strip(f.phys_lines[i]);
  }
  return s;
}

// Whether a branch contains only a single #error.
static bool is_error_only_branch(const input_file &f, const std::size_t b)
{
  const node_info &nb = f.nodes[b];
  return (nb.children.size() == 1 &&
	  f.nodes[nb.children[0]].kind == node_kind::line &&
	  (f.logical_lines[f.nodes[nb.children[0]].line_index].directive_kind ==
	   raw_pp_directive_kind::error));
}

static void build_tree(input_file &f, text_ids &ids)
{
  ted::tree_builder tb;
  std::vector<std::size_t> open;

  const auto push =
    [&](const node_kind kind) {
      const std::size_t n = tb.push_child();
      assert(n == f.nodes.size());
      node_info ni;
      ni.kind = kind;
      if (!open.empty()) {
	ni.parent = open.back();
	ni.child_pos = f.nodes[ni.parent].children.size();
	f.nodes[ni.parent].children.push_back(n);
      }
      f.nodes.push_back(std::move(ni));
      open.push_back(n);
      return n;
    };
  const auto pop =
    [&]() {
      tb.pop_node();
      open.pop_back();
    };

  push(node_kind::root);
  for (std::size_t li = 0; li < f.logical_lines.size(); ++li) {
    const raw_pp_logical_line &l = f.logical_lines[li];
    const std::size_t n_lines = l.end_line - l.first_line;
    const raw_pp_directive_kind k = l.directive_kind;

    if (raw_pp_directive_opens_cond_incl(k)) {
      const std::size_t c = push(node_kind::cond_incl);
      f.nodes[c].directives.push_back(li);
      f.nodes[c].cost = n_lines;
      const std::size_t b = push(node_kind::branch);
      f.nodes[b].branch_index = 0;
      f.nodes[b].line_index = li;

    } else if (raw_pp_directive_continues_cond_incl(k) ||
	       k == raw_pp_directive_kind::endif) {
      if (f.nodes[open.back()].kind != node_kind::branch) {
	throw input_error(f, l, (k == raw_pp_directive_kind::endif
				 ? "#endif without #if"
				 : "#elif or #else without #if"));
      }
      const std::size_t c = f.nodes[open.back()].parent;
      const raw_pp_logical_line &last =
	f.logical_lines[f.nodes[c].directives.back()];
      if (raw_pp_directive_continues_cond_incl(k) &&
	  last.directive_kind == raw_pp_directive_kind::else_) {
	throw input_error(f, l, "#elif or #else after #else");
      }

      pop();
      f.nodes[c].directives.push_back(li);
      f.nodes[c].cost += n_lines;
      if (k == raw_pp_directive_kind::endif) {
	pop();
      } else {
	const std::size_t b = push(node_kind::branch);
	f.nodes[b].branch_index = f.nodes[b].child_pos;
	f.nodes[b].line_index = li;
      }

    } else {
      const std::size_t n = push(node_kind::line);
      f.nodes[n].line_index = li;
      // Whitespace-only lines don't matter, so that they don't
      // influence the mappings. They never get guarded either.
      const std::string text = normalized_text(f, l);
      f.nodes[n].blank = text.find_first_not_of('\n') == std::string::npos;
      f.nodes[n].cost = f.nodes[n].blank ? 0 : n_lines;
      f.nodes[n].text_id = ids.get(text);
      pop();
    }
  }

  if (open.size() > 1) {
    const std::size_t c = f.nodes[open.back()].parent;
    throw input_error(f, f.logical_lines[f.nodes[c].directives.front()],
		      "unterminated #if");
  }
  pop();
  f.tree.emplace(tb.grab_result());

  // Conditional inclusion constructs match if all of their
  // directives do. klp-ccp adds an #else with an #error to constructs
  // with a branch taken, but not to those without, so such constructs
  // also match if they do after removing that #else.
  for (std::size_t n = 0; n < f.nodes.size(); ++n) {
    node_info &ni = f.nodes[n];
    if (ni.kind != node_kind::cond_incl)
      continue;
    const std::size_t last_branch = ni.children.back();
    const bool strip_else =
      ((f.logical_lines[f.nodes[last_branch].line_index].directive_kind ==
	raw_pp_directive_kind::else_) &&
       is_error_only_branch(f, last_branch));
    std::string s, stripped;
    for (std::size_t k = 0; k < ni.directives.size(); ++k) {
      const std::string d =
	normalized_text(f, f.logical_lines[ni.directives[k]]) + '\0';
      s += d;
      if (!strip_else || k + 2 != ni.directives.size())
	stripped += d;
    }
    ni.text_id = ids.get(s);
    ni.stripped_text_id = ids.get(stripped);
  }
}

static input_file read_input(const std::string &cond,
			     const std::string &filename, text_ids &ids)
{
  input_file f;
  f.cond = cond;
  f.filename = filename;
  f.phys_lines = read_phys_lines(filename);

  pp_result::header_inclusion_root hir{filename, false};
  code_remarks remarks;
  try {
    f.logical_lines = scan_raw_pp_logical_lines(hir, remarks);
  } catch (const pp_except &e) {
    throw merge_error{e.what()};
  }
  if (!remarks.empty())
    std::cerr << remarks;

  if (!f.logical_lines.empty() &&
      f.logical_lines.back().end_line != f.phys_lines.size()) {
    throw merge_error{"error: inconsistent line count for '" + filename +
		      "'"};
  }

  build_tree(f, ids);
  return f;
}

namespace
{
  // Leaves and conditional inclusion constructs match only if their
  // texts are equal, branches only if they're at the same position
  // within their constructs.
  class merge_cost : public ted::tree_edit_cost
  {
  public:
    merge_cost(const input_file &f, const input_file &g) noexcept
      : _f(f), _g(g)
    {}

    virtual ted::cost_type cost_match(const std::size_t n0,
				      const std::size_t n1) const override
    {
      const node_info &a = _f.nodes[n0];
      const node_info &b = _g.nodes[n1];
      if (a.kind != b.kind)
	return ted::INFINITE_COST;
      switch (a.kind) {
      case node_kind::root:
	return 0;
      case node_kind::line:
	return a.text_id == b.text_id ? 0 : ted::INFINITE_COST;
      case node_kind::cond_incl:
	return ((a.text_id == b.text_id ||
		 a.stripped_text_id == b.stripped_text_id)
		? 0 : ted::INFINITE_COST);
      case node_kind::branch:
	return a.branch_index == b.branch_index ? 0 : ted::INFINITE_COST;
      }
      return ted::INFINITE_COST;
    }

    virtual ted::cost_type cost_del(const std::size_t n0) const override
    {
      return _f.nodes[n0].cost;
    }

    virtual ted::cost_type cost_insert(const std::size_t n1) const override
    {
      return _g.nodes[n1].cost;
    }

  private:
    const input_file &_f;
    const input_file &_g;
  };

  // Indexed by the input positions i and j and a node of input i: the
  // node of input j it's mapped to, INVALID if none.
  typedef std::vector<std::vector<std::vector<std::size_t>>> mappings_type;

  class merger
  {
  public:
    merger(const std::vector<input_file> &inputs,
	   const mappings_type &mappings) noexcept
      : _inputs(inputs), _mappings(mappings)
    {}

    std::vector<std::string> merge();

  private:
    // A range [lo, hi) of a node's children, by position.
    struct segment
    {
      std::size_t parent;
      std::size_t lo;
      std::size_t hi;
    };

    typedef std::shared_ptr<const std::vector<std::string>> lines_ptr;

    // Merges the inputs in group, sorted, each with the given segment,
    // all of the segments' parents being matched to each other.
    lines_ptr _merge_children(const std::vector<std::size_t> &group,
			      const std::vector<segment> &segments);

    // Emits segments with no common anchors for the inputs in group.
    void _emit_gap(const std::vector<std::size_t> &group,
		   const std::vector<segment> &segments,
		   std::vector<std::string> &out);

    // Like _emit_gap(), but with all of the segments guarded.
    void _emit_guarded(const std::vector<std::size_t> &group,
		       const std::vector<segment> &segments,
		       std::vector<std::string> &out);

    // Emits a node matched across all of group, given by its position
    // in each of the segments.
    void _emit_anchor(const std::vector<std::size_t> &group,
		      const std::vector<segment> &segments,
		      const std::vector<std::size_t> &positions,
		      std::vector<std::string> &out);

    // Emits an input's segment as-is.
    void _emit_raw(const std::size_t input, const segment &s,
		   std::vector<std::string> &out) const;

    // The disjunction of the group's conditions.
    std::string _cond_expr(const std::vector<std::size_t> &group) const;

    const std::vector<input_file> &_inputs;
    const mappings_type &_mappings;
    std::map<std::vector<std::size_t>, lines_ptr> _memo;
  };
}

std::vector<std::string> merger::merge()
{
  std::vector<std::size_t> group;
  std::vector<segment> segments;
  for (std::size_t i = 0; i < _inputs.size(); ++i) {
    group.push_back(i);
    segments.push_back(segment{0, 0, _inputs[i].nodes[0].children.size()});
  }
  return *_merge_children(group, segments);
}

merger::lines_ptr
merger::_merge_children(const std::vector<std::size_t> &group,
			const std::vector<segment> &segments)
{
  assert(group.size() == segments.size());
  std::vector<std::size_t> key;
  for (std::size_t m = 0; m < group.size(); ++m) {
    key.insert(key.end(), {group[m], segments[m].parent, segments[m].lo,
			   segments[m].hi});
  }
  const auto it = _memo.find(key);
  if (it != _memo.end())
    return it->second;

  auto out = std::make_shared<std::vector<std::string>>();
  if (group.size() == 1) {
    _emit_raw(group[0], segments[0], *out);
    _memo.emplace(std::move(key), out);
    return out;
  }

  // Find the anchors: the children of the first input's segment
  // mapped into all of the others' segments, consistently across all
  // pairs of inputs. The pairwise mappings preserve the sibling
  // order, so the anchors are in order in each of the segments.
  const input_file &f0 = _inputs[group[0]];
  std::vector<std::vector<std::size_t>> anchors;
  for (std::size_t p = segments[0].lo; p < segments[0].hi; ++p) {
    const std::size_t x = f0.nodes[segments[0].parent].children[p];
    std::vector<std::size_t> nodes{x};
    std::vector<std::size_t> positions{p};
    bool is_anchor = true;
    for (std::size_t m = 1; is_anchor && m < group.size(); ++m) {
      const std::size_t y = _mappings[group[0]][group[m]][x];
      if (y == INVALID) {
	is_anchor = false;
	break;
      }
      const node_info &ny = _inputs[group[m]].nodes[y];
      if (ny.parent != segments[m].parent ||
	  ny.child_pos < segments[m].lo || ny.child_pos >= segments[m].hi) {
	is_anchor = false;
	break;
      }
      nodes.push_back(y);
      positions.push_back(ny.child_pos);
    }
    for (std::size_t m = 1; is_anchor && m < group.size(); ++m) {
      for (std::size_t k = m + 1; k < group.size(); ++k) {
	if (_mappings[group[m]][group[k]][nodes[m]] != nodes[k]) {
	  is_anchor = false;
	  break;
	}
      }
    }
    if (is_anchor)
      anchors.push_back(std::move(positions));
  }

  // Emit the anchors and the gaps inbetween.
  std::vector<std::size_t> prev;
  for (const segment &s : segments)
    prev.push_back(s.lo);
  const auto emit_gap_until =
    [&](const std::vector<std::size_t> &ends) {
      std::vector<segment> gap;
      for (std::size_t m = 0; m < group.size(); ++m)
	gap.push_back(segment{segments[m].parent, prev[m], ends[m]});
      _emit_gap(group, gap, *out);
    };
  for (const std::vector<std::size_t> &a : anchors) {
    emit_gap_until(a);
    _emit_anchor(group, segments, a, *out);
    for (std::size_t m = 0; m < group.size(); ++m)
      prev[m] = a[m] + 1;
  }
  std::vector<std::size_t> ends;
  for (const segment &s : segments)
    ends.push_back(s.hi);
  emit_gap_until(ends);

  _memo.emplace(std::move(key), out);
  return out;
}

void merger::_emit_gap(const std::vector<std::size_t> &group,
		       const std::vector<segment> &segments,
		       std::vector<std::string> &out)
{
  // Whitespace-only lines at the start and at the end of the segments
  // don't get guarded, but emitted as they are, taken from the first
  // input having any. If there's nothing else, there's no guard at
  // all.
  std::vector<segment> trimmed{segments};
  std::size_t leading_from = INVALID;
  std::size_t trailing_from = INVALID;
  for (std::size_t m = 0; m < group.size(); ++m) {
    const input_file &f = _inputs[group[m]];
    const std::vector<std::size_t> &children =
      f.nodes[segments[m].parent].children;
    const auto is_blank =
      [&](const std::size_t p) {
	const node_info &n = f.nodes[children[p]];
	return n.kind == node_kind::line && n.blank;
      };
    segment &t = trimmed[m];
    while (t.lo < t.hi && is_blank(t.lo))
      ++t.lo;
    while (t.lo < t.hi && is_blank(t.hi - 1))
      --t.hi;
    if (leading_from == INVALID && t.lo > segments[m].lo)
      leading_from = m;
    if (trailing_from == INVALID && t.hi < segments[m].hi)
      trailing_from = m;
  }

  if (leading_from != INVALID) {
    const std::size_t m = leading_from;
    _emit_raw(group[m],
	      segment{segments[m].parent, segments[m].lo, trimmed[m].lo},
	      out);
  }
  _emit_guarded(group, trimmed, out);
  if (trailing_from != INVALID) {
    const std::size_t m = trailing_from;
    _emit_raw(group[m],
	      segment{segments[m].parent, trimmed[m].hi, segments[m].hi},
	      out);
  }
}

void merger::_emit_guarded(const std::vector<std::size_t> &group,
			   const std::vector<segment> &segments,
			   std::vector<std::string> &out)
{
  // Only the inputs with a nonempty segment contribute. Group them
  // greedily: starting out with one group per input, repeatedly join
  // the pair of groups whose joining shrinks the output the most, as
  // long as there is any. A group consisting of all inputs isn't
  // eligible: there are no common anchors within the segments, so
  // merging them would recurse on the same problem. Groups are
  // represented by their members' positions within group, sorted.
  typedef std::vector<std::size_t> cluster_type;
  std::vector<cluster_type> clusters;
  for (std::size_t m = 0; m < group.size(); ++m) {
    if (segments[m].lo < segments[m].hi)
      clusters.push_back(cluster_type{m});
  }
  if (clusters.empty())
    return;

  const auto cluster_lines =
    [&](const cluster_type &c) {
      std::vector<std::size_t> g;
      std::vector<segment> s;
      for (const std::size_t m : c) {
	g.push_back(group[m]);
	s.push_back(segments[m]);
      }
      return _merge_children(g, s);
    };
  // The output size: an #if or #elif for each group, its lines, and
  // a final #endif.
  const auto clustering_size =
    [&](const std::vector<cluster_type> &cs) {
      std::size_t n = 1;
      for (const cluster_type &c : cs)
	n += 1 + cluster_lines(c)->size();
      return n;
    };

  std::size_t cur_size = clustering_size(clusters);
  while (clusters.size() > 1) {
    std::size_t best_size = cur_size;
    std::vector<cluster_type> best;
    for (std::size_t a = 0; a < clusters.size(); ++a) {
      for (std::size_t b = a + 1; b < clusters.size(); ++b) {
	cluster_type joined = clusters[a];
	joined.insert(joined.end(), clusters[b].begin(), clusters[b].end());
	if (joined.size() == group.size())
	  continue;
	std::sort(joined.begin(), joined.end());
	std::vector<cluster_type> trial;
	for (std::size_t c = 0; c < clusters.size(); ++c) {
	  if (c == a)
	    trial.push_back(joined);
	  else if (c != b)
	    trial.push_back(clusters[c]);
	}
	const std::size_t size = clustering_size(trial);
	if (size < best_size) {
	  best_size = size;
	  best = std::move(trial);
	}
      }
    }
    if (best.empty())
      break;
    clusters = std::move(best);
    cur_size = best_size;
  }

  // The groups' order in the output is by their first members.
  std::sort(clusters.begin(), clusters.end());
  for (std::size_t c = 0; c < clusters.size(); ++c) {
    std::vector<std::size_t> g;
    for (const std::size_t m : clusters[c])
      g.push_back(group[m]);
    out.push_back((c ? "#elif " : "#if ") + _cond_expr(g));
    const lines_ptr lines = cluster_lines(clusters[c]);
    out.insert(out.end(), lines->begin(), lines->end());
  }
  out.push_back("#endif");
}

void merger::_emit_anchor(const std::vector<std::size_t> &group,
			  const std::vector<segment> &segments,
			  const std::vector<std::size_t> &positions,
			  std::vector<std::string> &out)
{
  std::vector<std::size_t> nodes;
  for (std::size_t m = 0; m < group.size(); ++m) {
    nodes.push_back(_inputs[group[m]].nodes[segments[m].parent]
		    .children[positions[m]]);
  }

  // Matched nodes have equal texts, take the first input's, unless
  // it doesn't have the line at all.
  const input_file &f0 = _inputs[group[0]];
  const node_info &n0 = f0.nodes[nodes[0]];
  const auto emit_logical_line =
    [&](const input_file &f, const std::size_t li) {
      const raw_pp_logical_line &l = f.logical_lines[li];
      out.insert(out.end(), f.phys_lines.begin() + l.first_line,
		 f.phys_lines.begin() + l.end_line);
    };

  if (n0.kind == node_kind::line) {
    emit_logical_line(f0, n0.line_index);
    return;
  }

  // A conditional inclusion construct. The directives are all equal,
  // except that some of the inputs may have a final #else with only a
  // single #error which the others lack. Merge each branch's
  // contents. Inputs whose branch contains only a single #error don't
  // take part, unless that's the case for all of them. If such an
  // #else is lacking from some of the inputs, it gets dropped.
  assert(n0.kind == node_kind::cond_incl);
  std::size_t n_branches = 0;
  for (std::size_t m = 0; m < group.size(); ++m) {
    n_branches = std::max(n_branches,
			  _inputs[group[m]].nodes[nodes[m]].children.size());
  }
  for (std::size_t k = 0; k < n_branches; ++k) {
    std::vector<std::size_t> real, error_only;
    bool lacking = false;
    for (std::size_t m = 0; m < group.size(); ++m) {
      const input_file &f = _inputs[group[m]];
      const node_info &nc = f.nodes[nodes[m]];
      if (k >= nc.children.size())
	lacking = true;
      else if (is_error_only_branch(f, nc.children[k]))
	error_only.push_back(m);
      else
	real.push_back(m);
    }
    // Inputs lacking the final #else only ever match ones having it
    // with a single #error.
    assert(!lacking || real.empty());
    if (lacking)
      continue;

    const std::vector<std::size_t> &members =
      real.empty() ? error_only : real;
    const input_file &fd = _inputs[group[members[0]]];
    emit_logical_line(fd, fd.nodes[nodes[members[0]]].directives[k]);

    std::vector<std::size_t> g;
    std::vector<segment> s;
    for (const std::size_t m : members) {
      const input_file &f = _inputs[group[m]];
      const std::size_t b = f.nodes[nodes[m]].children[k];
      g.push_back(group[m]);
      s.push_back(segment{b, 0, f.nodes[b].children.size()});
    }
    const lines_ptr lines = _merge_children(g, s);
    out.insert(out.end(), lines->begin(), lines->end());
  }
  emit_logical_line(f0, n0.directives.back());
}

void merger::_emit_raw(const std::size_t input, const segment &s,
		       std::vector<std::string> &out) const
{
  // A node's subtree covers a contiguous range of physical lines, and
  // so do consecutive siblings.
  if (s.lo == s.hi)
    return;
  const input_file &f = _inputs[input];
  const auto first_line =
    [&](const std::size_t n) {
      const node_info &ni = f.nodes[n];
      const std::size_t li = (ni.kind == node_kind::line
			      ? ni.line_index : ni.directives.front());
      return f.logical_lines[li].first_line;
    };
  const auto end_line =
    [&](const std::size_t n) {
      const node_info &ni = f.nodes[n];
      const std::size_t li = (ni.kind == node_kind::line
			      ? ni.line_index : ni.directives.back());
      return f.logical_lines[li].end_line;
    };
  const std::vector<std::size_t> &children = f.nodes[s.parent].children;
  out.insert(out.end(),
	     f.phys_lines.begin() + first_line(children[s.lo]),
	     f.phys_lines.begin() + end_line(children[s.hi - 1]));
}

// Parenthesizes a condition, unless it's parenthesized already or of
// the form defined(...).
static std::string parenthesize_cond(const std::string &cond)
{
  const std::string c = strip(cond);
  static const std::regex defined_re{
    "defined[[:space:]]*\\([[:space:]]*[A-Za-z_][A-Za-z0-9_]*[[:space:]]*\\)"};
  if (std::regex_match(c, defined_re))
    return c;

  if (!c.empty() && c.front() == '(') {
    // Parenthesized if the opening parenthesis is closed only at the
    // very end.
    std::size_t depth = 0;
    std::size_t i = 0;
    for (; i < c.size(); ++i) {
      if (c[i] == '(') {
	++depth;
      } else if (c[i] == ')') {
	if (!--depth)
	  break;
      }
    }
    if (i + 1 == c.size())
      return c;
  }

  return "(" + c + ")";
}

std::string merger::_cond_expr(const std::vector<std::size_t> &group) const
{
  std::string s;
  for (const std::size_t i : group) {
    if (!s.empty())
      s += " || ";
    s += parenthesize_cond(_inputs[i].cond);
  }
  return s;
}

static void copy_file(const std::string &from, const std::string &to)
{
  std::ifstream in{from, std::ios::binary};
  if (!in)
    throw merge_error{"error: failed to open '" + from + "'"};
  std::ofstream out{to, std::ios::binary | std::ios::trunc};
  if (!out)
    throw merge_error{"error: failed to open '" + to + "' for writing"};
  if (in.peek() != std::ifstream::traits_type::eof())
    out << in.rdbuf();
  out.close();
  if (in.bad() || !out)
    throw merge_error{"error: failed to copy '" + from + "' to '" + to + "'"};
}

static void merge_files(const std::vector<std::pair<std::string,
						    std::string>> &inputs,
			const std::string &outfile)
{
  text_ids ids;
  std::vector<input_file> files;
  for (const auto &i : inputs)
    files.push_back(read_input(i.first, i.second, ids));

  mappings_type mappings(files.size());
  for (std::size_t i = 0; i < files.size(); ++i) {
    mappings[i].resize(files.size());
    for (std::size_t j = 0; j < files.size(); ++j)
      mappings[i][j].assign(files[i].nodes.size(), INVALID);
  }
  for (std::size_t i = 0; i < files.size(); ++i) {
    for (std::size_t j = i + 1; j < files.size(); ++j) {
      const merge_cost cost{files[i], files[j]};
      for (const auto &m : ted::selkow_mapping(*files[i].tree,
					       *files[j].tree, cost)) {
	mappings[i][j][m.first] = m.second;
	mappings[j][i][m.second] = m.first;
      }
    }
  }

  const std::vector<std::string> lines = merger{files, mappings}.merge();

  std::ofstream out{outfile, std::ios::binary | std::ios::trunc};
  if (!out)
    throw merge_error{"error: failed to open '" + outfile + "' for writing"};
  for (const std::string &l : lines)
    out << l << '\n';
  out.close();
  if (!out)
    throw merge_error{"error: failed to write '" + outfile + "'"};
}

int main(int argc, char *argv[])
{
  bool do_help = false;
  const char *o_outfile = nullptr;

  int o;
  int longindex = -1;
  while (optind < argc && std::strcmp(argv[optind], "--") &&
	 (o = getopt_long(argc, argv, optstr, longopts, &longindex)) != -1) {
    switch (o) {
    case 'h':
      do_help = true;
      break;

    case 'o':
      if (o_outfile) {
	show_opt_duplicate(prog_name, "--outfile");
	return 1;
      }
      o_outfile = optarg;
      break;

    case '?':
      std::cerr << "command line error: invalid option '";
      if (optopt)
	std::cerr << '-' << static_cast<char>(optopt);
      else
	std::cerr << argv[optind - 1];
      std::cerr << '\'' << std::endl;
      show_usage(prog_name);
      return 1;

    case ':':
      std::cerr << "command line error: missing parameter for option '";
      if (optopt)
	std::cerr << '-' << static_cast<char>(optopt);
      else
	std::cerr << longopts[longindex].name;
      std::cerr << '\'' << std::endl;
      show_usage(prog_name);
      return 1;
    }
  }

  if (do_help) {
    show_help(prog_name);
    return 0;
  }

  if (optind != argc && !std::strcmp(argv[optind], "--"))
    ++optind;

  if (!o_outfile) {
    show_opt_missing(prog_name, "--outfile");
    return 1;
  }

  if (optind == argc) {
    std::cerr << "command line error: no input files" << std::endl;
    show_usage(prog_name);
    return 1;
  } else if ((argc - optind) % 2) {
    std::cerr << "command line error: "
	      << "expected pairs of condition and input file" << std::endl;
    show_usage(prog_name);
    return 1;
  }

  std::vector<std::pair<std::string, std::string>> inputs;
  for (int i = optind; i < argc; i += 2)
    inputs.emplace_back(argv[i], argv[i + 1]);

  try {
    if (inputs.size() == 1)
      copy_file(inputs[0].second, o_outfile);
    else
      merge_files(inputs, o_outfile);
  } catch (const merge_error &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
