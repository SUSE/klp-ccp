SUBDIRS = . testsuite

AM_YFLAGS = -d
BUILT_SOURCES = gnuc_parser.hh stack.hh

noinst_HEADERS =				\
	arch_gcc48_x86_64.hh			\
	architecture.hh			\
	ast.hh					\
	ast_evaluate.hh			\
	ast_impl.hh				\
	ast_processor.hh			\
	ast_processor_impl.hh			\
	builtins.hh				\
	callables_wrapper.hh			\
	code_remark.hh				\
	code_remarks.hh			\
	constexpr_value.hh			\
	cow_set.hh				\
	execution_charset_encoder.hh		\
	execution_charset_encoder_iconv.hh	\
	file_range.hh				\
	gnuc_parser_driver.hh			\
	header_inclusion_tree.hh		\
	header_resolver.hh			\
	macro.hh				\
	macro_undef.hh				\
	mp_arithmetic.hh			\
	offset_to_line_col_map.hh		\
	parse_except.hh			\
	path.hh				\
	pp_except.hh				\
	pp_token.hh				\
	pp_tokens.hh				\
	pp_tokenizer.hh			\
	preprocessor.hh			\
	ret_type_invoker.hh			\
	semantic_except.hh			\
	source_reader.hh			\
	target_float.hh			\
	target_int.hh				\
	type_set.hh				\
	types.hh				\
	types_impl.hh				\
	used_macros.hh				\
	used_macro_undefs.hh

noinst_LIBRARIES = libcp.a
libcp_a_SOURCES =				\
	arch_gcc48_x86_64.cc			\
	architecture.cc			\
	ast.cc					\
	ast_evaluate.cc			\
	ast_resolve.cc				\
	builtins.cc				\
	code_remark.cc				\
	code_remarks.cc			\
	constexpr_value.cc			\
	execution_charset_encoder.cc		\
	execution_charset_encoder_iconv.cc	\
	file_range.cc				\
	gnuc_parser.yy				\
	gnuc_parser_driver.cc			\
	header_inclusion_tree.cc		\
	header_resolver.cc			\
	macro.cc				\
	macro_undef.cc				\
	mp_arithmetic.cc			\
	offset_to_line_col_map.cc		\
	parse_except.cc			\
	path.cc				\
	pp_except.cc				\
	pp_token.cc				\
	pp_tokens.cc				\
	pp_tokenizer.cc			\
	preprocessor.cc			\
	semantic_except.cc			\
	source_reader.cc			\
	target_float.cc			\
	target_int.cc				\
	types.cc				\
	used_macros.cc				\
	used_macro_undefs.cc

check_PROGRAMS = cpp_tokenize normalize_path dirname cpp cparse \
	test_mp_arithmetic test_target_int test_target_float

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

test_mp_arithmetic_SOURCES = test_mp_arithmetic.cc
test_mp_arithmetic_LDADD = libcp.a

test_target_int_SOURCES = test_target_int.cc
test_target_int_LDADD = libcp.a

test_target_float_SOURCES = test_target_float.cc
test_target_float_LDADD = libcp.a
