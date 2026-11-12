/*
 * Copyright (C) 2024, 2026  SUSE Software Solutions Germany GmbH
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

#ifndef _MACRO_REWRITING_HH
#define _MACRO_REWRITING_HH

#include "raw_pp_tokens_range.hh"
#include "pp_result.hh"
#include "pp_token.hh"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include <limits>

namespace klp
{
  namespace ccp
  {
    class source_writer;
    class source_reader_cache;
    class rescan_macro_invocation_rewriter;

    enum class sticky_side
    {
      none,
      left,
      right,
    };

    // Whether two adjacent tokens need whitespace inbetween in order
    // not to get glued together into a different token sequence when
    // tokenized again.
    bool tokens_need_separation(const pp_token::type prev_type,
				const std::string &prev_value,
				const pp_token::type next_type,
				const std::string &next_value) noexcept;

    // Index into a rewritten_macro_definitions_pool.
    using rewritten_macro_definition_id = std::size_t;

    constexpr rewritten_macro_definition_id
    invalid_rewritten_macro_definition_id =
      std::numeric_limits<rewritten_macro_definition_id>::max();

    // A literal token to be emitted into either a rewritten macro
    // definition's replacement list or into a rewritten macro
    // invocation's source. If rename_ref is valid, the token is a
    // macro identifier which is to be replaced by the name assigned to
    // the referenced rewritten macro definition. If is_pp_operator is
    // set, the token is a '##' operator synthesized by the rewriter
    // for pasting a prefix or suffix onto a parameter's argument, as
    // opposed to a literal '##' punctuator from some expansion output
    // (which cannot be represented in a replacement list).
    struct rewritten_macro_token
    {
      struct pp_operator_tag {};

      rewritten_macro_token(const pp_token &_tok);
      rewritten_macro_token(const pp_token &_tok,
			    const rewritten_macro_definition_id _rename_ref);
      rewritten_macro_token(const pp_token &_tok, const pp_operator_tag&);

      bool operator==(const rewritten_macro_token &rhs) const noexcept;

      bool operator!=(const rewritten_macro_token &rhs) const noexcept
      { return !(*this == rhs); }

      pp_token tok;
      rewritten_macro_definition_id rename_ref;
      bool is_pp_operator;
    };

    class rewritten_macro_definition
    {
    private:
      struct _op
      {
	enum class action
	{
	  copy,
	  replace,
	  insert,
	  insert_ws,
	};

	bool operator==(const _op &rhs) const noexcept;

	action a;
	raw_pp_tokens_range r;
	sticky_side stickiness;
	std::vector<rewritten_macro_token> new_toks;
      };

      using _ops_container_type = std::vector<_op>;

      friend class rescan_macro_invocation_rewriter;

      // Collects the edits to some macro definition's replacement
      // list, in arbitrary order. All ranges/positions are indices
      // into pp_result::macro::get_repl(). finalize() turns the sparse
      // edits into a complete, normalized rewritten_macro_definition.
      class builder
      {
      public:
	builder(const pp_result::macro &orig_m) noexcept;

	bool empty() const noexcept
	{ return _ops.empty(); }

	void erase(const raw_pp_tokens_range &r);

	void replace(const raw_pp_tokens_range &r,
		     std::vector<rewritten_macro_token> &&new_toks);

	void insert(const raw_pp_token_index pos, const sticky_side stickiness,
		    std::vector<rewritten_macro_token> &&new_toks);

	void insert_ws(const raw_pp_token_index pos,
		       const sticky_side stickiness);

	rewritten_macro_definition finalize(const pp_result &pp_result) &&;

      private:
	const pp_result::macro &_orig_m;
	// Sparse edits: replace ops (with empty new_toks for erasures)
	// and insertion ops only.
	_ops_container_type _ops;
      };

    public:
      const pp_result::macro& get_orig_macro() const noexcept
      { return _orig_m; }

      // Parameter names, possibly renamed to avoid collisions with
      // identifiers inserted into the replacement list.
      const std::vector<std::string>& get_arg_names() const noexcept
      { return _arg_names; }

      bool operator==(const rewritten_macro_definition &rhs) const noexcept;

      bool operator!=(const rewritten_macro_definition &rhs) const noexcept
      { return !(*this == rhs); }

      // Whether the (normalized) rewritten definition is equivalent
      // to the original one.
      bool is_identity() const noexcept;

      // Create a definition forwarding to the original macro, i.e. one
      // whose replacement list consists of an invocation of orig_m
      // passing all parameters through. Used for renaming macro
      // invocations whose definition doesn't need any modification
      // otherwise.
      static rewritten_macro_definition
      make_forwarding(const pp_result::macro &orig_m,
		      const pp_result &pp_result);

      // Whether this definition has been created by make_forwarding()
      // and thus, invokes the original macro by its name.
      bool is_forwarding() const noexcept
      { return _forwarding; }

      void write_repl
	(source_writer &writer, const pp_result &pp_result,
	 source_reader_cache &source_reader_cache,
	 const std::function<const std::string&(rewritten_macro_definition_id)>
	   &resolve_name) const;

    private:
      friend class builder;

      void normalize_ws(const pp_result &pp_result);

      // Synthesized forwarding definitions don't have their
      // whitespace normalized.
      rewritten_macro_definition(const pp_result::macro &orig_m,
				 _ops_container_type &&ops,
				 std::vector<std::string> &&arg_names,
				 const pp_result &pp_result,
				 const bool forwarding = false);

      _ops_container_type _ops;

      const pp_result::macro &_orig_m;
      std::vector<std::string> _arg_names;
      bool _forwarding;
    };

    // Global pool of rewritten macro definitions. Structurally equal
    // definitions of the same original macro get merged.
    class rewritten_macro_definitions_pool
    {
    public:
      rewritten_macro_definitions_pool() = default;
      rewritten_macro_definitions_pool(const rewritten_macro_definitions_pool&)
	= delete;

      rewritten_macro_definition_id insert(rewritten_macro_definition &&d);

      std::size_t size() const noexcept
      { return _defs.size(); }

      // Drop all definitions inserted after the pool had the given
      // size. Used for rolling back a failed rewrite attempt.
      void truncate(const std::size_t size);

      const rewritten_macro_definition&
      operator[](const rewritten_macro_definition_id id) const noexcept;

      // Assign names to all definitions inserted so far. The callback
      // receives the original macro, the zero-based index among the
      // rewritten definitions of all original macros with that name
      // and whether or not there are more than one such. The indices
      // follow first_use_order, a list of definition ids ordered by
      // first use; definitions not listed come last in insertion
      // order.
      void assign_names
	(const std::vector<rewritten_macro_definition_id> &first_use_order,
	 const std::function<std::string(const pp_result::macro&,
					 std::size_t, bool)> &name_cb);

      const std::string& get_name(const rewritten_macro_definition_id id)
	const noexcept;

    private:
      std::vector<std::unique_ptr<rewritten_macro_definition>> _defs;
      std::vector<std::string> _names;
      std::map<const pp_result::macro*,
	       std::vector<rewritten_macro_definition_id>> _by_macro;
      bool _names_assigned = false;
    };

    // The desired edit to a macro invocation's preprocessed output, to
    // be propagated to the invocation's source and/or to rewritten
    // macro definitions. Positions are relative to the invocation's
    // output range.
    struct macro_invocation_output_edit_script_op
    {
      enum class kind
      {
	copy,
	replace,
	insert,
	insert_ws,
      };

      kind k;
      // copy: copied range; replace: range of the (single) replaced
      // token; insert/insert_ws: zero length range at the position.
      pp_tokens_range r;
      sticky_side stickiness;
      std::vector<pp_token> new_toks;
    };

    // The resulting edit to a macro invocation's source, as emitted
    // by rewrite_macro_invocation().
    struct macro_invocation_src_edit_script_op
    {
      enum class kind
      {
	copy,
	literal,
	ws,
      };

      kind k;
      // For copy: range relative to the invocation source's total
      // invocation range.
      pp_result::macro_invocation::expansion_forest::rescan_macro_invocation::
	output::tokens_range src_range;
      // For literal: the tokens to emit, with renamed macro identifiers
      // referring to their rewritten definition.
      std::vector<rewritten_macro_token> toks;
    };

    class rewritten_macro_invocation
    {
    public:
      struct used_macro
      {
	// Either an original macro definition (rewritten ==
	// invalid_rewritten_macro_definition_id) or a rewritten one.
	const pp_result::macro *m;
	rewritten_macro_definition_id rewritten;
      };

      const std::vector<macro_invocation_src_edit_script_op>&
      get_src_edit_script() const noexcept
      { return _src_edit_script; }

      const std::vector<used_macro>& get_used_macros() const noexcept
      { return _used_macros; }

      const std::vector<pp_result::macro_nondef_constraint>&
      get_macro_nondef_constraints() const noexcept
      { return _macro_nondef_constraints; }

      // Result construction interface.
      void add_src_edit_script_op(macro_invocation_src_edit_script_op &&op);
      void add_used_macro(const used_macro &um);
      void add_macro_nondef_constraint(pp_result::macro_nondef_constraint &&mnc);

    private:
      std::vector<macro_invocation_src_edit_script_op> _src_edit_script;
      std::vector<used_macro> _used_macros;
      std::vector<pp_result::macro_nondef_constraint> _macro_nondef_constraints;
    };

    // Rewrite a macro invocation such that the result of expanding
    // the rewritten invocation (and rewritten macro definitions)
    // matches the edited preprocessed output. The macro_ids map
    // provides all macro names known to the pp_result, mapping to
    // whether or not any function-like definition exists for it.
    // next_tok_is_lparen tells whether the first non-whitespace token
    // following the invocation in the output is an opening
    // parenthesis. All invocations of the macros listed in
    // forced_renames get renamed, those not requiring any other
    // modification to a forwarding definition
    // (rewritten_macro_definition::make_forwarding()).
    //
    // Returns false if the invocation cannot be represented in a
    // rewritten form, with a reason in error_msg.
    bool
    rewrite_macro_invocation
	(const pp_result &pp_result,
	 const pp_result::macro_invocation &mi,
	 const std::vector<macro_invocation_output_edit_script_op> &out_edits,
	 const bool next_tok_is_lparen,
	 const std::map<std::string, bool> &macro_ids,
	 const std::set<const pp_result::macro*> &forced_renames,
	 rewritten_macro_definitions_pool &pool,
	 rewritten_macro_invocation &result,
	 std::string &error_msg);
  }
}

#endif
