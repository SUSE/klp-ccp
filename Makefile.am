SUBDIRS = . testsuite

BUILT_SOURCES = gnuc_parser.hh stack.hh

noinst_HEADERS =			\
	ast.hh				\
	ast_processor.hh		\
	ast_processor_impl.hh		\
	callables_wrapper.hh		\
	code_remark.hh			\
	code_remarks.hh		\
	cow_set.hh			\
	file_range.hh			\
	gnuc_parser_driver.hh		\
	header_inclusion_tree.hh	\
	header_resolver.hh		\
	macro.hh			\
	macro_undef.hh			\
	offset_to_line_col_map.hh	\
	parse_except.hh		\
	path.hh			\
	pp_except.hh			\
	pp_token.hh			\
	pp_tokens.hh			\
	pp_tokenizer.hh		\
	preprocessor.hh		\
	semantic_except.hh		\
	type_set.hh			\
	used_macros.hh			\
	used_macro_undefs.hh

noinst_LIBRARIES = libcp.a
libcp_a_SOURCES =			\
	ast.cc				\
	ast_resolve.cc			\
	code_remark.cc			\
	code_remarks.cc		\
	file_range.cc			\
	gnuc_parser.yy			\
	gnuc_parser_driver.cc		\
	header_inclusion_tree.cc	\
	header_resolver.cc		\
	macro.cc			\
	macro_undef.cc			\
	offset_to_line_col_map.cc	\
	parse_except.cc		\
	path.cc			\
	pp_except.cc			\
	pp_token.cc			\
	pp_tokens.cc			\
	pp_tokenizer.cc		\
	preprocessor.cc		\
	semantic_except.cc		\
	used_macros.cc			\
	used_macro_undefs.cc

check_PROGRAMS = cpp_tokenize normalize_path dirname cpp cparse

cpp_tokenize_SOURCES = cpp_tokenize.cc
cpp_tokenize_LDADD = libcp.a

normalize_path_SOURCES = normalize_path.cc
normalize_path_LDADD = libcp.a

dirname_SOURCES = dirname.cc
dirname_LDADD = libcp.a

cpp_SOURCES = cpp.cc
cpp_LDADD = libcp.a


cparse_SOURCES = cparse.cc
cparse_LDADD = libcp.a
