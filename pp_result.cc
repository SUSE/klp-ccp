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

pp_result::inclusion_node::inclusion_node(const inclusion_node * const parent)
  : _parent(parent)
{}

pp_result::inclusion_node::~inclusion_node() noexcept = default;

pp_result::header_inclusion_child& pp_result::inclusion_node::
add_header_inclusion(const std::string &filename,
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
  _children.push_back(std::move(new_child));
  return _new_child;
}

pp_result::conditional_inclusion_node& pp_result::inclusion_node::
add_conditional_inclusion(const raw_pp_token_index range_begin,
			  used_macros &&used_macros,
			  used_macro_undefs &&used_macro_undefs)
{
  std::unique_ptr<conditional_inclusion_node> new_child
     (new conditional_inclusion_node(*this, range_begin,
				     std::move(used_macros),
				     std::move(used_macro_undefs)));
  conditional_inclusion_node &_new_child = *new_child;
  _children.push_back(std::move(new_child));
  return _new_child;
}


pp_result::header_inclusion_node::
header_inclusion_node(const std::string &filename)
  : _filename(filename)
{}

pp_result::header_inclusion_node::
header_inclusion_node(const inclusion_node &parent, const std::string &filename)
  : inclusion_node(&parent), _filename(filename)
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
header_inclusion_child(const inclusion_node &parent,
		       const std::string &filename,
		       const raw_pp_tokens_range &directive_range,
		       used_macros &&used_macros,
		       used_macro_undefs &&used_macro_undefs)
  : header_inclusion_node(parent, filename),
    _directive_range(directive_range),
    _used_macros(std::move(used_macros)),
    _used_macro_undefs(std::move(used_macro_undefs))
{}

pp_result::header_inclusion_child::~header_inclusion_child() noexcept = default;


pp_result::conditional_inclusion_node::
conditional_inclusion_node(const inclusion_node &parent,
			   const raw_pp_token_index range_begin,
			   used_macros &&used_macros,
			   used_macro_undefs &&used_macro_undefs)
  : inclusion_node(&parent), _range(range_begin),
    _used_macros(std::move(used_macros)),
    _used_macro_undefs(std::move(used_macro_undefs))
{}

pp_result::conditional_inclusion_node::~conditional_inclusion_node() = default;

const pp_result::header_inclusion_node&
pp_result::conditional_inclusion_node::get_containing_header() const noexcept
{
  return _parent->get_containing_header();
}

void pp_result::conditional_inclusion_node::
set_range_end(const raw_pp_token_index range_end) noexcept
{
  _range.end = range_end;
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
