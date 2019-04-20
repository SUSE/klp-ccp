#include <iostream>
#include "preprocessor.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "semantic_except.hh"
#include "arch_gcc48_x86_64.hh"

using namespace klp::ccp;

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "error: usage: " << argv[0] << " <file>" << std::endl;
    return 1;
  }

  std::unique_ptr<pp_result::header_inclusion_root> hir{
    new pp_result::header_inclusion_root(argv[1], false) };
  std::vector<std::unique_ptr<pp_result::header_inclusion_root>> hirs;
  hirs.emplace_back(std::move(hir));
  header_resolver hr;
  arch_gcc48_x86_64 arch;
  preprocessor p{pp_result::header_inclusion_roots{std::move(hirs)}, hr, arch};
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
