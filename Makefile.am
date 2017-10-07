SUBDIRS = . testsuite

noinst_HEADERS =			\
	code_remark.hh			\
	code_remarks.hh		\
	cow_set.hh			\
	file_loc.hh			\
	file_range.hh			\
	header_resolver.hh		\
	macro.hh			\
	macro_undef.hh			\
	path.hh			\
	pp_except.hh			\
	pp_token.hh			\
	pp_tokens.hh			\
	pp_tokenizer.hh		\
	preprocessor.hh		\
	used_macros.hh			\
	used_macro_undefs.hh

noinst_LIBRARIES = libcp.a
libcp_a_SOURCES =			\
	code_remark.cc			\
	code_remarks.cc		\
	file_loc.cc			\
	file_range.cc			\
	header_resolver.cc		\
	macro.cc			\
	macro_undef.cc			\
	path.cc			\
	pp_except.cc			\
	pp_token.cc			\
	pp_tokens.cc			\
	pp_tokenizer.cc		\
	preprocessor.cc		\
	used_macros.cc			\
	used_macro_undefs.cc

check_PROGRAMS = cpp_tokenize normalize_path dirname cpp

cpp_tokenize_SOURCES = cpp_tokenize.cc
cpp_tokenize_LDADD = libcp.a

normalize_path_SOURCES = normalize_path.cc
normalize_path_LDADD = libcp.a

dirname_SOURCES = dirname.cc
dirname_LDADD = libcp.a

cpp_SOURCES = cpp.cc
cpp_LDADD = libcp.a
