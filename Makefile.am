SUBDIRS = . arch testsuite

AM_YFLAGS = -d
BUILT_SOURCES = gnuc_parser.hh pp_expr_parser.hh stack.hh
EXTRA_DIST = stack.hh

stack.hh: gnuc_parser.hh pp_expr_parser.hh


noinst_HEADERS =				\
	architecture.hh			\
	arch_x86_64_gcc.hh			\
	ast.hh					\
	ast_evaluate.hh			\
	ast_impl.hh				\
	ast_processor.hh			\
	ast_processor_impl.hh			\
	builtins.hh				\
	callables_wrapper.hh			\
	cmdline_except.hh			\
	code_remark.hh				\
	code_remarks.hh			\
	constexpr_value.hh			\
	create_lp.hh				\
	depreprocessor.hh			\
	execution_charset_encoder.hh		\
	execution_charset_encoder_iconv.hh	\
	gnuc_parser_driver.hh			\
	header_resolver.hh			\
	lp_creation_policy.hh			\
	lp_except.hh				\
	mp_arithmetic.hh			\
	offset_to_line_col_map.hh		\
	output_remark.hh			\
	output_remarks.hh			\
	parse_except.hh			\
	path.hh				\
	pp_except.hh				\
	pp_expr_parser_driver.hh		\
	pp_result.hh				\
	pp_token.hh				\
	pp_tokens.hh				\
	pp_tokens_range.hh			\
	pp_tokenizer.hh			\
	preprocessor.hh			\
	range_in_file.hh			\
	raw_pp_token.hh			\
	raw_pp_tokens.hh			\
	ret_type_invoker.hh			\
	semantic_except.hh			\
	source_reader.hh			\
	source_writer.hh			\
	target_float.hh			\
	target_int.hh				\
	type_set.hh				\
	types.hh				\
	types_impl.hh

noinst_LIBRARIES = libcp.a
libcp_a_SOURCES =				\
	architecture.cc			\
	arch_x86_64_gcc.cc			\
	ast.cc					\
	ast_evaluate.cc			\
	ast_resolve.cc				\
	builtins.cc				\
	cmdline_except.cc			\
	code_remark.cc				\
	code_remarks.cc			\
	constexpr_value.cc			\
	create_lp.cc				\
	depreprocessor.cc			\
	execution_charset_encoder.cc		\
	execution_charset_encoder_iconv.cc	\
	gnuc_parser.yy				\
	gnuc_parser_driver.cc			\
	header_resolver.cc			\
	lp_creation_policy.cc	\
	lp_except.cc				\
	mp_arithmetic.cc			\
	offset_to_line_col_map.cc		\
	output_remark.cc			\
	output_remarks.cc			\
	parse_except.cc			\
	path.cc				\
	pp_except.cc				\
	pp_expr_parser.yy			\
	pp_expr_parser_driver.cc		\
	pp_result.cc				\
	pp_token.cc				\
	pp_tokens.cc				\
	pp_tokens_range.cc			\
	pp_tokenizer.cc			\
	preprocessor.cc			\
	raw_pp_token.cc			\
	semantic_except.cc			\
	source_reader.cc			\
	source_writer.cc			\
	target_float.cc			\
	target_int.cc				\
	types.cc

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
