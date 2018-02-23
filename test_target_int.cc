#include <dejagnu.h>
#include "target_int.hh"

using namespace suse::cp;

static const mpa::limbs::size_type long_width = 3 * mpa::limb::width;
static const mpa::limbs::size_type short_width = mpa::limb::width / 2;

static int target_int_test1(const mpa::limbs::size_type width)
{
  // Check proper overflow handling (wrap around) for unsigned
  // integers.
  mpa::limbs ls1;
  ls1.resize(mpa::limbs::width_to_size(width));
  ls1.set_bits_below(width, true);

  mpa::limbs ls2;
  ls2.resize(mpa::limbs::width_to_size(width));
  ls2.set_bit(0, true);

  const target_int t1(width, false, std::move(ls1));
  const target_int t2(width, false, std::move(ls2));
  const target_int t3 = t1 + t2;
  if (t3)
    return -1;

  const target_int t4 = t2 - t1;
  mpa::limbs ls4_expected;
  ls4_expected.resize(mpa::limbs::width_to_size(width));
  ls4_expected.set_bit(1, true);
  if (t4 != target_int(width, false, std::move(ls4_expected)))
    return -2;

  return 0;
}

static int target_int_test1_long()
{
  return target_int_test1(long_width);
}

static int target_int_test1_short()
{
  return target_int_test1(short_width);
}

static int target_int_test2(const mpa::limbs::size_type width)
{
  // Check proper overflow detection for signed integers.
  mpa::limbs ls1;
  ls1.resize(mpa::limbs::width_to_size(width));
  ls1.set_bits_below(width - 1, true);

  mpa::limbs ls2;
  ls2.resize(mpa::limbs::width_to_size(width));
  ls2.set_bit(0, true);

  const target_int t1(width - 1, true, std::move(ls1));
  const target_int t2(width - 1, true, std::move(ls2));
  bool pass = false;
  try {
    const target_int t3 = t1 + t2;
  } catch(...) {
    pass = true;
  }
  if (!pass)
    return -1;

  pass = false;
  try {
    const target_int t4 = t1 - (-t2);
  } catch(...) {
    pass = true;
  }
  if (!pass)
    return -2;

  const target_int t5 = -t1 - t2;
  mpa::limbs ls5_expected;
  ls5_expected.resize(mpa::limbs::width_to_size(width));
  ls5_expected.set_bits_at_and_above(width - 1, true);
  if (t5 != target_int(width - 1, true, std::move(ls5_expected)))
    return -3;

  pass = false;
  try {
    const target_int t6 = -t1 - t2 - t2;
  } catch(...) {
    pass = true;
  }
  if (!pass)
    return -4;

  return 0;
}

static int target_int_test2_long()
{
  return target_int_test2(long_width);
}

static int target_int_test2_short()
{
  return target_int_test2(short_width);
}

static int target_int_test3(const mpa::limbs::size_type width)
{
  // Check signed integer subtraction.
  mpa::limbs ls1;
  ls1.resize(mpa::limbs::width_to_size(width));
  ls1.set_bits_below(width - 1, true);

  mpa::limbs ls2;
  ls2.resize(mpa::limbs::width_to_size(width));
  ls2.set_bit(0, true);

  const target_int t1(width - 1, true, std::move(ls1));
  const target_int t2(width - 1, true, std::move(ls2));

  const target_int t3 = t2 - t1;
  mpa::limbs ls3_expected;
  ls3_expected.resize(mpa::limbs::width_to_size(width));
  ls3_expected.set_bits_at_and_above(width - 1, true);
  ls3_expected.set_bit(1, true);
  if (t3 != target_int(width - 1, true, std::move(ls3_expected)))
    return -1;

  if (t1 - t1)
    return -2;

  if (t2 - t2)
    return -3;

  if (t3 - t3)
    return -4;

  if (t3 - t2 != -t1)
    return -5;

  if (t3 + t1 != t2)
    return -6;

  if (t3 - t2 + t1)
    return -7;

  return 0;
}

static int target_int_test3_long()
{
  return target_int_test3(long_width);
}

static int target_int_test3_short()
{
  return target_int_test3(short_width);
}

static int target_int_test4(const mpa::limbs::size_type width)
{
  // Check unsigned integer multiplication, division and remainder
  // consistency.
  mpa::limbs ls1;
  ls1.resize(mpa::limbs::width_to_size(width));
  ls1.set_bits_below(width, true);
  const target_int t1(width, false, std::move(ls1));

  for (mpa::limbs::size_type i = 0; i < width; ++i) {
    for (mpa::limbs::size_type j = 0; j <= i; ++j) {
      mpa::limbs ls2;
      ls2.resize(mpa::limbs::width_to_size(width));
      ls2.set_bit(i, true);
      ls2.set_bit(j, true);
      const target_int t2(width, false, std::move(ls2));

      const target_int t3 = t1 / t2;
      const target_int t4 = t1 % t2;
      if (t3 * t2 + t4 != t1)
	return -1;
    }
  }

  return 0;
}

static int target_int_test4_long()
{
  return target_int_test4(long_width);
}

static int target_int_test4_short()
{
  return target_int_test4(short_width);
}

static int target_int_test5(const mpa::limbs::size_type width)
{
  // Check signed integer multiplication, division and remainder
  // consistency: negative dividend.
  mpa::limbs ls1;
  ls1.resize(mpa::limbs::width_to_size(width));
  ls1.set_bits_below(width - 1, true);
  const target_int t1(width - 1, true, std::move(ls1));

  for (mpa::limbs::size_type i = 0; i < width - 1; ++i) {
    for (mpa::limbs::size_type j = 0; j <= i; ++j) {
      mpa::limbs ls2;
      ls2.resize(mpa::limbs::width_to_size(width));
      ls2.set_bit(i, true);
      ls2.set_bit(j, true);
      const target_int t2(width - 1, true, std::move(ls2));
      const target_int q = -t1 / t2;
      const target_int r = -t1 % t2;

      if (q * t2 + r != -t1)
	return -1;
    }
  }

  return 0;
}

static int target_int_test5_long()
{
  return target_int_test5(long_width);
}

 static int target_int_test5_short()
{
  return target_int_test5(short_width);
}

static int target_int_test6(const mpa::limbs::size_type width)
{
  // Check signed integer multiplication, division and remainder
  // consistency: negative divisor.
  mpa::limbs ls1;
  ls1.resize(mpa::limbs::width_to_size(width));
  ls1.set_bits_below(width - 1, true);
  const target_int t1(width - 1, true, std::move(ls1));

  for (mpa::limbs::size_type i = 0; i < width - 1; ++i) {
    for (mpa::limbs::size_type j = 0; j <= i; ++j) {
      mpa::limbs ls2;
      ls2.resize(mpa::limbs::width_to_size(width));
      ls2.set_bit(i, true);
      ls2.set_bit(j, true);
      const target_int t2(width - 1, true, std::move(ls2));
      const target_int q = t1 / -t2;
      const target_int r = t1 % -t2;

      if (q * -t2 + r != t1)
	return -1;
    }
  }

  return 0;
}

static int target_int_test6_long()
{
  return target_int_test6(long_width);
}

 static int target_int_test6_short()
{
  return target_int_test6(short_width);
}

static int target_int_test7(const mpa::limbs::size_type width)
{
  // Check signed integer multiplication, division and remainder
  // consistency: negative dividend + divisor.
  mpa::limbs ls1;
  ls1.resize(mpa::limbs::width_to_size(width));
  ls1.set_bits_below(width - 1, true);
  const target_int t1(width - 1, true, std::move(ls1));

  for (mpa::limbs::size_type i = 0; i < width - 1; ++i) {
    for (mpa::limbs::size_type j = 0; j <= i; ++j) {
      mpa::limbs ls2;
      ls2.resize(mpa::limbs::width_to_size(width));
      ls2.set_bit(i, true);
      ls2.set_bit(j, true);
      const target_int t2(width - 1, true, std::move(ls2));
      const target_int q = -t1 / -t2;
      const target_int r = -t1 % -t2;

      if (q * -t2 + r != -t1)
	return -1;
    }
  }

  return 0;
}

static int target_int_test7_long()
{
  return target_int_test7(long_width);
}

 static int target_int_test7_short()
{
  return target_int_test7(short_width);
}

static int target_int_test8(const mpa::limbs::size_type width)
{
  // Check signed integer multiplication, division and remainder
  // consistency: positive dividend + divisor.
  mpa::limbs ls1;
  ls1.resize(mpa::limbs::width_to_size(width));
  ls1.set_bits_below(width - 1, true);
  const target_int t1(width - 1, true, std::move(ls1));

  for (mpa::limbs::size_type i = 0; i < width - 1; ++i) {
    for (mpa::limbs::size_type j = 0; j <= i; ++j) {
      mpa::limbs ls2;
      ls2.resize(mpa::limbs::width_to_size(width));
      ls2.set_bit(i, true);
      ls2.set_bit(j, true);
      const target_int t2(width - 1, true, std::move(ls2));
      const target_int q = t1 / t2;
      const target_int r = t1 % t2;

      if (q * t2 + r != t1)
	return -1;
    }
  }

  return 0;
}

static int target_int_test8_long()
{
  return target_int_test8(long_width);
}

 static int target_int_test8_short()
{
  return target_int_test8(short_width);
}


#define TEST_ENTRY(t)				\
  { #t, t }

static const struct test_entry {
  const char * name;
  int (*f)(void);
} tests[] = {
  TEST_ENTRY(target_int_test1_long),
  TEST_ENTRY(target_int_test1_short),
  TEST_ENTRY(target_int_test2_long),
  TEST_ENTRY(target_int_test2_short),
  TEST_ENTRY(target_int_test3_long),
  TEST_ENTRY(target_int_test3_short),
  TEST_ENTRY(target_int_test4_long),
  TEST_ENTRY(target_int_test4_short),
  TEST_ENTRY(target_int_test5_long),
  TEST_ENTRY(target_int_test5_short),
  TEST_ENTRY(target_int_test6_long),
  TEST_ENTRY(target_int_test6_short),
  TEST_ENTRY(target_int_test7_long),
  TEST_ENTRY(target_int_test7_short),
  TEST_ENTRY(target_int_test8_long),
  TEST_ENTRY(target_int_test8_short),
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
