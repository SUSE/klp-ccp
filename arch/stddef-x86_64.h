#define __SIZE_TYPE__ unsigned long
#define __PTRDIFF_TYPE__ long
#define __INTPTR_TYPE__ long
#define __UINTPTR_TYPE__ unsigned long
#define __INT32_TYPE__ int
#define __UINT32_TYPE__ unsigned int
#define __INT64_TYPE__ long
#define __UINT64_TYPE__ unsigned long
#define __CHAR16_TYPE__ short
#define __CHAR32_TYPE__ int
#define __WCHAR_TYPE__ int

typedef __int128 __int128_t;
typedef unsigned __int128 __uint128_t;

#define __CHAR_BIT__ 8
#define __SIZE_MAX__ 0xffffffffffffffffUL
#define __PTRDIFF_MAX__ __LONG_MAX__
#define __SCHAR_MAX__ 0x7f
#define __INT_MAX__ 0x7fffffff
#define __LONG_MAX__ 0x7fffffffffffffffL
#define __LLONG_MAX__ __LONG_MAX__
#define __LONG_LONG_MAX__ __LONG_MAX__
#define __SHRT_MAX__ 0x7fff
#define __WCHAR_MAX__ __INT_MAX__

#define __FLT_MIN__ 1.17549435082228750797e-38F
#define __FLT_MAX__ 3.40282346638528859812e+38F
#define __DBL_MIN__ ((double)2.22507385850720138309e-308L)
#define __DBL_MAX__ ((double)1.79769313486231570815e+308L)
#define __LDBL_MIN__ 3.36210314311209350626e-4932L
#define __LDBL_MAX__ 1.18973149535723176502e+4932L

#define __SIZEOF_INT__ sizeof(int)
#define __SIZEOF_LONG__ sizeof(long)


#define NULL ((void *) 0)

#define __func__ ""
#define __FUNCTION__ ""
#define __PRETTY_FUNCTION__ ""
#define __FILE__ ""
#define __BASE_FILE__ ""

