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

#ifndef PP_TOKENIZER_HH
#define PP_TOKENIZER_HH

#include <string>
#include <vector>
#include "code_remarks.hh"
#include "raw_pp_token.hh"
#include "pp_result.hh"

namespace klp
{
  namespace ccp
  {
    class pp_except;
    class source_reader;

    namespace impl
    {
      class _pp_tokenizer
      {
      protected:
	typedef std::vector<char> buffer_type;

	_pp_tokenizer();

	virtual ~_pp_tokenizer() noexcept = 0;

	void init_state();

	raw_pp_token read_next_token();

      private:
	virtual buffer_type read_raw() = 0;

	virtual void add_line(const std::streamoff length) = 0;

	virtual void report_warning(const std::string &msg,
				    const range_in_file::loc_type loc) = 0;
	virtual pp_except report_fatal(const std::string &msg,
				       const range_in_file::loc_type loc) = 0;

	void _refill_buf();
	void _refill_queue_half(const bool odd_half);
	void _advance_queue_to_pos(const unsigned int new_queue_pos);
	unsigned int _cur_queue_window_end_pos_wrap() const noexcept;
	unsigned int _cur_queue_window_end_pos_nowrap() const noexcept;
	unsigned int
	_cur_queue_window_end_pos_nowrap(unsigned int search_begin_pos)
	  const noexcept;

	unsigned int _next_queued_non_continuation_pos(unsigned int pos)
	  const noexcept;
	bool _at_eof() const noexcept;

	raw_pp_token _tokenize_string(const range_in_file::loc_type begin_loc,
				      const char delim,
				      const bool delim_escapable,
				      const pp_token::type tok_type);
	raw_pp_token _tokenize_punctuator();
	raw_pp_token _tokenize_pp_number();
	raw_pp_token _tokenize_id();
	std::size_t _skip_c_comment(const std::size_t n_trailing_spaces);
	void _skip_cpp_comment();
	raw_pp_token _tokenize_ws();

	buffer_type _buf;
	buffer_type::const_iterator _buf_it;

	range_in_file::loc_type _line_length;
	enum class expect_qh_str
	{
	  no,
	  newline,
	  sharp_seen,
	  expect_qh_str,
	};
	expect_qh_str _expect_qh_str;

	char _pending[2];
	// Only five look-ahead characters are needed (three "real"
	// characters, interspersed with up to two continuation line
	// markers (encoded as zero) inbetween, but a power of two
	// enables more efficient ring buffer index wrapround.
	// Make the queue at least twice as large as the next power of
	// two, to enable batched refill whenever crossing into the
	// other half.
	// Two (or more) consecutive zeroes encode EOF.
	static constexpr unsigned int _queue_capacity_log2 = 5;
	static constexpr unsigned int _queue_capacity =
	  1u << _queue_capacity_log2;
	char _queue[_queue_capacity];
	range_in_file::loc_type _queue_locs[_queue_capacity];
	range_in_file::loc_type _next_queue_loc;
	unsigned int _queue_pos;
      };
    }

    class pp_tokenizer final : public impl::_pp_tokenizer
    {
    public:
      pp_tokenizer(pp_result::header_inclusion_node &file);

      virtual ~pp_tokenizer() noexcept override;

      code_remarks& get_remarks() noexcept
      {
	return _remarks;
      }

      raw_pp_token read_next_token();

      const pp_result::header_inclusion_node& get_header_inclusion_node()
	const noexcept
      { return _file; }

    private:
      virtual buffer_type read_raw() override;

      virtual void add_line(const std::streamoff length) override;

      virtual void report_warning(const std::string &msg,
				  const range_in_file::loc_type loc) override;
      virtual pp_except report_fatal(const std::string &msg,
				     const range_in_file::loc_type loc)
	override;

      pp_result::header_inclusion_node &_file;
      std::unique_ptr<source_reader> _sr;
      code_remarks _remarks;
    };


    class pp_string_tokenizer final : public impl::_pp_tokenizer
    {
    public:
      typedef std::function<void(const std::string&)> report_warning_type;
      typedef std::function<pp_except(const std::string&)> report_fatal_type;

      pp_string_tokenizer(const std::string &s,
			  const report_warning_type &report_warning,
			  const report_fatal_type &report_fatal);

      virtual ~pp_string_tokenizer() noexcept override;

      raw_pp_token read_next_token();

      static raw_pp_tokens tokenize_builtin(const std::string &s);

    private:
      virtual buffer_type read_raw() override;

      virtual void add_line(const std::streamoff) override;

      virtual void report_warning(const std::string &msg,
				  const range_in_file::loc_type) override;
      virtual pp_except report_fatal(const std::string &msg,
				     const range_in_file::loc_type)
	override;

      buffer_type _buf;
      report_warning_type _report_warning;
      report_fatal_type _report_fatal;
    };
  }
}

#endif
