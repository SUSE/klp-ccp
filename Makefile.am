noinst_HEADERS =		\
	code_remark.hh		\
	code_remarks.hh	\
	file_loc.hh		\
	header_resolver.hh	\
	path.hh		\
	pp_token.hh		\
	pp_tokenizer.hh

noinst_LIBRARIES = libcp.a
libcp_a_SOURCES =		\
	code_remark.cc		\
	code_remarks.cc	\
	file_loc.cc		\
	header_resolver.cc	\
	path.cc		\
	pp_tokenizer.cc

noinst_PROGRAMS = cpp_tokenize normalize_path dirname

cpp_tokenize_SOURCES = cpp_tokenize.cc
cpp_tokenize_LDADD = libcp.a

normalize_path_SOURCES = normalize_path.cc
normalize_path_LDADD = libcp.a

dirname_SOURCES = dirname.cc
dirname_LDADD = libcp.a
