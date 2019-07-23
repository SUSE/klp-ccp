#include <unistd.h>
#include <getopt.h>
#include <cstring>
#include <iostream>
#include <vector>
#include "arch_x86_64_gcc.hh"
#include "cmdline_except.hh"
#include "user_policy_command.hh"
#include "header_resolver.hh"
#include "preprocessor.hh"
#include "gnuc_parser_driver.hh"
#include "pp_except.hh"
#include "parse_except.hh"
#include "semantic_except.hh"
#include "lp_except.hh"
#include "lp_creation_policy_user_commands.hh"
#include "output_remarks.hh"
#include "create_lp.hh"

using namespace klp::ccp;

static const char prog_name[] = "klp-ccp";

static const char optstr[] = ":hc:o:i:I:H:F:S:O:E:P:R:";

static const option longopts[] {
	{ "help", 0, nullptr, 'h' },
	{ "compiler", 1, nullptr, 'c' },
	{ "outfile", 1, nullptr, 'o' },
	{ "patched-functions", 1, nullptr, 'i' },
	{ "pol-cmd-is-patched", 1, nullptr, 'I' },
	{ "pol-cmd-may-include-header", 1, nullptr, 'H' },
	{ "pol-cmd-can-externalize-fun", 1, nullptr, 'F' },
	{ "pol-cmd-shall-externalize-fun", 1, nullptr, 'S' },
	{ "pol-cmd-shall-externalize-obj", 1, nullptr, 'O' },
	{ "pol-cmd-modify-externalized-sym", 1, nullptr, 'E' },
	{ "pol-cmd-modify-patched-fun-sym", 1, nullptr, 'P' },
	{ "pol-cmd-rename-rewritten-fun", 1, nullptr, 'R' },
	{ nullptr, 0, nullptr, 0 }
};

static void show_usage(const char * const prog)
{
  std::cerr << "See '" << prog << " --help' for usage." << std::endl;
}

static void show_help(const char * const prog)
{
  std::cout
    << "Usage: " << prog
    << " OPTIONS"
    << " -- ORIGINAL COMPILER COMMAND LINE"
    << std::endl;

  std::cout << "Options:" << std::endl;
  std::cout
    << " -c, --compiler=ARCH-COMPILER-VERSION"
    << "\t\tTarget compiler specification."
    << std::endl;
  std::cout
    << " -o, --outfile=FILE"
    << "\t\t\t\tOutput file."
    << std::endl;
  std::cout
    << " -i, --patched-functions=LIST"
    << "\t\t\tComma separated list of patched" << std::endl
    << "\t\t\t\t\t\tfunctions."
    << std::endl;
  std::cout
    << " -I, --pol-cmd-is-patched=CMD"
    << "\t\t\tPolicy command determining set" << std::endl
    << "\t\t\t\t\t\tof patched functions."
    << std::endl;
  std::cout
    << " -H, --pol-cmd-may-include-header=CMD"
    << "\t\tPolicy command determining" << std::endl
    << "\t\t\t\t\t\theader file includability."
    << std::endl;
  std::cout
    << " -F, --pol-cmd-can-externalize-fun=CMD"
    << "\t\tPolicy command determining" << std::endl
    << "\t\t\t\t\t\tfunction externalizability."
    << std::endl;
  std::cout
    << " -S, --pol-cmd-shall-externalize-fun=CMD"
    << "\tPolicy command determining" << std::endl
    << "\t\t\t\t\t\tpreference on function" << std::endl
    << "\t\t\t\t\t\texternalization."
    << std::endl;
  std::cout
    << " -O, --pol-cmd-shall-externalize-obj=CMD"
    << "\tPolicy command determining" << std::endl
    << "\t\t\t\t\t\tobject externalization."
    << std::endl;
  std::cout
    << " -E, --pol-cmd-modify-externalized-sym=CMD"
    << "\tPolicy command determining" << std::endl
    << "\t\t\t\t\t\tmodifications to externalized" << std::endl
    << "\t\t\t\t\t\tsymbols."
    << std::endl;
  std::cout
    << " -P, --pol-cmd-modify-patched-fun-sym=CMD"
    << "\tPolicy command determining" << std::endl
    << "\t\t\t\t\t\tmodifications to symbols" << std::endl
    << "\t\t\t\t\t\tof patched functions."
    << std::endl;
  std::cout
    << " -R, --pol-cmd-renamed-rewritten-fun=CMD"
    << "\tPolicy command determining" << std::endl
    << "\t\t\t\t\t\trenames to apply to rewritten," << std::endl
    << "\t\t\t\t\t\tbut non-patched functions." << std::endl
    << std::endl;

  std::cout
    << "ORIGINAL COMPILER COMMAND LINE"
    << "\tdenotes the compiler invocation from the live" << std::endl
    << "\t\t\t\tpatching target's original compilation, without" << std::endl
    << "\t\t\t\tthe program name."
    << std::endl << std::endl;
  std::cout
    << "For further information, please see"
    << " <https://github.com/suse/klp-ccp>."
    << std::endl;
}

static void show_opt_duplicate(const char * const prog,
			       const char * const o)
{
  std::cerr << "command line error: "
	    << " option '" << o << "' specified more than once"
	    << std::endl;
  show_usage(prog);
}

static void show_opt_missing(const char * const prog,
			       const char * const o)
{
  std::cerr << "command line error: "
	    << " option '" << o << "' is required"
	    << std::endl;
  show_usage(prog);
}


static void split_patched_funs(const char *arg, std::vector<std::string> &funs)
{
  while (true) {
    const char *delim = std::strchr(arg, ',');
    if (!delim) {
      delim = arg + std::strlen(arg);
    }

    const auto& f = std::use_facet<std::ctype<char>>(std::locale());
    while (arg != delim &&
	   f.is(std::ctype_base::blank, *arg)) {
      ++arg;
    }

    const char *cur_end = delim;
    while (cur_end != arg &&
	   f.is(std::ctype_base::blank, *(cur_end - 1))) {
      --cur_end;
    }

    if (arg != cur_end)
      funs.emplace_back(arg, cur_end);

    if (*delim == '\0')
      break;

    arg = delim + 1;
  }
}

int main(int argc, char *argv[], char *envp[])
{

  bool do_help = false;
  char *o_compiler = nullptr;
  const char *o_outfile = nullptr;
  std::vector<std::string> patched_funs;
  const char *o_pol_cmd_is_patched = nullptr;
  const char *o_pol_cmd_may_include_header = nullptr;
  const char *o_pol_cmd_can_externalize_fun = nullptr;
  const char *o_pol_cmd_shall_externalize_fun = nullptr;
  const char *o_pol_cmd_shall_externalize_obj = nullptr;
  const char *o_pol_cmd_mod_externalized_sym = nullptr;
  const char *o_pol_cmd_mod_patched_fun_sym = nullptr;
  const char *o_pol_cmd_rename_rewritten_fun = nullptr;

  int o;
  int longindex = -1;
  while (optind < argc && std::strcmp(argv[optind], "--") &&
	 (o = getopt_long(argc, argv, optstr, longopts, &longindex)) != -1) {
    switch (o) {
    case 'h':
      do_help = true;
      break;

    case 'c':
      if (o_compiler) {
	show_opt_duplicate(prog_name, "--compiler");
	return 1;
      }
      o_compiler = optarg;
      break;

    case 'o':
      if (o_outfile) {
	show_opt_duplicate(prog_name, "--outfile");
	return 1;
      }
      o_outfile = optarg;
      break;

    case 'i':
      split_patched_funs(optarg, patched_funs);
      break;

    case 'I':
      if (o_pol_cmd_is_patched) {
	show_opt_duplicate(prog_name, "--pol-cmd-is-patched");
	return 1;
      }
      o_pol_cmd_is_patched = optarg;
      break;

    case 'H':
      if (o_pol_cmd_may_include_header) {
	show_opt_duplicate(prog_name, "--pol-cmd-may-include-header");
	return 1;
      }
      o_pol_cmd_may_include_header = optarg;
      break;

    case 'F':
      if (o_pol_cmd_can_externalize_fun) {
	show_opt_duplicate(prog_name, "--pol-cmd-can-externalize-fun");
	return 1;
      }
      o_pol_cmd_can_externalize_fun = optarg;
      break;

    case 'S':
      if (o_pol_cmd_shall_externalize_fun) {
	show_opt_duplicate(prog_name, "--pol-cmd-shall-externalize-fun");
	return 1;
      }
      o_pol_cmd_shall_externalize_fun = optarg;
      break;

    case 'O':
      if (o_pol_cmd_shall_externalize_obj) {
	show_opt_duplicate(prog_name, "--pol-cmd-shall-externalize-obj");
	return 1;
      }
      o_pol_cmd_shall_externalize_obj = optarg;
      break;

    case 'E':
      if (o_pol_cmd_mod_externalized_sym) {
	show_opt_duplicate(prog_name, "--pol-cmd-modify-externalized-sym");
	return 1;
      }
      o_pol_cmd_mod_externalized_sym = optarg;
      break;

    case 'P':
      if (o_pol_cmd_mod_patched_fun_sym) {
	show_opt_duplicate(prog_name, "--pol-cmd-modify-patched-fun-sym");
	return 1;
      }
      o_pol_cmd_mod_patched_fun_sym = optarg;
      break;

    case 'R':
      if (o_pol_cmd_rename_rewritten_fun) {
	show_opt_duplicate(prog_name, "--pol-cmd-rename-rewritten-fun");
	return 1;
      }
      o_pol_cmd_rename_rewritten_fun = optarg;
      break;

    case '?':
      std::cerr << "command line error: invalid option '";
      if (optopt)
	std::cerr << '-' << static_cast<char>(optopt);
      else
	std::cerr << argv[optind - 1];
      std::cerr << '\'' << std::endl;
      show_usage(prog_name);
      return 1;

    case ':':
      std::cerr << "command line error: missing parameter for option '";
      if (optopt)
	std::cerr << '-' << static_cast<char>(optopt);
      else
	std::cerr << longopts[longindex].name;
      std::cerr << '\'' << std::endl;
      show_usage(prog_name);
      return 1;
    }
  }

  if (do_help) {
    show_help(prog_name);
    return 0;
  }

  if (optind != argc && !std::strcmp(argv[optind], "--"))
    ++optind;

  if (optind == argc) {
    std::cerr << "command line error: missing compiler invocation" << std::endl;
    show_usage(prog_name);
    return 1;
  }

  if (!o_compiler) {
    show_opt_missing(prog_name, "--compiler");
    return 1;
  }

  char *o_compiler_version = std::strrchr(o_compiler, '-');
  if (!o_compiler_version || !std::strlen(o_compiler_version + 1)) {
    std::cerr
      << "command line error: compiler version specification missing in '"
      << o_compiler << '\'' << std::endl;
    show_usage(prog_name);
    return 1;
  }
  *o_compiler_version = '\0';
  ++o_compiler_version;


  if (!o_outfile) {
    show_opt_missing(prog_name, "--outfile");
    return 1;
  }

  if (!o_pol_cmd_is_patched && patched_funs.empty()) {
    std::cerr
      << "command line error: "
      << "either --patched-functions or --pol-cmd-is-patched is required"
      << std::endl;
    show_usage(prog_name);
    return 1;
  }


  std::unique_ptr<const user_policy_command> pol_cmd_is_patched;
  if (o_pol_cmd_is_patched) {
    try {
      pol_cmd_is_patched.reset(new user_policy_command{o_pol_cmd_is_patched});

    } catch (const user_policy_command::cmd_parse_except &e) {
      std::cerr << "command line error: failed to parse policy command '"
		<< o_pol_cmd_is_patched << "': "
		<< e.what() << std::endl;
      show_usage(prog_name);
      return 1;
    }
  }

  if (!o_pol_cmd_may_include_header) {
    std::cerr
      << "command line error: --pol-cmd-may-include-header is required"
      << std::endl;
    show_usage(prog_name);
    return 1;
  }
  std::unique_ptr<user_policy_command> pol_cmd_may_include_header;
  try {
    pol_cmd_may_include_header.reset
      (new user_policy_command{o_pol_cmd_may_include_header});

  } catch (const user_policy_command::cmd_parse_except &e) {
    std::cerr << "command line error: failed to parse policy command '"
	      << o_pol_cmd_may_include_header << "': "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }

  if (!o_pol_cmd_can_externalize_fun) {
    std::cerr
      << "command line error: --pol-cmd-can-externalize-fun is required"
      << std::endl;
    show_usage(prog_name);
    return 1;
  }
  std::unique_ptr<user_policy_command> pol_cmd_can_externalize_fun;
  try {
    pol_cmd_can_externalize_fun.reset
      (new user_policy_command{o_pol_cmd_can_externalize_fun});

  } catch (const user_policy_command::cmd_parse_except &e) {
    std::cerr << "command line error: failed to parse policy command '"
	      << o_pol_cmd_can_externalize_fun << "': "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }

  if (!o_pol_cmd_shall_externalize_fun) {
    std::cerr
      << "command line error: --pol-cmd-shall-externalize-fun is required"
      << std::endl;
    show_usage(prog_name);
    return 1;
  }
  std::unique_ptr<user_policy_command> pol_cmd_shall_externalize_fun;
  try {
    pol_cmd_shall_externalize_fun.reset
      (new user_policy_command{o_pol_cmd_shall_externalize_fun});

  } catch (const user_policy_command::cmd_parse_except &e) {
    std::cerr << "command line error: failed to parse policy command '"
	      << o_pol_cmd_shall_externalize_fun << "': "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }

  if (!o_pol_cmd_shall_externalize_obj) {
    std::cerr
      << "command line error: --pol-cmd-shall-externalize-obj is required"
      << std::endl;
    show_usage(prog_name);
    return 1;
  }
  std::unique_ptr<user_policy_command> pol_cmd_shall_externalize_obj;
  try {
    pol_cmd_shall_externalize_obj.reset
      (new user_policy_command{o_pol_cmd_shall_externalize_obj});

  } catch (const user_policy_command::cmd_parse_except &e) {
    std::cerr << "command line error: failed to parse policy command '"
	      << o_pol_cmd_shall_externalize_obj << "': "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }

  if (!o_pol_cmd_mod_externalized_sym) {
    std::cerr
      << "command line error: --pol-cmd-modify-externalized-sym is required"
      << std::endl;
    show_usage(prog_name);
    return 1;
  }
  std::unique_ptr<user_policy_command> pol_cmd_mod_externalized_sym;
  try {
    pol_cmd_mod_externalized_sym.reset
      (new user_policy_command{o_pol_cmd_mod_externalized_sym});

  } catch (const user_policy_command::cmd_parse_except &e) {
    std::cerr << "command line error: failed to parse policy command '"
	      << o_pol_cmd_mod_externalized_sym << "': "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }

  if (!o_pol_cmd_mod_patched_fun_sym) {
    std::cerr
      << "command line error: --pol-cmd-modify-patched-fun-sym is required"
      << std::endl;
    show_usage(prog_name);
    return 1;
  }
  std::unique_ptr<user_policy_command> pol_cmd_mod_patched_fun_sym;
  try {
    pol_cmd_mod_patched_fun_sym.reset
      (new user_policy_command{o_pol_cmd_mod_patched_fun_sym});

  } catch (const user_policy_command::cmd_parse_except &e) {
    std::cerr << "command line error: failed to parse policy command '"
	      << o_pol_cmd_mod_patched_fun_sym << "': "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }

  if (!o_pol_cmd_rename_rewritten_fun) {
    std::cerr
      << "command line error: --pol-cmd-rename-rewritten-fun is required"
      << std::endl;
    show_usage(prog_name);
    return 1;
  }
  std::unique_ptr<user_policy_command> pol_cmd_rename_rewritten_fun;
  try {
    pol_cmd_rename_rewritten_fun.reset
      (new user_policy_command{o_pol_cmd_rename_rewritten_fun});

  } catch (const user_policy_command::cmd_parse_except &e) {
    std::cerr << "command line error: failed to parse policy command '"
	      << o_pol_cmd_rename_rewritten_fun << "': "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }


  std::unique_ptr<architecture> arch;
  if (!strcmp(o_compiler, "x86_64-gcc")) {
    try {
      arch.reset(new arch_x86_64_gcc{o_compiler_version});
    } catch (const cmdline_except &e) {
      std::cerr << "command line error: " << e.what() << std::endl;
      show_usage(prog_name);
      return 1;
    }
  } else {
    std::cerr << "command line error: "
	      << "unrecognized compiler specification '"
	      << o_compiler << '\'' << std::endl;
    show_usage(prog_name);
    return 1;
  }

  header_resolver hr;
  preprocessor pp(hr, *arch);
  try {
    arch->parse_command_line(argc - optind,
			     const_cast<const char **>(&argv[optind]),
			     hr, pp,
			     [&](const std::string &warning) {
			       std::cerr << "compiler command line warning: "
					 << warning << std::endl;
			     });
  } catch (const cmdline_except &e) {
    std::cerr << "compiler command line error: "
	      << e.what() << std::endl;
    show_usage(prog_name);
    return 1;
  }


  int r = 0;
  yy::gnuc_parser_driver pd{std::move(pp), *arch};
  try {
    pd.parse();

  } catch (const pp_except &) {
    r = 2;
  } catch (const parse_except&) {
    r = 2;
  } catch (const semantic_except&) {
    r = 2;
  }

  if (!pd.get_remarks().empty())
    std::cerr << pd.get_remarks();
  if (r)
    return r;


  ast::ast_translation_unit ast(pd.grab_result());
  try {
    ast.resolve(*arch);
  } catch (const semantic_except&) {
    r = 2;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;

  try {
    ast.evaluate(*arch);
  } catch (const semantic_except&) {
    r = 2;
  }
  if (!ast.get_remarks().empty())
    std::cerr << ast.get_remarks();
  ast.get_remarks().clear();
  if (r)
    return r;


  lp_creation_policy_user_commands pol{
	ast.get_pp_result(),
	envp,
	std::move(patched_funs),
	std::move(pol_cmd_is_patched),
	std::move(*pol_cmd_may_include_header),
	std::move(*pol_cmd_can_externalize_fun),
	std::move(*pol_cmd_shall_externalize_fun),
	std::move(*pol_cmd_shall_externalize_obj),
	std::move(*pol_cmd_mod_externalized_sym),
	std::move(*pol_cmd_mod_patched_fun_sym),
	std::move(*pol_cmd_rename_rewritten_fun),
  };

  code_remarks iremarks;
  output_remarks oremarks;
  try {
    create_lp(o_outfile, ast, pol, iremarks, oremarks);

  } catch (const lp_except&) {
    r = 3;

  } catch (const user_policy_command::execution_except &e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 4;

  } catch (const std::system_error &e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 5;
  }

  if (!iremarks.empty())
    std::cerr << iremarks;
  iremarks.clear();
  if (r)
    return r;

  if (!oremarks.empty())
    std::cerr << oremarks;
  oremarks.clear();
  if (r)
    return r;

  return 0;
}
