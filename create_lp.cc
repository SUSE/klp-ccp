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

#include <set>
#include <map>
#include <memory>
#include <stack>
#include <vector>
#include <functional>
#include <algorithm>
#include <queue>
#include "pp_token.hh"
#include "ast.hh"
#include "ast_impl.hh"
#include "code_remarks.hh"
#include "types_impl.hh"
#include "lp_creation_policy.hh"
#include "lp_except.hh"
#include "source_writer.hh"
#include "depreprocessor.hh"
#include "create_lp.hh"

using namespace klp::ccp;

namespace
{
  class sou_def_info;
  class sou_ref_info;
  class sou_info;

  class dep_on_sou_decl
  {
  public:
    enum class kind
    {
      def,
      ref,
    };

    dep_on_sou_decl(sou_def_info &soud) noexcept;
    dep_on_sou_decl(sou_ref_info &sour) noexcept;

    bool operator<(const dep_on_sou_decl &d) const noexcept;

    const ast::sou_decl_list_node& get_decl_list_node() const noexcept;

    sou_info& get_sou_info() const noexcept;

    const kind k;

    sou_def_info& get_soud() const noexcept;
    sou_ref_info& get_sour() const noexcept;

  private:
    union
    {
      sou_def_info * const _soud;
      sou_ref_info * const _sour;
    };
  };

  class deps_on_sou_decls
  {
  private:
    typedef std::set<dep_on_sou_decl> _container_type;

  public:
    deps_on_sou_decls& operator+=(const deps_on_sou_decls &rhs);

    bool add(const dep_on_sou_decl &d);

    _container_type::const_iterator begin() const noexcept
    { return _deps.begin(); }

    _container_type::const_iterator end() const noexcept
    { return _deps.end(); }

    void clear()
    { _deps.clear(); }

  private:
    _container_type _deps;
  };

  struct dep_on_sou_def
  {
    dep_on_sou_def(sou_def_info &_soud) noexcept;

    bool operator<(const dep_on_sou_def &d) const noexcept;

    sou_def_info &soud;
  };

  class deps_on_sou_defs
  {
  private:
    typedef std::set<dep_on_sou_def> _container_type;

  public:
    void add(const dep_on_sou_def &d);

    _container_type::const_iterator begin() const noexcept
    { return _deps.begin(); }

    _container_type::const_iterator end() const noexcept
    { return _deps.end(); }

  private:
    _container_type _deps;
  };

  class enum_def_info;
  class enum_ref_info;
  class enum_info;

  class dep_on_enum_decl
  {
  public:
    enum class kind
    {
      def,
      ref,
    };

    dep_on_enum_decl(enum_def_info &ed) noexcept;
    dep_on_enum_decl(enum_ref_info &er) noexcept;

    bool operator<(const dep_on_enum_decl &d) const noexcept;

    const ast::enum_decl_list_node& get_decl_list_node() const noexcept;

    enum_info& get_enum_info() const noexcept;

    const kind k;

    enum_def_info& get_ed() const noexcept;
    enum_ref_info& get_er() const noexcept;

  private:
    union
    {
      enum_def_info * const _ed;
      enum_ref_info * const _er;
    };
  };

  class deps_on_enum_decls
  {
  private:
    typedef std::set<dep_on_enum_decl> _container_type;

  public:
    deps_on_enum_decls& operator+=(const deps_on_enum_decls &rhs);

    bool add(const dep_on_enum_decl &d);

    _container_type::const_iterator begin() const noexcept
    { return _deps.begin(); }

    _container_type::const_iterator end() const noexcept
    { return _deps.end(); }

    void clear()
    { _deps.clear(); }

  private:
    std::set<dep_on_enum_decl> _deps;
  };

  struct dep_on_enum_def
  {
    dep_on_enum_def(enum_def_info &_ed) noexcept;

    bool operator<(const dep_on_enum_def &d) const noexcept;

    enum_def_info &ed;
  };

  class deps_on_enum_defs
  {
  private:
    typedef std::set<dep_on_enum_def> _container_type;

  public:
    void add(const dep_on_enum_def &d);

    _container_type::const_iterator begin() const noexcept
    { return _deps.begin(); }

    _container_type::const_iterator end() const noexcept
    { return _deps.end(); }

  private:
    _container_type _deps;
  };

  // Dependency on a struct/union/enum tag not being defined,
  // i.e. a "reverse" dependency.
  struct dep_on_tag_non_decl
  {
    dep_on_tag_non_decl(const std::string &_id) noexcept;

    bool operator<(const dep_on_tag_non_decl &d) const noexcept;

    const std::string id;
  };

  class deps_on_tag_non_decls
  {
  public:
    deps_on_tag_non_decls& operator+=(const deps_on_tag_non_decls &rhs);

    void add(const dep_on_tag_non_decl &d);

    bool empty() const noexcept;

    bool contains(const std::string &id) const;

  private:
    std::set<dep_on_tag_non_decl> _deps;
  };

  class typedef_init_declarator_info;

  struct dep_on_typedef
  {
    dep_on_typedef(typedef_init_declarator_info &_tdidi) noexcept;

    bool operator<(const dep_on_typedef &d) const noexcept;

    typedef_init_declarator_info &tdidi;
  };

  class deps_on_typedefs
  {
  private:
    typedef std::set<dep_on_typedef> _container_type;

  public:
    deps_on_typedefs& operator+=(const deps_on_typedefs &rhs);

    void add(const dep_on_typedef &d);

    _container_type::const_iterator begin() const noexcept
    { return _deps.begin(); }

    _container_type::const_iterator end() const noexcept
    { return _deps.end(); }

  private:
    _container_type _deps;
  };

  class function_definition_info;
  class function_init_declarator_info;
  class object_init_declarator_info;

  class dep_on_decl_type
  {
  public:
    enum class kind
    {
      k_function_definition,
      k_function_init_declarator,
      k_object_init_declarator,
    };

    dep_on_decl_type(function_definition_info &fdi,
		     const bool _need_composite,
		     const ast::expr_id &_from_eid) noexcept;

    dep_on_decl_type(function_init_declarator_info &fidi,
		     const bool _need_composite,
		     const ast::expr_id &_from_eid) noexcept;

    dep_on_decl_type(object_init_declarator_info &oidi,
		     const bool _need_composite,
		     const ast::expr_id &_from_eid) noexcept;

    kind get_kind() const noexcept
    { return _k; }

    const lp_creation_policy::externalized_symbol_modification*
    get_target_externalized_sym_mod() const noexcept;

    const function_definition_info& get_fdi() const noexcept;
    const function_init_declarator_info& get_fidi() const noexcept;
    const object_init_declarator_info& get_oidi() const noexcept;

    template<typename callable_type>
    void for_each_visible(callable_type &&c) const;

    const ast::expr_id &from_eid;
    const bool need_composite;

  private:
    kind _k;
    union
    {
      function_definition_info * const _fdi;
      function_init_declarator_info * const _fidi;
      object_init_declarator_info * const _oidi;
    };
  };


  class ast_info;

  class deps_on_decl_types
  {
  private:
    typedef std::vector<dep_on_decl_type> _container_type;

  public:
    void add(const dep_on_decl_type &d);

    _container_type::const_iterator begin() const noexcept
    { return _deps.begin(); }

    _container_type::const_iterator end() const noexcept
    { return _deps.end(); }

  private:
    _container_type _deps;
  };


  class object_info;

  struct dep_on_obj
  {
    dep_on_obj(object_init_declarator_info &_oidi,
	       const ast::expr_id &_from_eid) noexcept;

    dep_on_obj(object_init_declarator_info &_oidi) noexcept;

    object_info& get_object_info() noexcept;
    const object_info& get_object_info() const noexcept;

    std::reference_wrapper<object_init_declarator_info> oidi;
    const ast::expr_id * const from_eid;

    bool externalize;
  };

  class deps_on_objs
  {
  private:
    typedef std::vector<dep_on_obj> _container_type;

  public:
    typedef _container_type::iterator iterator;
    typedef _container_type::const_iterator const_iterator;

    void add(const dep_on_obj &d);

    iterator begin() noexcept
    { return _deps.begin(); }

    iterator end() noexcept
    { return _deps.end(); }

    const_iterator begin() const noexcept
    { return _deps.begin(); }

    const_iterator end() const noexcept
    { return _deps.end(); }

  private:
    _container_type _deps;
  };


  class function_info;

  struct dep_on_fun
  {
    dep_on_fun(function_definition_info &_fdi,
	       const ast::expr_id &_from_eid) noexcept;
    dep_on_fun(function_init_declarator_info &_fidi,
	       const ast::expr_id &_from_eid) noexcept;

    function_info &get_function_info() noexcept;
    const function_info &get_function_info() const noexcept;

    const ast::direct_declarator_id get_direct_declarator_id() const noexcept;

    function_definition_info * const fdi;
    function_init_declarator_info * const fidi;

    const ast::expr_id &from_eid;

    bool externalize;
  };

  struct deps_on_funs
  {
  private:
    typedef std::vector<dep_on_fun> _container_type;

  public:
    typedef _container_type::iterator iterator;
    typedef _container_type::const_iterator const_iterator;

    void add(const dep_on_fun &d);

    iterator begin() noexcept
    { return _deps.begin(); }

    iterator end() noexcept
    { return _deps.end(); }

    const_iterator begin() const noexcept
    { return _deps.begin(); }

    const_iterator end() const noexcept
    { return _deps.end(); }

  private:
    std::vector<dep_on_fun> _deps;
  };
}

namespace
{
  struct deps_on_types
  {
    deps_on_sou_decls on_sou_decls;
    deps_on_sou_defs on_sou_defs;
    deps_on_enum_decls on_enum_decls;
    deps_on_enum_defs on_enum_defs;
    deps_on_tag_non_decls on_tag_non_decls;
    deps_on_typedefs on_typedefs;
    deps_on_decl_types on_decl_types;
  };

  struct deps_on
  {
    deps_on_types on_types;
    deps_on_objs on_objs;
    deps_on_funs on_funs;
  };
}

static const ast::storage_class_specifier *
_find_scs(const ast::declaration_specifiers &ds) noexcept;

namespace
{
  class declaration_info;
  class static_assert_info;
  class initializer_info;

  class tag_parent
  {
  public:
    enum class kind
    {
      k_declaration,
      k_static_assert,
      k_function_definition,
      k_function_init_declarator,
      k_object_init_declarator,
      k_object_initializer,
      k_typedef_init_declarator,
      k_sou_def,
      k_enum_def,
    };

    const kind k;

    template<typename callable_type>
    bool on_outermost_tag_def(callable_type &&c);

  private:
    friend class ast_info;

    tag_parent(declaration_info &p) noexcept;
    tag_parent(static_assert_info &p) noexcept;
    tag_parent(function_definition_info &p) noexcept;
    tag_parent(function_init_declarator_info &p) noexcept;
    tag_parent(object_init_declarator_info &p) noexcept;
    tag_parent(initializer_info &p) noexcept;
    tag_parent(typedef_init_declarator_info &p) noexcept;
    tag_parent(sou_def_info &p) noexcept;
    tag_parent(enum_def_info &p) noexcept;

    union
    {
      declaration_info * const _parent_decl;
      static_assert_info * const _parent_sa;
      function_definition_info * const _parent_fd;
      function_init_declarator_info * const _parent_fid;
      object_init_declarator_info * const _parent_oid;
      initializer_info * const _parent_ii;
      typedef_init_declarator_info * const _parent_tid;
      sou_def_info * const _parent_soud;
      enum_def_info * const _parent_ed;
    };
  };

  class tags_infos
  {
  public:
    template<typename callable_type>
    void for_each_child_tag(callable_type &&c);

    template<typename callable_type>
    void for_each_child_tag(callable_type &&c) const;

    template<typename callable_type>
    void for_each_tag_recursive(callable_type &&c);

    template<typename callable_type>
    void for_each_tag_recursive(callable_type &&c) const;

    template<typename callable_type>
    void for_each_deps(callable_type &&c) const;

    bool empty() const noexcept;

  private:
    friend class ast_info;

    typedef std::vector<std::reference_wrapper<sou_def_info>> _souds_type;
    typedef std::vector<std::reference_wrapper<sou_ref_info>> _sours_type;
    typedef std::vector<std::reference_wrapper<enum_def_info>> _eds_type;
    typedef std::vector<std::reference_wrapper<enum_ref_info>> _ers_type;


    struct _walk_stack_entry
    {
      _walk_stack_entry(const tags_infos &_ti,
			const _souds_type::const_iterator &_it_soud,
			const _eds_type::const_iterator &_it_ed);

      const tags_infos &ti;
      _souds_type::const_iterator it_soud;
      _eds_type::const_iterator it_ed;
    };

    _souds_type _souds;
    _sours_type _sours;
    _eds_type _eds;
    _ers_type _ers;
  };

  class sou_info;

  class sou_def_info
  {
  public:
    const ast::struct_or_union_def &soud;
    tag_parent parent;
    sou_info &soui;

    deps_on_types deps;
    tags_infos child_tags;

    bool emit_tag_decl;
    bool emit_def;

    template<typename callable_type>
    void on_outermost_tag_def(callable_type &&c);

  private:
    friend class ast_info;

    sou_def_info(const ast::struct_or_union_def &_soud,
		 const tag_parent &p,
		 sou_info &_soui) noexcept;
  };

  struct sou_ref_info
  {
  public:
    const ast::struct_or_union_ref &sour;
    tag_parent parent;
    sou_info &soui;

    bool emit_tag_decl;

  private:
    friend class ast_info;

    sou_ref_info(const ast::struct_or_union_ref &_sour,
		 const tag_parent &p,
		 sou_info &_soui) noexcept;
  };

  class sou_info
  {
  private:
    typedef std::vector<std::unique_ptr<sou_ref_info>> _sours_type;

  public:
    class iterator
      : public std::iterator<std::forward_iterator_tag, sou_ref_info>
    {
    public:
	bool operator==(const iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept
	{ return **_it; }

	pointer operator->() const noexcept
	{ return (*_it).get(); }

	iterator& operator++() noexcept
	{ ++_it; return *this; }

	const iterator operator++(int) noexcept
	{ return iterator{_it++}; }

      private:
	friend class sou_info;

	iterator(const _sours_type::const_iterator &it) noexcept
	  : _it(it)
	{}

	_sours_type::const_iterator _it;
    };

    iterator sours_begin() noexcept
    { return iterator{_sours.begin()}; }

    iterator sours_end() noexcept
    { return iterator{_sours.end()}; }

    sou_def_info * get_soud() noexcept
    { return _soud.get(); }

  private:
    friend class ast_info;

    std::unique_ptr<sou_def_info> _soud;
    _sours_type _sours;
  };


  class enum_info;

  class enum_def_info
  {
  public:
    const ast::enum_def &ed;
    tag_parent parent;
    enum_info &ei;

    deps_on_types deps;
    tags_infos child_tags;

    bool emit_tag_decl;
    bool emit_def;

    template<typename callable_type>
    void on_outermost_tag_def(callable_type &&c);

  private:
    friend class ast_info;

    enum_def_info(const ast::enum_def &_ed,
		  const tag_parent &p,
		  enum_info &_ei) noexcept;
  };

  struct enum_ref_info
  {
  public:
    const ast::enum_ref &er;
    tag_parent parent;
    enum_info &ei;

    bool emit_tag_decl;

  private:
    friend class ast_info;

    enum_ref_info(const ast::enum_ref &_er,
		  const tag_parent &p,
		  enum_info &_ei) noexcept;
  };

  class enum_info
  {
  private:
    typedef std::vector<std::unique_ptr<enum_ref_info>> _ers_type;

  public:
    class iterator
      : public std::iterator<std::forward_iterator_tag, enum_ref_info>
    {
    public:
	bool operator==(const iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept
	{ return **_it; }

	pointer operator->() const noexcept
	{ return (*_it).get(); }

	iterator& operator++() noexcept
	{ ++_it; return *this; }

	const iterator operator++(int) noexcept
	{ return iterator{_it++}; }

      private:
	friend class enum_info;

	iterator(const _ers_type::const_iterator &it) noexcept
	  : _it(it)
	{}

	_ers_type::const_iterator _it;
    };

    iterator ers_begin() noexcept
    { return iterator{_ers.begin()}; }

    iterator ers_end() noexcept
    { return iterator{_ers.end()}; }

    enum_def_info * get_ed() noexcept
    { return _ed.get(); }

  private:
    friend class ast_info;

    std::unique_ptr<enum_def_info> _ed;
    _ers_type _ers;
  };


  class init_declarator_info_list_head;

  class init_declarator_info_list_node
  {
  private:
    enum class next_kind
    {
      nk_null,
      nk_fidi,
      nk_oidi,
      nk_tidi,
    };


  public:
    init_declarator_info_list_node() noexcept
      : _k(next_kind::nk_null)
    {}

  private:
    friend class init_declarator_info_list_head;

    next_kind _k;
    union
    {
      function_init_declarator_info *_fidi;
      object_init_declarator_info *_oidi;
      typedef_init_declarator_info *_tidi;
    };
  };

  class init_declarator_info_list_head
  {
  public:
    void append(function_init_declarator_info &fidi) noexcept;
    void append(object_init_declarator_info &oidi) noexcept;
    void append(typedef_init_declarator_info &tidi) noexcept;

    template<typename callable_type>
    void for_each(callable_type &&c);

    template<typename callable_type>
    void for_each(callable_type &&c) const;

  private:
    init_declarator_info_list_node& _find_tail_node() noexcept;

    init_declarator_info_list_node _head;
  };

  class declaration_info
  {
  public:
    declaration_info(const ast::declaration &d,
		     function_definition_info * const _enclosing_fd = nullptr);

    const ast::declaration &declaration;

    // For 'extern' declaration within function definitions.
    function_definition_info * const enclosing_fd;

    const ast::storage_class_specifier * const storage_class_specifier;

    init_declarator_info_list_head init_declarators;

    deps_on_types declaration_specifiers_deps;
    tags_infos child_tags;

    template<typename callable_type>
    void for_each_deps_on_types(callable_type &&c) const;
  };

  class declarations_infos
  {
  private:
    typedef std::vector<std::unique_ptr<declaration_info>> _container_type;

  public:
    class iterator
      : public std::iterator<std::random_access_iterator_tag, declaration_info>
    {
    public:
	bool operator==(const iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const iterator &rhs)
	  const noexcept
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

	iterator&
	operator+=(const difference_type n) noexcept
	{ _it += n; return *this; }

	iterator&
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

	reference operator[](const difference_type n) const noexcept
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
	friend class declarations_infos;

	iterator(const _container_type::const_iterator &it) noexcept
	  : _it(it)
	{}

	_container_type::const_iterator _it;
    };

    class const_iterator
      : public std::iterator<std::random_access_iterator_tag,
			     const declaration_info>
    {
    public:
	bool operator==(const const_iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const const_iterator &rhs)
	  const noexcept
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

	const_iterator&
	operator+=(const difference_type n) noexcept
	{ _it += n; return *this; }

	const_iterator&
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
	friend class declarations_infos;

	const_iterator(const _container_type::const_iterator &it) noexcept
	  : _it(it)
	{}

	_container_type::const_iterator _it;
    };

    iterator begin() noexcept
    { return iterator{_dis.begin()}; }

    iterator end() noexcept
    { return iterator{_dis.end()}; }

    const_iterator begin() const noexcept
    { return const_iterator{_dis.begin()}; }

    const_iterator end() const noexcept
    { return const_iterator{_dis.end()}; }

    template <typename... args_types>
    iterator emplace_back(args_types&&... args)
    {
      return (iterator{
	  _dis.insert(_dis.end(),
		      std::unique_ptr<declaration_info> {
			new declaration_info{std::forward<args_types>(args)...}
		      })
	});
    }

  private:
    _container_type _dis;
  };

  class static_assert_info
  {
  public:
    static_assert_info(const ast::static_assertion &sa) noexcept;

    const ast::static_assertion &static_assertion;
    deps_on_types deps;
    tags_infos child_tags;
  };

  class static_asserts_infos
  {
  private:
    typedef std::vector<std::unique_ptr<static_assert_info>> _container_type;

  public:
    class iterator
      : public
	  std::iterator<std::random_access_iterator_tag, static_assert_info>
    {
    public:
	bool operator==(const iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const iterator &rhs)
	  const noexcept
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

	iterator&
	operator+=(const difference_type n) noexcept
	{ _it += n; return *this; }

	iterator&
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

	reference operator[](const difference_type n) const noexcept
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
	friend class static_asserts_infos;

	iterator(const _container_type::const_iterator &it) noexcept
	  : _it(it)
	{}

	_container_type::const_iterator _it;
    };

    class const_iterator
      : public std::iterator<std::random_access_iterator_tag,
			     const static_assert_info>
    {
    public:
	bool operator==(const const_iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const const_iterator &rhs)
	  const noexcept
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

	const_iterator&
	operator+=(const difference_type n) noexcept
	{ _it += n; return *this; }

	const_iterator&
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
	friend class static_asserts_infos;

	const_iterator(const _container_type::const_iterator &it) noexcept
	  : _it(it)
	{}

	_container_type::const_iterator _it;
    };

    iterator begin() noexcept
    { return iterator{_sais.begin()}; }

    iterator end() noexcept
    { return iterator{_sais.end()}; }

    const_iterator begin() const noexcept
    { return const_iterator{_sais.begin()}; }

    const_iterator end() const noexcept
    { return const_iterator{_sais.end()}; }

    template <typename... args_types>
    iterator emplace_back(args_types&&... args)
    {
      return (iterator{
		_sais.insert
		  (_sais.end(),
		   std::unique_ptr<static_assert_info> {
		     new static_assert_info{std::forward<args_types>(args)...}
		   })
	      });
    }

  private:
    _container_type _sais;
  };

  class function_init_declarator_info
  {
  public:
    const ast::init_declarator &init_declarator;

    declaration_info &enclosing_declaration;
    init_declarator_info_list_node same_declaration_list_node;

    function_info &fi;

    deps_on_types declarator_deps;
    tags_infos child_tags;

    bool emit_orig_decl;

  private:
    friend class ast_info;

    function_init_declarator_info(const ast::init_declarator &id,
				  declaration_info &enclosing_d,
				  class function_info &_fi);
  };

  class initializer_info;

  class object_init_declarator_info
  {
  public:
    const ast::init_declarator &init_declarator;

    declaration_info &enclosing_declaration;
    init_declarator_info_list_node same_declaration_list_node;

    object_info &oi;

    deps_on_types declarator_deps;

    tags_infos child_tags;

    unsigned int walk_gen_nr;

    initializer_info& create_initializer_info();

    initializer_info* get_initializer() noexcept
    { return _ii.get(); }

    const initializer_info* get_initializer() const noexcept
    { return _ii.get(); }

    bool need_decl_type;
    bool emit_orig_decl;
    bool need_orig_decl_type;

  private:
    friend class ast_info;

    object_init_declarator_info(const ast::init_declarator &id,
				declaration_info &enclosing_d,
				class object_info &_oi);

    std::unique_ptr<initializer_info> _ii;
  };

  class initializer_info
  {
  public:
    object_init_declarator_info &enclosing_id;
    deps_on deps;
    tags_infos child_tags;

    bool rewrite_needed;

  private:
    friend class object_init_declarator_info;
    initializer_info(object_init_declarator_info &_enclosing_id);
  };

  class typedef_info;

  class typedef_init_declarator_info
  {
  public:
    const ast::init_declarator &init_declarator;

    declaration_info &enclosing_declaration;
    init_declarator_info_list_node same_declaration_list_node;

    typedef_info &tdi;

    deps_on_types declarator_deps;
    tags_infos child_tags;

    bool emit;

  private:
    friend class ast_info;

    typedef_init_declarator_info(const ast::init_declarator &id,
				 declaration_info &enclosing_d,
				 class typedef_info &_tdi);
  };

  class function_definition_info
  {
  public:
    function_definition_info(const ast::function_definition &fd,
			     function_info &_fi);

    const ast::function_definition &function_definition;
    function_info &fi;

    deps_on_types declarator_deps;
    deps_on body_deps;
    tags_infos child_tags;

    bool is_patched;
    bool is_externalization_preferred;

    bool rewrite_needed;
    // Used for patched function definitions or those where
    // ->in_closure is set and which have to get rewritten.
    lp_creation_policy::symbol_modification sym_mod;

    unsigned int walk_gen_nr;

    void init_walk_data(unsigned int gen_nr) noexcept;
    unsigned int walk_gen_nr_data;
    bool has_reachable_incoming_er_edge;
    bool in_candidate_set;

    const declarations_infos& get_declarations() const noexcept
    { return _declarations_infos; }

    template<typename callable_type>
    void for_each_body_deps_on_types(callable_type &&c) const;

    template<typename callable_type>
    void for_each_deps_on_types(callable_type &&c) const;

    bool emit_orig_decl;

  private:
    friend class ast_info;

    // 'extern' declarations in body
    declarations_infos _declarations_infos;
  };


  class function_info
  {
  private:
    typedef std::vector<std::unique_ptr<function_init_declarator_info>>
    _idi_container_type;

  public:
    class init_declarator_iterator
      : public std::iterator<std::random_access_iterator_tag,
			     function_init_declarator_info>
    {
    public:
	bool operator==(const init_declarator_iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const init_declarator_iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept
	{ return **_it; }

	pointer operator->() const noexcept
	{ return (*_it).get(); }

	init_declarator_iterator& operator++() noexcept
	{ ++_it; return *this; }

	const init_declarator_iterator operator++(int) noexcept
	{ return init_declarator_iterator{_it++}; }

	init_declarator_iterator& operator--() noexcept
	{ --_it; return *this; }

	const init_declarator_iterator operator--(int) noexcept
	{ return init_declarator_iterator{_it--}; }

	init_declarator_iterator&
	operator+=(const difference_type n) noexcept
	{ _it += n; return *this; }

	init_declarator_iterator&
	operator-=(const difference_type n) noexcept
	{ _it -= n; return *this; }

	const init_declarator_iterator
	operator+(const difference_type n) const noexcept
	{ return init_declarator_iterator(_it + n); }

	const init_declarator_iterator
	operator-(const difference_type n) const noexcept
	{ return init_declarator_iterator(_it - n); }

	const difference_type
	operator-(const init_declarator_iterator &op) const noexcept
	{ return this->_it - op._it; }

	reference operator[](const difference_type n) const noexcept
	{ return *(*this + n); }

	bool operator<(const init_declarator_iterator &op) const noexcept
	{ return this->_it < op._it; }

	bool operator>(const init_declarator_iterator &op) const noexcept
	{ return this->_it > op._it; }

	bool operator<=(const init_declarator_iterator &op) const noexcept
	{ return this->_it <= op._it; }

	bool operator>=(const init_declarator_iterator &op) const noexcept
	{ return this->_it >= op._it; }

      private:
	friend class function_info;

	init_declarator_iterator(const _idi_container_type::const_iterator &it)
	  noexcept
	  : _it(it)
	{}

	_idi_container_type::const_iterator _it;
    };

    function_info();

    init_declarator_iterator init_declarators_begin() noexcept
    { return init_declarator_iterator(_idis.begin()); }

    init_declarator_iterator init_declarators_end() noexcept
    { return init_declarator_iterator(_idis.end()); }

    function_definition_info* get_function_definition() noexcept
    { return _fdi.get(); }

    const function_definition_info* get_function_definition() const noexcept
    { return _fdi.get(); }

    bool can_externalize_valid;
    bool can_externalize;

    bool externalize;
    bool in_closure;
    lp_creation_policy::externalized_symbol_modification externalized_sym_mod;

  private:
    friend class ast_info;

    _idi_container_type _idis;
    std::unique_ptr<function_definition_info> _fdi;
  };

  class object_info
  {
  private:
    typedef std::vector<std::unique_ptr<object_init_declarator_info>>
    _idi_container_type;

  public:
    class init_declarator_iterator
      : public std::iterator<std::random_access_iterator_tag,
			     object_init_declarator_info>
    {
    public:
	bool operator==(const init_declarator_iterator &rhs)
	  const noexcept
	{ return this->_it == rhs._it; }

	bool operator!=(const init_declarator_iterator &rhs)
	  const noexcept
	{ return !(*this == rhs); }

	reference operator*() const noexcept
	{ return **_it; }

	pointer operator->() const noexcept
	{ return (*_it).get(); }

	init_declarator_iterator& operator++() noexcept
	{ ++_it; return *this; }

	const init_declarator_iterator operator++(int) noexcept
	{ return init_declarator_iterator{_it++}; }

	init_declarator_iterator& operator--() noexcept
	{ --_it; return *this; }

	const init_declarator_iterator operator--(int) noexcept
	{ return init_declarator_iterator{_it--}; }

	init_declarator_iterator&
	operator+=(const difference_type n) noexcept
	{ _it += n; return *this; }

	init_declarator_iterator&
	operator-=(const difference_type n) noexcept
	{ _it -= n; return *this; }

	const init_declarator_iterator
	operator+(const difference_type n) const noexcept
	{ return init_declarator_iterator(_it + n); }

	const init_declarator_iterator
	operator-(const difference_type n) const noexcept
	{ return init_declarator_iterator(_it - n); }

	const difference_type
	operator-(const init_declarator_iterator &op) const noexcept
	{ return this->_it - op._it; }

	reference operator[](const difference_type n) const noexcept
	{ return *(*this + n); }

	bool operator<(const init_declarator_iterator &op) const noexcept
	{ return this->_it < op._it; }

	bool operator>(const init_declarator_iterator &op) const noexcept
	{ return this->_it > op._it; }

	bool operator<=(const init_declarator_iterator &op) const noexcept
	{ return this->_it <= op._it; }

	bool operator>=(const init_declarator_iterator &op) const noexcept
	{ return this->_it >= op._it; }

      private:
	friend class object_info;

	init_declarator_iterator(const _idi_container_type::const_iterator &it)
	  noexcept
	  : _it(it)
	{}

	_idi_container_type::const_iterator _it;
    };

    object_info();

    init_declarator_iterator init_declarators_begin() noexcept
    { return init_declarator_iterator(_idis.begin()); }

    init_declarator_iterator init_declarators_end() noexcept
    { return init_declarator_iterator(_idis.end()); }

    object_init_declarator_info* get_initializing_init_declarator() noexcept
    { return _initializing_id; }

    bool shall_externalize_valid;
    bool shall_externalize;

    bool externalize;
    bool in_closure;
    lp_creation_policy::externalized_symbol_modification externalized_sym_mod;

  private:
    friend class ast_info;

    _idi_container_type _idis;
    object_init_declarator_info *_initializing_id;
  };

  struct typedef_info
  {
    std::vector<std::unique_ptr<typedef_init_declarator_info>> idis;
  };

  class header_info
  {
  public:
    header_info(const pp_result::header_inclusion_root &root_node) noexcept;
    header_info(const pp_result::header_inclusion_child &child_node,
		header_info & _parent) noexcept;

    bool is_root() const noexcept
    { return !parent; }

    const pp_result::header_inclusion_root&
    get_root_node() const noexcept;

    const pp_result::header_inclusion_child&
    get_child_node() const noexcept;

    const raw_pp_tokens_range& get_range() const noexcept;

    header_info * const parent;

    bool eligible; // may be included from live patch
    bool only_directives;
    bool include;

  private:
    union
    {
      const pp_result::header_inclusion_root * const _root_node;
      const pp_result::header_inclusion_child * const _child_node;
    };
  };

  // In C99 speak, the language production "external declaration"
  // refers to either a declaration or function definition at file
  // scope.
  class external_declaration_info
  {
  public:
    enum class kind
    {
      declaration,
      static_assertion,
      function_definition,
    };

    external_declaration_info(declaration_info &di) noexcept;
    external_declaration_info(static_assert_info &di) noexcept;
    external_declaration_info(function_definition_info &fdi) noexcept;

    const kind k;

    const pp_tokens_range& get_tokens_range() const noexcept;

    declaration_info& get_declaration() noexcept;
    static_assert_info& get_static_assert() noexcept;
    function_definition_info& get_function_definition() noexcept;

  private:
    union
    {
      declaration_info * const _di;
      static_assert_info * const _sai;
      function_definition_info * const _fdi;
    };
  };


  class ast_info
  {
  private:
    typedef
    std::map<const ast::linkage::linkage_id, std::unique_ptr<function_info>>
    _function_info_container_type;

    typedef
    std::map<const ast::linkage::linkage_id, std::unique_ptr<object_info>>
    _object_info_container_type;

    typedef std::vector<external_declaration_info>
    _external_declaration_info_container_type;

    typedef std::map<const ast::sou_id, std::unique_ptr<sou_info>>
    _sou_info_container_type;

    typedef std::map<const ast::enum_id, std::unique_ptr<enum_info>>
    _enum_info_container_type;

  public:
    ast_info(const ast::ast_translation_unit &_atu);

    unsigned int start_walk() noexcept;
    void finish_walk() noexcept;

    template <typename parent_type, typename... tag_parent_args>
    sou_def_info& create_info(const ast::struct_or_union_def &soud,
			      parent_type &parent,
			      tag_parent_args&&... args);

    template <typename parent_type, typename... tag_parent_args>
    sou_ref_info& create_info(const ast::struct_or_union_ref &sour,
			      parent_type &parent,
			      tag_parent_args&&... args);

    template <typename parent_type, typename... tag_parent_args>
    enum_def_info& create_info(const ast::enum_def &ed,
			       parent_type &parent,
			       tag_parent_args&&... args);

    template <typename parent_type, typename... tag_parent_args>
    enum_ref_info& create_info(const ast::enum_ref &er,
			       parent_type &parent,
			       tag_parent_args&&... args);

    declaration_info& create_info(const ast::declaration &d);

    declaration_info& create_info(const ast::declaration &d,
				  function_definition_info &enclosing_fd);

    static_assert_info& create_info(const ast::static_assertion &sa);

    struct function_tag{};

    function_info& create_info(const ast::linkage::linkage_id &lid,
			       const function_tag&);

    function_init_declarator_info& create_info(const ast::init_declarator &id,
					       declaration_info &enclosing_d,
					       const function_tag&);
    struct object_tag{};

    object_info& create_info(const ast::linkage::linkage_id &lid,
			     const object_tag&);

    object_init_declarator_info& create_info(const ast::init_declarator &id,
					     declaration_info &enclosing_d,
					     const object_tag&);
    struct typedef_tag{};

    typedef_init_declarator_info&
    create_info(const ast::init_declarator &id, declaration_info &enclosing_d,
		const typedef_tag&);
    function_definition_info& create_info(const ast::function_definition &fd);

    function_info& get_function_info_by_lid(const ast::linkage::linkage_id &lid)
      noexcept;
    object_info& get_object_info_by_lid(const ast::linkage::linkage_id &lid)
      noexcept;

    class function_iterator
      : public std::iterator<std::forward_iterator_tag,
			     function_info>
    {
    public:
      bool operator==(const function_iterator &rhs) const noexcept
      { return this->_it == rhs._it; }

      bool operator!=(const function_iterator &rhs) const noexcept
      { return !(*this == rhs); }

      reference operator*() const noexcept
      { return *_it->second; }

      pointer operator->() const noexcept
      { return _it->second.get(); }

      function_iterator& operator++() noexcept
      { ++_it; return *this; }

      const function_iterator operator++(int) noexcept
      { return function_iterator{_it++}; }

    private:
      friend class ast_info;

      function_iterator(const _function_info_container_type::iterator &it)
	noexcept
	: _it(it)
      {}

      _function_info_container_type::iterator _it;
    };

    function_iterator functions_begin() noexcept
    {
      return function_iterator{
		_lid_to_functions_infos.begin()
	     };
    }

    function_iterator functions_end() noexcept
    {
      return function_iterator{
		_lid_to_functions_infos.end()
	     };
    }

    class object_iterator
      : public std::iterator<std::forward_iterator_tag,
			     object_info>
    {
    public:
      bool operator==(const object_iterator &rhs) const noexcept
      { return this->_it == rhs._it; }

      bool operator!=(const object_iterator &rhs) const noexcept
      { return !(*this == rhs); }

      reference operator*() const noexcept
      { return *_it->second; }

      pointer operator->() const noexcept
      { return _it->second.get(); }

      object_iterator& operator++() noexcept
      { ++_it; return *this; }

      const object_iterator operator++(int) noexcept
      { return object_iterator{_it++}; }

    private:
      friend class ast_info;

      object_iterator(const _object_info_container_type::iterator &it)
	noexcept
	: _it(it)
      {}

      _object_info_container_type::iterator _it;
    };

    object_iterator objects_begin() noexcept
    {
      return object_iterator{
		_lid_to_objects_infos.begin()
	     };
    }

    object_iterator objects_end() noexcept
    {
      return object_iterator{
		_lid_to_objects_infos.end()
	     };
    }

    class sou_iterator
      : public std::iterator<std::forward_iterator_tag,
			     sou_info>
    {
    public:
      bool operator==(const sou_iterator &rhs) const noexcept
      { return this->_it == rhs._it; }

      bool operator!=(const sou_iterator &rhs) const noexcept
      { return !(*this == rhs); }

      reference operator*() const noexcept
      { return *_it->second; }

      pointer operator->() const noexcept
      { return _it->second.get(); }

      sou_iterator& operator++() noexcept
      { ++_it; return *this; }

      const sou_iterator operator++(int) noexcept
      { return sou_iterator{_it++}; }

    private:
      friend class ast_info;

      sou_iterator(const _sou_info_container_type::iterator &it)
	noexcept
	: _it(it)
      {}

      _sou_info_container_type::iterator _it;
    };

    sou_iterator sous_begin() noexcept
    {
      return sou_iterator{
		_sou_ids_to_infos.begin()
	     };
    }

    sou_iterator sous_end() noexcept
    {
      return sou_iterator{
		_sou_ids_to_infos.end()
	     };
    }

    sou_info* get_sou_info_by_id(const ast::sou_id &sou_id) noexcept;

    class enum_iterator
      : public std::iterator<std::forward_iterator_tag,
			     enum_info>
    {
    public:
      bool operator==(const enum_iterator &rhs) const noexcept
      { return this->_it == rhs._it; }

      bool operator!=(const enum_iterator &rhs)
      { return !(*this == rhs); }

      reference operator*() const noexcept
      { return *_it->second; }

      pointer operator->() const noexcept
      { return _it->second.get(); }

      enum_iterator& operator++() noexcept
      { ++_it; return *this; }

      const enum_iterator operator++(int) noexcept
      { return enum_iterator{_it++}; }

    private:
      friend class ast_info;

      enum_iterator(const _enum_info_container_type::iterator &it)
	noexcept
	: _it(it)
      {}

      _enum_info_container_type::iterator _it;
    };

    enum_iterator enums_begin() noexcept
    {
      return enum_iterator{
		_enum_ids_to_infos.begin()
	     };
    }

    enum_iterator enums_end() noexcept
    {
      return enum_iterator{
		_enum_ids_to_infos.end()
	     };
    }

    enum_info* get_enum_info_by_id(const ast::enum_id &enum_id) noexcept;

    const lp_creation_policy::allocated_ids_type get_allocated_ids() noexcept
    { return _allocated_ids; }

    void
    register_sym_mod(const lp_creation_policy::symbol_modification &sym_mod)
      noexcept;

    const ast::ast_translation_unit &atu;

    typedef std::vector<header_info> header_infos_type;
    header_infos_type header_infos;

    typedef _external_declaration_info_container_type::iterator
	external_declaration_iterator;

    external_declaration_iterator
    intersecting_external_declarations_begin(const raw_pp_tokens_range &r)
      noexcept;

    external_declaration_iterator
    intersecting_external_declarations_end(const raw_pp_tokens_range &r)
      noexcept;

    external_declaration_iterator external_declarations_begin() noexcept
    { return _external_declaration_infos.begin(); }

    external_declaration_iterator external_declarations_end() noexcept
    { return _external_declaration_infos.end(); }

    void mark_header_uneligible(header_info &hi) noexcept;

  private:
    void _init_header_infos(const pp_result &ppr);

    sou_info& _create_info(const ast::sou_id &sou_id);
    enum_info& _create_info(const ast::enum_id &enum_id);

    declarations_infos _declarations_infos;
    static_asserts_infos _static_asserts_infos;
    _function_info_container_type _lid_to_functions_infos;
    _object_info_container_type _lid_to_objects_infos;
    std::map<const ast::linkage::linkage_id,
	     std::unique_ptr<typedef_info> > _lid_to_typedefs_infos;

    lp_creation_policy::allocated_ids_type _allocated_ids;

    _sou_info_container_type _sou_ids_to_infos;
    _enum_info_container_type _enum_ids_to_infos;

    // Sorted by file order
    _external_declaration_info_container_type _external_declaration_infos;

    unsigned int _last_walk_gen_nr;
  };
}

dep_on_sou_decl::dep_on_sou_decl(sou_def_info &soud) noexcept
  : k(kind::def), _soud(&soud)
{}

dep_on_sou_decl::dep_on_sou_decl(sou_ref_info &sour) noexcept
  : k(kind::ref), _sour(&sour)
{
  assert(_sour->sour.get_link_to_decl().get_target_kind() ==
	 ast::sou_decl_link::target_kind::unlinked);
}

bool dep_on_sou_decl::operator<(const dep_on_sou_decl &d) const noexcept
{
  return this->get_decl_list_node().get_id() < d.get_decl_list_node().get_id();
}

const ast::sou_decl_list_node& dep_on_sou_decl::get_decl_list_node()
  const noexcept
{
  switch (k) {
    case kind::def:
      return _soud->soud.get_decl_list_node();
    case kind::ref:
      return _sour->sour.get_decl_list_node();
  }
}

sou_info& dep_on_sou_decl::get_sou_info() const noexcept
{
  switch (k) {
    case kind::def:
      return _soud->soui;
    case kind::ref:
      return _sour->soui;
  }
}

sou_def_info& dep_on_sou_decl::get_soud() const noexcept
{
  assert(k == kind::def);
  return *_soud;
}

sou_ref_info& dep_on_sou_decl::get_sour() const noexcept
{
  assert(k == kind::ref);
  return *_sour;
}


deps_on_sou_decls& deps_on_sou_decls::operator+=(const deps_on_sou_decls &rhs)
{
  if (this == &rhs)
    return *this;

  for (const auto &d : rhs)
    this->add(d);

  return *this;
}

bool deps_on_sou_decls::add(const dep_on_sou_decl &d)
{
  const auto it = _deps.find(d);

  if (it == _deps.end()) {
    _deps.insert(d);
    return true;
  }

  // See if d is visible from *it and replace it if so.
  if (d.get_decl_list_node().is_visible_from(it->get_decl_list_node())) {
    _deps.erase(it);
    _deps.insert(d);
    return true;
  } else {
    assert(it->get_decl_list_node().is_visible_from(d.get_decl_list_node()));
    return false;
  }
}


dep_on_sou_def::dep_on_sou_def(sou_def_info &_soud) noexcept
  : soud(_soud)
{}

bool dep_on_sou_def::operator<(const dep_on_sou_def &d) const noexcept
{
  return &this->soud < &d.soud;
}


void deps_on_sou_defs::add(const dep_on_sou_def &d)
{
  _deps.insert(d);
}


dep_on_enum_decl::dep_on_enum_decl(enum_def_info &ed) noexcept
  : k(kind::def), _ed(&ed)
{}

dep_on_enum_decl::dep_on_enum_decl(enum_ref_info &er) noexcept
  : k(kind::ref), _er(&er)
{
  assert(_er->er.get_link_to_decl().get_target_kind() ==
	 ast::enum_decl_link::target_kind::unlinked);
}

bool dep_on_enum_decl::operator<(const dep_on_enum_decl &d) const noexcept
{
  return this->get_decl_list_node().get_id() < d.get_decl_list_node().get_id();
}

const ast::enum_decl_list_node& dep_on_enum_decl::get_decl_list_node()
  const noexcept
{
  switch (k) {
    case kind::def:
      return _ed->ed.get_decl_list_node();
    case kind::ref:
      return _er->er.get_decl_list_node();
  }
}

enum_info& dep_on_enum_decl::get_enum_info() const noexcept
{
  switch (k) {
  case kind::def:
    return _ed->ei;
  case kind::ref:
    return _er->ei;
  }
}

enum_def_info& dep_on_enum_decl::get_ed() const noexcept
{
  assert(k == kind::def);
  return *_ed;
}

enum_ref_info& dep_on_enum_decl::get_er() const noexcept
{
  assert(k == kind::ref);
  return *_er;
}


deps_on_enum_decls&
deps_on_enum_decls::operator+=(const deps_on_enum_decls &rhs)
{
  if (this == &rhs)
    return *this;

  for (const auto &d : rhs)
    this->add(d);

  return *this;
}

bool deps_on_enum_decls::add(const dep_on_enum_decl &d)
{
  const auto it = _deps.find(d);

  if (it == _deps.end()) {
    _deps.insert(d);
    return true;
  }

  // See if d is visible from *it and replace it if so.
  if (d.get_decl_list_node().is_visible_from(it->get_decl_list_node())) {
    _deps.erase(it);
    _deps.insert(d);
    return true;
  } else {
    assert(it->get_decl_list_node().is_visible_from(d.get_decl_list_node()));
    return false;
  }
}


dep_on_enum_def::dep_on_enum_def(enum_def_info &_ed) noexcept
  : ed(_ed)
{}

bool dep_on_enum_def::operator<(const dep_on_enum_def &d) const noexcept
{
  return &this->ed < &d.ed;
}


void deps_on_enum_defs::add(const dep_on_enum_def &d)
{
  _deps.insert(d);
}

dep_on_tag_non_decl::dep_on_tag_non_decl(const std::string &_id) noexcept
  : id(_id)
{}

bool dep_on_tag_non_decl::operator<(const dep_on_tag_non_decl &d) const noexcept
{
  return this->id < d.id;
}


deps_on_tag_non_decls& deps_on_tag_non_decls::
operator+=(const deps_on_tag_non_decls &rhs)
{
  if (this == &rhs)
    return *this;

  for (const auto &d : rhs._deps)
    this->add(d);

  return *this;
}

void deps_on_tag_non_decls::add(const dep_on_tag_non_decl &d)
{
  _deps.insert(d);
}

bool deps_on_tag_non_decls::contains(const std::string &id) const
{
  const dep_on_tag_non_decl dep{id};

  return _deps.count(dep);
}

bool deps_on_tag_non_decls::empty() const noexcept
{
  return _deps.empty();
}


dep_on_typedef::dep_on_typedef(typedef_init_declarator_info &_tdidi) noexcept
  : tdidi(_tdidi)
{}

bool dep_on_typedef::operator<(const dep_on_typedef &d) const noexcept
{
  return (this->tdidi.init_declarator.get_linkage().get_id()
	  < d.tdidi.init_declarator.get_linkage().get_id());
}


deps_on_typedefs& deps_on_typedefs::operator+=(const deps_on_typedefs &rhs)
{
  if (this == &rhs)
    return *this;

  for (const auto &d : rhs)
    this->add(d);

  return *this;
}

void deps_on_typedefs::add(const dep_on_typedef &d)
{
  const auto it = _deps.find(d);

  if (it == _deps.end()) {
    _deps.insert(d);
    return;
  }

  // See if *it is visible from d, otherwise replace it
  if (&it->tdidi == &d.tdidi)
    return;
  it->tdidi.init_declarator.get_linkage().for_each_visible
    (wrap_callables<default_action_unreachable<bool, type_set<> >::type>
     ([&](const ast::init_declarator &id) {
	if (&id == &d.tdidi.init_declarator) {
	  _deps.erase(it);
	  _deps.insert(d);
	  return false;
	}
	return true;
      }));
}


dep_on_decl_type::dep_on_decl_type(function_definition_info &fdi,
				   const bool _need_composite,
				   const ast::expr_id &_from_eid) noexcept
  : from_eid(_from_eid), _k(kind::k_function_definition), _fdi(&fdi),
    need_composite(_need_composite)
{}

dep_on_decl_type::dep_on_decl_type(function_init_declarator_info &fidi,
				   const bool _need_composite,
				   const ast::expr_id &_from_eid) noexcept
  : from_eid(_from_eid), _k(kind::k_function_init_declarator), _fidi(&fidi),
   need_composite(_need_composite)
{}

dep_on_decl_type::dep_on_decl_type(object_init_declarator_info &oidi,
				   const bool _need_composite,
				   const ast::expr_id &_from_eid) noexcept
  : from_eid(_from_eid), _k(kind::k_object_init_declarator), _oidi(&oidi),
    need_composite(_need_composite)
{}

const lp_creation_policy::externalized_symbol_modification*
dep_on_decl_type::get_target_externalized_sym_mod() const noexcept
{
  switch (_k) {
  case kind::k_function_definition:
    if (!_fdi->fi.externalize)
      return nullptr;

    return &_fdi->fi.externalized_sym_mod;

  case kind::k_function_init_declarator:
    if (!_fidi->fi.externalize)
      return nullptr;

    return &_fidi->fi.externalized_sym_mod;

  case kind::k_object_init_declarator:
    if (!_oidi->oi.externalize)
      return nullptr;

    return &_oidi->oi.externalized_sym_mod;
  }
}

const function_definition_info& dep_on_decl_type::get_fdi() const noexcept
{
  assert(_k == kind::k_function_definition);
  return *_fdi;
}

const function_init_declarator_info& dep_on_decl_type::get_fidi() const noexcept
{
  assert(_k == kind::k_function_init_declarator);
  return *_fidi;
}

const object_init_declarator_info& dep_on_decl_type::get_oidi() const noexcept
{
  assert(_k == kind::k_object_init_declarator);
  return *_oidi;
}

template<typename callable_type>
void dep_on_decl_type::for_each_visible(callable_type &&c) const
{
  function_info *fi = nullptr;
  object_info *oi = nullptr;
  const ast::linkage *l;

  switch (_k) {
  case kind::k_function_definition:
    fi = &_fdi->fi;
    l = &_fdi->function_definition.get_linkage();
    if (!c(*_fdi))
      return;
    break;

  case kind::k_function_init_declarator:
    fi = &_fidi->fi;
    l = &_fidi->init_declarator.get_linkage();
    if (!c(*_fidi))
      return;
    break;

  case kind::k_object_init_declarator:
    oi = &_oidi->oi;
    l = &_oidi->init_declarator.get_linkage();
    if (!c(*_oidi))
      return;
    break;
  }

  l->for_each_visible
    (wrap_callables<no_default_action>
     ([&](const ast::init_declarator &id) {
	if (fi) {
	  const auto it_end = fi->init_declarators_end();
	  function_init_declarator_info *fidi = nullptr;
	  for (auto it = fi->init_declarators_begin(); it != it_end; ++it) {
	    if (&it->init_declarator == &id) {
	      fidi = &*it;
	      break;
	    }
	  }
	  assert(fidi);
	  return c(*fidi);
	} else {
	  const auto it_end = oi->init_declarators_end();
	  object_init_declarator_info *oidi = nullptr;
	  for (auto it = oi->init_declarators_begin(); it != it_end; ++it) {
	    if (&it->init_declarator == &id) {
	      oidi = &*it;
	      break;
	    }
	  }
	  assert(oidi);
	  return c(*oidi);
	}
      },
      [&](const ast::function_definition &fd) {
	assert(fi);
	function_definition_info * const fdi = fi->get_function_definition();
	assert(fdi);
	assert(&fdi->function_definition == &fd);
	return c(*fdi);
      }));
}


void deps_on_decl_types::add(const dep_on_decl_type &d)
{
  _deps.push_back(d);
}


dep_on_obj::dep_on_obj(struct object_init_declarator_info &_oidi,
		       const ast::expr_id &_from_eid) noexcept
  : oidi(_oidi), from_eid(&_from_eid), externalize(false)
{}

dep_on_obj::dep_on_obj(object_init_declarator_info &_oidi) noexcept
  : oidi(_oidi), from_eid(nullptr), externalize(false)
{}

object_info& dep_on_obj::get_object_info() noexcept
{
  return this->oidi.get().oi;
}

const object_info& dep_on_obj::get_object_info() const noexcept
{
  return this->oidi.get().oi;
}


void deps_on_objs::add(const dep_on_obj &d)
{
  _deps.push_back(d);
}


dep_on_fun::dep_on_fun(struct function_definition_info &_fdi,
		       const ast::expr_id &_from_eid) noexcept
  : fdi(&_fdi), fidi(nullptr),
    from_eid(_from_eid), externalize(false)
{}

dep_on_fun::dep_on_fun(struct function_init_declarator_info &_fidi,
		       const ast::expr_id &_from_eid) noexcept
  : fdi(nullptr), fidi(&_fidi),
    from_eid(_from_eid), externalize(false)
{}

function_info& dep_on_fun::get_function_info() noexcept
{
  assert(fdi || fidi);
  if (fdi)
    return fdi->fi;
  else
    return fidi->fi;
}

const function_info& dep_on_fun::get_function_info() const noexcept
{
  assert(fdi || fidi);
  if (fdi)
    return fdi->fi;
  else
    return fidi->fi;
}

const ast::direct_declarator_id dep_on_fun::get_direct_declarator_id()
  const noexcept
{
  const ast::declarator *d;
  assert(fdi || fidi);
  if (fdi)
    d = &fdi->function_definition.get_declarator();
  else
    d = &fidi->init_declarator.get_declarator();
  return d->get_direct_declarator_id();
}


void deps_on_funs::add(const dep_on_fun &d)
{
  _deps.push_back(d);
}


tag_parent::tag_parent(declaration_info &p) noexcept
  : k(kind::k_declaration), _parent_decl(&p)
{}

tag_parent::tag_parent(static_assert_info &p) noexcept
  : k(kind::k_static_assert), _parent_sa(&p)
{}

tag_parent::tag_parent(function_definition_info &p) noexcept
  : k(kind::k_function_definition), _parent_fd(&p)
{}

tag_parent::tag_parent(function_init_declarator_info &p) noexcept
  : k(kind::k_function_init_declarator), _parent_fid(&p)
{}

tag_parent::tag_parent(object_init_declarator_info &p) noexcept
  : k(kind::k_object_init_declarator), _parent_oid(&p)
{}

tag_parent::tag_parent(initializer_info &p) noexcept
  : k(kind::k_object_initializer), _parent_ii(&p)
{}

tag_parent::tag_parent(typedef_init_declarator_info &p) noexcept
  : k(kind::k_typedef_init_declarator), _parent_tid(&p)
{}

tag_parent::tag_parent(sou_def_info &p) noexcept
  : k(kind::k_sou_def), _parent_soud(&p)
{}

tag_parent::tag_parent(enum_def_info &p) noexcept
  : k(kind::k_enum_def), _parent_ed(&p)
{}

template<typename callable_type>
bool tag_parent::on_outermost_tag_def(callable_type &&c)
{
  // Find the outermost tag definition.
  sou_def_info *outermost_soudi = nullptr;
  enum_def_info *outermost_edi = nullptr;

  const tag_parent *tp = this;
  while (tp) {
    switch (tp->k) {
    case tag_parent::kind::k_sou_def:
      outermost_soudi = tp->_parent_soud;
      tp = &outermost_soudi->parent;
      outermost_edi = nullptr;
      break;

    case tag_parent::kind::k_enum_def:
      outermost_soudi = nullptr;
      outermost_edi = tp->_parent_ed;
      tp = &outermost_edi->parent;
      break;

    default:
      tp = nullptr;
      break;
    }
  };

  if (outermost_soudi) {
    c(*outermost_soudi);
    return true;
  } else if (outermost_edi) {
    c(*outermost_edi);
    return true;
  }

  return false;
}

template<typename callable_type>
void tags_infos::for_each_child_tag(callable_type &&c)
{
  for (auto soudi : _souds) {
    c(soudi.get());
  }
  for (auto edi : _eds) {
    c(edi.get());
  }
  for (auto souri : _sours) {
    c(souri.get());
  }
  for (auto eri : _ers) {
    c(eri.get());
  }
}

template<typename callable_type>
void tags_infos::for_each_child_tag(callable_type &&c) const
{
  for (auto soudi : _souds) {
    c(const_cast<const sou_def_info&>(soudi.get()));
  }
  for (auto edi : _eds) {
    c(const_cast<const enum_def_info&>(edi.get()));
  }
  for (auto souri : _sours) {
    c(const_cast<const sou_ref_info&>(souri.get()));
  }
  for (auto eri : _ers) {
    c(const_cast<const enum_ref_info&>(eri.get()));
  }
}

template<typename callable_type>
void tags_infos::for_each_tag_recursive(callable_type &&c)
{
  std::stack<_walk_stack_entry> ws;
  ws.emplace(*this, _souds.cbegin(), _eds.cbegin());

  // Walk in DFS pre order.
  while(!ws.empty()) {
    _walk_stack_entry &top = ws.top();

    if (top.it_soud != top.ti._souds.cend()) {
      if (c(top.it_soud->get())) {
	const tags_infos &next = top.it_soud->get().child_tags;
	ws.emplace(next, next._souds.cbegin(), next._eds.cbegin());
      }
      ++top.it_soud;

    } else if (top.it_ed != top.ti._eds.cend()) {
      if (c(top.it_ed->get())) {
	const tags_infos &next = top.it_ed->get().child_tags;
	ws.emplace(next, next._souds.cbegin(), next._eds.cbegin());
      }
      ++top.it_ed;

    } else {
      for (const auto &sour : top.ti._sours) {
	c(sour.get());
      }
      for (const auto &er : top.ti._ers) {
	c(er.get());
      }
      ws.pop();
    }
  }
}

template<typename callable_type>
void tags_infos::for_each_tag_recursive(callable_type &&c) const
{
  std::stack<_walk_stack_entry> ws;
  ws.emplace(*this, _souds.cbegin(), _eds.cbegin());

  // Walk in DFS pre order.
  while(!ws.empty()) {
    _walk_stack_entry &top = ws.top();

    if (top.it_soud != top.ti._souds.cend()) {
      if (c(const_cast<const sou_def_info&>(top.it_soud->get()))) {
	const tags_infos &next = top.it_soud->get().child_tags;
	ws.emplace(next, next._souds.cbegin(), next._eds.cbegin());
      }
      ++top.it_soud;

    } else if (top.it_ed != top.ti._eds.cend()) {
      if (c(const_cast<const enum_def_info&>(top.it_ed->get()))) {
	const tags_infos &next = top.it_ed->get().child_tags;
	ws.emplace(next, next._souds.cbegin(), next._eds.cbegin());
      }
      ++top.it_ed;

    } else {
      for (const auto &sour : top.ti._sours) {
	c(const_cast<const sou_ref_info&>(sour.get()));
      }
      for (const auto &er : top.ti._ers) {
	c(const_cast<const enum_ref_info&>(er.get()));
      }
      ws.pop();
    }
  }
}

template<typename callable_type>
void tags_infos::for_each_deps(callable_type &&c) const
{
  this->for_each_tag_recursive
    (wrap_callables<default_action_nop>
     ([&](const sou_def_info &sdi) {
	c(sdi.deps);
	return true;
      },
      [&](const enum_def_info &edi) {
	c(edi.deps);
	return true;
      }));
}

bool tags_infos::empty() const noexcept
{
  return _souds.empty() && _sours.empty() && _eds.empty() && _ers.empty();
}


tags_infos::_walk_stack_entry::
_walk_stack_entry(const tags_infos &_ti,
		  const _souds_type::const_iterator &_it_soud,
		  const _eds_type::const_iterator &_it_ed)
  : ti(_ti), it_soud(_it_soud), it_ed(_it_ed)
{}


sou_def_info::sou_def_info(const ast::struct_or_union_def &_soud,
			   const tag_parent &p,
			   sou_info &_soui) noexcept
  : soud(_soud), parent(p), soui(_soui),
    emit_tag_decl(false), emit_def(false)
{}

template<typename callable_type>
void sou_def_info::on_outermost_tag_def(callable_type &&c)
{
  if (!this->parent.on_outermost_tag_def(std::forward<callable_type>(c)))
    c(*this);
}


sou_ref_info::sou_ref_info(const ast::struct_or_union_ref &_sour,
			   const tag_parent &p,
			   sou_info &_soui) noexcept
  : sour(_sour), parent(p), soui(_soui), emit_tag_decl(false)
{}


enum_def_info::enum_def_info(const ast::enum_def &_ed,
			     const tag_parent &p,
			     enum_info &_ei) noexcept
  : ed(_ed), parent(p), ei(_ei),
    emit_tag_decl(false), emit_def(false)
{}

template<typename callable_type>
void enum_def_info::on_outermost_tag_def(callable_type &&c)
{
  if (!this->parent.on_outermost_tag_def(std::forward<callable_type>(c)))
    c(*this);
}


enum_ref_info::enum_ref_info(const ast::enum_ref &_er,
			     const tag_parent &p,
			     enum_info &_ei) noexcept
  : er(_er), parent(p), ei(_ei),
    emit_tag_decl(false)
{}


void init_declarator_info_list_head::append(function_init_declarator_info &fidi)
  noexcept
{
  assert(fidi.same_declaration_list_node._k ==
	 init_declarator_info_list_node::next_kind::nk_null);
  auto &tail = _find_tail_node();
  tail._k = init_declarator_info_list_node::next_kind::nk_fidi;
  tail._fidi = &fidi;
}

void init_declarator_info_list_head::append(object_init_declarator_info &oidi)
  noexcept
{
  assert(oidi.same_declaration_list_node._k ==
	 init_declarator_info_list_node::next_kind::nk_null);
  auto &tail = _find_tail_node();
  tail._k = init_declarator_info_list_node::next_kind::nk_oidi;
  tail._oidi = &oidi;
}

void init_declarator_info_list_head::append(typedef_init_declarator_info &tidi)
  noexcept
{
  assert(tidi.same_declaration_list_node._k ==
	 init_declarator_info_list_node::next_kind::nk_null);
  auto &tail = _find_tail_node();
  tail._k = init_declarator_info_list_node::next_kind::nk_tidi;
  tail._tidi = &tidi;
}

template<typename callable_type>
void init_declarator_info_list_head::for_each(callable_type &&c)
{
  init_declarator_info_list_node *cur = &_head;

  while (cur->_k != init_declarator_info_list_node::next_kind::nk_null) {
    switch (cur->_k) {
    case init_declarator_info_list_node::next_kind::nk_null:
      assert(0);
      __builtin_unreachable();

    case init_declarator_info_list_node::next_kind::nk_fidi:
      c(*cur->_fidi);
      cur = &cur->_fidi->same_declaration_list_node;
      break;

    case init_declarator_info_list_node::next_kind::nk_oidi:
      c(*cur->_oidi);
      cur = &cur->_oidi->same_declaration_list_node;
      break;

    case init_declarator_info_list_node::next_kind::nk_tidi:
      c(*cur->_tidi);
      cur = &cur->_tidi->same_declaration_list_node;
      break;
    }
  }
}

template<typename callable_type>
void init_declarator_info_list_head::for_each(callable_type &&c) const
{
  const init_declarator_info_list_node *cur = &_head;

  while (cur->_k != init_declarator_info_list_node::next_kind::nk_null) {
    switch (cur->_k) {
    case init_declarator_info_list_node::next_kind::nk_null:
      assert(0);
      __builtin_unreachable();

    case init_declarator_info_list_node::next_kind::nk_fidi:
      c(const_cast<function_init_declarator_info&>(*cur->_fidi));
      cur = &cur->_fidi->same_declaration_list_node;
      break;

    case init_declarator_info_list_node::next_kind::nk_oidi:
      c(const_cast<object_init_declarator_info&>(*cur->_oidi));
      cur = &cur->_oidi->same_declaration_list_node;
      break;

    case init_declarator_info_list_node::next_kind::nk_tidi:
      c(const_cast<typedef_init_declarator_info&>(*cur->_tidi));
      cur = &cur->_tidi->same_declaration_list_node;
      break;
    }
  }
}

init_declarator_info_list_node&
init_declarator_info_list_head::_find_tail_node() noexcept
{
  init_declarator_info_list_node *cur = &_head;
  while (cur->_k != init_declarator_info_list_node::next_kind::nk_null) {
    switch (cur->_k) {
    case init_declarator_info_list_node::next_kind::nk_null:
      assert(0);
      __builtin_unreachable();

    case init_declarator_info_list_node::next_kind::nk_fidi:
      cur = &cur->_fidi->same_declaration_list_node;
      break;

    case init_declarator_info_list_node::next_kind::nk_oidi:
      cur = &cur->_oidi->same_declaration_list_node;
      break;

    case init_declarator_info_list_node::next_kind::nk_tidi:
      cur = &cur->_tidi->same_declaration_list_node;
      break;
    }
  }

  return *cur;
}


const ast::storage_class_specifier *
_find_scs(const ast::declaration_specifiers &ds) noexcept
{
  const ast::storage_class_specifier *scs = nullptr;

  // There will be at most one.
  ds.for_each_storage_class_specifier
    ([&scs](const ast::storage_class_specifier &_scs) {
       assert(!scs);
       scs = &_scs;
     });
  return scs;
}


declaration_info::
declaration_info(const ast::declaration &d,
		 function_definition_info * const _enclosing_fd)
  : declaration(d), enclosing_fd(_enclosing_fd),
    storage_class_specifier(_find_scs(d.get_declaration_specifiers()))
{}

template<typename callable_type>
void declaration_info::for_each_deps_on_types(callable_type &&c) const
{
  c(this->declaration_specifiers_deps);
  this->child_tags.for_each_deps(c);
  this->init_declarators.for_each
    (wrap_callables<no_default_action>
     ([&](const function_init_declarator_info &fidi) {
	c(fidi.declarator_deps);
	fidi.child_tags.for_each_deps(c);
      },
      [&](const object_init_declarator_info &oidi) {
	c(oidi.declarator_deps);
	oidi.child_tags.for_each_deps(c);

	const initializer_info * const ii = oidi.get_initializer();
	if (ii) {
	  c(ii->deps.on_types);
	  ii->child_tags.for_each_deps(c);
	}
      },
      [&](const typedef_init_declarator_info &tidi) {
	c(tidi.declarator_deps);
	tidi.child_tags.for_each_deps(c);
      }));
}


static_assert_info::static_assert_info(const ast::static_assertion &sa) noexcept
  : static_assertion{sa}
{}


function_init_declarator_info::
function_init_declarator_info(const ast::init_declarator &id,
			      declaration_info &enclosing_d,
			      struct function_info &_fi)
  : init_declarator(id), enclosing_declaration(enclosing_d),
    fi(_fi), emit_orig_decl(false)
{
  enclosing_d.init_declarators.append(*this);
}


object_init_declarator_info::
object_init_declarator_info(const ast::init_declarator &id,
			    declaration_info &enclosing_d,
			    struct object_info &_oi)
  : init_declarator(id), enclosing_declaration(enclosing_d),
    oi(_oi), walk_gen_nr(0),
    need_decl_type(false), emit_orig_decl(false), need_orig_decl_type(false)
{
  enclosing_d.init_declarators.append(*this);
}

initializer_info& object_init_declarator_info::create_initializer_info()
{
  assert (!_ii);
  _ii.reset(new initializer_info(*this));
  return *_ii;
}


initializer_info::initializer_info(object_init_declarator_info &_enclosing_id)
  : enclosing_id(_enclosing_id), rewrite_needed(false)
{}


typedef_init_declarator_info::
typedef_init_declarator_info(const ast::init_declarator &id,
			     declaration_info &enclosing_d,
			     struct typedef_info &_tdi)
  : init_declarator(id), enclosing_declaration(enclosing_d),
    tdi(_tdi), emit(false)
{
  enclosing_d.init_declarators.append(*this);
}


function_definition_info::
function_definition_info(const ast::function_definition &fd,
			 struct function_info &_fi)
  : function_definition(fd), fi(_fi),
    walk_gen_nr(0), is_patched(false), is_externalization_preferred(false),
    rewrite_needed(false), walk_gen_nr_data(0),
    has_reachable_incoming_er_edge(false), in_candidate_set(false),
    emit_orig_decl(false)
{}

void function_definition_info::init_walk_data(unsigned int gen_nr) noexcept
{
  if (walk_gen_nr_data == gen_nr)
    return;

  walk_gen_nr_data = gen_nr;
  has_reachable_incoming_er_edge = false;
  in_candidate_set = false;
}

template<typename callable_type>
void function_definition_info::for_each_body_deps_on_types(callable_type &&c)
  const
{
  c(body_deps.on_types);
  for (const auto &d : _declarations_infos) {
    d.for_each_deps_on_types(c);
  }
}

template<typename callable_type>
void function_definition_info::for_each_deps_on_types(callable_type &&c) const
{
  c(this->declarator_deps);
  this->child_tags.for_each_deps(c);
  this->for_each_body_deps_on_types(c);
}


function_info::function_info()
  : _fdi(nullptr), can_externalize_valid(false), can_externalize(false),
    externalize(false), in_closure(false)
{}

object_info::object_info()
  : _initializing_id(nullptr), shall_externalize_valid(false),
    shall_externalize(false), externalize(false), in_closure(false)
{}


header_info::header_info(const pp_result::header_inclusion_root &root_node)
noexcept
  : _root_node(&root_node), parent(nullptr), eligible(true),
    only_directives(false), include(false)
{}

header_info::header_info(const pp_result::header_inclusion_child &child_node,
			 header_info &_parent) noexcept
  : _child_node(&child_node), parent(&_parent), eligible(true),
    only_directives(false), include(false)
{}

const pp_result::header_inclusion_root&
header_info::get_root_node() const noexcept
{
  assert(is_root());
  return *_root_node;
}

const pp_result::header_inclusion_child&
header_info::get_child_node() const noexcept
{
  assert(!is_root());
  return *_child_node;
}

const raw_pp_tokens_range& header_info::get_range() const noexcept
{
  return is_root() ? _root_node->get_range() : _child_node->get_range();
}


external_declaration_info::external_declaration_info(declaration_info &di)
  noexcept
  : k(kind::declaration), _di(&di)
{}

external_declaration_info::external_declaration_info(static_assert_info &sai)
  noexcept
  : k(kind::static_assertion), _sai(&sai)
{}

external_declaration_info::
external_declaration_info(function_definition_info &fdi) noexcept
  : k(kind::function_definition), _fdi(&fdi)
{}

const pp_tokens_range& external_declaration_info::get_tokens_range()
  const noexcept
{
  switch (k) {
  case kind::declaration:
    return _di->declaration.get_tokens_range();

  case kind::static_assertion:
    return _sai->static_assertion.get_tokens_range();

  case kind::function_definition:
    return _fdi->function_definition.get_tokens_range();
  }
}

declaration_info& external_declaration_info::get_declaration() noexcept
{
  assert(k == kind::declaration);
  return *_di;
}

static_assert_info& external_declaration_info::get_static_assert() noexcept
{
  assert(k == kind::static_assertion);
  return *_sai;
}

function_definition_info& external_declaration_info::get_function_definition()
  noexcept
{
  assert(k == kind::function_definition);
  return *_fdi;
}


ast_info::ast_info(const ast::ast_translation_unit &_atu)
  : atu(_atu), _last_walk_gen_nr(0)
{
  _init_header_infos(atu.get_pp_result());
}

unsigned int ast_info::start_walk() noexcept
{
  assert(!(_last_walk_gen_nr % 2));
  return ++_last_walk_gen_nr;
}

void ast_info::finish_walk() noexcept
{
  assert((_last_walk_gen_nr % 2));
  ++_last_walk_gen_nr;
}

template <typename parent_type, typename... tag_parent_args>
sou_def_info& ast_info::create_info(const ast::struct_or_union_def &soud,
				    parent_type &parent,
				    tag_parent_args&&... args)
{
  sou_info &soui = _create_info(soud.get_decl_list_node().get_id());
  std::unique_ptr<sou_def_info> soudi {
	new sou_def_info{soud,
			 tag_parent{parent,
				    std::forward<tag_parent_args>(args)...},
			 soui}
  };
  sou_def_info &ret = *soudi;
  assert(!soui._soud);
  soui._soud = std::move(soudi);
  parent.child_tags._souds.push_back(std::ref(ret));
  return ret;
}

template <typename parent_type, typename... tag_parent_args>
sou_ref_info& ast_info::create_info(const ast::struct_or_union_ref &sour,
				    parent_type &parent,
				    tag_parent_args&&... args)
{
  sou_info &soui = _create_info(sour.get_decl_list_node().get_id());
  std::unique_ptr<sou_ref_info> souri {
	new sou_ref_info{sour,
			 tag_parent{parent,
				    std::forward<tag_parent_args>(args)...},
			 soui}
  };
  sou_ref_info &ret = *souri;
  soui._sours.push_back(std::move(souri));
  parent.child_tags._sours.push_back(std::ref(ret));
  return ret;
}

template <typename parent_type, typename... tag_parent_args>
enum_def_info& ast_info::create_info(const ast::enum_def &ed,
				     parent_type &parent,
				     tag_parent_args&&... args)
{
  enum_info &ei = _create_info(ed.get_decl_list_node().get_id());
  std::unique_ptr<enum_def_info> edi {
	new enum_def_info{ed,
			  tag_parent{parent,
				     std::forward<tag_parent_args>(args)...},
			  ei}
  };
  enum_def_info &ret = *edi;
  assert(!ei._ed);
  ei._ed = std::move(edi);
  parent.child_tags._eds.push_back(std::ref(ret));
  return ret;
}

template <typename parent_type, typename... tag_parent_args>
enum_ref_info& ast_info::create_info(const ast::enum_ref &er,
				     parent_type &parent,
				     tag_parent_args&&... args)
{
  enum_info &ei = _create_info(er.get_decl_list_node().get_id());
  std::unique_ptr<enum_ref_info> eri {
	new enum_ref_info{er,
			  tag_parent{parent,
				     std::forward<tag_parent_args>(args)...},
			  ei}
  };
  enum_ref_info &ret = *eri;
  ei._ers.push_back(std::move(eri));
  parent.child_tags._ers.push_back(std::ref(ret));
  return ret;
}

declaration_info& ast_info::create_info(const ast::declaration &d)
{
  const auto it_di = _declarations_infos.emplace_back(d);
  _external_declaration_infos.emplace_back(*it_di);
  return *it_di;
}

static_assert_info& ast_info::create_info(const ast::static_assertion &sa)
{
  const auto it_sai = _static_asserts_infos.emplace_back(sa);
  _external_declaration_infos.emplace_back(*it_sai);
  return *it_sai;
}

declaration_info& ast_info::create_info(const ast::declaration &d,
					function_definition_info &enclosing_fd)
{
  const auto it_di =
    enclosing_fd._declarations_infos.emplace_back(d, &enclosing_fd);
  return *it_di;
}

function_info& ast_info::create_info(const ast::linkage::linkage_id &lid,
				     const function_tag&)
{
  auto it_fi = _lid_to_functions_infos.find(lid);
  if (it_fi == _lid_to_functions_infos.end()) {
    std::unique_ptr<function_info> fi {new function_info{}};
    it_fi = (_lid_to_functions_infos.insert(std::make_pair(lid,
							   std::move(fi)))
	     .first);
  }
  return *it_fi->second;
}

function_init_declarator_info&
ast_info::create_info(const ast::init_declarator &id,
		      declaration_info &enclosing_d,
		      const function_tag&)
{
  function_info &fi = create_info(id.get_linkage().get_id(), function_tag{});
  std::unique_ptr<function_init_declarator_info> fidi {
    new function_init_declarator_info{id, enclosing_d, fi}
  };
  function_init_declarator_info &ret = *fidi;

  fi._idis.push_back(std::move(fidi));

  const pp_token_index id_tok =
    id.get_declarator().get_direct_declarator_id().get_id_tok();
  const std::string &name =
    atu.get_pp_result().get_pp_tokens()[id_tok].get_value();
  _allocated_ids.insert(name);

  return ret;
}

object_info& ast_info::create_info(const ast::linkage::linkage_id &lid,
				   const object_tag&)
{
  auto it_oi = _lid_to_objects_infos.find(lid);
  if (it_oi == _lid_to_objects_infos.end()) {
    std::unique_ptr<object_info> oi {new object_info{}};
    it_oi = (_lid_to_objects_infos.insert(std::make_pair(lid,
							 std::move(oi)))
	     .first);
  }
  return *it_oi->second;
}

object_init_declarator_info&
ast_info::create_info(const ast::init_declarator &id,
		      declaration_info &enclosing_d,
		      const object_tag&)
{
  object_info &oi = create_info(id.get_linkage().get_id(), object_tag{});
  std::unique_ptr<object_init_declarator_info> oidi {
    new object_init_declarator_info{id, enclosing_d, oi}
  };
  object_init_declarator_info &ret = *oidi;

  if (id.get_initializer())
    oi._initializing_id = &ret;

  oi._idis.push_back(std::move(oidi));

  const pp_token_index id_tok =
    id.get_declarator().get_direct_declarator_id().get_id_tok();
  const std::string &name =
    atu.get_pp_result().get_pp_tokens()[id_tok].get_value();
  _allocated_ids.insert(name);

  return ret;
}

typedef_init_declarator_info&
ast_info::create_info(const ast::init_declarator &id,
		      declaration_info &enclosing_d, const typedef_tag&)
{
  const ast::linkage::linkage_id &lid = id.get_linkage().get_id();
  auto it_tdi = _lid_to_typedefs_infos.find(lid);
  if (it_tdi == _lid_to_typedefs_infos.end()) {
    std::unique_ptr<typedef_info> tdi{new typedef_info{}};
    it_tdi = (_lid_to_typedefs_infos.insert(std::make_pair(lid, std::move(tdi)))
	      .first);
  }

  std::unique_ptr<typedef_init_declarator_info> tdidi{
	new typedef_init_declarator_info{id, enclosing_d, *it_tdi->second}
  };
  typedef_init_declarator_info &ret = *tdidi;

  it_tdi->second->idis.push_back(std::move(tdidi));
  return ret;
}

function_definition_info& ast_info::
create_info(const ast::function_definition &fd)
{
  function_info &fi = create_info(fd.get_linkage().get_id(),
				  function_tag{});


  std::unique_ptr<function_definition_info> fdi {
	 new function_definition_info{fd, fi}
  };
  function_definition_info * const ret = fdi.get();
  fi._fdi = std::move(fdi);
  _external_declaration_infos.emplace_back(*ret);

  const pp_token_index id_tok =
    fd.get_declarator().get_direct_declarator_id().get_id_tok();
  const std::string &name =
    atu.get_pp_result().get_pp_tokens()[id_tok].get_value();
  _allocated_ids.insert(name);

  return *ret;
}

function_info& ast_info::
get_function_info_by_lid(const ast::linkage::linkage_id &lid) noexcept
{
  auto it_fi = _lid_to_functions_infos.find(lid);
  assert(it_fi != _lid_to_functions_infos.end());
  return *it_fi->second;
}

object_info& ast_info::
get_object_info_by_lid(const ast::linkage::linkage_id &lid) noexcept
{
  auto it_oi = _lid_to_objects_infos.find(lid);
  assert(it_oi != _lid_to_objects_infos.end());
  return *it_oi->second;
}

sou_info* ast_info::get_sou_info_by_id(const ast::sou_id &sou_id) noexcept
{
  auto it = _sou_ids_to_infos.find(sou_id);
  if (it == _sou_ids_to_infos.end())
    return nullptr;

  return &*it->second;
}

enum_info* ast_info::get_enum_info_by_id(const ast::enum_id &enum_id)
  noexcept
{
  auto it = _enum_ids_to_infos.find(enum_id);
  if (it == _enum_ids_to_infos.end())
    return nullptr;

  return &*it->second;
}


void ast_info::
register_sym_mod(const lp_creation_policy::symbol_modification &sym_mod)
  noexcept
{
  if (!sym_mod.is_rename())
    return;

  assert(!_allocated_ids.count(sym_mod.new_name));
  _allocated_ids.insert(sym_mod.new_name);
}


ast_info::external_declaration_iterator
ast_info::intersecting_external_declarations_begin(const raw_pp_tokens_range &r)
  noexcept
{
  return
    (std::lower_bound
     (_external_declaration_infos.begin(),
      _external_declaration_infos.end(),
      r,
      [&](const external_declaration_info &edi, const raw_pp_tokens_range &r) {
	const raw_pp_tokens_range &edi_range =
	  atu.get_pp_result().pp_tokens_range_to_raw(edi.get_tokens_range());
	return edi_range < r;
      }));
}

ast_info::external_declaration_iterator
ast_info::intersecting_external_declarations_end(const raw_pp_tokens_range &r)
  noexcept
{
  return
    (std::upper_bound
     (_external_declaration_infos.begin(),
      _external_declaration_infos.end(),
      r,
      [&](const raw_pp_tokens_range &r, const external_declaration_info &edi) {
	const raw_pp_tokens_range &edi_range =
	  atu.get_pp_result().pp_tokens_range_to_raw(edi.get_tokens_range());
	return r < edi_range;
      }));
}

void ast_info::mark_header_uneligible(header_info &hi) noexcept
{
  for (const pp_result::header_inclusion_node *cur =
	 (hi.is_root() ?
	  &(static_cast<const pp_result::header_inclusion_node&>
	    (hi.get_root_node())) :
	  &hi.get_child_node());
       cur;
       cur = (cur->get_parent() ?
	      &cur->get_parent()->get_containing_header() : nullptr)) {
    this->header_infos[cur->get_id()].eligible = false;
  }
}


void ast_info::_init_header_infos(const pp_result &ppr)
{
  header_infos.reserve(ppr.get_header_ids_end());

  // Traverse the headers in exactly the same order as the
  // preprocessor assigns the ids to them such that each header_info
  // ends up in the slot indexed by its id.
  for (auto &hr : ppr.get_header_inclusion_roots()) {
    assert(hr.get_id() == header_infos.size());
    header_infos.emplace_back(hr);

    const auto childs_end = hr.header_inclusions_recursive_end();
    for (auto it_child = hr.header_inclusions_recursive_begin();
	 it_child != childs_end; ++it_child) {
      assert(it_child->get_id() == header_infos.size());
      assert(it_child->get_parent());
      const pp_result::header_inclusion_node &parent =
	it_child->get_parent()->get_containing_header();
      assert(parent.get_id() < header_infos.size());

      header_infos.emplace_back(*it_child, header_infos[parent.get_id()]);
    }
  }
}

sou_info& ast_info::_create_info(const ast::sou_id &sou_id)
{
  auto it = _sou_ids_to_infos.find(sou_id);
  if (it == _sou_ids_to_infos.end()) {
    std::unique_ptr<sou_info> soui{new sou_info{}};
    it = (_sou_ids_to_infos.insert(std::make_pair(sou_id, std::move(soui)))
	  .first);
  }
  return *it->second;
}

enum_info& ast_info::_create_info(const ast::enum_id &enum_id)
{
  auto it = _enum_ids_to_infos.find(enum_id);
  if (it == _enum_ids_to_infos.end()) {
    std::unique_ptr<enum_info> ei{new enum_info{}};
    it = (_enum_ids_to_infos.insert(std::make_pair(enum_id, std::move(ei)))
	  .first);
  }
  return *it->second;
}


namespace
{
  class _ast_info_collector
  {
  public:
    _ast_info_collector(ast_info &ai, code_remarks &remarks) noexcept;

    void operator()();

  private:
    void _require_complete_type(const types::type &t);
    void _require_complete_pointed_to_type(const types::type &t);
    void _require_complete_function_ret_and_param_types
					(const types::addressable_type &t);

    static bool _type_involves_complete_arrays(const types::type &t);

    void _enter_init_declarator(const ast::init_declarator &id);

    template <typename tag_info_type, typename ast_entity_type>
    tag_info_type& _create_tag_info(const ast_entity_type &a);

    void _enter_soud(const ast::struct_or_union_def &soud);
    void _enter_ed(const ast::enum_def &ed);
    void _leave_souoed();

    bool _handle_expr(const ast::expr_sizeof_expr &e);
    bool _handle_expr(const ast::expr_alignof_expr &e);
    void _handle_expr(const ast::expr_sizeof_type_name &e);
    void _handle_expr(const ast::expr_alignof_type_name &e);
    bool _handle_expr(const ast::expr_generic &e);
    bool _handle_expr(const ast::expr_builtin_choose_expr &e);
    void _handle_expr(const ast::expr_builtin_offsetof &e);
    void _handle_expr(const ast::expr_member_deref &e);
    void _handle_expr(const ast::expr_cast &e);
    void _handle_expr(const ast::expr_compound_literal &e);
    void _handle_expr(const ast::expr_func_invocation &e);
    void _handle_expr(const ast::expr_unop_pre &e);
    void _handle_expr(const ast::expr_unop_post &e);
    bool _handle_expr(const ast::expr_binop &e);
    void _handle_expr(const ast::expr_assignment &e);
    void _handle_expr(const ast::expr_array_subscript &e);
    bool _handle_expr(const ast::expr_conditional &e);
    void _handle_expr(const ast::expr_id &e);

    void _handle_stmt(const ast::stmt_expr &s);
    void _handle_stmt(const ast::stmt_if &s);
    void _handle_stmt(const ast::stmt_switch &s);
    void _handle_stmt(const ast::stmt_while &s);
    void _handle_stmt(const ast::stmt_do &s);
    void _handle_stmt(const ast::stmt_for_init_expr &s);
    void _handle_stmt(const ast::stmt_for_init_decl &s);
    void _handle_stmt(const ast::stmt_for_init_static_assert &s);
    void _handle_stmt(const ast::stmt_return &s);
    void _handle_asm_stmt_op(const ast::asm_operand &o);

    void _handle_array_declarator(const types::addressable_type &t);

    void _handle_sou_ref(const ast::struct_or_union_ref &sour);
    void _handle_enum_ref(const ast::enum_ref &er);
    void _handle_typedef_ref(const ast::type_specifier_tdid &ts_tdid);

    typedef std::vector<std::reference_wrapper<const ast::_ast_entity>>
	_unevaluated_stack_entry;

    void _push_unevaluated(_unevaluated_stack_entry &&u);
    void _push_unevaluated(const ast::_ast_entity &e);
    void _pop_unevaluated();
    bool _is_evaluated(const ast::expr &e) const noexcept;

    enum class _constexpr_bool_value
    {
      unknown,
      nonzero,
      zero
    };

    static _constexpr_bool_value _expr_to_bool(const ast::expr &e) noexcept;

    static bool _is_non_local_linkage(const ast::linkage &l) noexcept;

    ast_info &_ai;
    code_remarks &_remarks;

    declaration_info *_cur_declaration;
    static_assert_info *_cur_static_assert;
    function_definition_info *_cur_function_definition;

    function_init_declarator_info *_cur_fun_init_declarator;
    object_init_declarator_info *_cur_obj_init_declarator;
    initializer_info *_cur_initializer;
    typedef_init_declarator_info *_cur_td_init_declarator;

    deps_on_types *_cur_deps_on_types;
    deps_on *_cur_deps;
    std::size_t _in_parameter_declaration_list;

    std::stack<_unevaluated_stack_entry> _unevaluated_stack;

    struct _souoed_stack_entry
    {
      _souoed_stack_entry(sou_def_info &soudi,
			  deps_on_types &_saved_deps_on_types) noexcept;
      _souoed_stack_entry(enum_def_info &edi,
			  deps_on_types &_saved_deps_on_types) noexcept;

      enum class kind
      {
	k_soud,
	k_ed,
      };

      const kind k;

      union
      {
	sou_def_info *soud;
	enum_def_info *ed;
      };

      std::reference_wrapper<deps_on_types> saved_deps_on_types;
    };

    std::stack<_souoed_stack_entry> _souoed_stack;
    deps_on *_souoed_saved_deps;

    std::map<const ast::struct_or_union_def *,
	     std::reference_wrapper<sou_def_info>> _soud_to_soudi;
    std::map<const ast::struct_or_union_ref *,
	     std::reference_wrapper<sou_ref_info>> _sour_to_souri;
    std::map<const ast::enum_def *,
	     std::reference_wrapper<enum_def_info>> _ed_to_edi;
    std::map<const ast::enum_ref *,
	     std::reference_wrapper<enum_ref_info>> _er_to_eri;
    std::map<const ast::function_definition *,
	     std::reference_wrapper<function_definition_info>> _fd_to_fdi;
    std::map<const ast::init_declarator *,
	     std::reference_wrapper<function_init_declarator_info>> _id_to_fidi;
    std::map<const ast::init_declarator *,
	     std::reference_wrapper<object_init_declarator_info>> _id_to_oidi;
    std::map<const ast::direct_declarator_id *,
	     std::reference_wrapper<typedef_init_declarator_info>>
    _ddid_to_tdidi;
  };
}

_ast_info_collector::_ast_info_collector(ast_info &ai, code_remarks &remarks)
  noexcept
  : _ai(ai), _remarks(remarks),
    _cur_declaration(nullptr),
    _cur_static_assert(nullptr),
    _cur_function_definition(nullptr),
    _cur_fun_init_declarator(nullptr),
    _cur_obj_init_declarator(nullptr),
    _cur_initializer(nullptr),
    _cur_td_init_declarator(nullptr),
    _cur_deps_on_types(nullptr), _cur_deps(nullptr),
    _in_parameter_declaration_list(0),
    _souoed_saved_deps(nullptr)
{}


void _ast_info_collector::operator()()
{
  auto &&pre =
    (wrap_callables<default_action_return_value<bool, false>::type>
     ([this](const ast::function_definition &fd) {
	assert(!_cur_declaration);
	assert(!_cur_static_assert);
	assert(!_cur_obj_init_declarator && !_cur_fun_init_declarator &&
	       !_cur_td_init_declarator);
	if (_cur_function_definition) {
	  assert(!_is_non_local_linkage(fd.get_linkage()));
	  _require_complete_function_ret_and_param_types
		(*fd.get_declarator().get_innermost_type());
	  return false;
	}

	assert(_is_non_local_linkage(fd.get_linkage()));
	assert(!_cur_deps_on_types);
	assert(!_cur_deps);
	_cur_function_definition = &_ai.create_info(fd);
	_fd_to_fdi.insert(std::make_pair(&fd,
					 std::ref(*_cur_function_definition)));
	_cur_deps_on_types = &_cur_function_definition->declarator_deps;
	return true;
      },
      [this](const ast::declaration_list&) {
	if (!_cur_deps) {
	  assert(_cur_function_definition);
	  _cur_deps = &_cur_function_definition->body_deps;
	  _cur_deps_on_types = &_cur_deps->on_types;
	  return true;
	}
	return false;
      },
      [this](const ast::stmt_compound &s) {
	if (!_cur_deps) {
	  assert(_cur_function_definition);
	  // The function's return and parameter types must all be
	  // complete in the function body.
	  _cur_deps_on_types = &_cur_function_definition->body_deps.on_types;
	  s.process_parent
	    (wrap_callables<default_action_unreachable<void, type_set<> >::type>
	     ([&](const ast::function_definition &fd) {
		_require_complete_function_ret_and_param_types
			(*fd.get_declarator().get_innermost_type());
	      }));

	  _cur_deps = &_cur_function_definition->body_deps;
	  return true;
	}
	return false;
      },
      [this](const ast::parameter_declaration_list&) {
	++_in_parameter_declaration_list;
	return true;
      },
      [this](const ast::declaration &d) {
	assert(!_cur_declaration);
	assert(!_cur_static_assert);
	assert(!_cur_fun_init_declarator && !_cur_obj_init_declarator &&
	       !_cur_td_init_declarator);
	if (_cur_function_definition) {
	  // A declaration at function scope.
	  // See if there's an 'extern' or 'static' storage class
	  // specifier.
	  const ast::storage_class_specifier * const scs =
	    _find_scs(d.get_declaration_specifiers());
	  if (scs &&
	      (scs->get_storage_class() == ast::storage_class::sc_extern ||
	       scs->get_storage_class() == ast::storage_class::sc_static)) {
	    assert(_cur_deps == &_cur_function_definition->body_deps);
	    assert(_cur_deps_on_types == &_cur_deps->on_types);
	    _cur_declaration = &_ai.create_info(d, *_cur_function_definition);
	    _cur_deps_on_types =
	      &_cur_declaration->declaration_specifiers_deps;
	    _cur_deps = nullptr;
	    return true;
	  }
	  return false;
	}

	_cur_declaration = &_ai.create_info(d);
	assert(!_cur_deps_on_types);
	assert(!_cur_deps);
	_cur_deps_on_types = &_cur_declaration->declaration_specifiers_deps;
	return true;
      },
      [this](const ast::static_assertion &sa) {
	// If there's a containing function definition or declaration,
	// use that for dependency recording. That is, only
	// create records for _Static_assert() at file scope.
	if (_cur_declaration || _cur_function_definition)
	  return false;

	_cur_static_assert = &_ai.create_info(sa);
	assert(!_cur_deps_on_types);
	assert(!_cur_deps);
	_cur_deps_on_types = &_cur_static_assert->deps;
	return true;
      },
      [this](const ast::init_declarator &id) {
	if (_cur_function_definition) {
	  // The init_declarator can still have extern or static storage class.
	  // If so, make sure it gets registered below.
	  if (!_cur_declaration) {
	    _require_complete_type(*id.get_declarator().get_innermost_type());
	    return false;
	  }
	}

	_enter_init_declarator(id);

	return true;

      },
      [this](const ast::initializer_expr &ie) {
	if (_cur_obj_init_declarator && !_cur_initializer) {
	  _cur_initializer =
	    &_cur_obj_init_declarator->create_initializer_info();
	  _cur_deps = &_cur_initializer->deps;
	  _cur_deps_on_types = &_cur_deps->on_types;

	  _require_complete_type(*ie.get_expr().get_type());
	  return true;
	}

	_require_complete_type(*ie.get_expr().get_type());
	return false;
      },
      [this](const ast::initializer&) {
	if (_cur_obj_init_declarator && !_cur_initializer) {
	  _cur_initializer =
	    &_cur_obj_init_declarator->create_initializer_info();
	  _cur_deps = &_cur_initializer->deps;
	  _cur_deps_on_types = &_cur_deps->on_types;
	  return true;
	}

	return false;
      },
      [](const ast::struct_declarator &) {
	return true;
      },
      [&](const ast::struct_or_union_def &soud) {
	if ((_cur_function_definition && _cur_deps) ||
	    _in_parameter_declaration_list ||
	    !soud.has_id()) {
	  // The struct or union is either not defined at global scope
	  // or there's no tag. Either way, it can't get referred to
	  // and there's no need to build this definition's
	  // dependencies separately.
	  return false;
	}

	_enter_soud(soud);
	return true;
      },
      [&](const ast::enum_def &ed) {
	if ((_cur_function_definition && _cur_deps) ||
	    _in_parameter_declaration_list) {
	  // The enum is not defined at global scope. It can't get
	  // referred to and there's no need to build this
	  // definition's dependencies separately.
	  return false;
	}

	_enter_ed(ed);
	return true;
      },
      [this](const ast::typeof_expr &e) {
	_push_unevaluated(e.get_expr());
	return true;
      },
      [this](const ast::expr_sizeof_expr &e) {
	return _handle_expr(e);
      },
      [this](const ast::expr_alignof_expr &e) {
	return _handle_expr(e);
      },
      [this](const ast::expr_sizeof_type_name &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_alignof_type_name &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_generic &e) {
	return _handle_expr(e);
      },
      [this](const ast::expr_builtin_choose_expr &e) {
	return _handle_expr(e);
      },
      [this](const ast::expr_builtin_offsetof &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_member_deref &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_cast &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_compound_literal &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_func_invocation &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_unop_pre &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_unop_post &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_binop &e) {
	return _handle_expr(e);
      },
      [this](const ast::expr_assignment &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_array_subscript &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::expr_conditional &e) {
	return _handle_expr(e);
      },
      [this](const ast::expr_id &e) {
	_handle_expr(e);
	return false;
      },
      [this](const ast::stmt_expr &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::stmt_if &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::stmt_switch &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::stmt_while &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::stmt_do &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::stmt_for_init_expr &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::stmt_for_init_decl &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::stmt_for_init_static_assert &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::stmt_return &s) {
	_handle_stmt(s);
	return false;
      },
      [this](const ast::asm_operand &o) {
	_handle_asm_stmt_op(o);
	return false;
      },
      [this](const ast::direct_declarator_array &dda) {
	_handle_array_declarator(*dda.get_type());
	return false;
      },
      [this](const ast::direct_abstract_declarator_array &dada) {
	_handle_array_declarator(*dada.get_type());
	return false;
      },
      [this](const ast::struct_or_union_ref &sour) {
	_handle_sou_ref(sour);
	return false;
      },
      [this](const ast::enum_ref &er) {
	_handle_enum_ref(er);
	return false;
      },
      [this](const ast::type_specifier_tdid &ts_tdid) {
	_handle_typedef_ref(ts_tdid);
	return false;
      }));

  auto &&post =
    (wrap_callables<default_action_unreachable<void, type_set<>>::type>
     ([this](const ast::function_definition&) {
	assert(_cur_function_definition);
	assert(!_cur_declaration);
	assert(!_cur_deps);
	_cur_deps_on_types = nullptr;
	_cur_function_definition = nullptr;
      },
      [this](const ast::declaration_list&) {
	assert(_cur_function_definition);
	assert(_cur_deps == &_cur_function_definition->body_deps);
	_cur_deps = nullptr;
	_cur_deps_on_types = &_cur_function_definition->declarator_deps;
      },
      [this](const ast::stmt_compound&) {
	assert(_cur_function_definition);
	assert(!_cur_declaration);
	assert(_cur_deps == &_cur_function_definition->body_deps);
	_cur_deps = nullptr;
	_cur_deps_on_types = &_cur_function_definition->declarator_deps;
      },
      [this](const ast::parameter_declaration_list&) {
	--_in_parameter_declaration_list;
      },
      [this](const ast::declaration&) {
	assert(_cur_declaration);
	assert(!_cur_deps);
	if (!_cur_function_definition) {
	  _cur_deps_on_types = nullptr;
	} else {
	  _cur_deps = &_cur_function_definition->body_deps;
	  _cur_deps_on_types = &_cur_deps->on_types;
	}
	_cur_declaration = nullptr;
      },
      [this](const ast::static_assertion&) {
	assert(_cur_static_assert);
	assert(!_cur_deps);
	_cur_deps_on_types = nullptr;
	_cur_static_assert = nullptr;
      },
      [this](const ast::init_declarator&) {
	assert((!!_cur_obj_init_declarator ^ !!_cur_fun_init_declarator ^
		!!_cur_td_init_declarator) &&
	       !(_cur_obj_init_declarator && _cur_fun_init_declarator &&
		 _cur_td_init_declarator));
	assert(!_cur_deps);
	_cur_deps_on_types = nullptr;
	_cur_fun_init_declarator = nullptr;
	_cur_obj_init_declarator = nullptr;
	_cur_td_init_declarator = nullptr;
      },
      [this](const ast::initializer_expr&) {
	_cur_initializer = nullptr;
	assert(_cur_obj_init_declarator);
	_cur_deps = nullptr;
	_cur_deps_on_types = &_cur_obj_init_declarator->declarator_deps;
      },
      [this](const ast::initializer&) {
	_cur_initializer = nullptr;
	assert(_cur_obj_init_declarator);
	_cur_deps = nullptr;
	_cur_deps_on_types = &_cur_obj_init_declarator->declarator_deps;
      },
      [this](const ast::typeof_expr&) {
	_pop_unevaluated();
      },
      [this](const ast::expr_sizeof_expr&) {
	_pop_unevaluated();
      },
      [this](const ast::expr_alignof_expr&) {
	_pop_unevaluated();
      },
      [this](const ast::expr_generic&) {
	_pop_unevaluated();
      },
      [this](const ast::expr_builtin_choose_expr&) {
	_pop_unevaluated();
      },
      [this](const ast::expr_binop&) {
	_pop_unevaluated();
      },
      [this](const ast::expr_conditional&) {
	_pop_unevaluated();
      },
      [&](const ast::struct_declarator &sd) {
	const ast::declarator * const d = sd.get_declarator();
	if (!d)
	  return;

	// For bitfields, this will add dependency to the underlying
	// integer type, which is what is needed in case that's an
	// enum type.
	_require_complete_type(*d->get_innermost_type());
      },
      [&](const ast::struct_or_union_def&) {
	_leave_souoed();
      },
      [&](const ast::enum_def&) {
	_leave_souoed();
      }));

  _ai.atu.for_each_dfs_pre_and_po
    <type_set<const ast::function_definition,
	      const ast::declaration_list,
	      const ast::stmt_compound,
	      const ast::parameter_declaration_list,
	      const ast::declaration,
	      const ast::static_assertion,
	      const ast::init_declarator,
	      const ast::initializer_expr,
	      const ast::initializer,
	      const ast::struct_declarator,
	      const ast::struct_or_union_def,
	      const ast::enum_def,
	      const ast::typeof_expr,
	      const ast::expr_sizeof_expr,
	      const ast::expr_alignof_expr,
	      const ast::expr_sizeof_type_name,
	      const ast::expr_alignof_type_name,
	      const ast::expr_generic,
	      const ast::expr_builtin_choose_expr,
	      const ast::expr_builtin_offsetof,
	      const ast::expr_member_deref,
	      const ast::expr_cast,
	      const ast::expr_compound_literal,
	      const ast::expr_func_invocation,
	      const ast::expr_unop_pre,
	      const ast::expr_unop_post,
	      const ast::expr_binop,
	      const ast::expr_assignment,
	      const ast::expr_array_subscript,
	      const ast::expr_conditional,
	      const ast::expr_id,
	      const ast::stmt_expr,
	      const ast::stmt_if,
	      const ast::stmt_switch,
	      const ast::stmt_while,
	      const ast::stmt_do,
	      const ast::stmt_for_init_expr,
	      const ast::stmt_for_init_decl,
	      const ast::stmt_for_init_static_assert,
	      const ast::stmt_return,
	      const ast::asm_operand,
	      const ast::direct_declarator_array,
	      const ast::direct_abstract_declarator_array,
	      const ast::struct_or_union_ref,
	      const ast::enum_ref,
	      const ast::type_specifier_tdid>,
     type_set<const ast::function_definition,
	      const ast::declaration_list,
	      const ast::stmt_compound,
	      const ast::parameter_declaration_list,
	      const ast::declaration,
	      const ast::static_assertion,
	      const ast::init_declarator,
	      const ast::initializer_expr,
	      const ast::initializer,
	      const ast::typeof_expr,
	      const ast::expr_sizeof_expr,
	      const ast::expr_alignof_expr,
	      const ast::expr_generic,
	      const ast::expr_builtin_choose_expr,
	      const ast::expr_binop,
	      const ast::expr_conditional,
	      const ast::struct_declarator,
	      const ast::struct_or_union_def,
	      const ast::enum_def>>
    (std::move(pre), std::move(post));
}

void _ast_info_collector::
_require_complete_type(const types::type &t)
{
  assert(_cur_deps_on_types);

  types::handle_types<void>
    ((wrap_callables<default_action_nop>
      ([this](const types::struct_or_union_type &sout) {
	 const ast::sou_decl_list_node * const decl_node =
	   sout.get_decl_list_node();
	 assert(decl_node);

	 const ast::struct_or_union_def *soud = decl_node->find_definition();
	 if (!soud) {
	   // The AST evaluation has emitted a warning/error already.
	   return;
	 }

	 const auto it_soud = _soud_to_soudi.find(soud);
	 if (it_soud == _soud_to_soudi.end()) {
	   // No registered info means the definition is not global.
	   return;
	 }

	 _cur_deps_on_types->on_sou_defs.add
	   (dep_on_sou_def{it_soud->second.get()});

       },
       [this](const types::enum_type &et) {
	 const ast::enum_decl_list_node &decl_node = et.get_decl_list_node();

	 const ast::enum_def *ed = decl_node.find_definition();
	 if (!ed) {
	   // The AST evaluation has emitted a warning/error already.
	   return;
	 }

	 const auto it_ed = _ed_to_edi.find(ed);
	 if (it_ed == _ed_to_edi.end()) {
	   // No registered info means the definition is not global.
	   return;
	 }

	 _cur_deps_on_types->on_enum_defs.add
	   (dep_on_enum_def{it_ed->second.get()});

       })),
     t);
}

void _ast_info_collector::
_require_complete_pointed_to_type(const types::type &t)
{
  types::handle_types<void>
    ((wrap_callables<default_action_nop>
      ([this](const types::pointer_type &pt) {
	 _require_complete_type(*pt.get_pointed_to_type());
       })),
     t);
}

void _ast_info_collector::_require_complete_function_ret_and_param_types
					(const types::addressable_type &t)
{
  types::handle_types<void>
    ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
      ([&](const types::prototyped_function_type &pft) {
	 _require_complete_type(*pft.get_return_type());

	 const types::parameter_type_list &ptl = pft.get_parameter_type_list();
	 for (const auto &p : ptl) {
	   _require_complete_type(*p);
	 }

       },
	[&](const types::unprototyped_function_type &upft) {
	  _require_complete_type(*upft.get_return_type());

	})),
     t);
}

bool _ast_info_collector::_type_involves_complete_arrays(const types::type &t)
{
  const types::type *cur_t = &t;
  bool result = false;
  bool last_was_array = false;

  while (cur_t) {
    cur_t =
      (types::handle_types<const types::type*>
       ((wrap_callables<default_action_return_value<const types::type*,
						    nullptr>::type>
	 ([&](const types::pointer_type &pt) {
	    last_was_array = false;
	    return pt.get_pointed_to_type().get();
	  },
	  [&](const types::function_type &ft) {
	    last_was_array = false;
	    return ft.get_return_type().get();
	  },
	  [&](const types::array_type &at) -> const types::type* {
	    // Inner arrays are always complete.
	    if (last_was_array) {
	      assert(at.is_complete());
	      return at.get_element_type().get();
	    }
	    if (at.is_length_constant()) {
	      result = true;
	      return nullptr;
	    } else {
	      last_was_array = true;
	      return at.get_element_type().get();
	    }
	  })),
	*cur_t));
  }

  return result;
}

void
_ast_info_collector::_enter_init_declarator(const ast::init_declarator &id)
{
  assert(_cur_declaration);
  assert(!_cur_fun_init_declarator && !_cur_obj_init_declarator &&
	 !_cur_td_init_declarator);
  assert(!_cur_deps);

  if (_cur_declaration->storage_class_specifier &&
      (_cur_declaration->storage_class_specifier->get_storage_class() ==
       ast::storage_class::sc_typedef)) {
    _cur_td_init_declarator =
      &_ai.create_info(id, *_cur_declaration, ast_info::typedef_tag{});
    _ddid_to_tdidi.insert
      (std::make_pair(&id.get_declarator().get_direct_declarator_id(),
		      std::ref(*_cur_td_init_declarator)));
    _cur_deps_on_types = &_cur_td_init_declarator->declarator_deps;

  } else {
    assert((_cur_declaration->storage_class_specifier &&
	    (_cur_declaration->storage_class_specifier->get_storage_class() ==
	     ast::storage_class::sc_register)) ||
	   _is_non_local_linkage(id.get_linkage()) ||
	   (_cur_declaration->enclosing_fd &&
	    _cur_declaration->storage_class_specifier &&
	    (_cur_declaration->storage_class_specifier->get_storage_class() ==
	     ast::storage_class::sc_static)));

    const types::addressable_type &t =
      *id.get_declarator().get_innermost_type();
    if (types::is_type<types::function_type>(t)) {
      _cur_fun_init_declarator =
	&_ai.create_info(id, *_cur_declaration, ast_info::function_tag{});
      _id_to_fidi.insert(std::make_pair(&id,
					std::ref(*_cur_fun_init_declarator)));
      _cur_deps_on_types = &_cur_fun_init_declarator->declarator_deps;

    } else {
      _cur_obj_init_declarator =
	&_ai.create_info(id, *_cur_declaration, ast_info::object_tag{});
      _id_to_oidi.insert(std::make_pair(&id,
					std::ref(*_cur_obj_init_declarator)));

      if (_cur_declaration->enclosing_fd &&
	  _cur_declaration->storage_class_specifier &&
	  (_cur_declaration->storage_class_specifier->get_storage_class() ==
	   ast::storage_class::sc_static)) {
	// Force a dependency on the local static variable just in
	// case no expr_id from the function references it.
	_cur_function_definition
	  ->body_deps.on_objs.add(dep_on_obj{*_cur_obj_init_declarator});
      }

      _cur_deps_on_types = &_cur_obj_init_declarator->declarator_deps;

    }
  }
}

template <typename tag_info_type, typename ast_entity_type>
tag_info_type& _ast_info_collector::
_create_tag_info(const ast_entity_type &ast_entity)
{
  if (!_souoed_stack.empty()) {
    const _souoed_stack_entry &top = _souoed_stack.top();
    switch (top.k) {
    case _souoed_stack_entry::kind::k_soud:
      return _ai.create_info(ast_entity, *top.soud);
    case _souoed_stack_entry::kind::k_ed:
      return _ai.create_info(ast_entity, *top.ed);
    }

  } else if (_cur_function_definition) {
    return _ai.create_info(ast_entity, *_cur_function_definition);

  } else if (_cur_fun_init_declarator) {
    return _ai.create_info(ast_entity, *_cur_fun_init_declarator);

  } else if (_cur_obj_init_declarator) {
    if (_cur_initializer) {
      return _ai.create_info(ast_entity, *_cur_initializer);
    } else {
      return _ai.create_info(ast_entity, *_cur_obj_init_declarator);
    }
  } else if (_cur_td_init_declarator) {
    return _ai.create_info(ast_entity, *_cur_td_init_declarator);

  } else if (_cur_declaration) {
    return _ai.create_info(ast_entity, *_cur_declaration);

  } else {
    assert(_cur_static_assert);
    return _ai.create_info(ast_entity, *_cur_static_assert);

  }
}

void _ast_info_collector::_enter_soud(const ast::struct_or_union_def &soud)
{
  sou_def_info &soudi = _create_tag_info<sou_def_info>(soud);

  assert(_soud_to_soudi.find(&soud) == _soud_to_soudi.end());
  _soud_to_soudi.insert(std::make_pair(&soud, std::ref(soudi)));

  if (_souoed_stack.empty()) {
    assert(!_souoed_saved_deps);
    _souoed_saved_deps = _cur_deps;
    _cur_deps = nullptr;
  } else {
    assert(!_cur_deps);
  }

  assert(_cur_deps_on_types);
  _souoed_stack.emplace(soudi, *_cur_deps_on_types);
  _cur_deps_on_types = &soudi.deps;
}

void _ast_info_collector::_enter_ed(const ast::enum_def &ed)
{
  enum_def_info &edi = _create_tag_info<enum_def_info>(ed);

  assert(_ed_to_edi.find(&ed) == _ed_to_edi.end());
  _ed_to_edi.insert(std::make_pair(&ed, std::ref(edi)));

  if (_souoed_stack.empty()) {
    assert(!_souoed_saved_deps);
    _souoed_saved_deps = _cur_deps;
    _cur_deps = nullptr;
  } else {
    assert(!_cur_deps);
  }

  assert(_cur_deps_on_types);
  _souoed_stack.emplace(edi, *_cur_deps_on_types);
  _cur_deps_on_types = &edi.deps;
}

void _ast_info_collector::_leave_souoed()
{
  assert(!_cur_deps);
  assert(!_souoed_stack.empty());
  _cur_deps_on_types = &_souoed_stack.top().saved_deps_on_types.get();
  _souoed_stack.pop();
  if (_souoed_stack.empty()) {
    _cur_deps = _souoed_saved_deps;
    _souoed_saved_deps = nullptr;
  }
}

bool _ast_info_collector::_handle_expr(const ast::expr_sizeof_expr &e)
{
  _require_complete_type(*e.get_type());
  _push_unevaluated(e);
  return true;
}

bool  _ast_info_collector::_handle_expr(const ast::expr_alignof_expr &e)
{
  _require_complete_type(*e.get_type());
  _push_unevaluated(e);
  return true;
}

void _ast_info_collector::_handle_expr(const ast::expr_sizeof_type_name &e)
{
  _require_complete_type(*e.get_type_name().get_type());
}

void _ast_info_collector::_handle_expr(const ast::expr_alignof_type_name &e)
{
  _require_complete_type(*e.get_type_name().get_type());
}

bool _ast_info_collector::_handle_expr(const ast::expr_generic &e)
{
  _require_complete_type(*e.get_control_expr().get_type());

  if (!_is_evaluated(e))
    return false;

  _unevaluated_stack_entry unevaluated = {
    std::cref<ast::_ast_entity>(e.get_control_expr())
  };
  const ast::expr &match = e.get_match();
  e.get_association_list().for_each
    ([&](const ast::generic_association &ga) {
      const ast::type_name * const tn = ga.get_type_name();

      if (tn)
	_require_complete_type(*tn->get_type());

      const ast::expr &e = ga.get_expr();
      if (&e != &match)
	unevaluated.push_back(std::cref<ast::_ast_entity>(e));
    });

  if (unevaluated.empty())
    return false;

  _push_unevaluated(std::move(unevaluated));
  return true;
}

bool _ast_info_collector::_handle_expr(const ast::expr_builtin_choose_expr &e)
{
  if (!_is_evaluated(e))
    return false;

  switch (_expr_to_bool(e.get_cond())) {
  case _constexpr_bool_value::unknown:
    assert(0);
    return false;

  case _constexpr_bool_value::nonzero:
    _push_unevaluated(e.get_expr_false());
    break;

  case _constexpr_bool_value::zero:
    _push_unevaluated(e.get_expr_true());
    break;
  };

  return true;
}

void _ast_info_collector::_handle_expr(const ast::expr_builtin_offsetof &e)
{
  // Requiring an outermost complete type implies completeness of all
  // the inner ones.
  _require_complete_type(*e.get_type_name().get_type());
}

void _ast_info_collector::_handle_expr(const ast::expr_member_deref &e)
{
  const types::type &base_type = *e.get_base().get_type();
  if (e.get_deref_type() == ast::member_deref_type::ptr_base) {
    _require_complete_pointed_to_type(base_type);
  } else {
    _require_complete_type(base_type);
  }
}

void _ast_info_collector::_handle_expr(const ast::expr_cast &e)
{
  _require_complete_type(*e.get_type_name().get_type());
  _require_complete_type(*e.get_expr().get_type());
}

void _ast_info_collector::_handle_expr(const ast::expr_compound_literal &e)
{
  _require_complete_type(*e.get_type_name().get_type());
}

void _ast_info_collector::_handle_expr(const ast::expr_func_invocation &e)
{
  // For a function invocation, the compiler will have to set things
  // up on the stack etc. and the return types as well as the
  // parameter types must all be complete.
  types::handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const types::pointer_type &pt) {
	 _require_complete_function_ret_and_param_types
					(*pt.get_pointed_to_type());
       })),
     *e.get_func().get_type());
}

void _ast_info_collector::_handle_expr(const ast::expr_unop_pre &e)
{
  switch (e.get_op()) {
  case ast::unary_op_pre::inc:
    /* fall through */
  case ast::unary_op_pre::dec:
    /* fall through */
  case ast::unary_op_pre::deref:
    _require_complete_pointed_to_type(*e.get_expr().get_type());
    break;

  case ast::unary_op_pre::addr:
    /* fall through */
  case ast::unary_op_pre::plus:
    /* fall through */
  case ast::unary_op_pre::neg:
    /* fall through */
  case ast::unary_op_pre::bin_neg:
    /* fall through */
  case ast::unary_op_pre::logical_not:
    /* fall through */
    break;
  }

  _require_complete_type(*e.get_expr().get_type());
}

void _ast_info_collector::_handle_expr(const ast::expr_unop_post &e)
{
  switch (e.get_op()) {
  case ast::unary_op_post::inc:
    /* fall through */
  case ast::unary_op_post::dec:
    _require_complete_pointed_to_type(*e.get_expr().get_type());
    break;
  }

  _require_complete_type(*e.get_expr().get_type());
}

bool _ast_info_collector::_handle_expr(const ast::expr_binop &e)
{
  _require_complete_type(*e.get_left_expr().get_type());
  _require_complete_type(*e.get_right_expr().get_type());

  switch (e.get_op()) {
  case ast::binary_op::add:
    /* fall through */
  case ast::binary_op::sub:
    /* fall through */
    _require_complete_pointed_to_type(*e.get_left_expr().get_type());
    _require_complete_pointed_to_type(*e.get_right_expr().get_type());
    break;

  case ast::binary_op::logical_and:
    if (!_is_evaluated(e))
      return false;

    if (_expr_to_bool(e.get_left_expr()) == _constexpr_bool_value::zero) {
      _push_unevaluated(e.get_right_expr());
      return true;
    }
    break;

  case ast::binary_op::logical_or:
    if (!_is_evaluated(e))
      return false;

    if (_expr_to_bool(e.get_left_expr()) == _constexpr_bool_value::nonzero) {
      _push_unevaluated(e.get_right_expr());
      return true;
    }
    break;

  default:
    break;
  }

  return false;
}

void _ast_info_collector::_handle_expr(const ast::expr_assignment &e)
{
  if (e.get_op() == ast::assign_op::add || e.get_op() == ast::assign_op::sub) {
    _require_complete_pointed_to_type(*e.get_lhs().get_type());
  }
  _require_complete_type(*e.get_lhs().get_type());
  _require_complete_type(*e.get_rhs().get_type());
}

void _ast_info_collector::_handle_expr(const ast::expr_array_subscript &e)
{
  _require_complete_type(*e.get_base().get_type());
  _require_complete_pointed_to_type(*e.get_base().get_type());
  _require_complete_type(*e.get_index().get_type());
  _require_complete_pointed_to_type(*e.get_index().get_type());
}

bool _ast_info_collector::_handle_expr(const ast::expr_conditional &e)
{
  _require_complete_type(*e.get_cond().get_type());
  if (e.get_expr_true())
    _require_complete_type(*e.get_expr_true()->get_type());
  _require_complete_type(*e.get_expr_false().get_type());

  if (!_is_evaluated(e))
    return false;

  switch (_expr_to_bool(e.get_cond())) {
  case _constexpr_bool_value::unknown:
    return false;

  case _constexpr_bool_value::nonzero:
    _push_unevaluated(e.get_expr_false());
    return true;

  case _constexpr_bool_value::zero:
    if (e.get_expr_true()) {
      _push_unevaluated(*e.get_expr_true());
      return true;
    } else {
      return false;
    }
  }
}

void _ast_info_collector::_handle_expr(const ast::expr_id &e)
{
  if (!e.is_evaluated()) {
    // This is a expr_id in an attribute specifier which hasn't been
    // resolved to anything.
    return;
  } else if (!e.is_resolved()) {
    // Identifier in function invocation without any prior declaration.
    // The resolver has warned about it already, ignore it for now.
    return;
  }

  const ast::expr_id::resolved &r = e.get_resolved();
  switch (r.get_kind()) {
  case ast::expr_id::resolved::resolved_kind::init_declarator:
    {
      const ast::init_declarator &id = r.get_init_declarator();
      auto it_fidi = _id_to_fidi.find(&id);
      if (it_fidi != _id_to_fidi.end()) {
	if (_is_evaluated(e)) {
	  if (_cur_deps) {
	    _cur_deps->on_funs.add(dep_on_fun{it_fidi->second.get(), e});
	  } else {
	      const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	      const std::string &name = toks[e.get_id_tok()].get_value();
	      code_remark remark
		(code_remark::severity::warning,
		 ("dependency on function \"" + name +
		  "\" not allowed in this context"),
		 _ai.atu.get_pp_result(), e.get_tokens_range());
	      _remarks.add(remark);
	  }
	} else {
	  const ast::declarator &d = id.get_declarator();
	  const types::addressable_type &t = *d.get_innermost_type();
	  assert(_cur_deps_on_types);
	  _cur_deps_on_types->on_decl_types.add
	    (dep_on_decl_type{it_fidi->second.get(),
			      _type_involves_complete_arrays(t), e});
	}

      } else {
	auto it_oidi = _id_to_oidi.find(&id);
	if (it_oidi == _id_to_oidi.end()) {
	  assert(!_is_non_local_linkage(id.get_linkage()));
	  return;
	}

	if (_is_evaluated(e)) {
	  if (_cur_deps) {
	    _cur_deps->on_objs.add(dep_on_obj{it_oidi->second.get(), e});
	  } else {
	      const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	      const std::string &name = toks[e.get_id_tok()].get_value();
	      code_remark remark
		(code_remark::severity::warning,
		 ("dependency on object \"" + name +
		  "\" not allowed in this context"),
		 _ai.atu.get_pp_result(), e.get_tokens_range());
	      _remarks.add(remark);
	  }
	} else {
	  const ast::declarator &d = id.get_declarator();
	  const types::addressable_type &t = *d.get_innermost_type();
	  assert(_cur_deps_on_types);
	  _cur_deps_on_types->on_decl_types.add
	    (dep_on_decl_type{it_oidi->second.get(),
			      _type_involves_complete_arrays(t), e});
	}
      }
    }
    break;

  case ast::expr_id::resolved::resolved_kind::function_definition:
    {
      const ast::function_definition &fd = r.get_function_definition();
      if (!_is_non_local_linkage(fd.get_linkage()))
	return;

      auto it_fdi = _fd_to_fdi.find(&fd);
      assert(it_fdi != _fd_to_fdi.end());

      if (_is_evaluated(e)) {
	  if (_cur_deps) {
	    _cur_deps->on_funs.add(dep_on_fun{it_fdi->second.get(), e});
	  } else {
	      const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	      const std::string &name = toks[e.get_id_tok()].get_value();
	      code_remark remark
		(code_remark::severity::warning,
		 ("dependency on function \"" + name +
		  "\" not allowed in this context"),
		 _ai.atu.get_pp_result(), e.get_tokens_range());
	      _remarks.add(remark);
	  }
      } else {
	const ast::declarator &d = fd.get_declarator();
	const types::addressable_type &t = *d.get_innermost_type();
	assert(_cur_deps_on_types);
	_cur_deps_on_types->on_decl_types.add
	  (dep_on_decl_type{it_fdi->second.get(),
			    _type_involves_complete_arrays(t), e});
      }
    }
    break;

  case ast::expr_id::resolved::resolved_kind::enumerator:
    {
      const ast::enum_def &ed
	= (r.get_enumerator().get_unique_parent<ast::enumerator_list>()
	   .get_unique_parent<ast::enum_def>());

      const auto it_ed = _ed_to_edi.find(&ed);
      if (it_ed == _ed_to_edi.end()) {
	// No registered info means the definition is not global.
	return;
      }

      assert(_cur_deps_on_types);
      _cur_deps_on_types->on_enum_defs.add
	(dep_on_enum_def{it_ed->second.get()});
    }
    break;

  case ast::expr_id::resolved::resolved_kind::builtin_func:
    /* fall through */
  case ast::expr_id::resolved::resolved_kind::builtin_var:
    /* fall through */
  case ast::expr_id::resolved::resolved_kind::parameter_declaration_declarator:
    /* fall through */
  case ast::expr_id::resolved::resolved_kind::stmt_labeled:
    /* fall through */
  case ast::expr_id::resolved::resolved_kind::in_param_id_list:
    break;

  case ast::expr_id::resolved::resolved_kind::none:
    // Can happen for invocations of functions w/o any preceeding
    // declaration.  The AST evualtion has warned about it already.
    break;
  }
}

void _ast_info_collector::_handle_stmt(const ast::stmt_expr &s)
{
  if (s.get_expr())
    _require_complete_type(*s.get_expr()->get_type());
}

void _ast_info_collector::_handle_stmt(const ast::stmt_if &s)
{
  _require_complete_type(*s.get_cond().get_type());
}

void _ast_info_collector::_handle_stmt(const ast::stmt_switch &s)
{
  _require_complete_type(*s.get_expr().get_type());
}

void _ast_info_collector::_handle_stmt(const ast::stmt_while &s)
{
  _require_complete_type(*s.get_cond().get_type());
}

void _ast_info_collector::_handle_stmt(const ast::stmt_do &s)
{
  _require_complete_type(*s.get_cond().get_type());
}

void _ast_info_collector::_handle_stmt(const ast::stmt_for_init_expr &s)
{
  if (s.get_init_expr())
    _require_complete_type(*s.get_init_expr()->get_type());
  if (s.get_cond())
    _require_complete_type(*s.get_cond()->get_type());
  if (s.get_next_expr())
    _require_complete_type(*s.get_next_expr()->get_type());
}

void _ast_info_collector::_handle_stmt(const ast::stmt_for_init_decl &s)
{
  if (s.get_cond())
    _require_complete_type(*s.get_cond()->get_type());
  if (s.get_next_expr())
    _require_complete_type(*s.get_next_expr()->get_type());
}

void _ast_info_collector::
_handle_stmt(const ast::stmt_for_init_static_assert &s)
{
  if (s.get_cond())
    _require_complete_type(*s.get_cond()->get_type());
  if (s.get_next_expr())
    _require_complete_type(*s.get_next_expr()->get_type());
}

void _ast_info_collector::_handle_stmt(const ast::stmt_return &s)
{
  if (s.get_expr())
    _require_complete_type(*s.get_expr()->get_type());
}

void _ast_info_collector::_handle_asm_stmt_op(const ast::asm_operand &o)
{
  _require_complete_type(*o.get_expr().get_type());
}

void _ast_info_collector::
_handle_array_declarator(const types::addressable_type &t)
{
  types::handle_types<void>
    ((wrap_callables<default_action_unreachable<void, type_set<> >::type>
      ([&](const types::array_type &at) {
	 _require_complete_type(*at.get_element_type());
       },
       [&](const types::pointer_type &pt) {
	 // Innermost abstract array declarators in function parameter
	 // declarations get decayed to pointers, yet GCC requires the
	 // underlying type to be complete.
	 _require_complete_type(*pt.get_pointed_to_type());
       })),
     t);
}

void _ast_info_collector::_handle_sou_ref(const ast::struct_or_union_ref &sour)
{
  assert(_cur_deps_on_types);
  if (!(_cur_function_definition && _cur_deps) &&
      !_in_parameter_declaration_list) {
    // We're at global scope and any reference, including the current one,
    // can serve as a declaration.
    sou_ref_info &souri = _create_tag_info<sou_ref_info>(sour);
    assert(_sour_to_souri.find(&sour) == _sour_to_souri.end());
    _sour_to_souri.insert(std::make_pair(&sour, std::ref(souri)));
    return;
  }

  // Test if the reference resolves to some file scope tag
  // declaration.
  const ast::sou_decl_link &dl = sour.get_link_to_decl();
  switch (dl.get_target_kind()) {
  case ast::sou_decl_link::target_kind::unlinked:
    // The reference doesn't resolve to any declaration in some outer
    // scope and could be a declaration in an inner scope. If it is and
    // if it's not standalone, it depends on the tag in question not
    // having been declared before.
    if (sour.get_decl_list_node().is_first() && !sour.is_standalone_decl()) {
      _cur_deps_on_types->on_tag_non_decls.add
		(_ai.atu.get_pp_tokens()[sour.get_id_tok()].get_value());
    }
    return;

  case ast::sou_decl_link::target_kind::def:
    {
      auto it_soud = _soud_to_soudi.find(&dl.get_target_sou_def());
      if (it_soud != _soud_to_soudi.end()) {
	// Resolves to definition at file scope.
	_cur_deps_on_types->on_sou_decls.add
	  (dep_on_sou_decl{it_soud->second.get()});
      }
    }
    break;

  case ast::sou_decl_link::target_kind::ref:
    {
      auto it_sour = _sour_to_souri.find(&dl.get_target_sou_ref());
      if (it_sour != _sour_to_souri.end()) {
	// Resolves to declaration at file scope.
	_cur_deps_on_types->on_sou_decls.add
	  (dep_on_sou_decl{it_sour->second.get()});
      }
    }
    break;
  }
}

void _ast_info_collector::_handle_enum_ref(const ast::enum_ref &er)
{
  assert(_cur_deps_on_types);
  if (!(_cur_function_definition && _cur_deps) &&
      !_in_parameter_declaration_list) {
    // We're at global scope and any reference, including the current one,
    // can serve as a declaration.
    enum_ref_info &eri = _create_tag_info<enum_ref_info>(er);
    assert(_er_to_eri.find(&er) == _er_to_eri.end());
    _er_to_eri.insert(std::make_pair(&er, std::ref(eri)));
    return;
  }

  // Test if the reference resolves to some file scope tag
  // declaration.
  const ast::enum_decl_link &dl = er.get_link_to_decl();
  switch (dl.get_target_kind()) {
  case ast::enum_decl_link::target_kind::unlinked:
    // The reference doesn't resolve to any declaration in some outer
    // scope and could be a declaration in an inner scope. If it is and
    // if it's not standalone, it depends on the tag in question not
    // having been declared before.
    if (er.get_decl_list_node().is_first() && !er.is_standalone_decl()) {
      _cur_deps_on_types->on_tag_non_decls.add
		(_ai.atu.get_pp_tokens()[er.get_id_tok()].get_value());
    }
    return;

  case ast::enum_decl_link::target_kind::def:
    {
      auto it_ed = _ed_to_edi.find(&dl.get_target_enum_def());
      if (it_ed != _ed_to_edi.end()) {
	// Resolves to definition at file scope.
	_cur_deps_on_types->on_enum_decls.add
	  (dep_on_enum_decl{it_ed->second.get()});
      }
    }
    break;

  case ast::enum_decl_link::target_kind::ref:
    {
      auto it_er = _er_to_eri.find(&dl.get_target_enum_ref());
      if (it_er != _er_to_eri.end()) {
	// Resolves to declaration at file scope.
	_cur_deps_on_types->on_enum_decls.add
	  (dep_on_enum_decl{it_er->second.get()});
      }
    }
    break;
  }
}

void _ast_info_collector::
_handle_typedef_ref(const ast::type_specifier_tdid &ts_tdid)
{
  const ast::type_specifier_tdid::resolved &r = ts_tdid.get_resolved();

  if (r.get_kind() ==
      ast::type_specifier_tdid::resolved::resolved_kind::builtin_typedef) {
    return;
  }

  const ast::direct_declarator_id &resolved_ddid =
    r.get_init_declarator().get_declarator().get_direct_declarator_id();
  auto it_tdidi = _ddid_to_tdidi.find(&resolved_ddid);
  if (it_tdidi == _ddid_to_tdidi.end()) {
    // Does not refer to a file scope typedef.
    return;
  }

  assert(_cur_deps_on_types);
  _cur_deps_on_types->on_typedefs.add(dep_on_typedef{it_tdidi->second.get()});
}

void _ast_info_collector::_push_unevaluated(_unevaluated_stack_entry &&u)
{
  _unevaluated_stack.push(std::move(u));
}

void _ast_info_collector::_push_unevaluated(const ast::_ast_entity &e)
{
  _unevaluated_stack_entry u = {std::cref(e)};
  _push_unevaluated(std::move(u));
}

void _ast_info_collector::_pop_unevaluated()
{
  assert(!_unevaluated_stack.empty());
  _unevaluated_stack.pop();
}

bool _ast_info_collector::_is_evaluated(const ast::expr &e) const noexcept
{
  const ast::_ast_entity * c = &e;

  if (_unevaluated_stack.empty())
    return true;

  const auto &stack_top = _unevaluated_stack.top();
  while (c) {
    for (const auto &u : stack_top) {
      if (c == &u.get())
	return false;
    }
    c = c->get_parent();
  }

  return true;
}

_ast_info_collector::_constexpr_bool_value
_ast_info_collector::_expr_to_bool(const ast::expr &e) noexcept
{
  if (!e.is_constexpr())
    return _constexpr_bool_value::unknown;

  if (e.get_constexpr_value().is_zero())
    return _constexpr_bool_value::zero;
  else
    return _constexpr_bool_value::nonzero;
}

bool _ast_info_collector::_is_non_local_linkage(const ast::linkage &l) noexcept
{
  return (l.get_linkage_kind() == ast::linkage::linkage_kind::internal ||
	  l.get_linkage_kind() == ast::linkage::linkage_kind::external);
}

_ast_info_collector::_souoed_stack_entry::
_souoed_stack_entry(sou_def_info &soudi,
		    deps_on_types &_saved_deps_on_types) noexcept
  : k(kind::k_soud), soud(&soudi), saved_deps_on_types(_saved_deps_on_types)
{}

_ast_info_collector::_souoed_stack_entry::
_souoed_stack_entry(enum_def_info &edi,
		    deps_on_types &_saved_deps_on_types) noexcept
  : k(kind::k_soud), ed(&edi), saved_deps_on_types(_saved_deps_on_types)
{}


namespace
{
  class _closure_builder
  {
  public:
    _closure_builder(ast_info &ai,
		     const lp_creation_policy &pol,
		     code_remarks &remarks) noexcept;

    void operator()();

  private:
    class _fun_and_obj_deps_dfs_walker
    {
    public:
      typedef std::function<bool(dep_on_fun&)> dep_on_fun_visitor;
      typedef std::function<bool(dep_on_obj&)> dep_on_obj_visitor;
      typedef std::vector<function_definition_info*> start_set;

      _fun_and_obj_deps_dfs_walker(const dep_on_fun_visitor &dof_visitor,
				   const dep_on_obj_visitor &doo_visitor)
	noexcept;

      void walk(const start_set &start, const unsigned int walk_gen_nr) const;

      struct _stack_entry
      {
	_stack_entry(function_definition_info &_fdi) noexcept;
	_stack_entry(object_init_declarator_info &_oidi) noexcept;

	enum class kind
	{
	 function_definition,
	 object,
	};

	const kind k;

	union
	{
	  function_definition_info * const fdi;
	  object_init_declarator_info * const oidi;
	};

	deps_on_objs::iterator obj_deps_it;
	deps_on_funs::iterator fun_deps_it;
      };

    private:
      const dep_on_fun_visitor _dof_visitor;
      const dep_on_obj_visitor _doo_visitor;
    };

    void _construct_fd_id_closure();
    static bool _fun_expr_id_needs_externalization(const ast::expr_id &eid);
    bool _is_range_within_eligible_headers(const raw_pp_tokens_range &r) const;

    ast_info &_ai;
    const lp_creation_policy &_pol;
    code_remarks &_remarks;
  };
}

_closure_builder::_closure_builder(ast_info &ai,
				   const lp_creation_policy &pol,
				   code_remarks &remarks) noexcept
  : _pol(pol), _ai(ai), _remarks(remarks)
{}

void _closure_builder::operator()()
{
  // Externalization can depend on whether or not some function
  // definition is contained within some header eligible for
  // inclusion. Query the policy for eligibility now.
  for (auto &h : _ai.header_infos) {
    if (!h.eligible)
      continue;

    if (h.is_root()) {
      const pp_result::header_inclusion_root &n = h.get_root_node();
      if (n.is_preinclude()) {
	if (!_pol.is_header_eligible(n, _remarks))
	  _ai.mark_header_uneligible(h);
      } else {
	_ai.mark_header_uneligible(h);
      }

    } else {
      if (!_pol.is_header_eligible(h.get_child_node(), _remarks))
	_ai.mark_header_uneligible(h);
    }
  }

  _construct_fd_id_closure();
}


void _closure_builder::_construct_fd_id_closure()
{
  // For each function definition, externalization is either
  // - impossible,
  // - possible
  // - or even preferred.
  //
  // For each referencing expr_id, externalization is either a
  // strict requirement (because it somehow hands out the address
  // of the function) or it is optional.
  //
  // Those function definitions for which externalization is possible,
  // but not preferred, shall not get externalized, if feasible. One
  // use case are function definitions provided through public
  // headers, which are cheap to provide but where an externalization
  // would cause rewrites of either the header file or of the
  // references to it (or both). OTOH, function definitions not
  // defined in some public header and not being patched should
  // preferrably get externalized in order to reduce the overall size
  // of the closure.
  //
  // Clearly any function definition (except the patched ones) should
  // not get both externalized as well as emitted verbatim: if we're
  // already paying the price for the rewrites, then we would at least
  // want to take advantage of a potentially reduced closure size
  // by dropping the function definition and all of its dependencies.
  //
  // Thus, if some expr_id strictly requiring externalization is
  // encountered during closure construction, the target function
  // definition will get externalized and all other referencing
  // expr_id's then bound to that externalization.
  //
  // However, this means that we have to be careful to keep the
  // closure construction deterministic: some externalized function
  // definition could in turn have some expr_id strictly requiring
  // externalization of some second function. Dropping that reference
  // might lead to that second function having only references to it
  // left which do not require any externalization. Thus, in a naive
  // approach, the set of externalized function would in general
  // depend on the order the individual decisions have been made.
  //
  // Let's describe the problem by a directed graph where the function
  // definitions are represented by nodes and the edges corresponnd to
  // the dep_on_fun dependencies between those, i.e. are in 1:1
  // correspondence to some expr_ids. Each such edge to some function
  // definitions is labeled with either of
  // - "EI" externalization of the target is impossible
  // - "EO" externalization of the target is optional
  // - "ER" externalization of the target is required (due to
  //        the referencing expr_id's context).
  //
  // "EO" edges pointing to function definitions where externalization
  // is preferred will always be externalized. Any EO or ER edges to
  // function definitions for which externalization is preferred will
  // be neglected in what follows as there's no freedom of choice.
  //
  // Consider all paths from the patched set involving only EI and EO
  // edges where the final node has got at least one outgoing ER edge.
  // Observations:
  // 1.) All ER edges not met by at least one such path can be
  //     ignored: there's no way this dependency can be reached from
  //     the patched set.
  // 2.) Consider such a path where all of its nodes haven't got any
  //     incoming ER edges attached to them. All of those nodes won't
  //     get externalized and the final node's outgoing ER edges will
  //     all be reached in any case, i.e. there isn't any choice but to
  //     externalized their target function definitions.
  // This leaves us with the remaining case where all such paths from
  // the patched set involving only EI and EO edges and ending at some
  // node with at least one outgoing ER edge have at least one node
  // with an incoming ER edge on them. This means that for every ER
  // edge, whether or not it will be reachable from the patched set
  // depends on the externalization decision to be made for some
  // prior node (actually on the entirety of decisions made for each
  // set of such nodes separating the ER edge in question from the
  // patched set). [Note: it can be shown that there exists at least
  // one cycle involving one or more ER edges, possibly intermixed
  // with EI and EO edges.]
  //
  // Obviously there's a freedom of choice and we'll have to come up
  // with some rules to make the closure construction algorithm
  // deterministic.
  //
  // Strictly minimizing the set of externalized function definitions
  // is NP hard, because this problem is equivalent to finding maximum
  // anticliques in a certain graph. Thus, we're going to apply some
  // heuristics in this case.
  //
  // Consider the set of all nodes which have an incoming ER edge and
  // are reachable from the patched set through EI/EO paths crossing
  // no other such node with an incoming ER edge. This set is
  // non-empty and well-defined. Observe that each EI/EO path from the
  // patched set to some ER edge will always cross one of the nodes in
  // this set and thus, they're good candidates for externalizations,
  // because that would hopefully disconnect a large number of RE
  // edges from the patched set. Let's call this set the "candidate
  // set" in what follows. In principle, we could choose to
  // externalize all of the nodes from the candidate set at this
  // point, but there's room for improvement: externalizing one of
  // those could perhaps render the externalization of another such
  // node unnecessary.
  // Consider two nodes from the candidate set with an EI/EO path from
  // the first node to an ER edge ending in the second
  // one. Externalizing the path's originating source might again
  // disconnect the ER edge from the patched set. Thus, if there's
  // more than one node in the candidate set, we'll choose to
  // externalize all those for which there doesn't exist any such path
  // ending at them.
  // If OTOH, there's no such node meeting these requirements, we'll
  // pick a single node and make this choice deterministic by choosing
  // the function definition from the candidate set coming last in
  // file order.
  const std::vector<function_definition_info *> patched_set
    = [&]() {
	std::vector<function_definition_info *> result;
	for (auto it = _ai.functions_begin();
	     it != _ai.functions_end(); ++it) {
	  function_definition_info * const fdi = it->get_function_definition();
	  if (fdi && fdi->is_patched)
	    result.push_back(fdi);
	}
	return result;
      }();

  // In a first sweep through the EI/EO-connected component the
  // patched set is contained in, initialize externalizability etc.
  std::size_t n_pending_nodes_w_incoming_er_edges = 0;
  {
    const unsigned int cur_walk_gen_nr = _ai.start_walk();
    for (auto fdi : patched_set)
      fdi->init_walk_data(cur_walk_gen_nr);

    auto &&dep_on_fun_visitor
      = [&](dep_on_fun &d) {
	  assert(!d.externalize);
	  function_info &fi = d.get_function_info();
	  function_definition_info * const fdi = fi.get_function_definition();
	  if (fdi) {
	    d.externalize = _fun_expr_id_needs_externalization(d.from_eid);

	    if (!fi.can_externalize_valid) {
	      fi.can_externalize =
		_pol.is_function_externalizable(fdi->function_definition,
						_remarks);
	      if (fi.can_externalize && !fdi->is_patched) {
		const bool in_eligible_headers
		  = (_is_range_within_eligible_headers
		     (_ai.atu.get_pp_result().pp_tokens_range_to_raw
		      (fdi->function_definition.get_tokens_range())));
		fdi->is_externalization_preferred
		  = (_pol.is_function_externalization_preferred
		     (fdi->function_definition, in_eligible_headers,
		      _remarks));
	      }

	      fi.can_externalize_valid = true;
	    }

	    if (fdi->is_externalization_preferred) {
	      assert(!fdi->is_patched);
	      d.externalize = true;
	    } else if (d.externalize && !fi.can_externalize) {
	      d.externalize = false;

	      const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	      const std::string &name =
		toks[d.from_eid.get_id_tok()].get_value();
	      code_remark remark
		(code_remark::severity::warning,
		 ("required function externalization of \"" + name +
		  "\" is impossible"),
		 _ai.atu.get_pp_result(), d.from_eid.get_tokens_range());
	      _remarks.add(remark);
	    }

	    fdi->init_walk_data(cur_walk_gen_nr);
	    if (d.externalize) {
	      if (!fdi->has_reachable_incoming_er_edge) {
		++n_pending_nodes_w_incoming_er_edges;
		fdi->has_reachable_incoming_er_edge = true;
	      }
	      return false;
	    } else {
	      return true;
	    }

	  } else {
	    if (!fi.can_externalize_valid) {
	      assert(d.fidi);
	      fi.can_externalize =
		_pol.is_function_externalizable(d.fidi->init_declarator,
						_remarks);
	      fi.can_externalize_valid = true;
	    }

	    if (fi.can_externalize) {
	      d.externalize = true;

	    } else {
	      d.externalize = false;

	      const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	      const std::string &name =
		toks[d.from_eid.get_id_tok()].get_value();
	      code_remark remark
		(code_remark::severity::warning,
		 ("function \"" + name +
		  "\" without definition cannot get externalized"),
		 _ai.atu.get_pp_result(), d.from_eid.get_tokens_range());
	      _remarks.add(remark);
	    }

	    return false;
	  }
	};

    auto &&dep_on_obj_visitor
      = [&](dep_on_obj &d) {
	  object_info &oi = d.get_object_info();
	  if (!oi.shall_externalize_valid) {
	    const ast::storage_class_specifier * const scs =
	      d.oidi.get().enclosing_declaration.storage_class_specifier;
	    const ast::linkage &l = d.oidi.get().init_declarator.get_linkage();
	    if (!scs ||
		(scs->get_storage_class() != ast::storage_class::sc_register)) {
	      oi.shall_externalize =
		(_pol.shall_externalize_object(d.oidi.get().init_declarator,
					       _remarks));
	      oi.shall_externalize_valid = true;

	      if (!oi.shall_externalize) {
		const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
		const ast::declarator &decl =
		  d.oidi.get().init_declarator.get_declarator();
		const std::string &name = toks[decl.get_id_tok()].get_value();
		code_remark remark
		  (code_remark::severity::warning,
		   "referenced object \"" + name + "\" cannot get externalized",
		   _ai.atu.get_pp_result(), decl.get_tokens_range());
		_remarks.add(remark);
	      }
	    } else {
	      // File scope declaration with 'register' storage class specifier.
	      oi.shall_externalize = false;
	      oi.shall_externalize_valid = true;
	    }
	  }

	  assert(!d.externalize);
	  d.externalize = oi.shall_externalize;
	  if (!d.externalize && oi.get_initializing_init_declarator())
	    return true;
	  return false;
	};

    _fun_and_obj_deps_dfs_walker walker{dep_on_fun_visitor,
					dep_on_obj_visitor};
    walker.walk(patched_set, cur_walk_gen_nr);
    _ai.finish_walk();
  }

  while (n_pending_nodes_w_incoming_er_edges) {
    // First, externalize all function definitions for which it's
    // inevitable, i.e. all those which have an incoming ER edge
    // reachable from the patched set by EI/EO paths not crossing some
    // node with an incoming ER edge.
    bool externalized_some = false;
    std::vector<function_definition_info*> candidate_set;
    {
      auto &&dep_on_fun_visitor
	= [&](dep_on_fun &d) {
	    function_info &fi = d.get_function_info();
	    function_definition_info * const fdi = fi.get_function_definition();
	    if (!fdi)
	      return false;

	    if (fi.externalize)
	      return false;

	    if (!fdi->has_reachable_incoming_er_edge) {
	      assert(!d.externalize);
	      return true;
	    } else if (d.externalize) {
	      if (!fi.externalize) {
		--n_pending_nodes_w_incoming_er_edges;
		fi.externalize = true;
		externalized_some = true;
	      }
	      return false;

	    } else {
	      if (!fdi->is_patched) {
		// The target node has an incoming ER edge and is
		// reachable from the patched set by an EI/EO path not
		// crossing any other node with an incoming ER
		// node. Add it to the candidate set, which will only
		// be used for !externalized_some, i.e. in case we'll
		// be unable to externalize anything in this walk.
		if (!fdi->in_candidate_set) {
		  candidate_set.push_back(fdi);
		  fdi->in_candidate_set = true;
		}
	      }
	      return false;
	    }
	  };

      auto &&dep_on_obj_visitor
	= [&](dep_on_obj &d) {
	    if (!d.externalize &&
		d.get_object_info().get_initializing_init_declarator()) {
	      return true;
	    }
	    return false;
	  };

      const unsigned int cur_walk_gen_nr = _ai.start_walk();
      _fun_and_obj_deps_dfs_walker walker{dep_on_fun_visitor,
					  dep_on_obj_visitor};
      walker.walk(patched_set, cur_walk_gen_nr);
      _ai.finish_walk();
    }

    if (!n_pending_nodes_w_incoming_er_edges)
      break;

    if (externalized_some) {
      // The externalization of some of the function definitions might
      // have disconnected some ER edges from the patched set.
      goto update_reachable_incoming_er_edges;
    }

    // Ok, there are nodes for which we'll have to make a choice.
    assert(!candidate_set.empty());

    // See if there are any nodes in the candidate set which don't
    // have an incoming ER edge reachable from that set -- these are
    // preferred externalization victims. Otherwise choose the
    // function definition coming last in file order.
    {
      for (auto fdi : candidate_set)
	fdi->has_reachable_incoming_er_edge = false;

      auto &&dep_on_fun_visitor
	= [&](dep_on_fun &d) {
	    function_info &fi = d.get_function_info();
	    function_definition_info * const fdi = fi.get_function_definition();
	    if (!fdi)
	      return false;

	    if (fi.externalize)
	      return false;

	    if (d.externalize) {
	      if (fdi->in_candidate_set)
		fdi->has_reachable_incoming_er_edge = true;
	      return false;
	    }

	    return true;
	  };

      auto &&dep_on_obj_visitor
	= [&](dep_on_obj &d) {
	    if (!d.externalize &&
		d.get_object_info().get_initializing_init_declarator()) {
	      return true;
	    }
	    return false;
	  };

      const unsigned int cur_walk_gen_nr = _ai.start_walk();
      _fun_and_obj_deps_dfs_walker walker{dep_on_fun_visitor,
					  dep_on_obj_visitor};
      walker.walk(candidate_set, cur_walk_gen_nr);
      _ai.finish_walk();
    }

    if (std::any_of(candidate_set.begin(),
		    candidate_set.end(),
		    [](const function_definition_info *fdi) {
		      return !fdi->has_reachable_incoming_er_edge;
		    })) {
      for (auto fdi : candidate_set) {
	if (!fdi->has_reachable_incoming_er_edge)
	  fdi->fi.externalize = true;
      }

    } else {
      function_definition_info *last_fdi = *candidate_set.begin();
      for (auto it = candidate_set.begin() + 1; it != candidate_set.end();
	   ++it) {
	if ((*it)->function_definition.get_tokens_range().begin >
	    last_fdi->function_definition.get_tokens_range().begin) {
	  last_fdi = *it;
	}
      }
      last_fdi->fi.externalize = true;
    }

update_reachable_incoming_er_edges:
    // Recalculate n_pending_nodes_w_incoming_er_edges to see if it
    // has reached zero. Reinitialize the per-iteration members
    // of all reachable function_definition instances.
    {
      n_pending_nodes_w_incoming_er_edges = 0;

      const unsigned int cur_walk_gen_nr = _ai.start_walk();
      for (auto fdi : patched_set)
	fdi->init_walk_data(cur_walk_gen_nr);

      auto &&dep_on_fun_visitor
	= [&](dep_on_fun &d) {
	    function_info &fi = d.get_function_info();
	    function_definition_info * const fdi = fi.get_function_definition();
	    if (!fdi)
	      return false;

	    fdi->init_walk_data(cur_walk_gen_nr);

	    if (fi.externalize) {
	      return false;
	    } else if (d.externalize) {
	      if (!fdi->has_reachable_incoming_er_edge) {
		++n_pending_nodes_w_incoming_er_edges;
		fdi->has_reachable_incoming_er_edge = true;
	      }
	      return false;
	    }

	    return true;
	  };

      auto &&dep_on_obj_visitor
	= [&](dep_on_obj &d) {
	    if (!d.externalize &&
		d.get_object_info().get_initializing_init_declarator()) {
	      return true;
	    }
	    return false;
	  };

      _fun_and_obj_deps_dfs_walker walker{dep_on_fun_visitor,
					  dep_on_obj_visitor};
      walker.walk(patched_set, cur_walk_gen_nr);
      _ai.finish_walk();
    }
  }

  // At this point, all externalization definitions have been made.
  // Do a final dependency graph walk to externalize all remaining
  // dep_on_fun instances referring to externalized function
  // definitions, i.e. set the ->externalize flag on all EO edges
  // pointing to externalized function definitions. Also set the
  // ->in_closure member of all function definitions reachable from
  // the patched set and not externalized.
  {
    auto &&dep_on_fun_visitor
      = [&](dep_on_fun &d) {
	  function_info &fi = d.get_function_info();
	  function_definition_info * const fdi = fi.get_function_definition();

	  if (!fdi) {
	    if (d.externalize)
	      fi.externalize = true;
	    else
	      fi.in_closure = true;
	    return false;
	  }

	  if (fdi->is_patched)
	    return false;

	  if (fi.externalize) {
	    d.externalize = true;
	    return false;
	  } else {
	    fi.in_closure = true;
	    return true;
	  }
	};

    auto &&dep_on_obj_visitor
      = [&](dep_on_obj &d) {
	  object_info &oi = d.get_object_info();
	  assert(oi.shall_externalize_valid);
	  if (!d.externalize) {
	    assert(!oi.shall_externalize);
	    oi.in_closure = true;
	    if (oi.get_initializing_init_declarator())
	      return true;
	    else
	      return false;
	  } else {
	    assert(oi.shall_externalize);
	    oi.externalize = true;
	    return false;
	  }
	};

    const unsigned int cur_walk_gen_nr = _ai.start_walk();
    _fun_and_obj_deps_dfs_walker walker{dep_on_fun_visitor,
					dep_on_obj_visitor};
    walker.walk(patched_set, cur_walk_gen_nr);
    _ai.finish_walk();
  }
}

bool _closure_builder::
_fun_expr_id_needs_externalization(const ast::expr_id &eid)
{
  // See whether an expr_id denotes a function whose pointer can
  // somehow be leaked to memory or whether the actual pointer value
  // matters. False negatives must be ruled out and false positives
  // should be minimized.
  const ast::_ast_entity *p = eid.get_parent();
  const ast::expr *c = &eid;
  bool need_externalization = true;
  const ast::stmt_expr *last_stmt = nullptr;
  while (p) {
    using handled_entities =
      ast::expr::parent_types
	::add_type<ast::stmt_compound>
	::add_type<ast::expr_statement>;
    p->process<void, handled_entities >
      (wrap_callables<no_default_action>
       ([&](const ast::expr_cast &e) {
	  c = &e;
	  p = c->get_parent();
	},
	[&](const ast::expr_unop_pre &e) {
	  switch (e.get_op()) {
	  case ast::unary_op_pre::inc:
	    /* fall through */
	  case ast::unary_op_pre::dec:
	    // Increment/decrement want an lvalue, should be
	    // impossible here.
	    assert(0);
	    __builtin_unreachable();
	    break;

	  case ast::unary_op_pre::addr:
	    /* fall through */
	  case ast::unary_op_pre::deref:
	    c = &e;
	    p = c->get_parent();
	    break;

	  case ast::unary_op_pre::plus:
	    /* fall through */
	  case ast::unary_op_pre::neg:
	    /* fall through */
	  case ast::unary_op_pre::bin_neg:
	    // That's suspicious.
	    need_externalization = true;
	    p = nullptr;
	    break;

	  case ast::unary_op_pre::logical_not:
	    need_externalization = false;
	    p = nullptr;
	    break;
	  }
	},
	[&](const ast::expr_sizeof_expr&) {
	  // expr_id's appearing in sizeof/alignof/sizeof won't make
	  // it here.
	  assert(0);
	  __builtin_unreachable();
	},
	[&](const ast::expr_alignof_expr&) {
	  // expr_id's appearing in sizeof/alignof/sizeof won't make
	  // it here.
	  assert(0);
	  __builtin_unreachable();
	},
	[&](const ast::expr_builtin_choose_expr &e) {
	  if (c == &e.get_cond()) {
	    // The condition is evaluated as a scalar.
	    need_externalization = false;
	    p = nullptr;
	  } else {
	    c = &e;
	    p = c->get_parent();
	  }
	},
	[&](const ast::offset_member_designator&) {
	  // This shouldn't be possible except for complete nonsense
	  // code involving casts.
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::expr_array_subscript&) {
	  // This shouldn't be possible except for complete nonsense
	  // code involving casts.
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::expr_func_invocation &e) {
	  if (c == &e.get_func()) {
	    // Function designator from invocation.
	    need_externalization = false;
	    p = nullptr;
	    return;
	  }

	  // Expression has been passed as an argument to some
	  // function invocation.
	  need_externalization = true;
	  p = nullptr;

	},
	[&](const ast::expr_member_deref&) {
	  // This shouldn't be possible except for complete nonsense
	  // code involving casts.
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::expr_unop_post&) {
	  // Increment/decrement want an lvalue, should be impossible here.
	  assert(0);
	  __builtin_unreachable();
	},
	[&](const ast::expr_parenthesized &e) {
	  c = &e;
	  p = c->get_parent();
	},
	[&](const ast::expr_comma &e) {
	  if (c == &e.get_left()) {
	    need_externalization = false;
	    p = nullptr;
	  } else {
	    c = &e;
	    p = c->get_parent();
	  }
	},
	[&](const ast::expr_assignment&) {
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::expr_conditional &e) {
	  if (c == &e.get_cond()) {
	    need_externalization = false;
	    p = nullptr;
	  } else {
	    c = &e;
	    p = c->get_parent();
	  }
	},
	[&](const ast::expr_binop &e) {
	  if (e.get_op() == ast::binary_op::logical_and ||
	      e.get_op() == ast::binary_op::logical_or) {
	    need_externalization = false;
	  } else {
	    need_externalization = true;
	  }

	  p = nullptr;
	},
	[&](const ast::expr_builtin_va_arg&) {
	  // This shouldn't be possible except for complete nonsense
	  // code involving casts.
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::direct_abstract_declarator_array&) {
	  // This shouldn't be possible except for complete nonsense
	  // code involving casts.
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::direct_declarator_array&) {
	  // This shouldn't be possible except for complete nonsense
	  // code involving casts.
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::struct_declarator&) {
	  // This is impossible because of constness requirements.
	  assert(0);
	  __builtin_unreachable();
	},
	[&](const ast::enumerator&) {
	  // This is impossible because of constness requirements.
	  assert(0);
	  __builtin_unreachable();
	},
	[&](const ast::typeof_expr&) {
	  // expr_id's appearing in sizeof/alignof/sizeof won't make
	  // it here.
	  assert(0);
	  __builtin_unreachable();
	},
	[&](const ast::initializer_expr&) {
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::designator_array&) {
	  // This shouldn't be possible except for complete nonsense
	  // code involving casts.
	  need_externalization = false;
	  p = nullptr;
	},
	[&](const ast::stmt_case&) {
	  // This is impossible because of constness requirements.
	  assert(0);
	  __builtin_unreachable();
	},
	[&](const ast::stmt_case_range&) {
	  // This is impossible because of constness requirements.
	  assert(0);
	  __builtin_unreachable();
	},
	[&](const ast::stmt_expr &s) {
	  // Can be the last statement in a statement expression
	  c = nullptr;
	  p = s.get_parent();
	  last_stmt = &s;
	},
	[&](const ast::stmt_compound &sc) {
	  // Can be part of a statement expression.
	  // Find the last statement in the list and compare it agains
	  // last_stmt.
	  assert(last_stmt);
	  const ast::block_item_list * const bil = sc.get_block_item_list();
	  assert(bil);

	  // Find the last statement in the block item list
	  const ast::block_item_stmt *bis = nullptr;
	  bil->for_each_reverse
	    (wrap_callables<default_action_return_value<bool, true>::type>
	     ([&bis](const ast::block_item_stmt &bi) {
		bis = &bi;
		return false;
	      }));
	  assert(bis);

	  if (&bis->get_stmt() != last_stmt) {
	    need_externalization = false;
	    p = nullptr;
	  } else {
	    p = sc.get_parent();
	  }
	},
	[&](const ast::expr_statement &e) {
	  assert(last_stmt);
	  last_stmt = nullptr;
	  c = &e;
	  p = c->get_parent();
	},
	[&](const ast::stmt_if&) {
	  last_stmt = nullptr;
	  need_externalization = false;
	  p = nullptr;
	},
	[&](const ast::stmt_switch&) {
	  last_stmt = nullptr;
	  need_externalization = false;
	  p = nullptr;
	},
	[&](const ast::stmt_while&) {
	  last_stmt = nullptr;
	  need_externalization = false;
	  p = nullptr;
	},
	[&](const ast::stmt_do&) {
	  last_stmt = nullptr;
	  need_externalization = false;
	  p = nullptr;
	},
	[&](const ast::stmt_for_init_expr&) {
	  last_stmt = nullptr;
	  need_externalization = false;
	  p = nullptr;
	},
	[&](const ast::stmt_for_init_decl&) {
	  last_stmt = nullptr;
	  need_externalization = false;
	  p = nullptr;
	},
	[&](const ast::stmt_for_init_static_assert&) {
	  last_stmt = nullptr;
	  need_externalization = false;
	  p = nullptr;
	},
	[&](const ast::stmt_goto&) {
	  // Odd goto.
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::stmt_return&) {
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::asm_operand&) {
	  need_externalization = true;
	  p = nullptr;
	},
	[&](const ast::_ast_entity&) {
	  need_externalization = false;
	  p = nullptr;
	}));
  }

  return need_externalization;
}


bool _closure_builder::
_is_range_within_eligible_headers(const raw_pp_tokens_range &r) const
{
  const pp_result &ppr = _ai.atu.get_pp_result();
  const auto it_end = ppr.intersecting_sources_end(r);
  for (auto it = ppr.intersecting_sources_begin(r); it != it_end; ++it) {
    const header_info &hi = _ai.header_infos[it->get_id()];
    if (!hi.eligible)
      return false;
  }
  return true;
}


_closure_builder::_fun_and_obj_deps_dfs_walker::
_fun_and_obj_deps_dfs_walker(const dep_on_fun_visitor &dof_visitor,
			     const dep_on_obj_visitor &doo_visitor) noexcept
  : _dof_visitor(dof_visitor), _doo_visitor(doo_visitor)
{}

void _closure_builder::_fun_and_obj_deps_dfs_walker::
walk(const start_set &start, const unsigned int walk_gen_nr) const
{
  std::vector<_stack_entry> _dfs_stack;

  for (auto it_start = start.cbegin(); it_start != start.cend(); ++it_start) {
    if ((*it_start)->walk_gen_nr == walk_gen_nr)
      continue;
    (*it_start)->walk_gen_nr = walk_gen_nr;
    _dfs_stack.emplace_back(**it_start);

    while (!_dfs_stack.empty()) {
    new_stack_top:
      _stack_entry &top = _dfs_stack.back();

      const deps_on_objs::iterator deps_on_objs_end =
	(top.k == _stack_entry::kind::function_definition ?
	 top.fdi->body_deps.on_objs.end() :
	 top.oidi->get_initializer()->deps.on_objs.end());
      while (top.obj_deps_it != deps_on_objs_end) {
	if (!_doo_visitor(*top.obj_deps_it)) {
	  ++top.obj_deps_it;
	} else {
	  object_init_declarator_info *oidi =
	    (top.obj_deps_it
	     ->get_object_info().get_initializing_init_declarator());
	  assert(oidi);
	  assert(oidi->get_initializer());
	  ++top.obj_deps_it;
	  if (oidi->walk_gen_nr != walk_gen_nr) {
	    oidi->walk_gen_nr = walk_gen_nr;
	    _dfs_stack.emplace_back(*oidi);
	    goto new_stack_top;
	  }
	}
      }

      const deps_on_funs::iterator deps_on_funs_end =
	(top.k == _stack_entry::kind::function_definition ?
	 top.fdi->body_deps.on_funs.end() :
	 top.oidi->get_initializer()->deps.on_funs.end());
      while (top.fun_deps_it != deps_on_funs_end) {
	if (!_dof_visitor(*top.fun_deps_it)) {
	  ++top.fun_deps_it;
	} else {
	  function_definition_info * const fdi =
	    top.fun_deps_it->get_function_info().get_function_definition();
	  assert(fdi);
	  ++top.fun_deps_it;
	  if (fdi->walk_gen_nr != walk_gen_nr) {
	    fdi->walk_gen_nr = walk_gen_nr;
	    _dfs_stack.emplace_back(*fdi);
	    goto new_stack_top;
	  }
	}
      }

      _dfs_stack.pop_back();
    }
  }
}

_closure_builder::_fun_and_obj_deps_dfs_walker::_stack_entry::
_stack_entry(function_definition_info &_fdi) noexcept
  : k(kind::function_definition), fdi(&_fdi),
    obj_deps_it(_fdi.body_deps.on_objs.begin()),
    fun_deps_it(_fdi.body_deps.on_funs.begin())
{}

_closure_builder::_fun_and_obj_deps_dfs_walker::_stack_entry::
_stack_entry(object_init_declarator_info &_oidi) noexcept
  : k(kind::object), oidi(&_oidi),
    obj_deps_it(_oidi.get_initializer()->deps.on_objs.begin()),
    fun_deps_it(_oidi.get_initializer()->deps.on_funs.begin())
{}



namespace
{
  class _symbol_modification_resolver
  {
  public:
    _symbol_modification_resolver(ast_info &ai,
				  const lp_creation_policy &pol,
				  code_remarks &remarks) noexcept;

    void operator()();

  private:
    ast_info &_ai;
    const lp_creation_policy &_pol;
    code_remarks &_remarks;
  };
}

_symbol_modification_resolver::
_symbol_modification_resolver(ast_info &ai,
			      const lp_creation_policy &pol,
			      code_remarks &remarks) noexcept
  : _pol(pol), _ai(ai), _remarks(remarks)
{}


void _symbol_modification_resolver::operator()()
{
  using linkage_change =
    lp_creation_policy::symbol_modification::linkage_change;

  bool renamed_some = false;

  // First, query the policy for all symbol modifiers for externalized
  // symbols as well as patched function definitions.
  for (auto it_oi = _ai.objects_begin(); it_oi != _ai.objects_end(); ++it_oi) {
    if (it_oi->externalize) {
      const ast::init_declarator &id
	= (it_oi->init_declarators_begin()->init_declarator);
      it_oi->externalized_sym_mod =
	_pol.get_sym_mod_for_externalized_obj(id, _ai.get_allocated_ids(),
					      _remarks);
      _ai.register_sym_mod(it_oi->externalized_sym_mod.sym_mod);
      if (it_oi->externalized_sym_mod.sym_mod.is_rename() ||
	  it_oi->externalized_sym_mod.make_pointer) {
	renamed_some = true;
      }
    }
  }

  for (auto it_fi = _ai.functions_begin(); it_fi != _ai.functions_end();
       ++it_fi) {
    function_definition_info * const fdi = it_fi->get_function_definition();
    if (it_fi->externalize) {
      const ast::direct_declarator_id &ddid
	= (fdi ?
	   (fdi->function_definition.get_declarator()
	    .get_direct_declarator_id()) :
	   (it_fi->init_declarators_begin()->init_declarator.get_declarator()
	    .get_direct_declarator_id()));
      it_fi->externalized_sym_mod =
	_pol.get_sym_mod_for_externalized_fun(ddid, _ai.get_allocated_ids(),
					      _remarks);
      _ai.register_sym_mod(it_fi->externalized_sym_mod.sym_mod);
      if (it_fi->externalized_sym_mod.sym_mod.is_rename() ||
	  it_fi->externalized_sym_mod.make_pointer) {
	renamed_some = true;
      }
    }

    if (fdi) {
      if (fdi->is_patched) {
	fdi->rewrite_needed = true;
	fdi->sym_mod =
	  _pol.get_sym_mod_for_patched_fun(fdi->function_definition,
					   _ai.get_allocated_ids(),
					   _remarks);
	_ai.register_sym_mod(fdi->sym_mod);
	if (fdi->sym_mod.is_rename())
	  renamed_some = true;

	if (it_fi->externalize &&
	    !it_fi->externalized_sym_mod.sym_mod.is_rename() &&
	    !fdi->sym_mod.is_rename()) {
	  // The patched function's definition will have to get
	  // emitted directly as well as externalized. At least one of
	  // the two instances must get renamed.
	  code_remark remark
	    (code_remark::severity::fatal,
	     "name clash for patched and externalized function",
	     _ai.atu.get_pp_result(),
	     (fdi->function_definition.get_declarator()
	      .get_direct_declarator_id().get_tokens_range()));
	  _remarks.add(remark);
	  throw lp_except(remark);
	}
      } else if (it_fi->in_closure) {
	// Force linkage of non-externalized and non-patched functions
	// within the closure to be internal.
	//
	// Also, if a function in the closure is extern, gnu_inline,
	// but not always_inline, it needs to be made non-extern,
	// because otherwise the compiler would be free to ignore this
	// definition (based on its inlining decision) and create an
	// external reference itself.
	assert(!it_fi->externalize);
	const ast::function_definition &fd = fdi->function_definition;
	if (!(fd.is_gnu_inline() && fd.is_always_inline())) {
	  fdi->sym_mod.new_linkage =
	    (lp_creation_policy::symbol_modification::linkage_change
	     ::lc_make_static);
	}
      }

      // Force all dep_on_funs bound to "extern" declarations in
      // function scope to get externalized if there will be an
      // externalized variant of the referenced target function. Note
      // that for patched functions it is possible to have both in the
      // output, a patched and an externalized variant. Rebinding the
      // dependencies here allows for simpler code when dealing with
      // the rewrites for local "extern" declarations.
      if (fdi->is_patched || it_fi->in_closure) {
	for (auto &d : fdi->body_deps.on_funs) {
	  if (d.fidi && d.fidi->enclosing_declaration.enclosing_fd &&
	      d.fidi->fi.externalize) {
	    d.externalize = true;
	  }
	}
      }
    }
  }

  // Now, some function_definitions in the closure might have to get
  // rewritten in order to update references to renamed symbols. Query
  // the policy on how to rename the corrsponding symbols.

  auto dep_on_obj_requires_rewrite =
    [](const dep_on_obj &d) {
      const object_info &target_oi = d.get_object_info();
      assert(target_oi.in_closure || target_oi.externalize);
      assert(d.externalize == target_oi.externalize);

      return (d.externalize &&
	      (target_oi.externalized_sym_mod.sym_mod.is_rename() ||
	       target_oi.externalized_sym_mod.make_pointer));
    };

  auto dep_on_fun_requires_rewrite =
    [](const dep_on_fun &d) {
      const function_info &target_fi = d.get_function_info();
      assert(!d.externalize || target_fi.externalize);
      const function_definition_info * const target_fdi =
	target_fi.get_function_definition();
      assert(target_fi.externalize ||
	     (target_fdi && target_fdi->is_patched) ||
	     target_fi.in_closure);
      return ((d.externalize &&
	       (target_fi.externalized_sym_mod.sym_mod.is_rename() ||
		target_fi.externalized_sym_mod.make_pointer)) ||
	      (!d.externalize && target_fdi &&
	       target_fdi->sym_mod.is_rename()));
    };

  while (renamed_some) {
    renamed_some = false;

    for (auto it_fi = _ai.functions_begin(); it_fi != _ai.functions_end();
	 ++it_fi) {
      if (!it_fi->in_closure)
	continue;

      function_definition_info * const fdi = it_fi->get_function_definition();
      if (!fdi || fdi->rewrite_needed)
	continue;

      const deps_on &deps = fdi->body_deps;
      for (auto &d : deps.on_objs) {
	if (dep_on_obj_requires_rewrite(d))
	  fdi->rewrite_needed = true;
      }

      for (auto &d : deps.on_funs) {
	if (dep_on_fun_requires_rewrite(d))
	  fdi->rewrite_needed = true;
      }

      for (const auto &di_in_fd : fdi->get_declarations()) {
	di_in_fd.init_declarators.for_each
	  (wrap_callables<no_default_action>
	   ([&](const function_init_declarator_info &fidi_in_fd) {
	      const function_info &fidi_in_fd_fi = fidi_in_fd.fi;
	      const function_definition_info * const referenced_fdi
		= fidi_in_fd_fi.get_function_definition();
	      if ((fidi_in_fd_fi.externalize &&
		   (fidi_in_fd_fi.externalized_sym_mod.sym_mod.is_rename() ||
		    fidi_in_fd_fi.externalized_sym_mod.make_pointer)) ||
		  (!fidi_in_fd_fi.externalize &&
		   referenced_fdi &&
		   (referenced_fdi->is_patched ||
		    referenced_fdi->rewrite_needed) &&
		   referenced_fdi->sym_mod.is_rename())) {
		fdi->rewrite_needed = true;
	      }
	    },
	    [&](const object_init_declarator_info &oidi_in_fd) {
	      const object_info &oi = oidi_in_fd.oi;
	      if (oi.externalize) {
		const ast::linkage &l =
		  oidi_in_fd.init_declarator.get_linkage();
		if (oi.externalized_sym_mod.sym_mod.is_rename() ||
		    oi.externalized_sym_mod.make_pointer ||
		    ((oi.externalized_sym_mod.sym_mod.new_linkage !=
		      linkage_change::lc_none) &&
		     (l.get_linkage_kind() ==
		      ast::linkage::linkage_kind::none))) {
		  fdi->rewrite_needed = true;
		}

	      } else if (oidi_in_fd.get_initializer()) {
		const initializer_info &ii = *oidi_in_fd.get_initializer();

		for (auto &d : ii.deps.on_objs) {
		  if (dep_on_obj_requires_rewrite(d))
		    fdi->rewrite_needed = true;
		}

		for (auto &d : ii.deps.on_funs) {
		  if (dep_on_fun_requires_rewrite(d))
		    fdi->rewrite_needed = true;
		}
	      }
	    },
	    [&](const typedef_init_declarator_info &tdidi) {
	      assert(0);
	      __builtin_unreachable();
	    }));
      }

      if (fdi->rewrite_needed) {
	fdi->sym_mod.new_name =
	    _pol.rename_rewritten_closure_fun(fdi->function_definition,
					      _ai.get_allocated_ids(),
					      _remarks);
	_ai.register_sym_mod(fdi->sym_mod);
	  if (fdi->sym_mod.is_rename())
	    renamed_some = true;
      }
    }
  }

  // Finally sweep over all object initializers and mark those which
  // will have to get rewritten.
  for (auto it_oi = _ai.objects_begin(); it_oi != _ai.objects_end();
       ++it_oi) {
    if (!it_oi->in_closure)
      continue;

    for (auto it_oidi = it_oi->init_declarators_begin();
	 it_oidi != it_oi->init_declarators_end(); ++it_oidi) {
      initializer_info * const ii = it_oidi->get_initializer();

      if (ii) {
	for (auto &d : ii->deps.on_objs) {
	  if (dep_on_obj_requires_rewrite(d))
	    ii->rewrite_needed = true;
	}

	for (auto &d : ii->deps.on_funs) {
	  if (dep_on_fun_requires_rewrite(d))
	    ii->rewrite_needed = true;
	}
      }
    }
  }
}



namespace
{
  class _conflicting_header_finder
  {
  public:
    _conflicting_header_finder(ast_info &ai,
			       const lp_creation_policy &pol,
			       code_remarks &remarks) noexcept;

    void operator()();

  private:
    void _mark_headers_uneligible(const pp_tokens_range &r);

    void _check_header_ordering_constraints
	(const pp_result::header_inclusion_node &h);

    ast_info &_ai;
    const lp_creation_policy &_pol;
    code_remarks &_remarks;
  };
}

_conflicting_header_finder::
_conflicting_header_finder(ast_info &ai,
			   const lp_creation_policy &pol,
			   code_remarks &remarks) noexcept
  : _pol(pol), _ai(ai), _remarks(remarks)
{}

void _conflicting_header_finder::operator()()
{
  using linkage_change =
    lp_creation_policy::symbol_modification::linkage_change;

  // First, mark those headers uneligible, which provide some
  // self-conflicting declaration or definitions. "Self-conflicting"
  // here means that the symbol is somehow required by the live patch
  // and must be altered but the policy nonetheless decided not to
  // rename it.
  //
  // Note that symbols can always be made to have internal linkage by
  // issuing an extra 'static' declaration before the first occurence,
  // i.e. in
  //  static int a;
  //  extern int a;
  // 'a' will have internal linkage.
  //
  // It might still be the case that such an extra declaration
  // conflicts with other constraints required for a particular header
  // like e.g. non-declaredness of tags etc, but this will be handled
  // in a later step below.
  for (auto it_oi = _ai.objects_begin(); it_oi != _ai.objects_end(); ++it_oi) {
    const lp_creation_policy::externalized_symbol_modification &esm
      = it_oi->externalized_sym_mod;
    for (auto it_oidi = it_oi->init_declarators_begin();
	 it_oidi != it_oi->init_declarators_end(); ++it_oidi) {
      const ast::init_declarator &id = it_oidi->init_declarator;
      const declaration_info &di = it_oidi->enclosing_declaration;
      const bool old_is_external =
	(id.get_linkage().get_linkage_kind() ==
	 ast::linkage::linkage_kind::external);
      if (it_oi->externalize) {
	if (!esm.sym_mod.is_rename()) {
	  // Will the linkage be external?
	  const bool new_is_external
	    = ((esm.sym_mod.new_linkage == linkage_change::lc_make_extern) ||
	       (esm.sym_mod.new_linkage == linkage_change::lc_none &&
		old_is_external));
	  if (new_is_external &&
	      (!di.storage_class_specifier || id.get_initializer())) {
	    // There must not be any (tentative) definition.
	    _mark_headers_uneligible(di.declaration.get_tokens_range());

	  } else if (new_is_external && di.storage_class_specifier &&
		     (di.storage_class_specifier->get_storage_class() ==
		      ast::storage_class::sc_static)) {
	    // Any declaration having a 'static' storage class
	    // will conflict with a (preceeding) 'extern'.
	    _mark_headers_uneligible(di.declaration.get_tokens_range());

	  } else if (esm.make_pointer) {
	    _mark_headers_uneligible(di.declaration.get_tokens_range());

	  }

	} else if (old_is_external &&
		   (!di.storage_class_specifier || id.get_initializer())) {
	  // Don't leave an externally visible (tentative) definition
	  // behind after doing the externalization with renaming.
	  _mark_headers_uneligible(di.declaration.get_tokens_range());

	}

	// Externalized objects always get their initializers dropped,
	// because the closure builder doesn't consider these.
	if (id.get_initializer()) {
	  _mark_headers_uneligible(di.declaration.get_tokens_range());
	}

      } else if (it_oi->in_closure && it_oidi->get_initializer() &&
		 it_oidi->get_initializer()->rewrite_needed &&
		 !it_oidi->enclosing_declaration.enclosing_fd) {
	// The object defined at file scope is in the closure and, the
	// initializer needs to get rewritten.
	_mark_headers_uneligible(di.declaration.get_tokens_range());

      } else if (!it_oi->in_closure && old_is_external &&
		 (!di.storage_class_specifier || id.get_initializer())) {
	// Don't emit (tentative) definitions for unreachable objects
	// with external linkage.
	_mark_headers_uneligible(di.declaration.get_tokens_range());
      }
    }
  }

  for (auto it_fi = _ai.functions_begin(); it_fi != _ai.functions_end();
       ++it_fi) {
    const lp_creation_policy::externalized_symbol_modification &esm
      = it_fi->externalized_sym_mod;
    const function_definition_info * const fdi =
      it_fi->get_function_definition();
    if (fdi) {
      const ast::function_definition &fd = fdi->function_definition;
      const bool is_old_external =
	((fd.get_linkage().get_linkage_kind() ==
	  ast::linkage::linkage_kind::external) &&
	 !fd.is_gnu_inline());

      if (it_fi->externalize && !esm.sym_mod.is_rename()) {
	  _mark_headers_uneligible(fdi->function_definition.get_tokens_range());

      } else if ((fdi->is_patched || fdi->rewrite_needed)
		 && !fdi->sym_mod.is_rename()) {
	  _mark_headers_uneligible(fdi->function_definition.get_tokens_range());

      } else if (is_old_external) {
	// For simplicity, don't emit unreachable function definitions
	// with external visibility. First of all, it's uncommon to
	// have function definitions with external linkage in public
	// headers anyway.  Also, the function's linkage could in
	// principle be made internal by issuing an extra 'static'
	// declaration in front of all others, but this could
	// introduce additional constraints on header inclusion
	// eligibility at a point when it's not clear yet that the
	// definition will actually get emitted -- it might not be
	// defined within some header. So avoid excessive special
	// casing at other places by making sure we won't ever have
	// unneeded function definitions with external linkage.
	assert(!it_fi->externalize ||
	       (esm.sym_mod.is_rename() && !it_fi->in_closure));
	assert(!fdi->is_patched ||
	       (fdi->sym_mod.is_rename() && !it_fi->in_closure));
	assert(!fdi->rewrite_needed || fdi->sym_mod.is_rename());
	if (fdi->rewrite_needed || !it_fi->in_closure) {
	  _mark_headers_uneligible(fdi->function_definition.get_tokens_range());
	}
      }
    }

    for (auto it_fidi = it_fi->init_declarators_begin();
	 it_fidi != it_fi->init_declarators_end(); ++it_fidi) {
      const ast::init_declarator &id = it_fidi->init_declarator;
      const declaration_info &di = it_fidi->enclosing_declaration;

      if (it_fi->externalize && !esm.sym_mod.is_rename()) {
	if (esm.make_pointer) {
	    _mark_headers_uneligible(di.declaration.get_tokens_range());
	}

	// Will the linkage of the externalized instance be external?
	const bool is_external
	  = ((esm.sym_mod.new_linkage == linkage_change::lc_make_extern) ||
	     ((esm.sym_mod.new_linkage == linkage_change::lc_none) &&
	      (id.get_linkage().get_linkage_kind() ==
	       ast::linkage::linkage_kind::external)));
	if (is_external && di.storage_class_specifier &&
	    (di.storage_class_specifier->get_storage_class() ==
	     ast::storage_class::sc_static)) {
	  // Any declaration having a 'static' storage class
	  // will conflict with a (preceeding) 'extern'.
	  _mark_headers_uneligible(di.declaration.get_tokens_range());
	}
      }

      if (fdi && (fdi->is_patched || it_fi->in_closure) &&
	  !fdi->sym_mod.is_rename()) {
	// Will the linkage of the patched resp. ->in_closure instance
	// be external?
	const bool is_external
	  = ((fdi->sym_mod.new_linkage == linkage_change::lc_make_extern) ||
	     ((fdi->sym_mod.new_linkage == linkage_change::lc_none) &&
	      (id.get_linkage().get_linkage_kind() ==
	       ast::linkage::linkage_kind::external)));
	if (is_external && di.storage_class_specifier &&
	    (di.storage_class_specifier->get_storage_class() ==
	     ast::storage_class::sc_static)) {
	  // Any declaration having a 'static' storage class
	  // will conflict with a (preceeding) 'extern'.
	  _mark_headers_uneligible(di.declaration.get_tokens_range());
	}
      }
    }
  }

  // Now, after having handled the self-conflicts, a header might
  // still be uneligible for inclusion from the live patch, because
  // ordering constraints of to be replicated declarations and
  // definitions cannot be fulfilled.
  //
  // There's one minor complication because headers don't necessarily
  // nest with AST entities. For example, consider two headers h1 and
  // h2 where h1's end and h2's beginning both are located within some
  // common declaration. If both h1 and h2 were to be included from
  // the live patch, all extra declarations and definitions must be
  // placed either in front of h1 or after h2.
  //
  // To handle these cases properly, first observe that if some header
  // is eligible for inclusion, then all the child headers it includes
  // will also be. That is so, because the constraints for parent
  // headers are a superset of those for the child headers.  Thus, in
  // what follows, we'll only inspect headers whose parents are all
  // non-eligible for inclusion. Whenever we encounter such a header
  // whose beginning is located in the middle of some declaration or
  // definition, we extend the currently considered range by moving
  // its beginning right in front of this AST entity. If the new
  // beginning is located in some header eligible for inclusion,
  // we repeat this step with the topmost such header.
  const auto &his = _ai.header_infos;

  // Do a DFS pre walk on the header tree.
  const pp_result::header_inclusion_roots &hrs =
    _ai.atu.get_pp_result().get_header_inclusion_roots();

  for (auto &hr : hrs) {
    if (his[hr.get_id()].eligible)
      _check_header_ordering_constraints(hr);

    const auto chlds_end =
      hr.header_inclusions_recursive_end();
    for (auto it_h = hr.header_inclusions_recursive_begin();
	 it_h != chlds_end; ++it_h) {
      auto id = it_h->get_id();
      if (!his[id].eligible)
	continue;

      // If any of the parent headers is eligible, this one
      // will automatically satisfy all ordering constraints.
      bool eligible_parent = false;
      assert(it_h->get_parent());
      const pp_result::header_inclusion_node &p =
	it_h->get_parent()->get_containing_header();

      if (his[p.get_id()].eligible)
	continue;

      _check_header_ordering_constraints(*it_h);
    }
  }
}

void _conflicting_header_finder::
_mark_headers_uneligible(const pp_tokens_range &r)
{
  // Mark all headers intersecting the given range and their
  // parents as uneligible for inclusion.
  const pp_result &ppr = _ai.atu.get_pp_result();
  const raw_pp_tokens_range &raw_range = ppr.pp_tokens_range_to_raw(r);

  const auto it_end = ppr.intersecting_sources_end(raw_range);
  for (auto it = ppr.intersecting_sources_begin(raw_range);
       it != it_end; ++it) {
    _ai.mark_header_uneligible(_ai.header_infos[it->get_id()]);
  }
}

void _conflicting_header_finder::
_check_header_ordering_constraints(const pp_result::header_inclusion_node &h)
{
  using linkage_change =
    lp_creation_policy::symbol_modification::linkage_change;

  auto &his = _ai.header_infos;
  auto &hi = his[h.get_id()];
  assert(hi.eligible);

  // We are going to check below whether or not declarations and
  // definitions can be replicated in front of resp. after the
  // header. However, header files don't necessarily nest properly
  // with AST productions. In a first step, find a minimum range
  // containg the header file and not starting or ending in the middle
  // of some external declarations.

  // See if the beginning of the header is located in the middle of some
  // external declaration.
  const pp_result &ppr = _ai.atu.get_pp_result();
  raw_pp_tokens_range r = h.get_range();
  while (true) {
    const raw_pp_tokens_range r_begin{r.begin, r.begin};
    auto it_ed_begin = _ai.intersecting_external_declarations_begin(r_begin);
    auto it_ed_end = _ai.intersecting_external_declarations_end(r_begin);

    if (it_ed_begin == it_ed_end)
      break;

    const raw_pp_tokens_range ed_r =
      ppr.pp_tokens_range_to_raw(it_ed_begin->get_tokens_range());
    assert(ed_r.begin < r.begin);
    r.begin = ed_r.begin;

    // See if the new r.begin is located within some header eligible
    // for inclusion. If so, take the topmost such header and move
    // r.begin to its beginning.
    const raw_pp_tokens_range ed_begin{r.begin, r.begin + 1};
    const auto it_h = ppr.intersecting_sources_begin(ed_begin);
    if (it_h == ppr.intersecting_sources_end(ed_begin))
      break;

    if (!his[it_h->get_id()].eligible)
      break;

    const pp_result::header_inclusion_node *topmost_eligible = &*it_h;
    while (topmost_eligible->get_parent()) {
      const pp_result::header_inclusion_node &parent_header
	= topmost_eligible->get_parent()->get_containing_header();
      if (!his[parent_header.get_id()].eligible)
	break;

      topmost_eligible = &parent_header;
    }

    // We never check childs of eligible headers. Thus, topmost_eligible
    // is not a parent of h.
    assert(topmost_eligible->get_range().end <= h.get_range().begin);
    assert(topmost_eligible->get_range().begin < r.begin);
    r.begin = topmost_eligible->get_range().begin;
  }

  // Ok, we've perhaps decreased the range's ->begin now such that it
  // isn't located in the middle of some external declaration or some
  // preceeding eligible header anymore. Now handle it's ->end: if it
  // is located in the middle of some external declaration, move it
  // past that.
  {
    const raw_pp_tokens_range r_end{r.end, r.end};
    auto it_ed_begin = _ai.intersecting_external_declarations_begin(r_end);
    auto it_ed_end = _ai.intersecting_external_declarations_end(r_end);

    if (it_ed_begin != it_ed_end) {
      const raw_pp_tokens_range ed_r =
	ppr.pp_tokens_range_to_raw((it_ed_end - 1)->get_tokens_range());
      assert(r.end < ed_r.end);
      r.end = ed_r.begin;
    }
  }

  // Now that the proper range containing the header in question has
  // been determined, check if there will be any conflicts due to
  // additional declarations and definitions to be replicated before
  // or after the header.
  //
  // An extra "static" declaration for a certain object or function
  // might have to get emitted in front of the header for making its
  // linkage internal. The possible cases are:
  // 1. Some object is to be externalized w/o renaming (and w/o
  //    ->make_pointer) and its linkage needs to get turned into
  //    internal linkage.
  // 2. Some function is to be externalized w/o renaming (and w/o
  //    ->make_pointer) and its linkage needs to get turned into
  //    internal linkage.
  // 3. Some non-renamed ->is_patched or ->in_closure function
  //    needs to get its linkage turned into internal linkage.
  //
  // Likewise, anything which will be renamed will be replicated
  // after the header:
  // - declarations for objects to be externalized with renaming.
  // - declaration for functions to be externalized with renaming.
  // - declarations and definitions for functions which are patched or
  //   need to get rewritten otherwise and which are to be renamed.
  //
  // In any case, placing additional declarations before or after the
  // block can introduce conflicts and render the header non-eligible
  // for inclusion from the live patch.
  //
  // For example, any declarations to be replicated in front of the
  // header might depend on declarations like typedefs provided by
  // that header only.
  //
  // Also, albeit certainly not of any relevance in practice,
  // dependencies on struct/union/enum tags' non-declaredness must be
  // considered. That is, any declaration or function definition from
  // the header might in theory depend on the non-declaredness of some
  // tag declared by some later declaration in that header which is
  // about to be replicated in front of it. Likewise, some declaration
  // or definiton to be replicated after the header could depend on
  // the non-declaredness of some tag being declared by some
  // subsequent declaration in that header.
  //
  // Iterate twice through all external declarations within the block:
  // - in the first run, check the compatibility of declarations to be
  //   replicated in front of the header while simultaneously
  //   collecting all declarared tags and
  // - in the second run, check for incompatibilities of declarations
  //   to be replicated after the header due to tags declared within
  //   the header.
  const auto it_ed_begin =
    _ai.intersecting_external_declarations_begin(r.begin);
  const auto it_ed_end = _ai.intersecting_external_declarations_end(r.end);

  if (it_ed_begin == it_ed_end) {
    hi.only_directives = true;
    return;
  }

  deps_on_tag_non_decls block_deps_on_tag_non_decls;
  std::set<std::string> block_tag_decls;
  for (auto it_ed = it_ed_begin; it_ed != it_ed_end; ++it_ed) {
    auto &&register_tags =
      [&](const tags_infos &ti) {
	const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();

	ti.for_each_tag_recursive
	  (wrap_callables<default_action_nop>
	   ([&](const sou_def_info &soud) {
	      block_deps_on_tag_non_decls += soud.deps.on_tag_non_decls;
	      assert(soud.soud.has_id());
	      block_tag_decls.insert(toks[soud.soud.get_id_tok()].get_value());
	      return true;
	    },
	    [&](const enum_def_info &ed) {
	      block_deps_on_tag_non_decls += ed.deps.on_tag_non_decls;
	      if (ed.ed.has_id())
		block_tag_decls.insert(toks[ed.ed.get_id_tok()].get_value());
	      return true;
	    },
	    [&](const sou_ref_info &sour) {
	      block_tag_decls.insert(toks[sour.sour.get_id_tok()].get_value());
	    },
	    [&](const enum_ref_info &er) {
	      block_tag_decls.insert(toks[er.er.get_id_tok()].get_value());
	    }));
      };

    auto &&fdi_statification_needed =
      [&](const function_definition_info &fdi) {
	const ast::linkage &l = fdi.function_definition.get_linkage();
	if (!l.is_first_in_chain())
	  return false;

	if (l.get_linkage_kind() == ast::linkage::linkage_kind::internal) {
	  return false;
	}

	const function_info &fi = fdi.fi;
	// At this point, function definitions with external linkage
	// in headers can be encountered only with ->in_closure.
	assert((fi.in_closure && !fdi.rewrite_needed &&
		!fdi.sym_mod.is_rename()) ||
	       fdi.function_definition.is_gnu_inline());
	return (fdi.sym_mod.new_linkage == linkage_change::lc_make_static);
      };

    auto &&fidi_statification_needed =
      [&](const function_init_declarator_info &fidi) {
	const ast::linkage &l = fidi.init_declarator.get_linkage();
	if (!l.is_first_in_chain())
	  return false;

	if (l.get_linkage_kind() == ast::linkage::linkage_kind::internal)
	  return false;

	const function_info &fi = fidi.fi;
	const function_definition_info * const fdi =
	  fi.get_function_definition();
	if (fdi && (fdi->is_patched || fi.in_closure) &&
	    !fdi->sym_mod.is_rename()) {
	  return (fdi->sym_mod.new_linkage == linkage_change::lc_make_static);

	} else if (fi.externalize &&
		   !fi.externalized_sym_mod.sym_mod.is_rename()) {
	  return (fi.externalized_sym_mod.sym_mod.new_linkage ==
		  linkage_change::lc_make_static);

	} else {
	  // No function definition will be emitted for this
	  // unreachable function instance. So no statification
	  // needed.
	  return false;

	}
      };

    auto &&oidi_statification_needed =
      [&](const object_init_declarator_info &oidi) {
	const ast::linkage &l = oidi.init_declarator.get_linkage();
	if (!l.is_first_in_chain())
	  return false;

	if (l.get_linkage_kind() == ast::linkage::linkage_kind::internal)
	  return false;

	const object_info &oi = oidi.oi;
	if (oi.externalize && !oi.externalized_sym_mod.sym_mod.is_rename()) {
	  return (oi.externalized_sym_mod.sym_mod.new_linkage ==
		  linkage_change::lc_make_static);
	}

	return false;
      };

    auto &&any_dep_to_block
      = [&](const deps_on_types &deps) {
	  auto &&is_range_in_block =
	    [&](const pp_tokens_range &rng) {
	      const raw_pp_tokens_range raw_rng
		= _ai.atu.get_pp_result().pp_tokens_range_to_raw(rng);
	      return !(r < raw_rng) && !(raw_rng < r);
	    };

	  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();

	  for (auto &d : deps.on_sou_decls) {
	    const pp_token_index id_tok =
	      [&](){
		switch (d.k) {
		case dep_on_sou_decl::kind::def:
		  return d.get_soud().soud.get_id_tok();
		case dep_on_sou_decl::kind::ref:
		  return d.get_sour().sour.get_id_tok();
	      }
	      }();

	    const std::string &id = toks[id_tok].get_value();
	    if (block_deps_on_tag_non_decls.contains(id)) {
	      // There can't be a declaration of this tag before the
	      // header. In particular, none may get added.
	      return true;
	    }
	  }

	  for (auto &d : deps.on_sou_defs) {
	    if (is_range_in_block(d.soud.soud.get_tokens_range()))
	      return true;
	  }

	  for (auto &d : deps.on_enum_decls) {
	    const pp_token_index id_tok =
	      [&](){
		switch (d.k) {
		case dep_on_enum_decl::kind::def:
		  return d.get_ed().ed.get_id_tok();
		case dep_on_enum_decl::kind::ref:
		  return d.get_er().er.get_id_tok();
		}
	      }();

	    const std::string &id = toks[id_tok].get_value();
	    if (block_deps_on_tag_non_decls.contains(id)) {
	      // There can't be a declaration of this tag before the
	      // header. In particular, it can't be added.
	      return true;
	    }
	  }

	  for (auto &d : deps.on_enum_defs) {
	    if (is_range_in_block(d.ed.ed.get_tokens_range()))
	      return true;
	  }

	  for (auto &d : deps.on_typedefs) {
	    const ast::declaration &decl
	      = d.tdidi.enclosing_declaration.declaration;
	    if (is_range_in_block(decl.get_tokens_range()))
	      return true;
	  }

	  for (auto &d : deps.on_decl_types) {
	    if (d.need_composite) {
	      switch (d.get_kind()) {
	      case dep_on_decl_type::kind::k_function_definition:
		{
		  const function_definition_info &fdi = d.get_fdi();
		  if ((is_range_in_block
		       (fdi.function_definition.get_tokens_range())) &&
		      !fdi_statification_needed(fdi)) {
		    return true;
		  }
		}
		break;

	      case dep_on_decl_type::kind::k_function_init_declarator:
		{
		  const function_init_declarator_info &fidi = d.get_fidi();
		  if ((is_range_in_block
		       (fidi.init_declarator.get_tokens_range())) &&
		      !fidi_statification_needed(fidi)) {
		    return true;
		  }
		}
		break;

	      case dep_on_decl_type::kind::k_object_init_declarator:
		{
		  const object_init_declarator_info &oidi = d.get_oidi();
		  if ((is_range_in_block
		       (oidi.init_declarator.get_tokens_range())) &&
		      !oidi_statification_needed(oidi)) {
		    return true;
		  }
		}
		break;
	      };
	    } else {
	      // Composite not needed. See if the first visible declaration
	      // will get emitted before the header block.
	      bool found_usable = false;
	      d.for_each_visible
		(wrap_callables<no_default_action>
		 ([&](const function_definition_info &fdi) {
		    if (!(is_range_in_block
		       (fdi.function_definition.get_tokens_range())) ||
			fdi_statification_needed(fdi)) {
		      found_usable = true;
		      return false;
		    }
		    return true;
		  },
		   [&](const function_init_declarator_info &fidi) {
		     if (!(is_range_in_block
			   (fidi.init_declarator.get_tokens_range())) ||
			 fidi_statification_needed(fidi)) {
		       found_usable = true;
		       return false;
		     }
		     return true;
		   },
		   [&](const object_init_declarator_info &oidi) {
		     if (!(is_range_in_block
			   (oidi.init_declarator.get_tokens_range())) ||
			 oidi_statification_needed(oidi)) {
		       found_usable = true;
		       return false;
		     }
		     return true;
		   }));

	      if (!found_usable)
		return true;
	    }
	  }

	  return false;
	};

    auto &&can_declare_tags_before_block
      = [&](const tags_infos &tis) {
	  bool conflict = false;

	  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	  tis.for_each_tag_recursive
	    (wrap_callables<no_default_action>
	     ([&](const sou_def_info &sdi) {
		const std::string &id = toks[sdi.soud.get_id_tok()].get_value();
		if (block_deps_on_tag_non_decls.contains(id))
		  conflict = true;
		return false; // Don't recurse
	      },
	      [&](const sou_ref_info &sri) {
		const std::string &id = toks[sri.sour.get_id_tok()].get_value();
		if (block_deps_on_tag_non_decls.contains(id))
		  conflict = true;
	      },
	      [&](const enum_def_info &edi) {
		const std::string &id = toks[edi.ed.get_id_tok()].get_value();
		if (block_deps_on_tag_non_decls.contains(id))
		  conflict = true;
		return false; // Don't recurse
	      },
	      [&](const enum_ref_info &eri) {
		const std::string &id = toks[eri.er.get_id_tok()].get_value();
		if (block_deps_on_tag_non_decls.contains(id))
		  conflict = true;
	      }));

	  return !conflict;
	};

    switch (it_ed->k) {
    case external_declaration_info::kind::declaration:
      {
	const declaration_info &di = it_ed->get_declaration();

	di.for_each_deps_on_types
	  ([&](const deps_on_types &deps) {
	     for (const auto &d : deps.on_decl_types) {
	       const auto * const esm = d.get_target_externalized_sym_mod();
	       if (esm && esm->make_pointer && !esm->sym_mod.is_rename()) {
		 hi.eligible = false;
		 return;
	       }
	     }
	   });
	if (!hi.eligible)
	  return;

	bool statification_needed = false;
	di.init_declarators.for_each
	  (wrap_callables<no_default_action>
	   ([&](const function_init_declarator_info &fidi) {
	      if (fidi_statification_needed(fidi)) {
		if (any_dep_to_block(fidi.declarator_deps) ||
		    !fidi.declarator_deps.on_tag_non_decls.empty() ||
		    !can_declare_tags_before_block(fidi.child_tags)) {
		  hi.eligible = false;
		  return;
		}
		statification_needed = true;
	      }

	      block_deps_on_tag_non_decls +=
		fidi.declarator_deps.on_tag_non_decls;
	      register_tags(fidi.child_tags);
	    },
	    [&](const object_init_declarator_info &oidi) {
	      if (oidi_statification_needed(oidi)) {
		if (any_dep_to_block(oidi.declarator_deps) ||
		    !oidi.declarator_deps.on_tag_non_decls.empty() ||
		    !can_declare_tags_before_block(oidi.child_tags)) {
		  hi.eligible = false;
		  return;
		}
		statification_needed = true;
	      }

	      block_deps_on_tag_non_decls +=
		oidi.declarator_deps.on_tag_non_decls;
	      register_tags(oidi.child_tags);

	      const initializer_info *ii = oidi.get_initializer();
	      if (ii) {
		for (const auto &d : ii->deps.on_objs) {
		  const object_info &oi = d.oidi.get().oi;
		  if (oi.externalize && oi.externalized_sym_mod.make_pointer &&
		      !oi.externalized_sym_mod.sym_mod.is_rename()) {
		    hi.eligible = false;
		    return;
		  }
		}
		for (const auto &d : ii->deps.on_funs) {
		  const function_info &fi = d.get_function_info();
		  if (fi.externalize && fi.externalized_sym_mod.make_pointer &&
		      !fi.externalized_sym_mod.sym_mod.is_rename()) {
		    hi.eligible = false;
		    return;
		  }
		}

		block_deps_on_tag_non_decls +=
		  ii->deps.on_types.on_tag_non_decls;
		register_tags(ii->child_tags);
	      }
	    },
	    [&](const typedef_init_declarator_info &tidi) {
	      block_deps_on_tag_non_decls +=
		tidi.declarator_deps.on_tag_non_decls;
	      register_tags(tidi.child_tags);
	    }));

	if (!hi.eligible)
	  return;

	if (statification_needed) {
	  if (any_dep_to_block(di.declaration_specifiers_deps) ||
	      !di.declaration_specifiers_deps.on_tag_non_decls.empty() ||
	      !can_declare_tags_before_block(di.child_tags)) {
	    hi.eligible = false;
	    return;
	  }
	}

	block_deps_on_tag_non_decls +=
	  di.declaration_specifiers_deps.on_tag_non_decls;
	register_tags(di.child_tags);
      }
      break;

    case external_declaration_info::kind::static_assertion:
      {
	const static_assert_info &sai = it_ed->get_static_assert();
	for (const auto &d : sai.deps.on_decl_types) {
	  const auto * const esm = d.get_target_externalized_sym_mod();
	  if (esm && esm->make_pointer && !esm->sym_mod.is_rename()) {
	    hi.eligible = false;
	    return;
	  }
	}
	block_deps_on_tag_non_decls += sai.deps.on_tag_non_decls;
      }
      break;

    case external_declaration_info::kind::function_definition:
      {
	const function_definition_info &fdi = it_ed->get_function_definition();

	fdi.for_each_deps_on_types
	  ([&](const deps_on_types &deps) {
	     for (const auto &d : deps.on_decl_types) {
	       const auto * const esm = d.get_target_externalized_sym_mod();
	       if (esm && esm->make_pointer && !esm->sym_mod.is_rename()) {
		 hi.eligible = false;
		 return;
	       }
	     }
	   });
	if (!hi.eligible)
	  return;

	for (const auto &d : fdi.body_deps.on_objs) {
	  const object_info &oi = d.oidi.get().oi;
	  if (oi.externalize && oi.externalized_sym_mod.make_pointer &&
	      !oi.externalized_sym_mod.sym_mod.is_rename()) {
	    hi.eligible = false;
	    return;
	  }
	}
	for (const auto &d : fdi.body_deps.on_funs) {
	  const function_info &fi = d.get_function_info();
	  if (fi.externalize && fi.externalized_sym_mod.make_pointer &&
	      !fi.externalized_sym_mod.sym_mod.is_rename()) {
	    hi.eligible = false;
	    return;
	  }
	}

	if (fdi_statification_needed(fdi)) {
	  if (any_dep_to_block(fdi.declarator_deps) ||
	      !fdi.declarator_deps.on_tag_non_decls.empty() ||
	      !can_declare_tags_before_block(fdi.child_tags)) {
	    hi.eligible = false;
	    return;
	  }
	}

	block_deps_on_tag_non_decls += fdi.declarator_deps.on_tag_non_decls;
	register_tags(fdi.child_tags);
	block_deps_on_tag_non_decls += fdi.body_deps.on_types.on_tag_non_decls;

	for (const auto &di : fdi.get_declarations()) {
	  di.init_declarators.for_each
	    (wrap_callables<default_action_unreachable<void, type_set<>>::type>
	     ([&](const function_init_declarator_info &fidi) {
		block_deps_on_tag_non_decls +=
		  fidi.declarator_deps.on_tag_non_decls;
		assert(fidi.child_tags.empty());
	      },
	      [&](const object_init_declarator_info &oidi) {
		block_deps_on_tag_non_decls +=
		  oidi.declarator_deps.on_tag_non_decls;
		assert(oidi.child_tags.empty());

		const initializer_info *ii = oidi.get_initializer();
		if (ii) {
		  block_deps_on_tag_non_decls +=
		    ii->deps.on_types.on_tag_non_decls;
		  assert(ii->child_tags.empty());
		}
	      }));
	}
      }
      break;
    }
  }

  auto &&conflicts_with_tag_from_block =
    [&](const deps_on_types &deps) {
      for (const auto &tag_from_block :  block_tag_decls) {
	if (deps.on_tag_non_decls.contains(tag_from_block))
	  return true;
      }

      return false;
    };

  for (auto it_ed = it_ed_begin; hi.eligible && it_ed != it_ed_end; ++it_ed) {
    switch (it_ed->k) {
    case external_declaration_info::kind::declaration:
      {
	const declaration_info &di = it_ed->get_declaration();
	bool need_decl_specifiers = false;
	di.init_declarators.for_each
	  (wrap_callables<no_default_action>
	   ([&](const function_init_declarator_info &fidi) {
	      const function_info &fi = fidi.fi;
	      const function_definition_info * const fdi =
		fi. get_function_definition();

	      if ((fi.externalize &&
		   fi.externalized_sym_mod.sym_mod.is_rename()) ||
		  (fdi && fdi->rewrite_needed &&
		   fdi->sym_mod.is_rename())) {
		if (conflicts_with_tag_from_block(fidi.declarator_deps)) {
		  hi.eligible = false;
		  return;
		}
		need_decl_specifiers = true;
	      }
	    },
	    [&](const object_init_declarator_info &oidi) {
	      const object_info &oi = oidi.oi;
	      if (oi.externalize &&
		  oi.externalized_sym_mod.sym_mod.is_rename()) {
		if (conflicts_with_tag_from_block(oidi.declarator_deps)) {
		  hi.eligible = false;
		  return;
		}
		need_decl_specifiers = true;
	      }
	    },
	    [&](const typedef_init_declarator_info &tidi) {
	    }));

	if (!hi.eligible)
	  return;

	if (need_decl_specifiers) {
	  if (conflicts_with_tag_from_block(di.declaration_specifiers_deps)) {
	    hi.eligible = false;
	    return;
	  }
	}
      }
      break;

    case external_declaration_info::kind::static_assertion:
      break;

    case external_declaration_info::kind::function_definition:
      {
	const function_definition_info &fdi = it_ed->get_function_definition();
	const function_info &fi = fdi.fi;

	assert(!fi.externalize || fi.externalized_sym_mod.sym_mod.is_rename());
	assert(!fdi.rewrite_needed || fdi.sym_mod.is_rename());
	if (fi.externalize || fdi.rewrite_needed) {
	  fdi.for_each_deps_on_types
	    ([&](const deps_on_types &deps) {
	       if (conflicts_with_tag_from_block(deps))
		 hi.eligible = false;
	     });
	  if (!hi.eligible)
	    return;
	}
      }
    }
  }
}



namespace
{
  class _lp_deps_resolver
  {
  public:
    _lp_deps_resolver(ast_info &ai,
		      const lp_creation_policy &pol,
		      code_remarks &remarks) noexcept;

    void operator()();

  private:
    void _queue_deps(const deps_on_types &deps, const bool ref_from_header);

    void _queue_tag_def(sou_def_info &soudi);
    void _queue_tag_def(enum_def_info &edi);
    void _queue_all_descendant_tag_defs_and_decls(tags_infos &child_tags);
    void _queue_tag_defs_at_and_below(sou_def_info &soudi);
    void _queue_tag_defs_at_and_below(enum_def_info &edi);
    void _queue_all_related_tag_defs(sou_def_info &soudi);
    void _queue_all_related_tag_defs(enum_def_info &edi);
    void _emit_child_tag_decls(tags_infos &child_tags);

    void _queue_dep_on_tag_decl(const dep_on_sou_decl &dep);
    void _queue_dep_on_tag_decl(const dep_on_enum_decl &dep);

    void _require_allocatable_type(const types::addressable_type &at);

    bool _queue_includes_for(const pp_tokens_range &r);
    void _queue_header_include(header_info &hi);

    std::pair<ast_info::external_declaration_iterator,
	      ast_info::external_declaration_iterator>
    _add_include(header_info &hi) const;

    void _queue_deps_on_decls_from_header(const deps_on_funs &deps);
    void _queue_deps_on_decls_from_header(const deps_on_objs &deps);

    struct _orig_decls_wl
    {
      struct entry
      {
	entry(function_definition_info &_fdi) noexcept;

	entry(function_init_declarator_info &_fidi) noexcept;

	entry(object_init_declarator_info &_oidi) noexcept;

	dep_on_decl_type::kind k;

	union
	{
	  function_definition_info * const fdi;
	  function_init_declarator_info * const fidi;
	  object_init_declarator_info * const oidi;
	};
      };

      void add(const dep_on_decl_type &dep, const bool can_rewrite_reference);
      bool empty() const noexcept
      { return wl.empty(); }

      std::queue<entry> wl;
    };

    ast_info &_ai;
    const lp_creation_policy &_pol;
    code_remarks &_remarks;

    deps_on_sou_decls _deps_on_sou_decls;
    deps_on_enum_decls _deps_on_enum_decls;
    std::queue<std::reference_wrapper<sou_def_info>> _sou_defs_wl;
    std::queue<std::reference_wrapper<enum_def_info>> _enum_defs_wl;
    std::queue<std::reference_wrapper<typedef_init_declarator_info>>
	_typedefs_wl;
    _orig_decls_wl _orig_decls_wl;
  };
}

_lp_deps_resolver::_lp_deps_resolver(ast_info &ai,
				     const lp_creation_policy &pol,
				     code_remarks &remarks) noexcept
  : _pol(pol), _ai(ai), _remarks(remarks)
{}


void _lp_deps_resolver::operator()()
{
  // Retain all #includes from the input source file.
  for (auto &hi : _ai.header_infos) {
    if (hi.eligible)
      _queue_header_include(hi);
  }

  using linkage_change =
    lp_creation_policy::symbol_modification::linkage_change;

  // First, collect all type dependencies from the closure.
  for (auto it_oi = _ai.objects_begin(); it_oi != _ai.objects_end(); ++it_oi) {
    if (!it_oi->externalize && !it_oi->in_closure)
      continue;

    for (auto it_oidi = it_oi->init_declarators_begin();
	 it_oidi != it_oi->init_declarators_end(); ++it_oidi) {
      declaration_info &di = it_oidi->enclosing_declaration;

      bool need_complete_type = false;
      if (it_oi->externalize) {
	const bool old_is_external =
	  (it_oidi->init_declarator.get_linkage().get_linkage_kind() ==
	   ast::linkage::linkage_kind::external);
	const lp_creation_policy::externalized_symbol_modification &esm
	  = it_oi->externalized_sym_mod;
	const bool new_is_external =
	  ((esm.sym_mod.new_linkage == linkage_change::lc_make_extern) ||
	   (esm.sym_mod.new_linkage == linkage_change::lc_none &&
	    old_is_external));

	if (!esm.make_pointer && !new_is_external) {
	  _require_allocatable_type
	    (*it_oidi->init_declarator.get_declarator().get_innermost_type());
	}

	bool from_header = false;
	if (!esm.make_pointer && !esm.sym_mod.is_rename())
	  from_header = _queue_includes_for(di.declaration.get_tokens_range());
	if (!from_header) {
	  _queue_deps(it_oidi->declarator_deps, false);
	  _emit_child_tag_decls(it_oidi->child_tags);
	  _queue_deps(di.declaration_specifiers_deps, false);
	  _emit_child_tag_decls(di.child_tags);
	}

      } else if (it_oi->in_closure) {
	const bool from_header =
	  _queue_includes_for(di.declaration.get_tokens_range());
	if (!from_header) {
	  _queue_deps(it_oidi->declarator_deps, false);
	  _emit_child_tag_decls(it_oidi->child_tags);
	  _queue_deps(di.declaration_specifiers_deps, false);
	  _emit_child_tag_decls(di.child_tags);
	}

	if (!di.storage_class_specifier ||
	    (di.storage_class_specifier->get_storage_class() ==
	     ast::storage_class::sc_static)) {
	  _require_allocatable_type
	    (*it_oidi->init_declarator.get_declarator().get_innermost_type());

	  initializer_info * const ii = it_oidi->get_initializer();
	  if (ii && !from_header) {
	    _queue_deps(ii->deps.on_types, false);
	    _emit_child_tag_decls(ii->child_tags);
	  }
	} else {
	  assert(!it_oidi->get_initializer());
	}
      }
    }
  }

  for (auto it_fi = _ai.functions_begin(); it_fi != _ai.functions_end();
       ++it_fi) {
    function_definition_info * const fdi =
      it_fi->get_function_definition();
    if (!it_fi->externalize && !it_fi->in_closure && (!fdi || !fdi->is_patched))
      continue;

    if (fdi) {
      if (it_fi->externalize) {
	// Externalized function definitions will have their body
	// dropped and can never come from headers.
	_queue_deps(fdi->declarator_deps, false);
	_emit_child_tag_decls(fdi->child_tags);
      }

      if (it_fi->in_closure || fdi->is_patched) {
	bool from_header = false;
	if (it_fi->in_closure && !fdi->rewrite_needed) {
	  from_header =
	    _queue_includes_for(fdi->function_definition.get_tokens_range());
	}

	if (!from_header) {
	  _queue_deps(fdi->declarator_deps, false);
	  _emit_child_tag_decls(fdi->child_tags);
	  fdi->for_each_body_deps_on_types
	    ([&](const deps_on_types &deps) {
	       _queue_deps(deps, false);
	     });
	}
      }
    }

    for (auto it_fidi = it_fi->init_declarators_begin();
	 it_fidi != it_fi->init_declarators_end(); ++it_fidi) {
      declaration_info &di = it_fidi->enclosing_declaration;

      if (it_fi->externalize) {
	bool from_header = false;
	if (!it_fi->externalized_sym_mod.make_pointer &&
	    !it_fi->externalized_sym_mod.sym_mod.is_rename()) {
	  from_header = _queue_includes_for(di.declaration.get_tokens_range());
	}

	if (!from_header) {
	  _queue_deps(it_fidi->declarator_deps, false);
	  _emit_child_tag_decls(it_fidi->child_tags);
	  _queue_deps(di.declaration_specifiers_deps, false);
	  _emit_child_tag_decls(di.child_tags);
	}
      }

      if (it_fi->in_closure || (fdi && fdi->is_patched)) {
	bool from_header = false;
	if (!fdi || !fdi->sym_mod.is_rename()) {
	  from_header = _queue_includes_for(di.declaration.get_tokens_range());
	}

	if (!from_header) {
	  _queue_deps(it_fidi->declarator_deps, false);
	  _emit_child_tag_decls(it_fidi->child_tags);
	  _queue_deps(di.declaration_specifiers_deps, false);
	  _emit_child_tag_decls(di.child_tags);
	}
      }
    }
  }

  // Recursively process the dependencies on types, declarations and
  // headers.
  while (!_sou_defs_wl.empty() || !_enum_defs_wl.empty() ||
	 !_typedefs_wl.empty() || !_orig_decls_wl.empty()) {
    while (!_sou_defs_wl.empty()) {
      sou_def_info &soudi = _sou_defs_wl.front().get();
      _sou_defs_wl.pop();

      const bool from_header =
	_queue_includes_for(soudi.soud.get_tokens_range());
      if (!from_header)
	_queue_deps(soudi.deps, false);
    }

    while (!_enum_defs_wl.empty()) {
      enum_def_info &edi = _enum_defs_wl.front().get();
      _enum_defs_wl.pop();

      const bool from_header =
	_queue_includes_for(edi.ed.get_tokens_range());
      if (!from_header)
	_queue_deps(edi.deps, false);
    }

    while (!_typedefs_wl.empty()) {
      typedef_init_declarator_info &tdidi = _typedefs_wl.front().get();
      _typedefs_wl.pop();

      declaration_info &di = tdidi.enclosing_declaration;
      const bool from_header =
	_queue_includes_for(di.declaration.get_tokens_range());
      if (!from_header) {
	_queue_deps(tdidi.declarator_deps, false);
	_emit_child_tag_decls(tdidi.child_tags);
	_queue_deps(di.declaration_specifiers_deps, false);
	_emit_child_tag_decls(di.child_tags);
      }
    }

    while (!_orig_decls_wl.empty()) {
      // For all deps on this worklist, it is already known that we
      // can't piggy-back on some declaration reachable from the live
      // patch.
      const _orig_decls_wl::entry decl = _orig_decls_wl.wl.front();
      _orig_decls_wl.wl.pop();

      switch(decl.k) {
      case dep_on_decl_type::kind::k_function_definition:
	{
	  function_definition_info &fdi = *decl.fdi;
	  const bool from_header =
	    _queue_includes_for(fdi.function_definition.get_tokens_range());
	  if (!from_header) {
	    _queue_deps(fdi.declarator_deps, false);
	    _emit_child_tag_decls(fdi.child_tags);
	  }
	}
	break;

      case dep_on_decl_type::kind::k_function_init_declarator:
	{
	  function_init_declarator_info &fidi = *decl.fidi;
	  declaration_info &di = fidi.enclosing_declaration;
	  const bool from_header =
	    _queue_includes_for(di.declaration.get_tokens_range());
	  if (!from_header) {
	    _queue_deps(fidi.declarator_deps, false);
	    _emit_child_tag_decls(fidi.child_tags);
	    _queue_deps(di.declaration_specifiers_deps, false);
	    _emit_child_tag_decls(di.child_tags);
	  }
	}
	break;
      case dep_on_decl_type::kind::k_object_init_declarator:
	{
	  object_init_declarator_info &oidi = *decl.oidi;
	  declaration_info &di = oidi.enclosing_declaration;
	  const bool from_header =
	    _queue_includes_for(di.declaration.get_tokens_range());
	  if (!from_header) {
	    _queue_deps(oidi.declarator_deps, false);
	    _emit_child_tag_decls(oidi.child_tags);
	    _queue_deps(di.declaration_specifiers_deps, false);
	    _emit_child_tag_decls(di.child_tags);
	  }
	}
	break;
      }
    }
  }

  // Finally, mark the tag declarations still not made available one
  // way or the other as needed.
  for (const auto &dep : _deps_on_sou_decls) {
    sou_info &soui = dep.get_sou_info();
    const auto &dn = dep.get_decl_list_node();
    sou_def_info *soudi = soui.get_soud();

    if (soudi && !soudi->soud.get_decl_list_node().is_visible_from(dn))
      soudi = nullptr;

    if (soudi && (soudi->emit_def || soudi->emit_tag_decl))
      continue;

    bool found = false;
    for (auto it_sour = soui.sours_begin();
	 (it_sour != soui.sours_end() &&
	  it_sour->sour.get_decl_list_node().is_visible_from(dn));
	 ++it_sour) {
      if (it_sour->emit_tag_decl) {
	found = true;
	break;
      }
    }
    if (found)
      continue;

    // If all candidate tag declarations had lived within some
    // struct/union/enum definition trees, the dependency queueing
    // code in _queue_dep_on_tag_decl() would have taken care of this
    // already (by emitting the whole first such tree). By now, it is
    // guaranteed that there exists at least one declaration of this
    // tag which is not within some struct/union/enum definition and
    // which is visible. Pick the first one in file order.
    if (soudi &&
	(soudi->parent.k == tag_parent::kind::k_sou_def ||
	 soudi->parent.k == tag_parent::kind::k_enum_def)) {
      soudi = nullptr;
    }

    for (auto it_sour = soui.sours_begin();
	 (it_sour != soui.sours_end() &&
	  (!soudi ||
	   !(soudi->soud.get_decl_list_node().is_visible_from
	     (it_sour->sour.get_decl_list_node()))));
	 ++it_sour) {
      if (it_sour->parent.k != tag_parent::kind::k_sou_def &&
	  it_sour->parent.k != tag_parent::kind::k_enum_def) {
	assert(it_sour->sour.get_decl_list_node().is_visible_from(dn));
	it_sour->emit_tag_decl = true;
	found = true;
	break;
      }
    }
    if (!found) {
      assert(soudi);
      assert(soudi->parent.k != tag_parent::kind::k_sou_def &&
	     soudi->parent.k != tag_parent::kind::k_enum_def);
      assert(soudi->soud.get_decl_list_node().is_visible_from(dn));
      assert(!soudi->emit_def);
      soudi->emit_tag_decl = true;
    }
  }
  _deps_on_sou_decls.clear();

  for (const auto &dep : _deps_on_enum_decls) {
    enum_info &ei = dep.get_enum_info();
    const auto &dn = dep.get_decl_list_node();
    enum_def_info *edi = ei.get_ed();

    if (edi && !edi->ed.get_decl_list_node().is_visible_from(dn))
      edi = nullptr;

    if (edi && (edi->emit_def || edi->emit_tag_decl))
      continue;

    bool found = false;
    for (auto it_er = ei.ers_begin();
	 (it_er != ei.ers_end() &&
	  it_er->er.get_decl_list_node().is_visible_from(dn));
	 ++it_er) {
      if (it_er->emit_tag_decl) {
	found = true;
	break;
      }
    }
    if (found)
      continue;

    // If all candidate tag declarations had lived within some
    // struct/union/enum definition trees, the dependency queueing
    // code in _queue_dep_on_tag_decl() would have taken care of this
    // already (by emitting the whole first such tree). By now, it is
    // guaranteed that there exists at least one declaration of this
    // tag which is not within some struct/union/enum definition and
    // which is visible. Pick the first one in file order.
    if (edi &&
	(edi->parent.k == tag_parent::kind::k_sou_def ||
	 edi->parent.k == tag_parent::kind::k_enum_def)) {
      edi = nullptr;
    }

    for (auto it_er = ei.ers_begin();
	 (it_er != ei.ers_end() &&
	 (!edi ||
	  !(edi->ed.get_decl_list_node().is_visible_from
	    (it_er->er.get_decl_list_node()))));
	 ++it_er) {
      if (it_er->parent.k != tag_parent::kind::k_sou_def &&
	  it_er->parent.k != tag_parent::kind::k_enum_def) {
	assert(it_er->er.get_decl_list_node().is_visible_from(dn));
	it_er->emit_tag_decl = true;
	found = true;
	break;
      }
    }

    if (!found) {
      assert(edi);
      assert(edi->parent.k != tag_parent::kind::k_sou_def &&
	     edi->parent.k != tag_parent::kind::k_enum_def);
      assert(edi->ed.get_decl_list_node().is_visible_from(dn));
      assert(!edi->emit_def);
      edi->emit_tag_decl = true;
    }
  }
  _deps_on_enum_decls.clear();
}

void _lp_deps_resolver::_queue_deps(const deps_on_types &deps,
				    const bool ref_from_header)
{
  for (const auto &d : deps.on_sou_decls)
    _queue_dep_on_tag_decl(d);
  for (const auto &d : deps.on_enum_decls)
    _queue_dep_on_tag_decl(d);

  for (const auto &d : deps.on_sou_defs)
    _queue_all_related_tag_defs(d.soud);

  for (const auto &d : deps.on_enum_defs)
    _queue_all_related_tag_defs(d.ed);

  for (const auto &d : deps.on_typedefs) {
    if (!d.tdidi.emit) {
      d.tdidi.emit = true;
      _typedefs_wl.push(std::ref(d.tdidi));
    }
  }

  for (const auto &d : deps.on_decl_types)
    _orig_decls_wl.add(d, !ref_from_header);
}

void _lp_deps_resolver::_queue_tag_def(sou_def_info &soudi)
{
  assert(!soudi.emit_def);
  soudi.emit_def = true;
  soudi.emit_tag_decl = false;
  _sou_defs_wl.push(std::ref(soudi));
}

void _lp_deps_resolver::_queue_tag_def(enum_def_info &edi)
{
  assert(!edi.emit_def);
  edi.emit_def = true;
  edi.emit_tag_decl = false;
  _enum_defs_wl.push(std::ref(edi));
}

void _lp_deps_resolver::
_queue_all_descendant_tag_defs_and_decls(tags_infos &child_tags)
{
  child_tags.for_each_tag_recursive
    (wrap_callables<no_default_action>
     ([&](sou_def_info &child_soudi) {
	_queue_tag_def(child_soudi);
	return true;
      },
      [&](enum_def_info &child_edi) {
	_queue_tag_def(child_edi);
	return true;
      },
      [](sou_ref_info &child_souri) {
	child_souri.emit_tag_decl = true;
      },
      [](enum_ref_info &child_eri) {
	child_eri.emit_tag_decl = true;
      }));
};

void _lp_deps_resolver::_queue_tag_defs_at_and_below(sou_def_info &soudi)
{
  if (soudi.emit_def)
    return;

  _queue_tag_def(soudi);
  _queue_all_descendant_tag_defs_and_decls(soudi.child_tags);
}

void _lp_deps_resolver::_queue_tag_defs_at_and_below(enum_def_info &edi)
{
  if (edi.emit_def)
    return;

  _queue_tag_def(edi);
  _queue_all_descendant_tag_defs_and_decls(edi.child_tags);
}

void _lp_deps_resolver::_queue_all_related_tag_defs(sou_def_info &soudi)
{
  // When encountering a dependency to a struct/union/enum definition,
  // queue dependencies on the outermost tag definition containing the
  // one in question as well as all the ones nested within that one.
  if (soudi.emit_def)
    return;

  soudi.on_outermost_tag_def
    (wrap_callables<default_action_nop>
     ([&](sou_def_info &outermost_soudi) {
	_queue_tag_defs_at_and_below(outermost_soudi);
      },
      [&](enum_def_info &outermost_edi) {
	_queue_tag_defs_at_and_below(outermost_edi);
      }));
}

void _lp_deps_resolver::_queue_all_related_tag_defs(enum_def_info &edi)
{
  // When encountering a depedency to a struct/union/enum definition,
  // queue dependencies on the outermost tag definition containing the
  // one in question as well as all the ones nested within that one.
  if (edi.emit_def)
    return;

  edi.on_outermost_tag_def
    (wrap_callables<default_action_nop>
     ([&](sou_def_info &outermost_soudi) {
	_queue_tag_defs_at_and_below(outermost_soudi);
      },
      [&](enum_def_info &outermost_edi) {
	_queue_tag_defs_at_and_below(outermost_edi);
      }));
}

void _lp_deps_resolver::_emit_child_tag_decls(tags_infos &child_tags)
{
  child_tags.for_each_child_tag
    (wrap_callables<no_default_action>
     ([&](sou_def_info &soudi) {
	if (!soudi.emit_def)
	  soudi.emit_tag_decl = true;
      },
      [&](enum_def_info &edi) {
	if (!edi.emit_def)
	  edi.emit_tag_decl = true;
      },
      [&](sou_ref_info &souri) {
	souri.emit_tag_decl = true;
      },
      [&](enum_ref_info &eri) {
	eri.emit_tag_decl = true;
      }));
}

void _lp_deps_resolver::_queue_dep_on_tag_decl(const dep_on_sou_decl &dep)
{
  // If no update, return early.
  if (!_deps_on_sou_decls.add(dep))
    return;

  // If all reachable tag declarations live within some
  // struct/union/enum definition, emit the whole first such
  // definition tree.
  const auto &dn = dep.get_decl_list_node();
  sou_info &soui = dep.get_sou_info();
  tag_parent *first_parent = nullptr;
  sou_def_info * const soudi = soui.get_soud();

  if (soudi && soudi->soud.get_decl_list_node().is_visible_from(dn)) {
    if (soudi->parent.k != tag_parent::kind::k_sou_def &&
	soudi->parent.k != tag_parent::kind::k_enum_def) {
      return;
    }
    if (soudi->soud.get_decl_list_node().is_first())
      first_parent = &soudi->parent;
  }

  for (auto it_sour = soui.sours_begin();
       (it_sour != soui.sours_end() &&
	it_sour->sour.get_decl_list_node().is_visible_from(dn));
       ++it_sour) {
    if (it_sour->parent.k != tag_parent::kind::k_sou_def &&
	it_sour->parent.k != tag_parent::kind::k_enum_def) {
      return;
    }
    if (!first_parent)
      first_parent = &it_sour->parent;
  }

  assert(first_parent);
  first_parent->on_outermost_tag_def
    (wrap_callables<no_default_action>
     ([&](sou_def_info &outermost_soudi) {
	_queue_tag_defs_at_and_below(outermost_soudi);
      },
      [&](enum_def_info &outermost_edi) {
	_queue_tag_defs_at_and_below(outermost_edi);
      }));
}

void _lp_deps_resolver::_queue_dep_on_tag_decl(const dep_on_enum_decl &dep)
{
  // If no update, return early.
  if (!_deps_on_enum_decls.add(dep))
    return;

  // If all reachable tag declarations live within some
  // struct/union/enum definition, emit the whole first such
  // definition tree.
  const auto &dn = dep.get_decl_list_node();
  enum_info &ei = dep.get_enum_info();
  tag_parent *first_parent = nullptr;
  enum_def_info * const edi = ei.get_ed();

  if (edi && edi->ed.get_decl_list_node().is_visible_from(dn)) {
    if (edi->parent.k != tag_parent::kind::k_sou_def &&
	edi->parent.k != tag_parent::kind::k_enum_def) {
      return;
    }
    if (edi->ed.get_decl_list_node().is_first())
      first_parent = &edi->parent;
  }

  for (auto it_er = ei.ers_begin();
       (it_er != ei.ers_end() &&
	it_er->er.get_decl_list_node().is_visible_from(dn));
       ++it_er) {
    if (it_er->parent.k != tag_parent::kind::k_sou_def &&
	it_er->parent.k != tag_parent::kind::k_enum_def) {
      return;
    }
    if (!first_parent)
      first_parent = &it_er->parent;
  }

  assert(first_parent);
  first_parent->on_outermost_tag_def
    (wrap_callables<no_default_action>
     ([&](sou_def_info &outermost_soudi) {
	_queue_tag_defs_at_and_below(outermost_soudi);
      },
      [&](enum_def_info &outermost_edi) {
	_queue_tag_defs_at_and_below(outermost_edi);
      }));
}

void
_lp_deps_resolver::_require_allocatable_type(const types::addressable_type &at)
{
  types::handle_types<void>
    ((wrap_callables<default_action_nop>
      ([&](const types::struct_or_union_type &sout) {
	 const ast::sou_decl_list_node * const decl_node =
	   sout.get_decl_list_node();
	 assert(decl_node);
	 sou_info * const soui = _ai.get_sou_info_by_id(decl_node->get_id());
	 if (!soui)
	   return;

	 sou_def_info * const soudi = soui->get_soud();
	 if (!soudi)
	   return;

	 _queue_all_related_tag_defs(*soudi);

       },
       [&](const types::enum_type &et) {
	 const ast::enum_decl_list_node &decl_node =
	   et.get_decl_list_node();
	 enum_info * const ei = _ai.get_enum_info_by_id(decl_node.get_id());
	 if (!ei)
	   return;

	 enum_def_info * const edi = ei->get_ed();
	 if (!edi)
	   return;

	 _queue_all_related_tag_defs(*edi);

       })),
     at);
}

bool _lp_deps_resolver::_queue_includes_for(const pp_tokens_range &r)
{
  bool from_header = false;
  const pp_result &ppr = _ai.atu.get_pp_result();
  const raw_pp_tokens_range r_raw = ppr.pp_tokens_range_to_raw(r);

  const auto h_end = ppr.intersecting_sources_end(r_raw);
  for (auto it_h = ppr.intersecting_sources_begin(r_raw); it_h != h_end;
       ++it_h) {
    auto &his = _ai.header_infos;
    auto &hi = his[it_h->get_id()];
    if (!hi.eligible)
      continue;

    from_header = true;
    _queue_header_include(hi);
  }

  return from_header;
}

void _lp_deps_resolver::_queue_header_include(header_info &hi)
{

    const auto ed_range = _add_include(hi);
    if (ed_range.first == ed_range.second) {
      // This header has already marked for inclusion
      return;
    }

    auto &&queue_deps_from_header
      = std::bind(&_lp_deps_resolver::_queue_deps,
		  this, std::placeholders::_1, true);

    auto &&handle_descendant_tags =
      [&](tags_infos &child_tags) {
	child_tags.for_each_tag_recursive
	  (wrap_callables<no_default_action>
	   ([&](sou_def_info &soudi) {
	      soudi.emit_def = true;
	      queue_deps_from_header(soudi.deps);
	      return true;
	    },
	    [&](enum_def_info &edi) {
	      edi.emit_def = true;
	      queue_deps_from_header(edi.deps);
	      return true;
	    },
	    [&](sou_ref_info &souri) {
	      souri.emit_tag_decl = true;
	    },
	    [&](enum_ref_info &eri) {
	      eri.emit_tag_decl = true;
	    }));
      };

    // Resolve the dependencies from anything overlapping with this header.
    for (auto it_ed = ed_range.first; it_ed != ed_range.second; ++it_ed) {
      switch (it_ed->k) {
      case external_declaration_info::kind::declaration:
	{
	  declaration_info &di = it_ed->get_declaration();
	  queue_deps_from_header(di.declaration_specifiers_deps);
	  handle_descendant_tags(di.child_tags);

	  di.init_declarators.for_each
	    (wrap_callables<no_default_action>
	     ([&](object_init_declarator_info &oidi) {
		queue_deps_from_header(oidi.declarator_deps);
		handle_descendant_tags(oidi.child_tags);

		if (!di.storage_class_specifier ||
		    (di.storage_class_specifier->get_storage_class() ==
		     ast::storage_class::sc_static)) {
		  _require_allocatable_type
		    (*oidi.init_declarator.get_declarator()
		     .get_innermost_type());
		}

		if (!oidi.oi.in_closure) {
		  if (!oidi.oi.externalize ||
		      oidi.oi.externalized_sym_mod.sym_mod.is_rename()) {
		    // _orig_decls_wl::add() will find itself unable
		    // to piggy-back. In order to keep things sane,
		    // set ->emit_orig_decl such that it won't
		    // unnecessarily requeue this declaration to the
		    // worklist.
		    oidi.emit_orig_decl = true;

		    initializer_info * const ii = oidi.get_initializer();
		    if (ii) {
		      queue_deps_from_header(ii->deps.on_types);
		      _queue_deps_on_decls_from_header(ii->deps.on_funs);
		      _queue_deps_on_decls_from_header(ii->deps.on_objs);
		      handle_descendant_tags(ii->child_tags);
		    }
		  } else {
		    // There can't be declarations with initializers
		    // for objects externalized w/o renaming.
		    assert(!oidi.get_initializer());
		  }
		}
	      },
	      [&](function_init_declarator_info &fidi) {
		queue_deps_from_header(fidi.declarator_deps);
		handle_descendant_tags(fidi.child_tags);

		const function_info &fi = fidi.fi;
		const function_definition_info *fdi =
		  fi.get_function_definition();
		if ((!fi.externalize ||
		     fi.externalized_sym_mod.sym_mod.is_rename()) &&
		    ((!fi.in_closure && !(fdi && fdi->is_patched)) ||
		     (fdi && fdi->sym_mod.is_rename()))) {
		  // _orig_decls_wl::add() will find itself unable to
		  // piggy-back. In order to keep things sane, set
		  // ->emit_orig_decl such that it won't unnecessarily
		  // requeue this declaration to the worklist.
		  fidi.emit_orig_decl = true;
		}
	      },
	      [&](typedef_init_declarator_info &tdidi) {
		queue_deps_from_header(tdidi.declarator_deps);
		handle_descendant_tags(tdidi.child_tags);
		tdidi.emit = true;
	      }));
	}
	break;

      case external_declaration_info::kind::static_assertion:
	{
	  const static_assert_info &sai = it_ed->get_static_assert();
	  queue_deps_from_header(sai.deps);
	}
	break;

      case external_declaration_info::kind::function_definition:
	{
	  function_definition_info &fdi = it_ed->get_function_definition();

	  queue_deps_from_header(fdi.declarator_deps);
	  fdi.for_each_body_deps_on_types
	    ([&](const deps_on_types &deps) {
	       queue_deps_from_header(deps);
	     });
	  handle_descendant_tags(fdi.child_tags);

	  const function_info &fi = fdi.fi;
	  if ((!fi.externalize ||
	       fi.externalized_sym_mod.sym_mod.is_rename()) &&
	      ((!fi.in_closure && !fdi.is_patched) ||
	       fdi.sym_mod.is_rename())) {
	    // _orig_decls_wl::add() will find itself unable to
	    // piggy-back. In order to keep things sane, set
	    // ->emit_orig_decl such that it won't unnecessarily
	    // requeue this declaration to the worklist.
	    fdi.emit_orig_decl = true;
	  }

	  if (!fdi.fi.in_closure || fdi.rewrite_needed) {
	    _queue_deps_on_decls_from_header(fdi.body_deps.on_funs);
	    _queue_deps_on_decls_from_header(fdi.body_deps.on_objs);
	    for (const auto &di : fdi.get_declarations()) {
	      di.init_declarators.for_each
		(wrap_callables<default_action_nop>
		 ([&](const object_init_declarator_info &oidi) {
		    if (!di.storage_class_specifier ||
			(di.storage_class_specifier->get_storage_class() ==
			 ast::storage_class::sc_static)) {
		      _require_allocatable_type
			(*oidi.init_declarator.get_declarator()
			 .get_innermost_type());
		    }

		    const initializer_info * const ii =
		      oidi.get_initializer();
		    if (ii) {
		      _queue_deps_on_decls_from_header(ii->deps.on_funs);
		      _queue_deps_on_decls_from_header(ii->deps.on_objs);
		    }
		  }));
	    }
	  }
	}
	break;
      }
    }
}

std::pair<ast_info::external_declaration_iterator,
	  ast_info::external_declaration_iterator>
_lp_deps_resolver::_add_include(header_info &hi) const
{
  assert(hi.eligible);

  // Find the topmost eligible header and include that.
  header_info *topmost = &hi;
  while (topmost->parent && topmost->parent->eligible)
    topmost = topmost->parent;

  if (topmost->include) {
    return std::make_pair(_ai.external_declarations_end(),
			  _ai.external_declarations_end());
  }

  topmost->include = true;

  const raw_pp_tokens_range &r = topmost->get_range();
  auto ed_begin = _ai.intersecting_external_declarations_begin(r);
  auto ed_end = _ai.intersecting_external_declarations_end(r);

  return std::make_pair(ed_begin, ed_end);
}

void
_lp_deps_resolver::_queue_deps_on_decls_from_header(const deps_on_funs &deps)
{
  // Transform dependencies on functions from included headers into
  // dependencies on declarations.
  for (const auto &d : deps) {
    if (d.fdi) {
      _orig_decls_wl.add(dep_on_decl_type{*d.fdi, false, d.from_eid}, false);
    } else {
      assert(d.fidi);
      _orig_decls_wl.add(dep_on_decl_type{*d.fidi, false, d.from_eid}, false);
    }
  }
}

void
_lp_deps_resolver::_queue_deps_on_decls_from_header(const deps_on_objs &deps)
{
  // Transform dependencies on objects from included headers into
  // dependencies on declarations.
  for (const auto &d : deps) {
    if (!d.from_eid)
      continue;

    _orig_decls_wl.add(dep_on_decl_type{d.oidi.get(), false, *d.from_eid},
		       false);
  }
}

_lp_deps_resolver::_orig_decls_wl::entry::
entry(function_definition_info &_fdi) noexcept
  : k(dep_on_decl_type::kind::k_function_definition), fdi(&_fdi)
{}

_lp_deps_resolver::_orig_decls_wl::entry::
entry(function_init_declarator_info &_fidi) noexcept
  : k(dep_on_decl_type::kind::k_function_init_declarator), fidi(&_fidi)
{}

_lp_deps_resolver::_orig_decls_wl::entry::
entry(object_init_declarator_info &_oidi) noexcept
  : k(dep_on_decl_type::kind::k_object_init_declarator), oidi(&_oidi)
{}

void _lp_deps_resolver::_orig_decls_wl::add(const dep_on_decl_type &d,
					    const bool can_rewrite_reference)
{
  // See if we can piggy-back on some declaration which is needed from
  // the live patch closure and will get emitted anyway.
  switch (d.get_kind()) {
  case dep_on_decl_type::kind::k_function_definition:
    {
      const function_definition_info &fdi = d.get_fdi();
      const function_info &fi = fdi.fi;
      if (fi.externalize &&
	  (can_rewrite_reference ||
	   !(fi.externalized_sym_mod.sym_mod.is_rename() ||
	     fi.externalized_sym_mod.make_pointer))) {
	return;
      } else if ((fdi.is_patched || fi.in_closure) &&
		 (can_rewrite_reference ||
		  !fdi.sym_mod.is_rename())) {
	return;
      }
    }
    break;

  case dep_on_decl_type::kind::k_function_init_declarator:
    {
      const function_info &fi = d.get_fidi().fi;
      const function_definition_info * const fdi =
	fi.get_function_definition();
      if (fi.externalize &&
	  (can_rewrite_reference ||
	   !(fi.externalized_sym_mod.sym_mod.is_rename() ||
	     fi.externalized_sym_mod.make_pointer))) {
	return;
      } else if (((fdi && fdi->is_patched) || fi.in_closure) &&
		 (can_rewrite_reference || !fdi ||
		  !fdi->sym_mod.is_rename())) {
	return;
      }
    }
    break;

  case dep_on_decl_type::kind::k_object_init_declarator:
    {
      const object_info &oi = d.get_oidi().oi;
      if ((oi.externalize &&
	   (can_rewrite_reference ||
	    !(oi.externalized_sym_mod.sym_mod.is_rename() ||
	      oi.externalized_sym_mod.make_pointer))) ||
	  oi.in_closure) {
	if (d.need_composite) {
	  d.for_each_visible
	    (wrap_callables<default_action_unreachable<bool, type_set<>>::type>
	     ([&](object_init_declarator_info &visible_oidi) {
		visible_oidi.need_decl_type = true;
		const ast::linkage &l =
		  visible_oidi.init_declarator.get_linkage();
		return l.is_type_modified_through_linkage();
	      }));
	}
	return;
      }
    }
    break;
  }

  if (!d.need_composite) {
    // Composite type not needed, use the first visible declaration (in
    // file order).
    object_init_declarator_info *first_oidi = nullptr;
    function_init_declarator_info *first_fidi = nullptr;
    function_definition_info *first_fdi = nullptr;
    d.for_each_visible
      (wrap_callables<no_default_action>
       ([&](object_init_declarator_info &oidi) {
	  first_oidi = &oidi;
	  first_fidi = nullptr;
	  first_fdi = nullptr;
	  return true;
	},
	[&](function_init_declarator_info &fidi) {
	  first_oidi = nullptr;
	  first_fidi = &fidi;
	  first_fdi = nullptr;
	  return true;
	},
	[&](function_definition_info &fdi) {
	  first_oidi = nullptr;
	  first_fidi = nullptr;
	  first_fdi = &fdi;
	  return true;
	}));

    if (first_oidi) {
      // Don't add (extern) declarations in function scope.
      if (!first_oidi->enclosing_declaration.enclosing_fd &&
	  !first_oidi->emit_orig_decl) {
	first_oidi->emit_orig_decl = true;
	wl.push(entry{*first_oidi});
      }
    } else if (first_fidi) {
      // Don't add (extern) declarations in function scope.
      if (!first_fidi->enclosing_declaration.enclosing_fd &&
	  !first_fidi->emit_orig_decl) {
	first_fidi->emit_orig_decl = true;
	wl.push(entry{*first_fidi});
      }
    } else {
      assert(first_fdi);
      if (!first_fdi->emit_orig_decl) {
	first_fdi->emit_orig_decl = true;
	wl.push(entry{*first_fdi});
      }
    }

  } else {
    // The composite type is needed. Add all visible declarations
    // which took part in type composition.
    d.for_each_visible
      (wrap_callables<no_default_action>
       ([&](function_definition_info &fdi) {
	  if (!fdi.emit_orig_decl) {
	    fdi.emit_orig_decl = true;
	    wl.push(entry{fdi});
	  }
	  const ast::linkage &l = fdi.function_definition.get_linkage();
	  return l.is_type_modified_through_linkage();
	},
	[&](function_init_declarator_info &fidi) {
	  const ast::linkage &l = fidi.init_declarator.get_linkage();
	  // Don't add (extern) declarations in function scope.
	  if (!fidi.enclosing_declaration.enclosing_fd &&
	      !fidi.emit_orig_decl) {
	    fidi.emit_orig_decl = true;
	    wl.push(entry{fidi});
	  }
	  return l.is_type_modified_through_linkage();
	},
	[&](object_init_declarator_info &oidi) {
	  const ast::linkage &l = oidi.init_declarator.get_linkage();
	    // Don't add (extern) declarations in function scope.
	  if (!oidi.enclosing_declaration.enclosing_fd) {
	    oidi.need_orig_decl_type = true;
	    if (!oidi.emit_orig_decl) {
	      oidi.emit_orig_decl = true;
	      wl.push(entry{oidi});
	    }
	  }
	  return l.is_type_modified_through_linkage();
	}));
  }
}


namespace
{
  class _lp_writer
  {
  public:
    _lp_writer(ast_info &ai, depreprocessor &d) noexcept;

    void operator()();

  private:
    void _emit_inclusion_sequence
       (const ast_info::external_declaration_iterator &sequence_eds_begin,
	std::vector<std::pair<const ast_info::header_infos_type::const_iterator,
			      ast_info::external_declaration_iterator> > &&seq);

    bool _emit(function_init_declarator_info &fidi,
	       const bool only_if_renamed);
    bool _emit(object_init_declarator_info &oidi,
	       const bool only_if_renamed);
    bool _emit(typedef_init_declarator_info &tdidi);
    bool _emit(function_definition_info &fdi,
	       const bool only_if_renamed);

    static void _mark_emitted_tags(tags_infos &tags);

    void _handle_emitted_tags(tags_infos &tags,
			      depreprocessor::transformed_input_chunk &tic);

    void _emit_tags(tags_infos &tags, const ast::declaration * const decl);

    static bool _any_tag_needed(const tags_infos &tags) noexcept;

    depreprocessor::transformed_input_chunk
    _prepare_tic_for_decl(function_init_declarator_info &fidi);

    depreprocessor::transformed_input_chunk
    _prepare_tic_for_decl(object_init_declarator_info &oidi,
			  const bool strip_initializer,
			  const bool need_decl_type);

    depreprocessor::transformed_input_chunk
    _prepare_tic_for_decl(typedef_init_declarator_info &tdidi);

    depreprocessor::transformed_input_chunk
    _create_tic_for_decl(const ast::declaration &d,
			 const ast::init_declarator &id);

    depreprocessor::transformed_input_chunk
    _prepare_tic_for_decl(function_definition_info &fdi);

    void _apply_sym_mod_to_id_tok
	(const lp_creation_policy::symbol_modification &sym_mod,
	 const bool make_pointer, const pp_token_index id_tok_pos,
	 depreprocessor::transformed_input_chunk &tic);

    void _apply_linkage_change_for_fun
	(const lp_creation_policy::symbol_modification::linkage_change lc,
	 const bool make_pointer,
	 const ast::linkage &l,
	 const ast::declaration_specifiers &ds,
	 const ast::storage_class_specifier * const scs,
	 depreprocessor::transformed_input_chunk &tic);

    void _set_storage_class(const ast::declaration_specifiers &ds,
			    const ast::storage_class_specifier *old_scs,
			    const ast::storage_class new_sc,
			    depreprocessor::transformed_input_chunk &tic);

    void _rewrite_references(const deps_on &deps,
			     depreprocessor::transformed_input_chunk &tic);

    void _rewrite_references(const deps_on_types &deps,
			     depreprocessor::transformed_input_chunk &tic);

    void _rewrite_references(const deps_on_decl_types &deps,
			     depreprocessor::transformed_input_chunk &tic);

    void _rewrite_references(const deps_on_objs &deps,
			     depreprocessor::transformed_input_chunk &tic);

    void _rewrite_references(const deps_on_funs &deps,
			     depreprocessor::transformed_input_chunk &tic);

    ast_info &_ai;
    depreprocessor &_d;
  };
}

_lp_writer::_lp_writer(ast_info &ai, depreprocessor &d) noexcept
  : _ai(ai), _d(d)
{}

void _lp_writer::operator()()
{
  auto skip_to_header_to_include =
    [&](ast_info::header_infos_type::const_iterator it_hi) {
      while(it_hi != _ai.header_infos.cend() && !it_hi->include)
	++it_hi;
      return it_hi;
    };
  auto it_hi_to_begin_ed =
    [&](const ast_info::header_infos_type::const_iterator &it_hi) {
      if (it_hi == _ai.header_infos.cend())
	return _ai.external_declarations_end();

      return _ai.intersecting_external_declarations_begin(it_hi->get_range());
    };
  ast_info::header_infos_type::const_iterator it_next_include_hi
    = skip_to_header_to_include(_ai.header_infos.cbegin());
  ast_info::external_declaration_iterator next_include_eds_begin
    = it_hi_to_begin_ed(it_next_include_hi);

  auto it_ed = _ai.external_declarations_begin();
  while (it_ed != _ai.external_declarations_end()) {
    if (it_ed != next_include_eds_begin) {
      switch (it_ed->k) {
      case external_declaration_info::kind::declaration:
	{
	  declaration_info &di = it_ed->get_declaration();
	  bool anything_emitted = false;
	  di.init_declarators.for_each
	    (wrap_callables<no_default_action>
	     ([&](function_init_declarator_info &fidi) {
		const bool anything_emitted_id = _emit(fidi, false);
		if (anything_emitted_id) {
		  anything_emitted = true;
		} else {
		  if (_any_tag_needed(fidi.child_tags)) {
		    if (!anything_emitted && _any_tag_needed(di.child_tags))
		      _emit_tags(di.child_tags, &di.declaration);
		    _emit_tags(fidi.child_tags, &di.declaration);
		  }
		}
	      },
	      [&](object_init_declarator_info &oidi) {
		const bool anything_emitted_id = _emit(oidi, false);
		if (anything_emitted_id) {
		  anything_emitted = true;
		} else {
		  if (_any_tag_needed(oidi.child_tags)) {
		    if (!anything_emitted && _any_tag_needed(di.child_tags))
		      _emit_tags(di.child_tags, &di.declaration);
		    _emit_tags(oidi.child_tags, &di.declaration);
		  }
		}

		initializer_info * const ii = oidi.get_initializer();
		if (ii && _any_tag_needed(ii->child_tags)) {
		  if (!anything_emitted && _any_tag_needed(di.child_tags))
		    _emit_tags(di.child_tags, &di.declaration);
		  _emit_tags(ii->child_tags, &di.declaration);
		}
	      },
	      [&](typedef_init_declarator_info &tdidi) {
		const bool anything_emitted_id = _emit(tdidi);
		if (anything_emitted_id) {
		  anything_emitted = true;
		} else {
		  if (_any_tag_needed(tdidi.child_tags)) {
		    if (!anything_emitted && _any_tag_needed(di.child_tags))
		      _emit_tags(di.child_tags, &di.declaration);
		    _emit_tags(tdidi.child_tags, &di.declaration);
		  }
		}
	      }));

	  if (!anything_emitted && _any_tag_needed(di.child_tags))
	    _emit_tags(di.child_tags, &di.declaration);
	}
	break;

      case external_declaration_info::kind::static_assertion:
	{
	  static_assert_info &sai = it_ed->get_static_assert();
	  if (_any_tag_needed(sai.child_tags))
	    _emit_tags(sai.child_tags, nullptr);
	}
	break;

      case external_declaration_info::kind::function_definition:
	{
	  function_definition_info &fdi = it_ed->get_function_definition();
	  const bool anything_emitted = _emit(fdi, false);
	  if (!anything_emitted && _any_tag_needed(fdi.child_tags))
	    _emit_tags(fdi.child_tags, nullptr);
	}
	break;
      }

      ++it_ed;

    } else {
      // The current position is some header to #include.
      // Note that it might actually be a sequence of #includes
      // overlapping on external_declaration language productions.
      std::vector<std::pair<const ast_info::header_infos_type::const_iterator,
			    ast_info::external_declaration_iterator> >
	inclusion_sequence;
      const ast_info::external_declaration_iterator inclusion_sequence_eds_begin
	= next_include_eds_begin;
      do {
	it_ed = (_ai.intersecting_external_declarations_end
		 (it_next_include_hi->get_range()));

	inclusion_sequence.push_back(std::make_pair(it_next_include_hi, it_ed));

	it_next_include_hi = skip_to_header_to_include(it_next_include_hi + 1);
	next_include_eds_begin = it_hi_to_begin_ed(it_next_include_hi);
      } while (next_include_eds_begin < it_ed);

      _emit_inclusion_sequence(inclusion_sequence_eds_begin,
			       std::move(inclusion_sequence));
    }
  }
}

void _lp_writer::_emit_inclusion_sequence
       (const ast_info::external_declaration_iterator &sequence_eds_begin,
	std::vector<std::pair<const ast_info::header_infos_type::const_iterator,
			      ast_info::external_declaration_iterator> > &&seq)
{
  using linkage_change =
    lp_creation_policy::symbol_modification::linkage_change;
  const ast_info::external_declaration_iterator sequence_eds_end =
    seq.back().second;

  for (auto it_ed = sequence_eds_begin; it_ed != sequence_eds_end; ++it_ed) {
    switch (it_ed->k) {
    case external_declaration_info::kind::declaration:
      {
	declaration_info &di = it_ed->get_declaration();
	_mark_emitted_tags(di.child_tags);

	di.init_declarators.for_each
	  (wrap_callables<no_default_action>
	   ([&](function_init_declarator_info &fidi) {
	      _mark_emitted_tags(fidi.child_tags);
	      fidi.emit_orig_decl = false;

	      const ast::linkage &l = fidi.init_declarator.get_linkage();
	      if (!l.is_first_in_chain())
		return;

	      const function_info &fi = fidi.fi;
	      const function_definition_info * const fdi =
		fidi.fi.get_function_definition();
	      linkage_change lc = linkage_change::lc_none;
	      if (fi.externalize &&
		  !fi.externalized_sym_mod.sym_mod.is_rename()) {
		assert(!fi.externalized_sym_mod.make_pointer);
		lc = fi.externalized_sym_mod.sym_mod.new_linkage;
	      } else if (fdi && (fdi->is_patched || fi.in_closure) &&
			 !fdi->sym_mod.is_rename()) {
		lc = fdi->sym_mod.new_linkage;
	      }

	      assert(lc == linkage_change::lc_none ||
		     lc != linkage_change::lc_make_extern ||
		     (l.get_linkage_kind() ==
		      ast::linkage::linkage_kind::external));
	      if (lc == linkage_change::lc_make_static &&
		  (l.get_linkage_kind() !=
		   ast::linkage::linkage_kind::internal)) {
		depreprocessor::transformed_input_chunk tic
		  = _prepare_tic_for_decl(fidi);
		const ast::storage_class_specifier *scs =
		  di.storage_class_specifier;
		_set_storage_class(di.declaration.get_declaration_specifiers(),
				   di.storage_class_specifier,
				   ast::storage_class::sc_static,
				   tic);
		_d.append(std::move(tic));
	      }
	    },
	    [&](object_init_declarator_info &oidi) {
	      _mark_emitted_tags(oidi.child_tags);
	      oidi.emit_orig_decl = false;

	      const ast::linkage &l = oidi.init_declarator.get_linkage();
	      if (!l.is_first_in_chain())
		return;

	      const object_info &oi = oidi.oi;
	      if (oi.externalize &&
		  !oi.externalized_sym_mod.sym_mod.is_rename()) {
		assert(!oi.externalized_sym_mod.make_pointer);

		if (oi.externalized_sym_mod.sym_mod.new_linkage ==
		    linkage_change::lc_make_static &&
		    (l.get_linkage_kind() !=
		     ast::linkage::linkage_kind::internal)) {
		  depreprocessor::transformed_input_chunk tic
		    = _prepare_tic_for_decl(oidi, true, false);

		  const declaration_info &di = oidi.enclosing_declaration;
		  _set_storage_class
		    (di.declaration.get_declaration_specifiers(),
		     di.storage_class_specifier, ast::storage_class::sc_static,
		     tic);
		  _d.append(std::move(tic));
		} else {
		  assert(di.storage_class_specifier &&
			 (di.storage_class_specifier->get_storage_class() ==
			  ast::storage_class::sc_extern));
		  assert(!oidi.get_initializer());
		}
	      } else {
		assert(oi.in_closure ||
		       (l.get_linkage_kind() !=
			ast::linkage::linkage_kind::external) ||
		       (di.storage_class_specifier &&
			!oidi.get_initializer()));
	      }
	    },
	    [&](typedef_init_declarator_info &tdidi) {
	      _mark_emitted_tags(tdidi.child_tags);
	    }));
      }
      break;

    case external_declaration_info::kind::static_assertion:
      break;

    case external_declaration_info::kind::function_definition:
      {
	function_definition_info &fdi = it_ed->get_function_definition();
	_mark_emitted_tags(fdi.child_tags);
	fdi.emit_orig_decl = false;

	assert(!fdi.fi.externalize ||
	       fdi.fi.externalized_sym_mod.sym_mod.is_rename());
	const ast::function_definition &fd = fdi.function_definition;
	const ast::linkage &l = fd.get_linkage();
	if (fdi.fi.in_closure && !fdi.rewrite_needed) {
	  assert(!fdi.sym_mod.is_rename());
	  assert(fdi.sym_mod.new_linkage == linkage_change::lc_make_static ||
		 (fd.is_gnu_inline() && fd.is_always_inline()));
	  if (fdi.sym_mod.new_linkage == linkage_change::lc_make_static &&
	      l.get_linkage_kind() != ast::linkage::linkage_kind::internal &&
	      l.is_first_in_chain()) {
	    depreprocessor::transformed_input_chunk tic =
	      _prepare_tic_for_decl(fdi);
	    const ast::declaration_specifiers &ds =
	      fd.get_declaration_specifiers();
	    _set_storage_class(ds, _find_scs(ds), ast::storage_class::sc_static,
			       tic);
	    _d.append(std::move(tic));
	  }
	} else {
	  assert(l.get_linkage_kind() == ast::linkage::linkage_kind::internal ||
		 fd.is_gnu_inline());
	}
      }
      break;
    }
  }

  // Now emit the #includes. Take care to emit any pp_tokens_range
  // contained within sequence_eds_begin and sequence_eds_end but not
  // covered by some header.
  const pp_result &ppr = _ai.atu.get_pp_result();
  const pp_tokens &toks = ppr.get_pp_tokens();
  if (sequence_eds_begin != _ai.external_declarations_end()) {
    const header_info &first_hi = *seq.front().first;
    const raw_pp_token_index gap_before_begin_raw
      = (ppr.pp_tokens_range_to_raw(sequence_eds_begin->get_tokens_range())
	 .begin);
    const raw_pp_token_index gap_before_end_raw = first_hi.get_range().begin;

    if (gap_before_begin_raw < gap_before_end_raw) {
      const pp_tokens_range gap_range
	= ppr.raw_pp_tokens_range_to_nonraw(raw_pp_tokens_range{
						gap_before_begin_raw,
						gap_before_end_raw
					    });
      if (gap_range.begin != gap_range.end) {

	depreprocessor::transformed_input_chunk tic{gap_range};
	tic.copy_subrange(gap_range, false, toks);
	_d.append(std::move(tic));
      }
    }
  }
  for (auto it = seq.begin(); it != seq.end(); ++it) {
    // Write the #include directive itself
    const header_info &hi = *it->first;
    if (!hi.is_root())
      _d.append(hi.get_child_node());
    else
      _d.append(hi.get_root_node());

    // Write the external_declaration overlapping with the header's
    // end, if any.
    const raw_pp_token_index gap_after_begin_raw = hi.get_range().end;
    raw_pp_token_index gap_after_end_raw;
    if (it + 1 == seq.end()) {
      const auto gap_ed_end = it->second;
      if (gap_ed_end != _ai.external_declarations_begin()) {
	gap_after_end_raw =
	  ppr.pp_tokens_range_to_raw((gap_ed_end - 1)->get_tokens_range()).end;
      } else {
	gap_after_end_raw = gap_after_begin_raw;
      }

    } else {
      gap_after_end_raw = (it + 1)->first->get_range().begin;
    }

    if (gap_after_end_raw <= gap_after_begin_raw)
      continue;

    const pp_tokens_range gap_range
      = ppr.raw_pp_tokens_range_to_nonraw(raw_pp_tokens_range{
						gap_after_begin_raw,
						gap_after_end_raw
					  });
    if (gap_range.begin == gap_range.end)
      continue;

    depreprocessor::transformed_input_chunk tic{gap_range};
    tic.copy_subrange(gap_range, false, toks);
    _d.append(std::move(tic));
  }

  // Finally, emit the to be rewritten declarations and function
  // definitions from this header.
  for (auto it_ed = sequence_eds_begin; it_ed != sequence_eds_end; ++it_ed) {
    switch (it_ed->k) {
    case external_declaration_info::kind::declaration:
      {
	declaration_info &di = it_ed->get_declaration();
	bool anything_emitted = false;
	di.init_declarators.for_each
	  (wrap_callables<no_default_action>
	   ([&](function_init_declarator_info &fidi) {
	      _emit(fidi, true);
	    },
	    [&](object_init_declarator_info &oidi) {
	      _emit(oidi, true);
	    },
	    [&](typedef_init_declarator_info &tdidi) {
	    }));
      }
      break;

    case external_declaration_info::kind::static_assertion:
      break;

    case external_declaration_info::kind::function_definition:
      {
	function_definition_info &fdi = it_ed->get_function_definition();
	_emit(fdi, true);
      }
      break;
    }
  }
}

bool _lp_writer::_emit(function_init_declarator_info &fidi,
		       const bool only_if_renamed)
{
  bool anything_emitted = false;

  const function_info &fi = fidi.fi;
  const function_definition_info * const fdi = fi.get_function_definition();
  declaration_info &di = fidi.enclosing_declaration;

  if (fidi.emit_orig_decl && !only_if_renamed) {
    assert(!(fi.externalize || fi.externalized_sym_mod.sym_mod.is_rename()));
    assert(!(fi.in_closure || (fdi && fdi->is_patched)) ||
	   (fdi && fdi->sym_mod.is_rename()));
    depreprocessor::transformed_input_chunk tic = _prepare_tic_for_decl(fidi);
    _rewrite_references(di.declaration_specifiers_deps, tic);
    _rewrite_references(fidi.declarator_deps, tic);
    _d.append(std::move(tic));
    fidi.emit_orig_decl = false;
    anything_emitted = true;
  }

  const ast::init_declarator &id = fidi.init_declarator;
  const ast::linkage &l = id.get_linkage();
  const ast::declaration_specifiers &ds =
    di.declaration.get_declaration_specifiers();
  const ast::storage_class_specifier * const scs = di.storage_class_specifier;
  if (fi.externalize &&
      (!only_if_renamed || fi.externalized_sym_mod.sym_mod.is_rename())) {
    depreprocessor::transformed_input_chunk tic = _prepare_tic_for_decl(fidi);
    _rewrite_references(di.declaration_specifiers_deps, tic);
    _rewrite_references(fidi.declarator_deps, tic);

    _apply_sym_mod_to_id_tok(fi.externalized_sym_mod.sym_mod,
			     fi.externalized_sym_mod.make_pointer,
			     (id.get_declarator().get_direct_declarator_id()
			      .get_id_tok()),
			     tic);
    _apply_linkage_change_for_fun(fi.externalized_sym_mod.sym_mod.new_linkage,
				  fi.externalized_sym_mod.make_pointer,
				  l, ds, scs, tic);

    _d.append(std::move(tic));
    anything_emitted = true;
  }

  if ((fi.in_closure || (fdi && fdi->is_patched)) &&
      (!only_if_renamed || (fdi && fdi->sym_mod.is_rename()))) {
    depreprocessor::transformed_input_chunk tic = _prepare_tic_for_decl(fidi);
    _rewrite_references(di.declaration_specifiers_deps, tic);
    _rewrite_references(fidi.declarator_deps, tic);

    if (fdi) {
      if (fdi->is_patched || fdi->rewrite_needed) {
	_apply_sym_mod_to_id_tok(fdi->sym_mod, false,
				 (id.get_declarator().get_direct_declarator_id()
				  .get_id_tok()),
				 tic);
      }
      _apply_linkage_change_for_fun(fdi->sym_mod.new_linkage, false,
				    l, ds, scs, tic);
    }

    _d.append(std::move(tic));
    anything_emitted = true;
  }

  return anything_emitted;
}

bool _lp_writer::_emit(object_init_declarator_info &oidi,
		       const bool only_if_renamed)
{
  bool anything_emitted = false;

  const object_info &oi = oidi.oi;
  declaration_info &di = oidi.enclosing_declaration;
  const ast::init_declarator &id = oidi.init_declarator;
  const ast::linkage &l = id.get_linkage();
  const ast::declaration_specifiers &ds =
    di.declaration.get_declaration_specifiers();
  const ast::storage_class_specifier * const scs = di.storage_class_specifier;

  assert(!(oidi.emit_orig_decl && oi.in_closure));
  assert(!(oi.externalize && oi.in_closure));
  if (oidi.emit_orig_decl && !only_if_renamed) {
    depreprocessor::transformed_input_chunk tic
      = _prepare_tic_for_decl(oidi, true, oidi.need_orig_decl_type);
    _rewrite_references(di.declaration_specifiers_deps, tic);
    _rewrite_references(oidi.declarator_deps, tic);

    if (l.get_linkage_kind() == ast::linkage::linkage_kind::external &&
	!scs) {
      // Don't emit externally visible tentative defintitions.
      _set_storage_class(ds, scs, ast::storage_class::sc_extern,
			 tic);
    }

    _d.append(std::move(tic));
    oidi.emit_orig_decl = false;
    anything_emitted = true;

  } else if (oi.in_closure && !only_if_renamed) {
    depreprocessor::transformed_input_chunk tic
      = _prepare_tic_for_decl(oidi, false, oidi.need_decl_type);
    _rewrite_references(di.declaration_specifiers_deps, tic);
    _rewrite_references(oidi.declarator_deps, tic);
    if (oidi.get_initializer())
      _rewrite_references(oidi.get_initializer()->deps, tic);
    _d.append(std::move(tic));
    anything_emitted = true;

  }

  if (oi.externalize &&
      (!only_if_renamed || oi.externalized_sym_mod.sym_mod.is_rename())) {
    depreprocessor::transformed_input_chunk tic
      = _prepare_tic_for_decl(oidi, true, oidi.need_decl_type);
    _rewrite_references(di.declaration_specifiers_deps, tic);
    _rewrite_references(oidi.declarator_deps, tic);

    _apply_sym_mod_to_id_tok(oi.externalized_sym_mod.sym_mod,
			     oi.externalized_sym_mod.make_pointer,
			     (id.get_declarator().get_direct_declarator_id()
			      .get_id_tok()),
			     tic);

    using linkage_change =
      lp_creation_policy::symbol_modification::linkage_change;

    switch (oi.externalized_sym_mod.sym_mod.new_linkage) {
    case linkage_change::lc_none:
      break;

    case linkage_change::lc_make_static:
      if (l.is_first_in_chain()) {
	_set_storage_class(ds, scs, ast::storage_class::sc_static, tic);
      }
      break;

    case linkage_change::lc_make_extern:
      // Make sure not to emit tentative definitions.
      _set_storage_class(ds, scs, ast::storage_class::sc_extern, tic);
      break;
    }

    _d.append(std::move(tic));
    anything_emitted = true;

  }

  return anything_emitted;
}

bool _lp_writer::_emit(typedef_init_declarator_info &tdidi)
{
  if (!tdidi.emit)
    return false;

  _d.append(_prepare_tic_for_decl(tdidi));
  return true;
}

bool _lp_writer::_emit(function_definition_info &fdi,
		       const bool only_if_renamed)
{
  using linkage_change =
    lp_creation_policy::symbol_modification::linkage_change;

  bool anything_emitted = false;

  const function_info &fi = fdi.fi;
  if (fdi.emit_orig_decl && !only_if_renamed) {
    assert(!(fi.externalize || fi.externalized_sym_mod.sym_mod.is_rename()));
    assert(!(fi.in_closure || fdi.is_patched) || fdi.sym_mod.is_rename());
    depreprocessor::transformed_input_chunk tic = _prepare_tic_for_decl(fdi);
    _rewrite_references(fdi.declarator_deps, tic);
    _d.append(std::move(tic));
    fdi.emit_orig_decl = false;
    anything_emitted = true;
  }

  const ast::function_definition &fd = fdi.function_definition;
  const ast::linkage &l = fd.get_linkage();
  const ast::declaration_specifiers &ds = fd.get_declaration_specifiers();
  const ast::storage_class_specifier * const scs = _find_scs(ds);
  if (fi.externalize &&
      (!only_if_renamed || fi.externalized_sym_mod.sym_mod.is_rename())) {
    depreprocessor::transformed_input_chunk tic = _prepare_tic_for_decl(fdi);
    _rewrite_references(fdi.declarator_deps, tic);

    _apply_sym_mod_to_id_tok(fi.externalized_sym_mod.sym_mod,
			     fi.externalized_sym_mod.make_pointer,
			     (fd.get_declarator().get_direct_declarator_id()
			      .get_id_tok()),
			     tic);
    _apply_linkage_change_for_fun(fi.externalized_sym_mod.sym_mod.new_linkage,
				  fi.externalized_sym_mod.make_pointer,
				  l, ds, scs, tic);

    _d.append(std::move(tic));
    anything_emitted = true;
  }

  if ((fi.in_closure || fdi.is_patched) &&
      (!only_if_renamed || fdi.sym_mod.is_rename())) {
    const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
    depreprocessor::transformed_input_chunk tic{fd.get_tokens_range()};
    tic.copy_subrange(fd.get_tokens_range(), false, toks);

    _rewrite_references(fdi.declarator_deps, tic);
    _rewrite_references(fdi.body_deps, tic);

    _apply_sym_mod_to_id_tok(fdi.sym_mod, false,
			     (fd.get_declarator().get_direct_declarator_id()
			      .get_id_tok()),
			     tic);
    _apply_linkage_change_for_fun(fdi.sym_mod.new_linkage, false,
				  l, ds, scs, tic);

    for (const auto &di_in_fd : fdi.get_declarations()) {
      bool need_split = false;
      di_in_fd.init_declarators.for_each
	(wrap_callables<no_default_action>
	 ([](const function_init_declarator_info &fidi_in_fd) {
	    // Function declarations within function definitions
	    // always will have an "extern" storage class specifier
	    // and it will be kept this way, independent of if and
	    // how externalization is to be done.
	  },
	  [&](const object_init_declarator_info &oidi_in_fd) {
	    const object_info &oi = oidi_in_fd.oi;
	    if (oi.externalize) {
	      const ast::linkage &l =
		oidi_in_fd.init_declarator.get_linkage();
	      if (oidi_in_fd.get_initializer() ||
		  (l.get_linkage_kind() == ast::linkage::linkage_kind::none &&
		   (oi.externalized_sym_mod.sym_mod.new_linkage !=
		    linkage_change::lc_none))) {
		// Either the initializer must get dropped or the
		// declaration storage classifier will be changed from
		// "static" to "extern" (independent of whether the
		// new linkage will be internal or external).
		need_split = true;
	      }
	    }
	  },
	  [](const typedef_init_declarator_info &tdidi) {
	    assert(0);
	    __builtin_unreachable();
	  }));

      if (!need_split) {
	_rewrite_references(di_in_fd.declaration_specifiers_deps, tic);

	di_in_fd.init_declarators.for_each
	  (wrap_callables<no_default_action>
	   ([&](const function_init_declarator_info &fidi_in_fd) {
	      _rewrite_references(fidi_in_fd.declarator_deps, tic);

	      const function_info &fidi_in_fd_fi = fidi_in_fd.fi;
	      const function_definition_info * const referenced_fdi
		= fidi_in_fd_fi.get_function_definition();
	      const pp_token_index id_tok_pos =
		(fidi_in_fd.init_declarator.get_declarator()
		 .get_direct_declarator_id().get_id_tok());
	      if (fidi_in_fd_fi.externalize) {
		_apply_sym_mod_to_id_tok
		  (fidi_in_fd_fi.externalized_sym_mod.sym_mod,
		   fidi_in_fd_fi.externalized_sym_mod.make_pointer,
		   id_tok_pos, tic);
	      } else if (referenced_fdi &&
			 (referenced_fdi->is_patched ||
			  referenced_fdi->rewrite_needed)) {
		_apply_sym_mod_to_id_tok(referenced_fdi->sym_mod,
					 false,
					 id_tok_pos, tic);
	      }
	    },
	    [&](const object_init_declarator_info &oidi_in_fd) {
	      _rewrite_references(oidi_in_fd.declarator_deps, tic);

	      const object_info &oidi_in_fd_oi = oidi_in_fd.oi;
	      if (oidi_in_fd_oi.externalize) {
		const pp_token_index id_tok_pos =
		  (oidi_in_fd.init_declarator.get_declarator()
		   .get_direct_declarator_id().get_id_tok());
		_apply_sym_mod_to_id_tok
		  (oidi_in_fd_oi.externalized_sym_mod.sym_mod,
		   oidi_in_fd_oi.externalized_sym_mod.make_pointer,
		   id_tok_pos, tic);
	      } else if (oidi_in_fd.get_initializer()) {
		_rewrite_references(oidi_in_fd.get_initializer()->deps, tic);
	      }
	    },
	    [&](const typedef_init_declarator_info &tdidi) {
	      assert(0);
	      __builtin_unreachable();
	    }));

      } else {
	_d.append(tic.split_head_off(di_in_fd.declaration.get_tokens_range()));

	di_in_fd.init_declarators.for_each
	  (wrap_callables<no_default_action>
	   ([&](function_init_declarator_info &fidi_in_fd) {
	      depreprocessor::transformed_input_chunk decl_tic
		= _prepare_tic_for_decl(fidi_in_fd);

	      _rewrite_references(di_in_fd.declaration_specifiers_deps,
				  decl_tic);
	      _rewrite_references(fidi_in_fd.declarator_deps, decl_tic);

	      const function_info &fidi_in_fd_fi = fidi_in_fd.fi;
	      const function_definition_info * const referenced_fdi
		= fidi_in_fd_fi.get_function_definition();
	      const pp_token_index id_tok_pos =
		(fidi_in_fd.init_declarator.get_declarator()
		 .get_direct_declarator_id().get_id_tok());
	      if (fidi_in_fd_fi.externalize) {
		_apply_sym_mod_to_id_tok
		  (fidi_in_fd_fi.externalized_sym_mod.sym_mod,
		   fidi_in_fd_fi.externalized_sym_mod.make_pointer,
		   id_tok_pos, decl_tic);
	      } else if (referenced_fdi &&
			 (referenced_fdi->is_patched ||
			  referenced_fdi->rewrite_needed)) {
		_apply_sym_mod_to_id_tok(referenced_fdi->sym_mod,
					 false,
					 id_tok_pos, decl_tic);
	      }

	      _d.append(std::move(decl_tic));
	    },
	    [&](object_init_declarator_info &oidi_in_fd) {
	      const object_info &oidi_in_fd_oi = oidi_in_fd.oi;
	      depreprocessor::transformed_input_chunk decl_tic
		= _prepare_tic_for_decl(oidi_in_fd,
					oidi_in_fd_oi.externalize,
					oidi_in_fd.need_decl_type);

	      _rewrite_references(di_in_fd.declaration_specifiers_deps,
				  decl_tic);
	      _rewrite_references(oidi_in_fd.declarator_deps, decl_tic);

	      if (oidi_in_fd_oi.externalize) {
		if (oidi_in_fd_oi.externalized_sym_mod.sym_mod.new_linkage !=
		    linkage_change::lc_none) {
		  // Unconditionally set the storage class specifier
		  // to "extern": due to the semantics of the "static"
		  // keyword within function scope, the linkage can be
		  // made internal only by a preceeding "static"
		  // declaration outside of function scope to which this
		  // declaration here can bind to.
		  const ast::declaration_specifiers &ds
		    = di_in_fd.declaration.get_declaration_specifiers();
		  _set_storage_class(ds, di_in_fd.storage_class_specifier,
				     ast::storage_class::sc_extern,
				     decl_tic);
		}

		const pp_token_index id_tok_pos =
		  (oidi_in_fd.init_declarator.get_declarator()
		   .get_direct_declarator_id().get_id_tok());
		_apply_sym_mod_to_id_tok
		  (oidi_in_fd_oi.externalized_sym_mod.sym_mod,
		   oidi_in_fd_oi.externalized_sym_mod.make_pointer,
		   id_tok_pos, decl_tic);
	      } else if (oidi_in_fd.get_initializer()) {
		_rewrite_references(oidi_in_fd.get_initializer()->deps,
				    decl_tic);
	      }

	      _d.append(std::move(decl_tic));
	    },
	    [&](const typedef_init_declarator_info &tdidi) {
	      assert(0);
	      __builtin_unreachable();
	    }));
      }
    }

    _d.append(std::move(tic));
    anything_emitted = true;
  }

  return anything_emitted;
}

void _lp_writer::_mark_emitted_tags(tags_infos &tags)
{
  tags.for_each_tag_recursive
    (wrap_callables<no_default_action>
     ([&](sou_def_info &soudi) {
	soudi.emit_tag_decl = false;
	const bool old_emit_def = soudi.emit_def;
	soudi.emit_def = false;
	return old_emit_def;
      },
      [&](sou_ref_info &souri) {
	souri.emit_tag_decl = false;
      },
      [&](enum_def_info &edi) {
	edi.emit_tag_decl = false;
	const bool old_emit_def = edi.emit_def;
	edi.emit_def = false;
	return old_emit_def;
      },
      [&](enum_ref_info &eri) {
	eri.emit_tag_decl = false;
      }));
}

void _lp_writer::
_handle_emitted_tags(tags_infos &tags,
		     depreprocessor::transformed_input_chunk &tic)
{
    tags.for_each_tag_recursive
    (wrap_callables<no_default_action>
     ([&](sou_def_info &soudi) {
	soudi.emit_tag_decl = false;
	if (!soudi.emit_def) {
	  const ast::struct_or_union_def &soud = soudi.soud;
	  assert(soud.has_id());
	  tic.purge_subrange(pp_tokens_range{
				soud.get_id_tok() + 1,
				soud.get_tokens_range().end
			     },
			     true);
	  if (soud.get_asl_before()) {
	    tic.purge_subrange
	      (pp_tokens_range{
			soud.get_asl_before()->get_tokens_range().begin,
			soud.get_id_tok()
	       },
	       false);
	  }

	  return false;

	} else {
	  soudi.emit_def = false;
	  _rewrite_references(soudi.deps, tic);
	  return true;
	}
      },
      [&](sou_ref_info &souri) {
	souri.emit_tag_decl = false;
      },
      [&](enum_def_info &edi) {
	edi.emit_tag_decl = false;
	if (!edi.emit_def) {
	  const ast::enum_def &ed = edi.ed;
	  assert(ed.has_id());
	  tic.purge_subrange(pp_tokens_range{
				ed.get_id_tok() + 1,
				ed.get_tokens_range().end
			     },
			     true);
	  if (ed.get_asl_before()) {
	    tic.purge_subrange
	      (pp_tokens_range{
			ed.get_asl_before()->get_tokens_range().begin,
			ed.get_id_tok()
	       },
	       false);
	  }

	  return false;

	} else {
	  edi.emit_def = false;
	  _rewrite_references(edi.deps, tic);
	  return true;
	}
      },
      [&](enum_ref_info &eri) {
	eri.emit_tag_decl = false;
      }));
}

void _lp_writer::_emit_tags(tags_infos &tags,
			    const ast::declaration * const decl)
{
  tags.for_each_child_tag
    (wrap_callables<no_default_action>
     ([&](sou_def_info &soudi) {
	if (soudi.emit_def) {
	  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	  const pp_tokens_range &r
	    = decl ? decl->get_tokens_range() : soudi.soud.get_tokens_range();
	  depreprocessor::transformed_input_chunk tic{r};
	  tic.copy_subrange(soudi.soud.get_tokens_range(), false, toks);

	  if (decl) {
	    assert (r.end && toks[r.end - 1].is_punctuator(";"));
	    tic.copy_subrange(pp_tokens_range{r.end - 1, r.end}, false, toks);
	  } else {
	    tic.insert_token(r.end,
			     pp_token{
			       pp_token::type::punctuator,
			       ";",
			       raw_pp_tokens_range{}
			     },
			     false, false);
	  }

	  _rewrite_references(soudi.deps, tic);
	  _handle_emitted_tags(soudi.child_tags, tic);
	  _d.append(std::move(tic));
	  soudi.emit_tag_decl = false;
	  soudi.emit_def = false;

	} else if (soudi.emit_tag_decl) {
	  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	  const pp_tokens_range &r
	    = decl ? decl->get_tokens_range() : soudi.soud.get_tokens_range();
	  depreprocessor::transformed_input_chunk tic{r};
	  tic.copy_subrange(pp_tokens_range{
				soudi.soud.get_tokens_range().begin,
				soudi.soud.get_id_tok() + 1
			    },
			    false, toks);
	  if (soudi.soud.get_asl_before()) {
	    tic.purge_subrange
	      (pp_tokens_range{
			soudi.soud.get_asl_before()->get_tokens_range().begin,
			soudi.soud.get_id_tok()
	       },
	       false);
	  }

	  if (decl) {
	    assert (r.end && toks[r.end - 1].is_punctuator(";"));
	    tic.copy_subrange(pp_tokens_range{r.end - 1, r.end}, false, toks);
	  } else {
	    tic.insert_token(r.end,
			     pp_token{
			       pp_token::type::punctuator,
			       ";",
			       raw_pp_tokens_range{}
			     },
			     false, false);
	  }

	  _d.append(std::move(tic));
	  soudi.emit_tag_decl = false;
	}

      },
      [&](sou_ref_info &souri) {
	if (souri.emit_tag_decl) {
	  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	  const pp_tokens_range &r
	    = decl ? decl->get_tokens_range() : souri.sour.get_tokens_range();
	  depreprocessor::transformed_input_chunk tic{r};
	  tic.copy_subrange(souri.sour.get_tokens_range(), false, toks);
	  if (decl) {
	    assert (r.end && toks[r.end - 1].is_punctuator(";"));
	    tic.copy_subrange(pp_tokens_range{r.end - 1, r.end}, false, toks);
	  } else {
	    tic.insert_token(r.end,
			     pp_token{
			       pp_token::type::punctuator,
			       ";",
			       raw_pp_tokens_range{}
			     },
			     false, false);
	  }

	  _d.append(std::move(tic));
	  souri.emit_tag_decl = false;
	}

      },
      [&](enum_def_info &edi) {
	if (edi.emit_def) {
	  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	  const pp_tokens_range &r
	    = decl ? decl->get_tokens_range() : edi.ed.get_tokens_range();
	  depreprocessor::transformed_input_chunk tic{r};
	  tic.copy_subrange(edi.ed.get_tokens_range(), false, toks);

	  if (decl) {
	    assert (r.end && toks[r.end - 1].is_punctuator(";"));
	    tic.copy_subrange(pp_tokens_range{r.end - 1, r.end}, false, toks);
	  } else {
	    tic.insert_token(r.end,
			     pp_token{
			       pp_token::type::punctuator,
			       ";",
			       raw_pp_tokens_range{}
			     },
			     false, false);
	  }

	  _rewrite_references(edi.deps, tic);
	  _handle_emitted_tags(edi.child_tags, tic);
	  _d.append(std::move(tic));
	  edi.emit_tag_decl = false;
	  edi.emit_def = false;

	} else if (edi.emit_tag_decl) {
	  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	  const pp_tokens_range &r
	    = decl ? decl->get_tokens_range() : edi.ed.get_tokens_range();
	  depreprocessor::transformed_input_chunk tic{r};
	  tic.copy_subrange(pp_tokens_range{
				edi.ed.get_tokens_range().begin,
				edi.ed.get_id_tok() + 1
			    },
			    false, toks);
	  if (edi.ed.get_asl_before()) {
	    tic.purge_subrange
	      (pp_tokens_range{
			edi.ed.get_asl_before()->get_tokens_range().begin,
			edi.ed.get_id_tok()
	       },
	       false);
	  }

	  if (decl) {
	    assert (r.end && toks[r.end - 1].is_punctuator(";"));
	    tic.copy_subrange(pp_tokens_range{r.end - 1, r.end}, false, toks);
	  } else {
	    tic.insert_token(r.end,
			     pp_token{
			       pp_token::type::punctuator,
			       ";",
			       raw_pp_tokens_range{}
			     },
			     false, false);
	  }

	  _d.append(std::move(tic));
	  edi.emit_tag_decl = false;
	}

      },
      [&](enum_ref_info &eri) {
	if (eri.emit_tag_decl) {
	  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
	  const pp_tokens_range &r
	    = decl ? decl->get_tokens_range() : eri.er.get_tokens_range();
	  depreprocessor::transformed_input_chunk tic{r};
	  tic.copy_subrange(eri.er.get_tokens_range(), false, toks);
	  if (decl) {
	    assert (r.end && toks[r.end - 1].is_punctuator(";"));
	    tic.copy_subrange(pp_tokens_range{r.end - 1, r.end}, false, toks);
	  } else {
	    tic.insert_token(r.end,
			     pp_token{
			       pp_token::type::punctuator,
			       ";",
			       raw_pp_tokens_range{}
			     },
			     false, false);
	  }

	  _d.append(std::move(tic));
	  eri.emit_tag_decl = false;
	}

      }));
}

bool _lp_writer::_any_tag_needed(const tags_infos &tags) noexcept
{
  bool any_needed = false;

  tags.for_each_child_tag
    (wrap_callables<no_default_action>
     ([&](const sou_def_info &soudi) {
	if (soudi.emit_def || soudi.emit_tag_decl)
	  any_needed = true;
      },
      [&](const sou_ref_info &souri) {
	if (souri.emit_tag_decl)
	  any_needed = true;
      },
      [&](const enum_def_info &edi) {
	if (edi.emit_def || edi.emit_tag_decl)
	  any_needed = true;
      },
      [&](const enum_ref_info &eri) {
	if (eri.emit_tag_decl)
	  any_needed = true;
      }));

  return any_needed;
}

depreprocessor::transformed_input_chunk
_lp_writer::_prepare_tic_for_decl(function_init_declarator_info &fidi)
{
  const ast::init_declarator &id = fidi.init_declarator;
  declaration_info &di = fidi.enclosing_declaration;
  const ast::declaration &d = di.declaration;
  depreprocessor::transformed_input_chunk tic = _create_tic_for_decl(d, id);

  _handle_emitted_tags(di.child_tags, tic);
  _handle_emitted_tags(fidi.child_tags, tic);
  return tic;
}

depreprocessor::transformed_input_chunk
_lp_writer::_prepare_tic_for_decl(object_init_declarator_info &oidi,
				  const bool strip_initializer,
				  const bool need_decl_type)
{
  const ast::init_declarator &id = oidi.init_declarator;
  declaration_info &di = oidi.enclosing_declaration;
  const ast::declaration &d = di.declaration;
  depreprocessor::transformed_input_chunk tic = _create_tic_for_decl(d, id);

  _handle_emitted_tags(di.child_tags, tic);
  _handle_emitted_tags(oidi.child_tags, tic);

  initializer_info * const ii = oidi.get_initializer();
  if (ii) {
    if (strip_initializer) {
      const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
      pp_tokens_range initializer_range =
	id.get_initializer()->get_tokens_range();

      while (!toks[initializer_range.begin].is_punctuator("=")) {
	assert(initializer_range.begin);
	--initializer_range.begin;
      }
      while (toks[initializer_range.begin - 1]
	     .is_type_any_of<pp_token::type::ws, pp_token::type::newline>()) {
	--initializer_range.begin;
      }
      if (id.get_asl_after())
	initializer_range.end = id.get_asl_after()->get_tokens_range().begin;

      tic.purge_subrange(initializer_range, !!id.get_asl_after());

      // The initializer's length might have completed an array type.
      // If so (and if it's needed), insert the length for the direct
      // array declarator's length expression.
      if (need_decl_type) {
	const ast::direct_declarator_id &ddid
	  = id.get_declarator().get_direct_declarator_id();

	const types::array_type * const at =
	  (types::handle_types<const types::array_type*>
	   ((wrap_callables<default_action_return_value
				<const types::array_type*, nullptr>::type>
	     ([](const types::array_type &_at) -> const types::array_type* {
		return &_at;
	      })),
	   *ddid.get_type()));
	if (at) {
	  assert(at->is_complete());

	  const ast::direct_declarator_array *dda = nullptr;
	  ddid.for_each_ancestor<type_set<ast::direct_declarator_array,
					  ast::declarator,
					  ast::direct_declarator_func>>
	    (wrap_callables<no_default_action>
	     ([&](const ast::direct_declarator_array &_dda) {
		dda = &_dda;
	      },
	      [&](const ast::declarator&) {},
	      [&](const ast::direct_declarator_func&) {}));
	  if (dda && !dda->get_size()) {
	    std::string length = at->get_length().to_string(mpa::limb{10});
	    assert(dda->get_tokens_range().end);
	    const raw_pp_token_index insert_pos =
	      dda->get_tokens_range().end - 1;
	    assert(toks[insert_pos].is_punctuator("]"));
	    tic.insert_token(insert_pos,
			     pp_token{
			       pp_token::type::pp_number,
			       std::move(length),
			       raw_pp_tokens_range{},
			     },
			     true, false);
	  }
	}
      }

    } else {
      _handle_emitted_tags(ii->child_tags, tic);

    }
  }

  return tic;
}

depreprocessor::transformed_input_chunk
_lp_writer::_prepare_tic_for_decl(typedef_init_declarator_info &tdidi)
{
  const ast::init_declarator &id = tdidi.init_declarator;
  declaration_info &di = tdidi.enclosing_declaration;
  const ast::declaration &d = di.declaration;
  depreprocessor::transformed_input_chunk tic = _create_tic_for_decl(d, id);

  _handle_emitted_tags(di.child_tags, tic);
  _handle_emitted_tags(tdidi.child_tags, tic);
  return tic;
}

depreprocessor::transformed_input_chunk
_lp_writer::_prepare_tic_for_decl(function_definition_info &fdi)
{
  const ast::function_definition &fd = fdi.function_definition;

  const pp_tokens_range r{
		fd.get_tokens_range().begin,
		(!fd.get_asl() ? fd.get_declarator().get_tokens_range().end :
		 fd.get_asl()->get_tokens_range().end)
  };
  depreprocessor::transformed_input_chunk tic{r};
  tic.copy_subrange(r, false, _ai.atu.get_pp_result().get_pp_tokens());
  tic.insert_token(r.end,
		   pp_token{
		     pp_token::type::punctuator,
		     ";",
		     raw_pp_tokens_range{}
		   },
		   false, false);

  _handle_emitted_tags(fdi.child_tags, tic);
  return tic;
}


depreprocessor::transformed_input_chunk
_lp_writer::_create_tic_for_decl(const ast::declaration &d,
				 const ast::init_declarator &id)
{
  depreprocessor::transformed_input_chunk tic{d.get_tokens_range()};
  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
  tic.copy_subrange(d.get_declaration_specifiers().get_tokens_range(), false,
		    toks);

  tic.copy_subrange(id.get_tokens_range(), true, toks);

  assert(d.get_tokens_range().end
	 && toks[d.get_tokens_range().end - 1].is_punctuator(";"));

  tic.copy_subrange(pp_tokens_range{
			d.get_tokens_range().end - 1,
			d.get_tokens_range().end
		    },
		    true, toks);

  return tic;
}

void _lp_writer::
_apply_sym_mod_to_id_tok(const lp_creation_policy::symbol_modification &sym_mod,
			 const bool make_pointer,
			 const pp_token_index id_tok_pos,
			 depreprocessor::transformed_input_chunk &tic)
{
  if (sym_mod.is_rename()) {
    tic.replace_token(id_tok_pos,
		      pp_token{
			pp_token::type::id,
			sym_mod.new_name,
			raw_pp_tokens_range{},
		      },
		      make_pointer);
  } else if (make_pointer) {
    pp_token id_tok =
      _ai.atu.get_pp_result().get_pp_tokens()[id_tok_pos];
    tic.replace_token(id_tok_pos, std::move(id_tok), true);

  }
}

void _lp_writer::_apply_linkage_change_for_fun
	(const lp_creation_policy::symbol_modification::linkage_change lc,
	 const bool make_pointer,
	 const ast::linkage &l,
	 const ast::declaration_specifiers &ds,
	 const ast::storage_class_specifier * const scs,
	 depreprocessor::transformed_input_chunk &tic)
{
  using linkage_change =
    lp_creation_policy::symbol_modification::linkage_change;

  if (l.is_first_in_chain()) {
    switch (lc) {
    case linkage_change::lc_none:
      break;

    case linkage_change::lc_make_static:
      if (!scs || scs->get_storage_class() != ast::storage_class::sc_static)
	_set_storage_class(ds, scs, ast::storage_class::sc_static, tic);
      break;

    case linkage_change::lc_make_extern:
      if ((!scs && make_pointer) ||
	  (scs && scs->get_storage_class() != ast::storage_class::sc_extern))
	_set_storage_class(ds, scs, ast::storage_class::sc_extern, tic);
    }
  } else {
    switch (lc) {
    case linkage_change::lc_none:
      break;

    case linkage_change::lc_make_static:
      if (scs && scs->get_storage_class() != ast::storage_class::sc_static)
	_set_storage_class(ds, scs, ast::storage_class::sc_none, tic);
      break;

    case linkage_change::lc_make_extern:
      if (!scs && make_pointer) {
	_set_storage_class(ds, scs, ast::storage_class::sc_extern, tic);
      } else if (scs &&
		 scs->get_storage_class() != ast::storage_class::sc_extern) {
	_set_storage_class(ds, scs, ast::storage_class::sc_none, tic);
      }
      break;
    }
  }
}

void _lp_writer::
_set_storage_class(const ast::declaration_specifiers &ds,
		   const ast::storage_class_specifier *old_scs,
		   const ast::storage_class new_sc,
		   depreprocessor::transformed_input_chunk &tic)
{
  const pp_tokens &toks = _ai.atu.get_pp_result().get_pp_tokens();
  if (old_scs) {
    if (old_scs->get_storage_class() == new_sc)
      return;

    switch (new_sc) {
    case ast::storage_class::sc_none:
      {
	// Remove the existing storage class specifier and
	// all whitespace surrounding it.
	pp_tokens_range r = old_scs->get_tokens_range();
	while (r.begin != ds.get_tokens_range().begin &&
	       toks[r.begin - 1].is_type_any_of<pp_token::type::ws,
						pp_token::type::newline>()) {
	  --r.begin;
	}
	while (r.end != ds.get_tokens_range().end &&
	       toks[r.end].is_type_any_of<pp_token::type::ws,
					  pp_token::type::newline>()) {
	  ++r.end;
	}

	tic.purge_subrange(r, true);
      }
      break;

    case ast::storage_class::sc_extern:
      {
	const pp_tokens_range &r = old_scs->get_tokens_range();
	assert(r.end == r.begin + 1);
	tic.replace_token(r.begin,
			  pp_token{
			    pp_token::type::id,
			    "extern",
			    raw_pp_tokens_range{}
			  },
			  false);
      }
      break;

    case ast::storage_class::sc_static:
      {
	const pp_tokens_range &r = old_scs->get_tokens_range();
	assert(r.end == r.begin + 1);
	tic.replace_token(r.begin,
			  pp_token{
			    pp_token::type::id,
			    "static",
			    raw_pp_tokens_range{}
			  },
			  false);
      }
      break;

    case ast::storage_class::sc_typedef:
      /* fall through */
    case ast::storage_class::sc_auto:
      /* fall through */
    case ast::storage_class::sc_register:
      /* fall through */
      assert(0);
      __builtin_unreachable();
    }

  } else {
    switch (new_sc) {
    case ast::storage_class::sc_none:
      break;

    case ast::storage_class::sc_extern:
      tic.insert_token(ds.get_tokens_range().begin,
		       pp_token{
			 pp_token::type::id,
			 "extern",
			 raw_pp_tokens_range{}
		       },
		       false, true);
      break;

    case ast::storage_class::sc_static:
      tic.insert_token(ds.get_tokens_range().begin,
		       pp_token{
			 pp_token::type::id,
			 "static",
			 raw_pp_tokens_range{}
		       },
		       false, true);
      break;

    case ast::storage_class::sc_typedef:
      /* fall through */
    case ast::storage_class::sc_auto:
      /* fall through */
    case ast::storage_class::sc_register:
      /* fall through */
      assert(0);
      __builtin_unreachable();
    }
  }
}

void _lp_writer::
_rewrite_references(const deps_on &deps,
		    depreprocessor::transformed_input_chunk &tic)
{
  _rewrite_references(deps.on_types, tic);
  _rewrite_references(deps.on_objs, tic);
  _rewrite_references(deps.on_funs, tic);
}

void _lp_writer::
_rewrite_references(const deps_on_types &deps,
		    depreprocessor::transformed_input_chunk &tic)
{
  _rewrite_references(deps.on_decl_types, tic);
}

void _lp_writer::
_rewrite_references(const deps_on_decl_types &deps,
		    depreprocessor::transformed_input_chunk &tic)
{
  for (const auto &d : deps) {
    switch (d.get_kind()) {
    case dep_on_decl_type::kind::k_function_definition:
      {
	const function_definition_info &fdi = d.get_fdi();
	const function_info &fi = fdi.fi;
	if (fi.externalize) {
	  _apply_sym_mod_to_id_tok(fi.externalized_sym_mod.sym_mod,
				   fi.externalized_sym_mod.make_pointer,
				   d.from_eid.get_id_tok(),
				   tic);
	} else if (fi.in_closure || fdi.is_patched) {
	  _apply_sym_mod_to_id_tok(fdi.sym_mod,
				   false,
				   d.from_eid.get_id_tok(),
				   tic);
	}
      }
      break;

    case dep_on_decl_type::kind::k_function_init_declarator:
      {
	const function_init_declarator_info &fidi = d.get_fidi();
	const function_info &fi = fidi.fi;
	const function_definition_info * const fdi =
	  fi.get_function_definition();
	if (fi.externalize) {
	  _apply_sym_mod_to_id_tok(fi.externalized_sym_mod.sym_mod,
				   fi.externalized_sym_mod.make_pointer,
				   d.from_eid.get_id_tok(),
				   tic);
	} else if (fdi && (fi.in_closure || fdi->is_patched)) {
	  _apply_sym_mod_to_id_tok(fdi->sym_mod,
				   false,
				   d.from_eid.get_id_tok(),
				   tic);
	}
      }
      break;

    case dep_on_decl_type::kind::k_object_init_declarator:
      {
	const object_init_declarator_info &oidi = d.get_oidi();
	const object_info &oi = oidi.oi;
	if (oi.externalize) {
	  _apply_sym_mod_to_id_tok(oi.externalized_sym_mod.sym_mod,
				   oi.externalized_sym_mod.make_pointer,
				   d.from_eid.get_id_tok(),
				   tic);
	}
      }
      break;
    }
  }
}

void _lp_writer::
_rewrite_references(const deps_on_objs &deps,
		    depreprocessor::transformed_input_chunk &tic)
{
  for (const auto &d : deps) {
    if (!d.from_eid)
      continue;

    if (d.externalize) {
      const object_info &oi = d.oidi.get().oi;
      assert(oi.externalize);
      _apply_sym_mod_to_id_tok(oi.externalized_sym_mod.sym_mod,
			       oi.externalized_sym_mod.make_pointer,
			       d.from_eid->get_id_tok(),
			       tic);
    }
  }
}

void _lp_writer::
_rewrite_references(const deps_on_funs &deps,
		    depreprocessor::transformed_input_chunk &tic)
{
  for (const auto &d : deps) {
    const function_info &fi = d.get_function_info();
    const function_definition_info * const fdi = fi.get_function_definition();
    assert(fi.externalize || fi.in_closure || (fdi && fdi->is_patched));
    if (d.externalize) {
      assert(fi.externalize);
      _apply_sym_mod_to_id_tok(fi.externalized_sym_mod.sym_mod,
			       fi.externalized_sym_mod.make_pointer,
			       d.from_eid.get_id_tok(),
			       tic);
    } else if (fdi) {
      _apply_sym_mod_to_id_tok(fdi->sym_mod, false,
			       d.from_eid.get_id_tok(),
			       tic);
    }
  }
}



void klp::ccp::create_lp(const std::string &outfile,
			 const ast::ast_translation_unit &atu,
			 const lp_creation_policy &pol,
			 code_remarks &remarks,
			 output_remarks &output_remarks)
{
  ast_info ai{atu};
  // Populate the ast_info and collect all interdependencies.
  _ast_info_collector{ai, remarks}();

  for (auto it_fi = ai.functions_begin(); it_fi != ai.functions_end();
       ++it_fi) {
    function_definition_info * const fdi = it_fi->get_function_definition();
    if (fdi && pol.is_patched(fdi->function_definition, remarks))
      fdi->is_patched = true;
  }

  // Find all objects and functions reachable from the patched
  // functions and decide on externalizations.
  _closure_builder{ai, pol, remarks}();

  // Recursively rename rewritten resp. externalized functions and
  // objects.
  _symbol_modification_resolver{ai, pol, remarks}();

  // Determine which of the headers from the original input
  // can be included from the live patch.
  _conflicting_header_finder{ai, pol, remarks}();

  // Recursively find all dependencies on types and declarations and
  // determine which headers are to be included from the live patch.
  _lp_deps_resolver{ai, pol, remarks}();

  // Write the live patch.
  depreprocessor d{atu.get_pp_result(), output_remarks};
  _lp_writer{ai, d}();
  d(outfile);
}
