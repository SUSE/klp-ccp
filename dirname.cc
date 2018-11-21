#include <iostream>
#include "path.hh"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "error: usage: " << argv[0] << " <path>" << std::endl;
    return 1;
  }

  std::cout << klp::ccp::dirname(argv[1])
	    << std::endl;

  return 0;
}
