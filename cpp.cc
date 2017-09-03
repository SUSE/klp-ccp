#include <iostream>
#include "preprocessor.hh"
#include "pp_except.hh"

using namespace suse::cp;

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "error: usage: " << argv[0] << " <file>" << std::endl;
    return 1;
  }

  header_resolver hr;
  preprocessor p(argv[1], hr);

  while(true) {
    try {
      auto tok = p.read_next_token();
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
    }
  }

  return 0;
}
