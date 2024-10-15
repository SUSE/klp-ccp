# klp-ccp - Preliminary documentation
## Introduction
klp-ccp aims at automating source based Linux kernel live patch creation.
It's main purpose is to create a minimal, self-contained source code file
containing all functions marked as "patched" from the original input
sources.

klp-ccp has no notion of "diffs" yet, so users would run klp-ccp
either on patched kernel sources or (preferred) on the unpatched
sources and apply the diff in question manually
afterwards. Implementing support for working directly with diffs is a
planned feature though.

**Warning: There are known issues, see the section at the end of
  this file.**

The main motivation for making the sources public at this point is to
allow attendees of the Live Patching MC at the upcoming Linux Plumbers
Conference 2019 to get an idea of klp-ccp's current capabilities and
its intended usage.


## Building
If on a desktop machine with about 8GB of RAM, close your browser and
don't use `-j`. You have been warned.

A C++ compiler with C++11 support is needed. A reasonably recent
GCC or clang will do. If you pulled from git, GNU bison 3.0 (tested:
3.0.4) will be required for the autogeneration of the parser
code. Anything before that won't work, because there had been some
incompatible changes.

To build klp-ccp, run the common
```
./configure
make
make check # optional

# make install not needed
```
sequence.

## Overview on the general approach
klp-ccp proceeds in several steps to create the live patch from the
input kernel source file:
1. The input source gets preprocessed. In the course of doing so, all
   information needed for restoring macro invocations and header
   inclusions in the final step gets recorded.
2. The output from the preprocessor is parsed to create an internal
   representation of the AST.
3. The AST is evaluted w.r.t types and constant expressions.
4. Starting from the set of functions marked as patched, a closure is
   built. In the course of doing so, externalization decisions from
   the user policy (c.f. below) are taken into account.
5. Patched, externalized and rewritten function's symbols are modified
   as determined by the user policy. Same for externalized
   objects.
6. Header files eligible for inclusion from the live patch
   (c.f. below) are checked for conflicting declarations and/or
   definitions and marked as non-eligible if any are found.
7. Type dependencies from the live patch closure get resolved
   recursively. Eligible header files providing any needed declaration
   or definition get selected for inclusion from the resulting live
   patch.
8. An internal representation of the resulting live patch source code
   is prepared and passed to the macro depreprocessor.
9. The macro depreprocessor determines the set of retainable macro
   invocations as well as the set of #defines and #undefs to get
   emitted into the resulting live patch's source code.
10. The depreprocessor writes the output file.

## Invocation
```
Usage: klp-ccp OPTIONS -- ORIGINAL COMPILER COMMAND LINE
```
where `ORIGINAL COMPILER COMMAND LINE` denotes the full compiler
invocation as used when building the input source file in question for
the live patching target, but with the leading compiler program name
stripped. Currently, klp-ccp uses this only for extracting
preprocessor options, but in the future it might warn on options
affecting code generation like `-fpack-struct` and the like.

### Options:
- `--compiler=ARCH-COMPILER-VERSION`: specification of the
  compiler which had been used for the live patch target's
  compilation. Affects mostly the predefined macros, but also
  type evaluation (sizeof etc.).
  Currently, only "`x86_64`" is supported for `ARCH` and "`gcc`" for
  `COMPILER`. Example: `--compiler=x86_64-gcc-4.8.5`.
- `--outfile=FILE`: the file to write the result to.
- `--patched-functions=LIST`: comma separated list of functions
  to consider as "patched".
- `--python-policy=POLICY-IMPL-CLASS`: the controlling user policy,
  c.f. below.

#### User policy
Even though many decisions need to be made in the course of live patch
creation, like about which headers includes may be retained in the
result, what and how to externalize, how to rename symbols and so on,
klp-ccp does not implement any of such policy in itself by
design. Instead, it is expected that the user implements a controlling
policy to be queried by klp-ccp in Python, by implementing the
abstract base class ("ABC") `ccp.LpCreationPolicyAbc` and specifes its
(fully qualified) name on the klp-ccp command line by means of the
`--python-policy=POLICY-IMPL-CLASS` option. The environment variable
`$KLP_CCP_PYTHONPOLICY_PATH` can be set to extend the search path for
such policies, in analogy to the well-known `$PYTHONPATH`.

For an example of an `ccp.LpCreationPolicyAbc` refer to the toy
policies used for running the testcases, implemented in
`testsuite/lib/ccp-toy-policy/`.

## Known issues and limitations
- klp-ccp is largely untested
- klp-cpp hasn't support for directly operating on diffs yet.
- You'll notice that a lot of `CONFIG_` defines get emitted into the
  resulting live patch source. That's because klp-ccp would currently
  pull in headers only if some declaration is needed from them --
  something which is clearly not the case with
  include/generated/autoconf.h. This behaviour might change in the
  future for headers containing nothing but preprocessor directives
  and/or pre-includes.
- Support for GCC's `vector_size` attribute is missing.
- A large number of machine specific predefined macros like
  `__PCLMUL__`, `__MMX__`, etc. aren't provided by klp-ccp yet.
- ...?
