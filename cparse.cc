#include <unistd.h>
#include <getopt.h>
#include "gnuc_parser_driver.hh"
#include "header_resolver.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "semantic_except.hh"
#include "arch_gcc48_x86_64.hh"

using namespace suse::cp;

static const option options[] = {
  { "include", true, nullptr, 'i' },
  { "help", true, nullptr, 'h' },
  {}
};

static const char optstring[] = "h";

static void print_help()
{
  std::cerr << "usage: "
	    << "cparse [-include <file>] [-h] <file>" << std::endl;
}

static void print_usage()
{
  std::cerr << "error: invalid command line" << std::endl;
}

int main(int argc, char* argv[])
{
  int r = 0;
  header_resolver hr;
  preprocessor::header_inclusion_roots_type hirs;

  while(true) {
    const int o = getopt_long(argc, argv, optstring, options, NULL);

    if (o == -1)
      break;

    switch (o) {
    case '?':
      print_usage();
      return 1;

    case 'h':
      print_help();
      return 0;

    case 'i':
      {
	const std::string resolved = hr.resolve(optarg,
						header_resolver::cwd);
	if (resolved.empty()) {
	  std::cerr << "error: " << optarg << " not found";
	  return 1;
	}

	std::unique_ptr<header_inclusion_root> hir{
	  new header_inclusion_root(resolved, true)};
	hirs.emplace_back(std::move(hir));
      };
    }
  }

  if (optind != argc - 1) {
    print_usage();
    return 1;
  }

  std::unique_ptr<header_inclusion_root> hir{
    new header_inclusion_root(argv[optind], false)};
  hirs.emplace_back(std::move(hir));
  yy::gnuc_parser_driver pd(preprocessor{hirs, hr});

  try {
#ifdef DEBUG_PARSER
    pd.parse_debug();
#else
    pd.parse();
#endif
  } catch (const pp_except&) {
    r = 2;
  } catch (const parse_except&) {
    r = 3;
  }

  if (!pd.get_remarks().empty())
    std::cerr << pd.get_remarks();
  if (r)
    return r;

  ast::ast ast(pd.grab_result());
  try {
    ast.resolve();
  } catch (const semantic_except&) {
    r = 4;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  arch_gcc48_x86_64 arch;
  try {
    ast.evaluate(arch);
  } catch (const semantic_except&) {
    r = 5;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  return r;
}
