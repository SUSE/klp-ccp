#ifndef HEADER_INCLUSION_TREE_HH
#define HEADER_INCLUSION_TREE_HH

#include <vector>
#include <memory>
#include <utility>
#include "pp_tokens.hh"
#include "offset_to_line_col_map.hh"
#include "used_macros.hh"
#include "used_macro_undefs.hh"

namespace suse
{
  namespace cp
  {
    class source_reader;

    class header_inclusion_child;

    class header_inclusion_node
    {
    public:
      header_inclusion_node(const header_inclusion_node&) = delete;
      header_inclusion_node(header_inclusion_node&&) = delete;

      virtual ~header_inclusion_node() noexcept;

      template<typename... Targs>
      header_inclusion_child& add_child(Targs&&... args);

      const std::string& get_filename() const noexcept
      {
	return _filename;
      }

      header_inclusion_node*
      get_parent() noexcept
      { return _parent; }

      void add_line(const std::streamoff length);

      std::pair<std::streamoff, std::streamoff>
      offset_to_line_col(const std::streamoff off) const noexcept;

      std::unique_ptr<source_reader> create_source_reader() const;

    protected:
      header_inclusion_node(header_inclusion_node *parent,
			    const std::string &filename);

    private:
      header_inclusion_node *_parent;
      std::vector<std::unique_ptr<header_inclusion_child> > _children;

      const std::string _filename;

      offset_to_line_col_map _offset_to_line_col_map;
    };


    class header_inclusion_root : public header_inclusion_node
    {
    public:
      header_inclusion_root(const std::string &filename,
			    const bool is_preinclude);
      virtual ~header_inclusion_root() noexcept;

    private:
      bool _is_preinclude;
    };


    class header_inclusion_child : public header_inclusion_node
    {
    public:
      virtual ~header_inclusion_child() noexcept;

    private:
      friend class header_inclusion_node;

      header_inclusion_child(header_inclusion_node &parent,
			     const std::string &filename,
			     file_range &&file_range,
			     used_macros &&used_macros,
			     used_macro_undefs &&used_macro_undefs);

      file_range _file_range;
      used_macros _used_macros;
      used_macro_undefs _used_macro_undefs;
    };


    template<typename... Targs>
    header_inclusion_child&
    header_inclusion_node::add_child(Targs&&... args)
    {
      _children.push_back(
	std::unique_ptr<header_inclusion_child>(
	   new header_inclusion_child(*this, std::forward<Targs>(args)...)));
      return *_children.back();
    }
  }
}

#endif
