#include "inclusion_tree.hh"
#include "source_reader.hh"

using namespace suse::cp;

inclusion_node::inclusion_node()
  : _parent(nullptr)
{}

inclusion_node::inclusion_node(const inclusion_node * const parent)
  : _parent(parent)
{}

inclusion_node::~inclusion_node() noexcept = default;

header_inclusion_child&
inclusion_node::add_header_inclusion(const std::string &filename,
				     file_range &&file_range,
				     used_macros &&used_macros,
				     used_macro_undefs &&used_macro_undefs)
{
  std::unique_ptr<header_inclusion_child> new_child
     (new header_inclusion_child(*this, filename,
				 std::move(file_range),
				 std::move(used_macros),
				 std::move(used_macro_undefs)));
  header_inclusion_child &_new_child = *new_child;
  _children.push_back(std::move(new_child));
  return _new_child;
}


header_inclusion_node::header_inclusion_node(const std::string &filename)
  : _filename(filename)
{}

header_inclusion_node::header_inclusion_node(const inclusion_node &parent,
					     const std::string &filename)
  : inclusion_node(&parent), _filename(filename)
{}

header_inclusion_node::~header_inclusion_node() noexcept = default;

const header_inclusion_node&
header_inclusion_node::get_containing_header() const noexcept
{
  return *this;
}

void header_inclusion_node::add_line(const std::streamoff length)
{
  _offset_to_line_col_map.add_line(length);
}

std::pair<std::streamoff, std::streamoff>
header_inclusion_node::offset_to_line_col(const std::streamoff off)
  const noexcept
{
  return _offset_to_line_col_map.offset_to_line_col(off);
}

std::unique_ptr<source_reader> header_inclusion_node::
create_source_reader() const
{
  return std::unique_ptr<source_reader>(new file_source_reader(_filename));
}


header_inclusion_root::header_inclusion_root(const std::string &filename,
					     const bool is_preinclude)
  : header_inclusion_node(filename), _is_preinclude(is_preinclude)
{}

header_inclusion_root::~header_inclusion_root() noexcept = default;


header_inclusion_child::header_inclusion_child(
				const inclusion_node &parent,
				const std::string &filename,
				file_range &&file_range,
				used_macros &&used_macros,
				used_macro_undefs &&used_macro_undefs)
  : header_inclusion_node(parent, filename),
    _file_range(std::move(file_range)),
    _used_macros(std::move(used_macros)),
    _used_macro_undefs(std::move(used_macro_undefs))
{}

header_inclusion_child::~header_inclusion_child() noexcept = default;
