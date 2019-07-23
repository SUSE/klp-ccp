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
  "Waddress",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "Warray-bounds",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
},
{
  "Wc++11-compat",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "Wc++0x-compat" },
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
  "Wchar-subscripts",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "Wctor-dtor-privacy",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wdeclaration-after-statement",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
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
  "Wformat-security",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "Winherited-variadic-ctor",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "Wnarrowing",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnested-externs",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wnoexcept",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "Wnormalized=",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_joined,
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
  "Wpacked-bitfield-compat",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "Wreorder",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "Wreturn-local-addr",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "Wsuggest-attribute=format",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "fcheck-new",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "fdebug-cpp",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
},
{
  "fdeduce-init-list",
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
},
{
  "fmudflapir",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
},
{
  "fmudflapth",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
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
  "fopenmp",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc |
   gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "fsquangle",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "std=c++1y",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "std=c1x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=c11" },
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
  "std=gnu++1y",
  (gcc_cmdline_parser::option::comp_cxx |
   gcc_cmdline_parser::option::comp_objcxx),
  gcc_cmdline_parser::option::arg_none,
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
  "std=gnu1x",
  (gcc_cmdline_parser::option::comp_c |
   gcc_cmdline_parser::option::comp_objc),
  gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = { "std=gnu11" },
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
