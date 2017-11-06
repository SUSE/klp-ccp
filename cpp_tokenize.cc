#include <fstream>
#include <iostream>
#include "pp_tokenizer.hh"
#include "header_inclusion_tree.hh"

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
    CASE(uchr8);
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

  header_inclusion_root hir (argv[1], false);
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
