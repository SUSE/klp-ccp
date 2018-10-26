#ifndef INCLUSION_TREE_HH
#define INCLUSION_TREE_HH

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

    class header_inclusion_node;

    class inclusion_node
    {
    public:
      inclusion_node(const inclusion_node&) = delete;
      inclusion_node(inclusion_node&&) = delete;

      virtual ~inclusion_node() noexcept;

      virtual const header_inclusion_node&
      get_containing_header() const noexcept = 0;

      header_inclusion_child&
      add_header_inclusion(const std::string &filename,
			   file_range &&file_range,
			   used_macros &&used_macros,
			   used_macro_undefs &&used_macro_undefs);

    protected:
      inclusion_node();
      inclusion_node(const inclusion_node * const parent);

    private:
      const inclusion_node * const _parent;
      std::vector<std::unique_ptr<inclusion_node> > _children;
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

      void add_line(const file_range::loc_type length);

      std::pair<file_range::loc_type, file_range::loc_type>
      offset_to_line_col(const file_range::loc_type off) const noexcept;

      std::unique_ptr<source_reader> create_source_reader() const;

    protected:
      header_inclusion_node(const std::string &filename);

      header_inclusion_node(const inclusion_node &parent,
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
			     file_range &&file_range,
			     used_macros &&used_macros,
			     used_macro_undefs &&used_macro_undefs);

      file_range _file_range;
      used_macros _used_macros;
      used_macro_undefs _used_macro_undefs;
    };
  }
}

#endif
