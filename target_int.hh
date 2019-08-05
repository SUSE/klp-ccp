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

#ifndef TARGET_INT_HH
#define TARGET_INT_HH

#include "mp_arithmetic.hh"

namespace klp
{
  namespace ccp
  {
    class target_float;

    class target_int
    {
    public:
      target_int();

      target_int(const mpa::limbs::size_type prec, const bool is_signed,
		 mpa::limbs &&ls);

      target_int(const target_int &ti);
      target_int(target_int &&ti);

      target_int& operator=(target_int &&rhs);

      bool operator==(const target_int &op) const noexcept;

      bool operator!=(const target_int &op) const noexcept
      {
	return !(*this == op);
      }

      bool operator<(const target_int &op) const noexcept;

      bool operator>(const target_int &op) const noexcept
      {
	return op < *this;
      }

      bool operator<=(const target_int &op) const noexcept;

      bool operator>=(const target_int &op) const noexcept
      {
	return op <= *this;
      }

      explicit operator bool() const noexcept;

      target_int operator-() const;
      target_int operator~() const;

      target_int operator+(const target_int &op) const;
      target_int operator-(const target_int &op) const;
      target_int operator*(const target_int &op) const;
      target_int operator/(const target_int &op) const;
      target_int operator%(const target_int &op) const;

      target_int operator^(const target_int &op) const;
      target_int operator&(const target_int &op) const;
      target_int operator|(const target_int &op) const;

      target_int operator<<(const target_int &op) const;
      target_int operator>>(const target_int &op) const;

      target_int convert(const mpa::limbs::size_type prec,
			 const bool is_signed) const;

      target_float to_float(const mpa::limbs::size_type f_width,
			    const mpa::limbs::size_type e_width) const;

      const mpa::limbs& get_limbs() const noexcept
      {
	return _limbs;
      }

      bool is_negative() const noexcept;

      bool is_signed() const noexcept
      { return _is_signed; }

      mpa::limbs::size_type min_required_width() const noexcept;

      mpa::limbs::size_type width() const noexcept
      {
	return _prec + _is_signed;
      }

      static target_int create_zero(const mpa::limbs::size_type prec,
				    const bool is_signed);
      static target_int create_one(const mpa::limbs::size_type prec,
				   const bool is_signed);

    private:
      void _assert_same_prec_and_signedness(const target_int &op)
	const noexcept;

      mpa::limbs::size_type _n_limbs() const noexcept;

      bool _is_negative() const noexcept;

      void _propagate_sign_to_high(mpa::limbs &ls) const noexcept;
      void _clamp_unsigned(mpa::limbs &ls) const noexcept;

      mpa::limbs::size_type _to_size_type() const;

      mpa::limbs::size_type _prec;
      bool _is_signed;
      mpa::limbs _limbs;
    };
  }
}

#endif
