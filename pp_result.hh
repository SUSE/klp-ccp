#ifndef PP_RESULT_HH
#define PP_RESULT_HH

#include <vector>
#include <memory>
#include <utility>
#include "raw_pp_tokens.hh"
#include "raw_pp_tokens_range.hh"
#include "pp_tokens.hh"
#include "used_macros.hh"
#include "used_macro_undefs.hh"
#include "offset_to_line_col_map.hh"

namespace klp
{
  namespace ccp
  {
    class macro_undef;
    class source_reader;

    class pp_result
    {
    public:
      class macro_invocation
      {
      public:
	const raw_pp_tokens_range& get_source_range() const noexcept
	{ return _invocation_range; }

      private:
	friend class pp_result;
	friend class preprocessor;

	macro_invocation(const macro &m,
			 const raw_pp_tokens_range &invocation_range);

	raw_pp_tokens_range _invocation_range;
	used_macros _used_macros;
	used_macro_undefs _used_macro_undefs;
      };


      class header_inclusion_node;
      class header_inclusion_child;
      class conditional_inclusion_node;

      class inclusion_node
      {
      private:
	struct _child
	{
	  _child(std::unique_ptr<header_inclusion_child> &&_h) noexcept;
	  _child(std::unique_ptr<conditional_inclusion_node> &&_c) noexcept;
	  _child(_child &&chld) noexcept;
	  ~_child() noexcept;

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

      protected:
	inclusion_node();
	inclusion_node(const inclusion_node * const parent,
		       const raw_pp_token_index range_begin);

	recursive_header_inclusion_iterator
	header_inclusions_recursive_begin() const;

	recursive_header_inclusion_iterator
	header_inclusions_recursive_end() const;

	const inclusion_node * const _parent;

      private:
	friend class preprocessor;

	header_inclusion_child&
	_add_header_inclusion(const std::string &filename,
			      const raw_pp_tokens_range &directive_range,
			      used_macros &&used_macros,
			      used_macro_undefs &&used_macro_undefs);

	conditional_inclusion_node&
	_add_conditional_inclusion(const raw_pp_token_index range_begin,
				   used_macros &&used_macros,
				   used_macro_undefs &&used_macro_undefs);

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

      protected:
	header_inclusion_node(const std::string &filename);

	header_inclusion_node(const inclusion_node &parent,
			      const raw_pp_token_index range_begin,
			      const std::string &filename);

      private:
	const std::string _filename;

	offset_to_line_col_map _offset_to_line_col_map;
      };


      class header_inclusion_root final : public header_inclusion_node
      {
      public:
	header_inclusion_root(const std::string &filename,
			      const bool is_preinclude);
	virtual ~header_inclusion_root() noexcept;

      private:
	bool _is_preinclude;
      };


      class header_inclusion_child final : public header_inclusion_node
      {
      public:
	virtual ~header_inclusion_child() noexcept;

      private:
	friend class inclusion_node;

	header_inclusion_child(const inclusion_node &parent,
			       const std::string &filename,
			       const raw_pp_tokens_range &directive_range,
			       used_macros &&used_macros,
			       used_macro_undefs &&used_macro_undefs);

	raw_pp_tokens_range _directive_range;
	used_macros _used_macros;
	used_macro_undefs _used_macro_undefs;
      };

      class conditional_inclusion_node final : public inclusion_node
      {
      public:
	virtual ~conditional_inclusion_node();

	virtual const header_inclusion_node&
	get_containing_header() const noexcept override;

      private:
	friend class inclusion_node;
	friend class preprocessor;

	conditional_inclusion_node(const inclusion_node &parent,
				   const raw_pp_token_index range_begin,
				   used_macros &&used_macros,
				   used_macro_undefs &&used_macro_undefs);

	used_macros _used_macros;
	used_macro_undefs _used_macro_undefs;
      };

      class header_inclusion_roots
      {
      private:
	typedef std::vector<std::unique_ptr<header_inclusion_root> >
		_container_type;

      public:
	class iterator : public std::iterator<std::forward_iterator_tag,
					      header_inclusion_root>
	{
	public:
	  bool operator==(const iterator &rhs) const noexcept
	  { return this->_it == rhs._it; }

	  bool operator!=(const iterator &rhs)
	  { return !(*this == rhs); }

	  reference operator*() const noexcept
	  { return **_it; }

	  pointer operator->() const noexcept
	  { return (*_it).get(); }

	  iterator& operator++()
	  { ++_it; return *this; }

	  const iterator operator++(int)
	  { return iterator{_it++}; }

	private:
	  friend class header_inclusion_roots;

	  iterator(const _container_type::iterator &it) noexcept
	    : _it(it)
	  {}

	  _container_type::iterator _it;
	};

	class const_iterator : public std::iterator<std::forward_iterator_tag,
						    const header_inclusion_root>
	{
	public:
	  bool operator==(const const_iterator &rhs) const noexcept
	  { return this->_it == rhs._it; }

	  bool operator!=(const const_iterator &rhs)
	  { return !(*this == rhs); }

	  reference operator*() const noexcept
	  { return **_it; }

	  pointer operator->() const noexcept
	  { return (*_it).get(); }

	  const_iterator& operator++()
	  { ++_it; return *this; }

	  const const_iterator operator++(int)
	  { return const_iterator{_it++}; }

	private:
	  friend class header_inclusion_roots;

	  const_iterator(const _container_type::const_iterator &it) noexcept
	    : _it(it)
	  {}

	  _container_type::const_iterator _it;
	};

	header_inclusion_roots();

	header_inclusion_roots(_container_type &&roots);

	header_inclusion_roots(header_inclusion_roots &&hirs);

	~header_inclusion_roots() noexcept;

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

      const raw_pp_tokens& get_raw_tokens() const noexcept
      { return _raw_tokens; }

      const pp_tokens &get_pp_tokens() const noexcept
      { return _pp_tokens; }

      pp_tokens &get_pp_tokens() noexcept
      { return _pp_tokens; }


    private:
      friend class preprocessor;

      pp_result();
      pp_result(header_inclusion_roots &&header_inclusion_roots);

      void _append_token(const raw_pp_token &tok);
      void _append_token(raw_pp_token &&tok);
      void _append_token(pp_token &&tok);

      raw_pp_tokens::size_type _get_last_raw_index() const noexcept;
      pp_tokens::size_type _get_last_pp_index() const noexcept;

      macro_invocation&
      _add_macro_invocation(const macro &m,
			    const raw_pp_tokens_range &invocation_range);

      void _extend_macro_invocation_range(macro_invocation &invocation,
					  const raw_pp_token_index new_end);

      const macro& _add_macro(const std::string &name,
			      const bool func_like,
			      const bool variadic,
			      std::vector<std::string> &&arg_names,
			      raw_pp_tokens &&repl,
			      const raw_pp_tokens_range &directive_range,
			      const macro_undef *prev_macro_undef);

      const macro_undef&
      _add_macro_undef(const macro &m,
		       const raw_pp_tokens_range &directive_range);

      header_inclusion_roots _header_inclusion_roots;
      raw_pp_tokens _raw_tokens;
      pp_tokens _pp_tokens;

      std::vector<std::unique_ptr<macro_invocation>> _macro_invocations;
      std::vector<std::unique_ptr<const macro>> _macros;
      std::vector<std::unique_ptr<const macro_undef>> _macro_undefs;
    };
  }
}

#endif