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

#ifndef _DEPREPROCESSOR_HH
#define _DEPREPROCESSOR_HH

#include <string>
#include <map>
#include "pp_result.hh"

namespace klp
{
  namespace ccp
  {
    class source_reader;
    class source_writer;
    class output_remarks;

    class depreprocessor
    {
    private:
      class _macro_define_to_emit;

      struct _macro_undef_to_emit
      {
	_macro_undef_to_emit(const pp_result::macro_undef &_original) noexcept;
	_macro_undef_to_emit(const std::string &_name);

	bool operator<(const _macro_undef_to_emit& rhs) const noexcept;
	bool operator<(const _macro_define_to_emit& rhs) const noexcept;

	const pp_result::macro_undef *original;
	std::string name;
      };

      struct _macro_define_to_emit
      {
	_macro_define_to_emit(const pp_result::macro &_m) noexcept;

	bool operator<(const _macro_define_to_emit& rhs) const noexcept;
	bool operator<(const _macro_undef_to_emit& rhs) const noexcept;

	std::reference_wrapper<const pp_result::macro> m;
      };

      class _source_reader_cache
      {
      public:
	source_reader& get(const pp_result::header_inclusion_node &h);

      private:
	std::vector<std::unique_ptr<source_reader>> _cache;
      };

    public:
      class transformed_input_chunk
      {
      public:
	transformed_input_chunk(const pp_tokens_range &bounding_r);

	void copy_subrange(const pp_tokens_range &r,
			   const bool need_whitespace_before,
			   const pp_tokens &toks);
	void purge_subrange(const pp_tokens_range &r,
			    const bool need_whitespace_before);
	void replace_token(const pp_token_index &index, pp_token &&repl_tok,
			   const bool add_pointer_deref);
	void insert_token(const pp_token_index &pos, pp_token &&new_tok,
			  const bool need_whitespace_before,
			  const bool need_whitespace_after);

	transformed_input_chunk split_head_off(const pp_tokens_range &r);

      private:
	friend class depreprocessor;

	struct _op
	{
	public:
	  enum class sticky_side
	  {
	    none,
	    left,
	    right,
	  };

	  _op(const pp_tokens_range &copied_range);
	  _op(const pp_tokens_range &replaced_range,
	      pp_token &&repl_tok, const bool _add_pointer_deref);
	  _op(const pp_token_index pos, pp_token &&_new_tok,
	      const sticky_side _stickiness);
	  _op(const pp_token_index pos, const sticky_side _stickiness);
	  _op(const pp_result::conditional_inclusion_node &_c,
	      const _cond_incl_transition_kind k);

	  bool operator<(const pp_tokens_range &rhs) const noexcept;
	  bool operator>(const pp_tokens_range &rhs) const noexcept;

	  raw_pp_tokens_range get_range_raw(const pp_result &pp_result)
	    const noexcept;

	  enum class action
	  {
	    copy,
	    replace,
	    insert,
	    insert_ws,
	  };

	  action a;
	  pp_tokens_range r;
	  sticky_side stickiness;

	  pp_token new_tok;
	  bool add_pointer_deref;
	};

	typedef std::vector<_op> _ops_type;

	struct _pos_in_chunk
	{
	  _pos_in_chunk(const _ops_type::size_type _op,
			const pp_token_index _tok) noexcept;

	  bool operator==(const _pos_in_chunk &rhs) const noexcept;

	  bool operator!=(const _pos_in_chunk &rhs) const noexcept
	  { return !(*this == rhs); }

	  bool operator<(const _pos_in_chunk &rhs) const noexcept;

	  bool operator<=(const _pos_in_chunk &rhs) const noexcept
	  { return *this < rhs || *this == rhs; }

	  _ops_type::size_type op;
	  pp_token_index tok;
	};

	struct _used_macro_in_chunk
	{
	  _used_macro_in_chunk(const _pos_in_chunk &_pos,
			       const pp_result::macro &_m) noexcept;

	  _pos_in_chunk pos;
	  std::reference_wrapper<const pp_result::macro> m;
	};

	typedef std::vector<_used_macro_in_chunk>
	  _used_macros_in_chunk_type;

	struct _macro_nondef_constraint_in_chunk
	{
	  _macro_nondef_constraint_in_chunk
		(const _pos_in_chunk &_pos,
		 pp_result::macro_nondef_constraint &&_mnc);

	  _pos_in_chunk pos;
	  pp_result::macro_nondef_constraint mnc;
	};

	typedef std::vector<_macro_nondef_constraint_in_chunk>
	  _macro_nondef_constraints_in_chunk_type;

	struct _macro_undef_to_emit_in_chunk
	{
	  _macro_undef_to_emit_in_chunk(const _pos_in_chunk &_pos,
					_macro_undef_to_emit &&_mu);

	  bool operator<(const _macro_undef_to_emit_in_chunk &rhs)
	    const noexcept;

	  _pos_in_chunk pos;
	  _macro_undef_to_emit mu;
	};

	typedef std::vector<_macro_undef_to_emit_in_chunk>
	  _macro_undefs_to_emit_type;

	struct _macro_define_to_emit_in_chunk
	{
	  _macro_define_to_emit_in_chunk(const _pos_in_chunk &_pos,
					 _macro_define_to_emit &&_m);

	  bool operator<(const _macro_define_to_emit_in_chunk &rhs)
	    const noexcept;

	  _pos_in_chunk pos;
	  _macro_define_to_emit m;
	};

	typedef std::vector<_macro_define_to_emit_in_chunk>
	  _macro_defines_to_emit_type;

	_ops_type::iterator _prepare_insert(const pp_tokens_range &r);

	pp_tokens_range _get_range() const noexcept;

	raw_pp_tokens_range _get_range_raw(const pp_result &pp_result)
	  const noexcept;

	void _trim();

	_pos_in_chunk _begin_pos_in_chunk() const noexcept;
	_pos_in_chunk _end_pos_in_chunk() const noexcept;
	_pos_in_chunk _next_pos_in_chunk(const _pos_in_chunk &cur_pos)
	  const noexcept;
	pp_token_index _pos_in_chunk_to_token_index(const _pos_in_chunk &pos)
	  const noexcept;

	std::pair<_ops_type::const_iterator, _ops_type::const_iterator>
	_find_overlapping_ops_range(const raw_pp_tokens_range &r,
				    const pp_result &pp_result) const noexcept;

	_pos_in_chunk _directive_range_to_pos_in_chunk
			(const raw_pp_tokens_range &directive_range,
			 const pp_result &pp_result)
	  const noexcept;

	bool _find_macro_constraints(const pp_result &pp_result,
				     bool next_raw_tok_is_opening_parenthesis);

	void _add_macro_undef_to_emit(const _pos_in_chunk &pos,
				      _macro_undef_to_emit &&mu);

	void _add_macro_define_to_emit(const _pos_in_chunk &pos,
				       _macro_define_to_emit &&m);

	void _write(source_writer &writer, const pp_result &pp_result,
		    _source_reader_cache &source_reader_cache,
		    output_remarks &remarks) const;

	pp_tokens_range _bounding_r;
	_ops_type _ops;

	_used_macros_in_chunk_type _used_macros_in_chunk;
	_macro_nondef_constraints_in_chunk_type
	  _macro_nondef_constraints_in_chunk;
	std::vector<pp_tokens_range> _ranges_to_emit_expanded;
	_macro_undefs_to_emit_type _macro_undefs_to_emit;
	_macro_defines_to_emit_type _macro_defines_to_emit;
      };

      depreprocessor(const pp_result &pp_result,
		     output_remarks &remarks);

      void append(transformed_input_chunk &&tic);
      void append(const pp_result::header_inclusion_child &include);
      void append(const pp_result::header_inclusion_root &hir);

      void operator()(const std::string &outfile);

    private:
      class _header_inclusion_chunk
      {
      private:
	typedef std::vector<std::reference_wrapper<const pp_result::macro>>
	  _new_macro_defines_type;

      public:
	class const_new_macro_define_iterator :
	  public std::iterator<std::forward_iterator_tag,
			       const pp_result::macro>
	{
	public:
	  bool operator==(const const_new_macro_define_iterator &rhs)
	    const noexcept
	  { return this->_it == rhs._it; }

	  bool operator!=(const const_new_macro_define_iterator &rhs)
	    const noexcept
	  {
	    return !(*this == rhs);
	  }

	  reference operator*() const noexcept
	  { return _it->get(); }

	  pointer operator->() const noexcept
	  { return &_it->get(); }

	  const_new_macro_define_iterator& operator++() noexcept
	  { ++_it; return *this; }

	  const const_new_macro_define_iterator operator++(int) noexcept
	  { return const_new_macro_define_iterator{_it++}; }

	private:
	  friend class _header_inclusion_chunk;

	  const_new_macro_define_iterator
		(const _new_macro_defines_type::const_iterator &it) noexcept
	    : _it(it)
	  {}

	  _new_macro_defines_type::const_iterator _it;
	};

	_header_inclusion_chunk
			(const pp_result::header_inclusion_child &child_node);
	_header_inclusion_chunk
			(const pp_result::header_inclusion_root &root_node);

	const pp_result::header_inclusion_node& get_inclusion_node()
	  const noexcept;

	void find_macro_constraints(const pp_result &pp_result);

	bool needs_undef_before_include(const pp_result::macro &m)
	  const noexcept;

	bool is_unmodified_by_include(const pp_result::macro &m) const noexcept;

	const_new_macro_define_iterator new_macro_defines_begin() const noexcept
	{ return const_new_macro_define_iterator{_new_macro_defines.begin()}; }

	const_new_macro_define_iterator new_macro_defines_end() const noexcept
	{ return const_new_macro_define_iterator{_new_macro_defines.end()}; }

	const pp_result::used_macros& get_used_macros() const noexcept
	{ return _used_macros; }

	void add_macro_undef_to_emit(_macro_undef_to_emit &&mu);

	void add_macro_define_to_emit(_macro_define_to_emit &&m);

	bool has_macro_undefs_or_defines_to_emit() const noexcept;

	void write(source_writer &writer, const pp_result &pp_result,
		   _source_reader_cache &source_reader_cache,
		   output_remarks &remarks) const;

      private:
	enum class _kind
	{
	  k_child,
	  k_root,
	};

	_kind _k;

	union
	{
	  const pp_result::header_inclusion_child *_child_node;
	  const pp_result::header_inclusion_root *_root_node;
	};

	pp_result::macro_nondef_constraints _macro_nondef_constraints;
	std::map<std::string, std::reference_wrapper<const pp_result::macro>>
	  _macro_defines_wo_prior_undef;
	std::set<std::string> _macro_undefs;
	_new_macro_defines_type _new_macro_defines;
	pp_result::used_macros _used_macros;
	std::vector<_macro_undef_to_emit> _macro_undefs_to_emit;
	std::vector<_macro_define_to_emit> _macro_defines_to_emit;
      };

      struct _chunk
      {
	enum class kind
	{
	  transformed_input,
	  header_inclusion,
	  _dead,
	};

	_chunk(transformed_input_chunk &&_tic, const pp_result &pp_result);
	_chunk(_header_inclusion_chunk &&_hic);
	_chunk(_chunk &&c);
	~_chunk() noexcept;

	_chunk& operator=(_chunk &&rhs);

	kind k;

	union
	{
	  transformed_input_chunk tic;
	  _header_inclusion_chunk hic;
	};

	raw_pp_tokens_range range_raw;
      };

      typedef std::vector<_chunk> _chunks_type;

      struct _in_order_chunk
      {
	_in_order_chunk(const _chunks_type::iterator _it_chunk) noexcept;

	_chunks_type::iterator it_chunk;
	raw_pp_tokens_range attributed_range_raw;
      };

      typedef std::vector<_in_order_chunk> _in_order_chunks_type;

      struct _pos_in_output
      {
	_pos_in_output
		(const _chunks_type::iterator &_chunk,
		 const transformed_input_chunk::_pos_in_chunk &_pos_in_chunk)
	  noexcept;

	_pos_in_output(const _chunks_type::iterator &_chunk) noexcept;

	bool operator==(const _pos_in_output &rhs) const noexcept;

	bool operator!=(const _pos_in_output &rhs) const noexcept
	{ return !(*this == rhs); }

	bool operator<(const _pos_in_output &rhs) const noexcept;

	bool operator<=(const _pos_in_output &rhs) const noexcept
	{ return *this < rhs || *this == rhs; }

	_chunks_type::iterator chunk;
	transformed_input_chunk::_pos_in_chunk pos_in_chunk;
      };

      struct _macro_state
      {
	_macro_state(const pp_result::macro &_m,
		     const _pos_in_output &_last_usage) noexcept;

	std::reference_wrapper<const pp_result::macro> m;
	_pos_in_output last_usage;
      };

      typedef std::map<std::string, _macro_state> _macro_states_type;

    private:
      friend class transformed_input_chunk;
      friend class _header_inclusion_chunk;

      void _try_merge_chunks_pre();
      void _compute_needed_macro_defs_and_undefs();
      void _compute_needed_macro_defs_and_undefs_tic
		(const _chunks_type::iterator &it_chunk,
		 const _in_order_chunks_type &in_order_chunks,
		 const _chunks_type::iterator &cur_run_begin,
		 _macro_states_type &macro_states);
      void _compute_needed_macro_defs_and_undefs_hic
		(const _chunks_type::iterator &it_chunk,
		 const _in_order_chunks_type &in_order_chunks,
		 const _chunks_type::iterator &cur_run_begin,
		 _macro_states_type &macro_states);
      void _emit_undef(const _macro_state &macro_state,
		       const _pos_in_output &mnc_pos,
		       const _in_order_chunks_type &in_order_chunks,
		       const _chunks_type::iterator &cur_run_begin);
      void _emit_define(const pp_result::macro &m,
			const _pos_in_output &um_pos,
			const _in_order_chunks_type &in_order_chunks,
			const _chunks_type::iterator &cur_run_begin,
			const _macro_state * const conflicting_macro_state);
      bool _can_place_define_at
		(const pp_result::macro &m,
		 const _pos_in_output &placement_pos,
		 const _pos_in_output &um_pos,
		 const _macro_state * const conflicting_macro_state) noexcept;
      bool _can_place_undef_at(const std::string &name,
			       const _pos_in_output &placement_pos,
			       const _pos_in_output &mnc_pos,
			       const _pos_in_output &last_macro_usage) noexcept;

      _pos_in_output _find_macro_undef_in_order_pos
				(const pp_result::macro_undef &mu,
				 const _in_order_chunks_type &in_order_chunks)
	noexcept;

      _pos_in_output _find_macro_define_in_order_pos
				(const pp_result::macro &m,
				 const _in_order_chunks_type &in_order_chunks)
	noexcept;

      _pos_in_output
      _find_directive_in_order_pos(const raw_pp_tokens_range &directive_range,
				   const _in_order_chunks_type &in_order_chunks)
	noexcept;

      _pos_in_output
      _find_macro_undef_in_run_pos(const pp_result::macro_undef &mu,
				    const _chunks_type::iterator &run_begin,
				    const _chunks_type::iterator &run_end)
	noexcept;

      _pos_in_output
      _find_macro_define_in_run_pos(const pp_result::macro &m,
				    const _chunks_type::iterator &run_begin,
				    const _chunks_type::iterator &run_end)
	noexcept;

      _pos_in_output
      _find_directive_in_run_pos(const raw_pp_tokens_range &directive_range,
				 const _chunks_type::iterator &run_begin,
				 const _chunks_type::iterator &run_end)
	noexcept;

      _pos_in_output
      _chunk_pos_in_output(const _chunks_type::iterator &it_chunk)
	noexcept;
      _pos_in_output _end_pos_in_output() noexcept;

      static void _write_directive(source_writer &writer,
				   const raw_pp_tokens_range &directive_range,
				   const pp_result &pp_result,
				   _source_reader_cache &source_reader_cache);
      static void _write_define(source_writer &writer,
				const pp_result::macro &m,
				const pp_result &pp_result,
				_source_reader_cache &source_reader_cache,
				output_remarks &remarks);
      static void _write_undef(source_writer &writer,
			       const _macro_undef_to_emit &mu,
			       const pp_result &pp_result,
			       _source_reader_cache &source_reader_cache);

      const pp_result &_pp_result;
      _chunks_type _chunks;
      output_remarks &_remarks;
    };
  }
}

#endif
