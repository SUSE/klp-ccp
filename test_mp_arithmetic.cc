#include <dejagnu.h>
#include "mp_arithmetic.hh"

using namespace suse::cp::mpa;

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
  const limbs s = l1 + l2;
  if (s.size() != 3 ||
      s[0] != limb(0) || s[1] != limb(0) || s[2] != limb(1))
    return -3;

  const limbs d = s - l2;
  if (d != l1)
    return -4;

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
  // Calculate
  //   (b^m - 1) (b^n - 1)
  // for various combinations of m and n.
  // C.f. D. E. Knuth, 4.3.1 ("The classical algorithms").
  const limbs b{0, 1};

  static unsigned int max_n = 100;
  limbs _l1{1};
  for (unsigned int m = 1; m <= max_n - 1; ++m) {
    _l1 = _l1 * b;
    const limbs l1 = _l1 - limbs{1};

    limbs _l2 = _l1;
    for (unsigned int n = m + 1; n <= max_n; ++n) {
      _l2 = _l2 * b;
      const limbs l2 = _l2 - limbs{1};

      // Product, verify the known outcome.
      const limbs p = l1 * l2;
      if (p.size() < m + n - 1)
	return -1;

      if (p[0] != limb(1))
	return -2;

      for (unsigned int j = 1; j < m; ++j) {
	if (p[j])
	  return -3;
      }

      for (unsigned int j = m; j < n; ++j) {
	if (p[j] != ~limb(0))
	  return -4;
      }

      limb expected = ~limb(0);
      expected.sub(true);
      if (p[n] != expected)
	return -5;

      for (unsigned j = n + 1; j < m + n; ++j) {
	if (p[j] != ~limb(0))
	  return -6;
      }

      for (unsigned j = m + n + 1; j < p.size(); ++j) {
	if (p[j])
	  return -7;
      }

      // Test the division
      const limbs rest{1};
      const limbs v = p + rest;

      // (p + rest) / l2 should equal l1
      const auto &d1 = v / l2;
      if (d1.second != rest)
	return -8;
      if (d1.first != l1)
	return -9;

      // (p + rest) / l1 should equal l2
      const auto &d2 = v / l1;
      if (d2.second != rest)
	return -10;
      if (d2.first != l2)
	return -11;
    }
  }

  return 0;
}

static int limbs_test7()
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

static int limbs_test8()
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

static int limbs_test9()
{
  const limbs l = limbs({~limb(0), ~limb(0)}).lshift(2 * limb::width);

  if (l[0])
    return -1;

  if (l[1])
    return -2;

  return 0;
}

static int limbs_test10()
{
  const limbs l = limbs({~limb(0), ~limb(0)}).lshift(0);

  if (l[0] != ~limb(0))
    return -1;

  if (l[1] != ~limb(0))
    return -2;

  return 0;
}

static int limbs_test11()
{
  const limbs l = limbs({1}).lshift(1);

  if (l[0] != limb(2))
    return -1;

  return 0;
}

static int limbs_test12()
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

static int limbs_test13()
{
  const limbs l = limbs({0, 0}).rshift(2 * limb::width, true);

  if (l[0] != ~limb(0))
    return -1;

  if (l[1] != ~limb(0))
    return -2;

  return 0;
}

static int limbs_test14()
{
  const limbs l = limbs({3}).rshift(1, false);

  if (l[0] != limb(1))
    return -1;

  return 0;
}

static int limbs_test15()
{
  const limbs l = limbs({3}).rshift(1, true);

  if (l[0] !=
      limb((static_cast<limb::limb_type>(1) << (limb::width - 1)) | 0x1))
    return -1;

  return 0;
}

static int limbs_test16()
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

static int limbs_test17()
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

static int limbs_test18()
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

static int limbs_test19()
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

static int limbs_test20()
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

static int limbs_test21()
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

static int limbs_test22()
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

static int limbs_test23()
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


static int limbs_test24()
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

static int limbs_test25()
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

static int limbs_test26()
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
