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
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "no-integrated-cpp" },
},
{
  "-no-line-commands",
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
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "-output=",
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
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "traditional" },
},
{
  "-traditional-cpp",
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
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "C",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "CC",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "D",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "E",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "F",
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
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "I",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "M",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "MD",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
},
{
  "MF",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "MG",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "MM",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "MMD",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_separate,
},
{
  "MP",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "MQ",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "MT",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "P",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "U",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "Wabi",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wabi-tag",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wabi=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wabsolute-value",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Waddress",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Waddress-of-packed-member",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Waligned-new",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Waligned-new=", "global", "none" },
  .min_gcc_version = {7, 1, 0},
},
{
  "Waligned-new=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wall",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Walloc-size-larger-than=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Walloc-zero",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Walloca",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Walloca-larger-than=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
},
{
  "Walloca-larger-than=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Warray-bounds",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Warray-bounds=",
  gcc_cmdline_parser::option::comp_none,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wassign-intercept",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wbad-function-cast",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wbool-compare",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wbool-operation",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wbuiltin-declaration-mismatch",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wbuiltin-macro-redefined",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wc++-compat",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wc++0x-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "Wc++0x-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++11-compat" },
  .min_gcc_version = {6, 1, 0},
},
{
  "Wc++11-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++0x-compat" },
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "Wc++11-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wc++14-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wc++17-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++1z-compat" },
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "Wc++17-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wc++1z-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "Wc++1z-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++17-compat" },
  .min_gcc_version = {8, 1, 0},
},
{
  "Wc11-c2x-compat",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Wc90-c99-compat",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wc99-c11-compat",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wcast-function-type",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wcast-qual",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wcatch-value",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wcatch-value=", "1", "0" },
  .min_gcc_version = {8, 1, 0},
},
{
  "Wcatch-value=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wchar-subscripts",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wchkp",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wclass-conversion",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Wclass-memaccess",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wclobbered",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wcomment",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wcomments",
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
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{
  "Wconversion",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wconversion-null",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wcpp",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wctor-dtor-privacy",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdangling-else",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wdate-time",
  gcc_cmdline_parser::option::comp_common,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  "Wdate-time",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wdeclaration-after-statement",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdelete-incomplete",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{
  "Wdelete-non-virtual-dtor",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdeprecated",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdeprecated-copy",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Wdeprecated-copy-dtor",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Wdesignated-init",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wdiscarded-array-qualifiers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wdiscarded-qualifiers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wdiv-by-zero",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdouble-promotion",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wduplicate-decl-specifier",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wduplicated-branches",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wduplicated-cond",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Weffc++",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wempty-body",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wendif-labels",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wenum-compare",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Werror",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Werror-implicit-function-declaration",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = { "Werror=", "implicit-function-declaration" },
},
{
  "Wexpansion-to-defined",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wextra",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wextra-semi",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wfloat-conversion",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{
  "Wfloat-equal",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat",
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
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-extra-args",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-nonliteral",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-overflow",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wformat-overflow=", "1", "0" },
  .min_gcc_version = {7, 1, 0},
},
{
  "Wformat-overflow=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wformat-security",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-signedness",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wformat-truncation",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wformat-truncation=", "1", "0" },
  .min_gcc_version = {7, 1, 0},
},
{
  "Wformat-truncation=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wformat-y2k",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat-zero-length",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wformat=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "Wframe-address",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wif-not-aligned",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wignored-attributes",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wignored-qualifiers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wimplicit",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wimplicit-fallthrough=",
  gcc_cmdline_parser::option::comp_none,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wimplicit-function-declaration",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wimplicit-int",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wimport",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wincompatible-pointer-types",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Winherited-variadic-ctor",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Winit-list-lifetime",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Winit-self",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wint-conversion",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wint-in-bool-context",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wint-to-pointer-cast",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Winvalid-offsetof",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Winvalid-pch",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wjump-misses-init",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wliteral-suffix",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wlogical-not-parentheses",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wlogical-op",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wlong-long",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmain",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmaybe-uninitialized",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmemset-elt-size",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wmemset-transposed-args",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wmisleading-indentation",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_common),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wmissing-attributes",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wmissing-braces",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-declarations",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-field-initializers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-format-attribute",
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
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-parameter-type",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmissing-prototypes",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmudflap",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmultichar",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wmultiple-inheritance",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wmultistatement-macros",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wnamespaces",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wnarrowing",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "Wnested-externs",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wno-alloc-size-larger-than",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Walloc-size-larger-than=", "18446744073709551615EiB", "none" },
  .min_gcc_version = {9, 1, 0},
},
{
  "Wno-alloca-larger-than",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Walloca-larger-than=", "18446744073709551615EiB", "none" },
  .min_gcc_version = {9, 1, 0},
},
{
  "Wno-vla-larger-than",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wvla-larger-than=", "18446744073709551615EiB", "none" },
  .min_gcc_version = {9, 1, 0},
},
{
  "Wnoexcept",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnoexcept-type",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wnon-template-friend",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnon-virtual-dtor",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnonnull",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnonnull-compare",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wnormalized",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wnormalized=", "nfc", "none" },
  .min_gcc_version = {5, 1, 0},
},
{
  "Wnormalized=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "Wnormalized=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wold-style-cast",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wold-style-declaration",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wold-style-definition",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wopenmp-simd",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{
  "Woverlength-strings",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Woverloaded-virtual",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Woverride-init",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Woverride-init-side-effects",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wpacked-bitfield-compat",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpacked-not-aligned",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wparentheses",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpedantic",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpessimizing-move",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Wplacement-new",
  gcc_cmdline_parser::option::comp_cxx,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wplacement-new=", "1", "0" },
  .min_gcc_version = {6, 1, 0},
},
{
  "Wplacement-new=",
  gcc_cmdline_parser::option::comp_cxx,
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wpmf-conversions",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpointer-arith",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpointer-compare",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wpointer-sign",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpointer-to-int-cast",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpragmas",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wprio-ctor-dtor",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Wproperty-assign-default",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wprotocol",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wpsabi",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wredundant-decls",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wredundant-move",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Wregister",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wreorder",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wrestrict",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wreturn-local-addr",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "Wreturn-type",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wscalar-storage-order",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wselector",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsequence-point",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wshadow-ivar",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wshift-count-negative",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wshift-count-overflow",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wshift-negative-value",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wshift-overflow",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wshift-overflow=", "1", "0" },
  .min_gcc_version = {6, 1, 0},
},
{
  "Wshift-overflow=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wsign-compare",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsign-conversion",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsign-promo",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsized-deallocation",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wsizeof-array-argument",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wsizeof-pointer-div",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wsizeof-pointer-memaccess",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-aliasing=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-null-sentinel",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-overflow=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-prototypes",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstrict-selector-match",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wstringop-overflow",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wstringop-overflow=", "2", "0" },
  .min_gcc_version = {7, 1, 0},
},
{
  "Wstringop-overflow=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "Wstringop-truncation",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "Wsubobject-linkage",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wsuggest-attribute=format",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsuggest-override",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wswitch",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wswitch-bool",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "Wswitch-default",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wswitch-enum",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsync-nand",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsynth",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wsystem-headers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wtautological-compare",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wtemplates",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wterminate",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wtraditional",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wtraditional-conversion",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wtrigraphs",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wundeclared-selector",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wundef",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wuninitialized",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunknown-pragmas",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunsuffixed-float-constants",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused-const-variable",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wunused-const-variable=", "2", "0" },
  .min_gcc_version = {6, 1, 0},
},
{
  "Wunused-const-variable=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wunused-local-typedefs",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused-macros",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused-result",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wunused-variable",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wuseless-cast",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvarargs",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvariadic-macros",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvirtual-inheritance",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "Wvirtual-move-assign",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvla",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wvla-larger-than=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
  .max_gcc_version = {8, 3, std::numeric_limits<unsigned int>::max()},
},
{
  "Wvla-larger-than=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "Wvolatile-register-var",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wwrite-strings",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wzero-as-null-pointer-constant",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ansi",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "d",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "fabi-compat-version=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "faccess-control",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fada-spec-parent=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "faligned-new",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "faligned-new=", "1", "0" },
  .min_gcc_version = {7, 1, 0},
},
{
  "faligned-new=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "fall-virtual",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fallow-parameterless-variadic-functions",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "falt-external-templates",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fasm",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fbuilding-libgcc",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fbuiltin",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fbuiltin-",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "fcanonical-system-headers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fchar8_t",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "fcheck-new",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  "fcheck-pointer-bounds",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-check-incomplete-type",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-check-read",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-check-write",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-first-field-has-own-bounds",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-flexible-struct-trailing-arrays",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "fchkp-instrument-calls",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-instrument-marked-only",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-narrow-bounds",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-narrow-to-innermost-array",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-optimize",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-store-bounds",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-treat-zero-dynamic-size-as-infinite",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-fast-string-functions",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-nochk-string-functions",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-static-bounds",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-static-const-bounds",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-use-wrappers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fchkp-zero-input-bounds-for-main",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fcilkplus",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{
  "fcond-mismatch",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fconserve-space",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fconstant-string-class=",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "fconstexpr-depth=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fconstexpr-ops-limit=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "fdebug-cpp",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdeclone-ctor-dtor",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{  "fdeduce-init-list",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdefault-inline",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdiagnostics-show-template-tree",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "fdirectives-only",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdollars-in-identifiers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdump-ada-spec",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "fdump-ada-spec-slim",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "felide-constructors",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "felide-type",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "femit-struct-debug-baseonly",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "femit-struct-debug-detailed=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "femit-struct-debug-reduced",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fenforce-eh-specs",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fenum-int-equiv",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fexec-charset=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fext-numeric-literals",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fextended-identifiers",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fextern-tls-init",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fexternal-templates",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ffor-scope",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ffreestanding",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ffriend-injection",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fgimple",
  gcc_cmdline_parser::option::comp_c,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "fgnu-keywords",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fgnu-runtime",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "fgnu89-inline",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fguiding-decls",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fhandle-exceptions",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "fexceptions" },
},
{
  "fhonor-std",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fhosted",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fhuge-objects",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fimplement-inlines",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fimplicit-inline-templates",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fimplicit-templates",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "finput-charset=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fivar-visibility=",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fkeep-inline-dllexport",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "flabels-ok",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "flax-vector-conversions",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "flocal-ivars",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fmacro-prefix-map=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "fms-extensions",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fmudflap",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  "fmudflap",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{
  "fmudflapir",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  "fmudflapir",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{
  "fmudflapth",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 8, 5},
},
{
  "fmudflapth",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 0},
},
{
  "fname-mangling-version-",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
},
{
  "fnew-abi",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fnew-inheriting-ctors",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "fnew-ttp-matching",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "fnext-runtime",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "fnil-receivers",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fnonansi-builtins",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fnonnull-objects",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fnothrow-opt",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-abi-version=",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fobjc-call-cxx-cdtors",
  gcc_cmdline_parser::option::comp_objcxx,
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-direct-dispatch",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-exceptions",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-gc",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-nilcheck",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-sjlj-exceptions",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fobjc-std=objc1",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fopenacc",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fopenacc-dim=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
},
{
  "fopenmp",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fopenmp-simd",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {4, 9, 0},
},
{
  "foperator-names",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "foptional-diags",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpch-deps",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpch-preprocess",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpermissive",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fplan9-extensions",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpreprocessed",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fpretty-templates",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fprintf-return-value",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "freplace-objc-classes",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "frepo",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "frtti",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fshort-double",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {5, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "fshort-enums",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fshort-wchar",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fsigned-bitfields",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fsigned-char",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fsized-deallocation",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "fsquangle",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fsso-struct=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {6, 1, 0},
  .max_gcc_version = {6, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "fsso-struct=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "fstats",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fstrict-enums",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fstrict-prototype",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fstrong-eval-order",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx |
   gcc_cmdline_parser::option::comp_common),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "fstrong-eval-order=", "all", "none" },
  .min_gcc_version = {7, 1, 0},
},
{
  "fstrong-eval-order=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx |
   gcc_cmdline_parser::option::comp_common),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "ftabstop=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "ftemplate-backtrace-limit=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "ftemplate-depth-",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = { "ftemplate-depth=" },
},
{
  "ftemplate-depth=",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fthis-is-variable",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fthreadsafe-statics",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "ftrack-macro-expansion",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = true,
},
{
  "ftrack-macro-expansion=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined_or_missing,
  .reject_negative = true,
},
{
  "funsigned-bitfields",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "funsigned-char",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fuse-cxa-atexit",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fuse-cxa-get-exception-ptr",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fvisibility-inlines-hidden",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fvisibility-ms-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fvtable-gc",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fvtable-thunks",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fweak",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fwide-exec-charset=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  "fworking-directory",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fxref",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fzero-link",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "gen-decls",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx |
   gcc_cmdline_parser::option::comp_driver),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {6, 5, std::numeric_limits<unsigned int>::max()},
},
{
  "gen-decls",
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {7, 1, 0},
},
{
  "idirafter",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "imacros",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "imultilib",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "include",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "iprefix",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "iquote",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "isysroot",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "isystem",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "iwithprefix",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "iwithprefixbefore",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "lang-asm",
  gcc_cmdline_parser::option::comp_c,
  gcc_cmdline_parser::option::arg_none,
},
{
  "no-integrated-cpp",
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
},
{
  "nostdinc",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "nostdinc++",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "o",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  (gcc_cmdline_parser::option::arg_joined |
   gcc_cmdline_parser::option::arg_separate),
},
{
  "pedantic",
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
  (gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "remap",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "static-libmpx",
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "static-libmpxwrappers",
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "std=c++03",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++98" },
},
{
  "std=c++0x",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++11" },
},
{
  "std=c++11",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c++14",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++1y" },
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  "std=c++14",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "std=c++17",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++1z" },
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "std=c++17",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c++1y",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "std=c++1y",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++14" },
  .min_gcc_version = {5, 1, 0},
},
{
  "std=c++1z",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "std=c++1z",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c++17" },
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c++2a",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c++98",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c11",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c17",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c18",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .min_gcc_version = {8, 1, 0},
},
{
  "std=c1x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c11" },
},
{
  "std=c2x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "std=c89",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c90" },
},
{
  "std=c90",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c99",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=c9x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  "std=gnu++03",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++98" },
},
{
  "std=gnu++0x",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++11" },
},
{
  "std=gnu++11",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu++14",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++1y" },
  .min_gcc_version = {4, 9, 0},
  .max_gcc_version = {4, 9, 4},
},
{
  "std=gnu++14",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
},
{
  "std=gnu++17",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++1z" },
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "std=gnu++17",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu++1y",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {},
  .max_gcc_version = {4, 9, 4},
},
{
  "std=gnu++1y",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "gnu++14" },
  .min_gcc_version = {5, 1, 0},
},
{
  "std=gnu++1z",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {5, 1, 0},
  .max_gcc_version = {7, 4, std::numeric_limits<unsigned int>::max()},
},
{
  "std=gnu++1z",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu++17" },
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu++2a",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu++98",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu11",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu17",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu18",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu17" },
  .min_gcc_version = {8, 1, 0},
},
{
  "std=gnu1x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu11" },
},
{
  "std=gnu2x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .min_gcc_version = {9, 1, 0},
},
{
  "std=gnu89",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu90" },
},
{
  "std=gnu90",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu99",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=gnu9x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu99" },
},
{
  "std=iso9899:1990",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c90" },
},
{
  "std=iso9899:199409",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "std=iso9899:1999",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  "std=iso9899:199x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c99" },
},
{
  "std=iso9899:2011",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c11" },
},
{
  "std=iso9899:2017",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .min_gcc_version = {8, 1, 0},
},
{
  "std=iso9899:2018",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c17" },
  .min_gcc_version = {8, 1, 0},
},
{
  "traditional",
  gcc_cmdline_parser::option::comp_driver,
  gcc_cmdline_parser::option::arg_none,
},
{
  "traditional-cpp",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "trigraphs",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "undef",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "v",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "w",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
