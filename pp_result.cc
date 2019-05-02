#include <cassert>
#include "pp_result.hh"
#include "raw_pp_token.hh"
#include "macro.hh"
#include "macro_undef.hh"
#include "source_reader.hh"
#include <limits>

using namespace klp::ccp;


pp_result::macro_invocation::
macro_invocation(const macro &m,
		 const raw_pp_tokens_range &invocation_range)
  : _invocation_range{invocation_range}
{
  _used_macros += m;
}


pp_result::inclusion_node::inclusion_node()
  : _parent(nullptr)
{}

pp_result::inclusion_node::inclusion_node(inclusion_node * const parent,
					  const raw_pp_token_index range_begin)
  : _parent(parent), _range(range_begin, range_begin)
{}

pp_result::inclusion_node::~inclusion_node() noexcept = default;

pp_result::inclusion_node::recursive_header_inclusion_iterator
pp_result::inclusion_node::header_inclusions_recursive_begin() const
{
  return recursive_header_inclusion_iterator{_recursive_child_iterator{*this}};
}

pp_result::inclusion_node::recursive_header_inclusion_iterator
pp_result::inclusion_node::header_inclusions_recursive_end() const
{
  return recursive_header_inclusion_iterator{};
}


pp_result::header_inclusion_child& pp_result::inclusion_node::
_add_header_inclusion(const std::string &filename,
		      const raw_pp_tokens_range &directive_range,
		      used_macros &&used_macros,
		      used_macro_undefs &&used_macro_undefs)
{
  std::unique_ptr<header_inclusion_child> new_child
     (new header_inclusion_child(*this, filename,
				 directive_range,
				 std::move(used_macros),
				 std::move(used_macro_undefs)));
  header_inclusion_child &_new_child = *new_child;
  _children.emplace_back(std::move(new_child));
  return _new_child;
}

pp_result::conditional_inclusion_node& pp_result::inclusion_node::
_add_conditional_inclusion(const raw_pp_token_index range_begin,
			   used_macros &&used_macros,
			   used_macro_undefs &&used_macro_undefs)
{
  std::unique_ptr<conditional_inclusion_node> new_child
     (new conditional_inclusion_node(*this, range_begin,
				     std::move(used_macros),
				     std::move(used_macro_undefs)));
  conditional_inclusion_node &_new_child = *new_child;
  _children.emplace_back(std::move(new_child));
  return _new_child;
}

void pp_result::inclusion_node::
_set_range_begin(const raw_pp_token_index range_begin) noexcept
{
  _range.begin = range_begin;
}

void pp_result::inclusion_node::
_set_range_end(const raw_pp_token_index range_end) noexcept
{
  _range.end = range_end;
}

pp_result::inclusion_node::_child::
_child(std::unique_ptr<header_inclusion_child> &&_h) noexcept
  : k(kind::header), h(std::move(_h))
{}

pp_result::inclusion_node::_child::
_child(std::unique_ptr<conditional_inclusion_node> &&_c) noexcept
  : k(kind::conditional), c(std::move(_c))
{}

pp_result::inclusion_node::_child::_child(_child &&chld) noexcept
  : k(chld.k)
{
  switch(k) {
  case kind::header:
    h = std::move(chld.h);
    break;

  case kind::conditional:
    c = std::move(chld.c);
    break;
  }
}

pp_result::inclusion_node::_child::~_child() noexcept
{
  switch (k) {
  case kind::header:
    h.~unique_ptr<header_inclusion_child>();
    break;

  case kind::conditional:
    c.~unique_ptr<conditional_inclusion_node>();
    break;
  }
}

const pp_result::inclusion_node&
pp_result::inclusion_node::_child::get_inclusion_node() const noexcept
{
  switch (k) {
  case kind::header:
    return *h;

  case kind::conditional:
    return *c;
  }
}


pp_result::inclusion_node::_recursive_child_iterator::
_recursive_child_iterator() = default;

pp_result::inclusion_node::_recursive_child_iterator::
_recursive_child_iterator(const inclusion_node &n)
{
  if (!n._children.empty()) {
    _walk_stack.push_back(std::make_pair(n._children.begin(),
					 n._children.end()));
  }
}

pp_result::inclusion_node::_recursive_child_iterator&
pp_result::inclusion_node::_recursive_child_iterator::operator++()
{
  // DFS in preorder
  const _children_container_type &children
    = (_walk_stack.back().first->k == _child::kind::header ?
       _walk_stack.back().first->h->_children :
       _walk_stack.back().first->c->_children);

  if (!children.empty()) {
    _walk_stack.push_back(std::make_pair(children.begin(), children.end()));
    return *this;
  }

  ++_walk_stack.back().first;
  while (_walk_stack.back().first == _walk_stack.back().second) {
    _walk_stack.pop_back();
    if (_walk_stack.empty())
      return *this;
    ++_walk_stack.back().first;
  }

  return *this;
}

const pp_result::inclusion_node::_recursive_child_iterator
pp_result::inclusion_node::_recursive_child_iterator::operator++(int)
{
  _recursive_child_iterator it = *this;
  ++*this;
  return it;
}


pp_result::inclusion_node::recursive_header_inclusion_iterator::
recursive_header_inclusion_iterator() = default;

pp_result::inclusion_node::recursive_header_inclusion_iterator::
recursive_header_inclusion_iterator(_recursive_child_iterator &&it)
  : _it(std::move(it))
{
  _skip_non_headers();
}

pp_result::inclusion_node::recursive_header_inclusion_iterator&
pp_result::inclusion_node::recursive_header_inclusion_iterator::operator++()
{
  ++_it;
  _skip_non_headers();
  return *this;
}

const pp_result::inclusion_node::recursive_header_inclusion_iterator
pp_result::inclusion_node::recursive_header_inclusion_iterator::operator++(int)
{
  recursive_header_inclusion_iterator it = *this;
  ++*this;
  return it;
}

void pp_result::inclusion_node::recursive_header_inclusion_iterator::
_skip_non_headers()
{
  while(!_it.is_end() && _it->k != _child::kind::header)
    ++_it;
}


pp_result::const_intersecting_source_iterator::
const_intersecting_source_iterator(const header_inclusion_roots &roots,
				   const raw_pp_tokens_range &range,
				   const init_begin_iterator_tag&)
  : _range(range), _pos(_range.begin), _cur_val(nullptr)
{
  assert(!roots.empty());

  std::tie(_roots_begin, _roots_end) =
    _intersecting_roots_subrange(roots.begin(), roots.end());

  if (_roots_begin != roots.end()) {
    _roots_it = _roots_begin;
    _enter_root();
  } else {
    // Range is an empty range at EOF, special case it.
    --_roots_begin;
    _roots_it = _roots_begin;
    assert(range.begin == range.end);
    assert(range.begin == _roots_begin->get_range().end);
    _cur_val = &*_roots_begin;
  }
}

pp_result::const_intersecting_source_iterator::
const_intersecting_source_iterator(const header_inclusion_roots &roots,
				   const raw_pp_tokens_range &range,
				   const init_end_iterator_tag&)
  : _range(range), _pos(_range.end), _cur_val(nullptr)
{
  assert(!roots.empty());

  std::tie(_roots_begin, _roots_end) =
    _intersecting_roots_subrange(roots.begin(), roots.end());

  if (_roots_begin == roots.end()) {
    // C.f. begin iterator construction
    --_roots_begin;
  }
  _roots_it = _roots_end;
}

bool pp_result::const_intersecting_source_iterator::
operator==(const const_intersecting_source_iterator &rhs) const noexcept
{
  if (this->_pos != rhs._pos)
    return false;
  else if (this->_cur_val != rhs._cur_val)
    return false;
  else if (this->_range != rhs._range)
    return false;
  else
    return true;
}

pp_result::const_intersecting_source_iterator&
pp_result::const_intersecting_source_iterator::operator++()
{
  typedef inclusion_node::_child _child;

  if (_walk_stack.empty()) {
  next_root:
    ++_roots_it;
    if (_roots_it == _roots_end) {
      _pos = _range.end;
      _cur_val = nullptr;
      return *this;
    }

    assert(_pos <= _roots_it->get_range().begin);
    _pos = _roots_it->get_range().begin;
    _enter_root();
    return *this;
  }

  assert(_walk_stack.back().chlds_it->k == _child::kind::header);

  if (_pos < _walk_stack.back().chlds_it->h->get_range().begin) {
    // The current _pos is somewhere to the left of the current
    // leaf header. Find the deepest header coming next.
    const auto beginning_gt_pos =
      std::find_if(_walk_stack.cbegin(), _walk_stack.cend(),
		   [&](const _walk_stack_entry &e) {
		     return (e.chlds_it->k == _child::kind::header &&
			     e.chlds_it->h->get_range().begin > _pos);
		   });
    assert(beginning_gt_pos != _walk_stack.cend());
    _pos = beginning_gt_pos->chlds_it->h->get_range().begin;
    const auto deepest =
      std::find_if(_walk_stack.crbegin(), _walk_stack.crend(),
		   [&](const _walk_stack_entry &e) {
		     return (e.chlds_it->k == _child::kind::header &&
			     e.chlds_it->h->get_range().begin == _pos);
		   });
    assert(deepest != _walk_stack.crend());
    _cur_val = deepest->chlds_it->h.get();

  } else {
    assert(_walk_stack.back().chlds_it->h->get_range().begin <= _pos);
    assert(_pos <= _walk_stack.back().chlds_it->h->get_range().end);
    const raw_pp_token_index end_pos =
      _walk_stack.back().chlds_it->h->get_range().end;
    if (_range.end <= end_pos) {
      // We're done.
      _pos = _range.end;
      ++_roots_it;
      assert(_roots_it == _roots_end);
      _walk_stack.clear();
      _cur_val = nullptr;
      return *this;
    }

    _pos = end_pos;

    // Pop all ancestor headers off the stack which have their
    // ->get_range().end == end_pos and forward the walk stack to the
    // next header.
    while (_walk_stack.back().chlds_it->h->get_range().end == end_pos) {
      ++_walk_stack.back().chlds_it;
      while (_walk_stack.back().chlds_it == _walk_stack.back().chlds_end) {
	_walk_stack.pop_back();
	if (_walk_stack.empty())
	  break;
	++_walk_stack.back().chlds_it;
      }
      if (!_walk_stack.empty())
	_fwd_walk_to_header_dfs_po();

      if (_walk_stack.empty()) {
	if (_roots_it->get_range().end == end_pos) {
	  goto next_root;
	} else {
	  _cur_val = &*_roots_it;
	  return *this;
	}
      }

      assert(_walk_stack.back().chlds_it != _walk_stack.back().chlds_end);
      assert(_walk_stack.back().chlds_it->k == _child::kind::header);
    }

    assert(_walk_stack.back().chlds_it->h->get_range().end > end_pos);
    auto deepest_beginning_leq_pos =
      std::find_if(_walk_stack.crbegin(), _walk_stack.crend(),
		   [&](const _walk_stack_entry &e) {
		     return (e.chlds_it->k == _child::kind::header &&
			     e.chlds_it->h->get_range().begin <= _pos);
		   });

    if (deepest_beginning_leq_pos == _walk_stack.crend()) {
      _cur_val = &*_roots_it;
    } else {
      _cur_val = deepest_beginning_leq_pos->chlds_it->h.get();
    }
  }

  return *this;
}

const pp_result::const_intersecting_source_iterator
pp_result::const_intersecting_source_iterator::operator++(int)
{
  const_intersecting_source_iterator it = *this;
  ++*this;
  return it;
}

std::pair<pp_result::header_inclusion_roots::const_iterator,
	  pp_result::header_inclusion_roots::const_iterator>
pp_result::const_intersecting_source_iterator::_intersecting_roots_subrange
			(const header_inclusion_roots::const_iterator &b,
			 const header_inclusion_roots::const_iterator &e)
{
  const auto srb =
    std::lower_bound(b, e, _range,
		     [&](const header_inclusion_root &root,
			 const raw_pp_tokens_range &range) {
		       return root.get_range() < range;
		     });
  const auto sre =
    std::upper_bound(srb, e, _range,
		     [&](const raw_pp_tokens_range &range,
			 const header_inclusion_root &root) {
		       return range < root.get_range();
		     });

  return std::make_pair(srb, sre);
}

std::pair<pp_result::inclusion_node::_children_container_type::const_iterator,
	  pp_result::inclusion_node::_children_container_type::const_iterator>
pp_result::const_intersecting_source_iterator::_intersecting_chlds_subrange
	(const inclusion_node::_children_container_type::const_iterator &b,
	 const inclusion_node::_children_container_type::const_iterator &e)
{
  const auto srb =
    std::lower_bound(b, e, _range,
		     [](const inclusion_node::_child &c,
			const raw_pp_tokens_range &r) {
		       return (c.get_inclusion_node().get_range() < r);
		     });
  const auto sre =
    std::upper_bound(srb, e, _range,
		     [](const raw_pp_tokens_range &r,
			const inclusion_node::_child &c) {
		       return r < c.get_inclusion_node().get_range();
		     });
  return std::make_pair(srb, sre);
}


void pp_result::const_intersecting_source_iterator::_enter_root()
{
  typedef inclusion_node::_child _child;

  auto chlds_sr = _intersecting_chlds_subrange(_roots_it->_children.cbegin(),
					       _roots_it->_children.end());
  if (chlds_sr.first != chlds_sr.second) {
    _walk_stack.emplace_back(_walk_stack_entry{chlds_sr.first,
					       chlds_sr.first,
					       chlds_sr.second});
    _fwd_walk_to_header_dfs_po();
  }

  auto deepest_beginning_leq_pos =
    std::find_if(_walk_stack.crbegin(), _walk_stack.crend(),
		 [&](const _walk_stack_entry &e) {
		   return (e.chlds_it->k == _child::kind::header &&
			   e.chlds_it->h->get_range().begin <= _pos);
		 });
  if (deepest_beginning_leq_pos == _walk_stack.crend()) {
    _cur_val = &*_roots_it;
  } else {
    _cur_val = deepest_beginning_leq_pos->chlds_it->h.get();
  }
}

void pp_result::const_intersecting_source_iterator::_fwd_walk_to_header_dfs_po()
{
  typedef inclusion_node::_child _child;

  assert(!_walk_stack.empty());
  assert(_walk_stack.back().chlds_it != _walk_stack.back().chlds_end);
  while (true) {
    // Determine begin and end of children overlapping with the range.
    const _child &cur = *_walk_stack.back().chlds_it;
    const inclusion_node &n = cur.get_inclusion_node();
    auto chlds_sr = _intersecting_chlds_subrange(n._children.cbegin(),
					       n._children.end());
    if (chlds_sr.first != chlds_sr.second) {
      // Descend
      _walk_stack.emplace_back(_walk_stack_entry{chlds_sr.first,
						 chlds_sr.first,
						 chlds_sr.second});

    } else {
      // Go to next sibling or up, but leave only non-header nodes.
      if (_walk_stack.back().chlds_it->k == _child::kind::header)
	return;

      ++_walk_stack.back().chlds_it;
      while (_walk_stack.back().chlds_it == _walk_stack.back().chlds_end) {
	_walk_stack.pop_back();
	if (_walk_stack.empty() ||
	    _walk_stack.back().chlds_it->k == _child::kind::header) {
	  return;
	}
	++_walk_stack.back().chlds_it;
      }
    }
  }
}


pp_result::header_inclusion_node::
header_inclusion_node(const std::string &filename)
  : _filename(filename)
{}

pp_result::header_inclusion_node::
header_inclusion_node(inclusion_node &parent,
		      const raw_pp_token_index range_begin,
		      const std::string &filename)
  : inclusion_node(&parent, range_begin), _filename(filename)
{}

pp_result::header_inclusion_node::~header_inclusion_node() noexcept = default;

const pp_result::header_inclusion_node&
pp_result::header_inclusion_node::get_containing_header() const noexcept
{
  return *this;
}

void pp_result::header_inclusion_node::add_line(const std::streamoff length)
{
  _offset_to_line_col_map.add_line(length);
}

std::pair<std::streamoff, std::streamoff>
pp_result::header_inclusion_node::offset_to_line_col(const std::streamoff off)
  const noexcept
{
  return _offset_to_line_col_map.offset_to_line_col(off);
}

std::unique_ptr<source_reader>
pp_result::header_inclusion_node::create_source_reader() const
{
  return std::unique_ptr<source_reader>(new file_source_reader(_filename));
}


pp_result::header_inclusion_root::
header_inclusion_root(const std::string &filename, const bool is_preinclude)
  : header_inclusion_node(filename), _is_preinclude(is_preinclude)
{}

pp_result::header_inclusion_root::~header_inclusion_root() noexcept = default;


pp_result::header_inclusion_child::
header_inclusion_child(inclusion_node &parent,
		       const std::string &filename,
		       const raw_pp_tokens_range &directive_range,
		       used_macros &&used_macros,
		       used_macro_undefs &&used_macro_undefs)
  : header_inclusion_node(parent, directive_range.end, filename),
    _directive_range(directive_range),
    _used_macros(std::move(used_macros)),
    _used_macro_undefs(std::move(used_macro_undefs))
{}

pp_result::header_inclusion_child::~header_inclusion_child() noexcept = default;


pp_result::conditional_inclusion_node::
conditional_inclusion_node(inclusion_node &parent,
			   const raw_pp_token_index range_begin,
			   used_macros &&used_macros,
			   used_macro_undefs &&used_macro_undefs)
  : inclusion_node(&parent, range_begin),
    _used_macros(std::move(used_macros)),
    _used_macro_undefs(std::move(used_macro_undefs))
{}

pp_result::conditional_inclusion_node::~conditional_inclusion_node() = default;

const pp_result::header_inclusion_node&
pp_result::conditional_inclusion_node::get_containing_header() const noexcept
{
  return _parent->get_containing_header();
}


pp_result::header_inclusion_roots::header_inclusion_roots() = default;

pp_result::header_inclusion_roots::
header_inclusion_roots(_container_type &&roots)
  : _roots(std::move(roots))
{}

pp_result::header_inclusion_roots::
header_inclusion_roots(header_inclusion_roots &&hirs)
  : _roots(std::move(hirs._roots))
{}

pp_result::header_inclusion_roots::~header_inclusion_roots() noexcept = default;


pp_result::pp_result() = default;

pp_result::pp_result(header_inclusion_roots &&header_inclusion_roots)
  : _header_inclusion_roots(std::move(header_inclusion_roots))
{}

std::pair<pp_result::const_macro_invocation_iterator,
	  pp_result::const_macro_invocation_iterator>
pp_result::find_overlapping_macro_invocations(const raw_pp_tokens_range &r)
  const
{
  const const_macro_invocation_iterator range_begin
    = std::lower_bound(macro_invocations_begin(), macro_invocations_end(), r);
  const const_macro_invocation_iterator range_end
    = std::upper_bound(range_begin, macro_invocations_end(), r);

  return std::make_pair(range_begin, range_end);
}

pp_result::const_intersecting_source_iterator
pp_result::intersecting_sources_begin(const raw_pp_tokens_range &range) const
{
  return (const_intersecting_source_iterator
	  {_header_inclusion_roots, range,
	   const_intersecting_source_iterator::init_begin_iterator_tag{}});
}

pp_result::const_intersecting_source_iterator
pp_result::intersecting_sources_end(const raw_pp_tokens_range &range) const
{
  return (const_intersecting_source_iterator
	  {_header_inclusion_roots, range,
	   const_intersecting_source_iterator::init_end_iterator_tag{}});
}

const pp_result::header_inclusion_node&
pp_result::get_raw_token_source(const raw_pp_token_index tok_index) const
{
  const auto it =
    this->intersecting_sources_begin(raw_pp_tokens_range{tok_index});
  assert(it != this->intersecting_sources_end(raw_pp_tokens_range{tok_index}));
  return *it;
}

void pp_result::_append_token(const raw_pp_token &tok)
{
  _raw_tokens.push_back(tok);
}

void pp_result::_append_token(raw_pp_token &&tok)
{
  _raw_tokens.push_back(std::move(tok));
}

void pp_result::_append_token(pp_token &&tok)
{
  _pp_tokens.push_back(std::move(tok));
}

raw_pp_tokens::size_type pp_result::_get_last_raw_index() const noexcept
{
  assert(!_raw_tokens.empty());
  return _raw_tokens.size() - 1;
}

pp_tokens::size_type pp_result::_get_last_pp_index() const noexcept
{
  assert(!_pp_tokens.empty());
  return _pp_tokens.size() - 1;
}

pp_result::macro_invocation& pp_result::
_add_macro_invocation(const macro &m,
		      const raw_pp_tokens_range &invocation_range)
{
  _macro_invocations.push_back
    (std::unique_ptr<macro_invocation>
	{new macro_invocation{m, invocation_range}});
  return *_macro_invocations.back();
}

void pp_result::
_extend_macro_invocation_range(macro_invocation &invocation,
			       const raw_pp_token_index new_end)
{
  assert(new_end >= invocation._invocation_range.end);
  invocation._invocation_range.end = new_end;
}

const macro& pp_result::
_add_macro(const std::string &name,
	   const bool func_like,
	   const bool variadic,
	   std::vector<std::string> &&arg_names,
	   raw_pp_tokens &&repl,
	   const raw_pp_tokens_range &directive_range,
	   const macro_undef *prev_macro_undef)
{
  _macros.push_back(std::unique_ptr<const macro>{
			new macro{name, func_like, variadic,
				  std::move(arg_names), std::move(repl),
				  directive_range, prev_macro_undef}});
  return *_macros.back();
}

const macro_undef& pp_result::
_add_macro_undef(const macro &m, const raw_pp_tokens_range &directive_range)
{
  _macro_undefs.push_back(std::unique_ptr<const macro_undef>{
				new macro_undef{m, directive_range}});
  return *_macro_undefs.back();
}

void pp_result::_drop_pp_tokens_tail(const pp_tokens::size_type new_end)
{
  if (new_end == _pp_tokens.size())
    return;
  assert(new_end < _pp_tokens.size());

  // The preprocessor temporarily pushed some pp_token instances when
  // doing macro expansion for #include directives or evaluating
  // conditional #if expressions. Purge these alongside with any
  // associated macro_invocations.
  const raw_pp_token_index tail_begin_raw =
    _pp_tokens[new_end].get_token_source().begin;
  auto it_macro_invocations_begin
    = std::lower_bound(_macro_invocations.begin(),
		       _macro_invocations.end(),
		       tail_begin_raw,
		       [](const std::unique_ptr<macro_invocation> &i,
			  const raw_pp_token_index e)
		       { return i->get_source_range().end <= e; });
  assert(it_macro_invocations_begin == _macro_invocations.end() ||
	 (it_macro_invocations_begin->get()->get_source_range().begin >=
	  tail_begin_raw));
  _macro_invocations.erase(it_macro_invocations_begin,
			   _macro_invocations.end());

  _pp_tokens.shrink(new_end);
}
