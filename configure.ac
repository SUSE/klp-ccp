AC_INIT([cp], [0.0.1], [nstange@suse.de])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AC_PROG_RANLIB

AC_PROG_YACC

AC_ARG_ENABLE([debug-parser],
	      [AS_HELP_STRING([--enable-debug-parser],
			      [enable bison parser state output (default no)])])

if [[ "x$enable_debug_parser" = xyes ]]; then
   AC_DEFINE([DEBUG_PARSER], [1], [bison parser state output])
fi

AC_CONFIG_FILES([Makefile
		 testsuite/Makefile
		 testsuite/lib/Makefile
		 testsuite/cpp/Makefile
		 testsuite/cparse/Makefile
		 testsuite/cparse/gcc.dg/Makefile
		 testsuite/cparse/gcc.c-torture/Makefile
		 testsuite/cparse/gcc.c-torture/compile/Makefile
		 testsuite/cparse/gcc.c-torture/execute/Makefile
		 testsuite/unit/Makefile])
AC_OUTPUT
