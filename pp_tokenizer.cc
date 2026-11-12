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

#include <cassert>
#include <cerrno>
#include <system_error>
#include <limits>
#include "pp_tokenizer.hh"
#include "pp_except.hh"
#include "source_reader.hh"

using namespace klp::ccp;
using namespace klp::ccp::impl;

_pp_tokenizer::_pp_tokenizer()
  : _buf{}, _buf_it{_buf.cbegin()},
    _line_length{0}, _expect_qh_str{expect_qh_str::newline},
    _pending{0}, _queue{0}, _queue_locs{0},
    _next_queue_loc{0}, _queue_pos{0}
{}

_pp_tokenizer::~_pp_tokenizer() noexcept = default;

void _pp_tokenizer::init_state()
{
  // Pure virtual ->read_raw() can't be called from the base class
  // constructor. Provide this init_state() to be called from
  // the derived class' constructor.
  // As the _buf is still empty(), we need to have a non-zero char in
  // the "last" position to get _refill_queue_half() going, otherwise
  // it would report an EOF.
  _queue[_queue_capacity - 1] = std::numeric_limits<char>::max();
  _refill_queue_half(false);
  _refill_queue_half(true);
}

void _pp_tokenizer::_refill_buf()
{
  assert(_buf_it == _buf.cend());
  _buf = this->read_raw();
  _buf_it = _buf.cbegin();
  if (_buf.empty()) {
    if (_line_length) {
      this->add_line(_line_length);
      _line_length = 0;
    }
  }

  auto prev = _buf.cbegin();
  auto next = std::find(prev, _buf.cend(), '\n');
  while (next != _buf.cend()) {
    ++next;
    _line_length += next - prev;
    this->add_line(_line_length);
    _line_length = 0;
    prev = next;
    next = std::find(prev, _buf.cend(), '\n');
  }
  _line_length += next - prev;
}

void _pp_tokenizer::_refill_queue_half(bool odd_half)
{
  const unsigned int queue_half_begin_pos =
    static_cast<unsigned int>(odd_half) << (_queue_capacity_log2 - 1);
  char * const queue_half = &_queue[queue_half_begin_pos];
  range_in_file::loc_type * const queue_half_locs =
    &_queue_locs[queue_half_begin_pos];

  // Fast path: copy everything available over from _buf and hope that
  // a.) there are enough bytes available in _buf and b.) there are no
  // continuation lines which would require special handling.
  auto copy_dst_pos = 0;
  if (_pending[0]) {
    queue_half[copy_dst_pos] = _pending[0];
    queue_half_locs[copy_dst_pos++] = _next_queue_loc++;
    if (_pending[1]) {
      queue_half[copy_dst_pos] = _pending[1];
      queue_half_locs[copy_dst_pos++] = _next_queue_loc++;
    }
    _pending[0] = 0;
    _pending[1] = 0;
  }
  auto n_copy = std::min<unsigned int>(_buf.cend() - _buf_it,
				      (_queue_capacity >> 1) - copy_dst_pos);
  std::copy(_buf_it, _buf_it + n_copy, &queue_half[copy_dst_pos]);
  std::generate(&queue_half_locs[copy_dst_pos],
		&queue_half_locs[copy_dst_pos + n_copy],
		[&]() { return _next_queue_loc++; });
  _buf_it += n_copy;
  copy_dst_pos += n_copy;

  // Optimistically assume that the queue_half[] half got fully refilled
  // above, search for a backslash possibly indicating a continuation
  // line, in the hope there is none.
  unsigned int backslash_pos = std::find(queue_half,
					 queue_half + (_queue_capacity >> 1),
					 '\\') - queue_half;

  while (copy_dst_pos != (_queue_capacity >> 1) ||
	 backslash_pos != (_queue_capacity >> 1)) [[unlikely]] {
    // Slow path, either of the following has happened:
    // - There had not been enough bytes available in _buf[]
    //   to refill the full queue half or
    // - there's a backslash, which might start a continuation line
    //   marker encoding or
    // - there had been sequence of continuation lines removed
    //   somewhere and the resulting gap at the end needs another
    //   refill.
    const auto copy_dst_begin_pos = copy_dst_pos;
    if (_pending[0]) {
      queue_half[copy_dst_pos] = _pending[0];
      queue_half_locs[copy_dst_pos++] = _next_queue_loc++;
      if (_pending[1]) {
	queue_half[copy_dst_pos] = _pending[1];
	queue_half_locs[copy_dst_pos++] = _next_queue_loc++;
      }
      _pending[0] = 0;
      _pending[1] = 0;
    }
    while (copy_dst_pos != (_queue_capacity >> 1)) {
      if (_buf_it == _buf.cend()) {
	// Don't retry reading from the input once the EOF
	// has been encountered. During normal operation, the
	// other queue half last's byte being zero is a necessary
	// condition for having seen the EOF. At startup, this
	// this is explictly flipped to a non-zero value to
	// get things going when _buf.empty() is still true.
	const auto other_queue_half_begin_pos =
	  queue_half_begin_pos ^ (1u << (_queue_capacity_log2 - 1));
	const auto other_queue_half_last_pos =
	  other_queue_half_begin_pos + ((_queue_capacity >> 1) - 1);
	if (!_buf.empty() || _queue[other_queue_half_last_pos] != 0)
	  _refill_buf();
	if (_buf.empty()) {
	  std::fill(&queue_half[copy_dst_pos],
		    &queue_half[_queue_capacity >> 1], 0);
	  std::fill(&queue_half_locs[copy_dst_pos],
		    &queue_half_locs[_queue_capacity >> 1],
		    _next_queue_loc);
	  copy_dst_pos = _queue_capacity >> 1;
	  break;
	}
      }
      n_copy = std::min<unsigned int>(_buf.cend() - _buf_it,
				      (_queue_capacity >> 1) - copy_dst_pos);
      std::copy(_buf_it, _buf_it + n_copy, &queue_half[copy_dst_pos]);
      std::generate(&queue_half_locs[copy_dst_pos],
		    &queue_half_locs[copy_dst_pos + n_copy],
		    [&]() { return _next_queue_loc++; });
      _buf_it += n_copy;
      copy_dst_pos += n_copy;
    }

    // If the found backslash position, if any, is after or at the
    // beginning of the range just refilled, the search needs to be
    // redone.
    if (backslash_pos >= copy_dst_begin_pos) {
      backslash_pos = std::find(&queue_half[copy_dst_begin_pos],
				queue_half + (_queue_capacity >> 1),
				'\\') - queue_half;
    }

    if (backslash_pos != (_queue_capacity >> 1)) {
      unsigned int cont_lines_end_pos;
      for (cont_lines_end_pos = backslash_pos;
	   cont_lines_end_pos != ((_queue_capacity >> 1) - (backslash_pos & 1));
	   cont_lines_end_pos += 2) {
	if (queue_half[cont_lines_end_pos] != '\\' ||
	    queue_half[cont_lines_end_pos + 1] != '\n')
	  break;
      }
      if (cont_lines_end_pos == ((_queue_capacity >> 1) - 1)) {
	// The initial backslash is at an odd position, and the search
	// extends beyond the queue half.
	// That is, if the last, not yet inspected character from
	// the queue_half[] is a '\\', then peek one more character
	// from the input to see if the two combine to
	// a continuation line marker encoding.
	if (queue_half[cont_lines_end_pos] == '\\') {
	  if (_buf_it == _buf.cend() && !_buf.empty())
	    _refill_buf();
	  if (!_buf.empty()) {
	    _pending[0] = *(_buf_it++);
	    if (_pending[0] == '\n') {
	      _pending[0] = 0;
	      ++_next_queue_loc;
	      ++cont_lines_end_pos;
	      assert(cont_lines_end_pos == (_queue_capacity >> 1));
	    }
	  }
	}
      }
      // If the continuation line markers extend to the end of
      // queue_half, attempt to consume more from the input, if any.
      if (cont_lines_end_pos == (_queue_capacity >> 1)) {
	do {
	  assert(_pending[0] == 0 && _pending[1] == 0);
	  assert(!_buf.empty());
	  if (_buf_it == _buf.cend())
	    _refill_buf();
	  if (_buf.empty())
	    break;
	  _pending[0] = *(_buf_it++);
	  if (_pending[0] == '\\') {
	    assert(!_buf.empty());
	    if (_buf_it == _buf.cend())
	      _refill_buf();
	    if (_buf.empty())
	      break;
	    _pending[1] = *(_buf_it++);
	    if (_pending[1] == '\n') {
	      _pending[0] = 0;
	      _pending[1] = 0;
	      _next_queue_loc += 2;
	    }
	  }
	} while (_pending[0] == 0);
      }

      if (cont_lines_end_pos != backslash_pos) {
	// Remove the continuation lines from the queue by moving
	// subsequent characters to their starting position.
	// Successive continuation lines are collated into one
	// and recorded with a single '\0' character.
	queue_half[backslash_pos] = 0;
	if (cont_lines_end_pos != backslash_pos + 1) {
	  std::copy(&queue_half[cont_lines_end_pos],
		    &queue_half[_queue_capacity >> 1],
		    &queue_half[backslash_pos + 1]);
	  // Keep the recorded file locations in sync with the moved
	  // characters.
	  std::copy(&queue_half_locs[cont_lines_end_pos],
		    &queue_half_locs[_queue_capacity >> 1],
		    &queue_half_locs[backslash_pos + 1]);
	} else {
	  assert(cont_lines_end_pos == (_queue_capacity >> 1));
	}
	// Make the next loop iteration to fill up the gap at
	// the tail.
	copy_dst_pos =
	  backslash_pos + 1 + ((_queue_capacity >> 1) - cont_lines_end_pos);
      }

      // In either case, search for possibly more occurences of '\\'.
      backslash_pos = std::find(&queue_half[backslash_pos + 1],
				queue_half + (_queue_capacity >> 1),
				'\\') - queue_half;
    }
  }
}

void _pp_tokenizer::_advance_queue_to_pos(const unsigned int new_queue_pos)
{
  // With queue refilling in batches of the two queue halves,
  // advancing over a whole half without ever visiting it here is not
  // possible.
  assert(((((new_queue_pos ^ _queue_pos) >> (_queue_capacity_log2 - 1)) & 1)
	  != 0) ||
	  new_queue_pos >= _queue_pos);
  const auto old_pos = _queue_pos;
  _queue_pos = new_queue_pos;
  if ((old_pos ^ _queue_pos) & (_queue_capacity >> 1)) {
    // Crossed into the other queue half. Refill the half just left.
    _refill_queue_half(old_pos >> (_queue_capacity_log2 - 1));

    // When crossing a queue half boundary, wrapping _queue_pos
    // might be needed (i.e. every other time).
    _queue_pos &= _queue_capacity - 1;
  }
}

unsigned int _pp_tokenizer::_cur_queue_window_end_pos_wrap() const noexcept
{
  // When in the first half, everything up to but including the last
  // character from the second half may be consumed without an
  // intermediate refill being required (moving _queue_pos from the
  // first half to that position of the last character in the second
  // half would still trigger a refill of the first half eventually.
  // When in the second half, it's vice-versa.
  const unsigned int in_odd = _queue_pos >> (_queue_capacity_log2 - 1);
  return (_queue_capacity >> in_odd) - 1;
}

unsigned int _pp_tokenizer::_cur_queue_window_end_pos_nowrap() const noexcept
{
  // When in the first half, everything up to but including the last
  // character from the second half may be consumed without an
  // intermediate refill being required (moving _queue_pos from the
  // first half to that position of the last character in the second
  // half would still trigger a refill of the first half eventually).
  // When in the second half, the search range in limited by
  // wraparound.
  const unsigned int in_even = 1 - (_queue_pos >> (_queue_capacity_log2 - 1));
  return _queue_capacity - in_even;
}

unsigned int _pp_tokenizer::
_cur_queue_window_end_pos_nowrap(unsigned int search_begin_pos) const noexcept
{
  search_begin_pos &= _queue_capacity - 1;
  const unsigned int queue_pos_in_even =
    1 - (_queue_pos >> (_queue_capacity_log2 - 1));
  const unsigned int search_begin_pos_in_even =
    1 - (search_begin_pos >> (_queue_capacity_log2 - 1));
  // It is assumed that search_begin_pos comes after the current
  // queue head, _queue_pos in ringbuffer order. So except if
  // search_begin_pos wrapped around, the former is >= the latter.
  assert(queue_pos_in_even ^ search_begin_pos_in_even ||
	 _queue_pos <= search_begin_pos);
  // The queue refilling logic requires that the queue gets advanced
  // to some byte in the half _queue_pos is not currently in before
  // re-entering the currently active half. This, and ringbuffer
  // wraparound, limits the search range. The table is:
  // queue_pos_in_even search_begin_pos_in_even    search range end.
  // 0                 0                           _queue_capacity
  // 0                 1                           (_queue_capacity >> 1) - 1
  // 1                 0                           _queue_capacity - 1
  // 1                 1                           _queue_capacity - 1
  const unsigned int any_in_even =
    queue_pos_in_even | search_begin_pos_in_even;
  // Instead of computing
  // _queue_capacity >> (search_begin_pos_in_even & ~queue_pos_in_even),
  // enable subexpression reuse by shifting to the
  // left by one if any_in_even and undo if queue_pos_in_even.
  // To reduce data dependency paths, reverse that order of shifts,
  // so that the shift left by queue_pos_in_even can be computed
  // when any_in_even isn't available yet.
  assert(any_in_even - queue_pos_in_even ==
	 (search_begin_pos_in_even & ~queue_pos_in_even));
  static_assert((_queue_capacity << 1) >> 1  == _queue_capacity,
		"_queue_capacity's width exceeded in computation");
  return ((_queue_capacity << queue_pos_in_even) >> any_in_even) - any_in_even;
}

unsigned int _pp_tokenizer::_next_queued_non_continuation_pos(unsigned int pos)
  const noexcept
{
  pos &= _queue_capacity - 1;
  const unsigned char c = static_cast<unsigned char>(_queue[pos]);
  return (pos + (c == 0)) & (_queue_capacity - 1);
}

bool _pp_tokenizer::_at_eof() const noexcept
{
  return ((_queue[_queue_pos] |
	   _queue[(_queue_pos + 1) & (_queue_capacity - 1)])
	  == 0);
}

raw_pp_token _pp_tokenizer::
_tokenize_string(const range_in_file::loc_type begin_loc,
		 const char delim,
		 const bool delim_escapable,
		 const pp_token::type tok_type)
{
  std::string value;
  bool in_escape = false;

  _advance_queue_to_pos(_next_queued_non_continuation_pos(_queue_pos));
  char cur = _queue[_queue_pos];
  const char needle[] = { delim, '\0', delim_escapable ? '\\' : '\0' };
  while(cur) {
    if (cur != delim || (delim_escapable && in_escape)) {
      assert(_queue_pos + 1 != _queue_capacity ||
	     _cur_queue_window_end_pos_nowrap() == _queue_capacity);
      const auto next_poi_pos =
	(std::find_first_of(&_queue[_queue_pos + 1],
			    &_queue[_cur_queue_window_end_pos_nowrap()],
			    &needle[0],
			    &needle[3])
	 - &_queue[0]);
      in_escape = !in_escape && (cur == '\\') && next_poi_pos == _queue_pos + 1;
      if (next_poi_pos != _queue_capacity) {
	value.append(&_queue[_queue_pos], &_queue[next_poi_pos]);
	_advance_queue_to_pos(next_poi_pos);
      } else {
	// The search stopped at queue ringbuffer wraparound. Continue
	// the there to minimize allocations.
	const unsigned int next_poi_pos_wrapped =
	  (std::find_first_of(&_queue[0],
			      &_queue[_cur_queue_window_end_pos_wrap()],
			      &needle[0],
			      &needle[3])
	   - &_queue[0]);
	in_escape &= next_poi_pos_wrapped == 0;
	value.reserve(value.size() +
		      (next_poi_pos - _queue_pos) +
		      next_poi_pos_wrapped);
	value.append(&_queue[_queue_pos], &_queue[next_poi_pos]);
	value.append(&_queue[0], &_queue[next_poi_pos_wrapped]);
	_advance_queue_to_pos(next_poi_pos_wrapped);
      }
      cur = _queue[_queue_pos];
      if (!cur) {
	_advance_queue_to_pos(_next_queued_non_continuation_pos(_queue_pos));
	cur = _queue[_queue_pos];
      }
    } else {
      _advance_queue_to_pos(_queue_pos + 1);
      return raw_pp_token(tok_type, std::move(value),
			  range_in_file(begin_loc, _queue_locs[_queue_pos]));
    }
  }

  throw this->report_fatal("encountered EOF while searching for end of string",
			   _queue_locs[_queue_pos]);
}

raw_pp_token _pp_tokenizer::_tokenize_punctuator()
{
  std::string value;
  const char cur = _queue[_queue_pos];
  const auto begin_loc = _queue_locs[_queue_pos];
  auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
  const char next = _queue[next_pos];
  auto consumed_queue_end = _queue_pos + 1;
  if (cur == '-' && next == '>') {
    value = "->";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '+' && next == '+') {
    value = "++";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '-' && next == '-') {
    value = "--";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '<' && next == '<') {
    const auto next_next_pos = _next_queued_non_continuation_pos(next_pos + 1);
    const char next_next = _queue[next_next_pos];
    if (next_next != '=') {
      value = "<<";
      consumed_queue_end = next_pos + 1;
    } else {
      value = "<<=";
      consumed_queue_end = next_next_pos + 1;
    }
  } else if (cur == '>' && next == '>') {
    const auto next_next_pos = _next_queued_non_continuation_pos(next_pos + 1);
    const char next_next = _queue[next_next_pos];
    if (next_next != '=') {
      value = ">>";
      consumed_queue_end = next_pos + 1;
    } else {
      value = ">>=";
      consumed_queue_end = next_next_pos + 1;
    }
  } else if (cur == '<' && next == '=') {
    value = "<=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '>' && next == '=') {
    value = ">=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '=' && next == '=') {
    value = "==";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '!' && next == '=') {
    value = "!=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '*' && next == '=') {
    value = "*=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '/' && next == '=') {
    value = "/=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '%' && next == '=') {
    value = "%=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '+' && next == '=') {
    value = "+=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '-' && next == '=') {
    value = "-=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '&' && next == '=') {
    value = "&=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '^' && next == '=') {
    value = "^=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '|' && next == '=') {
    value = "|=";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '&' && next == '&') {
    value = "&&";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '|' && next == '|') {
    value = "||";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '.' && next == '.') {
    const auto next_next_pos = _next_queued_non_continuation_pos(next_pos + 1);
    const char next_next = _queue[next_next_pos];
    if (next_next == '.') {
      value = "...";
      consumed_queue_end = next_next_pos + 1;
    } else {
      value = ".";
    }
  } else if (cur == '#' && next == '#') {
    value = "##";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '<' && next == ':') {
    value = "<:";
    consumed_queue_end = next_pos + 1;
  } else if (cur == ':' && next == '>') {
    value = ":>";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '<' && next == '%') {
    value = "<%";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '%' && next == '>') {
    value = "%>";
    consumed_queue_end = next_pos + 1;
  } else if (cur == '%' && next == ':') {
    _advance_queue_to_pos(next_pos + 1);
    consumed_queue_end = next_pos + 1;
    const auto next_next_pos = _next_queued_non_continuation_pos(next_pos + 1);
    const auto next_next_next_pos =
      _next_queued_non_continuation_pos(next_next_pos + 1);
    const char next_next = _queue[next_next_pos];
    const char next_next_next = _queue[next_next_next_pos];
    if (next_next == '%' && next_next_next == ':') {
      consumed_queue_end = next_next_next_pos + 1;
      value = "%:%:";
    } else {
      value = "%:";
    }
  } else {
    value = cur;
  }
  _advance_queue_to_pos(consumed_queue_end);
  return raw_pp_token(pp_token::type::punctuator, value,
		      range_in_file(begin_loc, _queue_locs[_queue_pos]));
}

raw_pp_token _pp_tokenizer::_tokenize_pp_number()
{
  std::string value;
  bool done = false;

  assert(_queue[_queue_pos] == '.' ||
	 ('0' <= _queue[_queue_pos] && _queue[_queue_pos] <= '9'));
  const auto begin_loc = _queue_locs[_queue_pos];
  while(!done) {
    const auto cur_pos = _next_queued_non_continuation_pos(_queue_pos);
    const char cur = _queue[cur_pos];
    switch (cur) {
    case '+':
    case '-':
      if (!value.empty() &&
	  (value.back() == 'e' || value.back() == 'E' ||
	   value.back() == 'p' || value.back() == 'P')) {
	_advance_queue_to_pos(cur_pos + 1);
	value.push_back(cur);
      } else {
	done = true;
      }
      break;

    case '.':
    case '0' ... '9':
    case '_':
    case 'a' ... 'z':
    case 'A' ... 'Z':
      {
	const auto &not_is_pp_number_char =
	  [](const char c) {
	    return
	      !(c == '.' || c == '_' ||
		('0' <= c && c <= '9') ||
		('a' <= c && c <= 'z') ||
		('A' <= c && c <= 'Z'));
	  };

	assert(_cur_queue_window_end_pos_nowrap(cur_pos) >= cur_pos + 1);
	const auto next_poi_pos =
	  (std::find_if(&_queue[cur_pos + 1],
			&_queue[_cur_queue_window_end_pos_nowrap(cur_pos)],
			not_is_pp_number_char)
	   - &_queue[0]);
	if (next_poi_pos != _queue_capacity) {
	  value.append(&_queue[cur_pos], &_queue[next_poi_pos]);
	  _advance_queue_to_pos(next_poi_pos);
	} else {
	  // The search stopped at queue ringbuffer wraparound. Continue
	  // the there to minimize allocations.
	  const unsigned int next_poi_pos_wrapped =
	    (std::find_if(&_queue[0],
			  &_queue[_cur_queue_window_end_pos_wrap()],
			  not_is_pp_number_char)
	     - &_queue[0]);
	  value.reserve(value.size() +
			(next_poi_pos - cur_pos) +
			next_poi_pos_wrapped);
	  value.append(&_queue[cur_pos], &_queue[next_poi_pos]);
	  value.append(&_queue[0], &_queue[next_poi_pos_wrapped]);
	  _advance_queue_to_pos(next_poi_pos_wrapped);
	}
      }
      break;

    default:
      done = true;
    }
  }

  return raw_pp_token(pp_token::type::pp_number, std::move(value),
		      range_in_file(begin_loc, _queue_locs[_queue_pos]));
}

raw_pp_token _pp_tokenizer::_tokenize_id()
{
  std::string value;
  bool done = false;
  assert(_queue[_queue_pos] == '_' ||
	 ('a' <= _queue[_queue_pos] && _queue[_queue_pos] <= 'z') ||
	 ('A' <= _queue[_queue_pos] && _queue[_queue_pos] <= 'Z'));
  const auto begin_loc = _queue_locs[_queue_pos];
  while (!done) {
    const auto cur_pos = _next_queued_non_continuation_pos(_queue_pos);
    const char cur = _queue[cur_pos];
    switch (cur) {
    case '_':
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '0' ... '9':
      {
	const auto &not_is_id_char =
	  [](const char c) {
	    return
	      !(c == '_' ||
		('0' <= c && c <= '9') ||
		('a' <= c && c <= 'z') ||
		('A' <= c && c <= 'Z'));
	  };

	assert(_cur_queue_window_end_pos_nowrap(cur_pos) >= cur_pos + 1);
	const auto next_poi_pos =
	  (std::find_if(&_queue[cur_pos + 1],
			&_queue[_cur_queue_window_end_pos_nowrap(cur_pos)],
			not_is_id_char)
	   - &_queue[0]);
	if (next_poi_pos != _queue_capacity) {
	  value.append(&_queue[cur_pos], &_queue[next_poi_pos]);
	  _advance_queue_to_pos(next_poi_pos);
	} else {
	  // The search stopped at queue ringbuffer wraparound. Continue
	  // the there to minimize allocations.
	  const unsigned int next_poi_pos_wrapped =
	    (std::find_if(&_queue[0],
			  &_queue[_cur_queue_window_end_pos_wrap()],
			  not_is_id_char)
	     - &_queue[0]);
	  value.reserve(value.size() +
			(next_poi_pos - cur_pos) +
			next_poi_pos_wrapped);
	  value.append(&_queue[cur_pos], &_queue[next_poi_pos]);
	  value.append(&_queue[0], &_queue[next_poi_pos_wrapped]);
	  _advance_queue_to_pos(next_poi_pos_wrapped);
	}
      }
      break;

    default:
      done = true;
    }
  }

  return raw_pp_token(pp_token::type::id, std::move(value),
		      range_in_file(begin_loc, _queue_locs[_queue_pos]));
}

std::size_t _pp_tokenizer::_skip_c_comment(const std::size_t n_trailing_spaces)
{
  assert(_queue[_queue_pos] == '/');
  const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
  assert(_queue[next_pos] == '*');
  std::size_t n_chars_in_last_line =
    ((next_pos == ((_queue_pos + 1) & (_queue_capacity - 1))) ?
     n_trailing_spaces + 2 :
     1);
  _advance_queue_to_pos(next_pos + 1);

  const char needle[] = { '\0', '\n', '*' };
  while (true) {
    const auto next_poi_pos =
      (std::find_first_of(&_queue[_queue_pos],
			  &_queue[_cur_queue_window_end_pos_nowrap()],
			  &needle[0],
			  &needle[3])
       - &_queue[0]);
    n_chars_in_last_line += next_poi_pos - _queue_pos;
    _advance_queue_to_pos(next_poi_pos);

    const auto cur = _queue[_queue_pos];
    if (cur != '\n' && cur != 0) {
      if (cur == '*') {
	const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
	if (_queue[next_pos] == '/') {
	  n_chars_in_last_line =
	    ((next_pos == ((_queue_pos + 1) & (_queue_capacity - 1))) ?
	     n_chars_in_last_line + 2 :
	     1);
	  _advance_queue_to_pos(next_pos + 1);
	  break;
	}
	// Either a '*' not followed by a '/' or the end of the
	// current search window had been hit above.
	++n_chars_in_last_line;
      }
    } else {
      assert(cur == '\n' || cur == 0);
      if (cur == 0 && _at_eof()) {
	throw this->report_fatal("EOF within C-style comment",
				 _queue_locs[_queue_pos]);
      }
      n_chars_in_last_line = 0;
    }
    _advance_queue_to_pos(_queue_pos + 1);
  }

  return n_chars_in_last_line;
}

void _pp_tokenizer::_skip_cpp_comment()
{
  assert(_queue[_queue_pos] == '/');
  const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
  assert(_queue[next_pos] == '/');
  _advance_queue_to_pos(next_pos + 1);

  const char needle[] = { '\0', '\n' };
  while(_queue[_queue_pos] != '\n') {
    const auto next_poi_pos =
      (std::find_first_of(&_queue[_queue_pos],
			  &_queue[_cur_queue_window_end_pos_nowrap()],
			  &needle[0],
			  &needle[2])
       - &_queue[0]);
    _advance_queue_to_pos(next_poi_pos);

    if (!_queue[_queue_pos] && _at_eof()) {
      this->report_warning("EOF within C++-style comment",
			   _queue_locs[_queue_pos]);
    }
  }
}

raw_pp_token _pp_tokenizer::_tokenize_ws()
{
  // According to the standard (5.1.1.2(3)):
  // - Each comment is replaced replaced by a single space.
  // - Newlines are retained.
  // - Implementations may replace whitespace sequences other than
  //   newlines with a single space.
  //
  // GNU cpp does the following:
  // At the beginning of lines:
  //   - Comments are transformed into a sequence of space characters
  //     equal in number to the number of comment's characters after its
  //     last newline.
  //   - Any whitespace character is transformed into a space.
  // At the end of lines:
  //   - Any space, including any arising from comments, is stripped.
  // In the middle of lines:
  //   - Comments are replaced with a single space
  //   - Sequences of whitespace are replaced with a single space.
  //
  // In addition, GNU CPP collapses multiple newlines into a single one.
  // We don't do this here.
  std::size_t n_spaces = 0;
  bool done = false;

  char cur = _queue[_queue_pos];
  {
    const char next = _queue[_next_queued_non_continuation_pos(_queue_pos + 1)];
    assert(cur == ' ' || cur == '\t' || cur == '\v' || cur == '\f' ||
	   (cur == '/' && (next == '*' || next == '/')));
  }
  const auto begin_loc = _queue_locs[_queue_pos];
  bool multispaces = false;
  while (!done) {
    cur = _queue[_queue_pos];
    switch (cur) {
    case '\n':
      // Whitespace at end of line, return a single newline.
      _advance_queue_to_pos(_queue_pos + 1);
      _expect_qh_str = expect_qh_str::newline;
      return raw_pp_token(pp_token::type::newline, "\n",
			  range_in_file(begin_loc, _queue_locs[_queue_pos]));

    case '/':
      {
	const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
	const char next = _queue[next_pos];
	if (next == '*') {
	  n_spaces = _skip_c_comment(n_spaces);
	} else if (next == '/') {
	  // Whitespace + comment at end of line, return a single newline.
	  _skip_cpp_comment();
	  assert(_at_eof() || _queue[_queue_pos] == '\n');
	  _advance_queue_to_pos(_queue_pos + 1);
	  _expect_qh_str = expect_qh_str::newline;
	  return raw_pp_token(pp_token::type::newline, "\n",
			      range_in_file(begin_loc,
					    _queue_locs[_queue_pos]));
	} else {
	  done = true;
	}
      }
      break;

    case ' ':
    case '\t':
    case '\v':
    case '\f':
      {
	assert(_queue_pos + 1 <= _cur_queue_window_end_pos_nowrap());
	const auto next_poi_pos =
	  (std::find_if(&_queue[_queue_pos + 1],
			&_queue[_cur_queue_window_end_pos_nowrap()],
			[](const char c) { return c != ' ' && c != '\t'; })
	   - &_queue[0]);
	n_spaces += next_poi_pos - _queue_pos;
	_advance_queue_to_pos(next_poi_pos);
      }
      break;

    default:
      done = true;
    }
  }

  return raw_pp_token(pp_token::type::ws,
	std::string(_expect_qh_str == expect_qh_str::newline ? n_spaces : 1,
		    ' '),
	range_in_file(begin_loc, _queue_locs[_queue_pos]));
}

raw_pp_token _pp_tokenizer::read_next_token()
{
  const auto begin_loc = _queue_locs[_queue_pos];
  switch (_queue[_queue_pos]) {
  case 0:
    if (!_at_eof()) {
      // It's a continuation line between tokens, return an empty
      // token for recording that.
      _advance_queue_to_pos(_queue_pos + 1);
      return raw_pp_token(pp_token::type::empty, "",
			  range_in_file(begin_loc, _queue_locs[_queue_pos]));
    } else {
      return raw_pp_token(pp_token::type::eof, "",
			  range_in_file(_queue_locs[_queue_pos],
					_queue_locs[_queue_pos]));
    }

  case '\n':
    {
      _expect_qh_str = expect_qh_str::newline;
      _advance_queue_to_pos(_queue_pos + 1);
      return raw_pp_token(pp_token::type::newline,
			  std::string(1, '\n'),
			  range_in_file(begin_loc, _queue_locs[_queue_pos]));
    }

  case ' ':
  case '\t':
  case '\v':
  case '\f':
    return _tokenize_ws();

  case '/':
    {
      const auto next =
	_queue[_next_queued_non_continuation_pos(_queue_pos + 1)];
      if (next == '*') {
	return _tokenize_ws();
      } else if (next == '/') {
	return _tokenize_ws();
      } else {
	_expect_qh_str = expect_qh_str::no;
	return _tokenize_punctuator();
      }
    }

  case '\'':
    _expect_qh_str = expect_qh_str::no;
    _advance_queue_to_pos(_queue_pos + 1);
    return _tokenize_string(begin_loc, '\'', true, pp_token::type::chr);

  case '\"':
    _advance_queue_to_pos(_queue_pos + 1);
    if (_expect_qh_str != expect_qh_str::expect_qh_str) {
      _expect_qh_str = expect_qh_str::no;
      return _tokenize_string(begin_loc, '"', true, pp_token::type::str);
    } else {
      _expect_qh_str = expect_qh_str::no;
      return _tokenize_string(begin_loc, '"', false, pp_token::type::qstr);
    }

  case 'L':
    {
      _expect_qh_str = expect_qh_str::no;
      const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
      const auto next = _queue[next_pos];
	if (next == '"') {
	  _advance_queue_to_pos(next_pos + 1);
	  return _tokenize_string(begin_loc, '"', true, pp_token::type::wstr);
	} else if (next == '\'') {
	  _advance_queue_to_pos(next_pos + 1);
	  return _tokenize_string(begin_loc, '\'', true, pp_token::type::wchr);
	} else {
	  return _tokenize_id();
	}
    }

  case 'u':
    {
      _expect_qh_str = expect_qh_str::no;
      const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
      const auto next = _queue[next_pos];
      if (next == '8') {
	const auto next_next_pos =
	  _next_queued_non_continuation_pos(next_pos + 1);
	const auto next_next = _queue[next_next_pos];
	if (next_next == '"') {
	  _advance_queue_to_pos(next_next_pos + 1);
	  return _tokenize_string(begin_loc, '"', true, pp_token::type::ustr8);
	} else {
	  return _tokenize_id();
	}
      } else if (next == '"') {
	_advance_queue_to_pos(next_pos + 1);
	return _tokenize_string(begin_loc, '"', true, pp_token::type::ustr16);
      } else if (next == '\'') {
	_advance_queue_to_pos(next_pos + 1);
	return _tokenize_string(begin_loc, '\'', true, pp_token::type::uchr16);
      } else {
	return _tokenize_id();
      }
    }

  case 'U':
    {
      _expect_qh_str = expect_qh_str::no;
      const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
      const auto next = _queue[next_pos];
      if (next == '"') {
	_advance_queue_to_pos(next_pos + 1);
	return _tokenize_string(begin_loc, '"', true, pp_token::type::ustr32);
      } else if (next == '\'') {
	_advance_queue_to_pos(next_pos + 1);
	return _tokenize_string(begin_loc, '\'', true, pp_token::type::uchr32);
      } else {
	return _tokenize_id();
      }
    }

  case '_':
  case 'a' ... 't':
  case 'v' ... 'z':
  case 'A' ... 'K':
  case 'M' ... 'T':
  case 'V' ... 'Z':
    {
      raw_pp_token tok = _tokenize_id();
      if (_expect_qh_str == expect_qh_str::sharp_seen &&
	  tok.get_value() == "include") {
	_expect_qh_str = expect_qh_str::expect_qh_str;
      } else {
	_expect_qh_str = expect_qh_str::no;
      }
      return tok;
    }

  case '0' ... '9':
    _expect_qh_str = expect_qh_str::no;
    return _tokenize_pp_number();

  case '.':
    {
      const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
      const auto next = _queue[next_pos];
      _expect_qh_str = expect_qh_str::no;
      if ('0' <= next && next <= '9') {
	return _tokenize_pp_number();
      } else {
	return _tokenize_punctuator();
      }
    }

  case '#':
    {
      const auto next_pos = _next_queued_non_continuation_pos(_queue_pos + 1);
      const auto next = _queue[next_pos];
      if (_expect_qh_str == expect_qh_str::newline && next != '#') {
	_expect_qh_str = expect_qh_str::sharp_seen;
      } else {
	_expect_qh_str = expect_qh_str::no;
      }
      return _tokenize_punctuator();
    }

  case '<':
    if (_expect_qh_str != expect_qh_str::expect_qh_str) {
      _expect_qh_str = expect_qh_str::no;
      return _tokenize_punctuator();
    } else {
      _expect_qh_str = expect_qh_str::no;
      _advance_queue_to_pos(_queue_pos + 1);
      return _tokenize_string(begin_loc, '>', false, pp_token::type::hstr);
    }

  case '[':
  case ']':
  case '(':
  case ')':
  case '{':
  case '}':
  case '-':
  case '+':
  case '&':
  case '*':
  case '~':
  case '!':
  case '%':
  case '>':
  case '=':
  case '^':
  case '|':
  case '?':
  case ':':
  case ';':
  case ',':
    _expect_qh_str = expect_qh_str::no;
    return _tokenize_punctuator();

  default:
    {
      const char cur = _queue[_queue_pos];
      _expect_qh_str = expect_qh_str::no;
      _advance_queue_to_pos(_queue_pos + 1);
      return raw_pp_token(pp_token::type::non_ws_char,
			  std::string(1, cur),
			  range_in_file(begin_loc, _queue_locs[_queue_pos]));
    }
  }
  // unreachable
}


pp_tokenizer::pp_tokenizer(pp_result::header_inclusion_node &file)
  : _file(file), _sr(_file.create_source_reader())
{
  init_state();
}

pp_tokenizer::~pp_tokenizer() noexcept = default;

raw_pp_token pp_tokenizer::read_next_token()
{
  return impl::_pp_tokenizer::read_next_token();
}

_pp_tokenizer::buffer_type pp_tokenizer::read_raw()
{
  return _sr->read();
}

void pp_tokenizer::add_line(const std::streamoff length)
{
  _file.add_line(length);
}

void pp_tokenizer::report_warning(const std::string &msg,
				  const range_in_file::loc_type loc)
{
  _remarks.add(code_remark(code_remark::severity::warning,
			   msg, _file, range_in_file{loc}));
}

pp_except pp_tokenizer::report_fatal(const std::string &msg,
				     const range_in_file::loc_type loc)
{
  code_remark remark{code_remark::severity::fatal,
		     msg, _file, range_in_file{loc}};
  _remarks.add(remark);
  return pp_except{remark};
}


pp_string_tokenizer::
pp_string_tokenizer(const std::string &s,
		    const report_warning_type &report_warning,
		    const report_fatal_type &report_fatal)
  : _buf(s.begin(), s.end()),
    _report_warning(report_warning), _report_fatal(report_fatal)
{
  init_state();
}

pp_string_tokenizer::~pp_string_tokenizer() noexcept = default;

raw_pp_token pp_string_tokenizer::read_next_token()
{
  return impl::_pp_tokenizer::read_next_token();
}

raw_pp_tokens pp_string_tokenizer::tokenize_builtin(const std::string &s)
{
  pp_string_tokenizer tokenizer{s,
				[](const std::string&) {
				  assert(0);
				  __builtin_unreachable();
				},
				[](const std::string&) -> pp_except {
				  assert(0);
				  __builtin_unreachable();
				}};

  raw_pp_tokens result;
  while (true) {
    raw_pp_token tok = tokenizer.read_next_token();
    if (tok.is_eof())
      break;

    result.push_back(std::move(tok));
  }

  return result;
}

_pp_tokenizer::buffer_type pp_string_tokenizer::read_raw()
{
  return std::move(_buf);
}

void pp_string_tokenizer::add_line(const std::streamoff)
{}

void pp_string_tokenizer::report_warning(const std::string &msg,
					 const range_in_file::loc_type)
{
  _report_warning(msg);
}

pp_except pp_string_tokenizer::report_fatal(const std::string &msg,
					    const range_in_file::loc_type)
{
  return _report_fatal(msg);
}

