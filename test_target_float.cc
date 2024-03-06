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

#include <tuple>
#include <dejagnu.h>
#include "target_float.hh"
#include "target_int.hh"

using namespace klp::ccp;

static const mpa::limbs::size_type f_width = 24;
static const mpa::limbs::size_type e_width = 8;
static const mpa::limbs::size_type abs_e_min = 126;
static const mpa::limbs::size_type e_max = 127;

static int target_float_test1()
{
  // Test underflow to zero.
  mpa::limbs m({1});
  mpa::limbs e =
    mpa::limbs::from_size_type(abs_e_min + (f_width - 1) + 1).complement();

  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));
  if (!tf.is_zero())
    return -1;

  return 0;
}

static int target_float_test2()
{
  // Test various ways to create the minimum possible float distinct
  // from zero, i.e. 2^-(f_width - 1) * 2^-abs_e_min
  mpa::limbs ref_m({1});
  mpa::limbs ref_e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1).complement();

  const target_float ref_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(ref_m), std::move(ref_e));
  if (ref_tf.is_zero())
    return -1;

  for (mpa::limbs::size_type i = 1; i <= 8 * f_width; ++i) {
    mpa::limbs m;
    m.resize(mpa::limbs::width_to_size(i + 1));
    m.set_bit(i, true);

    mpa::limbs e =
      (mpa::limbs::from_size_type(abs_e_min + f_width - 1).complement() -
       mpa::limbs::from_size_type(i));

    const target_float tf =
      target_float::from_base2_exp(f_width, e_width,
				   std::move(m), std::move(e));
    if (tf != ref_tf)
      return -2;
  }

  return 0;
}

static int target_float_test3()
{
  // Test overflow to infinity.
  mpa::limbs e = mpa::limbs::from_size_type(e_max - (f_width - 1) + 1);

  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width));
  m.set_bit(f_width - 1, true);

  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));
  if (!tf.is_inf())
    return -1;

  return 0;
}

static int target_float_test4()
{
  // Test various ways to create the maximum possible float distinct
  // from infinity.
  mpa::limbs ref_e = mpa::limbs::from_size_type(e_max - (f_width - 1));
  mpa::limbs ref_m;
  ref_m.resize(mpa::limbs::width_to_size(f_width));
  ref_m.set_bits_below(f_width, true);

  const target_float ref_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(ref_m), std::move(ref_e));

  if (ref_tf.is_inf())
    return -1;

  // Positive exponents
  for (mpa::limbs::size_type i = 0; i < e_max -  (f_width - 1); ++i) {
    mpa::limbs e = mpa::limbs::from_size_type(i);

    mpa::limbs m;
    m.resize(mpa::limbs::width_to_size(f_width + e_max - (f_width - 1) - i));
    m.set_bits_below(f_width, true);
    m = m.lshift(e_max - (f_width - 1) - i);

    const target_float tf =
      target_float::from_base2_exp(f_width, e_width,
				   std::move(m), std::move(e));
    if (tf != ref_tf)
      return -2;
  }

  // Negative exponents
  for (mpa::limbs::size_type i = 1; i <= 8 * f_width; ++i) {
    mpa::limbs e = mpa::limbs::from_size_type(i).complement();

    mpa::limbs m;
    m.resize(mpa::limbs::width_to_size(f_width + e_max - (f_width - 1) + i));
    m.set_bits_below(f_width, true);
    m = m.lshift(e_max - (f_width - 1) + i);

    const target_float tf =
      target_float::from_base2_exp(f_width, e_width,
				   std::move(m), std::move(e));
    if (tf != ref_tf)
      return -3;
  }

  return 0;
}

static int target_float_test5()
{
  // Test proper rounding: ...1.01 shall be rounded downwards
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 2));
  m.set_bits_below(f_width + 2, true);
  m.set_bit(1, false);

  mpa::limbs e = mpa::limbs::from_size_type(2).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);

  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test6()
{
  // Test proper rounding: ...0.10 tie shall be rounded downwards (even).
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 2));
  m.set_bits_below(f_width + 2, true);
  m.set_bit(0, false);
  m.set_bit(2, false);

  mpa::limbs e = mpa::limbs::from_size_type(2).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);
  expected_m.set_bit(0, false);

  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test7()
{
  // Test proper rounding: ...1.10 tie shall be rounded to upwards (even).
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 2));
  m.set_bits_below(f_width + 2, true);
  m.set_bit(0, false);

  mpa::limbs e = mpa::limbs::from_size_type(2).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width + 1));
  expected_m.set_bit(f_width, true);

  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test8()
{
  // Test proper rounding: ...0.101 shall be rounded to upwards.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 3));
  m.set_bits_below(f_width + 3, true);
  m.set_bit(1, false);
  m.set_bit(3, false);

  mpa::limbs e = mpa::limbs::from_size_type(3).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);

  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test9()
{
  // Test proper rounding: ...0.11 shall be rounded to upwards.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 2));
  m.set_bits_below(f_width + 2, true);
  m.set_bit(2, false);

  mpa::limbs e = mpa::limbs::from_size_type(2).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);

  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test10()
{
  // Test proper rounding of subnormals: 01...1.011 shall be rounded
  // downwards
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 3));
  m.set_bits_below(f_width + 3 - 1, true);
  m.set_bit(2, false);

  mpa::limbs e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1 + 3).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width - 1, true);

  mpa::limbs expected_e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1).complement();

  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test11()
{
  // Test proper rounding of subnormals: 01...0.10 tie shall be
  // rounded downwards (even)
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 2));
  m.set_bits_below(f_width + 2 - 1, true);
  m.set_bit(0, false);
  m.set_bit(2, false);

  mpa::limbs e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1 + 2).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width - 1, true);
  expected_m.set_bit(0, false);

  mpa::limbs expected_e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1).complement();

  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test12()
{
  // Test proper rounding of subnormals: 01...1.10 tie shall be
  // rounded upwards (even).
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 2));
  m.set_bits_below(f_width + 2 - 1, true);
  m.set_bit(0, false);

  mpa::limbs e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1 + 2).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bit(f_width - 1, true);

  mpa::limbs expected_e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1).complement();

  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test13()
{
  // Test proper rounding of subnormals: 01...0.101 shall be rounded
  // upwards.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 3));
  m.set_bits_below(f_width + 3 - 1, true);
  m.set_bit(1, false);
  m.set_bit(3, false);

  mpa::limbs e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1 + 3).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width - 1, true);

  mpa::limbs expected_e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1).complement();

  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test14()
{
  // Test proper rounding of subnormals: 01...0.11 shall be rounded
  // upwards.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 2));
  m.set_bits_below(f_width + 2 - 1, true);
  m.set_bit(2, false);

  mpa::limbs e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1 + 2).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width - 1, true);

  mpa::limbs expected_e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1).complement();

  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test15()
{
  // Test proper rounding of subnormals: 01...0.111 shall be rounded
  // upwards.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width + 3));
  m.set_bits_below(f_width + 3 - 1, true);
  m.set_bit(3, false);

  mpa::limbs e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1 + 3).complement();
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width - 1, true);

  mpa::limbs expected_e =
    mpa::limbs::from_size_type(abs_e_min + f_width - 1).complement();

  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test16()
{
  // Test radix 10 conversion for special value with known, exact
  // outcome.
  mpa::limbs m({375});

  mpa::limbs e10 =
    mpa::limbs::from_size_type(2).complement();
  const target_float tf =
    target_float::from_base10_exp(f_width, e_width,
				  std::move(m), std::move(e10));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(4));
  expected_m.set_bits_below(4, true);

  mpa::limbs expected_e2 =
    mpa::limbs::from_size_type(2).complement();

  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e2));
  if (tf != expected_tf)
    return -1;

  return 0;
}


static int target_float_test17()
{
  // Check addition by adding f_width ones at different places
  mpa::limbs m({0});
  mpa::limbs e({0});
  target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  for (mpa::limbs::size_type i = 0; i < f_width; ++i) {
    mpa::limbs _m({1});
    mpa::limbs _e = mpa::limbs::from_size_type(i).complement();
    target_float _tf =
      target_float::from_base2_exp(f_width, e_width,
				   std::move(_m), std::move(_e));
    tf = tf + _tf;
  }


  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);

  mpa::limbs expected_e = mpa::limbs::from_size_type(f_width - 1).complement();
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test18()
{
  // Check subtraction by adding f_width negative ones at different places
  mpa::limbs m({0});
  mpa::limbs e({0});
  target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  for (mpa::limbs::size_type i = 0; i < f_width; ++i) {
    mpa::limbs _m({1});
    mpa::limbs _e = mpa::limbs::from_size_type(i).complement();
    target_float _tf =
      target_float::from_base2_exp(f_width, e_width,
				   std::move(_m), std::move(_e));
    tf = tf + (-_tf);
  }


  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);

  mpa::limbs expected_e = mpa::limbs::from_size_type(f_width - 1).complement();
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (-tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test19()
{
  // Check subtraction by subtracting f_width ones at different places
  mpa::limbs m({0});
  mpa::limbs e({0});
  target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  for (mpa::limbs::size_type i = 0; i < f_width; ++i) {
    mpa::limbs _m({1});
    mpa::limbs _e = mpa::limbs::from_size_type(i).complement();
    target_float _tf =
      target_float::from_base2_exp(f_width, e_width,
				   std::move(_m), std::move(_e));
    tf = tf - _tf;
  }


  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);

  mpa::limbs expected_e = mpa::limbs::from_size_type(f_width - 1).complement();
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (-tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test20()
{
  // Check subtraction by subtracting f_width negative ones at different places
  mpa::limbs m({0});
  mpa::limbs e({0});
  target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  for (mpa::limbs::size_type i = 0; i < f_width; ++i) {
    mpa::limbs _m({1});
    mpa::limbs _e = mpa::limbs::from_size_type(i).complement();
    target_float _tf =
      target_float::from_base2_exp(f_width, e_width,
				   std::move(_m), std::move(_e));
    tf = tf - (-_tf);
  }


  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);

  mpa::limbs expected_e = mpa::limbs::from_size_type(f_width - 1).complement();
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test21()
{
  // Check that multiplication's exponent handling is correct.
  mpa::limbs m1({1});
  mpa::limbs e1 ({1});
  const target_float tf1 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m1), std::move(e1));

  mpa::limbs m2({1});
  mpa::limbs e2 = mpa::limbs::from_size_type(1).complement();
  const target_float tf2 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m2), std::move(e2));

  const target_float tf = tf1 * tf2;

  mpa::limbs expected_m({1});
  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test22()
{
  // Check that multiplication's sign handling is correct.
  mpa::limbs m1({1});
  mpa::limbs e1 ({1});
  const target_float tf1 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m1), std::move(e1));

  mpa::limbs m2({1});
  mpa::limbs e2 = mpa::limbs::from_size_type(1).complement();
  const target_float tf2 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m2), std::move(e2));

  const target_float tf = -tf1 * tf2;

  mpa::limbs expected_m({1});
  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (-tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test23()
{
  // Check that multiplication's sign handling is correct.
  mpa::limbs m1({1});
  mpa::limbs e1 ({1});
  const target_float tf1 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m1), std::move(e1));

  mpa::limbs m2({1});
  mpa::limbs e2 = mpa::limbs::from_size_type(1).complement();
  const target_float tf2 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m2), std::move(e2));

  const target_float tf = tf1 * -tf2;

  mpa::limbs expected_m({1});
  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (-tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test24()
{
  // Check that multiplication's sign handling is correct.
  mpa::limbs m1({1});
  mpa::limbs e1 ({1});
  const target_float tf1 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m1), std::move(e1));

  mpa::limbs m2({1});
  mpa::limbs e2 = mpa::limbs::from_size_type(1).complement();
  const target_float tf2 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m2), std::move(e2));

  const target_float tf = -tf1 * -tf2;

  mpa::limbs expected_m({1});
  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}


static int target_float_test25()
{
  // Check that divison's exponent handling is correct.
  mpa::limbs m1({1});
  mpa::limbs e1 ({1});
  const target_float tf1 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m1), std::move(e1));

  mpa::limbs m2({1});
  mpa::limbs e2 = mpa::limbs::from_size_type(1).complement();
  const target_float tf2 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m2), std::move(e2));

  const target_float tf = tf1 / tf2;

  mpa::limbs expected_m({1});
  mpa::limbs expected_e({2});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test26()
{
  // Check that divison's sign handling is correct.
  mpa::limbs m1({1});
  mpa::limbs e1 ({1});
  const target_float tf1 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m1), std::move(e1));

  mpa::limbs m2({1});
  mpa::limbs e2 = mpa::limbs::from_size_type(1).complement();
  const target_float tf2 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m2), std::move(e2));

  const target_float tf = -tf1 / tf2;

  mpa::limbs expected_m({1});
  mpa::limbs expected_e({2});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (-tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test27()
{
  // Check that divison's sign handling is correct.
  mpa::limbs m1({1});
  mpa::limbs e1 ({1});
  const target_float tf1 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m1), std::move(e1));

  mpa::limbs m2({1});
  mpa::limbs e2 = mpa::limbs::from_size_type(1).complement();
  const target_float tf2 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m2), std::move(e2));

  const target_float tf = tf1 / -tf2;

  mpa::limbs expected_m({1});
  mpa::limbs expected_e({2});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (-tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test28()
{
  // Check that divison's sign handling is correct.
  mpa::limbs m1({1});
  mpa::limbs e1 ({1});
  const target_float tf1 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m1), std::move(e1));

  mpa::limbs m2({1});
  mpa::limbs e2 = mpa::limbs::from_size_type(1).complement();
  const target_float tf2 =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m2), std::move(e2));

  const target_float tf = -tf1 / -tf2;

  mpa::limbs expected_m({1});
  mpa::limbs expected_e({2});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(expected_m), std::move(expected_e));
  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test29()
{
  // Check conversion to integer for +0.
  mpa::limbs m;
  mpa::limbs e;
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  const target_int ti = tf.to_int(31, true);

  mpa::limbs ls;
  ls.resize(mpa::limbs::width_to_size(31 + 1));
  const target_int expected_ti(31, true, std::move(ls));

  if (ti != expected_ti)
    return -1;

  return 0;
}

static int target_float_test30()
{
  // Check conversion to integer for -0.
  mpa::limbs m;
  mpa::limbs e;
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  const target_int ti = (-tf).to_int(31, true);

  mpa::limbs ls;
  ls.resize(mpa::limbs::width_to_size(31 + 1));
  const target_int expected_ti(31, true, std::move(ls));

  if (ti != expected_ti)
    return -1;

  return 0;
}

static int target_float_test31()
{
  // Check conversion to integer for +1.
  mpa::limbs m({1});
  mpa::limbs e;
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  const target_int ti = tf.to_int(31, true);

  mpa::limbs ls;
  ls.resize(mpa::limbs::width_to_size(31 + 1));
  ls.set_bit(0, true);
  const target_int expected_ti(31, true, std::move(ls));

  if (ti != expected_ti)
    return -1;

  return 0;
}

static int target_float_test32()
{
  // Check conversion to integer for -1.
  mpa::limbs m({1});
  mpa::limbs e;
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  const target_int ti = (-tf).to_int(31, true);

  mpa::limbs ls;
  ls.resize(mpa::limbs::width_to_size(31 + 1));
  ls.set_bits_at_and_above(0, true);
  const target_int expected_ti(31, true, std::move(ls));

  if (ti != expected_ti)
    return -1;

  return 0;
}

static int target_float_test33()
{
  // Check overflow reporting in conversion to signed integer.
  mpa::limbs m({1});
  mpa::limbs e({31});
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  bool pass = false;
  try {
    const target_int ti = tf.to_int(31, true);
  } catch(...) {
    pass = true;
  }

  if (!pass)
    return -1;

  return 0;
}

static int target_float_test34()
{
  // Check overflow handling in conversion to unsigned integer.
  mpa::limbs m({1});
  mpa::limbs e({32});
  const target_float tf =
    target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e));

  const target_int ti = tf.to_int(32, false);

  mpa::limbs ls;
  ls.resize(mpa::limbs::width_to_size(32));
  const target_int expected_ti(32, false, std::move(ls));

  if (ti != expected_ti)
    return -1;

  return 0;
}

static int target_float_test35()
{
  // Check conversion to float with one less exponent width.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width));
  m.set_bits_below(f_width, true);

  mpa::limbs e({0});
  const target_float tf =
    (target_float::from_base2_exp(f_width, e_width,
				 std::move(m), std::move(e))
     .convert(f_width, e_width - 1));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);
  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width - 1,
				 std::move(expected_m), std::move(expected_e));

  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test36()
{
  // Check conversion to float with one more exponent width.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width));
  m.set_bits_below(f_width, true);

  mpa::limbs e({0});
  const target_float tf =
    (target_float::from_base2_exp(f_width, e_width,
				  std::move(m), std::move(e))
     .convert(f_width, e_width + 1));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width));
  expected_m.set_bits_below(f_width, true);
  mpa::limbs expected_e({0});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width, e_width + 1,
				 std::move(expected_m), std::move(expected_e));

  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test37()
{
  // Check conversion to float with one less significand width.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width));
  m.set_bits_below(f_width, true);

  mpa::limbs e({0});
  const target_float tf =
    (target_float::from_base2_exp(f_width, e_width,
				  std::move(m), std::move(e))
     .convert(f_width - 1, e_width));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width - 1));
  expected_m.set_bit(f_width - 2, true);
  mpa::limbs expected_e({2});
  const target_float expected_tf =
    target_float::from_base2_exp(f_width - 1, e_width,
				 std::move(expected_m), std::move(expected_e));

  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test38()
{
  // Check conversion to float with one more significand width.
  mpa::limbs m;
  m.resize(mpa::limbs::width_to_size(f_width));
  m.set_bits_below(f_width, true);

  mpa::limbs e({0});
  const target_float tf =
    (target_float::from_base2_exp(f_width, e_width,
				  std::move(m), std::move(e))
     .convert(f_width + 1, e_width));

  mpa::limbs expected_m;
  expected_m.resize(mpa::limbs::width_to_size(f_width + 1));
  expected_m.set_bits_below(f_width + 1, true);
  expected_m.set_bit(0, false);
  mpa::limbs expected_e = mpa::limbs::from_size_type(1).complement();
  const target_float expected_tf =
    target_float::from_base2_exp(f_width + 1, e_width,
				 std::move(expected_m), std::move(expected_e));

  if (tf != expected_tf)
    return -1;

  return 0;
}

static int target_float_test39()
{
  auto &&create_max_float =
    [](const mpa::limbs::size_type f_width,
       const mpa::limbs::size_type e_width) {
      mpa::limbs m;
      m.resize(mpa::limbs::width_to_size(f_width));
      m.set_bits_below(f_width, true);

      mpa::limbs e;
      e.resize(mpa::limbs::width_to_size(e_width));
      // bias is 2^{e_width - 1} - 1;
      e.set_bits_below(e_width - 1, true);
      e = e - mpa::limbs::from_size_type(f_width - 1);

      return target_float::from_base2_exp(f_width, e_width,
					  std::move(m), std::move(e));
    };

  auto &&create_denorm_min_float =
    [](const mpa::limbs::size_type f_width,
       const mpa::limbs::size_type e_width) {
      mpa::limbs m;
      m.resize(mpa::limbs::width_to_size(f_width));
      m.set_bit(0, true);

      mpa::limbs e;
      e.resize(mpa::limbs::width_to_size(e_width));
      // bias is 2^{e_width - 1} - 1;
      e.set_bits_below(e_width - 1, true);
      e += f_width - 1;
      e -= mpa::limb{1};
      e = e.complement();

      return target_float::from_base2_exp(f_width, e_width,
					  std::move(m), std::move(e));
    };

  auto &&create_epsilon_float =
    [](const mpa::limbs::size_type f_width,
       const mpa::limbs::size_type e_width) {
      mpa::limbs m;
      m.resize(mpa::limbs::width_to_size(f_width));
      m.set_bit(0, true);

      mpa::limbs e = mpa::limbs::from_size_type(f_width - 1);
      e.resize(mpa::limbs::width_to_size(e_width));
      e = e.complement();

      return target_float::from_base2_exp(f_width, e_width,
					  std::move(m), std::move(e));
    };

  auto &&fmt_float =
    [](const target_float &f, const std::size_t ndigits) {
      std::string sf;
      std::string se;

      std::tie(sf, se) = f.to_decimal(ndigits);
      if (!se.empty())
	return sf + 'e' + se;
      else
	return sf;
    };

  // The strings below are identical to what GCC emits for
  // __<TYPE>_MAX__, __<TYPE>_DENORM_MIN__ and __<TYPE>_EPSILON__ with
  // the given combinations of significand and exponent width each.
  static struct {
    mpa::limbs::size_type f_width;
    mpa::limbs::size_type e_width;
    const char *expected_max;
    const char *expected_denorm_min;
    const char *expected_epsilon;
  } tests[] = {
    {
      24, 8, // single precision
      "3.40282346638528859811704183484516925e+38",
      "1.40129846432481707092372958328991613e-45",
      "1.19209289550781250000000000000000000e-7"
    },
    {
      53, 11, // double precision
      "1.79769313486231570814527423731704357e+308",
      "4.94065645841246544176568792868221372e-324",
      "2.22044604925031308084726333618164062e-16"
    },
    {
      64, 15, // extended 80 bit precision
      "1.18973149535723176502126385303097021e+4932",
      "3.64519953188247460252840593361941982e-4951",
      "1.08420217248550443400745280086994171e-19"
    },
    {
      113, 15, // extended 128 bit precision
      "1.18973149535723176508575932662800702e+4932",
      "6.47517511943802511092443895822764655e-4966",
      "1.92592994438723585305597794258492732e-34"
    },
    { 0, 0, NULL, NULL, NULL}
  };

  // GCC's __FLT128_DECIMAL_DIG__ equals 36 on x86_64 and it uses that
  // for all floating point values, independent of the type.
  const unsigned int ndigits = 36;

  for (const auto *t = tests; t->f_width; ++t) {
    if (fmt_float(create_max_float(t->f_width, t->e_width),
		  ndigits) !=
	t->expected_max) {
      return -1;
    }

    if (fmt_float(create_denorm_min_float(t->f_width, t->e_width),
		  ndigits) !=
	t->expected_denorm_min) {
      return -2;
    }

    if (fmt_float(create_epsilon_float(t->f_width, t->e_width),
		  ndigits) !=
	t->expected_epsilon) {
      return -3;
    }
  }

  return 0;
}


#define TEST_ENTRY(t)				\
  { #t, t }

static const struct test_entry {
  const char * name;
  int (*f)(void);
} tests[] = {
  TEST_ENTRY(target_float_test1),
  TEST_ENTRY(target_float_test2),
  TEST_ENTRY(target_float_test3),
  TEST_ENTRY(target_float_test4),
  TEST_ENTRY(target_float_test5),
  TEST_ENTRY(target_float_test6),
  TEST_ENTRY(target_float_test7),
  TEST_ENTRY(target_float_test8),
  TEST_ENTRY(target_float_test9),
  TEST_ENTRY(target_float_test10),
  TEST_ENTRY(target_float_test11),
  TEST_ENTRY(target_float_test12),
  TEST_ENTRY(target_float_test13),
  TEST_ENTRY(target_float_test14),
  TEST_ENTRY(target_float_test15),
  TEST_ENTRY(target_float_test16),
  TEST_ENTRY(target_float_test17),
  TEST_ENTRY(target_float_test18),
  TEST_ENTRY(target_float_test19),
  TEST_ENTRY(target_float_test20),
  TEST_ENTRY(target_float_test21),
  TEST_ENTRY(target_float_test22),
  TEST_ENTRY(target_float_test23),
  TEST_ENTRY(target_float_test24),
  TEST_ENTRY(target_float_test25),
  TEST_ENTRY(target_float_test26),
  TEST_ENTRY(target_float_test27),
  TEST_ENTRY(target_float_test28),
  TEST_ENTRY(target_float_test29),
  TEST_ENTRY(target_float_test30),
  TEST_ENTRY(target_float_test31),
  TEST_ENTRY(target_float_test32),
  TEST_ENTRY(target_float_test33),
  TEST_ENTRY(target_float_test34),
  TEST_ENTRY(target_float_test35),
  TEST_ENTRY(target_float_test36),
  TEST_ENTRY(target_float_test37),
  TEST_ENTRY(target_float_test38),
  TEST_ENTRY(target_float_test39),
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
