#include <limits.h>
#include <cerrno>
#include <system_error>
#include <stdexcept>
#include <limits>
#include "semantic_except.hh"
#include "ast.hh"
#include "execution_charset_encoder_iconv.hh"

using namespace klp::ccp;

#if defined(WORDS_BIGENDIAN)
#define UCS4_CODE "UCS-4BE"
#else
#define UCS4_CODE "UCS-4LE"
#endif

execution_charset_encoder_iconv::
execution_charset_encoder_iconv(const architecture &arch,
			const types::std_int_type::kind target_char_kind,
			const std::string &target_code,
			const bool target_code_is_big_endian)
  : execution_charset_encoder(arch, target_char_kind),
    _target_code_is_big_endian(target_code_is_big_endian)
{
  _iconv = iconv_open(target_code.c_str(), UCS4_CODE);
  if (_iconv == reinterpret_cast<iconv_t>(-1)) {
      throw std::system_error(errno, std::system_category(), "iconv_open");
  }
}

execution_charset_encoder_iconv::~execution_charset_encoder_iconv() noexcept
{
  iconv_close(_iconv);
}

std::vector<mpa::limbs> execution_charset_encoder_iconv::
encode_char(ast::ast &a, const pp_token_index error_reporting_tok_ix,
	    char32_t ucs4_char)
{
  char encbuf[MB_LEN_MAX];
  char *outbuf = encbuf;
  char *inbuf = reinterpret_cast<char*>(&ucs4_char);
  size_t inbytesleft = 4, outbytesleft = sizeof(encbuf);

  size_t r = iconv(_iconv, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
  if (r == static_cast<size_t>(-1)) {
    if (errno == E2BIG) {
      throw std::logic_error("iconv out buffer unexpectedly too small");

    } else if (errno == EILSEQ || errno == EINVAL) {
      code_remark_pp remark(code_remark_pp::severity::fatal,
			    "character set conversion failure",
			    a.get_pp_tokens(), error_reporting_tok_ix);
      a.get_remarks().add(remark);
      throw semantic_except(remark);

    } else {
      throw std::system_error(errno, std::system_category(), "iconv");

    }
  }

  if (inbytesleft)
    throw std::logic_error("iconv did not consume whole USC4 character");

  const size_t outlen = sizeof(encbuf) - outbytesleft;
  static_assert(std::numeric_limits<unsigned char>::digits == 8,
		"CHAR_WIDTH != 8 not supported");
  assert(!(get_target_char_width() % 8));

  if ((outlen * 8) % get_target_char_width())
    throw std::logic_error("unexpected iconv encoding length");

  const size_t target_char_len = get_target_char_width() / 8;
  const std::vector<mpa::limbs>::size_type n_encoded = outlen / target_char_len;
  std::vector<mpa::limbs> encoded;
  encoded.reserve(n_encoded);
  for (std::vector<mpa::limbs>::size_type i = 0; i < n_encoded; ++i) {
    mpa::limbs c;
    c.resize(mpa::limbs::width_to_size(get_target_char_width()));

    if (_target_code_is_big_endian) {
      for (size_t j = 0; j < target_char_len; ++j) {
	const mpa::limbs::size_type v =
	  static_cast<unsigned char>(encbuf[i * target_char_len + j]);
	c = (c.lshift(std::numeric_limits<unsigned char>::digits) |
	     mpa::limbs::from_size_type(v));
      }
    } else {
      for (size_t j = target_char_len; j > 0; --j) {
	const mpa::limbs::size_type v =
	  static_cast<unsigned char>(encbuf[i * target_char_len + j - 1]);
	c = (c.lshift(std::numeric_limits<unsigned char>::digits) |
	     mpa::limbs::from_size_type(v));
      }
    }

    encoded.push_back(std::move(c));
  }

  return encoded;
}
