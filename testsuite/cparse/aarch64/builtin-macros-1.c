/* Test aarch64 builtin preprocessor macros. */

#ifndef __aarch64__
#error "__aarch64__ is not defined"
#endif

#ifndef __ARM_64BIT_STATE
#error "__ARM_64BIT_STATE is not defined"
#endif

#if __CHAR_BIT__ != 8
#error "__CHAR_BIT__ is not 8"
#endif

#if __LDBL_MANT_DIG__ != 113
#error "__LDBL_MANT_DIG__ is not 113 (IEEE quad)"
#endif

#if __DBL_MANT_DIG__ != 53
#error "__DBL_MANT_DIG__ is not 53"
#endif

#if __FLT_MANT_DIG__ != 24
#error "__FLT_MANT_DIG__ is not 24"
#endif

#ifndef __SIZEOF_INT128__
#error "__SIZEOF_INT128__ is not defined"
#endif

#if __BIGGEST_ALIGNMENT__ != 16
#error "__BIGGEST_ALIGNMENT__ is not 16"
#endif

/* Verify the default __ARM_ARCH is set. */
#if __ARM_ARCH < 8
#error "__ARM_ARCH is less than 8"
#endif

int x;
