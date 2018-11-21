#include <cassert>
#include <algorithm>
#include <array>
#include "mp_arithmetic.hh"

using namespace klp::ccp::mpa;

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


bool double_limb::add(const limb &op) noexcept
{
  const bool carry = _low.add(op);
  return _high.add(carry);
}

limb double_limb::div(const limb divisor) noexcept
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

limbs::limbs(_limbs_type &&ls)
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
    result.emplace_back(l);
  }

  if (borrow)
    result.emplace_back(~limb(0));

  return limbs(std::move(result));
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
  if (!distance)
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
  for (size_type __j = size(); __j > limb_offset; --__j) {
    const size_type j = __j - 1;
    const limb::limb_type value =
      ((_limbs[j - limb_offset].value() >> (limb::width - low_bits)) |
       (_limbs[j - limb_offset + 1].value() << low_bits));
    result[j] = limb(value);
  };

  result[limb_offset - 1] = limb(_limbs[0].value() << low_bits);
  for (size_type __j = limb_offset - 1; __j > 0; --__j) {
    const size_type j = __j - 1;
    result[j] = limb(0);
  }

  return limbs(std::move(result));
}

limbs limbs::rshift(const size_type distance, const bool fill_value) const
{
  if (!distance)
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
  for (size_type j = 0; j + limb_offset + 1 < size(); ++j) {
    const limb::limb_type value =
      ((_limbs[j + limb_offset].value() >> high_bits |
	_limbs[j + limb_offset + 1].value() << (limb::width - high_bits)));
    result[j] = limb(value);
  }

  result[size() - limb_offset - 1] =
    limb((_limbs.back().value() >> high_bits) |
	 (fill << (limb::width - high_bits)));

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
