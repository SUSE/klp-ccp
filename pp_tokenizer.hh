#ifndef PP_TOKENIZER_HH
#define PP_TOKENIZER_HH

#include <fstream>
#include <string>
#include "code_remark.hh"
#include "code_remarks.hh"
#include "pp_token.hh"

namespace suse
{
  namespace cp
  {
    class pp_tokenizer
    {
    public:
      pp_tokenizer(header_inclusion_node &file);

      code_remarks& get_remarks() noexcept
      {
	return _remarks;
      }

      pp_token read_next_token();

    private:
      char _read_next_char_raw();

      char _read_next_char(std::streamoff &loc);
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
      std::ifstream _i;
      std::streamoff _line_length;
      std::streamoff _cur_loc;
      std::streamoff _next_loc;
      std::streamoff _tok_loc;
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
    };
  }
}

#endif
