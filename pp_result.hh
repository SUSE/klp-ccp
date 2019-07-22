#ifndef PP_RESULT_HH
#define PP_RESULT_HH

#include <vector>
#include <memory>
#include <utility>
#include <set>
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
	{ return _directive_range < r; }

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

      private:
	friend class pp_result;

	macro(const std::string &name,
	      const bool func_like,
	      const bool variadic,
	      std::vector<std::string> &&arg_names,
	      raw_pp_tokens &&repl,
	      const raw_pp_tokens_range &directive_range);

	macro(const std::string &name,
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

	raw_pp_tokens::const_iterator
	_next_non_ws_repl(const raw_pp_tokens::const_iterator it)
	  const noexcept;

	raw_pp_tokens::const_iterator
	_skip_stringification_or_single(const raw_pp_tokens::const_iterator &it)
	  const noexcept;

	bool _is_concat_op(const raw_pp_tokens::const_iterator &it)
	  const noexcept;

	std::string _name;
	std::vector<std::string> _arg_names;
	raw_pp_tokens _repl;
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
	{ return _directive_range < r; }

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

      private:
	_macro_nondef_constraints_type _macro_nondef_constraints;
      };

      class macro_invocation
      {
      public:
	const raw_pp_tokens_range& get_source_range() const noexcept
	{ return _invocation_range; }

	bool operator<(const raw_pp_tokens_range &r) const noexcept
	{ return _invocation_range < r; }

	const used_macros& get_used_macros() const noexcept
	{ return _used_macros; }

	const macro_nondef_constraints& get_macro_nondef_constraints()
	  const noexcept
	{ return _macro_nondef_constraints; }

      private:
	friend class pp_result;
	friend class preprocessor;

	macro_invocation(const macro &m,
			 const raw_pp_tokens_range &invocation_range);

	raw_pp_tokens_range _invocation_range;
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
	_add_conditional_inclusion(const raw_pp_token_index range_begin,
			used_macros &&used_macros,
			macro_nondef_constraints &&macro_nondef_constraints);

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

	bool is_preinclude() const noexcept
	{ return _is_preinclude; }

      private:
	bool _is_preinclude;
      };


      class header_inclusion_child final : public header_inclusion_node
      {
      public:
	virtual ~header_inclusion_child() noexcept;

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
	virtual ~conditional_inclusion_node();

	virtual const header_inclusion_node&
	get_containing_header() const noexcept override;

	const raw_pp_token_index get_directive_begin() const noexcept
	{ return this->get_range().begin; }

	const used_macros& get_used_macros() const noexcept
	{ return _used_macros; }

	const macro_nondef_constraints& get_macro_nondef_constraints()
	  const noexcept
	{ return _macro_nondef_constraints; }

      private:
	friend class inclusion_node;
	friend class preprocessor;

	conditional_inclusion_node(inclusion_node &parent,
			const raw_pp_token_index range_begin,
			used_macros &&used_macros,
			macro_nondef_constraints &&macro_nondef_constraints);

	used_macros _used_macros;
	macro_nondef_constraints _macro_nondef_constraints;
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
					   const init_end_iterator_tag&);

	bool operator==(const const_intersecting_source_iterator &rhs)
	  const noexcept;

	bool operator!=(const const_intersecting_source_iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept
	{ return *_cur_val; }

	pointer operator->() const noexcept
	{ return _cur_val; }

	const_intersecting_source_iterator&
	operator++();

	const const_intersecting_source_iterator operator++(int);

      private:
	std::pair<header_inclusion_roots::const_iterator,
		  header_inclusion_roots::const_iterator>
	_intersecting_roots_subrange
		(const header_inclusion_roots::const_iterator &b,
		 const header_inclusion_roots::const_iterator &e) noexcept;

	std::pair<inclusion_node::_children_container_type::const_iterator,
		  inclusion_node::_children_container_type::const_iterator>
	_intersecting_chlds_subrange
	(const inclusion_node::_children_container_type::const_iterator &b,
	 const inclusion_node::_children_container_type::const_iterator &e)
	  noexcept;

	void _enter_root();

	void _fwd_walk_to_header_dfs_po();

	struct _walk_stack_entry
	{
	  inclusion_node::_children_container_type::const_iterator chlds_it;
	  inclusion_node::_children_container_type::const_iterator chlds_begin;
	  inclusion_node::_children_container_type::const_iterator chlds_end;
	};

	raw_pp_tokens_range _range;
	raw_pp_token_index _pos;

	header_inclusion_roots::const_iterator _roots_begin;
	header_inclusion_roots::const_iterator _roots_end;
	header_inclusion_roots::const_iterator _roots_it;

	const header_inclusion_node *_cur_val;

	typedef std::vector<_walk_stack_entry> _walk_stack_type;
	_walk_stack_type _walk_stack;
      };

      const_intersecting_source_iterator
      intersecting_sources_begin(const raw_pp_tokens_range &range) const;

      const_intersecting_source_iterator
      intersecting_sources_end(const raw_pp_tokens_range &range) const;

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
			      const raw_pp_tokens_range &directive_range);

      const macro& _add_macro(const std::string &name,
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
      _add_macro_invocation(const macro &m,
			    const raw_pp_tokens_range &invocation_range);

      void _extend_macro_invocation_range(macro_invocation &invocation,
					  const raw_pp_token_index new_end);

      std::pair<used_macros, macro_nondef_constraints>
      _drop_pp_tokens_tail(const pp_tokens::size_type new_end);

      header_inclusion_roots _header_inclusion_roots;
      raw_pp_tokens _raw_tokens;
      pp_tokens _pp_tokens;

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
    { return r < m.get_directive_range(); }

    static inline bool operator<(const raw_pp_tokens_range &r,
				 const pp_result::macro_undef &mu) noexcept
    { return r < mu.get_directive_range(); }

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
