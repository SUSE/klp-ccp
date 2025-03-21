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

#ifndef MP_ARITHMETIC_HH
#define MP_ARITHMETIC_HH

#include <limits>
#include <vector>
#include <utility>
#include <initializer_list>
#include <string>
#include <stdexcept>

namespace klp
{
  namespace ccp
  {
    namespace mpa
    {
      class double_limb;

      class limb
      {
      public:
	typedef unsigned int limb_type;

	static constexpr unsigned int width =
	  std::numeric_limits<limb_type>::digits;

	constexpr limb() noexcept
	  : _value(0)
	{}

	explicit constexpr limb(const limb_type value) noexcept
	  : _value(value)
	{}

	constexpr limb_type value() const noexcept
	{ return _value; }

	explicit constexpr operator bool() const noexcept
	{ return !!_value; }

	constexpr const limb operator~() const noexcept
	{ return limb(~_value); }

	constexpr bool operator<(const limb &op) const noexcept
	{ return _value < op._value; }

	constexpr bool operator>(const limb &op) const noexcept
	{ return op < *this; }

	constexpr bool operator<=(const limb &op) const noexcept
	{ return _value <= op._value; }

	constexpr bool operator>=(const limb &op) const noexcept
	{ return op <= *this; }

	constexpr bool operator==(const limb &op) const noexcept
	{ return _value == op._value; }

	constexpr bool operator!=(const limb &op) const noexcept
	{ return !(*this == op); }

	limb& operator--() noexcept
	{
	  --_value;
	  return *this;
	}

	limb& operator|=(const limb &op) noexcept
	{
	  _value |= op._value;
	  return *this;
	}

	const limb operator<<(const std::size_t n) const noexcept
	{
	  return limb{_value << n};
	}

	const limb operator>>(const std::size_t n) const noexcept
	{
	  return limb{_value >> n};
	}

	const limb operator&(const limb &op) const noexcept
	{
	  return limb{_value & op._value};
	}

	const limb operator|(const limb &op) const noexcept
	{
	  return limb(_value | op._value);
	}

	unsigned int fls() const noexcept;
	unsigned int ffs() const noexcept;
	unsigned int clz() const noexcept;
	unsigned int clrsb() const noexcept;

	bool add(const bool op) noexcept;
	bool add(const limb &op) noexcept;
	bool sub(const bool op) noexcept;
	bool sub(const limb &op) noexcept;
	const double_limb operator*(const limb &op) const noexcept;

	static constexpr const limb mask(const unsigned int w)
	{
	  return limb(w == width ? ~static_cast<limb_type>(0) :
		      (static_cast<limb_type>(1) << w) - 1);
	}

	const limb reverse(const unsigned int rev_unit_width_log2)
	  const noexcept;

      private:
	limb_type _value;
      };

      class double_limb
      {
      public:
	constexpr double_limb(const limb &high, const limb &low) noexcept
	  : _high(high), _low(low)
	{}

	constexpr const limb& high() const noexcept
	{ return _high; }

	limb& high() noexcept
	{ return _high; }

	constexpr const limb& low() const noexcept
	{ return _low; }

	limb& low() noexcept
	{ return _low; }

	bool add(const limb &op) noexcept;
	limb div(const limb divisor);

      private:
	limb _high;
	limb _low;
      };

      class limbs
      {
      private:
	typedef std::vector<limb> _limbs_type;

      public:
	typedef _limbs_type::size_type size_type;

	limbs();
	limbs(const std::initializer_list<limb::limb_type> &il);
	limbs(const std::initializer_list<limb> &il);
	explicit limbs(const size_type n);
	limbs(const limbs &ls);
	limbs(limbs &&ls) noexcept;

	limbs& operator=(const limbs &rhs);

	bool operator==(const limbs &op) const noexcept;

	bool operator!=(const limbs &op) const noexcept
	{
	  return !(*this == op);
	}

	bool operator<(const limbs &op) const noexcept;

	bool operator>(const limbs &op) const noexcept
	{
	  return op < *this;
	}

	bool operator<=(const limbs &op) const noexcept;

	bool operator>=(const limbs &op) const noexcept
	{
	  return op <= *this;
	}

	explicit operator bool() const noexcept;

	const limb& operator[](const size_type i) const noexcept
	{ return _limbs[i]; }

	limb& operator[](const size_type i) noexcept
	{ return _limbs[i]; }

	bool empty() const noexcept
	{ return _limbs.empty(); }

	size_type size() const noexcept
	{ return _limbs.size(); }

	size_type width() const noexcept
	{ return _limbs.size() * limb::width; }

	limbs complement() const;
	limbs operator+(const limbs &op) const;
	limbs operator+(size_type op) const;
	limbs& operator+=(const limbs &op);
	limbs& operator+=(const limb op);
	limbs& operator+=(size_type op);
	limbs operator-(const limbs &op) const;
	limbs& operator-=(const limb op);
	limbs operator*(const limbs &op) const;
	limbs operator*(const limb op) const;
	limbs& operator*=(const limbs &op);
	limbs& operator*=(const limb op);

	std::pair<limbs, limbs>
	operator/(const limbs &op) const;

	limbs operator~() const;

	limbs operator^(const limbs &op) const;
	limbs operator&(const limbs &op) const;
	limbs operator|(const limbs &op) const;

	limbs lshift(const size_type distance) const;
	limbs rshift(const size_type distance, const bool fill_value) const;

	limbs add_signed(const mpa::limbs &op) const;

	void set_bits_at_and_above(const size_type pos, const bool value)
	  noexcept;
	void set_bits_below(const size_type pos, const bool value) noexcept;

	void resize(const size_type n);
	void shrink_to_fit();

	size_type fls() const noexcept;
	size_type ffs() const noexcept;
	size_type clrsb() const noexcept;
	bool test_bit(const size_type i) const noexcept;
	void set_bit(const size_type i, const bool value) noexcept;

	bool is_any_set_below(const size_type i) const noexcept;
	bool is_any_set_at_or_above(const size_type i) const noexcept;
	bool are_all_set_below(const size_type i) const noexcept;
	bool are_all_set_at_or_above(const size_type i) const noexcept;

	static limbs from_size_type(size_type value);

	template<typename T>
	bool fits_into_type() const noexcept;

	template<typename T>
	T to_type() const;

	limbs align(const mpa::limbs::size_type log2_align) const;
	limbs align_down(const mpa::limbs::size_type log2_align) const;

	limbs reverse(const limbs::size_type n_rev_units,
		      const unsigned int rev_unit_width_log2) const;

	static constexpr size_type
	width_to_size(const size_type width) noexcept
	{
	  return (width + limb::width - 1) / limb::width;
	}

	static limbs from_string(const std::string::const_iterator &begin,
				 const std::string::const_iterator &end,
				 const limb &base);

	std::string to_string(const limb &base) const;

      private:
	limbs(_limbs_type &&ls) noexcept;

	_limbs_type _limbs;
      };

      template<typename T>
      bool limbs::fits_into_type() const noexcept
      {
	const size_type width = std::numeric_limits<T>::digits;
	return !is_any_set_at_or_above(width);
      }

      template<typename T>
      T limbs::to_type() const
      {
	if (!fits_into_type<T>()) {
	  throw std::overflow_error("type overflow");
	}

	T r = 0;
	const size_type width = std::numeric_limits<size_type>::digits;
	for (limbs::size_type n = 0, i = 0;
	     n < width && i < _limbs.size();
	     n += limb::width, ++i) {
	  r |= static_cast<size_type>(_limbs[i].value()) << n;
	}

	return r;
      }
    }
  }
}

#endif
