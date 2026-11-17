/*
 * Copyright (C) 2026  SUSE Software Solutions Germany GmbH
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

#ifndef RAW_PP_DIRECTIVE_HH
#define RAW_PP_DIRECTIVE_HH

#include <cstddef>
#include <vector>
#include "pp_result.hh"

// Examination of raw, i.e. unpreprocessed, source files for
// preprocessor directives, without doing any actual preprocessing:
// no header inclusion, no macro expansion, no evaluation of
// conditions.
namespace klp
{
  namespace ccp
  {
    class code_remarks;

    // The kind of preprocessor directive found on a logical line, if
    // any.
    enum class raw_pp_directive_kind : unsigned char
    {
      // Not a directive.
      none,
      // The null directive, i.e. a lone '#'.
      null,
      if_,
      ifdef,
      ifndef,
      elif,
      elifdef,
      elifndef,
      else_,
      endif,
      error,
      // Any other directive, e.g. #define or #include, or one not
      // known at all.
      other,
    };

    // Whether the directive opens a conditional inclusion, i.e. is an
    // #if, #ifdef or #ifndef.
    bool raw_pp_directive_opens_cond_incl(const raw_pp_directive_kind k)
      noexcept;

    // Whether the directive starts another branch of a conditional
    // inclusion, i.e. is an #elif, #elifdef, #elifndef or #else.
    bool raw_pp_directive_continues_cond_incl(const raw_pp_directive_kind k)
      noexcept;

    // A logical line of a source file: a maximal range of physical
    // lines joined by backslash line continuations or by /* ... */
    // comments spanning multiple lines. Physical lines are numbered
    // from zero.
    struct raw_pp_logical_line
    {
      // The range [first_line, end_line) of physical lines covered.
      std::size_t first_line;
      std::size_t end_line;
      raw_pp_directive_kind directive_kind;
    };

    // Tokenizes the file, splits it into logical lines and identifies
    // the preprocessor directives on these. Every physical line is
    // covered by exactly one of the returned logical lines, which are
    // in order. Warnings from the tokenizer get added to remarks.
    // Throws pp_except on fatal tokenization errors, e.g. for
    // unterminated comments, after adding the error to remarks.
    std::vector<raw_pp_logical_line>
    scan_raw_pp_logical_lines(pp_result::header_inclusion_node &file,
			      code_remarks &remarks);
  }
}

#endif
