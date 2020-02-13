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

#include <iostream>
#include "preprocessor.hh"
#include "cmdline_except.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "semantic_except.hh"
#include "target_x86_64_gcc.hh"

using namespace klp::ccp;

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "error: usage: " << argv[0] << " <file>" << std::endl;
    return 1;
  }

  header_resolver hr;
  target_x86_64_gcc tgt{"4.8.5"};
  preprocessor p{hr, tgt};
  try {
    tgt.parse_command_line(1, const_cast<const char**>(&argv[1]), hr, p,
			   [](const std::string&) {
			     assert(0);
			     __builtin_unreachable();
			   });
  } catch (const cmdline_except &e) {
    std::cerr << "internal compiler command line error: "
	      << e.what() << std::endl;
    return 1;
  }

  const pp_tokens &tokens = p.get_result().get_pp_tokens();
  while(true) {
    try {
      auto &tok = tokens[p.read_next_token()];
      if (!p.get_remarks().empty()) {
	std::cerr << p.get_remarks();
	p.get_remarks().clear();
      }

      if (tok.is_eof())
	break;

      std::cout << tok.stringify();

    } catch (const pp_except&) {
	std::cerr << p.get_remarks();
	return 1;
    } catch (const parse_except&) {
      std::cerr << p.get_remarks();
      return 2;
    } catch (const semantic_except&) {
      std::cerr << p.get_remarks();
      return 3;
    }
  }

  return 0;
}
