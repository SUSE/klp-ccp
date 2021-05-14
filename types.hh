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

#ifndef TYPES_HH
#define TYPES_HH

#include <array>
#include <vector>
#include <memory>
#include <functional>
#include "mp_arithmetic.hh"
#include "target_int.hh"
#include "builtins.hh"

namespace klp
{
  namespace ccp
  {
    class target;

    namespace ast
    {
      class expr;
      class sou_decl_list_node;
      class enum_decl_list_node;
      class enumerator;
    }

    namespace types
    {
      class type;
      class addressable_type;
      class void_type;
      class arithmetic_type;
      class int_type;
      class plain_char_type;
      class bool_type;
      class std_int_type;
      class ext_int_type;
      class enum_type;
      class real_float_type;
      class std_float_type;
      class ext_float_type;
      class complex_float_type;
      class struct_or_union_type;
      class array_type;
      class prototyped_function_type;
      class unprototyped_function_type;
      class pointer_type;
      class bitfield_type;
      class object_type;
      class returnable_type;
      class builtin_func_type;

      typedef std::vector<std::shared_ptr<const addressable_type> >
      parameter_type_list;

      enum class qualifier
      {
	q_const,
	q_volatile,
	q_restrict,
	_q_end,
      };

      class qualifiers
      {
      public:
	qualifiers() noexcept;

	qualifiers(const std::initializer_list<qualifier> &il) noexcept;

	bool operator==(const qualifiers &qs) const noexcept;

	bool operator!=(const qualifiers &qs) const noexcept
	{ return !(*this == qs); }

	void add(const qualifier q);

	void add(const qualifiers qs);

	bool any() const noexcept;

	bool is_subset_of(const qualifiers &qs) const noexcept;

      private:
	std::array<bool, static_cast<unsigned int>(qualifier::_q_end)> _qs;
      };

      class alignment
      {
      public:
	alignment() noexcept;

	explicit alignment(const mpa::limbs::size_type log2_value)
	  noexcept;

	alignment& operator=(const mpa::limbs::size_type log2_value)
	  noexcept;

	bool operator==(const alignment &rhs) const noexcept;

	bool operator!=(const alignment &rhs) const noexcept
	{ return !(*this == rhs); }

	bool is_set() const noexcept
	{ return !!_align_ffs; }

	mpa::limbs::size_type get_log2_value() const noexcept;

	static const alignment& max(const alignment &a1, const alignment &a2)
	  noexcept;

      private:
	mpa::limbs::size_type _align_ffs;
      };

      class type : public std::enable_shared_from_this<type>
      {
      public:
	enum class type_id
	{
	  tid_prototyped_function,
	  tid_unprototyped_function,
	  tid_array,
	  tid_void,
	  tid_pointer,
	  tid_struct_or_union,
	  tid_std_int,
	  tid_ext_int,
	  tid_plain_char,
	  tid_bool,
	  tid_enum,
	  tid_bitfield,
	  tid_std_float,
	  tid_ext_float,
	  tid_complex_float,
	  tid_builtin_func,
	};

	type(type&&) = delete;

	virtual ~type() noexcept = 0;

	virtual type_id get_type_id() const noexcept = 0;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers) const = 0;
	virtual bool is_compatible_with(const target &tgt,
					const void_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const plain_char_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const bool_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const std_int_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const ext_int_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const enum_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const std_float_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const ext_float_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const complex_float_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const struct_or_union_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const array_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const prototyped_function_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const unprototyped_function_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const pointer_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const bitfield_type &t,
					const bool ignore_qualifiers) const;
	virtual bool is_compatible_with(const target &tgt,
					const builtin_func_type &t,
					const bool ignore_qualifiers) const;

	const qualifiers& get_qualifiers() const noexcept
	{ return _qs; }

	qualifiers& get_qualifiers() noexcept
	{ return _qs; }

	std::shared_ptr<const type> amend_qualifiers(const qualifiers &qs) const;

	std::shared_ptr<const type> strip_qualifiers() const;

      protected:
	type();

	type(const qualifiers &qs);

	type(const type&);

	template<typename self_type>
	std::shared_ptr<const self_type> _self_ptr() const noexcept
	{
	  return std::dynamic_pointer_cast<const self_type>(shared_from_this());
	}

	virtual void _amend_qualifiers(const qualifiers &qs);

      private:
	virtual type* _clone() const = 0;

	qualifiers _qs;
      };

      class addressable_type : public virtual type
      {
      public:
	virtual ~addressable_type() noexcept = 0;

	std::shared_ptr<const addressable_type>
	amend_qualifiers(const qualifiers &qs) const;

	std::shared_ptr<const addressable_type>
	strip_qualifiers() const;

	std::shared_ptr<const pointer_type>
	derive_pointer(const qualifiers &qs = qualifiers{},
		       const alignment &user_align = alignment{}) const;

	virtual bool is_complete() const noexcept = 0;

	virtual bool is_size_constant() const noexcept = 0;

	virtual mpa::limbs get_size(const target &tgt) const = 0;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept = 0;

	alignment&
	get_user_alignment() noexcept
	{ return _user_align; }

	const alignment&
	get_user_alignment() const noexcept
	{ return _user_align; }

	std::shared_ptr<const addressable_type>
	set_user_alignment(const alignment &user_align) const;

	mpa::limbs::size_type get_effective_alignment(const target &tgt)
	  const noexcept;

	virtual std::shared_ptr<const addressable_type>
	construct_composite(const target &tgt,
			    const addressable_type& prev_type) const;

      protected:
	addressable_type();

	addressable_type(const alignment &user_align);

	addressable_type(const addressable_type&);

      private:
	friend class prototyped_function_type;
	friend class unprototyped_function_type;
	friend class array_type;
	friend class pointer_type;

	virtual addressable_type* _clone() const override = 0;

	virtual std::shared_ptr<const array_type>
	_construct_composite(const target&,
			     const array_type&) const;

	virtual std::shared_ptr<const prototyped_function_type>
	_construct_composite(const target&,
			     const unprototyped_function_type&) const;

	virtual std::shared_ptr<const prototyped_function_type>
	_construct_composite(const target&,
			     const prototyped_function_type&) const;

	virtual std::shared_ptr<const pointer_type>
	_construct_composite(const target&,
			     const pointer_type&) const;

	alignment _user_align;
      };

      class function_type : public addressable_type
      {
      public:
	virtual ~function_type() noexcept = 0;

	virtual bool is_complete() const noexcept override;

	virtual bool is_size_constant() const noexcept override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	const std::shared_ptr<const returnable_type>&
	get_return_type() const noexcept
	{ return _return_type; }

      protected:
	function_type(std::shared_ptr<const returnable_type> &&rt);

	function_type(const function_type&);

      private:
	virtual function_type* _clone() const override = 0;

	virtual void _amend_qualifiers(const qualifiers &qs) override;

	std::shared_ptr<const returnable_type> _return_type;
      };

      class object_type : public virtual addressable_type
      {
      public:
	virtual ~object_type() noexcept = 0;

	std::shared_ptr<const object_type> amend_qualifiers(const qualifiers &qs)
	  const;

	std::shared_ptr<const array_type>
	derive_array(const ast::expr * const length_expr) const;

	std::shared_ptr<const array_type>
	derive_array(mpa::limbs &&initializer_length) const;

	std::shared_ptr<const array_type>
	derive_array(const bool unspec_vla) const;

      protected:
	object_type();
	object_type(const object_type&);

      private:
	friend class array_type;

	virtual object_type* _clone() const override = 0;

	virtual std::shared_ptr<const object_type>
	_construct_composite(const target &tgt,
			     const object_type& prev_type) const;
      };

      class returnable_type : public virtual addressable_type
      {
      public:
	virtual ~returnable_type() noexcept = 0;

	std::shared_ptr<const prototyped_function_type>
	derive_function(parameter_type_list &&ptl,
			const bool variadic) const;

	std::shared_ptr<const unprototyped_function_type>
	derive_function(const bool from_definition) const;

	std::shared_ptr<const unprototyped_function_type>
	derive_function(const std::size_t n_args) const;

      protected:
	returnable_type();

	returnable_type(const returnable_type&);

      private:
	friend class prototyped_function_type;

	virtual returnable_type* _clone() const override = 0;

	virtual std::shared_ptr<const returnable_type>
	_construct_composite(const target &tgt,
			     const returnable_type& prev_type) const;
      };

      class returnable_object_type : public object_type, public returnable_type
      {
      public:
	virtual ~returnable_object_type() noexcept = 0;

	virtual bool is_complete() const noexcept override;

	virtual bool is_size_constant() const noexcept override;

      protected:
	returnable_object_type();

	returnable_object_type(const returnable_object_type&);

      private:
	virtual returnable_object_type* _clone() const override = 0;
      };

      class prototyped_function_type final : public function_type
      {
      public:
	virtual ~prototyped_function_type() noexcept;

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual bool is_compatible_with(const target &tgt,
					const prototyped_function_type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const unprototyped_function_type &t,
					const bool ignore_qualifiers)
	  const override;

	const parameter_type_list& get_parameter_type_list() const noexcept
	{ return _ptl; }

	bool is_variadic() const noexcept
	{ return _variadic; }

      private:
	friend class returnable_type;
	friend class unprototyped_function_type;

	prototyped_function_type(std::shared_ptr<const returnable_type> &&rt,
				 parameter_type_list &&ptl,
				 const bool variadic);

	prototyped_function_type(const prototyped_function_type&);

	virtual prototyped_function_type* _clone() const override;

	virtual std::shared_ptr<const addressable_type>
	construct_composite(const target &tgt,
			    const addressable_type& prev_type) const override;

	virtual std::shared_ptr<const prototyped_function_type>
	_construct_composite(const target &tgt,
			     const unprototyped_function_type& next_type)
	  const override;

	virtual std::shared_ptr<const prototyped_function_type>
	_construct_composite(const target &tgt,
			     const prototyped_function_type& next_type)
	  const override;

	const parameter_type_list _ptl;
	const bool _variadic;
      };

      class unprototyped_function_type final : public function_type
      {
      public:
	virtual ~unprototyped_function_type() noexcept;

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const unprototyped_function_type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const prototyped_function_type &t,
					const bool ignore_qualifiers)
	  const override;

	bool is_from_definition() const noexcept
	{  return _from_definition; }

	std::size_t get_n_args() const noexcept;

      private:
	friend class returnable_type;

	unprototyped_function_type(std::shared_ptr<const returnable_type> &&rt,
				   const bool from_definition);

	unprototyped_function_type(std::shared_ptr<const returnable_type> &&rt,
				   const std::size_t n_args);

	unprototyped_function_type(const unprototyped_function_type&);

	virtual unprototyped_function_type* _clone() const override;

	virtual std::shared_ptr<const addressable_type>
	construct_composite(const target &tgt,
			    const addressable_type& prev_type) const override;

	virtual std::shared_ptr<const prototyped_function_type>
	_construct_composite(const target &tgt,
			     const prototyped_function_type& next_type)
	  const override;

	const std::size_t _n_args;
	const bool _from_definition;
      };

      class array_type final : public object_type
      {
      public:
	virtual ~array_type() noexcept;

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const array_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual bool is_complete() const noexcept override;

	virtual bool is_size_constant() const noexcept override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	std::shared_ptr<const array_type>
	set_user_alignment(const alignment &user_align) const;

	const std::shared_ptr<const object_type>&
	get_element_type() const noexcept
	{ return _element_type; }

	bool is_length_constant() const noexcept;

	const mpa::limbs& get_length() const noexcept;

	std::shared_ptr<const array_type>
	set_length_from_initializer(mpa::limbs &&l) const;

      private:
	friend class object_type;

	array_type(std::shared_ptr<const object_type> &&element_type,
		   const ast::expr * const length_expr,
		   const alignment &user_align = alignment{});
	array_type(std::shared_ptr<const object_type> &&element_type,
		   mpa::limbs &&initializer_length,
		   const alignment &user_align = alignment{});
	array_type(std::shared_ptr<const object_type> &&element_type,
		   const bool unspec_vla,
		   const alignment &user_align = alignment{});

	array_type(const array_type&);

	virtual array_type* _clone() const override;

	virtual void _amend_qualifiers(const qualifiers &qs) override;

	virtual std::shared_ptr<const addressable_type>
	construct_composite(const target &tgt,
			    const addressable_type& prev_type) const override;

	virtual std::shared_ptr<const object_type>
	_construct_composite(const target &tgt,
			     const object_type& prev_type) const override;

	virtual std::shared_ptr<const array_type>
	_construct_composite(const target &tgt,
			     const array_type& next_type) const override;


	std::shared_ptr<const object_type> _element_type;
	const ast::expr * const _length_expr;
	const mpa::limbs _initializer_length;
	const bool _unspec_vla;
      };

      class void_type final : public returnable_type
      {
      public:
	virtual ~void_type() noexcept;

	static std::shared_ptr<const void_type>
	create(const qualifiers &qs = {});

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&, const void_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual bool is_complete() const noexcept override;

	virtual bool is_size_constant() const noexcept override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

      private:
	void_type(const qualifiers &qs);

	void_type(const void_type&);

	virtual void_type* _clone() const override;
      };

      class pointer_type final : public returnable_object_type
      {
      public:
	virtual ~pointer_type() noexcept;

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt,
					const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const pointer_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	std::shared_ptr<const pointer_type>
	set_user_alignment(const alignment &user_align) const;

	const std::shared_ptr<const addressable_type>&
	get_pointed_to_type() const noexcept
	{ return _pointed_to_type; }

      private:
	friend class addressable_type;

	pointer_type(std::shared_ptr<const addressable_type> &&pointed_to_type,
		     const qualifiers &qs, const alignment &user_align);

	pointer_type(const pointer_type&);

	virtual pointer_type* _clone() const override;

	virtual std::shared_ptr<const addressable_type>
	construct_composite(const target &tgt,
			    const addressable_type& prev_type) const override;

	virtual std::shared_ptr<const object_type>
	_construct_composite(const target &tgt,
			     const object_type& prev_type) const override;

	virtual std::shared_ptr<const returnable_type>
	_construct_composite(const target &tgt,
			     const returnable_type& prev_type) const override;

	virtual std::shared_ptr<const pointer_type>
	_construct_composite(const target &tgt,
			     const pointer_type& next_type) const override;

	const std::shared_ptr<const addressable_type> _pointed_to_type;
      };

      enum class struct_or_union_kind
      {
	souk_struct,
	souk_union,
      };

      class struct_or_union_content
      {
      public:
	class member
	{
	public:
	  member(const std::string &name, std::shared_ptr<const type> &&type);

	  member(std::shared_ptr<const struct_or_union_type> &&sou_type);

	  member(member &&m);

	  ~member() noexcept;

	  const std::string&
	  get_name() const noexcept
	  { return _name; }

	  bool is_unnamed_sou() const noexcept
	  { return _is_unnamed_sou; }

	  std::shared_ptr<const type>
	  get_type() const noexcept;

	  const std::shared_ptr<const struct_or_union_type>&
	  get_sou_type() const noexcept;

	  void set_offset(const mpa::limbs &offset);
	  const mpa::limbs& get_offset() const noexcept;

	  void set_bitpos(const mpa::limbs &bitpos);
	  const mpa::limbs& get_bitpos() const noexcept;

	  void set_has_constant_offset(const bool val) noexcept;
	  bool has_constant_offset() const noexcept;

	private:
	  const std::string _name;
	  union
	  {
	    std::shared_ptr<const type> _type;
	    std::shared_ptr<const struct_or_union_type> _sou_type;
	  };
	  mpa::limbs _offset;
	  mpa::limbs _bitpos;
	  bool _has_constant_offset;
	  bool _is_unnamed_sou;
	};

	typedef std::vector<member>::iterator member_iterator;
	typedef std::vector<member>::const_iterator const_member_iterator;

	typedef std::vector<const_member_iterator> lookup_result;

	struct_or_union_content();

	void add_member(member &&m);

	lookup_result lookup(const std::string &name) const;

	template <typename callable_type>
	void for_each_member_flat(callable_type &&c) const;

	member_iterator members_begin()
	{ return _members.begin(); }

	member_iterator members_end()
	{ return _members.end(); }

	const_member_iterator members_begin() const
	{ return _members.cbegin(); }

	const_member_iterator members_end() const
	{ return _members.cend(); }

	void set_alignment(const mpa::limbs::size_type log2_align) noexcept;
	void set_size(mpa::limbs &&size);
	void set_size_constant(const bool is_size_constant) noexcept;

	bool is_size_constant() const noexcept;

	const mpa::limbs& get_size() const noexcept;

	mpa::limbs::size_type get_alignment() const noexcept;

      private:
	bool _has_been_layout() const noexcept;

	std::vector<member> _members;
	mpa::limbs::size_type _align_ffs;
	mpa::limbs _size;
	bool _is_size_constant;
      };

      class struct_or_union_type final : public returnable_object_type
      {
      public:
	virtual ~struct_or_union_type() noexcept override;

	static std::shared_ptr<const struct_or_union_type>
	create(const struct_or_union_kind kind,
	       const ast::sou_decl_list_node &decl_node,
	       const qualifiers &qs = qualifiers{});

	static std::shared_ptr<const struct_or_union_type>
	create(const struct_or_union_kind kind,
	       const struct_or_union_content &content);

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&,
					const struct_or_union_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual bool is_complete() const noexcept override;

	virtual bool is_size_constant() const noexcept override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	const struct_or_union_content* get_content() const noexcept;

	const struct_or_union_kind get_kind() const noexcept
	{ return _kind; }

	const ast::sou_decl_list_node* get_decl_list_node() const noexcept
	{ return _decl_node; }

      private:
	struct_or_union_type(const struct_or_union_kind kind,
			     const ast::sou_decl_list_node &decl_node,
			     const qualifiers &qs);

	struct_or_union_type(const struct_or_union_kind kind,
			     const struct_or_union_content &content);

	struct_or_union_type(const struct_or_union_type&);

	virtual struct_or_union_type* _clone() const override;

	const struct_or_union_kind _kind;
	const ast::sou_decl_list_node *_decl_node;

	mutable const struct_or_union_content *_content;
      };

      class arithmetic_type : public returnable_object_type
      {
      public:
	virtual ~arithmetic_type() noexcept = 0;

	std::shared_ptr<const arithmetic_type> strip_qualifiers() const;

	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const arithmetic_type &at) const = 0;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const int_type &it) const = 0;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const real_float_type &ft) const = 0;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const complex_float_type &ct) const = 0;

      protected:
	arithmetic_type();

	arithmetic_type(const arithmetic_type&);

      private:
	virtual arithmetic_type* _clone() const override = 0;
      };


      class int_type : public arithmetic_type
      {
      public:
	virtual ~int_type() noexcept = 0;

	std::shared_ptr<const int_type> strip_qualifiers() const;

	std::shared_ptr<const int_type>
	set_user_alignment(const alignment &user_align) const;

	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const arithmetic_type &at) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const int_type &it) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const real_float_type &ft) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const complex_float_type &ct) const override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const int_type &it) const = 0;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const std_int_type &it)
	  const;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const ext_int_type &it)
	  const;

	virtual bool is_signed(const target &tgt) const noexcept = 0;

	virtual mpa::limbs::size_type get_width(const target &tgt)
	  const noexcept = 0;

	virtual std::shared_ptr<const int_type>
	promote(const target &tgt) const = 0;

	virtual std::shared_ptr<const int_type> to_unsigned()
	  const = 0;

      protected:
	int_type();

	int_type(const int_type&);

      private:
	virtual int_type* _clone() const override = 0;
      };

      class std_int_type final : public int_type
      {
      public:
	enum class kind
	{
	  k_char,
	  k_short,
	  k_int,
	  k_long,
	  k_long_long,
	};

	virtual ~std_int_type() noexcept;

	static std::shared_ptr<const std_int_type>
	create(const kind k, const bool is_signed,
	       const qualifiers &qs = qualifiers{});

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&,
					const std_int_type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const enum_type &t,
					const bool ignore_qualifiers)
	  const override;

	std::shared_ptr<const std_int_type> strip_qualifiers() const;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const int_type &it)
	  const override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const std_int_type &it)
	  const override;

	virtual bool is_signed(const target &tgt)
	  const noexcept override;

	virtual mpa::limbs::size_type get_width(const target &tgt)
	  const noexcept override;

	virtual std::shared_ptr<const int_type>
	promote(const target &tgt) const override;

	virtual std::shared_ptr<const int_type> to_unsigned()
	  const override;

	kind get_kind() const noexcept
	{ return _k; }


      private:
	std_int_type(const kind k, const bool is_signed, const qualifiers &qs);

	std_int_type(const std_int_type&);

	virtual std_int_type* _clone() const override;

	std::shared_ptr<const std_int_type> _promote() const;

	const kind _k;
	const bool _signed;
      };

      class ext_int_type final : public int_type
      {
      public:
	class kind
	{
	public:
	  kind() noexcept = default;

	  constexpr explicit kind(const int tgt_id) noexcept
	    : _tgt_id(tgt_id)
	  {}

	  constexpr kind(const kind &k) noexcept = default;

	  constexpr explicit operator int() const
	  { return _tgt_id; }

	  kind& operator=(const kind &rhs) noexcept = default;

	  constexpr bool operator==(const kind &rhs) const noexcept
	  {
	    return this->_tgt_id == rhs._tgt_id;
	  }

	  constexpr bool operator!=(const kind &rhs) const noexcept
	  {
	    return !(*this == rhs);
	  }

	private:
	  int _tgt_id;
	};

	virtual ~ext_int_type() noexcept;

	static std::shared_ptr<const ext_int_type>
	create(const kind k, const bool is_signed,
	       const qualifiers &qs = qualifiers{});

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&,
					const ext_int_type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const enum_type &t,
					const bool ignore_qualifiers)
	  const override;

	std::shared_ptr<const ext_int_type> strip_qualifiers() const;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const int_type &it)
	  const override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const std_int_type &it)
	  const override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const ext_int_type &it)
	  const override;

	virtual bool is_signed(const target &tgt)
	  const noexcept override;

	virtual mpa::limbs::size_type get_width(const target &tgt)
	  const noexcept override;

	virtual std::shared_ptr<const int_type>
	promote(const target &tgt) const override;

	virtual std::shared_ptr<const int_type> to_unsigned()
	  const override;

	const kind get_kind() const noexcept
	{ return _k; }

      private:
	ext_int_type(const kind k, const bool is_signed, const qualifiers &qs);

	ext_int_type(const ext_int_type&);

	virtual ext_int_type* _clone() const override;

	const kind _k;
	const bool _signed;
      };

      class plain_char_type final : public int_type
      {
      public:
	virtual ~plain_char_type() noexcept;

	static std::shared_ptr<const plain_char_type>
	create(const qualifiers &qs = qualifiers{});

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&,
					const plain_char_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const int_type &it)
	  const override;

	virtual bool is_signed(const target &tgt)
	  const noexcept override;

	virtual mpa::limbs::size_type get_width(const target &tgt)
	  const noexcept override;

	virtual std::shared_ptr<const int_type>
	promote(const target &tgt) const override;

	virtual std::shared_ptr<const int_type> to_unsigned()
	  const override;

      private:
	plain_char_type(const qualifiers &qs);

	plain_char_type(const plain_char_type&);

	virtual plain_char_type* _clone() const override;

	std::shared_ptr<const std_int_type> _promote() const;
      };

      class bool_type final : public int_type
      {
      public:
	virtual ~bool_type() noexcept;

	static std::shared_ptr<const bool_type>
	create(const qualifiers &qs = qualifiers{});

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&,
					const bool_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const int_type &it)
	  const override;

	virtual bool is_signed(const target &tgt)
	  const noexcept override;

	virtual mpa::limbs::size_type get_width(const target &tgt)
	  const noexcept override;

	virtual std::shared_ptr<const int_type>
	promote(const target &tgt) const override;

	virtual std::shared_ptr<const int_type> to_unsigned()
	  const override;

      private:
	bool_type(const qualifiers &qs);

	bool_type(const bool_type&);

	virtual bool_type* _clone() const override;

	std::shared_ptr<const std_int_type> _promote() const;
      };

      class enum_content
      {
      public:
	class member
	{
	public:
	  member(const ast::enumerator &e, const std::string &name,
		 const std::shared_ptr<const int_type> &initial_type,
		 const target_int &value);

	  const ast::enumerator &
	  get_enumerator() const noexcept
	  { return _e; }

	  const std::string&
	  get_name() const noexcept
	  { return _name; }

	  const target_int&
	  get_value() const noexcept
	  { return _value; }

	  void convert_value(const mpa::limbs::size_type prec,
			     const bool is_signed);

	  void set_type(const std::shared_ptr<const int_type> &t) noexcept;
	  void set_type(std::shared_ptr<const int_type> &&t) noexcept;

	  const std::shared_ptr<const int_type>&
	  get_type() const noexcept;

	private:
	  const ast::enumerator &_e;
	  const std::string _name;
	  target_int _value;
	  std::shared_ptr<const int_type> _type;
	};

	void add_member(const ast::enumerator &e, const std::string &name,
			const std::shared_ptr<const int_type> &initial_type,
			const target_int &value);

	void add_member(const ast::enumerator &e, const std::string &name,
			const target &tgt);

	const member* lookup(const std::string &name) const noexcept;

	bool empty() const noexcept
	{ return _members.empty(); }

	void set_underlying_type(std::shared_ptr<const int_type> &&ut)
	  noexcept;

	const std::shared_ptr<const int_type>&
	get_underlying_type() const noexcept;

	template <typename callable_type>
	void for_each_member(callable_type &&c);

      private:
	std::vector<member> _members;
	std::shared_ptr<const int_type> _underlying_type;
      };

      class enum_type final : public int_type
      {
      public:
	virtual ~enum_type() noexcept;

	static std::shared_ptr<const enum_type>
	create(const ast::enum_decl_list_node &decl_node, const qualifiers &qs);

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&, const enum_type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const std_int_type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const ext_int_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual bool is_complete() const noexcept override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	virtual std::shared_ptr<const int_type>
	integer_conversion(const target &tgt, const int_type &it)
	  const override;

	virtual bool is_signed(const target &tgt)
	  const noexcept override;

	virtual mpa::limbs::size_type get_width(const target &tgt)
	  const noexcept override;

	virtual std::shared_ptr<const int_type>
	promote(const target &tgt) const override;

	virtual std::shared_ptr<const int_type> to_unsigned()
	  const override;

	const std::shared_ptr<const int_type>&
	get_underlying_type() const noexcept;

	const ast::enum_decl_list_node& get_decl_list_node() const noexcept
	{ return _decl_node; }

      private:
	enum_type(const ast::enum_decl_list_node &decl_node,
		  const qualifiers qs);

	enum_type(const enum_type&);

	virtual enum_type* _clone() const override;

	const enum_content* _get_content() const noexcept;

	const ast::enum_decl_list_node &_decl_node;

	mutable const enum_content *_content;
      };

      class bitfield_type final : public type
      {
      public:
	virtual ~bitfield_type() noexcept;

	static std::shared_ptr<const bitfield_type>
	create(std::shared_ptr<const int_type> &&base_type,
	       const mpa::limbs::size_type width);

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;

	std::shared_ptr<const bitfield_type> strip_qualifiers() const;

	bool is_signed(const target &tgt) const noexcept;

	mpa::limbs::size_type get_width(const target &tgt) const noexcept;

	std::shared_ptr<const int_type> promote(const target &tgt) const;

	const std::shared_ptr<const int_type>& get_base_type()
	  const noexcept
	{ return _base_type; }

	const alignment& get_user_alignment() const noexcept
	{ return _user_align; }

	std::shared_ptr<const bitfield_type>
	set_user_alignment(const alignment &user_align) const;

	bool is_packed() const noexcept
	{ return _packed; }

	std::shared_ptr<const bitfield_type> set_packed() const;

      private:
	bitfield_type(std::shared_ptr<const int_type> &&base_type,
		      const mpa::limbs::size_type width);

	bitfield_type(const bitfield_type&);

	virtual bitfield_type* _clone() const override;

	std::shared_ptr<const int_type> _base_type;
	const mpa::limbs::size_type _width;

	alignment _user_align;
	bool _packed;
      };

      class real_float_type : public arithmetic_type
      {
      public:
	struct format
	{
	  format(const mpa::limbs::size_type _significand_width,
		 const mpa::limbs::size_type _exponent_width) noexcept;

	  bool operator==(const format &rhs) const noexcept;
	  bool operator<(const format &rhs) const noexcept;
	  bool operator<=(const format &rhs) const noexcept;

	  const mpa::limbs::size_type significand_width;
	  const mpa::limbs::size_type exponent_width;
	};


	~real_float_type() noexcept = 0;

	std::shared_ptr<const real_float_type> strip_qualifiers() const;

	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const arithmetic_type &at) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target&,
			      const int_type&) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const real_float_type &ft) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const complex_float_type &ct) const override;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const real_float_type &ft) const = 0;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const std_float_type &ft) const = 0;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const ext_float_type &ft) const = 0;

	virtual std::shared_ptr<const real_float_type>
	promote(const target &tgt) const = 0;

	virtual format get_format(const target &tgt) const noexcept = 0;

      protected:
	real_float_type();

	real_float_type(const real_float_type&);

      private:
	virtual real_float_type* _clone() const override = 0;
      };

      class std_float_type final : public real_float_type
      {
      public:
	enum class kind
	{
	 k_float,
	 k_double,
	 k_long_double,
	};

	virtual ~std_float_type() noexcept;

	static std::shared_ptr<const std_float_type>
	create(const kind k, const qualifiers &qs = qualifiers{});

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&,
					const std_float_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const real_float_type &ft) const override;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const std_float_type &ft) const override;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const ext_float_type &ft) const override;

	virtual std::shared_ptr<const real_float_type> promote(const target&)
	  const override;

	virtual format get_format(const target &tgt) const noexcept override;

	kind get_kind() const noexcept
	{ return _k; }

      private:
	std_float_type(const kind k, const qualifiers &qs);

	std_float_type(const std_float_type&);

	virtual std_float_type* _clone() const override;

	kind _k;
      };

      class ext_float_type : public real_float_type
      {
      public:
	class kind
	{
	public:
	  kind() noexcept = default;

	  constexpr explicit kind(const int tgt_id) noexcept
	    : _tgt_id(tgt_id)
	  {}

	  constexpr kind(const kind &k) noexcept = default;

	  constexpr explicit operator int() const
	  { return _tgt_id; }

	  kind& operator=(const kind &rhs) noexcept = default;

	  constexpr bool operator==(const kind &rhs) const noexcept
	  {
	    return this->_tgt_id == rhs._tgt_id;
	  }

	  constexpr bool operator!=(const kind &rhs) const noexcept
	  {
	    return !(*this == rhs);
	  }

	private:
	  int _tgt_id;
	};

	virtual ~ext_float_type() noexcept;

	static std::shared_ptr<const ext_float_type>
	create(const kind k, const qualifiers &qs = qualifiers{});

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target&,
					const ext_float_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const real_float_type &ft) const override;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const std_float_type &ft) const override;

	virtual std::shared_ptr<const real_float_type>
	real_float_conversion(const target &tgt,
			      const ext_float_type &ft) const override;

	virtual std::shared_ptr<const real_float_type>
	promote(const target &tgt) const override;

	virtual format get_format(const target &tgt) const noexcept override;

	kind get_kind() const noexcept
	{ return _k; }

      private:
	ext_float_type(const kind k, const qualifiers &qs);

	ext_float_type(const ext_float_type&);

	virtual ext_float_type* _clone() const override;

	kind _k;
      };

      class complex_float_type final : public arithmetic_type
      {
      public:
	virtual ~complex_float_type() noexcept;

	static std::shared_ptr<const complex_float_type>
	create(std::shared_ptr<const real_float_type> &&base_type,
	       const qualifiers &qs = qualifiers{});

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;
	virtual bool is_compatible_with(const target &tgt,
					const complex_float_type &t,
					const bool ignore_qualifiers)
	  const override;

	virtual mpa::limbs get_size(const target &tgt) const override;

	virtual mpa::limbs::size_type
	get_type_alignment(const target &tgt) const noexcept override;

	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const arithmetic_type &at) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target&,
			      const int_type&) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const real_float_type &ft) const override;
	virtual std::shared_ptr<const arithmetic_type>
	arithmetic_conversion(const target &tgt,
			      const complex_float_type &ct) const override;

	std::shared_ptr<const complex_float_type> promote(const target &tgt)
	  const;

      private:
	complex_float_type(std::shared_ptr<const real_float_type> &&base_type,
			   const qualifiers &qs);

	complex_float_type(const complex_float_type&);

	virtual complex_float_type* _clone() const override;

	std::shared_ptr<const real_float_type> _base_type;
      };

      class builtin_func_type final : public type
      {
      public:
	virtual ~builtin_func_type() noexcept override;

	static std::shared_ptr<const builtin_func_type>
	create(const builtin_func::factory &builtin_func_fac);

	virtual type_id get_type_id() const noexcept override;

	virtual bool is_compatible_with(const target &tgt, const type &t,
					const bool ignore_qualifiers)
	  const override;

	const builtin_func::factory& get_builtin_func_factory() const noexcept
	{ return *_builtin_func_fac; }

      private:
	builtin_func_type(const builtin_func::factory &builtin_func_fac);

	builtin_func_type(const builtin_func_type&);

	virtual builtin_func_type* _clone() const override;

	const builtin_func::factory * const _builtin_func_fac;
      };
    }
  }
}

#include "types_impl.hh"

#endif
