#define A(x) x
#define B 1
#define C 0

void pu_f(void)
{
#if B /* 1 */
	A(
#endif
#if B /* 2 */
#if B /* 3 */
		C +
#else
		undefined
#endif
#else
		undefined
#endif
#if !B /* 4 */
		undefined
#else
		C
#endif
#if B /* 5 */
	)
#endif
	;
}
