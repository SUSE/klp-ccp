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

#include <fstream>
#include <iostream>
#include "pp_tokenizer.hh"

using namespace klp::ccp;

static const char* type2name(pp_token::type type)
{
#define CASE(t) \
  case pp_token::type::t:	\
    return #t;

  switch (type) {
    CASE(ws);
    CASE(newline);
    CASE(id);
    CASE(pp_number);
    CASE(chr);
    CASE(wchr);
    CASE(uchr16);
    CASE(uchr32);
    CASE(str);
    CASE(wstr);
    CASE(ustr8);
    CASE(ustr16);
    CASE(ustr32);
    CASE(qstr);
    CASE(hstr);
    CASE(punctuator);
    CASE(non_ws_char);
    CASE(eof);
    CASE(empty);
  }
#undef CASE
  __builtin_unreachable();
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "error: usage: " << argv[0] << " <file>" << std::endl;
    return 1;
  }

  pp_result::header_inclusion_root hir (argv[1], false);
  pp_tokenizer tokker(hir);
  while (true)
    {
      auto tok = tokker.read_next_token();
      if (tok.is_eof())
	break;

      std::cout << '{' << type2name(tok.get_type())
		<< ':' << tok.get_value() << '}';
    }

  if (!tokker.get_remarks().empty()) {
    std::cerr << tokker.get_remarks();
    return 1;
  }

  return 0;
}
