AM_CPPFLAGS = -I$(top_srcdir)

AUTOMAKE_OPTIONS = subdir-objects

noinst_LIBRARIES = libtarget.a

noinst_HEADERS = \
		 gcc/gcc_cmdline_opts_c_family.cc	\
		 gcc/gcc_cmdline_opts_common.cc		\
		 gcc/target_gcc.hh			\
		 gcc/x86_64/gcc_cmdline_opts_i386.cc	\
		 gcc/aarch64/gcc_cmdline_opts_aarch64.cc	\
		 target.hh

libtarget_a_SOURCES = \
		      builtins_impl.cc				\
		      cmdline_except.cc				\
		      execution_charset_encoder_iconv.cc	\
		      gcc/gcc_cmdline_parser.cc			\
		      gcc/target_gcc.cc				\
		      gcc/x86_64/target_x86_64_gcc.cc		\
		      gcc/aarch64/target_aarch64_gcc.cc			\
		      target.cc
