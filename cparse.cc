#include "gnuc_parser_driver.hh"
#include "header_resolver.hh"
#include "pp_except.hh"
#include "parse_except.hh"

using namespace suse::cp;

int main(int argc, char* argv[])
{
  int r = 0;

  if (argc != 2) {
    std::cerr << "error: usage: " << argv[0] << " <file>" << std::endl;
    return 1;
  }

  header_resolver hr;
  yy::gnuc_parser_driver pd(argv[1], hr);

  try {
#ifdef DEBUG_PARSER
    pd.parse_debug();
#else
    pd.parse();
#endif
  } catch (const pp_except&) {
    r = 1;
  } catch (const parse_except&) {
    r = 2;
  }

  if (!pd.get_remarks().empty())
    std::cerr << pd.get_remarks();

  return r;
}
