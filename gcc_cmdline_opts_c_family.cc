/*
 * Copyright (C) 2019  SUSE Software Solutions Germany GmbH
 *
 * This file is part of klp-ccp.
 *
 * klp-ccp is free software: you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * klp-ccp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
 */

{
  .name = "-all-warnings",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wall" },
},
{
  .name = "-ansi",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "ansi" },
},
{
  .name = "-assert",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "A" },
},
{
  .name = "-assert=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "A" },
},
{
  .name = "-comments",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "C" },
},
{
  .name = "-comments-in-macros",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "CC" },
},
{
  .name = "-define-macro",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "D" },
},
{
  .name = "-define-macro=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "D" },
},
{
  .name = "-dependencies",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "M" },
},
{
  .name = "-dump",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "d" },
},
{
  .name = "-dump=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "d" },
},
{
  .name = "-imacros",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "imacros" },
},
{
  .name = "-imacros=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "imacros" },
},
{
  .name = "-include",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "include" },
},
{
  .name = "-include-barrier",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "I", "-" },
},
{
  .name = "-include-directory",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "I" },
},
{
  .name = "-include-directory-after",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "idirafter" },
},
{
  .name = "-include-directory-after=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "idirafter" },
},
{
  .name = "-include-directory=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "I" },
},
{
  .name = "-include-prefix",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "iprefix" },
},
{
  .name = "-include-prefix=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = false,
  .alias = { "iprefix" },
},
{
  .name = "-include-with-prefix",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "iwithprefix" },
},
{
  .name = "-include-with-prefix-after",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "iwithprefix" },
},
{
  .name = "-include-with-prefix-after=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = false,
  .alias = { "iwithprefix" },
},
{
  .name = "-include-with-prefix-before",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "iwithprefixbefore" },
},
{
  .name = "-include-with-prefix-before=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = false,
  .alias = { "iwithprefixbefore" },
},
{
  .name = "-include-with-prefix=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = false,
  .alias = { "iwithprefix" },
},
{
  .name = "-include=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "include" },
},
{
  .name = "-no-integrated-cpp",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_driver,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "no-integrated-cpp" },
},
{
  .name = "-no-line-commands",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "P" },
},
{
  .name = "-no-standard-includes",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "nostdinc" },
},
{
  .name = "-no-warnings",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "w" },
},
{
  .name = "-output",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "o" },
},
{
  .name = "-output-pch=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "-output=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "o" },
},
{
  .name = "-pedantic",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "pedantic" },
},
{
  .name = "-preprocess",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "E" },
},
{
  .name = "-print-missing-file-dependencies",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "MG" },
},
{
  .name = "-trace-includes",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "H" },
},
{
  .name = "-traditional",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_driver,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "traditional" },
},
{
  .name = "-traditional-cpp",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "traditional-cpp" },
},
{
  .name = "-trigraphs",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "trigraphs" },
},
{
  .name = "-undefine-macro",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "U" },
},
{
  .name = "-undefine-macro=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "U" },
},
{
  .name = "-user-dependencies",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "MM" },
},
{
  .name = "-verbose",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_common |
	   gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "v" },
},
{
  .name = "-write-dependencies",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "MD" },
},
{
  .name = "-write-user-dependencies",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "MMD" },
},
{
  .name = "A",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "C",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "CC",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "D",
  .code = opt_code_c_family_D,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "E",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "F",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_driver |
	   gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "H",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "I",
  .code = opt_code_c_family_I,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "M",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "MD",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
},
{
  .name = "MF",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "MG",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "MM",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "MMD",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_separate,
},
{
  .name = "MP",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "MQ",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "MT",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "P",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "U",
  .code = opt_code_c_family_U,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "Wabi",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wabi-tag",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wabi=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wabsolute-value",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Waddress",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Waddress-of-packed-member",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Waligned-new",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Waligned-new=", "global", "none" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Waligned-new=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wall",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Walloc-size-larger-than=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Walloc-zero",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Walloca",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Walloca-larger-than=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "Walloca-larger-than=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Warray-bounds",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Warray-bounds=",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_none,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wassign-intercept",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wbad-function-cast",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wbool-compare",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wbool-operation",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wbuiltin-declaration-mismatch",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wbuiltin-macro-redefined",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wc++-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wc++0x-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "Wc++0x-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++11-compat" },
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wc++11-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++0x-compat" },
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "Wc++11-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wc++14-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wc++17-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++1z-compat" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "Wc++17-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wc++1z-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "Wc++1z-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++17-compat" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wc11-c2x-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wc90-c99-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wc99-c11-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wcast-function-type",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wcast-qual",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wcatch-value",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wcatch-value=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wcatch-value=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wchar-subscripts",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wchkp",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wclass-conversion",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wclass-memaccess",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wclobbered",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wcomment",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wcomments",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wcomment" },
},
{
  .name = "Wconditionally-supported",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "Wconversion",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wconversion-null",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wcpp",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wctor-dtor-privacy",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wdangling-else",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wdate-time",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_common,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  .name = "Wdate-time",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wdeclaration-after-statement",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wdelete-incomplete",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "Wdelete-non-virtual-dtor",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wdeprecated",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wdeprecated-copy",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wdeprecated-copy-dtor",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wdesignated-init",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wdiscarded-array-qualifiers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wdiscarded-qualifiers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wdiv-by-zero",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wdouble-promotion",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wduplicate-decl-specifier",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wduplicated-branches",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wduplicated-cond",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Weffc++",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wempty-body",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wendif-labels",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wenum-compare",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Werror",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Werror-implicit-function-declaration",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = { "Werror=", "implicit-function-declaration" },
},
{
  .name = "Wexpansion-to-defined",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wextra",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wextra-semi",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wfloat-conversion",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "Wfloat-equal",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wformat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wformat=", "1", "0" },
},
{
  .name = "Wformat-contains-nul",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wformat-extra-args",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wformat-nonliteral",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wformat-overflow",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wformat-overflow=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wformat-overflow=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wformat-security",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wformat-signedness",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wformat-truncation",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wformat-truncation=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wformat-truncation=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wformat-y2k",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wformat-zero-length",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wformat=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "Wframe-address",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wif-not-aligned",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wignored-attributes",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_cxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wignored-qualifiers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_cxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wimplicit",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wimplicit-fallthrough=",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_none,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wimplicit-function-declaration",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wimplicit-int",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wimport",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wincompatible-pointer-types",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Winherited-variadic-ctor",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Winit-list-lifetime",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Winit-self",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wint-conversion",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wint-in-bool-context",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wint-to-pointer-cast",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Winvalid-offsetof",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Winvalid-pch",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wjump-misses-init",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wliteral-suffix",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wlogical-not-parentheses",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wlogical-op",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wlong-long",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmain",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmaybe-uninitialized",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmemset-elt-size",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wmemset-transposed-args",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wmisleading-indentation",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_common),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wmissing-attributes",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wmissing-braces",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmissing-declarations",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmissing-field-initializers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmissing-format-attribute",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wsuggest-attribute=format" },
},
{
  .name = "Wmissing-include-dirs",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmissing-parameter-type",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmissing-prototypes",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmudflap",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmultichar",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wmultiple-inheritance",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wmultistatement-macros",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wnamespaces",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wnarrowing",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  .name = "Wnested-externs",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wno-alloc-size-larger-than",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Walloc-size-larger-than=", "18446744073709551615EiB", "none" },
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wno-alloca-larger-than",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Walloca-larger-than=", "18446744073709551615EiB", "none" },
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wno-vla-larger-than",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wvla-larger-than=", "18446744073709551615EiB", "none" },
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wnoexcept",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wnoexcept-type",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wnon-template-friend",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wnon-virtual-dtor",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wnonnull",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wnonnull-compare",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wnormalized",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wnormalized=", "nfc", "none" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wnormalized=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  .name = "Wnormalized=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wold-style-cast",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wold-style-declaration",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wold-style-definition",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wopenmp-simd",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_cxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "Woverlength-strings",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Woverloaded-virtual",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Woverride-init",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Woverride-init-side-effects",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wpacked-bitfield-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wpacked-not-aligned",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wparentheses",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wpedantic",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wpessimizing-move",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wplacement-new",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_cxx,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wplacement-new=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wplacement-new=",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_cxx,
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wpmf-conversions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wpointer-arith",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wpointer-compare",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wpointer-sign",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wpointer-to-int-cast",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wpragmas",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wprio-ctor-dtor",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wproperty-assign-default",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wprotocol",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wpsabi",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wredundant-decls",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wredundant-move",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wregister",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wreorder",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wrestrict",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wreturn-local-addr",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  .name = "Wreturn-type",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wscalar-storage-order",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wselector",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wsequence-point",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wshadow-ivar",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wshift-count-negative",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wshift-count-overflow",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wshift-negative-value",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wshift-overflow",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wshift-overflow=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wshift-overflow=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wsign-compare",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wsign-conversion",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wsign-promo",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wsized-deallocation",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wsizeof-array-argument",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wsizeof-pointer-div",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wsizeof-pointer-memaccess",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wstrict-aliasing=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wstrict-null-sentinel",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wstrict-overflow=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wstrict-prototypes",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wstrict-selector-match",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wstringop-overflow",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wstringop-overflow=", "2", "0" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wstringop-overflow=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "Wstringop-truncation",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "Wsubobject-linkage",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wsuggest-attribute=format",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wsuggest-override",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wswitch",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wswitch-bool",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "Wswitch-default",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wswitch-enum",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wsync-nand",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_cxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wsynth",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wsystem-headers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wtautological-compare",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wtemplates",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wterminate",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wtraditional",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wtraditional-conversion",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wtrigraphs",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wundeclared-selector",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wundef",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wuninitialized",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wunknown-pragmas",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wunsuffixed-float-constants",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wunused",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wunused-const-variable",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wunused-const-variable=", "2", "0" },
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wunused-const-variable=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wunused-local-typedefs",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wunused-macros",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wunused-result",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wunused-variable",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wuseless-cast",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wvarargs",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wvariadic-macros",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wvirtual-inheritance",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "Wvirtual-move-assign",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wvla",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wvla-larger-than=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "Wvla-larger-than=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "Wvolatile-register-var",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wwrite-strings",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "Wzero-as-null-pointer-constant",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "ansi",
  .code = opt_code_c_family_ansi,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "d",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
},
{
  .name = "fabi-compat-version=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "faccess-control",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fada-spec-parent=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "faligned-new",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "faligned-new=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "faligned-new=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "fall-virtual",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fallow-parameterless-variadic-functions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "falt-external-templates",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fasm",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fbuilding-libgcc",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fbuiltin",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fbuiltin-",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
},
{
  .name = "fcanonical-system-headers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fchar8_t",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "fcheck-new",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  .name = "fcheck-pointer-bounds",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-check-incomplete-type",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-check-read",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-check-write",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-first-field-has-own-bounds",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-flexible-struct-trailing-arrays",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "fchkp-instrument-calls",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-instrument-marked-only",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-narrow-bounds",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-narrow-to-innermost-array",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-optimize",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-store-bounds",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-treat-zero-dynamic-size-as-infinite",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-use-fast-string-functions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-use-nochk-string-functions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-use-static-bounds",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-use-static-const-bounds",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-use-wrappers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fchkp-zero-input-bounds-for-main",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fcilkplus",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "fcond-mismatch",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fconserve-space",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fconstant-string-class=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
},
{
  .name = "fconstexpr-depth=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "fconstexpr-ops-limit=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "fdebug-cpp",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fdeclone-ctor-dtor",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "fdeduce-init-list",
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fdefault-inline",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fdiagnostics-show-template-tree",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "fdirectives-only",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fdollars-in-identifiers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fdump-ada-spec",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  .name = "fdump-ada-spec-slim",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  .name = "felide-constructors",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "felide-type",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "femit-struct-debug-baseonly",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "femit-struct-debug-detailed=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
},
{
  .name = "femit-struct-debug-reduced",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fenforce-eh-specs",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fenum-int-equiv",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fexec-charset=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "fext-numeric-literals",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fextended-identifiers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fextern-tls-init",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fexternal-templates",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "ffor-scope",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "ffreestanding",
  .code = opt_code_c_family_ffreestanding,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "ffriend-injection",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fgimple",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_c,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "fgnu-keywords",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fgnu-runtime",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  .name = "fgnu89-inline",
  .code = opt_code_c_family_fgnu89_inline,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fguiding-decls",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fhandle-exceptions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "fexceptions" },
},
{
  .name = "fhonor-std",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fhosted",
  .code = opt_code_c_family_fhosted,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fhuge-objects",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fimplement-inlines",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fimplicit-inline-templates",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fimplicit-templates",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "finput-charset=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "fivar-visibility=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fkeep-inline-dllexport",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "flabels-ok",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "flax-vector-conversions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "flocal-ivars",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fmacro-prefix-map=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "fms-extensions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fmudflap",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  .name = "fmudflap",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "fmudflapir",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  .name = "fmudflapir",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "fmudflapth",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  .name = "fmudflapth",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 0},
},
{
  .name = "fname-mangling-version-",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
},
{
  .name = "fnew-abi",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fnew-inheriting-ctors",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "fnew-ttp-matching",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "fnext-runtime",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  .name = "fnil-receivers",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fnonansi-builtins",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fnonnull-objects",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fnothrow-opt",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fobjc-abi-version=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "fobjc-call-cxx-cdtors",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_objcxx,
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fobjc-direct-dispatch",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fobjc-exceptions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fobjc-gc",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fobjc-nilcheck",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fobjc-sjlj-exceptions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fobjc-std=objc1",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fopenacc",
  .code = opt_code_c_family_fopenacc,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fopenacc-dim=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  .name = "fopenmp",
  .code = opt_code_c_family_fopenmp,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fopenmp-simd",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  .name = "foperator-names",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "foptional-diags",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fpch-deps",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fpch-preprocess",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fpermissive",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fplan9-extensions",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fpreprocessed",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fpretty-templates",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fprintf-return-value",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "freplace-objc-classes",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "frepo",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "frtti",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fshort-double",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "fshort-enums",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fshort-wchar",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fsigned-bitfields",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fsigned-char",
  .code = opt_code_c_family_fsigned_char,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fsized-deallocation",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "fsquangle",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fsso-struct=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
  .max_gcc_version = {6, 5, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "fsso-struct=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "fstats",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fstrict-enums",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fstrict-prototype",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fstrong-eval-order",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx |
	   gcc_cmdline_parser::option::comp_common),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "fstrong-eval-order=", "all", "none" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "fstrong-eval-order=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx |
	   gcc_cmdline_parser::option::comp_common),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "ftabstop=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "ftemplate-backtrace-limit=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "ftemplate-depth-",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = { "ftemplate-depth=" },
},
{
  .name = "ftemplate-depth=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "fthis-is-variable",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fthreadsafe-statics",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "ftrack-macro-expansion",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = true,
},
{
  .name = "ftrack-macro-expansion=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = true,
},
{
  .name = "funsigned-bitfields",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "funsigned-char",
  .code = opt_code_c_family_funsigned_char,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fuse-cxa-atexit",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fuse-cxa-get-exception-ptr",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fvisibility-inlines-hidden",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fvisibility-ms-compat",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fvtable-gc",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fvtable-thunks",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fweak",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fwide-exec-charset=",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "fworking-directory",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fxref",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "fzero-link",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "gen-decls",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx |
	   gcc_cmdline_parser::option::comp_driver),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {6, 5, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "gen-decls",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "idirafter",
  .code = opt_code_c_family_idirafter,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "imacros",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "imultilib",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "include",
  .code = opt_code_c_family_include,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "iprefix",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "iquote",
  .code = opt_code_c_family_iquote,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "isysroot",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "isystem",
  .code = opt_code_c_family_isystem,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "iwithprefix",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "iwithprefixbefore",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "lang-asm",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_c,
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "no-integrated-cpp",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_driver,
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "nostdinc",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "nostdinc++",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "o",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = (gcc_cmdline_parser::option::arg_joined |
	  gcc_cmdline_parser::option::arg_separate),
},
{
  .name = "pedantic",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wpedantic" },
},
{
  .name = "print-objc-runtime-info",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "remap",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "static-libmpx",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_driver,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "static-libmpxwrappers",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_driver,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "std=c++03",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++98" },
},
{
  .name = "std=c++0x",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++11" },
},
{
  .name = "std=c++11",
  .code = opt_code_c_family_std_cxx11,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=c++14",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++1y" },
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  .name = "std=c++14",
  .code = opt_code_c_family_std_cxx14,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "std=c++17",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++1z" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "std=c++17",
  .code = opt_code_c_family_std_cxx17,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=c++1y",
  .code = opt_code_c_family_std_cxx1y,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  .name = "std=c++1y",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++14" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "std=c++1z",
  .code = opt_code_c_family_std_cxx1z,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "std=c++1z",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=c++2a",
  .code = opt_code_c_family_std_cxx2a,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=c++98",
  .code = opt_code_c_family_std_cxx98,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=c11",
  .code = opt_code_c_family_std_c11,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=c17",
  .code = opt_code_c_family_std_c17,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=c18",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=c1x",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c11" },
},
{
  .name = "std=c2x",
  .code = opt_code_c_family_std_c2x,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "std=c89",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c90" },
},
{
  .name = "std=c90",
  .code = opt_code_c_family_std_c90,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=c99",
  .code = opt_code_c_family_std_c99,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=c9x",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  .name = "std=gnu++03",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++98" },
},
{
  .name = "std=gnu++0x",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++11" },
},
{
  .name = "std=gnu++11",
  .code = opt_code_c_family_std_gnuxx11,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=gnu++14",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++1y" },
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  .name = "std=gnu++14",
  .code = opt_code_c_family_std_gnuxx14,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "std=gnu++17",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++1z" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "std=gnu++17",
  .code = opt_code_c_family_std_gnuxx17,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=gnu++1y",
  .code = opt_code_c_family_std_gnuxx1y,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  .name = "std=gnu++1y",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "gnu++14" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  .name = "std=gnu++1z",
  .code = opt_code_c_family_std_gnuxx1z,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  .name = "std=gnu++1z",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=gnu++2a",
  .code = opt_code_c_family_std_gnuxx2a,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=gnu++98",
  .code = opt_code_c_family_std_gnuxx98,
  .comp = (gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=gnu11",
  .code = opt_code_c_family_std_gnu11,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=gnu17",
  .code = opt_code_c_family_std_gnu17,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=gnu18",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=gnu1x",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu11" },
},
{
  .name = "std=gnu2x",
  .code = opt_code_c_family_std_gnu2x,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  .name = "std=gnu89",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu90" },
},
{
  .name = "std=gnu90",
  .code = opt_code_c_family_std_gnu90,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=gnu99",
  .code = opt_code_c_family_std_gnu99,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=gnu9x",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu99" },
},
{
  .name = "std=iso9899:1990",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c90" },
},
{
  .name = "std=iso9899:199409",
  .code = opt_code_c_family_std_iso9899_199409,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "std=iso9899:1999",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  .name = "std=iso9899:199x",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  .name = "std=iso9899:2011",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c11" },
},
{
  .name = "std=iso9899:2017",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "std=iso9899:2018",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc),
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  .name = "traditional",
  .code = opt_code_c_family_unused,
  .comp = gcc_cmdline_parser::option::comp_driver,
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "traditional-cpp",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "trigraphs",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "undef",
  .code = opt_code_c_family_undef,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "v",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "w",
  .code = opt_code_c_family_unused,
  .comp = (gcc_cmdline_parser::option::comp_c |
	   gcc_cmdline_parser::option::comp_objc |
	   gcc_cmdline_parser::option::comp_cxx |
	   gcc_cmdline_parser::option::comp_objcxx),
  .arg = gcc_cmdline_parser::option::arg_none,
},
