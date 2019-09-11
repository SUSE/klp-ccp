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

#### Policy commands
The remaining options are all related to "policy commands". These are
user provided shell scrips which govern various aspects of the live
patch creation process. klp-ccp invokes these with arguments as
described below.

The result is read back from a single stdout line beginning with
`RESULT:`. In addition, lines on stdout beginning with `WARNING:` or
`ERROR:` can be used to report warning and errors to the user. klp-ccp
will amend these with with location information for the entity in
question before printing them.

Any unexpected output on stdout, anything on stderr or a non-zero exit
status will be considered to be a failure of the policy command itself
and cause klp-ccp to abort the live patch creation.

The toy policy implementation found in `testsuite/lib/ccp-toy-policy/`
is purely based on the naming of identifiers and might serve as a good
starting point. For a description of the naming scheme, c.f. commit
`015130b68679 ('testsuite: introduce DejaGnu tool specification "ccp"')`.

Options:
- `--pol-cmd-is-patched=CMD`:
  Whether or not a particular function is to be considered as patched.
  This option is intended to be used from the name based toy policy
  implementation from the testsuite only.

- `--pol-cmd-may-include-header=CMD`:
   Whether or not some header file is eligible for inclusion from the
   final live patch. It is expected that policy implementations mark
   public headers as eligible and internal, private ones as
   non-eligible.

   Arguments: `<HEADER-FILE-NAME> [pre-include]`  
   Result format: `YES|NO`

- `--pol-cmd-can-externalize-fun=CMD`:
   Whether or not a certain function can be externalized,
   i.e. whether references to it can be bound to some instance in the
   live patch's target binary by means of some "externalization
   method", c.f. below. The policy must not return false negatives
   here, because externalizing some function might turn out to be
   non-optional in case there's a referencing expression in the
   closure which is not a function call.

   Arguments: `<NAME> [internal|external <DEFINITION-LOCATION>]`  
   Result format: `YES|NO`

- `--pol-cmd-shall-externalize-fun=CMD`:
   Whether or not externalization of some externalizable function is
   preferred. For those functions reachable from the closure which are
   not patched, externalizable and for which a function definition is
   available in the input source, externalization is optional and
   thus, is policy decision.  In general, doing the externalization
   would result in a smaller live patch size and should be
   preferred. However there's one exception: if the method of
   externalization (c.f. below) would not involve symbol renaming and
   if the function's definition happened to be provided by some header
   eligible for inclusion from the live patch, externalizing it would
   render that header non-eligible (because the externalizing
   declaration would clash with the original definition) and should
   certainly be avoided, if possible.

   Arguments: `<NAME> [from-header]`  
   Result format: `YES|NO`

- `--pol-cmd-shall-externalize-obj=CMD`:
  Whether or not some object with static storage duration shall be
  externalized. This will certainly be almost the case for objects not
  newly introduced by the live patch.

  Arguments: `<NAME>`  
  Result format: `YES|NO`

- `--pol-cmd-modify-patched-fun-sym=CMD`:
  How to tweak a patched function's symbol. The policy may decide to
  rename it and optionally to force its linkage to be internal
  ("static") or external. Renaming is probably something any policy
  implementation would choose to do: if nothing else, it can help to
  avoid clashes with original definitions provided by eligible
  headers. If and how to change the linkage will probably depend on
  how the user intends to supplement the kernel live patching glue
  code and also, on whether or not the resulting source file will be
  linked together with unrelated ones into a cumulative (klp_patch
  ->replace) live patch.

  Arguments: `<NAME> <RETRY>`  
  Result format: `<NEW-NAME> KEEP-LINKAGE|MAKE-INTERNAL|MAKE-EXTERNAL`

  `RETRY` is initially set to zero. In case `NEW-NAME` differs from
  `NAME`, the command will be invoked with `RETRY` incremented until
  `NEW-NAME` is unique.

- `--pol-cmd-modify-externalized-sym=CMD`:
   How to do externalizations. In addition to the symbol modification
   described for the patched functions above, a policy may request to turn
   the type of the symbol in question into "pointer to original type".
   Note that there are currently two known real world externalization
   methods in use with Linux kernel live patches:
   - `klp-convert` resp. `.klp.*` relocations: the policy would
     instruct the live patch creation code to not rename the symbol
     and to not change its type, but to force its linkage to be
     "extern", so that an UNDEF reference will get emitted into the
     resulting live patch's ELF.
   - the kallsyms method: this involves turning the symbol in question
     into "pointer to original type" which would then be initialized
     by the live patch module's init code.

  Arguments: `function|object <NAME> <RETRY>`  
  Result format: `<NEW-NAME> KEEP-LINKAGE|MAKE-INTERNAL|MAKE-EXTERNAL [MAKE-POINTER]`

  `RETRY` is initially set to zero. In case `NEW-NAME` differs from
  `NAME`, the command will be invoked with `RETRY` incremented until
  `NEW-NAME` is unique.

- `--pol-cmd-renamed-rewritten-fun=CMD`:
  If and how to rename rewritten functions. Non-patched function
  definitions in the closure might have to get rewritten in order to
  fixup references to symbols altered due to e.g. externalizations.
  Again, to retain header eligibility, it's probably a good idea to
  rename these. Note that a non-patched function definition's linkage
  will be unconditionally forced to be internal ("static").

  Arguments: `<NAME> <RETRY>`  
  Result format: `<NEW-NAME>`

  `RETRY` is initially set to zero. In case `NEW-NAME` differs from
  `NAME`, the command will be invoked with `RETRY` incremented until
  `NEW-NAME` is unique.

## Known issues and limitations
- klp-ccp is largely untested
- klp-ccp hasn't support for directly operating on diffs yet.
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
