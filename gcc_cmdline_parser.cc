/*
 * Copyright (C) 2019  SUSE Software Solutions Germany GmbH
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

#include <cstring>
#include <cassert>
#include <algorithm>
#include "gcc_cmdline_parser.hh"
#include "cmdline_except.hh"

using namespace klp::ccp;

gcc_cmdline_parser::gcc_cmdline_parser(const gcc_version version) noexcept
  : _version(version)
{}

void gcc_cmdline_parser::register_table(const option * const table)
{
  std::size_t nentries = 0;
  for (const option *entry = table; entry->name; ++entry) {
    ++nentries;
    assert(entry == table || std::strcmp((entry - 1)->name, entry->name) <= 0);
    assert(!(entry->arg & option::arg_joined_or_missing) ||
	   entry->arg == option::arg_joined_or_missing);
  }

  _tables.push_back(std::make_pair(table, nentries));
}

void gcc_cmdline_parser::
operator()(const int argc, const char *argv[],
	   const std::function<void(const char *name,
				    const char *val,
				    bool negative)> &callback) const
{
  int optind = 0;
  while (optind < argc) {
    if (argv[optind][0] != '-') {
      callback(nullptr, argv[optind], false);
      ++optind;
      continue;
    }

    bool negative = false;
    const char *cur_arg = argv[optind];
    const option *o = nullptr;
    o = _find_option(cur_arg + 1);
    if (!o &&
	(cur_arg[1] == 'f' || cur_arg[1] == 'W' ||
	 cur_arg[1] == 'm') &&
	cur_arg[2] == 'n' && cur_arg[3] == 'o' && cur_arg[4] == '-') {
      negative = true;
      const std::size_t cur_arg_len = std::strlen(cur_arg);
      std::string name;
      name.reserve(cur_arg_len - 1 - 3);
      name.push_back(cur_arg[1]);
      name.append(&cur_arg[5], cur_arg + cur_arg_len);
      o = _find_option(name.c_str());
    }

    if (!o || (negative && o->reject_negative)) {
      throw cmdline_except{
	      std::string("unrecognized gcc option '") + argv[optind] + "'"
	    };
    }

    cur_arg += 1 + std::strlen(o->name);
    if (negative)
      cur_arg += 3;

    if (*cur_arg == '\0')
      cur_arg = nullptr;

    if (o->arg != option::argument::arg_joined_or_missing) {
      if (cur_arg) {
	if (!(o->arg & option::argument::arg_joined)) {
		throw cmdline_except{
			(std::string("unrecognized gcc option '") +
			 argv[optind] + "'")
		      };
	}
      } else {
	if (o->arg & option::argument::arg_separate &&
	    optind + 1 < argc) {
	  cur_arg = argv[optind + 1];
	  ++optind;
	} else if (o->arg != option::argument::arg_none &&
		   o->arg != option::argument::arg_joined_or_missing) {
		throw cmdline_except{
			(std::string("missing argument to gcc option '") +
			 argv[optind] + "'")
		      };
	}
      }
    }

    while (o->alias.name) {
      const option::alias_type a = o->alias;
      o = _find_option(a.name);
      assert(o);

      if (a.neg_arg) {
	assert(!cur_arg);
	if (negative) {
	  negative = false;
	  cur_arg = a.neg_arg;
	} else {
	  cur_arg = a.pos_arg;
	}
      } else if (a.pos_arg) {
	assert(!negative);
	assert(!cur_arg);
	cur_arg = a.pos_arg;
      }
    }

    callback(o->name, cur_arg, negative);
    ++optind;
  }
}



const gcc_cmdline_parser::option*
gcc_cmdline_parser::_find_option(const char *s, const _table_type &table)
  const noexcept
{
  // Find the longest match within table, if any.
  // table is assumed to be sorted by option name.
  const option * const opts_begin = table.first;
  const option * const opts_end = opts_begin + table.second;

  // Find the last applicable option for which ->name equals the
  // beginning of s. If any exists, it will be the last in the table
  // applicable to the specified GCC version which strcmp()s <=
  // s. Find the first option whose name compares > s. The matching
  // one, if any, will be the first option before that one which is
  // applicable to the specified GCC version.
  const option *o =
    std::upper_bound(opts_begin, opts_end, s,
		     [&](const char *_s, const option &entry) {
		       return (std::strcmp(_s, entry.name) < 0);
		     });
  if (o == opts_begin)
    return nullptr;

  // Find the first preceeding option applicable to
  // the specified GCC version.
  while (o != opts_begin &&
	 !((o - 1)->min_gcc_version <= _version &&
	   _version <= (o - 1)->max_gcc_version)) {
    --o;
  }

  if (o == opts_begin)
    return nullptr;
  --o;

  // And see if it is a prefix of s.
  const std::size_t o_name_len = std::strlen(o->name);
  if (std::strlen(s) < o_name_len)
    return nullptr;

  if (!std::equal(o->name, o->name + o_name_len, s))
    return nullptr;

  return o;
}

const gcc_cmdline_parser::option*
gcc_cmdline_parser::_find_option(const char *s) const noexcept
{
  const option *o = nullptr;
  std::size_t o_name_len = 0;

  for (const auto &t : _tables) {
    const option * const _o = _find_option(s, t);
    if (_o) {
      const std::size_t _o_name_len = std::strlen(_o->name);
      if (_o_name_len > o_name_len) {
	o = _o;
	o_name_len = _o_name_len;
      }
    }
  }

  return o;
}
