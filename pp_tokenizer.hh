#ifndef PP_TOKENIZER_HH
#define PP_TOKENIZER_HH

#include <fstream>
#include <string>
#include "file_loc.hh"
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
      pp_tokenizer(const std::string &filename);

      code_remarks& get_remarks() noexcept
      {
	return _remarks;
      }

      pp_token read_next_token();

    private:
      char _read_next_char(file_loc &loc, const char prev);
      void _advance_to_next_char();
      void _skip_next_char();

      pp_token _tokenize_c_comment();
      pp_token _tokenize_cpp_comment();
      pp_token _tokenize_string(const char delim, const bool delim_escapable,
				const pp_token::type tok_type);
      pp_token _tokenize_punctuator();
      pp_token _tokenize_pp_number();
      pp_token _tokenize_id();
      pp_token _tokenize_ws();

      std::string _filename;
      std::ifstream _i;
      file_loc _cur_loc;
      file_loc _next_loc;
      file_loc _tok_loc;
      enum class expect_qh_str
      {
	no,
	newline,
	sharp_seen,
	expect_qh_str,
      };
      expect_qh_str _expect_qh_str;
      code_remarks _remarks;
      char _cur;
      char _next;
    };
  }
}

#endif
