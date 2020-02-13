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
  "-all-warnings",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wall" },
},
{
  "-ansi",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "ansi" },
},
{
  "-assert",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "A" },
},
{
  "-assert=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "A" },
},
{
  "-comments",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "C" },
},
{
  "-comments-in-macros",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "CC" },
},
{
  "-define-macro",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "D" },
},
{
  "-define-macro=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "D" },
},
{
  "-dependencies",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "M" },
},
{
  "-dump",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "d" },
},
{
  "-dump=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "d" },
},
{
  "-imacros",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "imacros" },
},
{
  "-imacros=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "imacros" },
},
{
  "-include",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "include" },
},
{
  "-include-barrier",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "I", "-" },
},
{
  "-include-directory",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "I" },
},
{
  "-include-directory-after",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "idirafter" },
},
{
  "-include-directory-after=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "idirafter" },
},
{
  "-include-directory=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "I" },
},
{
  "-include-prefix",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "iprefix" },
},
{
  "-include-prefix=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = false,
  .alias = { "iprefix" },
},
{
  "-include-with-prefix",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "iwithprefix" },
},
{
  "-include-with-prefix-after",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "iwithprefix" },
},
{
  "-include-with-prefix-after=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = false,
  .alias = { "iwithprefix" },
},
{
  "-include-with-prefix-before",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "iwithprefixbefore" },
},
{
  "-include-with-prefix-before=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = false,
  .alias = { "iwithprefixbefore" },
},
{
  "-include-with-prefix=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = false,
  .alias = { "iwithprefix" },
},
{
  "-include=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "include" },
},
{
  "-no-integrated-cpp",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "no-integrated-cpp" },
},
{
  "-no-line-commands",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "P" },
},
{
  "-no-standard-includes",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "nostdinc" },
},
{
  "-no-warnings",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "w" },
},
{
  "-output",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "o" },
},
{
  "-output-pch=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "-output=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "o" },
},
{
  "-pedantic",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "pedantic" },
},
{
  "-preprocess",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "E" },
},
{
  "-print-missing-file-dependencies",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "MG" },
},
{
  "-trace-includes",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "H" },
},
{
  "-traditional",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "traditional" },
},
{
  "-traditional-cpp",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "traditional-cpp" },
},
{
  "-trigraphs",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "trigraphs" },
},
{
  "-undefine-macro",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "U" },
},
{
  "-undefine-macro=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = { "U" },
},
{
  "-user-dependencies",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "MM" },
},
{
  "-verbose",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_common |
   gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "v" },
},
{
  "-write-dependencies",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "MD" },
},
{
  "-write-user-dependencies",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
  .reject_negative = false,
  .alias = { "MMD" },
},
{
  "A",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "C",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "CC",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "D",
  opt_code_c_family_D,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "E",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "F",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_driver |
   gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "H",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "I",
  opt_code_c_family_I,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "M",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "MD",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
},
{
  "MF",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "MG",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "MM",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "MMD",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
},
{
  "MP",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "MQ",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "MT",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "P",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "U",
  opt_code_c_family_U,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "Wabi",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wabi-tag",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wabi=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wabsolute-value",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Waddress",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Waddress-of-packed-member",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Waligned-new",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Waligned-new=", "global", "none" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Waligned-new=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wall",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Walloc-size-larger-than=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Walloc-zero",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Walloca",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Walloca-larger-than=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
},
{
  "Walloca-larger-than=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Warray-bounds",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Warray-bounds=",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_none,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wassign-intercept",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wbad-function-cast",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wbool-compare",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wbool-operation",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wbuiltin-declaration-mismatch",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wbuiltin-macro-redefined",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wc++-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wc++0x-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "Wc++0x-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++11-compat" },
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wc++11-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++0x-compat" },
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "Wc++11-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wc++14-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wc++17-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++1z-compat" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "Wc++17-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wc++1z-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "Wc++1z-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++17-compat" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wc11-c2x-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wc90-c99-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wc99-c11-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wcast-function-type",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wcast-qual",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wcatch-value",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wcatch-value=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wcatch-value=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wchar-subscripts",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wchkp",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wclass-conversion",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wclass-memaccess",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wclobbered",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wcomment",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wcomments",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wcomment" },
},
{
  "Wconditionally-supported",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  "Wconversion",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wconversion-null",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wcpp",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wctor-dtor-privacy",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdangling-else",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wdate-time",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_common,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  "Wdate-time",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wdeclaration-after-statement",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdelete-incomplete",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  "Wdelete-non-virtual-dtor",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdeprecated",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdeprecated-copy",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wdeprecated-copy-dtor",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wdesignated-init",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wdiscarded-array-qualifiers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wdiscarded-qualifiers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wdiv-by-zero",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdouble-promotion",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wduplicate-decl-specifier",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wduplicated-branches",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wduplicated-cond",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Weffc++",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wempty-body",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wendif-labels",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wenum-compare",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Werror",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Werror-implicit-function-declaration",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = { "Werror=", "implicit-function-declaration" },
},
{
  "Wexpansion-to-defined",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wextra",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wextra-semi",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wfloat-conversion",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  "Wfloat-equal",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wformat=", "1", "0" },
},
{
  "Wformat-contains-nul",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-extra-args",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-nonliteral",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-overflow",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wformat-overflow=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wformat-overflow=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wformat-security",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-signedness",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wformat-truncation",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wformat-truncation=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wformat-truncation=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wformat-y2k",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-zero-length",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "Wframe-address",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wif-not-aligned",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wignored-attributes",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wignored-qualifiers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wimplicit",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wimplicit-fallthrough=",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_none,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wimplicit-function-declaration",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wimplicit-int",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wimport",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wincompatible-pointer-types",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Winherited-variadic-ctor",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Winit-list-lifetime",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Winit-self",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wint-conversion",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wint-in-bool-context",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wint-to-pointer-cast",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Winvalid-offsetof",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Winvalid-pch",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wjump-misses-init",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wliteral-suffix",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wlogical-not-parentheses",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wlogical-op",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wlong-long",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmain",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmaybe-uninitialized",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmemset-elt-size",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wmemset-transposed-args",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wmisleading-indentation",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_common),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wmissing-attributes",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wmissing-braces",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-declarations",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-field-initializers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-format-attribute",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wsuggest-attribute=format" },
},
{
  "Wmissing-include-dirs",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-parameter-type",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-prototypes",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmudflap",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmultichar",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmultiple-inheritance",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wmultistatement-macros",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wnamespaces",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wnarrowing",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "Wnested-externs",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wno-alloc-size-larger-than",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Walloc-size-larger-than=", "18446744073709551615EiB", "none" },
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wno-alloca-larger-than",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Walloca-larger-than=", "18446744073709551615EiB", "none" },
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wno-vla-larger-than",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wvla-larger-than=", "18446744073709551615EiB", "none" },
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wnoexcept",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnoexcept-type",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wnon-template-friend",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnon-virtual-dtor",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnonnull",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnonnull-compare",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wnormalized",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wnormalized=", "nfc", "none" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wnormalized=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "Wnormalized=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wold-style-cast",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wold-style-declaration",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wold-style-definition",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wopenmp-simd",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  "Woverlength-strings",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Woverloaded-virtual",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Woverride-init",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Woverride-init-side-effects",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wpacked-bitfield-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpacked-not-aligned",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wparentheses",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpedantic",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpessimizing-move",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wplacement-new",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_cxx,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wplacement-new=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wplacement-new=",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_cxx,
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wpmf-conversions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpointer-arith",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpointer-compare",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wpointer-sign",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpointer-to-int-cast",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpragmas",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wprio-ctor-dtor",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wproperty-assign-default",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wprotocol",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpsabi",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wredundant-decls",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wredundant-move",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wregister",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wreorder",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wrestrict",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wreturn-local-addr",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "Wreturn-type",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wscalar-storage-order",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wselector",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsequence-point",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wshadow-ivar",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wshift-count-negative",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wshift-count-overflow",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wshift-negative-value",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wshift-overflow",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wshift-overflow=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wshift-overflow=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wsign-compare",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsign-conversion",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsign-promo",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsized-deallocation",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wsizeof-array-argument",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wsizeof-pointer-div",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wsizeof-pointer-memaccess",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-aliasing=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-null-sentinel",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-overflow=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-prototypes",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-selector-match",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstringop-overflow",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wstringop-overflow=", "2", "0" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wstringop-overflow=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "Wstringop-truncation",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "Wsubobject-linkage",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wsuggest-attribute=format",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsuggest-override",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wswitch",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wswitch-bool",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "Wswitch-default",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wswitch-enum",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsync-nand",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsynth",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsystem-headers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wtautological-compare",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wtemplates",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wterminate",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wtraditional",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wtraditional-conversion",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wtrigraphs",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wundeclared-selector",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wundef",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wuninitialized",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunknown-pragmas",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunsuffixed-float-constants",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused-const-variable",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wunused-const-variable=", "2", "0" },
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wunused-const-variable=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wunused-local-typedefs",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused-macros",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused-result",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused-variable",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wuseless-cast",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvarargs",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvariadic-macros",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvirtual-inheritance",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "Wvirtual-move-assign",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvla",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvla-larger-than=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
},
{
  "Wvla-larger-than=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "Wvolatile-register-var",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wwrite-strings",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wzero-as-null-pointer-constant",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ansi",
  opt_code_c_family_ansi,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "d",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "fabi-compat-version=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "faccess-control",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fada-spec-parent=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "faligned-new",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "faligned-new=", "1", "0" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "faligned-new=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "fall-virtual",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fallow-parameterless-variadic-functions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "falt-external-templates",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fasm",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fbuilding-libgcc",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fbuiltin",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fbuiltin-",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "fcanonical-system-headers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fchar8_t",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "fcheck-new",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  "fcheck-pointer-bounds",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-check-incomplete-type",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-check-read",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-check-write",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-first-field-has-own-bounds",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-flexible-struct-trailing-arrays",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "fchkp-instrument-calls",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-instrument-marked-only",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-narrow-bounds",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-narrow-to-innermost-array",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-optimize",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-store-bounds",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-treat-zero-dynamic-size-as-infinite",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-fast-string-functions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-nochk-string-functions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-static-bounds",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-static-const-bounds",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-wrappers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-zero-input-bounds-for-main",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fcilkplus",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  "fcond-mismatch",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fconserve-space",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fconstant-string-class=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "fconstexpr-depth=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fconstexpr-ops-limit=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "fdebug-cpp",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdeclone-ctor-dtor",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{  "fdeduce-init-list",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdefault-inline",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdiagnostics-show-template-tree",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "fdirectives-only",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdollars-in-identifiers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdump-ada-spec",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "fdump-ada-spec-slim",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "felide-constructors",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "felide-type",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "femit-struct-debug-baseonly",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "femit-struct-debug-detailed=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "femit-struct-debug-reduced",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fenforce-eh-specs",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fenum-int-equiv",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fexec-charset=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fext-numeric-literals",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fextended-identifiers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fextern-tls-init",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fexternal-templates",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ffor-scope",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ffreestanding",
  opt_code_c_family_ffreestanding,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ffriend-injection",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fgimple",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_c,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "fgnu-keywords",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fgnu-runtime",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "fgnu89-inline",
  opt_code_c_family_fgnu89_inline,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fguiding-decls",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fhandle-exceptions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "fexceptions" },
},
{
  "fhonor-std",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fhosted",
  opt_code_c_family_fhosted,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fhuge-objects",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fimplement-inlines",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fimplicit-inline-templates",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fimplicit-templates",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "finput-charset=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fivar-visibility=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fkeep-inline-dllexport",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "flabels-ok",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "flax-vector-conversions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "flocal-ivars",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fmacro-prefix-map=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "fms-extensions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fmudflap",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  "fmudflap",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  "fmudflapir",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  "fmudflapir",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  "fmudflapth",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  "fmudflapth",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 0},
},
{
  "fname-mangling-version-",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "fnew-abi",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fnew-inheriting-ctors",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "fnew-ttp-matching",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "fnext-runtime",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "fnil-receivers",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fnonansi-builtins",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fnonnull-objects",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fnothrow-opt",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-abi-version=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fobjc-call-cxx-cdtors",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_objcxx,
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-direct-dispatch",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-exceptions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-gc",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-nilcheck",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-sjlj-exceptions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-std=objc1",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fopenacc",
  opt_code_c_family_fopenacc,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fopenacc-dim=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
},
{
  "fopenmp",
  opt_code_c_family_fopenmp,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fopenmp-simd",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
},
{
  "foperator-names",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "foptional-diags",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpch-deps",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpch-preprocess",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpermissive",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fplan9-extensions",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpreprocessed",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpretty-templates",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fprintf-return-value",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "freplace-objc-classes",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "frepo",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "frtti",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fshort-double",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "fshort-enums",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fshort-wchar",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fsigned-bitfields",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fsigned-char",
  opt_code_c_family_fsigned_char,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fsized-deallocation",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "fsquangle",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fsso-struct=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {6, 1, 0},
  .max_gcc_version = {6, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "fsso-struct=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "fstats",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fstrict-enums",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fstrict-prototype",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fstrong-eval-order",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx |
   gcc_cmdline_parser::option::comp_common),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "fstrong-eval-order=", "all", "none" },
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "fstrong-eval-order=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx |
   gcc_cmdline_parser::option::comp_common),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "ftabstop=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "ftemplate-backtrace-limit=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "ftemplate-depth-",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = { "ftemplate-depth=" },
},
{
  "ftemplate-depth=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fthis-is-variable",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fthreadsafe-statics",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ftrack-macro-expansion",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = true,
},
{
  "ftrack-macro-expansion=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = true,
},
{
  "funsigned-bitfields",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "funsigned-char",
  opt_code_c_family_funsigned_char,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fuse-cxa-atexit",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fuse-cxa-get-exception-ptr",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fvisibility-inlines-hidden",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fvisibility-ms-compat",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fvtable-gc",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fvtable-thunks",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fweak",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fwide-exec-charset=",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fworking-directory",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fxref",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fzero-link",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "gen-decls",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx |
   gcc_cmdline_parser::option::comp_driver),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {6, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "gen-decls",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  "idirafter",
  opt_code_c_family_idirafter,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "imacros",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "imultilib",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "include",
  opt_code_c_family_include,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "iprefix",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "iquote",
  opt_code_c_family_iquote,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "isysroot",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "isystem",
  opt_code_c_family_isystem,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "iwithprefix",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "iwithprefixbefore",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "lang-asm",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_c,
  gcc_cmdline_parser::option::arg_none,
},
{
  "no-integrated-cpp",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
},
{
  "nostdinc",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "nostdinc++",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "o",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "pedantic",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wpedantic" },
},
{
  "print-objc-runtime-info",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "remap",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "static-libmpx",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "static-libmpxwrappers",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "std=c++03",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++98" },
},
{
  "std=c++0x",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++11" },
},
{
  "std=c++11",
  opt_code_c_family_std_cxx11,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c++14",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++1y" },
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  "std=c++14",
  opt_code_c_family_std_cxx14,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "std=c++17",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++1z" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "std=c++17",
  opt_code_c_family_std_cxx17,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c++1y",
  opt_code_c_family_std_cxx1y,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "std=c++1y",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++14" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "std=c++1z",
  opt_code_c_family_std_cxx1z,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "std=c++1z",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c++2a",
  opt_code_c_family_std_cxx2a,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c++98",
  opt_code_c_family_std_cxx98,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c11",
  opt_code_c_family_std_c11,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c17",
  opt_code_c_family_std_c17,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c18",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c1x",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c11" },
},
{
  "std=c2x",
  opt_code_c_family_std_c2x,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "std=c89",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c90" },
},
{
  "std=c90",
  opt_code_c_family_std_c90,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c99",
  opt_code_c_family_std_c99,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c9x",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  "std=gnu++03",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++98" },
},
{
  "std=gnu++0x",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++11" },
},
{
  "std=gnu++11",
  opt_code_c_family_std_gnuxx11,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu++14",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++1y" },
  .negative = nullptr,
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  "std=gnu++14",
  opt_code_c_family_std_gnuxx14,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "std=gnu++17",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++1z" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "std=gnu++17",
  opt_code_c_family_std_gnuxx17,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu++1y",
  opt_code_c_family_std_gnuxx1y,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "std=gnu++1y",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "gnu++14" },
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
},
{
  "std=gnu++1z",
  opt_code_c_family_std_gnuxx1z,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "std=gnu++1z",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu++2a",
  opt_code_c_family_std_gnuxx2a,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu++98",
  opt_code_c_family_std_gnuxx98,
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu11",
  opt_code_c_family_std_gnu11,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu17",
  opt_code_c_family_std_gnu17,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu18",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu1x",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu11" },
},
{
  "std=gnu2x",
  opt_code_c_family_std_gnu2x,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {9, 1, 0},
},
{
  "std=gnu89",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu90" },
},
{
  "std=gnu90",
  opt_code_c_family_std_gnu90,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu99",
  opt_code_c_family_std_gnu99,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu9x",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu99" },
},
{
  "std=iso9899:1990",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c90" },
},
{
  "std=iso9899:199409",
  opt_code_c_family_std_iso9899_199409,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=iso9899:1999",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  "std=iso9899:199x",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  "std=iso9899:2011",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c11" },
},
{
  "std=iso9899:2017",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "std=iso9899:2018",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .negative = nullptr,
  .min_gcc_version = {8, 1, 0},
},
{
  "traditional",
  opt_code_c_family_unused,
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
},
{
  "traditional-cpp",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "trigraphs",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "undef",
  opt_code_c_family_undef,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "v",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "w",
  opt_code_c_family_unused,
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
