#include <unistd.h>
#include <getopt.h>
#include "gnuc_parser_driver.hh"
#include "header_resolver.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "semantic_except.hh"
#include "arch_x86_64_gcc.hh"

using namespace klp::ccp;

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
  arch_x86_64_gcc arch{"4.8.5"};
  preprocessor p{hr, arch};
  arch.parse_command_line(0, nullptr, hr, p,
			  [](const std::string&) {
			    assert(0);
			    __builtin_unreachable();
			  });
  std::vector<const char*> pre_includes;

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
      pre_includes.push_back(optarg);
      break;
    }
  }

  if (optind != argc - 1) {
    print_usage();
    return 1;
  }

  const std::string base_file{argv[optind]};

  for (const auto i : pre_includes) {
    const std::string resolved = hr.resolve(i, base_file,
					    header_resolver::cwd);
    if (resolved.empty()) {
      std::cerr << "error: " << i << " not found";
      return 1;
    }

    p.add_root_source(resolved, true);
  }

  p.add_root_source(base_file, false);
  p.set_base_file(base_file);
  yy::gnuc_parser_driver pd{std::move(p), arch};

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
  } catch (const semantic_except&) {
    r = 4;
  }

  if (!pd.get_remarks().empty())
    std::cerr << pd.get_remarks();
  if (r)
    return r;

  ast::ast_translation_unit ast(pd.grab_result());
  try {
    ast.resolve(arch);
  } catch (const semantic_except&) {
    r = 5;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  try {
    ast.evaluate(arch);
  } catch (const semantic_except&) {
    r = 6;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  return r;
}
