#ifndef PP_TOKENIZER_HH
#define PP_TOKENIZER_HH

#include <string>
#include "code_remark.hh"
#include "code_remarks.hh"
#include "pp_token.hh"
#include "source_reader.hh"

namespace klp
{
  namespace ccp
  {
    class source_reader;

    class pp_tokenizer
    {
    public:
      pp_tokenizer(header_inclusion_node &file);

      code_remarks& get_remarks() noexcept
      {
	return _remarks;
      }

      pp_token read_next_token();

      const header_inclusion_node& get_header_inclusion_node() const noexcept
      { return _file; }

    private:
      char _read_next_char_raw();

      char _read_next_char(file_range::loc_type &loc);
      void _advance_to_next_char();
      void _skip_next_char();

      pp_token _tokenize_string(const char delim,const bool delim_escapable,
				const pp_token::type tok_type);
      pp_token _tokenize_punctuator();
      pp_token _tokenize_pp_number();
      pp_token _tokenize_id();
      std::size_t _skip_c_comment(const std::size_t n_trailing_spaces);
      void _skip_cpp_comment();
      pp_token _tokenize_ws();

      header_inclusion_node &_file;
      std::unique_ptr<source_reader> _sr;
      source_reader::buffer_type _buf;
      source_reader::buffer_type::const_iterator _buf_it;

      file_range::loc_type _line_length;
      file_range::loc_type _cur_loc;
      file_range::loc_type _next_loc;
      file_range::loc_type _next_next_loc;
      file_range::loc_type _tok_loc;
      enum class expect_qh_str
      {
	no,
	newline,
	sharp_seen,
	expect_qh_str,
      };
      expect_qh_str _expect_qh_str;
      code_remarks _remarks;
      char _pending;
      char _cur;
      char _next;
      char _next_next;
    };
  }
}

#endif
