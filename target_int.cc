#include <cassert>
#include <stdexcept>
#include "target_int.hh"

using namespace suse::cp;

target_int::target_int(const mpa::limbs::size_type prec, const bool is_signed,
		       mpa::limbs &&ls)
  : _prec(prec), _is_signed(is_signed), _limbs(std::move(ls))
{
  assert(prec);
  assert(_limbs.size() == _n_limbs());
  assert(is_signed || !_limbs.is_any_set_at_or_above(_width()));
  assert(!is_signed || _limbs.width() - _limbs.clrsb() <= _width());
}

target_int::target_int(const target_int &ti)
  : _prec(ti._prec), _is_signed(ti._is_signed), _limbs(ti._limbs)
{}

target_int::target_int(target_int &&ti)
  : _prec(ti._prec), _is_signed(ti._is_signed), _limbs(std::move(ti._limbs))
{}

bool target_int::operator==(const target_int &op) const noexcept
{
  _assert_same_prec_and_signedness(op);
  return _limbs == op._limbs;
}

bool target_int::operator<(const target_int &op) const noexcept
{
  _assert_same_prec_and_signedness(op);

  if (!_is_signed ||
      (!_is_negative() && !op._is_negative()) ||
      (_is_negative() && op._is_negative())) {
    return _limbs < op._limbs;

  } else if (_is_negative()) {
    assert(!op._is_negative());
    return true;

  } else {
    assert(!_is_negative() && op._is_negative());
    return false;
  }
}

bool target_int::operator<=(const target_int &op) const noexcept
{
  _assert_same_prec_and_signedness(op);

  if (!_is_signed ||
      (!_is_negative() && !op._is_negative()) ||
      (_is_negative() && op._is_negative())) {
    return _limbs <= op._limbs;

  } else if (_is_negative()) {
    assert(!op._is_negative());
    return true;

  } else {
    assert(!_is_negative() && op._is_negative());
    return false;
  }
}

target_int::operator bool() const noexcept
{
  return !!_limbs;
}

target_int target_int::operator-() const
{
  mpa::limbs ls = _limbs.complement();
  assert(ls.size() == _n_limbs());
  _propagate_sign_to_high(ls);
  _clamp_unsigned(ls);

  if (_is_signed && _is_negative() && ls.test_bit(_prec)) {
    // Only sign bit set: complement is an identity operation.
    throw std::overflow_error("integer overflow");
  }

  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::operator~() const
{
  mpa::limbs ls = ~_limbs;
  assert(ls.size() == _n_limbs());
  _propagate_sign_to_high(ls);
  _clamp_unsigned(ls);
  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::operator+(const target_int &op) const
{
  _assert_same_prec_and_signedness(op);
  mpa::limbs ls = _limbs + op._limbs;
  if (_is_signed && _is_negative() == op._is_negative() &&
      ls.test_bit(_prec) != _is_negative())
    throw std::overflow_error("integer overflow");

  ls.resize(_n_limbs());
  _propagate_sign_to_high(ls);
  _clamp_unsigned(ls);
  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::operator-(const target_int &op) const
{
  _assert_same_prec_and_signedness(op);
  mpa::limbs ls = _limbs - op._limbs;
  if (_is_signed && _is_negative() != op._is_negative() &&
      ls.test_bit(_prec) != _is_negative())
    throw std::overflow_error("integer overflow");

  assert(!_is_signed || ls.width() - ls.clrsb() <= _width());

  ls.resize(_n_limbs());
  _clamp_unsigned(ls);
  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::operator*(const target_int &op) const
{
  _assert_same_prec_and_signedness(op);

  if (_is_signed && _is_negative() && op._is_negative()) {
    mpa::limbs p = _limbs.complement() * op._limbs.complement();
    if (p.is_any_set_at_or_above(_prec))
      throw std::overflow_error("integer overflow");
    p.resize(_n_limbs());
    return target_int(_prec, _is_signed, std::move(p));

  } else if (_is_signed && _is_negative()) {
    mpa::limbs p = _limbs.complement() * op._limbs;
    if (p.is_any_set_at_or_above(_prec))
      throw std::overflow_error("integer overflow");
    p.resize(_n_limbs());
    p = p.complement();
    return target_int(_prec, _is_signed, std::move(p));

  } else if (_is_signed && op._is_negative()) {
    mpa::limbs p = _limbs * op._limbs.complement();
    if (p.is_any_set_at_or_above(_prec))
      throw std::overflow_error("integer overflow");
    p.resize(_n_limbs());
    p = p.complement();
    return target_int(_prec, _is_signed, std::move(p));

  }

  // Both operands >= 0
  mpa::limbs p = _limbs * op._limbs;
  if (p.is_any_set_at_or_above(_prec))
    throw std::overflow_error("integer overflow");
  p.resize(_n_limbs());
  return target_int(_prec, _is_signed, std::move(p));
}

target_int target_int::operator/(const target_int &op) const
{
  _assert_same_prec_and_signedness(op);

  if (_is_signed && _is_negative() && op._is_negative()) {
    std::pair<mpa::limbs, mpa::limbs> &&qr =
      _limbs.complement() / op._limbs.complement();
    mpa::limbs &q = qr.first;
    assert(q.size() <= _n_limbs());
    q.resize(_n_limbs());
    return target_int(_prec, _is_signed, std::move(q));

  } else if (_is_signed && _is_negative()) {
    std::pair<mpa::limbs, mpa::limbs> &&qr = _limbs.complement() / op._limbs;
    mpa::limbs &q = qr.first;
    assert(q.size() <= _n_limbs());
    q.resize(_n_limbs());
    q = q.complement();
    return target_int(_prec, _is_signed, std::move(q));

  } else if (_is_signed && op._is_negative()) {
    std::pair<mpa::limbs, mpa::limbs> &&qr = _limbs / op._limbs.complement();
    mpa::limbs &q = qr.first;
    assert(q.size() <= _n_limbs());
    q.resize(_n_limbs());
    q = q.complement();
    return target_int(_prec, _is_signed, std::move(q));

  }

  // Both operands >= 0
  std::pair<mpa::limbs, mpa::limbs> &&qr = _limbs / op._limbs;
  mpa::limbs &q = qr.first;
  assert(q.size() <= _n_limbs());
  q.resize(_n_limbs());
  return target_int(_prec, _is_signed, std::move(q));
}

target_int target_int::operator%(const target_int &op) const
{
  _assert_same_prec_and_signedness(op);

  if (_is_signed && _is_negative() && op._is_negative()) {
    std::pair<mpa::limbs, mpa::limbs> &&qr =
      _limbs.complement() / op._limbs.complement();
    mpa::limbs &r = qr.second;
    assert(r.size() <= _n_limbs());
    r.resize(_n_limbs());
    r = r.complement();
    return target_int(_prec, _is_signed, std::move(r));

  } else if (_is_signed && _is_negative()) {
    std::pair<mpa::limbs, mpa::limbs> &&qr = _limbs.complement() / op._limbs;
    mpa::limbs &r = qr.second;
    assert(r.size() <= _n_limbs());
    r.resize(_n_limbs());
    r = r.complement();
    return target_int(_prec, _is_signed, std::move(r));

  } else if (_is_signed && op._is_negative()) {
    std::pair<mpa::limbs, mpa::limbs> &&qr = _limbs / op._limbs.complement();
    mpa::limbs &r = qr.second;
    assert(r.size() <= _n_limbs());
    r.resize(_n_limbs());
    return target_int(_prec, _is_signed, std::move(r));

  }

  // Both operands >= 0
  std::pair<mpa::limbs, mpa::limbs> &&qr = _limbs / op._limbs;
  mpa::limbs &r = qr.second;
  assert(r.size() <= _n_limbs());
  r.resize(_n_limbs());
  return target_int(_prec, _is_signed, std::move(r));
}

target_int target_int::operator^(const target_int &op) const
{
  _assert_same_prec_and_signedness(op);
  mpa::limbs ls = _limbs ^ op._limbs;
  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::operator&(const target_int &op) const
{
  _assert_same_prec_and_signedness(op);
  mpa::limbs ls = _limbs & op._limbs;
  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::operator|(const target_int &op) const
{
  _assert_same_prec_and_signedness(op);
  mpa::limbs ls = _limbs | op._limbs;
  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::operator<<(const target_int &op) const
{
  const mpa::limbs::size_type distance = op._to_size_type();
  if (distance >= _width())
    throw std::overflow_error("integer overflow");

  mpa::limbs ls = _limbs.lshift(distance);
  if (_is_signed &&
      (ls.width() - ls.clrsb() > _width() ||
       _is_negative() != ls.test_bit(_prec))) {
    throw std::overflow_error("integer overflow");
  }
  _clamp_unsigned(ls);
  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::operator>>(const target_int &op) const
{
  const mpa::limbs::size_type distance = op._to_size_type();
  if (distance >= _width())
    throw std::overflow_error("integer overflow");

  const bool fill_value = (_is_signed && _is_negative()) ? true : false;
  mpa::limbs ls = _limbs.rshift(distance, fill_value);
  return target_int(_prec, _is_signed, std::move(ls));
}

target_int target_int::convert(const unsigned int prec,
			       const bool is_signed) const
{
  assert(prec);

  if (is_signed && prec < _prec &&
      ((_limbs.width() - _limbs.clrsb() - _is_signed > prec) ||
       (!_is_signed && _limbs.test_bit(_prec - 1)))) {
    throw std::overflow_error("integer overflow");
  }

  mpa::limbs ls = _limbs;
  const mpa::limbs::size_type n_limbs =
    mpa::limbs::width_to_size(prec + is_signed);
  ls.resize(n_limbs);
  if (_is_signed && _is_negative()) {
    ls.set_bits_at_and_above(prec, true);
  } else if (prec < _prec) {
    ls.set_bits_at_and_above(prec, false);
  }

  return target_int(prec, is_signed, std::move(ls));
}

bool target_int::is_negative() const noexcept
{
  return _is_signed && _is_negative();
}

mpa::limbs::size_type target_int::_to_size_type() const
{
  if (_is_signed && _is_negative()) {
    throw std::overflow_error("negative size_type");
  }

  return _limbs.to_size_type();
}

void target_int::_assert_same_prec_and_signedness(const target_int &op)
  const noexcept
{
  assert(_prec == op._prec);
  assert(_is_signed == op._is_signed);
}

mpa::limbs::size_type target_int::_n_limbs() const noexcept
{
  return mpa::limbs::width_to_size(_width());
}

bool target_int::_is_negative() const noexcept
{
  assert(_is_signed);
  return _limbs.test_bit(_prec);
}

void target_int::_propagate_sign_to_high(mpa::limbs &ls) const noexcept
{
  if (_is_signed)
    ls.set_bits_at_and_above(_prec, ls.test_bit(_prec));
}

void target_int::_clamp_unsigned(mpa::limbs &ls) const noexcept
{
  // Do the modulo 2^width() transformation
  if (!_is_signed)
    ls.set_bits_at_and_above(_prec, false);
}