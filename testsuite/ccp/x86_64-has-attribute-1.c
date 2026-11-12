/*
 * __has_attribute() is a special builtin macro whose value reflects
 * the target GCC's (9.1.0 here) set of known attributes: common
 * ones, x86 specific ones and unknown ones, with the __x__ spelling
 * and macro expanded operands. It can be queried with defined and
 * #ifdef and is usable outside of #if conditions as well.
 */
#ifndef __has_attribute
#error "__has_attribute not defined"
#endif
#if !defined(__has_attribute) || !defined __has_attribute
#error "__has_attribute not defined"
#endif

#define ATTR_UNUSED __unused__
#define ID(x) x

int pu_f(void)
{
	int r = 0;
#if __has_attribute(unused)
	r += 1;
#endif
#if __has_attribute(__packed__)
	r += 2;
#endif
#if __has_attribute(ATTR_UNUSED)
	r += 4;
#endif
#if __has_attribute(ID(aligned))
	r += 8;
#endif
#if __has_attribute(ms_abi)
	r += 16;
#endif
#if __has_attribute(aarch64_vector_pcs)
	r += 32;
#endif
#if __has_attribute(copy)
	r += 64;
#endif
#if __has_attribute(access)
	r += 128;
#endif
#if __has_attribute(bnd_legacy)
	r += 256;
#endif
#if __has_attribute(nonexistent)
	r += 512;
#endif
#if __has_attribute(noreturn) == 1
	r += 1024;
#endif
#if __has_attribute( __format__ )
	r += 2048;
#endif
	return r + __has_attribute(unused) + __has_attribute(nonexistent);
}
