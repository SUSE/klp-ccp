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

#include <dejagnu.h>
#include <cassert>
#include "mp_arithmetic.hh"

using namespace klp::ccp::mpa;

static int limb_test1()
{
  limb v = ~limb(0);
  if (v.add(false))
    return -1;
  if (!v.add(true))
    return -2;

  return 0;
}

static int limb_test2()
{
  limb v = ~limb(0);
  if (v.add(limb(0)))
    return -1;
  if (!v.add(limb(1)))
    return -2;

  return 0;
}

static int limb_test3()
{
  limb v(1);
  if (!v.add(~limb(0)))
    return -1;

  return 0;
}

static int limb_test4()
{
  limb v = limb(0);
  if (v.sub(false))
    return -1;
  if (!v.sub(true))
    return -2;

  return 0;
}

static int limb_test5()
{
  limb v = limb(0);
  if (v.sub(limb(0)))
    return -1;
  if (!v.sub(limb(1)))
    return -2;

  return 0;
}

static int limb_test6()
{
  limb v = ~limb(0);
  if (v.sub(true))
      return -1;

  if (!v.sub(~limb(0)))
    return -2;

  return 0;
}

static int limb_test7()
{
  const limb f = ~limb(0);
  const double_limb p = f * f;
  limb expected_high = ~limb(0);
  if (expected_high.sub(true))
    return -1;

  if (p.high() != expected_high)
    return -2;

  if (p.low() != limb(1))
    return -3;

  return 0;
}

static int limb_test8()
{
  if (limb(0).fls() != 0)
    return -1;

  if (limb(1).fls() != 1)
    return -2;

  if ((~limb(0)).fls() != limb::width)
    return -3;

  return 0;
}

static int limb_test9()
{
  if (limb(0).ffs() != 0)
    return -1;

  if (limb(1).ffs() != 1)
    return -2;

  if ((~limb(0)).ffs() != 1)
    return -3;

  return 0;
}

static int limb_test10()
{
  if (limb(0).clz() != limb::width)
    return -1;

  if (limb(1).clz() != limb::width - 1)
    return -2;

  if ((~limb(0)).clz() != 0)
    return -3;

  return 0;
}

static int limb_test11()
{
  if (limb(0).clrsb() != limb::width - 1)
    return -1;

  if ((~limb(0)).clrsb() != limb::width - 1)
    return -2;

  if (limb(1).clrsb() != limb::width - 2)
    return -3;

  if ((~limb(1)).clrsb() != limb::width - 2)
    return -4;

  if (limb(static_cast<limb::limb_type>(1) << (limb::width - 1)).clrsb())
    return -5;

  if ((~limb(static_cast<limb::limb_type>(1) << (limb::width - 1))).clrsb())
    return -6;

  return 0;
}


static int limb_test12()
{
  limb::limb_type l = 0;

  for (unsigned int rev_unit_width_log2 = 0;
       (1u << rev_unit_width_log2) < limb::width;
       ++rev_unit_width_log2) {
    const unsigned int rev_unit_width =
      1u << rev_unit_width_log2;
    const limb::limb_type rev_unit_mask =
      (static_cast<limb::limb_type>(1) << rev_unit_width) - 1;
    for (unsigned int i = limb::width >> rev_unit_width_log2; i > 0; --i) {
      l <<= rev_unit_width;
      l |= i & rev_unit_mask;
    }

    l = limb{l}.reverse(rev_unit_width_log2).value();

    for (unsigned int i = limb::width >> rev_unit_width_log2; i > 0; --i) {
      if ((l & rev_unit_mask) != (i & rev_unit_mask))
	return -1;
      l >>= rev_unit_width;
    }
  }

  return 0;
}


static int double_limb_test1()
{
  double_limb v(~limb(0), ~limb(0));

  if (v.add(limb(0)))
    return -1;

  if (!v.add(limb(1)))
    return -2;

  if (v.high() || v.low())
    return -3;

  return 0;
}

static int _double_limb_test_mul_div(const limb &result, const limb &divisor,
				     const limb &rest)
{
  double_limb v = result * divisor;
  if (v.add(rest))
    return -1;

  const limb r = v.div(divisor);
  if (r != rest)
    return -2;

  if (v.high())
    return -3;

  if (v.low() != result)
    return -4;

  return 0;
}

static int double_limb_test2()
{
  return _double_limb_test_mul_div(~limb(0), ~limb(0), limb(0));
}

static int double_limb_test3()
{
  return _double_limb_test_mul_div(limb(0), ~limb(0), limb(0));
}

static int double_limb_test4()
{
  limb rest = ~limb(0);
  rest.sub(limb(1));
  return _double_limb_test_mul_div(limb(0), ~limb(0), rest);
}

static int double_limb_test5()
{
  return _double_limb_test_mul_div(~limb(0), limb(1), limb(0));
}

static int double_limb_test6()
{
  for (unsigned int i = 0; i < limb::width; ++i) {
    for (unsigned int j1 = i; j1 < limb::width; ++j1) {
      for (unsigned int j2 = 0; j2 <= j1; ++j2) {
	const limb dividend_low = ~limb(0);
	const limb dividend_high(i ?
				 ((~limb(0).value()) >> (limb::width - i))
				 : 0);
	double_limb q(dividend_high, dividend_low);
	const limb divisor((static_cast<limb::limb_type>(1) << j1) |
			   (static_cast<limb::limb_type>(1) << j2));

	const limb r = q.div(divisor);
	if (q.high())
	  return -1;

	double_limb p = divisor * q.low();
	if (p.add(r))
	  return -2;

	if (p.high() != dividend_high || p.low() != dividend_low)
	  return -3;
      }
    }
  }

  return 0;
}


static int limbs_test1()
{
  const limbs l({1, 0, 1});
  const limbs c = l.complement();
  limb c2 = ~limb(0);
  --c2;
  if (c[0] != ~limb(0) || c[1] != ~limb(0) ||
      c[2] != c2)
    return -1;

  const limbs cc = c.complement();
  if (cc != l)
    return -2;

  return 0;
}

static int limbs_test2()
{
  const limbs l1{~limb(0), ~limb(0)};

  if (l1 + limbs{} != l1)
    return -1;

  if (l1 - limbs{} != l1)
    return -2;

  const limbs l2{limb(1)};
  const limbs s1 = l1 + l2;
  if (s1.size() != 3 ||
      s1[0] != limb(0) || s1[1] != limb(0) || s1[2] != limb(1))
    return -3;

  const limbs d1 = s1 - l2;
  if (d1 != l1)
    return -4;

  // Verify that operator+=() produces the same outcome.
  limbs s2 = l1;
  s2 += limbs{};
  if (s2 != l1)
    return -5;
  s2 += limb{0};
  if (s2 != l1)
    return -6;
  s2 += l2;
  if (s2 != s1)
    return -7;
  s2 = l1;
  s2 += limb{1};
  if (s2 != s1)
    return -8;

  // Verify that operator-=() produces the same outcome.
  limbs d2 = l1;
  d2 -= limb{0};
  if (d2 != l1)
    return -9;
  d2 = s1;
  d2 -= limb{1};
  if (d2 != d1)
    return -10;

  return 0;
}

static int limbs_test3()
{
  const limbs l1{};

  if (l1 + limbs{} != l1)
    return -1;

  if (l1 - limbs{} != l1)
    return -2;

  const limbs l2{limb(1)};
  const limbs d = l1 - l2;
  if (d.size() != 2 || d[0] != ~limb(0) || d[1] != ~limb(0))
    return -3;

  return 0;
}

static int limbs_test4()
{
  const limbs l1{};
  const limbs l2{~limb(0)};
  const limbs d = l1 - l2;
  if (d.size() != 2 || d[0] != limb(1) || d[1] != ~limb(0))
    return -1;

  return 0;
}

static int limbs_test5()
{
  const limbs l1{};
  const limbs l2{static_cast<limb::limb_type>(1) << (limb::width - 1)};
  const limbs d = l1 - l2;
  if (d.size() != 2 || d[0] != l2[0] || d[1] != ~limb(0))
    return -2;

  return 0;
}

static int limbs_test6()
{
  // Test limbs::operator+(size_type).
  const unsigned int size_t_width =
    std::numeric_limits<limbs::size_type>::digits;
  for (unsigned int i = 1; i < 2 * limbs::width_to_size(size_t_width); ++i) {
    limbs l1;
    l1.resize(i);
    const limbs::size_type l1_width = l1.width();
    l1.set_bits_below(l1_width, true);

    const limbs::size_type l2 = ~static_cast<limbs::size_type>(0);
    const limbs s1 = l1 + l2;

    const unsigned int min_width = std::min(size_t_width, i * limb::width);
    const unsigned int max_width = std::max(size_t_width, i * limb::width);
    if (s1.width() < max_width + 1)
      return -1;

    // Bit 0 shall be zero,
    // Bits up t min_width (excluding) are all 1
    // Bits from min_width to max_width (excluding) are all zero
    // Bit at max_width is 1.
    assert(max_width >= limb::width);
    if (s1[0] != (limb::mask(std::min(limb::width, min_width)) & ~limb{1})) {
      return -2;
    }

    if (min_width >= limb::width) {
      for (unsigned int j = 1; (j + 1) * limb::width <= min_width; ++j) {
	if (s1[j] != ~limb{0})
	  return -3;
      }

      const unsigned int j = min_width / limb::width;
      limb m = limb::mask(min_width - j * limb::width);
      if (max_width < (j + 1) * limb::width) {
	// The result's leading zero is also located in this limb.
	m |= limb{1} << (max_width - j * limb::width);
      }
      if (s1[j] != m)
	return -4;
    }

    for (unsigned int j = (min_width + limb::width - 1) / limb::width;
	 (j + 1) * limb::width <= max_width; ++j) {
      if (s1[j])
	return -5;
    }

    const unsigned int j = max_width / limb::width;
    // The case where min_width and max_width are in the same limb has
    // been handled above.
    if (min_width < j * limb::width) {
      if (s1[j] != limb{1} << (max_width - j * limb::width))
	return -6;
    }

    if (s1.is_any_set_at_or_above(max_width + 1))
      return -7;

    // Finally verify that operator+=() yields the same result.
    limbs s2 = l1;
    s2 += l2;
    if (s2 != s1)
      return -8;
  }

  return 0;
}

static int limbs_test7()
{
  // Calculate
  //   (b^m - 1) (b^n - 1)
  // for various combinations of m and n.
  // C.f. D. E. Knuth, 4.3.1 ("The classical algorithms").
  const limbs b{0, 1};

  static unsigned int max_n = 100;
  limbs _l1{1};
  for (unsigned int m = 1; m <= max_n - 1; ++m) {
    _l1 = _l1 * b;
    _l1.shrink_to_fit();
    const limbs l1 = _l1 - limbs{1};

    // First square. Verify that the outcome is correct and that
    // operator*() and operator*=(*this) produce the same result.
    limbs sq = l1;
    sq *= sq;
    if (sq != l1 * l1)
      return -1;

    if (sq[0] != limb(1))
      return -2;

    for (unsigned int j = 1; j < m; ++j) {
      if (sq[j])
	return -3;
    }

    if (sq[m] != ~limb(1))
      return -4;

    for (unsigned int j = m + 1; j < 2 * m; ++j) {
      if (sq[j] != ~limb(0))
	return -5;
    }

    limbs _l2 = _l1;
    for (unsigned int n = m + 1; n <= max_n; ++n) {
      _l2 = _l2 * b;
      const limbs l2 = _l2 - limbs{1};

      // Product, verify the known outcome.
      const limbs p = l1 * l2;
      if (p.size() < m + n - 1)
	return -6;

      if (p[0] != limb(1))
	return -7;

      for (unsigned int j = 1; j < m; ++j) {
	if (p[j])
	  return -8;
      }

      for (unsigned int j = m; j < n; ++j) {
	if (p[j] != ~limb(0))
	  return -9;
      }

      limb expected = ~limb(0);
      expected.sub(true);
      if (p[n] != expected)
	return -10;

      for (unsigned j = n + 1; j < m + n; ++j) {
	if (p[j] != ~limb(0))
	  return -11;
      }

      for (unsigned j = m + n + 1; j < p.size(); ++j) {
	if (p[j])
	  return -12;
      }

      // Test that operator*=() yields the same result.
      limbs _p = l1;
      _p *= l2;
      if (p != _p)
	return -13;

      // Test the division
      const limbs rest{1};
      const limbs v = p + rest;

      // (p + rest) / l2 should equal l1
      const auto &d1 = v / l2;
      if (d1.second != rest)
	return -14;
      if (d1.first != l1)
	return -15;

      // (p + rest) / l1 should equal l2
      const auto &d2 = v / l1;
      if (d2.second != rest)
	return -16;
      if (d2.first != l2)
	return -17;
    }
  }

  return 0;
}

static int limbs_test8()
{
  // Calculate
  //   (b^m - 1) (2^n - 1)
  // by means of limbs::operator*(limb) for various combinations of m
  // and n, with n < limb::width.
  // C.f. D. E. Knuth, 4.3.1 ("The classical algorithms").
  const limbs b{0, 1};

  static unsigned int max_m = 10;
  limbs _l1{1};
  for (unsigned int m = 1; m <= max_m; ++m) {
    _l1 = _l1 * b;
    _l1.shrink_to_fit();
    const limbs l1 = _l1 - limbs{1};
    for (unsigned int n = 1; n < limb::width; ++n) {
      const limb l2 = limb::mask(n);

      const limbs p1 = l1 * l2;
      if (p1.size() < m + 1)
	return -1;

      if (p1[0] != (~limb::mask(n) | limb{1}))
	return -2;

      for (unsigned int j = 1; j < m; ++j) {
	if (p1[j] != ~limb{0})
	  return -3;
      }

      if (p1[m] != (limb::mask(n) & ~limb{1}))
	return -4;

      // Verify that operator*= yields the same result.
      limbs p2 = l1;
      p2 *= l2;
      if (p2 != p1)
	return -5;
    }
  }

  return 0;
}

static int limbs_test9()
{
  for (limbs::size_type i = 0; i <= 3 * limb::width; ++i) {
    limbs dividend;
    dividend.resize(limbs::width_to_size(i));
    dividend.set_bits_below(i, true);
    for (limbs::size_type j1 = 0; j1 <= i; ++j1) {
      for (limbs::size_type j2 = 0; j2 <= j1; ++j2) {
	limbs divisor;
	divisor.resize(limbs::width_to_size(j1 + 1));
	divisor.set_bit(j1, true);
	divisor.set_bit(j2, true);

	std::pair<limbs, limbs> qr = dividend / divisor;
	const limbs &q = qr.first;
	const limbs &r = qr.second;
	limbs p = q * divisor;
	p = p + r;
	if (p != dividend)
	  return -1;
      }
    }
  }

  return 0;
}

static int limbs_test10()
{
  const limbs l = limbs({~limb(0), limb(1), limb(1)}).lshift(limb::width + 1);

  if (l[2] != limb(3))
    return -1;

  limb v1 = ~limb(0);
  v1.sub(true);
  if (l[1] != v1)
    return -2;

  if (l[0])
    return -3;

  return 0;
}

static int limbs_test11()
{
  const limbs l = limbs({~limb(0), ~limb(0)}).lshift(2 * limb::width);

  if (l[0])
    return -1;

  if (l[1])
    return -2;

  return 0;
}

static int limbs_test12()
{
  const limbs l = limbs({~limb(0), ~limb(0)}).lshift(0);

  if (l[0] != ~limb(0))
    return -1;

  if (l[1] != ~limb(0))
    return -2;

  return 0;
}

static int limbs_test13()
{
  const limbs l = limbs({1}).lshift(1);

  if (l[0] != limb(2))
    return -1;

  return 0;
}

static int limbs_test14()
{
  const limbs l =
    limbs({limb(1), ~limb(1), limb(1)}).rshift(limb::width + 1, true);

  if (l[2] != ~limb(0))
    return -1;

  if (l[1] != limb(static_cast<limb::limb_type>(1) << (limb::width - 1)))
    return -2;

  if (l[0] != ~limb(0))
    return -3;

  return 0;
}

static int limbs_test15()
{
  const limbs l = limbs({0, 0}).rshift(2 * limb::width, true);

  if (l[0] != ~limb(0))
    return -1;

  if (l[1] != ~limb(0))
    return -2;

  return 0;
}

static int limbs_test16()
{
  const limbs l = limbs({3}).rshift(1, false);

  if (l[0] != limb(1))
    return -1;

  return 0;
}

static int limbs_test17()
{
  const limbs l = limbs({3}).rshift(1, true);

  if (l[0] !=
      limb((static_cast<limb::limb_type>(1) << (limb::width - 1)) | 0x1))
    return -1;

  return 0;
}

static int limbs_test18()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);
    ls.set_bits_at_and_above(i, true);
    for (limbs::size_type j = 0; j < i; ++j) {
      if (ls.test_bit(j))
	return -1;
    }
    for (limbs::size_type j = i; j < 3 * limb::width; ++j) {
      if (!ls.test_bit(j))
	return -2;
    }
  }

  return 0;
}

static int limbs_test19()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(2);
    ls = ls - limbs({1});
    if (ls.size() != 3)
      return -1;

    ls.set_bits_at_and_above(i, false);
    for (limbs::size_type j = 0; j < i; ++j) {
      if (!ls.test_bit(j))
	return -2;
    }
    for (limbs::size_type j = i; j < 3 * limb::width; ++j) {
      if (ls.test_bit(j))
	return -3;
    }
  }

  return 0;
}

static int limbs_test20()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);
    ls.set_bits_below(i, true);
    for (limbs::size_type j = 0; j < i; ++j) {
      if (!ls.test_bit(j))
	return -1;
    }
    for (limbs::size_type j = i; j < 3 * limb::width; ++j) {
      if (ls.test_bit(j))
	return -2;
    }
  }

  return 0;
}

static int limbs_test21()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(2);
    ls = ls - limbs({1});
    if (ls.size() != 3)
      return -1;

    ls.set_bits_below(i, false);
    for (limbs::size_type j = 0; j < i; ++j) {
      if (ls.test_bit(j))
	return -2;
    }
    for (limbs::size_type j = i; j < 3 * limb::width; ++j) {
      if (!ls.test_bit(j))
	return -3;
    }
  }

  return 0;
}

static int limbs_test22()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);
    ls.set_bit(i, true);
    for (limbs::size_type j = 0; j <= i; ++j) {
      if (ls.is_any_set_below(j))
	return -1;
    }
    for (limbs::size_type j = i + 1; j <= 3 * limb::width; ++j) {
      if (!ls.is_any_set_below(j))
	return -2;
    }
  }

  return 0;
}

static int limbs_test23()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);
    ls.set_bit(i, true);
    for (limbs::size_type j = 0; j <= i; ++j) {
      if (!ls.is_any_set_at_or_above(j))
	return -1;
    }
    for (limbs::size_type j = i + 1; j <= 3 * limb::width; ++j) {
      if (ls.is_any_set_at_or_above(j))
	return -2;
    }
  }

  return 0;
}

static int limbs_test24()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);
    ls.set_bits_below(i, true);
    for (limbs::size_type j = 0; j <= i; ++j) {
      if (!ls.are_all_set_below(j))
	return -1;
    }
    for (limbs::size_type j = i + 1; j <= 3 * limb::width; ++j) {
      if (ls.are_all_set_below(j))
	return -2;
    }
  }

  return 0;
}

static int limbs_test25()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);
    ls.set_bits_at_and_above(i, true);
    for (limbs::size_type j = i; j < 3 * limb::width - 1; ++j) {
      if (!ls.are_all_set_at_or_above(j))
	return -1;
    }
    for (limbs::size_type j = i; j > 0; --j) {
      if (ls.are_all_set_at_or_above(j - 1))
	return -2;
    }
  }

  return 0;
}


static int limbs_test26()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);

    if (ls.fls())
      return -1;

    ls.set_bits_below(i + 1, true);
    if (ls.fls() != i + 1)
      return -2;
  }

  return 0;
}

static int limbs_test27()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);

    if (ls.ffs())
      return -1;

    ls.set_bits_at_and_above(i, true);
    if (ls.ffs() != i + 1)
      return -3;
  }

  return 0;
}

static int limbs_test28()
{
  for (limbs::size_type i = 0; i < 3 * limb::width; ++i) {
    limbs ls;
    ls.resize(3);

    ls.set_bits_below(i, true);
    if (ls.clrsb() != 3 * limb::width - i - 1)
      return -1;

    if ((~ls).clrsb() != 3 * limb::width - i - 1)
      return -2;
  }

  return 0;
}

static int limbs_test29()
{
  limbs ls1;
  ls1.resize(1);
  ls1.set_bits_at_and_above(0, true);

  limbs ls2;
  ls2.resize(2);
  ls2.set_bits_at_and_above(0, true);

  const limbs ls = ls1.add_signed(ls2);
  if (ls.size() != 1)
    return -1;

  if (ls.test_bit(0))
    return -2;

  if (!ls.are_all_set_at_or_above(1))
    return -3;

  return 0;
}

static int limbs_test30()
{
  limbs ls1;
  ls1.resize(1);
  ls1.set_bits_below(ls1.width() - 1, true);

  limbs ls2;
  ls2.resize(2);
  ls2.set_bit(0, true);

  const limbs ls = ls1.add_signed(ls2);
  if (ls.size() != 2)
    return -1;

  if (ls.is_any_set_below(ls1.width() - 1))
    return -2;

  if (!ls.test_bit(ls1.width() - 1))
    return -3;

  if (ls.is_any_set_at_or_above(ls1.width()))
    return -4;

  return 0;
}

static int limbs_test31()
{
  limbs ls1;
  ls1.resize(1);
  ls1.set_bit(0, true);

  limbs ls2;
  ls2.resize(2);
  ls2.set_bit(0, true);

  const limbs ls = ls1.add_signed(ls2);
  if (ls.size() != 1)
    return -1;

  if (ls.test_bit(0))
    return -2;

  if (!ls.test_bit(1))
    return -3;

  if (ls.is_any_set_at_or_above(2))
    return -4;

  return 0;
}

static int limbs_test32()
{
  limbs ls1;
  ls1.resize(1);
  ls1.set_bit(0, true);

  limbs ls2;
  ls2.resize(2);
  ls2.set_bits_at_and_above(0, true);

  const limbs ls = ls1.add_signed(ls2);
  if (ls.size() != 1)
    return -1;

  if (ls)
    return -2;

  return 0;
}

static int limbs_test33()
{
  limbs ls1;
  ls1.resize(1);
  ls1.set_bits_below(ls1.width() - 1, true);

  limbs ls2;
  ls2.resize(2);
  ls2.set_bits_at_and_above(0, true);

  const limbs ls = ls1.add_signed(ls2);
  if (ls.size() != 1)
    return -1;

  if (ls.test_bit(0))
    return -2;

  if (ls.test_bit(ls.width() - 1))
    return -3;

  return 0;
}


static int limbs_test34()
{
  unsigned int rev_unit_width_log2;
  for (rev_unit_width_log2 = 0;
       (1u << rev_unit_width_log2) < limb::width; ++rev_unit_width_log2) {
    const unsigned int units_per_limb = limb::width >> rev_unit_width_log2;
    for (limbs::size_type n_rev_units = 1; n_rev_units < 16 * units_per_limb;
	 ++n_rev_units) {
      const unsigned int rev_unit_width = 1 << rev_unit_width_log2;
      const limb::limb_type rev_unit_mask
	= (static_cast<limb::limb_type>(1) << rev_unit_width) - 1;
      limbs ls;
      ls.resize(limbs::width_to_size(16 * limb::width));
      for (limbs::size_type i = n_rev_units; i > 0; --i) {
	const limbs::size_type j =
	  ((i - 1) << rev_unit_width_log2) / limb::width;
	ls[j] = ((ls[j] << rev_unit_width) |
		 limb{static_cast<limb::limb_type>(i) & rev_unit_mask});
      }

      ls = ls.reverse(n_rev_units, rev_unit_width_log2);

      for (limbs::size_type i = n_rev_units; i > 0; --i) {
	const limbs::size_type j =
	  ((n_rev_units - i) << rev_unit_width_log2) / limb::width;
	if ((ls[j].value() & rev_unit_mask) !=
	    (static_cast<limb::limb_type>(i) & rev_unit_mask)) {
	  return -1;
	}
	ls[j] = ls[j] >> rev_unit_width;
      }
    }
  }

  assert((1u << rev_unit_width_log2) == limb::width);

  for(; (1u << rev_unit_width_log2) < 8 * limb::width; ++rev_unit_width_log2) {
    const unsigned int limbs_per_unit =
      (1u << rev_unit_width_log2) / limb::width;
    for (limbs::size_type n_rev_units = 1; n_rev_units < 12; ++n_rev_units) {
      limbs ls;
      ls.resize(n_rev_units * limbs_per_unit);
      for (limbs::size_type i = 0; i < ls.size(); ++i)
	ls[i] = limb{static_cast<limb::limb_type>(i + 1)};

      ls = ls.reverse(n_rev_units, rev_unit_width_log2);

      for (limbs::size_type i = 0; i < n_rev_units; ++i) {
	for (unsigned int j = 0; j < limbs_per_unit; ++j) {
	  const limbs::size_type k =
	    (n_rev_units - i - 1) * limbs_per_unit + j;
	  if (ls[k].value() != i * limbs_per_unit + j + 1)
	    return -1;
	}
      }
    }
  }

  return 0;
}


#define TEST_ENTRY(t)				\
  { "mpa:" #t, t }

static const struct test_entry {
  const char * name;
  int (*f)(void);
} tests[] = {
  TEST_ENTRY(limb_test1),
  TEST_ENTRY(limb_test2),
  TEST_ENTRY(limb_test3),
  TEST_ENTRY(limb_test4),
  TEST_ENTRY(limb_test5),
  TEST_ENTRY(limb_test6),
  TEST_ENTRY(limb_test7),
  TEST_ENTRY(limb_test8),
  TEST_ENTRY(limb_test9),
  TEST_ENTRY(limb_test10),
  TEST_ENTRY(limb_test11),
  TEST_ENTRY(limb_test12),
  TEST_ENTRY(double_limb_test1),
  TEST_ENTRY(double_limb_test2),
  TEST_ENTRY(double_limb_test3),
  TEST_ENTRY(double_limb_test4),
  TEST_ENTRY(double_limb_test5),
  TEST_ENTRY(double_limb_test6),
  TEST_ENTRY(limbs_test1),
  TEST_ENTRY(limbs_test2),
  TEST_ENTRY(limbs_test3),
  TEST_ENTRY(limbs_test4),
  TEST_ENTRY(limbs_test5),
  TEST_ENTRY(limbs_test6),
  TEST_ENTRY(limbs_test7),
  TEST_ENTRY(limbs_test8),
  TEST_ENTRY(limbs_test9),
  TEST_ENTRY(limbs_test10),
  TEST_ENTRY(limbs_test11),
  TEST_ENTRY(limbs_test12),
  TEST_ENTRY(limbs_test13),
  TEST_ENTRY(limbs_test14),
  TEST_ENTRY(limbs_test15),
  TEST_ENTRY(limbs_test16),
  TEST_ENTRY(limbs_test17),
  TEST_ENTRY(limbs_test18),
  TEST_ENTRY(limbs_test19),
  TEST_ENTRY(limbs_test20),
  TEST_ENTRY(limbs_test21),
  TEST_ENTRY(limbs_test22),
  TEST_ENTRY(limbs_test23),
  TEST_ENTRY(limbs_test24),
  TEST_ENTRY(limbs_test25),
  TEST_ENTRY(limbs_test26),
  TEST_ENTRY(limbs_test27),
  TEST_ENTRY(limbs_test28),
  TEST_ENTRY(limbs_test29),
  TEST_ENTRY(limbs_test30),
  TEST_ENTRY(limbs_test31),
  TEST_ENTRY(limbs_test32),
  TEST_ENTRY(limbs_test33),
  TEST_ENTRY(limbs_test34),
  { NULL, NULL }
};

int main(int, char*[])
{
  TestState s;

  for (test_entry const * t = &tests[0]; t->f; ++t) {
    if (t->f())
      s.fail(t->name);
    else
      s.pass(t->name);
  }

  return failed;
}
