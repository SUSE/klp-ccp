#include "header_inclusion_tree.hh"

using namespace suse::cp;

header_inclusion_node::header_inclusion_node(header_inclusion_node *parent,
					     const std::string &filename)
  : _parent(parent), _filename(filename)
{}

header_inclusion_node::~header_inclusion_node() noexcept = default;

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


header_inclusion_root::header_inclusion_root(const std::string &filename,
					     const bool is_preinclude)
  : header_inclusion_node(nullptr, filename),
    _is_preinclude(is_preinclude)
{}

header_inclusion_root::~header_inclusion_root() noexcept = default;


header_inclusion_child::header_inclusion_child(
				header_inclusion_node &parent,
				const std::string &filename,
				file_range &&file_range,
				used_macros &&used_macros,
				used_macro_undefs &&used_macro_undefs)
  : header_inclusion_node(&parent, filename),
    _file_range(std::move(file_range)),
    _used_macros(std::move(used_macros)),
    _used_macro_undefs(std::move(used_macro_undefs))
{}

header_inclusion_child::~header_inclusion_child() noexcept = default;
