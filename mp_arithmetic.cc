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
#include <algorithm>
#include <array>
#include "mp_arithmetic.hh"

using namespace klp::ccp::mpa;

const unsigned int limb::width;

unsigned int limb::fls() const noexcept
{
  // Width must be a power of two
  static_assert((width & -width) == width,
		"limb width must be a power of two");
  if (!_value)
    return 0;

  unsigned int r = 1;
  limb_type tmp = _value;
  unsigned int test_width = width / 2;

  while (test_width) {
    if (tmp >> test_width) {
      tmp >>= test_width;
      r += test_width;
    }

    test_width /= 2;
  }

  return r;
}

unsigned int limb::ffs() const noexcept
{
  // Clear any bits but the least significant one
  limb_type tmp = _value & (-_value);

  return limb{tmp}.fls();
}

unsigned int limb::clz() const noexcept
{
  return width - fls();
}

unsigned int limb::clrsb() const noexcept
{
  if (_value & (static_cast<limb_type>(1) << (width - 1))) {
    return (~*this).clz() - 1;
  } else {
    return clz() - 1;
  }
}

bool limb::add(const bool op) noexcept
{
  _value += op;
  return op && !_value;
}

bool limb::add(const limb &op) noexcept
{
  _value += op._value;
  return _value < op._value;
}

bool limb::sub(const bool op) noexcept
{
  _value -= op;
  return op && !~_value;
}

bool limb::sub(const limb &op) noexcept
{
  const limb_type old_value = _value;
  _value -= op._value;
  return old_value < _value;
}

const double_limb limb::operator*(const limb &op) const noexcept
{
  static_assert(width % 2 == 0,
		"not implemented: limb_type width must be even");
  const unsigned int half_width = width / 2;
  const limb_type low_half_mask = mask(half_width)._value;

  limb low((_value & low_half_mask) * (op._value & low_half_mask));
  limb high((_value >> half_width) * (op._value >> half_width));

  limb_type tmp = (_value & low_half_mask) * (op._value >> half_width);
  bool carry = low.add(limb(tmp << half_width));
  bool high_overflow = high.add(carry);
  assert(!high_overflow);
  high_overflow = high.add(limb(tmp >> half_width));
  assert(!high_overflow);

  tmp = ((_value >> half_width) * (op._value & low_half_mask));
  carry = low.add(limb(tmp << half_width));
  high_overflow = high.add(carry);
  assert(!high_overflow);
  high_overflow = high.add(limb(tmp >> half_width));
  assert(!high_overflow);

  return double_limb(high, low);
}

const limb limb::reverse(const unsigned int rev_unit_width_log2)
  const noexcept
{
  assert((1u << rev_unit_width_log2) < width);
  limb_type r = _value;
  limb_type m = ~static_cast<limb_type>(0);
  for (unsigned int d = width / 2; d >= (1u << rev_unit_width_log2); d >>= 1) {
    m ^= m << d;
    r = ((r & m) << d) | ((r >> d) & m);
  }

  return limb{r};
}

bool double_limb::add(const limb &op) noexcept
{
  const bool carry = _low.add(op);
  return _high.add(carry);
}

limb double_limb::div(const limb divisor)
{
  if (!divisor)
    throw std::invalid_argument("division by zero");

  static_assert(limb::width % 2 == 0,
		"not implemented: limb_type width must be even");

  const unsigned int half_width = limb::width / 2;
  const limb::limb_type low_half_mask = limb::mask(half_width).value();

  // Division algorithm according to D. E. Knuth,
  // c.f. limbs::operator/() for the specific case of four half limbs.
  // The below arrays' element types are all of time limb_type
  // but they actually represent half limbs.
  std::array<limb::limb_type, 3> v{{
    divisor.value() & low_half_mask,
    divisor.value() >> half_width,
    0, // convenience v[n]
  }};

  std::array<limb::limb_type, 5> u{{
    _low.value() & low_half_mask,
    _low.value() >> half_width,
    _high.value() & low_half_mask,
    _high.value() >> half_width,
    0,
  }};

  const unsigned int n = v[1] ? 2 : 1;
  const unsigned int m = 4 - n;

  if (n == 1) {
    std::array<limb::limb_type, 4> q{{0}};
    limb::limb_type r = 0;
    for (unsigned int __j = m + 1; __j > 0; --__j) {
      const unsigned int j = __j - 1;
      limb::limb_type tmp = (r << half_width) | u[j];
      r = tmp % v[0];
      tmp /= v[0];
      assert(!(tmp >> half_width));
      q[j] = tmp;
    }

    _low = limb((q[1] << half_width) | q[0]);
    _high = limb((q[3] << half_width) | q[2]);

    return limb(r);
  }

  // Calculate scaling b / (v_{n-1} + 1)
  const limb::limb_type b = static_cast<limb::limb_type>(1) << half_width;
  const limb::limb_type d = b / (v[n - 1] + 1);
  limb::limb_type carry = 0;

  for (auto &hl : v) {
    hl *= d;
    hl += carry;
    carry = hl >> half_width;
    hl &= low_half_mask;
  }
  assert(!carry);
  assert(!v[2]);
  assert(!v[n]);

  for (auto &hl : u) {
    hl *= d;
    hl += carry;
    carry = hl >> half_width;
    hl &= low_half_mask;
  }
  assert(!carry);

  // The normalization procedure guarantees that v_{n-1} >= floor(b/2)
  // and u_{m + n} < floor(b/2).
  assert(u[m + n] < v [n - 1]);

  std::array<limb::limb_type, 4> q{{0}};
  for (unsigned int __j = m + 1; __j > 0; --__j) {
    const unsigned int j = __j - 1;
    bool recheck = true;
    limb::limb_type _q = (u[j + n] << half_width) | (u[j + n - 1]);
    limb::limb_type r = _q % v[n - 1];
    _q /= v[n - 1];
    if (_q >> half_width) {
      // q >= b. It follows that u[j + n] >= v[n - 1].
      // Because u[j + n:j] / v < b, this means that
      assert(u[j + n] == v[n - 1]);
      _q = b - 1;
      r = u[j + n - 1] + v[n - 1];
      recheck = !(r >> half_width);
    }

    while (recheck) {
      const limb::limb_type tmp = _q * v [n - 2];
      if ((tmp >> half_width) > r ||
	  ((tmp >> half_width) == r && (tmp & low_half_mask) > u[j + n - 2])) {
	--_q;
	// An overflow will occur after <= 2 iterations.
	r += v[n - 1];
	recheck = !(r >> half_width);
      } else {
	recheck = false;
      }
    }

    // Multiply and subtract
    limb::limb_type borrow = 0;
    for (unsigned int i = 0; i <= n; ++i) {
      const limb::limb_type prod = v[i] * _q;
      assert(!(borrow >> half_width));
      assert(!(u[j + i] >> half_width));
      limb::limb_type hlu = u[j + i] - borrow;
      hlu -= (prod & low_half_mask);
      borrow = ((static_cast<limb::limb_type>(1) << half_width)
		- (hlu >> half_width));
      borrow &= low_half_mask;
      assert(borrow <= 2);
      hlu &= low_half_mask;
      borrow += (prod >> half_width);
      assert(!(borrow >> half_width));
      u[j + i] = hlu;
    }

    if (borrow) {
      assert(borrow == 1);
      // q is one too large, add v back.
      --_q;
      bool _carry = false;
      for (unsigned int i = 0; i <= n; ++i) {
	limb::limb_type hlresult = u[j + i];
	hlresult += _carry;
	hlresult += v[i];
	_carry = (hlresult >> half_width) ? true : false;
	hlresult &= low_half_mask;
	u[j + i] = hlresult;
      }
      // _carry cancels borrow
      assert(_carry);
    }

    q[j] = _q;
  }

  // Denormalize the remainder
  limb::limb_type r = 0;
  for (unsigned int __j = n; __j > 0; --__j) {
    const unsigned int j = __j - 1;
    limb::limb_type tmp = (r << half_width) | u[j];
    r = tmp % d;
    tmp /= d;
    assert(!(tmp >> half_width));
    u[j] = tmp;
  }
  assert(!r);
#ifndef NDEBUG
  for (unsigned int j = n; j < u.size(); ++j)
    assert(!u[j]);
#endif

  _low = limb((q[1] << half_width) | q[0]);
  _high = limb((q[3] << half_width) | q[2]);

  return limb((u[1] << half_width) | u[0]);
}

limbs::limbs() = default;

limbs::limbs(const std::initializer_list<limb::limb_type> &il)
{
  _limbs.reserve(il.size());
  for (auto v : il)
    _limbs.emplace_back(v);
}

limbs::limbs(const std::initializer_list<limb> &il)
  : _limbs(il)
{}

limbs::limbs(const size_type n)
  : limbs(from_size_type(n))
{}

limbs::limbs(const limbs &ls)
  : _limbs(ls._limbs)
{}

limbs::limbs(limbs &&ls) noexcept
  : _limbs(std::move(ls._limbs))
{}

limbs::limbs(_limbs_type &&ls) noexcept
  : _limbs(std::move(ls))
{}

limbs& limbs::operator=(const limbs &rhs)
{
  if (this != &rhs)
    _limbs = rhs._limbs;

  return *this;
}

bool limbs::operator==(const limbs &op) const noexcept
{
  const size_type n = std::min(size(), op.size());
  for (size_type i = 0; i < n; ++i) {
    if (_limbs[i] != op[i])
      return false;
  }

  for (size_type i = n; i < size(); ++i) {
    if (_limbs[i])
      return false;
  }

  for (size_type i = n; i < op.size(); ++i) {
    if (op[i])
      return false;
  }

  return true;
}

bool limbs::operator<(const limbs &op) const noexcept
{
  const size_type min_n = std::min(size(), op.size());

  for (size_type i = min_n; i < size(); ++i) {
    if (_limbs[i])
      return false;
  }

  for (size_type i = min_n; i < op.size(); ++i) {
    if (op[i])
      return true;
  }

  for (size_type __i = min_n; __i > 0; --__i) {
    const size_type i = __i - 1;
    if (_limbs[i] < op[i])
      return true;
    else if (_limbs[i] > op[i])
      return false;
  }

  return false;
}


bool limbs::operator<=(const limbs &op) const noexcept
{
  const size_type min_n = std::min(size(), op.size());

  for (size_type i = min_n; i < size(); ++i) {
    if (_limbs[i])
      return false;
  }

  for (size_type i = min_n; i < op.size(); ++i) {
    if (op[i])
      return true;
  }

  for (size_type __i = min_n; __i > 0; --__i) {
    const size_type i = __i - 1;
    if (_limbs[i] < op[i])
      return true;
    else if (_limbs[i] > op[i])
      return false;
  }

  return true;
}

limbs::operator bool() const noexcept
{
  return std::any_of(_limbs.begin(), _limbs.end(),
		     [](const limb &l) { return !!l; });
}

limbs limbs::complement() const
{
  _limbs_type result;
  result.reserve(size());
  bool carry = true;
  for (const limb &_l : _limbs) {
    limb l = ~_l;
    carry = l.add(carry);
    result.emplace_back(l);
  }

  return limbs(std::move(result));
}

limbs limbs::operator+(const limbs &op) const
{
  const size_type max_n = std::max(size(), op.size());
  const size_type min_n = std::min(size(), op.size());

  _limbs_type result;
  result.reserve(max_n + 1);

  bool carry = false;
  for (size_type i = 0; i < min_n; ++i) {
    limb l = _limbs[i];
    carry = l.add(carry);
    carry |= l.add(op[i]);
    result.emplace_back(l);
  }

  for (size_type i = min_n; i < size(); ++i) {
    limb l  = _limbs[i];
    carry = l.add(carry);
    result.emplace_back(l);
  }

  for (size_type i = min_n; i < op.size(); ++i) {
    limb l = op[i];
    carry = l.add(carry);
    result.emplace_back(l);
  }

  if (carry)
    result.emplace_back(carry);

  return limbs(std::move(result));
}

limbs limbs::operator+(size_type op) const
{
  const std::size_t szt_width = std::numeric_limits<size_type>::digits;
  const std::size_t szt_limb_mask =
    (szt_width > limb::width ?
     ((static_cast<std::size_t>(1) << limb::width) - 1) :
     (~(static_cast<std::size_t>(0))));

  _limbs_type result;
  result.reserve(std::max(width_to_size(szt_width), size()) + 1);

  bool carry = false;
  for (size_type i = 0; i < size(); ++i) {
    limb l{static_cast<limb::limb_type>(op & szt_limb_mask)};
    if (szt_width > limb::width)
      op >>= limb::width;
    else
      op = 0;
    carry = l.add(carry);
    carry |= l.add(_limbs[i]);
    result.emplace_back(l);
  }

  while (op) {
    assert(width_to_size(szt_width) > size());

    limb l{static_cast<limb::limb_type>(op & szt_limb_mask)};
    if (szt_width > limb::width)
      op >>= limb::width;
    else
      op = 0;
    carry = l.add(carry);
    result.emplace_back(l);
  }

  if (carry)
    result.emplace_back(carry);

  return limbs(std::move(result));
}

limbs& limbs::operator+=(const limbs &op)
{
  const size_type min_n = std::min(size(), op.size());

  bool carry = false;
  for (size_type i = 0; i < min_n; ++i) {
    const bool _carry = carry;
    carry = _limbs[i].add(op[i]);
    carry |= _limbs[i].add(_carry);
  }

  if (min_n < size()) {
    for (size_type i = min_n; i < size(); ++i)
      carry = _limbs[i].add(carry);

  } else {
    _limbs.reserve(op.size() + 1);
    for (size_type i = min_n; i < op.size(); ++i) {
      limb l = op[i];
      carry = l.add(carry);
      _limbs.emplace_back(l);
    }

  }

  if (carry)
    _limbs.emplace_back(carry);

  return *this;
}

limbs& limbs::operator+=(const limb op)
{
  if (_limbs.empty()) {
    _limbs.emplace_back(op);
    return *this;
  }

  bool carry = _limbs[0].add(op);
  for (size_type i = 1; i < size(); ++i)
    carry = _limbs[i].add(carry);

  if (carry)
    _limbs.emplace_back(carry);

  return *this;
}

limbs& limbs::operator+=(size_type op)
{
  const std::size_t szt_width = std::numeric_limits<size_type>::digits;
  const std::size_t szt_limb_mask =
    (szt_width > limb::width ?
     ((static_cast<std::size_t>(1) << limb::width) - 1) :
     (~(static_cast<std::size_t>(0))));

  bool carry = false;
  for (size_type i = 0; i < size(); ++i) {
    limb l{static_cast<limb::limb_type>(op & szt_limb_mask)};
    if (szt_width > limb::width)
      op >>= limb::width;
    else
      op = 0;
    carry = _limbs[i].add(carry);
    carry |= _limbs[i].add(l);
  }

  while (op) {
    assert(width_to_size(szt_width) > size());

    limb l{static_cast<limb::limb_type>(op & szt_limb_mask)};
    if (szt_width > limb::width)
      op >>= limb::width;
    else
      op = 0;
    carry = l.add(carry);
    _limbs.emplace_back(l);
  }

  if (carry)
    _limbs.emplace_back(carry);

  return *this;
}

limbs limbs::operator-(const limbs &op) const
{
  const size_type max_n = std::max(size(), op.size());
  const size_type min_n = std::min(size(), op.size());

  _limbs_type result;
  result.reserve(max_n + 1);

  bool borrow = false;
  for (size_type i = 0; i < min_n; ++i) {
    limb l = _limbs[i];
    borrow = l.sub(borrow);
    borrow |= l.sub(op[i]);
    result.emplace_back(l);
  }

  for (size_type i = min_n; i < size(); ++i) {
    limb l  = _limbs[i];
    borrow = l.sub(borrow);
    result.emplace_back(l);
  }

  for (size_type i = min_n; i < op.size(); ++i) {
    limb l = limb(0);
    borrow = l.sub(borrow);
    borrow |= l.sub(op[i]);
    assert(borrow || !op[i]);
    result.emplace_back(l);
  }

  if (borrow)
    result.emplace_back(~limb(0));

  return limbs(std::move(result));
}

limbs& limbs::operator-=(const limb op)
{
  if (_limbs.empty()) {
    _limbs.reserve(2);
    limb l = limb(0);
    const bool borrow = l.sub(op);
    assert(borrow || !op);
    _limbs.emplace_back(l);
    if (borrow)
      _limbs.emplace_back(~limb(0));
  }

  bool borrow = _limbs[0].sub(op);
  for (size_type i = 1; i < size(); ++i)
    borrow = _limbs[i].sub(borrow);

  if (borrow)
    _limbs.emplace_back(~limb(0));

  return *this;
}

limbs limbs::operator*(const limbs &op) const
{
  const size_type m = size();
  const size_type n = op.size();
  _limbs_type result(m, limb(0));
  result.reserve(m + n);

  for (size_type j = 0; j < n; ++j) {
    limb carry(0);
    for (size_type i = 0; i < m; ++i) {
      double_limb t = _limbs[i] * op[j];
      bool t_high_overflow = t.add(carry);
      assert(!t_high_overflow);
      t_high_overflow = t.add(result[i + j]);
      assert(!t_high_overflow);
      result[i + j] = t.low();
      carry = t.high();
    }
    result.emplace_back(carry);
  }

  return limbs(std::move(result));
}

limbs limbs::operator*(const limb op) const
{
  const size_type m = size();
  _limbs_type result;
  result.reserve(m + 1);

  limb carry(0);
  for (size_type i = 0; i < m; ++i) {
    double_limb t = _limbs[i] * op;
    bool t_high_overflow = t.add(carry);
    assert(!t_high_overflow);
    result.emplace_back(t.low());
    carry = t.high();
  }
  if (carry)
    result.emplace_back(carry);

  return limbs(std::move(result));
}

limbs& limbs::operator*=(const limbs &op)
{
  if (this == &op) {
    // In-place squaring.
    const size_type m = size();
    _limbs.resize(2 * m);

    for (size_type i = m; i > 0; --i) {
      const limb l = _limbs[i - 1];
      _limbs[i - 1] = limb{0};

      limb carry_low(0);
      bool carry_high = false;
      for (size_type j = 0; j < i - 1; ++j) {
	double_limb t = l * _limbs[j];
	// Multiply by 2. Store the shifted out high bit first.
	bool t_highbit = !!(t.high() >> (limb::width - 1));
	t = double_limb{(t.high() << 1) | (t.low() >> (limb::width - 1)),
			t.low() << 1};

	bool t_high_overflow = t.add(carry_low);
	assert(!t_high_overflow || !t_highbit);
	t_highbit |= t_high_overflow;
	t_high_overflow = t.add(_limbs[i - 1 + j]);
	assert(!t_high_overflow || !t_highbit);
	t_highbit |= t_high_overflow;

	_limbs[i - 1 + j] = t.low();

	carry_low = t.high();
	carry_high = carry_low.add(carry_high);
	assert(!carry_high || !t_highbit);
	carry_high |= t_highbit;
      }

      double_limb t = l * l;
      bool t_high_overflow = t.add(carry_low);
      assert(!t_high_overflow);
      t_high_overflow = t.add(_limbs[2 * i - 2]);
      assert(!t_high_overflow);

      _limbs[2 * i - 2] = t.low();

      carry_low = t.high();
      carry_high = carry_low.add(carry_high);

      // Propagate the carry to high.
      carry_low = limb{_limbs[2 * i - 1].add(carry_low)};
      carry_high = carry_low.add(carry_high);
      assert(!carry_high);
      for (size_type k = 2 * i; carry_low && k < 2 * m; ++k)
	carry_low = limb{_limbs[k].add(carry_low)};

      assert(!carry_low);
    }

    return *this;
  }

  const size_type m = size();
  const size_type n = op.size();
  _limbs.resize(m + n);

  for (size_type i = m; i > 0; --i) {
    const limb l = _limbs[i - 1];
    _limbs[i - 1] = limb{0};
    limb carry(0);
    for (size_type j = 0; j < n; ++j) {
      double_limb t = l * op[j];
      bool t_high_overflow = t.add(carry);
      assert(!t_high_overflow);
      t_high_overflow = t.add(_limbs[i - 1 + j]);
      assert(!t_high_overflow);
      _limbs[i - 1 + j] = t.low();
      carry = t.high();
    }

    // Propagate the carry to high.
    bool _carry = _limbs[i - 1 + n].add(carry);
    for (size_type k = i + n; _carry && k < m + n; ++k)
      _carry = _limbs[k].add(_carry);
    assert(!_carry);
  }

  return *this;
}

limbs& limbs::operator*=(const limb op)
{
  const size_type m = size();

  limb carry{0};
  for (size_t i = 0; i < m; ++i) {
    double_limb t = _limbs[i] * op;
    bool t_high_overflow = t.add(carry);
    assert(!t_high_overflow);
    _limbs[i] = t.low();
    carry = t.high();
  }

  if (carry)
    _limbs.emplace_back(carry);

  return *this;
}

std::pair<limbs, limbs>
limbs::operator/(const limbs &divisor) const
{
  // Find the divisor's highest non-zero limb.
  size_type n;
  for (n = divisor.size(); n > 0; --n) {
    if (divisor[n - 1])
      break;
  }

  if (!n)
    throw std::invalid_argument("division by zero");

  if (size() < n) {
    return std::make_pair(limbs{}, *this);
  }
  const size_type m = size() - n;

  // Division algorithm from D. E. Knuth: "The Art of Computer
  // Programming", sec. 4.3.1 ("The classical algorithms").
  //
  // Notation:
  // u = dividend
  // v = divisor
  // q = resulting quotient
  // b = base, i.e. 1 << limb::width

  // Calculate scaling b / (v_{n-1} + 1)
  limb d(0);
  if (!~divisor[n - 1]) {
    // All bits set: v_{n-1} == b - 1;
    d = limb(1);
  } else {
    const limb::limb_type &v_high = divisor[n - 1].value();
    limb::limb_type dv = (~limb(0)).value() / (v_high + 1);
    if ((~limb(0)).value() - dv * (v_high + 1) == v_high)
      ++dv;
     d = limb(dv);
  }

  _limbs_type v;
  v.reserve(n);
  limb carry(0);
  for (size_type i = 0; i < n; ++i) {
    double_limb prod = divisor[i] * d;
    const bool prod_overflow = prod.add(carry);
    assert(!prod_overflow);
    v.emplace_back(prod.low());
    carry = prod.high();
  }
  assert(!carry);

  _limbs_type u;
  u.reserve(size() + 1);
  carry = limb(0);
  for (auto &l : _limbs) {
    double_limb prod = l * d;
    const bool prod_overflow = prod.add(carry);
    assert(!prod_overflow);
    u.emplace_back(prod.low());
    carry = prod.high();
  }
  // The normalization procedure guarantees that v_{n-1} >= floor(b/2)
  // and u_{m + n} < floor(b/2).
  assert(carry < v.back());
  u.emplace_back(carry);

  // Add a zero limb to v for convenience in the multiply and subtract
  // step below.
  v.emplace_back(limb(0));

  _limbs_type q(m + 1, limb(0));
  for (size_type __j = m + 1; __j > 0; --__j) {
    const size_type j = __j - 1;
    bool recheck = true;
    double_limb _q(u[j + n], u[j + n - 1]);
    limb r = _q.div(v[n - 1]);
    if (_q.high()) {
      // q >= b. It follows that u[j + n] >= v[n - 1].
      // Because u[j + n:j] / v < b, this means that
      assert(u[j + n] == v[n - 1]);
      _q.high() = limb(0);
      _q.low() = ~limb(0);
      r = u[j + n - 1];
      recheck = !r.add(v[n - 1]);
    }
    while (recheck) {
      const double_limb tmp = _q.low() * v[n - 2];
      if (tmp.high() > r ||
	  (tmp.high() == r && tmp.low() > u[j + n - 2])) {
	--_q.low();
	// An overflow will occur after <= 2 iterations.
	recheck = !r.add(v[n - 1]);
      } else {
	recheck = false;
      }
    }

    // Multiply and subtract
    limb borrow(0);
    for (size_type i = 0; i <= n; ++i) {
      const double_limb prod = v[i] * _q.low();
      limb lu = u[j + i];
      limb::limb_type _borrow = lu.sub(borrow);
      _borrow += lu.sub(prod.low());
      assert(_borrow <= 2);
      borrow = limb(_borrow);
      const bool borrow_overflow = borrow.add(prod.high());
      assert(!borrow_overflow);
      u[j + i] = lu;
    }

    if (borrow) {
      assert(borrow == limb(1));
      // q is one too large, add v back.
      --_q.low();
      bool _carry = false;
      for (size_type i = 0; i <= n; ++i) {
	limb lresult = u[j + i];
	_carry = lresult.add(_carry);
	_carry |= lresult.add(v[i]);
	u[j + i] = lresult;
      }
      // _carry cancels borrow
      assert(_carry);
    }

    q[j] = _q.low();
  }

  v.clear();

  // Denormalize the remainder
  limb r(0);
  for (size_type __j = n; __j > 0; --__j) {
    const size_type j = __j - 1;
    double_limb tmp(r, u[j]);
    r = tmp.div(d);
    assert(!tmp.high());
    u[j] = tmp.low();
  }
  assert(!r);

#ifndef NDEBUG
  for (size_type j = n; j < u.size(); ++j)
    assert(!u[j]);
#endif
  u.resize(n);

  return std::make_pair(limbs(std::move(q)), limbs(std::move(u)));
}

limbs limbs::operator~() const
{
  _limbs_type result;
  result.reserve(size());
  for (const auto &l : _limbs)
    result.emplace_back(~l);

  return limbs(std::move(result));
}

limbs limbs::operator^(const limbs &op) const
{
  const size_type max_n = std::max(size(), op.size());
  const size_type min_n = std::min(size(), op.size());

  _limbs_type result;
  result.reserve(max_n);

  for (size_type i = 0; i < min_n; ++i) {
    result.emplace_back(_limbs[i].value() ^ op[i].value());
  }

  for (size_type i = min_n; i < size(); ++i) {
    result.emplace_back(_limbs[i]);
  }

  for (size_type i = min_n; i < op.size(); ++i) {
    result.emplace_back(op[i]);
  }

  return limbs(std::move(result));
}

limbs limbs::operator&(const limbs &op) const
{
  const size_type max_n = std::max(size(), op.size());
  const size_type min_n = std::min(size(), op.size());

  _limbs_type result;
  result.reserve(max_n);

  for (size_type i = 0; i < min_n; ++i) {
    result.emplace_back(_limbs[i].value() & op[i].value());
  }

  for (size_type i = min_n; i < max_n; ++i) {
    result.emplace_back(0);
  }

  return limbs(std::move(result));
}

limbs limbs::operator|(const limbs &op) const
{
  const size_type max_n = std::max(size(), op.size());
  const size_type min_n = std::min(size(), op.size());

  _limbs_type result;
  result.reserve(max_n);

  for (size_type i = 0; i < min_n; ++i) {
    result.emplace_back(_limbs[i].value() | op[i].value());
  }

  for (size_type i = min_n; i < size(); ++i) {
    result.emplace_back(_limbs[i]);
  }

  for (size_type i = min_n; i < op.size(); ++i) {
    result.emplace_back(op[i]);
  }

  return limbs(std::move(result));
}

limbs limbs::lshift(const size_type distance) const
{
  if (!distance || empty())
    return *this;

  _limbs_type result;
  result.resize(size());

  if (distance >= width()) {
    for (auto &l : result)
      l = limb(0);
    return limbs(std::move(result));
  }

  // Offset to the src limb where the dest limb gets its low order
  // bits from.
  const size_type limb_offset = width_to_size(distance);
  assert(limb_offset >= 1);

  const size_type low_bits = distance % limb::width;
  if (low_bits != 0) {
    for (size_type __j = size(); __j > limb_offset; --__j) {
      const size_type j = __j - 1;
      const limb::limb_type value =
	((_limbs[j - limb_offset].value() >> (limb::width - low_bits)) |
	 (_limbs[j - limb_offset + 1].value() << low_bits));
      result[j] = limb(value);
    }
    result[limb_offset - 1] = limb(_limbs[0].value() << low_bits);
  } else {
    for (size_type j = size() - 1; j >= limb_offset; --j) {
      result[j] = _limbs[j - limb_offset];
    }
    result[limb_offset - 1] = limb(0);
  }

  for (size_type __j = limb_offset - 1; __j > 0; --__j) {
    const size_type j = __j - 1;
    result[j] = limb(0);
  }

  return limbs(std::move(result));
}

limbs limbs::rshift(const size_type distance, const bool fill_value) const
{
  if (!distance || empty())
    return *this;

  _limbs_type result;
  result.resize(size());

  const limb::limb_type fill = fill_value ? ~limb(0).value() : 0;

  if (distance >= width()) {
    for (auto &l : result)
      l = limb(fill);
    return limbs(std::move(result));
  }

  // Offset to the src limb where the dest limb gets its low order
  // bits from.
  const size_type limb_offset = distance / limb::width;
  const size_type high_bits = distance % limb::width;
  if (high_bits != 0) {
    for (size_type j = 0; j + limb_offset + 1 < size(); ++j) {
      const limb::limb_type value =
	((_limbs[j + limb_offset].value() >> high_bits |
	  _limbs[j + limb_offset + 1].value() << (limb::width - high_bits)));
      result[j] = limb(value);
    }
    result[size() - limb_offset - 1] =
      limb((_limbs.back().value() >> high_bits) |
	   (fill << (limb::width - high_bits)));
  } else {
    for (size_type j = 0; j + limb_offset < size(); ++j)
      result[j] = _limbs[j + limb_offset];
  }

  for (size_type j = size() - limb_offset; j < size(); ++j)
    result[j] = limb(fill);

  return limbs(std::move(result));
}

limbs limbs::add_signed(const mpa::limbs &op) const
{
  // Infinite precision arithmetic where each limb is taken modulo
  // 2^{its width}.
  const mpa::limbs::size_type w1 = width();
  const mpa::limbs::size_type w2 = op.width();

  if (!w1)
    return op;
  else if (!w2)
    return *this;

  mpa::limbs op1 = *this;
  mpa::limbs op2 = op;
  const bool op1_is_negative = op1.test_bit(w1 - 1);
  const bool op2_is_negative = op2.test_bit(w2 - 1);
  const mpa::limbs::size_type max_w = std::max(w1, w2);
  if (w1 < max_w) {
    op1.resize(width_to_size(max_w));
    op1.set_bits_at_and_above(w1, op1_is_negative);
  } else if (w2 < max_w) {
    op2.resize(width_to_size(max_w));
    op2.set_bits_at_and_above(w2, op2_is_negative);
  }

  mpa::limbs r = op1 + op2;
  if (!op1_is_negative && !op2_is_negative) {
    assert(r.width() == max_w);
    if (r.test_bit(max_w - 1)) {
      // Preserve room for sign bit
      r.resize(width_to_size(max_w) + 1);
    } else {
      // Shrink if possible
      r.resize(width_to_size(r.width() - r.clrsb()));
    }

  } else if (op1_is_negative && op2_is_negative) {
    assert(r.width() > max_w);
    assert(r.test_bit(max_w));
    assert(!r.is_any_set_at_or_above(max_w + 1));
    // Propagate sign to high.
    r.set_bits_at_and_above(max_w, true);
    // Shrink if possible
    r.resize(width_to_size(r.width() - r.clrsb()));

  } else {
    assert(!r.is_any_set_at_or_above(max_w + 1));
    if (r.width() > max_w) {
      // Do the modulo operation.
      r.set_bit(max_w, false);
    }
    r.resize(width_to_size(r.width() - r.clrsb()));
  }

  return r;
}

void limbs::set_bits_at_and_above(const size_type pos, const bool value)
  noexcept
{
  size_type i = pos / limb::width;

  const unsigned int preserved_bits = pos % limb::width;
  if (preserved_bits) {
    if (!value) {
      _limbs[i] = limb(_limbs[i].value() & limb::mask(preserved_bits).value());
    } else {
      _limbs[i] = limb(_limbs[i].value() |
		       ((~limb(0)).value() << preserved_bits));
    }
    ++i;
  }

  const limb fill = value ? ~limb(0) : limb(0);
  for (; i < size(); ++i)
    _limbs[i] = fill;
}

void limbs::set_bits_below(const size_type pos, const bool value) noexcept
{
  assert(pos <= width());
  size_type i = pos / limb::width;
  const unsigned int modified_bits = pos % limb::width;

  if (modified_bits) {
    if (!value) {
      _limbs[i] = limb(_limbs[i].value() & ~limb::mask(modified_bits).value());
    } else {
      _limbs[i] = limb(_limbs[i].value() | limb::mask(modified_bits).value());
    }
  }

  const limb fill = value ? ~limb(0) : limb(0);
  for (; i > 0; --i) {
    _limbs[i - 1] = fill;
  }
}

void limbs::resize(const size_type n)
{
  _limbs.resize(n);
}

void limbs::shrink_to_fit()
{
  size_type n;
  for (n = size(); n > 0; --n) {
    if (_limbs[n - 1])
      break;
  }

  _limbs.resize(n);
}

limbs::size_type limbs::fls() const noexcept
{
  size_type n;
  for (n = size(); n > 0; --n) {
    if (_limbs[n - 1])
      break;
  }

  if (!n)
    return 0;

  --n;
  return n * limb::width + _limbs[n].fls();
}

limbs::size_type limbs::ffs() const noexcept
{
  size_type n;
  for (n = 0; n < size(); ++n) {
    if (_limbs[n])
      break;
  }

  if (n == size())
    return 0;

  return n * limb::width + _limbs[n].ffs();
}

limbs::size_type limbs::clrsb() const noexcept
{
  if (!size())
    return 0;

  const bool sign = !!(_limbs.back().value() >> (limb::width - 1));
  const limb all_sign_value = sign ? ~limb(0) : limb(0);
  size_type n;
  for (n = size(); n > 0; --n) {
    if (_limbs[n - 1] != all_sign_value)
      break;
  }

  if (!n)
    return width() - 1;

  size_type result = (size() - n) * limb::width;
  if (!!(_limbs[n - 1].value() >> (limb::width - 1)) == sign)
    result += _limbs[n - 1].clrsb();
  else if (result)
    --result;

  return result;
}

bool limbs::test_bit(const size_type i) const noexcept
{
  assert(i < width());
  const size_type n = i / limb::width;
  const unsigned int pos = i % limb::width;
  return !!(_limbs[n].value() & (static_cast<limb::limb_type>(1) << pos));
}

void limbs::set_bit(const size_type i, const bool value) noexcept
{
  assert(i < width());
  const size_type n = i / limb::width;
  const unsigned int pos = i % limb::width;

  const limb::limb_type mask = static_cast<limb::limb_type>(1) << pos;
  const limb::limb_type new_value
    = value ? (_limbs[n].value() | mask) : (_limbs[n].value() & ~mask);
  _limbs[n] = limb(new_value);
}

bool limbs::is_any_set_below(const size_type i) const noexcept
{
  assert(i <= width());
  size_type n = i / limb::width;
  const unsigned int pos = i % limb::width;

  if (pos && _limbs[n].value() & limb::mask(pos).value())
    return true;

  for (; n > 0; --n) {
    if (_limbs[n - 1])
      return true;
  }

  return false;
}

bool limbs::is_any_set_at_or_above(const size_type i) const noexcept
{
  if (i >= width())
    return false;

  size_type n = i / limb::width;
  assert(n < size());
  const unsigned int pos = i % limb::width;

  if (_limbs[n].value() >> pos)
    return true;

  for (++n; n < size(); ++n) {
    if (_limbs[n])
      return true;
  }

  return false;
}

bool limbs::are_all_set_below(const size_type i) const noexcept
{
  assert(i <= width());
  size_type n = i / limb::width;
  const unsigned int pos = i % limb::width;

  if (pos &&
      (_limbs[n].value() & limb::mask(pos).value()) != limb::mask(pos).value())
    return false;

  for (; n > 0; --n) {
    if (~_limbs[n - 1])
      return false;
  }

  return true;
}

bool limbs::are_all_set_at_or_above(const size_type i) const noexcept
{
  if (i >= width())
    return false;

  size_type n = i / limb::width;
  const unsigned int pos = i % limb::width;

  if ((_limbs[n].value() & ~limb::mask(pos).value()) !=
      ~limb::mask(pos).value()) {
    return false;
  }

  for (++n; n < size(); ++n) {
    if (~_limbs[n])
      return false;
  }

  return true;
}

limbs limbs::from_size_type(size_type value)
{
  _limbs_type ls;
  if (value) {
    const size_type width = std::numeric_limits<size_type>::digits;
    const size_type size = width_to_size(width);
    ls.reserve(size);
    while (value) {
      ls.emplace_back(static_cast<limb::limb_type>(value));
      value >>= limb::width;
    }
  } else {
    ls.resize(1);
  }

  return limbs(std::move(ls));
}

limbs limbs::align(const mpa::limbs::size_type log2_align) const
{
  if (!log2_align)
    return *this;

  limbs align_add;
  align_add.resize(width_to_size(log2_align));
  align_add.set_bits_below(log2_align, true);

  limbs result = *this + align_add;
  result.set_bits_below(log2_align, false);

  return result;
}

limbs limbs::align_down(const mpa::limbs::size_type log2_align) const
{
  limbs result = *this;
  result.set_bits_below(log2_align, false);

  return result;
}

limbs limbs::reverse(const limbs::size_type n_rev_units,
		     const unsigned int rev_unit_width_log2) const
{
  const unsigned int rev_unit_width = 1 << rev_unit_width_log2;
  mpa::limbs::size_type result_width = n_rev_units << rev_unit_width_log2;
  assert(!is_any_set_at_or_above(result_width));
  limbs r{*this};
  r.resize(width_to_size(result_width));

  // First, shift everything left to align with the left (most
  // significant) boundary.
  if (rev_unit_width <= limb::width) {
    const size_type lshift_distance = r.width() - result_width;
    assert((lshift_distance & (rev_unit_width - 1)) == 0);
    if (lshift_distance != 0) {
      const size_type low_bits = lshift_distance % limb::width;
      assert(low_bits != 0 && low_bits < limb::width);
      for (size_type __j = size(); __j > 1; --__j) {
	const size_type j = __j - 1;
	const limb::limb_type value =
	  ((r._limbs[j - 1].value() >> (limb::width - low_bits)) |
	   (r._limbs[j].value() << low_bits));
	r._limbs[j] = limb(value);
      }
      r._limbs[0] = r._limbs[0] << low_bits;
    }

    // Then reverse the limbs' order.
    std::reverse(r._limbs.begin(), r._limbs.end());

    // And finally, reverse each individual limb's rev_units.
    if (rev_unit_width < limb::width) {
      for (limb &l : r._limbs)
	l = l.reverse(rev_unit_width_log2);
    }

  } else {
    // Reverse chunks of limbs.
    for (limbs::size_type i = 0; i < n_rev_units / 2; ++i) {
      _limbs_type::iterator i0 =
	r._limbs.begin() + (i << rev_unit_width_log2) / limb::width;
      _limbs_type::iterator i1 =
	r._limbs.end() - ((i + 1) << rev_unit_width_log2) / limb::width;
      for (limbs::size_type j = 0; j < rev_unit_width / limb::width;
	   ++j, ++i0, ++i1) {
	std::swap(*i0, *i1);
      }
    }
  }

  return r;
}

limbs limbs::from_string(const std::string::const_iterator &begin,
			 const std::string::const_iterator &end,
			 const limb &base)
{
  limbs r({0});
  for (std::string::const_iterator it = begin; it != end; ++it) {
    // Multiply r by base.
    limb carry(0);
    for (size_type i = 0; i < r.size(); ++i) {
      double_limb t = base * r[i];
      bool t_high_overflow = t.add(carry);
      assert(!t_high_overflow);
      r[i] = t.low();
      carry = t.high();
    }
    if (carry)
      r._limbs.emplace_back(carry);

    // And add the new digit.
    char a = *it;
    if ('0' <= a && a <= '9') {
      a -= '0';
    } else if ('a' <= a && a <= 'z') {
      a -= ('a' - 10);
    } else if ('A' <= a && a <= 'Z') {
      a -= 'A' - 10;
    } else {
      throw std::range_error("Unrecognized digit");
    }

    const mpa::limb l(static_cast<limb::limb_type>(a));
    if (l >= base)
      throw std::range_error("Digit invalid for base");

    bool overflow = r[0].add(l);
    for (size_type i = 1; i < r.size(); ++i)
      overflow = r[i].add(overflow);

    if (overflow)
      r._limbs.emplace_back(overflow);
  }

  return r;
}

std::string limbs::to_string(const limb &base) const
{
  mpa::limbs val = *this;
  const mpa::limbs ls_base{base};
  std::string result;
  do {
    auto div = val / ls_base;
    val = std::move(div.first);

    assert(div.second.size() == 1);
    const limb::limb_type &remainder = div.second[0].value();
    assert(remainder < base.value());

    // Build the resulting string in reverse order for efficiency and
    // reverse it below.
    if (remainder <= 9) {
      result.append(1, remainder + '0');
    } else {
      result.append(1, remainder + 'a' - 10);
    }
  } while (val);

  std::reverse(result.begin(), result.end());
  return result;
}
