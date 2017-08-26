noinst_HEADERS =		\
	code_remark.hh		\
	code_remarks.hh	\
	file_loc.hh		\
	pp_token.hh		\
	pp_tokenizer.hh

noinst_LIBRARIES = libcp.a
libcp_a_SOURCES =		\
	code_remark.cc		\
	code_remarks.cc	\
	file_loc.cc		\
	pp_tokenizer.cc

noinst_PROGRAMS = cpp_tokenize
cpp_tokenize_SOURCES = cpp_tokenize.cc
cpp_tokenize_LDADD = libcp.a
