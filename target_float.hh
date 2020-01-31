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

#ifndef TARGET_FLOAT_HH
#define TARGET_FLOAT_HH

#include "mp_arithmetic.hh"

namespace klp
{
  namespace ccp
  {
    class target_int;

    class target_float
    {
    public:
      target_float();

      bool operator==(const target_float &op) const noexcept;

      bool operator!=(const target_float &op) const noexcept
      {
	return !(*this == op);
      }

      bool operator<(const target_float &op) const noexcept;

      bool operator>(const target_float &op) const noexcept
      {
	return op < *this;
      }

      bool operator<=(const target_float &op) const noexcept
      {
	return (*this == op || *this < op);
      }

      bool operator>=(const target_float &op) const noexcept
      {
	return op <= *this;
      }

      target_float operator-() const;

      target_float operator+(const target_float &op) const;
      target_float operator-(const target_float &op) const;
      target_float operator*(const target_float &op) const;
      target_float operator/(const target_float &op) const;

      static target_float from_base10_exp(const mpa::limbs::size_type f_width,
					  const mpa::limbs::size_type e_width,
					  mpa::limbs &&m, mpa::limbs &&e10);
      static target_float from_base2_exp(const mpa::limbs::size_type f_width,
					 const mpa::limbs::size_type e_width,
					 mpa::limbs &&m, mpa::limbs &&e2);

      target_float convert(const mpa::limbs::size_type f_width,
			   const mpa::limbs::size_type e_width) const;

      target_int to_int(const mpa::limbs::size_type prec,
			const bool is_signed) const;

      bool is_nan() const noexcept;
      bool is_inf() const noexcept;
      bool is_zero() const noexcept;

      std::pair<std::string, std::string> to_decimal(const std::size_t digits)
	const;

    private:
      target_float(const mpa::limbs::size_type f_width,
		   const mpa::limbs::size_type e_width,
		   const bool is_negative,
		   const mpa::limbs &f,
		   const mpa::limbs &e);

      target_float(const mpa::limbs::size_type f_width,
		   const mpa::limbs::size_type e_width,
		   const bool is_negative,
		   mpa::limbs &&f,
		   mpa::limbs &&e);

      static mpa::limbs _bias(const mpa::limbs::size_type e_width);

      void _assert_same_config(const target_float &op) const noexcept;

      target_float _add(const target_float &op, const bool negate_op) const;

      static void _normalize(const mpa::limbs::size_type f_width,
			     const mpa::limbs::size_type e_width,
			     mpa::limbs &f, mpa::limbs &e);

      static bool _round_at(mpa::limbs &f, const mpa::limbs::size_type pos);

      static target_float _create_nan(const mpa::limbs::size_type f_width,
				      const mpa::limbs::size_type e_width);
      static target_float _create_inf(const mpa::limbs::size_type f_width,
				      const mpa::limbs::size_type e_width,
				      const bool is_negative);
      static target_float _create_zero(const mpa::limbs::size_type f_width,
				       const mpa::limbs::size_type e_width,
				       const bool is_negative);

      mpa::limbs::size_type _f_width;
      mpa::limbs::size_type _e_width;
      mpa::limbs _f;
      mpa::limbs _e;
      bool _is_negative;
    };
  }
}

#endif
