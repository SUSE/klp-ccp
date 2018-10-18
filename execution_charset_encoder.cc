#include <stdexcept>
#include "execution_charset_encoder.hh"
#include "ast.hh"
#include "semantic_except.hh"
#include "architecture.hh"

using namespace suse::cp;

execution_charset_encoder::
execution_charset_encoder(const architecture &arch,
			  const types::std_int_type::kind target_char_kind)
  noexcept
  : _target_char_kind(target_char_kind),
    _target_char_width(arch.get_std_int_width(target_char_kind))
{}

std::vector<mpa::limbs>
execution_charset_encoder::encode_string(ast::ast &a, const pp_token &tok)
{
  const std::string &utf8_string = tok.get_value();
  std::vector<mpa::limbs> encoded;

  std::string::const_iterator it = utf8_string.begin();
  while (it != utf8_string.end()) {
    char32_t ucs4_char;

    if (*it == '\\') {
      // Some sort of escape sequence
      if (it + 1 == utf8_string.end()) {
	code_remark remark(code_remark::severity::fatal,
			   "unexpected end of string after '\\'",
			   tok.get_file_range());
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      ++it;
      if (*it == 'u' || *it == 'U') {
	// Universal character name
	const int n_digits = (*it == 'u' ? 4 : 8);
	++it;
	if (utf8_string.end() - it < n_digits) {
	  code_remark remark
	    (code_remark::severity::fatal,
	     "preliminary end of string in universal character name",
	     tok.get_file_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	mpa::limbs ls;
	try {
	  ls = mpa::limbs::from_string(it, it + n_digits, mpa::limb(16));
	} catch(const std::range_error&) {
	  code_remark remark(code_remark::severity::fatal,
			     "unrecognized digit in universal character name",
			     tok.get_file_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	assert(!ls.is_any_set_at_or_above(32));
	if (mpa::limb::width < 32) {
	  for(mpa::limbs::size_type i = ls.size(); i > 0; --i) {
	    ucs4_char <<= mpa::limb::width;
	    ucs4_char |= ls[i - 1].value();
	  }
	} else {
	  assert(ls.size() == 1);
	  ucs4_char = static_cast<char32_t>(ls[0].value());
	}

	it += n_digits;

      } else if (*it == 'x') {
	// Hexadecimal escape sequence
	++it;
	std::string::const_iterator hex_end;
	for (hex_end = it; hex_end != utf8_string.end(); ++hex_end) {
	  if (!((*hex_end >= '0' && *hex_end <= '9') ||
		(*hex_end >= 'a' && *hex_end <= 'f') ||
		(*hex_end >= 'A' && *hex_end <= 'F'))) {
	    break;
	  }
	}

	if (hex_end == it) {
	  code_remark remark(code_remark::severity::fatal,
			     "no digit in hexadecimal escape sequence",
			     tok.get_file_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	mpa::limbs ls =
	  mpa::limbs::from_string(it, hex_end, mpa::limb(16));
	if (ls.is_any_set_at_or_above(_target_char_width)) {
	  code_remark remark
	    (code_remark::severity::fatal,
	     "hexadecimal escape sequence exceeds target character width",
	     tok.get_file_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	encoded.emplace_back(std::move(ls));
	it = hex_end;
	continue;

      } else if (*it >= '0' && *it < '8') {
	// Octal escape sequence.
	std::string::const_iterator oct_end = it + 1;
	for (unsigned int i = 0; i < 2 && oct_end != utf8_string.end();
	     ++i, ++oct_end) {
	  if (!(*oct_end >= '0' && *oct_end < '8'))
	    break;
	}

	mpa::limbs ls =
	  mpa::limbs::from_string(it, oct_end, mpa::limb(8));
	if (ls.is_any_set_at_or_above(_target_char_width)) {
	  code_remark remark
	    (code_remark::severity::fatal,
	     "octal escape sequence exceeds target character width",
	     tok.get_file_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	encoded.emplace_back(std::move(ls));
	it = oct_end;
	continue;

      } else {
	// Some "simple" escape sequence.
	switch (*it) {
	case '\'':
	  ucs4_char = 0x27;
	  break;

	case '"':
	  ucs4_char = 0x22;
	  break;

	case '?':
	  ucs4_char = 0x3f;
	  break;

	case '\\':
	  ucs4_char = 0x5c;
	  break;

	case 'a':
	  ucs4_char = 0x07;
	  break;

	case 'b':
	  ucs4_char = 0x08;
	  break;

	case 'f':
	  ucs4_char = 0x0c;
	  break;

	case 'n':
	  ucs4_char = 0x0a;
	  break;

	case 'r':
	  ucs4_char = 0x0d;
	  break;

	case 't':
	  ucs4_char = 0x09;
	  break;

	case 'v':
	  ucs4_char = 0xb;
	  break;


	default:
	  code_remark remark(code_remark::severity::fatal,
			     "unrecognized escape sequence",
			     tok.get_file_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	++it;
      }

    } else {
      // An UTF-8 encoded source character.
      int n_bytes;
      unsigned char high_byte_mask;
      const unsigned char high_byte = static_cast<unsigned char>(*it);
      if ((high_byte >> 7) == 0x0) {
	n_bytes = 1;
	high_byte_mask = 0x7f;
      } else if ((high_byte >> 5) == 0x6) {
	n_bytes = 2;
	high_byte_mask = 0x1f;
      } else if ((high_byte >> 4) == 0xe) {
	n_bytes = 3;
	high_byte_mask = 0x0f;
      } else if ((high_byte >> 3) == 0x1e) {
	n_bytes = 4;
	high_byte_mask = 0x7;
      } else {
	code_remark remark(code_remark::severity::fatal,
			   "invalid UTF-8 character encoding",
			   tok.get_file_range());
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      if (utf8_string.end() - it < n_bytes) {
	code_remark remark(code_remark::severity::fatal,
			   "preliminary end in multibyte UTF-8 character",
			   tok.get_file_range());
	a.get_remarks().add(remark);
	throw semantic_except(remark);
      }

      ucs4_char = (high_byte & high_byte_mask);
      ++it;
      --n_bytes;
      while (n_bytes) {
	const unsigned char cur_byte = static_cast<unsigned char>(*it);
	if ((cur_byte >> 6) != 0x2) {
	  code_remark remark
	    (code_remark::severity::fatal,
	     "unexpected non-continuation byte in multibyte UTF-8 character",
	     tok.get_file_range());
	  a.get_remarks().add(remark);
	  throw semantic_except(remark);
	}

	ucs4_char <<= 6;
	ucs4_char |= (cur_byte & 0x3f);
	++it;
	--n_bytes;
      }
    }

    std::vector<mpa::limbs> encoded_char =
      this->encode_char(a, tok.get_file_range(), ucs4_char);
    encoded.insert(encoded.end(), encoded_char.begin(), encoded_char.end());
  }

#ifndef NDEBUG
  for (const auto &c : encoded) {
    assert(!c.is_any_set_at_or_above(_target_char_width));
  }
#endif

  return encoded;
}
