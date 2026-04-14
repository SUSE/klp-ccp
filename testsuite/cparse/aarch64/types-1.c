/* Test aarch64-specific type properties. */
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __WCHAR_TYPE__ wchar_t;

/* LP64: all pointer-width types are 64 bits. */
_Static_assert(sizeof(size_t) == 8, "size_t is 8 bytes");
_Static_assert(sizeof(ptrdiff_t) == 8, "ptrdiff_t is 8 bytes");
_Static_assert(sizeof(void *) == 8, "pointer is 8 bytes");
_Static_assert(sizeof(long) == 8, "long is 8 bytes");
_Static_assert(sizeof(long long) == 8, "long long is 8 bytes");

/* wchar_t is 4 bytes on aarch64. */
_Static_assert(sizeof(wchar_t) == 4, "wchar_t is 4 bytes");

/* Standard integer sizes. */
_Static_assert(sizeof(char) == 1, "char is 1 byte");
_Static_assert(sizeof(short) == 2, "short is 2 bytes");
_Static_assert(sizeof(int) == 4, "int is 4 bytes");

/* Float sizes. */
_Static_assert(sizeof(float) == 4, "float is 4 bytes");
_Static_assert(sizeof(double) == 8, "double is 8 bytes");

/* On aarch64, long double is IEEE 754 quad precision (128-bit). */
_Static_assert(sizeof(long double) == 16, "long double is 16 bytes");

/* __int128 is supported on aarch64. */
_Static_assert(sizeof(__int128) == 16, "__int128 is 16 bytes");

size_t s;
ptrdiff_t p;
wchar_t w;
