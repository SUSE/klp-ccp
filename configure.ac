AC_INIT([klp-ccp], [0.0.1], [nstange@suse.de])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AC_PROG_RANLIB

AC_PROG_YACC

AC_ARG_ENABLE([debug-parser],
	      [AS_HELP_STRING([--enable-debug-parser],
			      [enable bison parser state output (default no)])])

if [[ "x$enable_debug_parser" = xyes ]]; then
   AX_APPEND_FLAG([-DDEBUG_PARSER=1], [CPPFLAGS])
fi

AC_CONFIG_HEADERS([config.h])

bison_version=[`bison --version | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+'`]
AX_COMPARE_VERSION(["$bison_version"], [lt], ["3.6"],
        [AC_SUBST([BISON_YYLA_HAS_TYPE], [1])],
        [AC_SUBST([BISON_YYLA_HAS_TYPE], [0])])

AC_DEFINE_UNQUOTED([BISON_YYLA_HAS_TYPE], [$BISON_YYLA_HAS_TYPE],
                   [Bison < 3.6 has a .type entry in the lookahead token,
                    which was renamed to .kind_ in 3.6 (symbol_kind)])


python3_embed_found=no
for python3_embed_pkg in python-3.13-embed python-3.12-embed python-3.11-embed python3-embed; do
  PKG_CHECK_MODULES([PYTHON3_EMBED], [$python3_embed_pkg],
		    [python3_embed_found=yes], [:])
  if test "x$python3_embed_found" = xyes; then
    break
  fi
done
if test "x$python3_embed_found" != xyes; then
  AC_MSG_ERROR([Python development files not found])
fi

AC_CONFIG_FILES([Makefile
		 target/Makefile
		 lp-creation-python-policy/Makefile
		 testsuite/Makefile
		 testsuite/lib/Makefile
		 testsuite/lib/ccp-toy-policy/Makefile
		 testsuite/cpp/Makefile
		 testsuite/cpp/gcc.dg/Makefile
		 testsuite/cparse/Makefile
		 testsuite/cparse/gcc.dg/Makefile
		 testsuite/cparse/gcc.c-torture/Makefile
		 testsuite/cparse/gcc.c-torture/compile/Makefile
		 testsuite/cparse/gcc.c-torture/execute/Makefile
		 testsuite/unit/Makefile
		 testsuite/ccp/Makefile
		 examples/Makefile
		 examples/policies/Makefile
		 examples/policies/suse/Makefile])
AC_OUTPUT
