#include <fstream>
#include <iostream>
#include "pp_tokenizer.hh"

using namespace suse::cp;

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
    CASE(str);
    CASE(wstr);
    CASE(qstr);
    CASE(hstr);
    CASE(punctuator);
    CASE(non_ws_char);
    CASE(eof);
  }
#undef CASE
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "error: usage: " << argv[0] << " <file>" << std::endl;
    return 1;
  }

  pp_tokenizer tokker(argv[1]);
  while (true)
    {
      pp_token tok = tokker.read_next_token();
      if (!tok)
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
