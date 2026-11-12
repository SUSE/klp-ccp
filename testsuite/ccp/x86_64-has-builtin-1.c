/*
 * The __has_builtin() special builtin macro exists since GCC 10
 * only: with GCC 9.1.0, it must not be defined so that the usual
 * fallback definition gets taken.
 */
#ifdef __has_builtin
#error "__has_builtin defined"
#endif
#ifndef __has_builtin
#define __has_builtin(x) (0)
#endif

int pu_f(void)
{
	int r = 0;
#if __has_builtin(__builtin_expect)
	r += 1;
#endif
	return r;
}
