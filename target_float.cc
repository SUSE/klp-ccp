#include <cassert>
#include <stdexcept>
#include "target_float.hh"
#include "target_int.hh"

using namespace suse::cp;

target_float::target_float() = default;

target_float::target_float(const mpa::limbs::size_type f_width,
			   const mpa::limbs::size_type e_width,
			   const bool is_negative,
			   const mpa::limbs &f,
			   const mpa::limbs &e)
  : _f_width(f_width), _e_width(e_width),
    _f(f), _e(e), _is_negative(is_negative)
{
  assert(is_inf() || is_zero() || _f);
}

target_float::target_float(const mpa::limbs::size_type f_width,
			   const mpa::limbs::size_type e_width,
			   const bool is_negative,
			   mpa::limbs &&f,
			   mpa::limbs &&e)
  : _f_width(f_width), _e_width(e_width),
    _f(std::move(f)), _e(std::move(e)), _is_negative(is_negative)
{
  assert(is_inf() || is_zero() || _f);
}

bool target_float::operator==(const target_float &op) const noexcept
{
  _assert_same_config(op);

  if (is_zero() && op.is_zero())
    return true;

  if (is_nan() || op.is_nan())
    return false;

  return (_is_negative == op._is_negative &&
	  _e == op._e && _f == op._f);
}

bool target_float::operator<(const target_float &op) const noexcept
{
  if (is_zero() && op.is_zero())
    return false;

  if (is_nan() || op.is_nan())
    return false;

  if (_is_negative && !op._is_negative)
    return true;

  if (!_is_negative && op._is_negative)
    return false;

  // Same sign, compare absolute values first.
  bool abs_lt = false;
  bool abs_eq = false;
  if (_e < op._e) {
    abs_lt = true;
  } else if (_e == op._e) {
    abs_lt = (_f < op._f);
    if (!abs_lt)
      abs_eq = (_f == op._f);
  }

  if (abs_eq)
    return false;

  return abs_lt ^ _is_negative;
}

target_float target_float::operator-() const
{
  return target_float(_f_width, _e_width, !_is_negative, _f, _e);
}

target_float target_float::operator+(const target_float &op) const
{
  return _add(op, false);
}

target_float target_float::operator-(const target_float &op) const
{
  return _add(op, true);
}

target_float target_float::operator*(const target_float &op) const
{
  _assert_same_config(op);

  if (is_nan() || op.is_nan()) {
    return _create_nan(_f_width, _e_width);
  } else if (is_inf() || op.is_inf()) {
    if (is_zero() || op.is_zero()) {
      // Multiplication of infinity with zero gives NaN.
      return _create_nan(_f_width, _e_width);
    }
    return _create_inf(_f_width, _e_width, _is_negative ^ op._is_negative);
  }

  mpa::limbs ls = _f * op._f;
  mpa::limbs e = _e + op._e;
  if (e.test_bit(e.width() - 1)) {
    // Addition overflowed into sign bit, enlarge e.
    e.resize(e.size() + 1);
  }

  // In the interpretation of the floating point significand,
  // a factor of 2^{-(_f_width - 1)} is implicitly assumed.
  e = e - (_bias(_e_width)
	   + mpa::limbs::from_size_type(_f_width - 1));

  _normalize(_f_width, _e_width, ls, e);
  return target_float(_f_width, _e_width,
		      _is_negative ^ op._is_negative,
		      std::move(ls), std::move(e));
}

target_float target_float::operator/(const target_float &op) const
{
  _assert_same_config(op);

  if (is_nan() || op.is_nan()) {
    return _create_nan(_f_width, _e_width);
  } else if (is_inf()) {
    if (op.is_inf()) {
      // Inf / Inf gives NaN
      return _create_nan(_f_width, _e_width);
    }
    return _create_inf(_f_width, _e_width, _is_negative ^ op._is_negative);
  } else if (is_zero()) {
    if (op.is_zero()) {
      // 0/0 gives NaN
      return _create_nan(_f_width, _e_width);
    }
    return _create_zero(_f_width, _e_width, _is_negative ^ op._is_negative);
  } else if (op.is_zero()) {
    return _create_inf(_f_width, _e_width, _is_negative ^ op._is_negative);
  }

  // Let n = op._f.fls(), r = this->_f.fls().
  // Set s = _f_width + 2 + n - r
  // It follows that 2^s * this->_f will have its fls() equal to
  // s + r = _f_width + 2 + n.
  // Dividing it by op._f will yield an integer with
  // its fls() being either of _f_width + 2 + 1 or _f_width + 2
  // which is enough for rounding.
  const mpa::limbs::size_type n = op._f.fls();
  assert(n && n <= _f_width);
  const mpa::limbs::size_type r = _f.fls();
  assert(r && r <= _f_width);
  const mpa::limbs::size_type s = _f_width + 2 + n - r;
  mpa::limbs u = _f;
  u.resize(mpa::limbs::width_to_size(s + r));
  u = u.lshift(s);
  std::pair<mpa::limbs, mpa::limbs> qr = u / op._f;
  mpa::limbs &quot = qr.first;
  mpa::limbs &rem = qr.second;

  assert(quot.width() >= _f_width + 2 + 1);
  assert(quot.test_bit(_f_width + 2) || quot.test_bit(_f_width + 1));
  assert(!quot.is_any_set_at_or_above(_f_width + 2 + 1));

  // Prepare the last bit for proper rounding.
  if (quot.test_bit(1) && !quot.test_bit(0)) {
    // Rounding tie, if the remainder is non-zero, enforce rounding up.
    quot.set_bit(0, !!rem);
  }

  // In the interpretation of the floating point significand,
  // a factor of 2^{-(_f_width - 1)} is implicitly assumed.
  mpa::limbs e =
    _e + _bias(_e_width) + mpa::limbs::from_size_type(_f_width - 1);
  if (e.test_bit(e.width() - 1)) {
    // Addition overflowed into sign bit, enlarge e.
    e.resize(e.size() + 1);
  }
  e = e - (op._e + mpa::limbs::from_size_type(s));

  _normalize(_f_width, _e_width, quot, e);
  return target_float(_f_width, _e_width,
		      _is_negative ^ op._is_negative,
		      std::move(quot), std::move(e));
}

target_float target_float::from_base10_exp(const mpa::limbs::size_type f_width,
					   const mpa::limbs::size_type e_width,
					   mpa::limbs &&m, mpa::limbs &&e10)
{
  if (!m)
    return _create_zero(f_width, e_width, false);

  // First calculate v = 10^{|e10|}
  //
  // This is expensive for large |e10| and in fact, there's a gcc
  // testcase involving floating point literals with large exponents.
  // In order to avoid having this running forever, cancel the computation
  // when it's clear that m * 10^e10 is larger (smaller) in magnitude
  // than the largest (smallest) representable value.
  //
  // There are two cases:
  // 1.) e10 > 0: The maximum representable exponent is
  //     2^e_width - 2 - _bias(e_width)
  //     == 2^e_width - 2 - (2^{e_width - 1} - 1)
  //     == 2^{e_width - 1} - 1 == _bias(e_width)
  //
  //     Given that m is taken to be an integer >=1, terminate the
  //     calculation once v > 2^{_bias(e_width)}.
  //
  // 2.) e10 < 0: The smallest possible number not rounded unconditionally
  //     towards zero is
  //     2^{-f_width} * 2^{1 - _bias(e_width)}.
  //     Thus, if
  //     m / v < 2^{-f_width} * 2^{1 - _bias(e_width)} <=>
  //     m * 2^{f_width} * 2^{-1 + _bias(e_width)} < v
  //     then the value of m / v will get rounded to zero.
  //     Since m < 2^{m.fls()}, this condition is guaranteed to become
  //     true if
  //     2^{m.fls()} * 2^{f_width} * 2^{-1 + _bias(e_width)} <= v
  //
  const bool e10_is_negative = e10 && e10.test_bit(e10.width() - 1);

  mpa::limbs::size_type max_v_log2;
  if (e10_is_negative) {
    e10 = e10.complement();

    max_v_log2 = _bias(e_width).to_type<mpa::limbs::size_type>();
    const mpa::limbs::size_type m_fls = m.fls();
    if (max_v_log2 ==
	std::numeric_limits<mpa::limbs::size_type>::max() - m_fls + 1) {
      throw std::overflow_error("max_v_log2 overflow");
    }
    max_v_log2 += m_fls;

    if (max_v_log2 ==
	std::numeric_limits<mpa::limbs::size_type>::max() - f_width + 1) {
      throw std::overflow_error("max_v_log2 overflow");
    }
    max_v_log2 += f_width;

  } else {
    max_v_log2 = _bias(e_width).to_type<mpa::limbs::size_type>();
    if (max_v_log2 == std::numeric_limits<mpa::limbs::size_type>::max()) {
      throw std::overflow_error("max_v_log2 overflow");
    }
    max_v_log2 += 1;
  }

  const mpa::limbs ten({10});
  mpa::limbs v;
  if (e10) {
    v = ten;
    for (mpa::limbs::size_type i = e10.fls() - 1; i > 0; --i) {
      v = v * v;
      if (e10.test_bit(i - 1))
	v = v * ten;

      if (v.is_any_set_at_or_above(max_v_log2)) {
	if (e10_is_negative) {
	  // Result will be less than smallest subnormal number,
	  // return zero.
	  return _create_zero(f_width, e_width, false);

	} else {
	  // Result will be larger than largest representable number,
	  // return infinite.
	  return _create_inf(f_width, e_width, false);

	}
      }
    }
  } else {
    v = mpa::limbs({1});
  }

  // In what follows, the significand is calculated in m. s will be a
  // number to subtract from the resulting exponent.
  mpa::limbs::size_type s = 0;
  if (!e10_is_negative) {
    m = m * v;

  } else {
    // Calculate the quotient m / v, in analogy to the floating point division
    // Let n = v.fls(), r = m.fls().
    // Set s = f_width + 2 + n - r
    // It follows that 2^s * m will have its fls() equal to
    // s + r = f_width + 2 + n.
    // Dividing it by v will yield an integer with
    // its fls() being either of f_width + 2 + 1 or f_width + 2
    // which is enough for rounding.
    const mpa::limbs::size_type n = v.fls();
    assert(n);
    const mpa::limbs::size_type r = m.fls();
    assert(r);
    s = (r <= f_width + 2 + n) ? f_width + 2 + n - r : 0;
    if (s) {
      m.resize(mpa::limbs::width_to_size(s + r));
      m = m.lshift(s);
    }
    std::pair<mpa::limbs, mpa::limbs> qr = m / v;
    mpa::limbs &quot = qr.first;
    mpa::limbs &rem = qr.second;

    assert(quot.width() >= f_width + 2 + 1);
    if (s) {
      assert(quot.test_bit(f_width + 2) || quot.test_bit(f_width + 1));
      assert(!quot.is_any_set_at_or_above(f_width + 2 + 1));
    }

    // Prepare the last bit for proper rounding.
    if (quot.test_bit(1) && !quot.test_bit(0)) {
      // Rounding tie, if the remainder is non-zero, enforce rounding up.
      quot.set_bit(0, !!rem);
    }

    m = std::move(quot);
  }

  // In the interpretation of the floating point significand,
  // a factor of 2^{-(_f_width - 1)} is implicitly assumed.
  mpa::limbs e = _bias(e_width) + mpa::limbs::from_size_type(f_width - 1);
  if (e.test_bit(e.width() - 1)) {
    // Addition overflowed into sign bit, enlarge e.
    e.resize(e.size() + 1);
  }
  e = e - mpa::limbs::from_size_type(s);

  _normalize(f_width, e_width, m, e);
  return target_float(f_width, e_width, false, std::move(m), std::move(e));
}

target_float target_float::from_base2_exp(const mpa::limbs::size_type f_width,
					  const mpa::limbs::size_type e_width,
					  mpa::limbs &&m, mpa::limbs &&e2)
{
  // Calculate
  //  e2 + _bias(e_width) + mpa::limbs::from_size_type(f_width - 1),
  // but take care not to overflow into the sign bit.
  // Enlarge e2 if necessary.
  const mpa::limbs bias = _bias(e_width);
  mpa::limbs frac_width = mpa::limbs::from_size_type(f_width - 1);

  const mpa::limbs::size_type required_e2_width =
    ((std::max(std::max(frac_width.fls(), bias.fls()),
	       e2.width() ? e2.width() - e2.clrsb() - 1 : 0))
     + 3);

  if (e2.width() < required_e2_width) {
    const mpa::limbs::size_type old_e2_width = e2.width();
    e2.resize(mpa::limbs::width_to_size(required_e2_width));
    if (old_e2_width)
      e2.set_bits_at_and_above(old_e2_width, e2.test_bit(old_e2_width - 1));
  }

  const mpa::limbs::size_type e2_size = e2.size();
  // In the interpretation of the floating point significand,
  // a factor of 2^{-(_f_width - 1)} is implicitly assumed.
  e2 = e2 + bias + frac_width;
  if (e2.size() > e2_size) {
    // Transition from negative to >=0.
    assert(e2.size() == e2_size + 1 && e2[e2_size].value() == 1);
    e2.resize(e2_size);
    assert(!e2.test_bit(e2.width() - 1));
  }

  _normalize(f_width, e_width, m, e2);
  return target_float(f_width, e_width, false, std::move(m), std::move(e2));
}

bool target_float::is_nan() const noexcept
{
  return _e.are_all_set_below(_e_width) && _f;
}

bool target_float::is_inf() const noexcept
{
  return _e.are_all_set_below(_e_width) && !_f;
}

bool target_float::is_zero() const noexcept
{
  return _e.test_bit(0) && !_e.is_any_set_at_or_above(1) && !_f;
}

target_float target_float::convert(const mpa::limbs::size_type f_width,
				   const mpa::limbs::size_type e_width) const
{
  // Handle special cases first.
  if (is_nan()) {
    return _create_nan(f_width, e_width);
  } else if (is_inf()) {
    return _create_inf(f_width, e_width, _is_negative);
  } else if (is_zero()) {
    return _create_zero(f_width, e_width, _is_negative);
  }

  mpa::limbs e = _e + _bias(e_width) + mpa::limbs::from_size_type(f_width);
  if (e.test_bit(e.width() - 1)) {
    // Addition overflowed into sign bit, enlarge e.
    e.resize(e.size() + 1);
  }

  e = e - _bias(_e_width) - mpa::limbs::from_size_type(_f_width);

  mpa::limbs f = _f;

  _normalize(f_width, e_width, f, e);
  return target_float(f_width, e_width, _is_negative, std::move(f),
		      std::move(e));
}

target_int target_float::to_int(const mpa::limbs::size_type prec,
				const bool is_signed) const
{
  mpa::limbs e =
    _e - _bias(_e_width) - mpa::limbs::from_size_type(_f_width - 1);

  mpa::limbs val({0});
  if (e.test_bit(e.width() - 1)) {
    // Negative e, shift significand to the right.
    e = e.complement();
    if (!(e.is_any_set_at_or_above
	  (std::numeric_limits<mpa::limbs::size_type>::digits))) {
      const mpa::limbs::size_type sr = e.to_type<mpa::limbs::size_type>();
      if (sr < _f_width) {
	val = _f;
	val = val.rshift(sr, false);
      }
    }
  } else {
    // Positive e, shift significand to the left.
    if ((e.is_any_set_at_or_above
	 (std::numeric_limits<mpa::limbs::size_type>::digits))) {
      throw std::overflow_error("implementation limit exceeded");
    }
    const mpa::limbs::size_type sl = e.to_type<mpa::limbs::size_type>();
    if (std::numeric_limits<mpa::limbs::size_type>::max() - sl < _f_width) {
      throw std::overflow_error("implementation limit exceeded");
    }

    val = _f;
    val.resize(mpa::limbs::width_to_size(_f_width + sl));
    val = val.lshift(sl);
  }

  if (is_signed && val.is_any_set_at_or_above(prec)) {
    throw std::overflow_error("integer overflow");
  }

  val.resize(mpa::limbs::width_to_size(prec + is_signed));
  if (_is_negative)
    val = val.complement();

  if (!is_signed) {
    // Do a modulo 2^prec reduction.
    val.set_bits_at_and_above(prec, false);
  }

  return target_int(prec, is_signed, std::move(val));
}

void target_float::_assert_same_config(const target_float &op) const noexcept
{
  assert(_f_width == op._f_width);
  assert(_e_width == op._e_width);
}

mpa::limbs target_float::_bias(const mpa::limbs::size_type e_width)
{
  mpa::limbs b;
  b.resize(mpa::limbs::width_to_size(e_width));
  // bias is 2^{e_width - 1} - 1;
  b.set_bits_below(e_width - 1, true);

  return b;
}

target_float target_float::_add(const target_float &op, const bool negate_op)
  const
{
  _assert_same_config(op);
  assert(_e);
  assert(op._e);

  if (is_nan() || op.is_nan()) {
    return _create_nan(_f_width, _e_width);
  } else if ((_is_negative ^ (op._is_negative ^ negate_op)) &&
	     is_inf() && op.is_inf()) {
    // Addition of differently signed infinities gives NaN.
    return _create_nan(_f_width, _e_width);
  }

  const bool exchange_ops = _e < op._e;
  const target_float &u = exchange_ops ? op : *this;
  const target_float &v = exchange_ops ? *this : op;

  const mpa::limbs _e_diff = u._e - v._e;
  const mpa::limbs::size_type e_diff =
      (!(_e_diff.is_any_set_at_or_above
	 (std::numeric_limits<mpa::limbs::size_type>::digits)) ?
       _e_diff.to_type<mpa::limbs::size_type>() :
       std::numeric_limits<mpa::limbs::size_type>::max());
  if (e_diff >= _f_width + 2) {
    return target_float(u._f_width, u._e_width,
			u._is_negative ^ (exchange_ops && negate_op),
			u._f, u._e);
  }

  // The accumulator(s) will be layout as follows:
  // - the lowest _f_width + 3 bits hold the significand with the radix
  //   point being anchored at _f_width + 2,
  // - the bit at _f_width + 3 will receive the overflow, if any
  // - and the highest bit at _f_width + 4 will be the sign
  //   bit in two complement's arithmetic.
  const mpa::limbs::size_type acc_width = 1 + 1 + _f_width + 3;
  const mpa::limbs::size_type acc_n_limbs =
    mpa::limbs::width_to_size(acc_width);
  mpa::limbs lsv = v._f; // implicit right shift by 3 positions
  lsv.resize(acc_n_limbs);
  if (e_diff < 3) {
    // v has been shifted too much, undo without any loss.
    lsv = lsv.lshift(3 - e_diff);
  } else if (e_diff > 3) {
    // Complete right shift of v. This introduces some loss.
    lsv = lsv.rshift(e_diff - 3, false);

    // Now, lsv is equal to 2^-e_diff * v, truncated after its
    // _f_width + 3 high bits.
    //
    // It follows that the e_diff - 3 bits have been lost.
    //
    // Retain rounding information in lsv's least significant bit:
    // 2^(_f_width + 1) * lsv will have the same rounding behaviour
    // as 2^(_f_width + 1) * 2^-e_diff * v
    lsv.set_bit(0, v._f.is_any_set_below(e_diff - 2));
  }

  if (v._is_negative ^ (!exchange_ops && negate_op)) {
    lsv = lsv.complement();
  }

  mpa::limbs lsu = u._f;
  lsu.resize(acc_n_limbs);
  lsu = lsu.lshift(3);
  if (u._is_negative ^ (exchange_ops && negate_op))
    lsu = lsu.complement();

  mpa::limbs ls = lsu + lsv;
  bool is_negative = ls.test_bit(1 + _f_width + 3);
  if (is_negative) {
    ls = ls.complement();
    assert(!ls.test_bit(1 + _f_width + 3));
    ls.set_bits_at_and_above(1 + 1 + _f_width + 3, false);
  }

  mpa::limbs e = u._e;
  if (e.test_bit(e.width() - 1)) {
    // Addition overflowed into sign bit, enlarge e.
    e.resize(e.size() + 1);
  }
  e = e - mpa::limbs::from_size_type(3);

  _normalize(_f_width, _e_width, ls, e);

  // Handle zero sign
  if (e.test_bit(0) && !e.is_any_set_at_or_above(1) && !ls) {
    // If the outcome is zero due to addition of differently signed
    // numbers, then the result is positive.
    is_negative = _is_negative && (negate_op ^ op._is_negative);
  }

  return target_float(u._f_width, u._e_width,
		      is_negative,
		      std::move(ls), std::move(e));
}

void target_float::_normalize(const mpa::limbs::size_type f_width,
			      const mpa::limbs::size_type e_width,
			      mpa::limbs &f, mpa::limbs &e)
{
  // Search for the significand's end, i.e. set significand_end to
  // point after the highest set bit.
  mpa::limbs::size_type significand_end = f.fls();
  if (!significand_end) {
    e.resize(mpa::limbs::width_to_size(e_width));
    f.resize(mpa::limbs::width_to_size(f_width));

    // All bits zero, set exponent to one if not Inf and return.
    if (!e.are_all_set_below(e_width)) {
      e.set_bits_at_and_above(0, false);
      e.set_bit(0, true);
    }
    return;
  }

  // Round to f_width places
  bool rounded_up = false;
  if (significand_end > f_width) {
    rounded_up = _round_at(f, significand_end - f_width);
    // Rounding could have overflowed the significand.
    if (rounded_up && significand_end < f.width() &&
	f.test_bit(significand_end)) {
      ++significand_end;
      // Another rounding would have no effect in this case.
      assert(!f.test_bit(significand_end - f_width - 1));
    }
  }

  // At this point, we've found the significand's end and have rounded
  // it to f_width places. What remains to be done is to shift
  // the significand to the final place and adjust e accordingly.
  // Special care must be taken to deal with underflows, i.e.
  // the case where the resulting e is < 1. For overflows, Inf
  // will be returned.
  assert(!f.is_any_set_at_or_above(significand_end));
  assert(significand_end);
  assert(f.test_bit(significand_end - 1));

  // The passed e might have some additional bits beyond e_width,
  // i.e. it is taken as a signed integer in modulo 2^e.width()
  // arithmetic.
  mpa::limbs new_e;
  if (significand_end < f_width) {
    new_e = e -  mpa::limbs::from_size_type(f_width - significand_end);
  } else {
    // Add significand_end - f_width to e, but be careful not to overflow
    // into the sign bit.
    // Enlarge e if necessary.
    const mpa::limbs rshift =
      mpa::limbs::from_size_type(significand_end - f_width);
    const mpa::limbs::size_type required_e_width =
      std::max(rshift.fls(), e.width() ? e.width() - e.clrsb() - 1 : 0) + 2;
    if (e.width() < required_e_width) {
      const mpa::limbs::size_type old_e_width = e.width();
      e.resize(mpa::limbs::width_to_size(std::max(required_e_width, e_width)));
      if (old_e_width)
	e.set_bits_at_and_above(old_e_width, e.test_bit(old_e_width - 1));
    }

    const mpa::limbs::size_type e_size = e.size();
    new_e = e + rshift;
    if (new_e.size() > e_size) {
      // Transition from negative to >=0.
      assert(new_e.size() == e_size + 1 && new_e[e_size].value() == 1);

      new_e.resize(e_size);
      assert(!new_e.test_bit(new_e.width() - 1));
    }
  }
  if (!new_e || new_e.test_bit(new_e.width() - 1)) {
    // Exponent underflow. Recover by returning a subnormal number,
    // i.e. shifting the significand by 1 - e places to the right such
    // that the accordingly adjusted e will equal one.
    new_e = new_e.complement();
    mpa::limbs::size_type shift =
      std::numeric_limits<mpa::limbs::size_type>::max();
    if (!(new_e.is_any_set_at_or_above
	  (std::numeric_limits<mpa::limbs::size_type>::digits))) {
      shift = new_e.to_type<mpa::limbs::size_type>();
      if (shift < std::numeric_limits<mpa::limbs::size_type>::max()) {
	shift += 1;
      }
    }

    if (std::numeric_limits<mpa::limbs::size_type>::max() - shift <
	significand_end) {
      significand_end = std::numeric_limits<mpa::limbs::size_type>::max();
    } else {
      significand_end += shift;
    }

    // Round
    if (significand_end > f_width &&
	significand_end - f_width <= f.width()) {

      // It must be made sure that the previous rounding, if any
      // doesn't have any effect on the following rounding's outcome.
      //
      // If the previous rounding was downwards, then it did not touch
      // f's contents.
      //
      // The only problematic case is if it had produced a rounding
      // tie of the form
      //
      //   ... 1 1 0 0 0 ... 0 0
      //        ^
      //        |
      // position (significand_end - f_width)
      //
      // due to rounding any value in the range from
      //   ... 1 0 1 1 1 ... 1 1 0 ... 0 0
      //                      ^
      //                      |
      // to
      //   ... 1 0 1 1 1 ... 1 1 1 ... 1 1
      //                      ^
      //                      |
      // at a marked place: resolving this tie to even would now lead
      // to another increase and thus yield a too large value. Detect
      // this condition and act accordingly.
      assert(!rounded_up || significand_end - f_width - 1 < f.width());
      if (rounded_up &&
	  f.test_bit(significand_end - f_width - 1) &&
	  !f.is_any_set_below(significand_end - f_width - 1)) {
	f.set_bit(significand_end - f_width - 1, false);
      } else {
	// Significand overflow can't happen because the number is
	// subnormal.
	assert(significand_end > f.width() ||
	       !f.test_bit(significand_end - 1));
	_round_at(f, significand_end - f_width);
	assert(significand_end >= f.width() ||
	       !f.test_bit(significand_end));
      }
    }

    // Set e to one.
    e.resize(mpa::limbs::width_to_size(e_width));
    e.set_bits_at_and_above(0, false);
    e.set_bit(0, true);

  } else if (new_e.is_any_set_at_or_above(e_width) ||
	     new_e.are_all_set_below(e_width)) {
    // Exponent overflow, signal infinity.
    e.resize(mpa::limbs::width_to_size(e_width));
    f.resize(mpa::limbs::width_to_size(f_width));
    e.set_bits_at_and_above(0, true);
    e.set_bits_at_and_above(e_width, false);
    f.set_bits_at_and_above(0, false);
    return;

  } else {
    e = std::move(new_e);
    const mpa::limbs::size_type old_e_width = e.width();
    assert(old_e_width);
    e.resize(mpa::limbs::width_to_size(e_width));
    if (old_e_width < e.width())
      e.set_bits_at_and_above(old_e_width, e.test_bit(old_e_width - 1));

  }

  // Finally, shift the significand to its final place.
  if (significand_end >= f.width() + f_width) {
    f.set_bits_at_and_above(0, false);
  } else if (significand_end > f_width) {
    f = f.rshift(significand_end - f_width, false);
  } else if (significand_end < f_width) {
    f = f.lshift(f_width - significand_end);
  }

  f.resize(mpa::limbs::width_to_size(f_width));
}

bool target_float::_round_at(mpa::limbs &f, const mpa::limbs::size_type pos)
{
  assert(pos >= 1);
  assert(pos <= f.width());

  bool add_one = false;
  bool tie = false;
  if (f.test_bit(pos - 1)) {
    if (f.is_any_set_below(pos - 1))
      add_one = true;
    else
      tie = true;
  }

  if (tie) {
    assert(!add_one);
    add_one = pos < f.width() && f.test_bit(pos);
  }

  if (add_one) {
    mpa::limbs one({1});
    one.resize(mpa::limbs::width_to_size(pos + 1));
    one = one.lshift(pos);
    f = f + one;

    // Avoid another _round_at() invocation at a higher pos to
    // reconsider the low bits.
    f.set_bits_below(pos, false);
    return true;
  }

  return false;
}

target_float target_float::_create_nan(const mpa::limbs::size_type f_width,
				       const mpa::limbs::size_type e_width)
{
  mpa::limbs e;
  e.resize(mpa::limbs::width_to_size(e_width));
  e.set_bits_at_and_above(0, true);
  e.set_bits_at_and_above(e_width, false);

  mpa::limbs f;
  f.resize(mpa::limbs::width_to_size(f_width));
  f.set_bit(0, true);
  f.set_bits_at_and_above(1, false);

  return target_float(f_width, e_width, false,
		      std::move(f), std::move(e));
}

target_float target_float::_create_inf(const mpa::limbs::size_type f_width,
				       const mpa::limbs::size_type e_width,
				       const bool is_negative)
{
  mpa::limbs e;
  e.resize(mpa::limbs::width_to_size(e_width));
  e.set_bits_at_and_above(0, true);
  e.set_bits_at_and_above(e_width, false);

  mpa::limbs f;
  f.resize(mpa::limbs::width_to_size(f_width));

  return target_float(f_width, e_width, is_negative,
		      std::move(f), std::move(e));
}

target_float target_float::_create_zero(const mpa::limbs::size_type f_width,
					const mpa::limbs::size_type e_width,
					const bool is_negative)
{
  mpa::limbs e;
  e.resize(mpa::limbs::width_to_size(e_width));
  e.set_bit(0, true);
  e.set_bits_at_and_above(1, false);

  mpa::limbs f;
  f.resize(mpa::limbs::width_to_size(f_width));

  return target_float(f_width, e_width, is_negative,
		      std::move(f), std::move(e));
}
