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

#ifndef CONSTEXPR_VALUE_HH
#define CONSTEXPR_VALUE_HH

#include "target_int.hh"
#include "target_float.hh"
#include <memory>
#include <array>

namespace klp
{
  namespace ccp
  {
    class target;

    namespace types
    {
      class returnable_int_type;
      class real_float_type;
    };

    namespace ast
    {
      class init_declarator;
      class function_definition;
      class stmt_labeled;
      class string_literal;
      class expr_compound_literal;

      class constexpr_value
      {
      public:
	enum class constness
	{
	  c_none,
	  c_literal_fp_constant,
	  c_integer_constant_expr,
	  c_arithmetic_constant_expr,
	  c_address_constant,
	  _c_end,
	};

	enum class value_kind
	{
	  vk_int,
	  vk_float,
	  vk_address,
	};

	class address_constant
	{
	public:
	  enum class base_kind
	  {
	    bk_constant,
	    bk_init_declarator,
	    bk_function_definition,
	    bk_stmt_labeled,
	    bk_string_literal,
	    bk_compound_literal,
	  };

	  address_constant();
	  address_constant(const target_int &base);
	  address_constant(const init_declarator &base,
			   const bool dereferenced);
	  address_constant(const function_definition &base,
			   const bool dereferenced);
	  address_constant(const stmt_labeled &base,
			   const bool dereferenced);
	  address_constant(const string_literal &base,
			   const bool dereferenced);
	  address_constant(const expr_compound_literal &base,
			   const bool dereferenced);

	  bool is_dereferenced() const noexcept
	  { return _dereferenced; }

	  void undereference() noexcept;

	  void dereference() noexcept;

	  void add_to_offset(mpa::limbs &&ls);

	  void add_to_offset(bool negate, const target_int &i,
			     const mpa::limbs &j);

	  bool is_null() const noexcept;

	  bool has_same_base_as(const address_constant &ac) const noexcept;

	  const mpa::limbs& get_offset() const noexcept
	  { return _offset; }

	  base_kind get_base_kind() const noexcept
	  { return _bk; }

	private:
	  static mpa::limbs _target_int_to_limbs(const target_int &ti);

	  base_kind _bk;
	  union {
	    const init_declarator * const _base_id;
	    const function_definition * const _base_fd;
	    const stmt_labeled * const _base_stmt_labeled;
	    const string_literal * const _base_string_literal;
	    const expr_compound_literal * const _base_compound_literal;
	  };
	  mpa::limbs _offset;
	  std::size_t _dereferenced;
	};

	struct integer_constant_expr_tag {};
	struct literal_fp_constant_tag {};
	struct arithmetic_constant_expr_tag {};
	struct address_constant_tag {};

	constexpr_value(const integer_constant_expr_tag c_tag,
			const target_int &ti);
	constexpr_value(const arithmetic_constant_expr_tag c_tag,
			const target_int &ti);
	constexpr_value(const target_int &ti);
	constexpr_value(const literal_fp_constant_tag c_tag,
			const target_float &tf);
	constexpr_value(const arithmetic_constant_expr_tag c_tag,
			const target_float &tf);
	constexpr_value(const target_float &tf);
	constexpr_value(const address_constant_tag c_tag,
			const address_constant &ac);
	constexpr_value(const address_constant &ac);
	~constexpr_value() noexcept;

	std::unique_ptr<constexpr_value> clone() const;

	value_kind get_value_kind() const noexcept
	{ return _value_kind; }

	bool has_constness(const constness c) const noexcept;

	void drop_constness() noexcept;
	void drop_constness(const constness c) noexcept;

	const target_int& get_int_value() const noexcept;
	const target_float& get_float_value() const noexcept;
	const address_constant& get_address_value() const noexcept;
	address_constant& get_address_value() noexcept;

	target_int convert_to(const target &tgt,
			      const types::returnable_int_type &it) const;
	target_float convert_to(const target &tgt,
				const types::real_float_type &ft) const;

	bool is_zero() const noexcept;

      private:
	typedef std::array<bool, static_cast<unsigned int>(constness::_c_end)>
	  _constness_type;

	constexpr_value(const _constness_type &c, const target_int &ti);
	constexpr_value(const _constness_type &c, const target_float &tf);
	constexpr_value(const _constness_type &c, const address_constant &ac);

	static _constness_type
	_constness_from_tag() noexcept;
	static _constness_type
	_constness_from_tag(const integer_constant_expr_tag) noexcept;
	static _constness_type
	_constness_from_tag(const literal_fp_constant_tag) noexcept;
	static _constness_type
	_constness_from_tag(const arithmetic_constant_expr_tag) noexcept;
	static _constness_type
	_constness_from_tag(const address_constant_tag) noexcept;

	static _constness_type::size_type
	_constness_to_size_t(const constness c) noexcept;

	value_kind _value_kind;

	union {
	  target_int _ti;
	  target_float _tf;
	  address_constant _ac;
	};

	_constness_type _c;
      };
    }
  }
}

#endif
