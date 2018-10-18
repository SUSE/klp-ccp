#define __SIZE_TYPE__ unsigned long
#define __PTRDIFF_TYPE__ long
#define __INTPTR_TYPE__ long
#define __UINTPTR_TYPE__ unsigned long
#define __INT8_TYPE__ char
#define __UINT8_TYPE__ unsigned char
#define __INT_LEAST8_TYPE__ char
#define __UINT_LEAST8_TYPE__ unsigned char
#define __INT16_TYPE__ short
#define __UINT16_TYPE__ unsigned short
#define __INT_LEAST16_TYPE__ short
#define __UINT_LEAST16_TYPE__ unsigned short
#define __INT32_TYPE__ int
#define __UINT32_TYPE__ unsigned int
#define __INT_LEAST32_TYPE__ int
#define __UINT_LEAST32_TYPE__ unsigned int
#define __INT64_TYPE__ long
#define __UINT64_TYPE__ unsigned long
#define __INT_LEAST64_TYPE__ long
#define __UINT_LEAST64_TYPE__ unsigned long
#define __CHAR16_TYPE__ unsigned short
#define __CHAR32_TYPE__ unsigned int
#define __WCHAR_TYPE__ int
#define __WINT_TYPE__ long

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

#define __FLT_MANT_DIG__ 24
#define __DBL_MANT_DIG__ 53

#define __SIZEOF_POINTER__ sizeof(void*)
#define __SIZEOF_SIZE_T__ sizeof(unsigned long)
#define __SIZEOF_PTRDIFF_T__ sizeof(long)
#define __SIZEOF_SHORT__ sizeof(short)
#define __SIZEOF_INT__ sizeof(int)
#define __SIZEOF_LONG__ sizeof(long)
#define __SIZEOF_LONG_LONG__ sizeof(long long)
#define __SIZEOF_WCHAR_T__ sizeof(int)
#define __SIZEOF_WINT_T__ sizeof(long)
#define __SIZEOF_FLOAT__ sizeof(float)
#define __SIZEOF_DOUBLE__ sizeof(double)
#define __SIZEOF_LONG_DOUBLE__ sizeof(long double)


#define NULL ((void *) 0)

#define __func__ ""
#define __FUNCTION__ ""
#define __PRETTY_FUNCTION__ ""
#define __FILE__ ""
#define __BASE_FILE__ ""

#define __GCC_ATOMIC_TEST_AND_SET_TRUEVAL 1
#define __ATOMIC_ACQ_REL 4
#define __ATOMIC_ACQUIRE 2
#define __ATOMIC_CONSUME 1
#define __ATOMIC_RELAXED 0
#define __ATOMIC_RELEASE 3
#define __ATOMIC_SEQ_CST 5
