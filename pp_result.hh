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

#ifndef PP_RESULT_HH
#define PP_RESULT_HH

#include <vector>
#include <memory>
#include <utility>
#include <set>
#include <stack>
#include "raw_pp_tokens.hh"
#include "raw_pp_tokens_range.hh"
#include "pp_token.hh"
#include "pp_tokens.hh"
#include "pp_tokens_range.hh"
#include "offset_to_line_col_map.hh"

namespace klp
{
  namespace ccp
  {
    class source_reader;

    class pp_result
    {
    public:
      class directive
      {
      public:
	directive(const raw_pp_tokens_range &directive_range) noexcept;

	bool operator<(const raw_pp_tokens_range &r) const noexcept
	{ return _directive_range < r; }

	const raw_pp_tokens_range& get_source_range() const noexcept
	{ return _directive_range; }

      private:
	raw_pp_tokens_range _directive_range;
      };

    private:
      typedef std::vector<directive> _directives_container_type;

    public:
      typedef _directives_container_type::const_iterator
	const_directive_iterator;

      const_directive_iterator directives_begin() const noexcept
      { return _directives.begin(); }

      const_directive_iterator directives_end() const noexcept
      { return _directives.end(); }

      std::pair<const_directive_iterator, const_directive_iterator>
      find_overlapping_directives(const raw_pp_tokens_range &r) const noexcept;


      class macro
      {
      private:
	enum class _origin
	{
	  o_builtin,
	  o_builtin_special,
	  o_predefined_user,
	  o_source,
	};

      public:
	struct builtin_special_tag {};
	struct builtin_tag {};
	struct predefined_user_tag {};

	bool operator==(const macro &rhs) const noexcept;

	bool operator!=(const macro &rhs) const noexcept
	{ return !(*this == rhs); }

	bool operator<(const raw_pp_tokens_range &r) const noexcept
	{
	  if (!is_predefined())
	    return _directive_range < r;
	  else
	    return true;
	}

	bool is_func_like() const noexcept
	{ return _func_like; }

	bool is_variadic() const noexcept
	{ return _variadic; }

	const std::vector<std::string>& get_arg_names() const noexcept;
	size_t non_va_arg_count() const noexcept;
	bool shall_expand_arg(const size_t pos) const noexcept;

	const std::string& get_name() const noexcept
	{ return _name; }


	bool is_predefined() const noexcept
	{
	  return (_o == _origin::o_builtin ||
		  _o == _origin::o_builtin_special ||
		  _o == _origin::o_predefined_user);
	}

	bool is_builtin() const noexcept
	{
	  return (_o == _origin::o_builtin ||
		  _o == _origin::o_builtin_special);
	}

	bool is_builtin_special() const noexcept
	{
	  return _o == _origin::o_builtin_special;
	}

	const raw_pp_tokens_range& get_directive_range() const noexcept
	{
	  assert(!is_predefined());
	  return _directive_range;
	}

	unsigned long get_predefinition_pos() const noexcept
	{
	  assert(is_predefined());
	  return _predefinition_pos;
	}

	const raw_pp_tokens& get_repl() const noexcept
	{ return _repl; }

	const raw_pp_tokens_range& get_repl_range() const noexcept
	{ return _repl_range; }

      private:
	friend class pp_result;

	macro(const std::string &name,
	      const bool func_like,
	      const bool variadic,
	      std::vector<std::string> &&arg_names,
	      raw_pp_tokens &&repl,
	      const raw_pp_tokens_range &repl_range,
	      const raw_pp_tokens_range &directive_range);

	macro(const std::string &name,
	      const unsigned long predefinition_pos,
	      const builtin_special_tag&);

	macro(const std::string &name,
	      std::vector<std::string> &&arg_names,
	      const unsigned long predefinition_pos,
	      const builtin_special_tag&);

	macro(const std::string &name,
	      const bool func_like,
	      const bool variadic,
	      std::vector<std::string> &&arg_names,
	      raw_pp_tokens &&repl,
	      const unsigned long predefinition_pos,
	      const builtin_tag&);

	macro(const std::string &name,
	      const bool func_like,
	      const bool variadic,
	      std::vector<std::string> &&arg_names,
	      raw_pp_tokens &&repl,
	      const unsigned long predefinition_pos,
	      const predefined_user_tag&);

	macro(const std::string &name,
	      const bool func_like,
	      const bool variadic,
	      std::vector<std::string> &&arg_names,
	      raw_pp_tokens &&repl,
	      const unsigned long predefinition_pos,
	      const _origin o);

	void _init_do_expand_args();

	std::string _name;
	std::vector<std::string> _arg_names;
	raw_pp_tokens _repl;
	raw_pp_tokens_range _repl_range;
	_origin _o;
	union
	{
	  raw_pp_tokens_range _directive_range;
	  unsigned long _predefinition_pos;
	};
	std::vector<bool> _do_expand_args;
	bool _func_like;
	bool _variadic;
      };

    private:
      typedef std::vector<std::unique_ptr<const macro>>
	_macros_container_type;

    public:
      class const_macro_iterator
	: public std::iterator<std::random_access_iterator_tag,
			       const macro>
      {
      public:
	bool operator==(const const_macro_iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const const_macro_iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept
	{ return **_it; }

	pointer operator->() const noexcept
	{ return (*_it).get(); }

	const_macro_iterator& operator++() noexcept
	{ ++_it; return *this; }

	const const_macro_iterator operator++(int) noexcept
	{ return const_macro_iterator{_it++}; }

	const_macro_iterator& operator--() noexcept
	{ --_it; return *this; }

	const const_macro_iterator operator--(int) noexcept
	{ return const_macro_iterator{_it--}; }

	const_macro_iterator&
	operator+=(const difference_type n) noexcept
	{ _it += n; return *this; }

	const_macro_iterator&
	operator-=(const difference_type n) noexcept
	{ _it -= n; return *this; }

	const const_macro_iterator
	operator+(const difference_type n) const noexcept
	{ return const_macro_iterator(_it + n); }

	const const_macro_iterator
	operator-(const difference_type n) const noexcept
	{ return const_macro_iterator(_it - n); }

	const difference_type
	operator-(const const_macro_iterator &op) const noexcept
	{ return this->_it - op._it; }

	reference operator[](const difference_type n) const noexcept
	{ return *(*this + n); }

	bool operator<(const const_macro_iterator &op) const noexcept
	{ return this->_it < op._it; }

	bool operator>(const const_macro_iterator &op) const noexcept
	{ return this->_it > op._it; }

	bool operator<=(const const_macro_iterator &op) const noexcept
	{ return this->_it <= op._it; }

	bool operator>=(const const_macro_iterator &op) const noexcept
	{ return this->_it >= op._it; }

      private:
	friend class pp_result;

	const_macro_iterator(const _macros_container_type::const_iterator &it)
	  noexcept
	  : _it(it)
	{}

	_macros_container_type::const_iterator _it;
      };

      const_macro_iterator macros_begin() const noexcept
      { return const_macro_iterator{_macros.begin()}; }

      const_macro_iterator macros_end() const noexcept
      { return const_macro_iterator{_macros.end()}; }

      std::pair<const_macro_iterator, const_macro_iterator>
      find_overlapping_macros(const raw_pp_tokens_range &r) const noexcept;


      class macro_undef
      {
      private:
	enum class _origin
	{
	  o_predefined_user,
	  o_source,
	};

      public:
	struct predefined_user_tag{};

	macro_undef(const std::string &name,
		    const raw_pp_tokens_range &directive_range);

	macro_undef(const std::string &name,
		    const unsigned long predefinition_pos,
		    const predefined_user_tag&);

	bool operator<(const raw_pp_tokens_range &r) const noexcept
	{
	  if (!is_predefined())
	    return _directive_range < r;
	  else
	    return true;
	}

	const std::string& get_name() const noexcept
	{ return _name; }

	bool is_predefined() const noexcept
	{ return _o == _origin::o_predefined_user; }

	const raw_pp_tokens_range& get_directive_range() const noexcept
	{
	  assert(!is_predefined());
	  return _directive_range;
	}

	unsigned long get_predefinition_pos() const noexcept
	{
	  assert(is_predefined());
	  return _predefinition_pos;
	}

      private:
	std::string _name;
	_origin _o;
	union
	{
	  raw_pp_tokens_range _directive_range;
	  unsigned long _predefinition_pos;
	};
      };

    private:
      typedef std::vector<macro_undef> _macro_undefs_container_type;

    public:
      typedef _macro_undefs_container_type::const_iterator
	const_macro_undef_iterator;

      const_macro_undef_iterator macro_undefs_begin() const noexcept
      { return _macro_undefs.begin(); }

      const_macro_undef_iterator macro_undefs_end() const noexcept
      { return _macro_undefs.end(); }

      std::pair<const_macro_undef_iterator, const_macro_undef_iterator>
      find_overlapping_macro_undefs(const raw_pp_tokens_range &r)
	const noexcept;


      class used_macros
      {
      private:
	struct _compare
	{
	  bool operator()(const std::reference_wrapper<const macro> a,
			  const std::reference_wrapper<const macro> b)
	    const noexcept;
	};

	typedef std::set<std::reference_wrapper<const macro>, _compare>
		_used_macros_type;

      public:
	class const_iterator :
	  public std::iterator<std::forward_iterator_tag, const macro>
	{
	public:
	  bool operator==(const const_iterator &rhs) const noexcept
	  { return this->_it == rhs._it; }

	  bool operator!=(const const_iterator &rhs) const noexcept
	  {
	    return !(*this == rhs);
	  }

	  reference operator*() const noexcept
	  { return _it->get(); }

	  pointer operator->() const noexcept
	  { return &_it->get(); }

	  const_iterator& operator++() noexcept
	  { ++_it; return *this; }

	  const const_iterator operator++(int) noexcept
	  { return const_iterator{_it++}; }

	private:
	  friend class used_macros;

	  const_iterator(const _used_macros_type::const_iterator &it) noexcept
	    : _it(it)
	  {}

	  _used_macros_type::const_iterator _it;
	};

	used_macros() = default;

	bool empty() const noexcept
	{ return _used_macros.empty(); }

	void clear() noexcept;

	std::size_t count(const macro &m) const noexcept;

	used_macros& operator+=(const used_macros &rhs);

	used_macros& operator+=(const pp_result::macro &rhs);

	const_iterator begin() const noexcept
	{ return const_iterator{_used_macros.begin()}; }

	const_iterator end() const noexcept
	{ return const_iterator{_used_macros.end()}; }

      private:
	used_macros(_used_macros_type &&um);

	_used_macros_type _used_macros;
      };

      class macro_nondef_constraint
      {
      public:
	macro_nondef_constraint(const std::string &id,
				bool func_like_allowed = false);

	bool operator<(const macro_nondef_constraint &rhs) const;

	const std::string& get_name() const noexcept
	{ return _id; }

	bool is_func_like_allowed() const noexcept
	{ return _func_like_allowed; }

      private:
	std::string _id;
	bool _func_like_allowed;
      };

      class macro_nondef_constraints
      {
      private:
	typedef std::set<macro_nondef_constraint>
	  _macro_nondef_constraints_type;

      public:
	class const_iterator :
	  public std::iterator<std::forward_iterator_tag,
			       const macro_nondef_constraint>
	{
	public:
	  bool operator==(const const_iterator &rhs) const noexcept
	  { return this->_it == rhs._it; }

	  bool operator!=(const const_iterator &rhs) const noexcept
	  {
	    return !(*this == rhs);
	  }

	  reference operator*() const noexcept
	  { return *_it; }

	  pointer operator->() const noexcept
	  { return &*_it; }

	  const_iterator& operator++() noexcept
	  { ++_it; return *this; }

	  const const_iterator operator++(int) noexcept
	  { return const_iterator{_it++}; }

	private:
	  friend class macro_nondef_constraints;

	  const_iterator
		(const _macro_nondef_constraints_type::const_iterator &it)
		  noexcept
	    : _it(it)
	  {}

	  _macro_nondef_constraints_type::const_iterator _it;
	};

	macro_nondef_constraints() = default;

	macro_nondef_constraints(macro_nondef_constraints &&mnc);

	macro_nondef_constraints& operator=(macro_nondef_constraints &&rhs);

	macro_nondef_constraints&
	operator+=(const macro_nondef_constraints &rhs);

	macro_nondef_constraints&
	operator+=(const macro_nondef_constraint &rhs);

	const_iterator begin() const noexcept
	{ return const_iterator{_macro_nondef_constraints.begin()}; }

	const_iterator end() const noexcept
	{ return const_iterator{_macro_nondef_constraints.end()}; }

	const_iterator find(const std::string &id) const noexcept;

	_macro_nondef_constraints_type::size_type size() const noexcept
	{ return _macro_nondef_constraints.size(); }

      private:
	_macro_nondef_constraints_type _macro_nondef_constraints;
      };

      class macro_invocation
      {
      public:
	class expansion_forest
	{
	private:
	  class _rescan_macro_invocation;

	  // Represent the full macro expansion history with a forest
	  // of trees of _rescan_macro_invocation nodes.
	  //
	  // During macro expansion, the result gets (recursively)
	  // scanned for more macro invocations and further expanded
	  // as appropriate. This process is represented by a "rescan"
	  // tree of _rescan_macro_invocation nodes, where a single
	  // such node represents the non-recursive expansion of some
	  // macro invocation identified during rescan. Two
	  // _rescan_macro_invocation nodes form a parent-child
	  // relationship whenever the macro identifier token
	  // corresponding to the latter has been found in the
	  // rescanned expansion result of the former. Note that this
	  // does not necessarily mean that the whole token sequence
	  // making up the child's invocation comes from the parent's
	  // output -- in the case of multi-token invocations (i.e. of
	  // function-like macros), the trailing parts can also
	  // originate from outputs higher up in the tree hierarchy or
	  // from raw input even.
	  //
	  // Function-like macros can have arguments, which are also
	  // subject to recursive expansion and thus, a
	  // _rescan_macro_invocation's expanded arguments are
	  // likewise represented by "rescan" trees (*). Note that
	  // some other _rescan_macro_invocation being part of such an
	  // argument expansion tree can in turn have arguments and so
	  // on. Thus, some macro invocation's full expansion process
	  // can be described by a forest of "rescan" trees, with some
	  // additional structure on it:
	  // - There is one distinguished "rescan" tree corresponding
	  //   to the recursively rescanned macro invocation found in
	  //   the raw input.
	  // - Each _rescan_macro_invocation node can link to further
	  //   "rescan" trees, with each such one representing one of
	  //   the macro invocation's arguments. In what follows, this
	  //   _rescan_macro_invocation node is said to "own" the
	  //   "rescan" trees corresponding to its arguments each.
	  //
	  // For the internal representation, all of the forest's
	  // _rescan_macro_invocation nodes are kept in a single
	  // sequence. The relative order of a single tree's nodes
	  // within that sequence is defined to be of DFS pre order.
	  // All "rescan" trees corresponding to arguments are
	  // serialized immediately after their resp. owner node, in
	  // argument order. This fully specifies the serialization
	  // order of all of a forest's node.
	  //
	  // Each _rescan_macro_invocation refers to its "rescan" tree
	  // parent, if any, by means of an index within the forest's
	  // serialized node sequence,
	  // c.f. _rescan_macro_invocation::_rescan_parent. Any
	  // "rescan" tree root has its _rescan_parent set to some
	  // special value reserved for marking roots. Together with
	  // the number of each expansion node's arguments as stored
	  // in _rescan_macro_invocation::_nargs, this allows for the
	  // reconstruction of the full expansion forest structure.
	  //
	  // (*) Within an unexpanded token sequence forming some
	  //     macro argument, there can potentially be multiple,
	  //     non-intersecting macro invocations present, perhaps
	  //     interspersed with some more tokens not subject to
	  //     macro expansion. For the sake of code unformity,
	  //     these are all united under a single dummy
	  //     _rescan_macro_invocation root not corresponding to an
	  //     actual macro invocation, but rather an identity
	  //     transformation on the argument's unexpanded token
	  //     sequence.
	  class _forest_nodes
	  {
	  public:
	    typedef std::vector<_rescan_macro_invocation> _container_type;
	    _container_type _nodes;

	    class size_type
	    {
	    public:
	      constexpr bool operator==(const size_type rhs) const noexcept
	      {
		return this->_n == rhs._n;
	      }

	      constexpr bool operator!=(const size_type rhs) const noexcept
	      { return !(*this == rhs); }

	      static constexpr const size_type invalid() noexcept
	      {
		return size_type{
		  std::numeric_limits<_container_type::size_type>::max()
		};
	      }

	      constexpr bool operator<(const size_type rhs) const noexcept
	      {
		// invalid() is not comparable to anything.
		return rhs != invalid() && this->_n < rhs._n;
	      }

	    private:
	      friend class expansion_forest;
	      friend class _forest_nodes;
	      friend class preprocessor;

	      size_type() = delete;

	      explicit constexpr size_type(const _container_type::size_type n)
		noexcept
		: _n(n)
	      {}

	      _container_type::size_type _n;
	    }; // class size_type

	    _forest_nodes(const macro &m, const std::size_t initial_src_length);

	    _rescan_macro_invocation& operator[](const size_type n) noexcept;

	    const _rescan_macro_invocation& operator[](const size_type n)
	      const noexcept;

	    size_type
	    add_rescan_invocation(_rescan_macro_invocation &&rescan_invocation);

	    size_type
	    add_macro_arg_tree(const _forest_nodes::size_type owner,
			       _rescan_macro_invocation &&arg_root_node);

	    const size_type first_arg_of(const size_type node)
	      const noexcept;

	    const size_type next_arg(const size_type prev_arg_root)
	      const noexcept;

	    const size_type arg_owner(const size_type arg_root) const noexcept;

	    const size_type rescan_tree_root(const size_type descendant_node)
	      const noexcept;

	    const size_type
	    next_rescan_subtree_dfs_pre_node(const size_type subtree_root,
					     const size_type prev_node)
	      const noexcept;

	    const size_type
	    prev_rescan_subtree_dfs_pre_node(const size_type subtree_root,
					     const size_type next_node)
	      const noexcept;

	    const size_type
	    last_rescan_subtree_dfs_pre_node(const size_type root_node)
	      const noexcept;

	    // Position right after the last node of the subtree
	    // located at subtree_root, nested argument trees included.
	    const size_type _rescan_subtree_end(const size_type subtree_root)
	      const noexcept;

	  private:
	    const size_type
	    _next_rescan_subtree_dfs_pre_node(const size_type subtree_root,
					      const size_type prev_node)
	      const noexcept;

	    const size_type
	    _prev_rescan_subtree_dfs_pre_node(const size_type next_node)
	      const noexcept;

	    const size_type
	    _last_rescan_subtree_dfs_pre_node(const size_type subtree_root)
	      const noexcept;

	    const size_type
	    __last_rescan_subtree_dfs_pre_node(const size_type subtree_root,
					       const size_type subtree_end)
	      const noexcept;

	  }; // class _forest_nodes

	  class _rescan_macro_invocation_ptr;

	  class _rescan_macro_invocation_ref
	  {
	  public:
	    _rescan_macro_invocation_ref(const _forest_nodes &forest,
					 const _forest_nodes::size_type node)
	      noexcept;

	    explicit operator _rescan_macro_invocation_ptr() const noexcept;

	    bool operator==(const _rescan_macro_invocation_ref &rhs)
	      const noexcept;

	    const _forest_nodes& get_forest() const noexcept
	    { return _forest.get(); }

	    const _forest_nodes::size_type get_node() const noexcept
	    { return _node; }

	    const _rescan_macro_invocation& deref() const noexcept;

	    bool is_nil_node() const noexcept;

	  private:
	    std::reference_wrapper<const _forest_nodes> _forest;
	    _forest_nodes::size_type _node;
	  }; // class _rescan_macro_invocation_ref;

	  class _rescan_macro_invocation_ptr
	  {
	  public:
	    _rescan_macro_invocation_ptr() noexcept;

	    _rescan_macro_invocation_ptr(const _forest_nodes &forest,
					 const _forest_nodes::size_type node)
	      noexcept;

	    explicit operator bool() const noexcept;

	    explicit operator _rescan_macro_invocation_ref() const noexcept;

	    bool operator==(const _rescan_macro_invocation_ptr &rhs)
	      const noexcept;

	    bool operator!=(const _rescan_macro_invocation_ptr &rhs)
	      const noexcept
	    { return !(*this == rhs); }

	  private:
	    const _forest_nodes *_forest;
	    _forest_nodes::size_type _node;
	  }; // class _rescan_macro_invocation_ref;

	public:
	  class rescan_subtree;

	  class rescan_output_segment;

	  class const_all_nodes_iterator;

	  using nodes_size_type = _forest_nodes::size_type;

	  class rescan_macro_invocation
	  {
	  public:
	    class const_src_iterator
	    {
	    public:
	      using iterator_category = std::forward_iterator_tag;
	      using value_type = const rescan_output_segment;
	      using reference = value_type;

	      const_src_iterator() noexcept;

	      bool operator==(const const_src_iterator &rhs) const noexcept;

	      bool operator!=(const const_src_iterator &rhs) const noexcept
	      { return !(*this == rhs); }

	      reference operator*() const noexcept;

	      const_src_iterator& operator++() noexcept;
	      const_src_iterator operator++(int) noexcept;

	      std::size_t skip_src_distance(const std::size_t distance)
		noexcept;

	      std::size_t rescan_tree_distance() const noexcept
	      { return _rescan_tree_distance; }

	    private:
	      friend class rescan_macro_invocation;

	      struct _begin_tag{};
	      struct _end_tag{};

	      const_src_iterator(const _rescan_macro_invocation_ref &node,
				 const bool cross_arg_tree_roots,
				 const _begin_tag&) noexcept;

	      const_src_iterator(const _rescan_macro_invocation_ref &node,
				 const bool cross_arg_tree_roots,
				 const _end_tag&) noexcept;

	      void _advance_node_to_emitting() noexcept;

	      std::size_t _cur_src_segment_length() const noexcept;

	      const _forest_nodes *_forest;
	      _forest_nodes::size_type _node;
	      std::size_t _remaining_src_length;
	      std::size_t _src_begin;
	      std::size_t _rescan_tree_distance;
	      bool _cross_arg_tree_roots;
	    }; // class const_src_iterator

	    const_src_iterator src_begin(const bool cross_arg_tree_roots)
	      const noexcept;
	    const_src_iterator src_end(const bool cross_arg_tree_roots)
	      const noexcept;

	    class output
	    {
	    public:
	      class tokens_range
	      {
	      public:
		tokens_range() noexcept;

		tokens_range(const std::size_t _begin, const std::size_t _end)
		  noexcept;

		bool operator==(const tokens_range &rhs) const noexcept;

		bool operator!=(const tokens_range &rhs) const noexcept
		{ return !(*this == rhs); }

		bool operator<(const tokens_range &rhs) const noexcept;

		const std::size_t length() const noexcept;

		std::size_t begin;
		std::size_t end;
	      };

	      class fragment
	      {
	      private:
		class _concat_builder;
		friend class _concat_builder;

		friend class output;
		friend class preprocessor;

	      public:
		enum class op
		{
		  repl,
		  arg,
		  stringification,
		  concat,
		  conditional_comma,
		  empty,
		  builtin_special,
		};

		struct repl_tag{};
		struct arg_tag{};
		struct stringification_tag{};
		struct concat_tag{};
		struct conditional_comma_tag{};
		struct builtin_special_tag{};
		struct empty_tag{};

		class repl_ref
		{
		public:
		  std::size_t begin() const noexcept
		  { return _begin; }

		  std::size_t length() const noexcept
		  { return _length; }

		private:
		  friend class expansion_forest;

		  repl_ref() noexcept;

		  repl_ref(const std::size_t begin) noexcept;

		  repl_ref(const std::size_t begin,
			   const std::size_t length) noexcept;

		  void _extend(const std::size_t tail_begin) noexcept;

		  std::size_t _begin;
		  std::size_t _length;
		};

		class arg_ref
		{
		public:
		  const std::size_t get_arg_index() const noexcept
		  { return _arg_index; }

		private:
		  friend class fragment;
		  friend class _concat_builder;

		  arg_ref(const std::size_t arg_index) noexcept;


		  std::size_t _arg_index;
		};

		class stringification
		{
		public:
		  const arg_ref& get_arg() const noexcept
		  { return _arg; }

		private:
		  friend class fragment;
		  friend class _concat_builder;

		  stringification(const arg_ref &arg) noexcept;

		  arg_ref _arg;
		};

		class concat_sequence
		{
		public:
		  class component
		  {
		  public:
		    enum class op
		    {
		      repl,
		      arg,
		      stringification,
		      conditional_comma,
		    };

		  public:
		    template<typename callable_type>
		    auto operator()(callable_type &&c) const ->
		      decltype(c(std::declval<repl_tag>()));

		  private:
		    friend class _concat_builder;
		    friend class concat_sequence;

		    component(repl_ref &&repl, const repl_tag&) noexcept;

		    component(repl_ref &&repl, arg_ref &&arg,
			      const arg_tag&) noexcept;

		    component(repl_ref &&repl, stringification &&s,
			      const stringification_tag&) noexcept;

		    component(repl_ref &&repl, const bool skipped,
			      const conditional_comma_tag&)
		      noexcept;

		    op _op;
		    repl_ref _repl;

		    union {
		      arg_ref _arg;
		      stringification _stringification;
		      bool _conditional_comma_skipped;
		    };
		  }; // class component

		private:
		  using _components_container_type = std::vector<component>;

		public:
		  class output_fragment
		  {
		  public:
		    using repl_ref =
		      rescan_macro_invocation::output::fragment::repl_ref;
		    using arg_ref =
		      rescan_macro_invocation::output::fragment::arg_ref;

		    enum class kind
		    {
		      concatenated_tokens,
		      arg_tokens,
		      stringification,
		      conditional_comma,
		    };

		    class concatenated_tokens
		    {
		    public:
		      class concatenated_token_origin
		      {
		      public:
			enum class origin
			  {
			    repl,
			    arg,
			  };

			struct repl_token
			{
			  std::size_t concatenated_repl_token_index;
			};

			struct arg_token
			{
			  arg_ref arg;
			  std::size_t arg_ref_repl_token_index;
			  std::size_t concatenated_arg_token_index;
			};

			concatenated_token_origin
			(const repl_token &repl) noexcept;

			concatenated_token_origin
			(const arg_token &arg) noexcept;

			concatenated_token_origin
			  (const concatenated_token_origin &rhs) noexcept;

			~concatenated_token_origin() noexcept;

			origin get_origin() const noexcept
			{ return _origin; }

			const repl_token& get_repl_origin() const noexcept;

			const arg_token& get_arg_origin() const noexcept;

		      private:
			origin _origin;

			union {
			  repl_token _repl;
			  arg_token _arg;
			};
		      };

		      concatenated_tokens(const concatenated_tokens&);

		      concatenated_tokens(concatenated_tokens&&) noexcept;

		      ~concatenated_tokens() noexcept;

		      const std::vector<concatenated_token_origin>&
		      get_origins() const noexcept
		      { return _concatenated_tokens_origins; }

		    private:
		      friend class concat_sequence;

		      concatenated_tokens
			(std::vector<concatenated_token_origin>
			 &&concatenated_tokens_origins) noexcept;

			std::vector<concatenated_token_origin>
			_concatenated_tokens_origins;
		    };

		    struct arg_tokens
		    {
		      arg_ref arg;
		      rescan_macro_invocation::output::tokens_range arg_tokens;
		    };

		    struct stringification
		    {
		      arg_ref arg;
		    };

		    struct conditional_comma {};

		    output_fragment(output_fragment &&frag) noexcept;
		    ~output_fragment() noexcept;

		    const repl_ref& get_repl() const noexcept
		    { return _repl; }

		    std::size_t get_out_begin() const noexcept
		    { return _out_begin; }

		    kind get_kind() const noexcept
		    { return _kind; }

		    const concatenated_tokens&
		    get_concatenated_tokens_fragment() const noexcept;

		    const arg_tokens& get_arg_tokens_fragment()
		      const noexcept;

		    const stringification& get_stringification_fragment()
		      const noexcept;

		    const conditional_comma& get_conditional_comma_fragment()
		      const noexcept;

		  private:
		    friend class concat_sequence;

		    output_fragment(repl_ref &&repl,
				    const std::size_t out_begin,
				    concatenated_tokens &&concatenated_tokens)
		      noexcept;

		    output_fragment(repl_ref &&repl,
				    const std::size_t out_begin,
				    arg_tokens &&arg_tokens)
		      noexcept;

		    output_fragment(repl_ref &&repl,
				    const std::size_t out_begin,
				    stringification &&stringification)
		      noexcept;

		    output_fragment(repl_ref &&repl,
				    const std::size_t out_begin,
				    conditional_comma &&conditional_comma)
		      noexcept;

		    kind _kind;
		    union {
		      concatenated_tokens _concatenated_tokens;
		      arg_tokens _arg_tokens;
		      stringification _stringification;
		      conditional_comma _conditional_comma;
		    };
		    repl_ref _repl;

		    // Relative to the tokens emitted by the concat_sequence.
		    std::size_t _out_begin;
		  };

		  concat_sequence(const concat_sequence&) = delete;

		  concat_sequence(concat_sequence&&) = default;

		  const _components_container_type::size_type
		  size() const noexcept;

		  const component&
		  operator[](const _components_container_type::size_type i)
		    const noexcept;

		  bool is_stringfication() const noexcept;

		  bool is_conditional_comma() const noexcept;

		  std::vector<output_fragment> get_output_fragments
		    (const rescan_macro_invocation &emitting_rmi) const;

		private:
		  friend class fragment;
		  friend class _concat_builder;

		  concat_sequence() = default;

		  void _extend(component &&c);

		  _components_container_type _components;
		}; // class concat_sequence

		fragment(fragment &&seg);

		~fragment() noexcept;

		const tokens_range& get_out_range() const noexcept
		{ return _out_range; }

		template<typename callable_type>
		auto operator()(callable_type &&c) const ->
		  decltype(c(std::declval<repl_tag>()));

		const op get_op() const noexcept
		{ return _op; }

		const repl_ref &get_repl() const noexcept
		{ return _repl; }

		const arg_ref& get_arg_ref_fragment() const noexcept;

		const stringification&
		get_stringification_fragment() const noexcept;

		const concat_sequence& get_concat_sequence_fragment()
		  const noexcept;

	      private:
		friend class output;

		class _concat_builder
		{
		public:
		  void add_component(const std::size_t repl_begin,
				     const repl_tag&);

		  void add_component(const std::size_t repl_begin,
				     const std::size_t arg_index,
				     const std::size_t arg_length,
				     const arg_tag&);

		  void add_component(const std::size_t repl_begin,
				     const std::size_t repl_end,
				     const std::size_t arg_index,
				     const stringification_tag&) noexcept;

		  void add_component(const std::size_t repl_begin,
				     const std::size_t repl_end,
				     const bool skipped,
				     const conditional_comma_tag&) noexcept;

		private:
		  friend class output;

		  _concat_builder(fragment &frag) noexcept;

		  std::reference_wrapper<fragment> _frag;
		};

		fragment(const std::size_t repl_begin,
			 const std::size_t out_begin,
			 const repl_tag&);

		fragment(const std::size_t arg_ref_repl_pos,
			 const std::size_t out_begin,
			 const std::size_t out_length,
			 const std::size_t arg_index,
			 const arg_tag&);

		fragment(const std::size_t repl_begin,
			 const std::size_t repl_length,
			 const std::size_t out_begin,
			 const std::size_t arg_index,
			 const stringification_tag&);

		fragment(const std::size_t repl_begin,
			 const std::size_t out_begin,
			 const concat_tag&);

		fragment(const std::size_t repl_begin,
			 const std::size_t repl_length,
			 const std::size_t out_begin,
			 const bool skipped,
			 const conditional_comma_tag&);

		fragment(const std::size_t out_begin,
			 const empty_tag&);

		fragment(const std::size_t out_begin,
			 const std::size_t out_length,
			 const builtin_special_tag&);

		op _op;

		// Relative to the emitting rescan macro invocation's
		// output's beginning.
		tokens_range _out_range;

		repl_ref _repl;

		union {
		  arg_ref _arg;
		  stringification _stringification;
		  concat_sequence _concat_sequence;
		};
	      }; // class fragment

	      output() noexcept;

	      output(output &&) noexcept;

	      ~output() noexcept;

	      std::size_t out_length() const noexcept;

	      const pp_tokens& get_result() const noexcept
	      { return _result; }

	    private:
	      using _fragments_container_type = std::vector<fragment>;

	    public:
	      using const_fragment_iterator =
		_fragments_container_type::const_iterator;

	      const_fragment_iterator fragments_begin() const noexcept
	      { return _fragments.cbegin(); }

	      const_fragment_iterator fragments_end() const noexcept
	      { return _fragments.cend(); }

	      std::pair<const_fragment_iterator, const_fragment_iterator>
	      overlapping_fragments(const tokens_range &range) const noexcept
	      { return _overlapping_fragments(range); }

	    private:
	      friend class expansion_forest;
	      friend class preprocessor;

	      std::size_t _next_fragment_out_begin() const noexcept;

	      void _add_fragment(const std::size_t repl_begin,
				 const fragment::repl_tag&);

	      void _add_fragment(const std::size_t arg_ref_repl_pos,
				 const std::size_t arg_index,
				 const std::size_t out_length,
				 const fragment::arg_tag&);

	      void _add_fragment(const std::size_t repl_begin,
				 const std::size_t repl_end,
				 const std::size_t arg_index,
				 const fragment::stringification_tag&);

	      fragment::_concat_builder
	      _add_fragment(const std::size_t repl_begin,
			    const fragment::concat_tag&);

	      void _add_fragment(const std::size_t repl_begin,
				 const std::size_t repl_end,
				 const bool skipped,
				 const fragment::conditional_comma_tag&);

	      void _add_fragment(const fragment::empty_tag&);

	      void _add_fragment(const std::size_t out_length,
				 const fragment::builtin_special_tag&);

	      std::pair<_fragments_container_type::const_iterator,
			_fragments_container_type::const_iterator>
	      _overlapping_fragments(const tokens_range &range) const noexcept;

	      _fragments_container_type _fragments;
	      pp_tokens _result;
	    }; // class output


	    explicit operator rescan_subtree() const noexcept;

	    explicit operator nodes_size_type() const noexcept;

	    explicit operator rescan_output_segment() const noexcept;

	    bool operator==(const rescan_macro_invocation &rhs) const noexcept;

	    const bool is_nil_node() const noexcept;

	    const bool is_rescan_tree_root() const noexcept;

	    const rescan_macro_invocation get_rescan_parent() const noexcept;

	    const bool is_arg_tree_root() const noexcept;

	    const rescan_macro_invocation get_arg_tree_owner() const noexcept;

	    const macro& get_macro() const noexcept;

	    const rescan_subtree get_arg_tree(const std::size_t arg_index)
	      const noexcept;

	    const output& get_output() const noexcept;

	    // Length of the output. For argument tree roots, this is the
	    // length of the (identity mapped) unexpanded argument.
	    std::size_t out_length() const noexcept;

	    // The invocation's source range. For rescan children,
	    // relative to the rescan parent's output, for argument
	    // tree roots relative to the owner's rescan parent's
	    // output, and relative to the invocation_source otherwise.
	    const output::tokens_range& get_src_range() const noexcept;

	    // Number of arguments collected for this invocation.
	    std::size_t get_nargs() const noexcept;

	  private:
	    friend class expansion_forest;

	    rescan_macro_invocation(const _rescan_macro_invocation_ref &ref)
	      noexcept;

	    _rescan_macro_invocation_ref _ref;
	  }; // class rescan_macro_invocation

	  class rescan_output_segment
	  {
	  public:
	    class const_fragment_iterator
	    {
	    private:
	      using fragment = rescan_macro_invocation::output::fragment;
	    public:
	      using iterator_category = std::bidirectional_iterator_tag;

	      struct value_type
	      {
		const fragment &fragment;
		// Relative to the fragment output's beginning.
		rescan_macro_invocation::output::tokens_range range_in_fragment;
	      };

	      using reference = value_type;

	      const_fragment_iterator() noexcept;

	      bool operator==(const const_fragment_iterator &rhs)
		const noexcept;

	      bool operator!=(const const_fragment_iterator &rhs) const noexcept
	      { return !(*this == rhs); }

	      reference operator*() const noexcept;

	      const_fragment_iterator& operator++() noexcept;
	      const_fragment_iterator operator++(int) noexcept;

	      const_fragment_iterator& operator--() noexcept;
	      const_fragment_iterator operator--(int) noexcept;

	    private:
	      friend class rescan_output_segment;

	      using _fragments_container_type =
		rescan_macro_invocation::output::_fragments_container_type;

	      const_fragment_iterator
		(const rescan_output_segment &seg,
		 const _fragments_container_type::const_iterator it_frag)
		noexcept;

	      const_fragment_iterator
		(const _rescan_macro_invocation_ref &seg_emitting_node,
		 const rescan_macro_invocation::output::tokens_range &
		   seg_out_range_in_emitting_node,
		 const _fragments_container_type::const_iterator it_frag)
		noexcept;

	      _rescan_macro_invocation_ptr _seg_emitting_node;
	      rescan_macro_invocation::output::tokens_range
		_seg_out_range_in_emitting_node;
	      _fragments_container_type::const_iterator _it_frag;
	    }; // class const_fragment_iterator

	    bool operator==(const rescan_output_segment &rhs) const noexcept;

	    bool operator!=(const rescan_output_segment &rhs) const noexcept
	    { return !(*this == rhs); }

	    std::pair<const_fragment_iterator, const_fragment_iterator>
	    fragments() const noexcept;

	    const rescan_output_segment
	    crop(const rescan_macro_invocation::output::tokens_range &limit)
	      const noexcept;

	    const rescan_output_segment
	    crop_relative
	      (const rescan_macro_invocation::output::tokens_range &limit)
	      const noexcept;

	    const rescan_macro_invocation get_emitting_node() const noexcept;

	    const rescan_macro_invocation::output::tokens_range&
	    get_out_range_in_emitting_node() const noexcept
	    { return _range_in_emitting_node; }

	  private:
	    friend class expansion_forest;

	    rescan_output_segment
	      (const _rescan_macro_invocation_ref &emitting_node,
	       const rescan_macro_invocation::output::tokens_range
		range_in_emitting_node) noexcept;

	    _rescan_macro_invocation_ref _emitting_node;
	    rescan_macro_invocation::output::tokens_range
	      _range_in_emitting_node;
	  }; // class rescan_output_segment

	private:
	  class _rescan_macro_invocation
	  {
	  public:
	    const rescan_macro_invocation::output::tokens_range &
	    get_source_range() const noexcept
	    { return _src_range; }

	    std::size_t out_length() const noexcept;

	    bool is_rescan_tree_root() const noexcept;

	    bool is_arg_tree_root() const noexcept;

	    _forest_nodes::size_type get_rescan_parent() const noexcept;

	    std::size_t nargs() const noexcept
	    { return _nargs; }

	    const macro& get_macro() const noexcept;

	    const rescan_macro_invocation::output& get_output() const noexcept
	    { return _output; }

	  private:
	    friend class expansion_forest;
	    friend class preprocessor;

	    _rescan_macro_invocation
		(const _forest_nodes::size_type rescan_parent,
		 const macro &m,
		 const rescan_macro_invocation::output::tokens_range &src_range)
	      noexcept;

	    _rescan_macro_invocation
		(const rescan_macro_invocation::output::tokens_range &src_range)
	      noexcept;

	    _forest_nodes::size_type _rescan_parent;
	    std::size_t _nargs;

	    // For rescan childs, relative to _parent's output begin, if any,
	    // otherwise relative to the outer invocation_source.
	    // For argument nodes, relative to the owner's _parent's
	    // output begin, if any, otherwise relative to the outer
	    // invocation_source.
	    rescan_macro_invocation::output::tokens_range _src_range;

	    const macro *_m;

	    rescan_macro_invocation::output _output;
	  }; // class _rescan_macro_invocation

	public:
	  class rescan_subtree
	  {
	  public:
	    class const_dfs_pre_iterator
	    {
	    public:
	      using iterator_category = std::bidirectional_iterator_tag;
	      using value_type = const rescan_macro_invocation;
	      using reference = value_type;

	      const_dfs_pre_iterator() noexcept;

	      bool operator==(const const_dfs_pre_iterator &rhs) const noexcept;

	      bool operator!=(const const_dfs_pre_iterator &rhs) const noexcept
	      { return !(*this == rhs); }

	      reference operator*() const noexcept;

	      const_dfs_pre_iterator& operator++() noexcept;
	      const_dfs_pre_iterator operator++(int) noexcept;

	      const_dfs_pre_iterator& operator--() noexcept;
	      const_dfs_pre_iterator operator--(int) noexcept;

	    private:
	      friend class rescan_subtree;

	      struct _begin_tag{};
	      struct _end_tag{};

	      const_dfs_pre_iterator(const _rescan_macro_invocation_ref &root,
				     const _begin_tag&) noexcept;

	      const_dfs_pre_iterator(const _rescan_macro_invocation_ref &root,
				     const _end_tag&) noexcept;

	      const _forest_nodes *_forest;
	      _forest_nodes::size_type _subtree_root;
	      _forest_nodes::size_type _node;
	      bool _at_end;
	    }; // class const_dfs_pre_iterator

	    class const_dfs_pre_post_iterator
	    {
	    public:
	      using iterator_category = std::forward_iterator_tag;
	      using value_type =
		const std::tuple<bool, bool, rescan_macro_invocation>;
	      using reference = value_type;

	      const_dfs_pre_post_iterator() noexcept;

	      bool operator==(const const_dfs_pre_post_iterator &rhs)
		const noexcept;

	      bool operator!=(const const_dfs_pre_post_iterator &rhs)
		const noexcept
	      { return !(*this == rhs); }

	      reference operator*() const noexcept;

	      const_dfs_pre_post_iterator& operator++() noexcept;
	      const_dfs_pre_post_iterator operator++(int) noexcept;

	    private:
	      friend class rescan_subtree;

	      struct _begin_tag{};
	      struct _end_tag{};

	      const_dfs_pre_post_iterator
		(const _rescan_macro_invocation_ref &root, const _begin_tag&)
		noexcept;

	      const_dfs_pre_post_iterator
		(const _rescan_macro_invocation_ref &root, const _end_tag&)
		  noexcept;

	      const _forest_nodes *_forest;
	      _forest_nodes::size_type _subtree_root;
	      _forest_nodes::size_type _node;
	      _forest_nodes::size_type _next_dfs_pre_node;
	      bool _at_dfs_pre;
	    }; // class const_dfs_pre_post_iterator

	    class const_final_out_iterator
	    {
	    public:
	      using iterator_category = std::forward_iterator_tag;
	      using value_type = const rescan_output_segment;
	      using reference = value_type;

	      const_final_out_iterator() noexcept;

	      bool operator==(const const_final_out_iterator &rhs)
		const noexcept;

	      bool operator!=(const const_final_out_iterator &rhs)
		const noexcept
	      { return !(*this == rhs); }

	      reference operator*() const noexcept;

	      const_final_out_iterator& operator++();
	      const_final_out_iterator operator++(int);

	      std::size_t skip_out_distance(const std::size_t distance);

	      std::size_t depth() const noexcept;

	    private:
	      friend class rescan_subtree;

	      struct _begin_tag{};
	      struct _end_tag{};

	      const_final_out_iterator
		(const _rescan_macro_invocation_ref &subtree_root,
		 const _begin_tag&);

	      const_final_out_iterator
		(const _rescan_macro_invocation_ref &subtree_root,
		 const _end_tag&);

	      std::size_t _cur_out_segment_length() const noexcept;

	      void _enter_branch_at_pos();

	      const _forest_nodes *_forest;
	      _forest_nodes::size_type _subtree_root;
	      _forest_nodes::size_type _stack_top_node;
	      _forest_nodes::size_type _next_dfs_pre_node;

	      std::stack<std::size_t> _consumed_out_lengths_stack;
	    }; // class const_final_out_iterator

	    class const_final_out_origin_iterator
	    {
	    public:
	      class output_fragment_origin
	      {
	      public:
		enum class origin
		{
		  replacement,
		  verbatim_invocation_arg,
		  concatenated_tokens,
		  stringification,
		  builtin_special,
		  empty,
		};

		struct replacement
		{
		  std::reference_wrapper<const macro> m;
		  rescan_macro_invocation::output::fragment::repl_ref repl;
		};

		struct verbatim_invocation_arg
		{
		  rescan_macro_invocation::output::tokens_range src_tokens;
		};

		struct concatenated_tokens
		{
		  rescan_macro_invocation emitting_rmi;
		  std::size_t out_token_index;
		  rescan_macro_invocation::output::fragment::
		    concat_sequence::output_fragment::concatenated_tokens
		    concatenated_tokens;
		};

		struct stringification
		{
		  rescan_macro_invocation emitting_rmi;
		  std::size_t out_token_index;
		  rescan_macro_invocation::output::fragment::arg_ref arg;
		};

		struct builtin_special
		{
		  rescan_macro_invocation builtin_special_rmi;
		  rescan_macro_invocation::output::tokens_range out_tokens;
		};

		struct empty {};

		output_fragment_origin(output_fragment_origin &&rhs) noexcept;
		~output_fragment_origin() noexcept;

		origin get_origin() const noexcept
		{ return _origin; }

		const replacement& get_replacement_origin() const noexcept;

		const verbatim_invocation_arg&
		get_verbatim_invocation_arg_origin() const noexcept;

		const concatenated_tokens& get_concatenated_tokens_origin()
		  const noexcept;

		const stringification& get_stringification_origin()
		  const noexcept;

		const builtin_special& get_builtin_special_origin()
		  const noexcept;

	      private:
		friend class const_final_out_origin_iterator;

		output_fragment_origin(replacement &&replacement) noexcept;

		output_fragment_origin
		  (verbatim_invocation_arg &&verbatim_invocation_arg) noexcept;

		output_fragment_origin
		  (concatenated_tokens &&concatenated_tokens) noexcept;

		output_fragment_origin
		  (stringification &&stringification) noexcept;

		output_fragment_origin
		  (builtin_special &&builtin_special) noexcept;

		output_fragment_origin
		  (empty &&empty) noexcept;

		origin _origin;

		union {
		  replacement _replacement;
		  verbatim_invocation_arg _verbatim_invocation_arg;
		  concatenated_tokens _concatenated_tokens;
		  stringification _stringification;
		  builtin_special _builtin_special;
		  empty _empty;
		};
	      };

	      bool operator==(const const_final_out_origin_iterator &rhs)
		const noexcept;

	      bool operator!=(const const_final_out_origin_iterator &rhs)
		const noexcept
	      { return !(*this == rhs); }

	      const_final_out_origin_iterator& operator++();

	      output_fragment_origin operator*() const;

	    private:
	      friend class rescan_subtree;

	      const_final_out_origin_iterator() noexcept;

	      const_final_out_origin_iterator
		(const rescan_subtree &subtree,
		 const rescan_macro_invocation::output::tokens_range
		   &out_range);

	      using rmi_out_fragments_iter
	      = rescan_output_segment::const_fragment_iterator;
	      using rmi_out_fragment
	      = rescan_macro_invocation::output::fragment;
	      using concat_sequence_out_fragment =
		rmi_out_fragment::concat_sequence::output_fragment;

	      class _stack_entry_rmi_out_segments_iterator
	      {
	      public:
		_stack_entry_rmi_out_segments_iterator
		(const_final_out_iterator &&it,
		 const_final_out_iterator &&end,
		 const rescan_macro_invocation::output::tokens_range &out_range)
		  noexcept;

		_stack_entry_rmi_out_segments_iterator
		  (rescan_macro_invocation::const_src_iterator &&it,
		   rescan_macro_invocation::const_src_iterator &&end,
		 const rescan_macro_invocation::output::tokens_range &out_range)
		  noexcept;

		_stack_entry_rmi_out_segments_iterator
		  (_stack_entry_rmi_out_segments_iterator &&rhs) noexcept;

		~_stack_entry_rmi_out_segments_iterator() noexcept;

		bool
		operator==(const _stack_entry_rmi_out_segments_iterator &rhs)
		  const noexcept;

		const rescan_output_segment operator*() const noexcept;

		_stack_entry_rmi_out_segments_iterator& operator++();

		bool at_end() const noexcept;

		const rescan_macro_invocation::output::tokens_range&
		get_out_range() const noexcept
		{ return _out_range; }

	      private:
		enum _kind
		{
		  final_out,
		  src,
		};

		struct _final_out_segments
		{
		  const_final_out_iterator it;
		  const_final_out_iterator end;
		};

		struct _src_segments
		{
		  rescan_macro_invocation::const_src_iterator it;
		  rescan_macro_invocation::const_src_iterator end;
		};

		_kind _kind;

		union {
		  _final_out_segments _out_segs;
		  _src_segments _src_segs;
		};

		// Relative to the range covered by the current
		// _out_segs/_src_segs.
		rescan_macro_invocation::output::tokens_range _out_range;
	      };

	      struct _stack_entry_rmi_out_segment_cursor
	      {
		_stack_entry_rmi_out_segment_cursor
		  (const rescan_output_segment &_out_segment,
		   const rescan_macro_invocation::output::tokens_range
		     &out_range);

		bool operator==(const _stack_entry_rmi_out_segment_cursor &rhs)
		  const noexcept;

		void _enter_fragment();

		rescan_output_segment out_segment;

		std::pair<rmi_out_fragments_iter,
			  rmi_out_fragments_iter> fragments;

		std::vector<concat_sequence_out_fragment>
		  cur_concat_sequence_fragments;
		std::vector<concat_sequence_out_fragment>::const_iterator
		  it_cur_concat_sequence_fragments;
	      };

	      struct _stack_entry
	      {
		_stack_entry
		  (_stack_entry_rmi_out_segments_iterator &&_it_segment);

		bool operator==(const _stack_entry &rhs) const noexcept;

		_stack_entry_rmi_out_segments_iterator it_segment;
		_stack_entry_rmi_out_segment_cursor cursor;
	      };

	      void _enter_arg_fragment();

	      std::stack<_stack_entry> _stack;
	    }; // const_final_out_origin_iterator

	    explicit operator rescan_macro_invocation() const noexcept;

	    const_dfs_pre_iterator dfs_pre_begin() const noexcept;
	    const_dfs_pre_iterator dfs_pre_end() const noexcept;

	    const_dfs_pre_post_iterator dfs_pre_post_begin() const noexcept;
	    const_dfs_pre_post_iterator dfs_pre_post_end() const noexcept;

	    const_final_out_iterator final_out_begin() const;
	    const_final_out_iterator final_out_end() const;

	    // All nodes of the subtree, nested argument trees
	    // included, in serialization order: DFS pre order with
	    // argument trees right after their owner.
	    const_all_nodes_iterator all_nodes_begin() const noexcept;
	    const_all_nodes_iterator all_nodes_end() const noexcept;

	    std::size_t total_src_length() const;
	    std::size_t total_out_length() const;

	  private:
	    friend class expansion_forest;

	    rescan_subtree(const _rescan_macro_invocation_ref &subtree_root)
	      noexcept;

	    _rescan_macro_invocation_ref _subtree_root;
	  }; // class rescan_subtree

	  class const_all_nodes_iterator
	  {
	  public:
	    using iterator_category = std::bidirectional_iterator_tag;

	    using value_type = rescan_macro_invocation;
	    using reference = value_type;

	    const_all_nodes_iterator() noexcept;

	    bool operator==(const const_all_nodes_iterator &rhs)
	      const noexcept;

	    bool operator!=(const const_all_nodes_iterator &rhs) const noexcept
	    { return !(*this == rhs); }

	    reference operator*() const noexcept;

	    const_all_nodes_iterator& operator++() noexcept;
	    const_all_nodes_iterator operator++(int) noexcept;

	    const_all_nodes_iterator& operator--() noexcept;
	    const_all_nodes_iterator operator--(int) noexcept;

	  private:
	    friend class expansion_forest;
	    friend class rescan_subtree;

	    struct begin_tag{};
	    struct end_tag{};

	    const_all_nodes_iterator(const _forest_nodes &forest,
				     const begin_tag&) noexcept;

	    const_all_nodes_iterator(const _forest_nodes &forest,
				     const _forest_nodes::size_type node)
	      noexcept;

	    const_all_nodes_iterator(const _forest_nodes &forest,
				     const end_tag&) noexcept;

	    const _forest_nodes *_forest;
	    _forest_nodes::_container_type::size_type _node_index;
	  }; // class const_all_nodes_iterator

	  class invocation_source
	  {
	  public:
	    struct segment
	    {
	      explicit operator raw_pp_tokens_range() const noexcept;

	      raw_pp_token_index raw_pp_tokens_begin;
	      rescan_macro_invocation::output::tokens_range invocation_range;
	    };

	  private:
	    using _segments_container_type = std::vector<segment>;

	  public:
	    class const_segment_iterator
	    {
	    public:
	      using iterator_category = std::bidirectional_iterator_tag;

	      using value_type = segment;
	      using reference = value_type;

	      const_segment_iterator() noexcept;

	      bool operator==(const const_segment_iterator &rhs)
		const noexcept;

	      bool operator!=(const const_segment_iterator &rhs) const noexcept
	      { return !(*this == rhs); }

	      reference operator*() const noexcept;

	      const_segment_iterator& operator++() noexcept;
	      const_segment_iterator operator++(int) noexcept;


	      const_segment_iterator& operator--() noexcept;
	      const_segment_iterator operator--(int) noexcept;

	    private:
	      friend class invocation_source;

	      const_segment_iterator
		(const _segments_container_type::const_iterator it_seg,
		 const rescan_macro_invocation::output::tokens_range
		   invocation_range) noexcept;

	      _segments_container_type::const_iterator _it_seg;
	      rescan_macro_invocation::output::tokens_range
		_invocation_range;
	    }; // class const_segment_iterator

	    const_segment_iterator segments_begin() const noexcept;
	    const_segment_iterator segments_end() const noexcept;

	    std::pair<const_segment_iterator, const_segment_iterator>
	    overlapping_segments
	      (const rescan_macro_invocation::output::tokens_range &range)
	      const noexcept;

	    const raw_pp_tokens_range get_total_spanned_source_range()
	      const noexcept;

	    const rescan_macro_invocation::output::tokens_range
	    get_total_invocation_range() const noexcept
	    { return _get_total_invocation_range(); }

	  private:
	    friend class expansion_forest;
	    friend class preprocessor;

	    void _append_raw_pp_token(const raw_pp_token_index raw_tok_index);

	    const rescan_macro_invocation::output::tokens_range
	    _get_total_invocation_range() const noexcept;

	    _segments_container_type _segments;
	  }; // class invocation_source

	  expansion_forest
	    (const macro &m,
	     expansion_forest::invocation_source &&initial_invocation_source);

	  const rescan_macro_invocation
	  operator[](const nodes_size_type node_index) const noexcept;

	  const rescan_subtree get_main_tree() const noexcept;

	  const_all_nodes_iterator all_nodes_begin() const noexcept;
	  const_all_nodes_iterator all_nodes_end() const noexcept;

	  const invocation_source& get_invocation_source() const noexcept
	  { return _invocation_src; }

	private:
	  friend class preprocessor;

	  _forest_nodes _forest;
	  invocation_source _invocation_src;
	}; // class expansion_forest

      private:
	expansion_forest _expansion_forest;

      public:
	const expansion_forest& get_expansion_forest() const noexcept
	{ return _expansion_forest; }

	const raw_pp_tokens_range get_source_range() const noexcept;

	const pp_tokens_range get_output_range(const pp_result &pp_result)
	  const noexcept;

	bool operator<(const raw_pp_tokens_range &r) const noexcept
	{ return get_source_range() < r; }

	const used_macros& get_used_macros() const noexcept
	{ return _used_macros; }

	const macro_nondef_constraints& get_macro_nondef_constraints()
	  const noexcept
	{ return _macro_nondef_constraints; }

      private:
	friend class pp_result;
	friend class preprocessor;

	macro_invocation
	  (const macro &m,
	   expansion_forest::invocation_source &&initial_invocation_source);

	used_macros _used_macros;
	macro_nondef_constraints _macro_nondef_constraints;
      };

    private:
      typedef std::vector<std::unique_ptr<macro_invocation>>
	_macro_invocations_container_type;

    public:
      class const_macro_invocation_iterator
	: public std::iterator<std::random_access_iterator_tag,
			       const macro_invocation>
      {
      public:
	bool operator==(const const_macro_invocation_iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const const_macro_invocation_iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept
	{ return **_it; }

	pointer operator->() const noexcept
	{ return (*_it).get(); }

	const_macro_invocation_iterator& operator++() noexcept
	{ ++_it; return *this; }

	const const_macro_invocation_iterator operator++(int) noexcept
	{ return const_macro_invocation_iterator{_it++}; }

	const_macro_invocation_iterator& operator--() noexcept
	{ --_it; return *this; }

	const const_macro_invocation_iterator operator--(int) noexcept
	{ return const_macro_invocation_iterator{_it--}; }

	const_macro_invocation_iterator&
	operator+=(const difference_type n) noexcept
	{ _it += n; return *this; }

	const_macro_invocation_iterator&
	operator-=(const difference_type n) noexcept
	{ _it -= n; return *this; }

	const const_macro_invocation_iterator
	operator+(const difference_type n) const noexcept
	{ return const_macro_invocation_iterator(_it + n); }

	const const_macro_invocation_iterator
	operator-(const difference_type n) const noexcept
	{ return const_macro_invocation_iterator(_it - n); }

	const difference_type
	operator-(const const_macro_invocation_iterator &op) const noexcept
	{ return this->_it - op._it; }

	reference operator[](const difference_type n) const noexcept
	{ return *(*this + n); }

	bool operator<(const const_macro_invocation_iterator &op) const noexcept
	{ return this->_it < op._it; }

	bool operator>(const const_macro_invocation_iterator &op) const noexcept
	{ return this->_it > op._it; }

	bool operator<=(const const_macro_invocation_iterator &op)
	  const noexcept
	{ return this->_it <= op._it; }

	bool operator>=(const const_macro_invocation_iterator &op)
	  const noexcept
	{ return this->_it >= op._it; }

      private:
	friend class pp_result;

	const_macro_invocation_iterator
		(const _macro_invocations_container_type::const_iterator &it)
		  noexcept
	  : _it(it)
	{}

	_macro_invocations_container_type::const_iterator _it;
      };

      const const_macro_invocation_iterator
      macro_invocations_begin() const noexcept
      { return const_macro_invocation_iterator(_macro_invocations.cbegin()); }

      const const_macro_invocation_iterator
      macro_invocations_end() const noexcept
      { return const_macro_invocation_iterator(_macro_invocations.cend()); }

      std::pair<const_macro_invocation_iterator,
		const_macro_invocation_iterator>
      find_overlapping_macro_invocations(const raw_pp_tokens_range &r)
	const noexcept;

      std::pair<std::pair<const_macro_invocation_iterator, pp_token_index>,
		std::pair<const_macro_invocation_iterator, pp_token_index>>
      find_overlapping_macro_invocations(const pp_tokens_range &r)
	const noexcept;

    private:
      class _pp_tokens_origins {
      private:
	typedef _macro_invocations_container_type::size_type
	  _macro_invocations_index_type;

      public:
	struct from_raw_tag {};
	struct from_macro_expansion_tag {};

	_pp_tokens_origins() noexcept;

	_pp_tokens_origins(_pp_tokens_origins &&rhs) noexcept;

	~_pp_tokens_origins() noexcept;

	void append(const raw_pp_token_index raw_index,
		    const _macro_invocations_container_type &macro_invocations,
		    const from_raw_tag);

	void append(const _macro_invocations_index_type mi_index,
		    const _macro_invocations_container_type &macro_invocations,
		    const from_macro_expansion_tag);

	void drop_pp_tokens_tail
	  (const pp_token_index new_end,
	   const _macro_invocations_container_type &macro_invocations)
	  noexcept;

	raw_pp_tokens_range pp_tokens_range_to_raw
	  (const pp_tokens_range &range,
	   const _macro_invocations_container_type &macro_invocations)
	  const noexcept;

	raw_pp_tokens_range pp_token_index_to_raw
	  (const pp_token_index tok_index,
	   const _macro_invocations_container_type &macro_invocations)
	  const noexcept;

	pp_tokens_range
	raw_pp_tokens_range_to_nonraw
	  (const raw_pp_tokens_range &raw_range,
	   const _macro_invocations_container_type &macro_invocations)
	  const noexcept;

	std::pair<std::pair<const_macro_invocation_iterator, pp_token_index>,
		  std::pair<const_macro_invocation_iterator, pp_token_index>>
	find_overlapping_macro_invocations
	  (const pp_tokens_range &range,
	   const _macro_invocations_container_type &macro_invocations)
	  const noexcept;

      private:
	friend class pp_result;

	struct _pp_tokens_seq_origin {
	  _pp_tokens_seq_origin(const pp_token_index out_begin,
				const raw_pp_token_index raw_begin,
				const from_raw_tag) noexcept;

	  _pp_tokens_seq_origin
	  (const pp_token_index out_begin,
	   const _macro_invocations_index_type mi_index,
	   const from_macro_expansion_tag) noexcept;

	  std::size_t
	  out_length(const _macro_invocations_container_type &macro_invocations)
	    const noexcept;

	  pp_token_index
	  out_end(const _macro_invocations_container_type &macro_invocations)
	    const noexcept;

	  raw_pp_tokens_range get_raw_pp_tokens_range
	    (const _macro_invocations_container_type &macro_invocations)
	    const noexcept;

	  enum class _kind
	  {
	    from_raw,
	    from_macro_expansion,
	  };

	  struct _raw_source
	  {
	    raw_pp_tokens_range range;
	  };

	  struct _macro_expansion_source
	  {
	    _macro_invocations_index_type mi_index;
	  };

	  union _source
	  {
	    _raw_source raw_source;
	    _macro_expansion_source macro_expansion;
	  };

	  pp_token_index _out_begin;
	  _kind _kind;
	  _source _source;
	};

	typedef std::vector<_pp_tokens_seq_origin>
	  _pp_tokens_seqs_origins_container_type;

	typedef _pp_tokens_seqs_origins_container_type::size_type
	  _pp_tokens_seqs_origins_index_type;

	pp_token_index _total_out_end
	(const _macro_invocations_container_type &macro_invocations)
	  const noexcept;

	raw_pp_token_index _total_raw_end
	(const _macro_invocations_container_type &macro_invocations)
	  const noexcept;

	std::pair<_pp_tokens_seqs_origins_index_type, std::size_t>
	_lookup
	(const pp_token_index tok_index,
	 const _macro_invocations_container_type &macro_invocations)
	  const noexcept;

	std::pair<
	  std::pair<_pp_tokens_seqs_origins_index_type, std::size_t>,
	  std::pair<_pp_tokens_seqs_origins_index_type, std::size_t>
	>
	_lookup
	(const pp_tokens_range &range,
	 const _macro_invocations_container_type &macro_invocations)
	  const noexcept;

	_pp_tokens_seqs_origins_container_type _pp_tokens_seqs_origins;
      };

    public:
      class header_inclusion_node;
      class header_inclusion_child;
      class conditional_inclusion_node;
      class const_intersecting_source_iterator;

      class inclusion_node
      {
      private:
	friend class const_intersecting_source_iterator;

	struct _child
	{
	  _child(std::unique_ptr<header_inclusion_child> &&_h) noexcept;
	  _child(std::unique_ptr<conditional_inclusion_node> &&_c) noexcept;
	  _child(_child &&chld) noexcept;
	  ~_child() noexcept;

	  const inclusion_node& get_inclusion_node() const noexcept;

	  enum class kind
	  {
	    header,
	    conditional,
	  };

	  kind k;

	  union
	  {
	    std::unique_ptr<header_inclusion_child> h;
	    std::unique_ptr<conditional_inclusion_node> c;
	  };
	};

	typedef std::vector<_child> _children_container_type;

	class _recursive_child_iterator :
	  public std::iterator<std::forward_iterator_tag, const _child>
	{
	public:
	  _recursive_child_iterator();
	  _recursive_child_iterator(const inclusion_node &n);

	  bool operator==(const _recursive_child_iterator &rhs) const noexcept
	  { return this->_walk_stack == rhs._walk_stack; }

	  bool operator!=(const _recursive_child_iterator &rhs) const noexcept
	  {
	    return !(*this == rhs);
	  }

	  reference operator*() const noexcept
	  {
	    return *_walk_stack.back().first;
	  }

	  pointer operator->() const noexcept
	  {
	    return &*_walk_stack.back().first;
	  }

	  _recursive_child_iterator& operator++();

	  const _recursive_child_iterator operator++(int);

	  bool is_end() noexcept
	  { return _walk_stack.empty(); }

	private:
	  std::vector<std::pair<_children_container_type::const_iterator,
				_children_container_type::const_iterator> >
	  _walk_stack;
	};

      public:
	template<typename callable_type>
	void for_each_descendant_inclusion_node(callable_type &&c) const;

	class recursive_header_inclusion_iterator :
	  public std::iterator<std::forward_iterator_tag,
			       const header_inclusion_child>
	{
	public:
	  recursive_header_inclusion_iterator();

	  bool operator==(const recursive_header_inclusion_iterator &rhs)
	    const noexcept
	  { return this->_it == rhs._it; }

	  bool operator!=(const recursive_header_inclusion_iterator &rhs)
	    const noexcept
	  {
	    return !(*this == rhs);
	  }

	  reference operator*() const noexcept
	  {
	    return *_it->h;
	  }

	  pointer operator->() const noexcept
	  {
	    return _it->h.get();;
	  }

	  recursive_header_inclusion_iterator& operator++();

	  const recursive_header_inclusion_iterator operator++(int);

	private:
	  friend class inclusion_node;
	  recursive_header_inclusion_iterator(_recursive_child_iterator &&it);

	  void _skip_non_headers();

	  _recursive_child_iterator _it;
	};

	inclusion_node(const inclusion_node&) = delete;
	inclusion_node(inclusion_node&&) = delete;

	virtual ~inclusion_node() noexcept;

	virtual const header_inclusion_node&
	get_containing_header() const noexcept = 0;

	virtual const conditional_inclusion_node*
	get_containing_conditional_inclusion() const noexcept = 0;

	const raw_pp_tokens_range& get_range() const noexcept
	{ return _range; }

	recursive_header_inclusion_iterator
	header_inclusions_recursive_begin() const;

	recursive_header_inclusion_iterator
	header_inclusions_recursive_end() const;

	const inclusion_node *
	get_parent() const noexcept
	{ return _parent; }

      protected:
	inclusion_node();
	inclusion_node(inclusion_node * const parent,
		       const raw_pp_token_index range_begin);

	inclusion_node * const _parent;

      private:
	friend class pp_result;
	friend class preprocessor;

	header_inclusion_child&
	_add_header_inclusion(const std::string &filename,
			const raw_pp_tokens_range &directive_range,
			used_macros &&used_macros,
			macro_nondef_constraints &&macro_nondef_constraints,
			pp_result &container);

	conditional_inclusion_node&
	_add_conditional_inclusion(const raw_pp_token_index range_begin);

	void _set_range_begin(const raw_pp_token_index range_begin) noexcept;
	void _set_range_end(const raw_pp_token_index range_end) noexcept;

	_children_container_type _children;
	raw_pp_tokens_range _range;
      };

      class header_inclusion_node : public inclusion_node
      {
      public:
	virtual ~header_inclusion_node() noexcept = 0;

	virtual const header_inclusion_node&
	get_containing_header() const noexcept override;

	const std::string& get_filename() const noexcept
	{
	  return _filename;
	}

	void add_line(const std::streamoff length);

	std::pair<std::streamoff, std::streamoff>
	offset_to_line_col(const std::streamoff off) const noexcept;

	std::unique_ptr<source_reader> create_source_reader() const;

	unsigned long get_id() const noexcept;

      protected:
	header_inclusion_node(const std::string &filename);

	header_inclusion_node(inclusion_node &parent,
			      const raw_pp_token_index range_begin,
			      const std::string &filename,
			      const unsigned long id);

      private:
	friend class pp_result;

	void _set_id(const unsigned long id) noexcept;

	const std::string _filename;
	offset_to_line_col_map _offset_to_line_col_map;
	unsigned long _id;
      };


      class header_inclusion_root final : public header_inclusion_node
      {
      public:
	header_inclusion_root(const std::string &filename,
			      const bool is_preinclude);

	virtual ~header_inclusion_root() noexcept;

	virtual const conditional_inclusion_node*
	get_containing_conditional_inclusion() const noexcept override;

	bool is_preinclude() const noexcept
	{ return _is_preinclude; }

      private:
	bool _is_preinclude;
      };


      class header_inclusion_child final : public header_inclusion_node
      {
      public:
	virtual ~header_inclusion_child() noexcept;

	virtual const conditional_inclusion_node*
	get_containing_conditional_inclusion() const noexcept override;

	const raw_pp_tokens_range& get_directive_range() const noexcept
	{ return _directive_range; }

	const used_macros& get_used_macros() const noexcept
	{ return _used_macros; }

	const macro_nondef_constraints& get_macro_nondef_constraints()
	  const noexcept
	{ return _macro_nondef_constraints; }

      private:
	friend class inclusion_node;

	header_inclusion_child(inclusion_node &parent,
			const std::string &filename,
			const raw_pp_tokens_range &directive_range,
			used_macros &&used_macros,
			macro_nondef_constraints &&macro_nondef_constraints,
			const unsigned long id);

	raw_pp_tokens_range _directive_range;
	used_macros _used_macros;
	macro_nondef_constraints _macro_nondef_constraints;
      };

      class conditional_inclusion_node final : public inclusion_node
      {
      public:
	typedef std::vector<raw_pp_tokens_range> directive_ranges_type;

	virtual ~conditional_inclusion_node();

	virtual const header_inclusion_node&
	get_containing_header() const noexcept override;

	virtual const conditional_inclusion_node*
	get_containing_conditional_inclusion() const noexcept override;

	const raw_pp_token_index get_directive_begin() const noexcept
	{ return this->get_range().begin; }

	const used_macros& get_used_macros() const noexcept
	{ return _used_macros; }

	const macro_nondef_constraints& get_macro_nondef_constraints()
	  const noexcept
	{ return _macro_nondef_constraints; }

	directive_ranges_type::size_type nbranches() const noexcept;

	const raw_pp_tokens_range&
	get_branch_directive_range(const directive_ranges_type::size_type i)
	  const noexcept;

	bool has_taken_branch() const noexcept;

	directive_ranges_type::size_type get_taken_branch() const noexcept;

	bool has_else_branch() const noexcept
	{ return _has_else_branch; }

      private:
	friend class inclusion_node;
	friend class preprocessor;

	conditional_inclusion_node(inclusion_node &parent,
				   const raw_pp_token_index range_begin);

	void _finalize(const raw_pp_token_index range_end,
		       used_macros &&used_macros,
		       macro_nondef_constraints &&macro_nondef_constraints,
		       directive_ranges_type &&directive_ranges,
		       const directive_ranges_type::size_type taken_branch,
		       const bool has_else_branch);

	used_macros _used_macros;
	macro_nondef_constraints _macro_nondef_constraints;
	directive_ranges_type _directive_ranges;
	// Zero -> no branch taken, otherwise the index is 1-based.
	directive_ranges_type::size_type _taken_branch;
	bool _has_else_branch;
      };


      class header_inclusion_roots
      {
      private:
	typedef std::vector<std::unique_ptr<header_inclusion_root> >
		_container_type;

      public:
	class iterator : public std::iterator<std::random_access_iterator_tag,
					      header_inclusion_root>
	{
	public:
	  bool operator==(const iterator &rhs) const noexcept
	  { return this->_it == rhs._it; }

	  bool operator!=(const iterator &rhs) const noexcept
	  { return !(*this == rhs); }

	  reference operator*() const noexcept
	  { return **_it; }

	  pointer operator->() const noexcept
	  { return (*_it).get(); }

	  iterator& operator++() noexcept
	  { ++_it; return *this; }

	  const iterator operator++(int) noexcept
	  { return iterator{_it++}; }

	  iterator& operator--() noexcept
	  { --_it; return *this; }

	  const iterator operator--(int) noexcept
	  { return iterator{_it--}; }

	  const iterator&
	  operator+=(const difference_type n) noexcept
	  { _it += n; return *this; }

	  const iterator&
	  operator-=(const difference_type n) noexcept
	  { _it -= n; return *this; }

	  const iterator
	  operator+(const difference_type n) const noexcept
	  { return iterator(_it + n); }

	  const iterator
	  operator-(const difference_type n) const noexcept
	  { return iterator(_it - n); }

	  const difference_type
	  operator-(const iterator &op) const noexcept
	  { return this->_it - op._it; }

	  reference operator[](const difference_type n) const
	  { return *(*this + n); }

	  bool operator<(const iterator &op) const noexcept
	  { return this->_it < op._it; }

	  bool operator>(const iterator &op) const noexcept
	  { return this->_it > op._it; }

	  bool operator<=(const iterator &op) const noexcept
	  { return this->_it <= op._it; }

	  bool operator>=(const iterator &op) const noexcept
	  { return this->_it >= op._it; }

	private:
	  friend class header_inclusion_roots;

	  iterator(const _container_type::iterator &it) noexcept
	    : _it(it)
	  {}

	  _container_type::iterator _it;
	};

	class const_iterator
	  : public std::iterator<std::random_access_iterator_tag,
				 const header_inclusion_root>
	{
	public:
	  const_iterator()
	  {};

	  bool operator==(const const_iterator &rhs) const noexcept
	  { return this->_it == rhs._it; }

	  bool operator!=(const const_iterator &rhs) const noexcept
	  { return !(*this == rhs); }

	  reference operator*() const noexcept
	  { return **_it; }

	  pointer operator->() const noexcept
	  { return (*_it).get(); }

	  const_iterator& operator++() noexcept
	  { ++_it; return *this; }

	  const const_iterator operator++(int) noexcept
	  { return const_iterator{_it++}; }

	  const_iterator& operator--() noexcept
	  { --_it; return *this; }

	  const const_iterator operator--(int) noexcept
	  { return const_iterator{_it--}; }

	  const const_iterator&
	  operator+=(const difference_type n) noexcept
	  { _it += n; return *this; }

	  const const_iterator&
	  operator-=(const difference_type n) noexcept
	  { _it -= n; return *this; }

	  const const_iterator
	  operator+(const difference_type n) const noexcept
	  { return const_iterator(_it + n); }

	  const const_iterator
	  operator-(const difference_type n) const noexcept
	  { return const_iterator(_it - n); }

	  const difference_type
	  operator-(const const_iterator &op) const noexcept
	  { return this->_it - op._it; }

	  reference operator[](const difference_type n) const noexcept
	  { return *(*this + n); }

	  bool operator<(const const_iterator &op) const noexcept
	  { return this->_it < op._it; }

	  bool operator>(const const_iterator &op) const noexcept
	  { return this->_it > op._it; }

	  bool operator<=(const const_iterator &op) const noexcept
	  { return this->_it <= op._it; }

	  bool operator>=(const const_iterator &op) const noexcept
	  { return this->_it >= op._it; }

	private:
	  friend class header_inclusion_roots;

	  const_iterator(const _container_type::const_iterator &it) noexcept
	    : _it(it)
	  {}

	  _container_type::const_iterator _it;
	};

	header_inclusion_roots();

	header_inclusion_roots(header_inclusion_roots &&hirs);

	~header_inclusion_roots() noexcept;

	header_inclusion_root& add(const std::string &filename,
				   const bool is_preinclude);

	iterator begin() noexcept
	{ return iterator{_roots.begin()}; }

	iterator end() noexcept
	{ return iterator{_roots.end()}; }

	const_iterator begin() const noexcept
	{ return const_iterator{_roots.begin()}; }

	const_iterator end() const noexcept
	{ return const_iterator{_roots.end()}; }

	bool empty() const noexcept
	{ return _roots.empty(); }

      private:
	_container_type _roots;
      };

      const header_inclusion_roots& get_header_inclusion_roots() const noexcept
      { return _header_inclusion_roots; }

      header_inclusion_roots& get_header_inclusion_roots() noexcept
      { return _header_inclusion_roots; }

    private:
      template <typename node_type>
      class _const_intersecting_child_node_iterator
      {
      public:
	_const_intersecting_child_node_iterator
		(const header_inclusion_root &root,
		 const raw_pp_tokens_range &range);

	_const_intersecting_child_node_iterator
		(const raw_pp_tokens_range &range) noexcept;

	bool operator==(const _const_intersecting_child_node_iterator &rhs)
	  const noexcept;

	void advance();

	const node_type *get_value() const noexcept
	{ return _cur_val; }

      private:
	static inclusion_node::_child::kind _get_node_kind() noexcept;

	static const node_type& _access_chld(const inclusion_node::_child &c)
	  noexcept;

	std::pair<inclusion_node::_children_container_type::const_iterator,
		  inclusion_node::_children_container_type::const_iterator>
	_intersecting_chlds_subrange
	(const inclusion_node::_children_container_type::const_iterator &b,
	 const inclusion_node::_children_container_type::const_iterator &e)
	  noexcept;

	struct _walk_stack_entry
	{
	  inclusion_node::_children_container_type::const_iterator chlds_it;
	  inclusion_node::_children_container_type::const_iterator chlds_begin;
	  inclusion_node::_children_container_type::const_iterator chlds_end;
	};

	void _descend_walk_stack();
	void _fwd_walk_stack_to_node_kind();

	const node_type * _find_deepest_covering_pos() noexcept;
	const node_type * _find_node_after_pos() noexcept;

	raw_pp_tokens_range _range;
	raw_pp_token_index _pos;

	const node_type *_cur_val;

	typedef std::vector<_walk_stack_entry> _walk_stack_type;
	_walk_stack_type _walk_stack;
      };

    public:
      class const_intersecting_source_iterator :
	public std::iterator<std::forward_iterator_tag,
			     const header_inclusion_node>
      {
      public:
	struct init_begin_iterator_tag {};
	struct init_end_iterator_tag {};

	const_intersecting_source_iterator(const header_inclusion_roots &roots,
					   const raw_pp_tokens_range &range,
					   const init_begin_iterator_tag&);

	const_intersecting_source_iterator(const header_inclusion_roots &roots,
					   const raw_pp_tokens_range &range,
					   const init_end_iterator_tag&)
	  noexcept;

	bool operator==(const const_intersecting_source_iterator &rhs)
	  const noexcept;

	bool operator!=(const const_intersecting_source_iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept;

	pointer operator->() const noexcept
	{ return &**this; }

	const_intersecting_source_iterator&
	operator++();

	const const_intersecting_source_iterator operator++(int);

      private:
	std::pair<header_inclusion_roots::const_iterator,
		  header_inclusion_roots::const_iterator>
	_intersecting_roots_subrange
		(const header_inclusion_roots::const_iterator &b,
		 const header_inclusion_roots::const_iterator &e) noexcept;

	void _next_root();

	raw_pp_tokens_range _range;

	std::pair<header_inclusion_roots::const_iterator,
		  header_inclusion_roots::const_iterator> _roots;
	header_inclusion_roots::const_iterator _roots_it;

	_const_intersecting_child_node_iterator<header_inclusion_child>
		_child_it;
	raw_pp_token_index _max_pos_seen_from_chlds;
      };

      const_intersecting_source_iterator
      intersecting_sources_begin(const raw_pp_tokens_range &range) const;

      const_intersecting_source_iterator
      intersecting_sources_end(const raw_pp_tokens_range &range) const noexcept;


      class const_intersecting_conditional_iterator :
	public std::iterator<std::forward_iterator_tag,
			     const conditional_inclusion_node>
      {
      public:
	struct init_begin_iterator_tag {};
	struct init_end_iterator_tag {};

	const_intersecting_conditional_iterator
					(const header_inclusion_roots &roots,
					 const raw_pp_tokens_range &range,
					 const init_begin_iterator_tag&);

	const_intersecting_conditional_iterator
					(const header_inclusion_roots &roots,
					 const raw_pp_tokens_range &range,
					 const init_end_iterator_tag&)
	  noexcept;

	bool operator==(const const_intersecting_conditional_iterator &rhs)
	  const noexcept;

	bool operator!=(const const_intersecting_conditional_iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept;

	pointer operator->() const noexcept
	{ return &**this; }

	const_intersecting_conditional_iterator&
	operator++();

	const const_intersecting_conditional_iterator operator++(int);

      private:
	std::pair<header_inclusion_roots::const_iterator,
		  header_inclusion_roots::const_iterator>
	_intersecting_roots_subrange
		(const header_inclusion_roots::const_iterator &b,
		 const header_inclusion_roots::const_iterator &e) noexcept;

	void _enter_root();

	raw_pp_tokens_range _range;

	std::pair<header_inclusion_roots::const_iterator,
		  header_inclusion_roots::const_iterator> _roots;
	header_inclusion_roots::const_iterator _roots_it;

	_const_intersecting_child_node_iterator<conditional_inclusion_node>
		_child_it;
      };

      const_intersecting_conditional_iterator
      intersecting_conditionals_begin(const raw_pp_tokens_range &range) const;

      const_intersecting_conditional_iterator
      intersecting_conditionals_end(const raw_pp_tokens_range &range)
	const noexcept;


      const header_inclusion_node&
      get_raw_token_source(const raw_pp_token_index tok_index) const;

      const raw_pp_tokens& get_raw_tokens() const noexcept
      { return _raw_tokens; }

      const pp_tokens &get_pp_tokens() const noexcept
      { return _pp_tokens; }

      pp_tokens &get_pp_tokens() noexcept
      { return _pp_tokens; }

      unsigned long get_header_ids_end() const noexcept
      { return _next_header_node_id; }

      const raw_pp_tokens_range
      pp_tokens_range_to_raw(const pp_tokens_range &r) const noexcept;

      const raw_pp_tokens_range
      pp_token_index_to_raw(const pp_token_index tok_index) const noexcept;

      const pp_tokens_range
      raw_pp_tokens_range_to_nonraw(const raw_pp_tokens_range &r)
	const noexcept;

    private:
      friend class preprocessor;

      pp_result();
      pp_result(header_inclusion_roots &&header_inclusion_roots);

      void _enter_root(header_inclusion_root &hir,
		       const raw_pp_token_index range_begin) noexcept;

      void _append_token(const raw_pp_token &tok);
      void _append_token(raw_pp_token &&tok);
      void _append_token(pp_token &&tok);

      raw_pp_tokens::size_type _get_last_raw_index() const noexcept;
      pp_tokens::size_type _get_last_pp_index() const noexcept;

      void _add_directive(const raw_pp_tokens_range &directive_range);

      const macro& _add_macro(const std::string &name,
			      const bool func_like,
			      const bool variadic,
			      std::vector<std::string> &&arg_names,
			      raw_pp_tokens &&repl,
			      const raw_pp_tokens_range &repl_range,
			      const raw_pp_tokens_range &directive_range);

      const macro& _add_macro(const std::string &name,
			      const macro::builtin_special_tag &tag);

      const macro& _add_macro(const std::string &name,
			      std::vector<std::string> &&arg_names,
			      const macro::builtin_special_tag &tag);

      const macro& _add_macro(const std::string &name,
			      const bool func_like,
			      const bool variadic,
			      std::vector<std::string> &&arg_names,
			      raw_pp_tokens &&repl,
			      const macro::builtin_tag &tag);

      const macro& _add_macro(const std::string &name,
			      const bool func_like,
			      const bool variadic,
			      std::vector<std::string> &&arg_names,
			      raw_pp_tokens &&repl,
			      const macro::predefined_user_tag &tag);

      void _add_macro_undef(const std::string &name,
			    const raw_pp_tokens_range &directive_range);

      void _add_macro_undef(const std::string &name,
			    const macro_undef::predefined_user_tag &tag);

      macro_invocation&
      _add_macro_invocation
	(const macro &m,
	 macro_invocation::expansion_forest::invocation_source
	   &&initial_invocation_source);

      std::pair<used_macros, macro_nondef_constraints>
      _drop_pp_tokens_tail(const pp_tokens::size_type new_end);

      header_inclusion_roots _header_inclusion_roots;
      raw_pp_tokens _raw_tokens;
      pp_tokens _pp_tokens;
      _pp_tokens_origins _pp_tokens_origins;

      _directives_container_type _directives;
      _macros_container_type _macros;
      _macro_undefs_container_type _macro_undefs;
      _macro_invocations_container_type _macro_invocations;

      unsigned long _next_header_node_id;
      unsigned long _next_predefinition_pos;
    };

    template<typename callable_type>
    void pp_result::inclusion_node::
    for_each_descendant_inclusion_node(callable_type &&c) const
    {
      _recursive_child_iterator it{*this};
      while(!it.is_end()) {
	switch (it->k) {
	case _child::kind::header:
	  c(const_cast<const header_inclusion_child&>(*it->h));
	  break;

	case _child::kind::conditional:
	  c(const_cast<const conditional_inclusion_node&>(*it->c));
	  break;
	}
	++it;
      }
    }

    static inline bool operator<(const raw_pp_tokens_range &r,
				 const pp_result::directive &d) noexcept
    { return r < d.get_source_range(); }

    static inline bool operator<(const raw_pp_tokens_range &r,
				 const pp_result::macro &m) noexcept
    {
      if (!m.is_predefined())
       return r < m.get_directive_range();
      else
	return false;
    }

    static inline bool operator<(const raw_pp_tokens_range &r,
				 const pp_result::macro_undef &mu) noexcept
    {
      if (!mu.is_predefined())
	return r < mu.get_directive_range();
      else
	return false;
    }

    static inline bool operator<(const raw_pp_tokens_range &r,
				 const pp_result::macro_invocation &mi) noexcept
    { return r < mi.get_source_range(); }

    static inline const pp_result::const_macro_invocation_iterator
    operator+(const pp_result::const_macro_invocation_iterator
			::difference_type n,
	      const pp_result::const_macro_invocation_iterator &it)
    { return it + n; }
  }
}

#endif
