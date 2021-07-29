#define A(x) x
#define B 1

#if B /* 1 */
void A(
#endif
#if B /* 2 */
#if B /* 3 */
		pu_f
#else
		undefined
#endif
#else
		undefined
#endif
#if !B /* 4 */
		undefined
#else
		(void)
#endif
#if B /* 5 */
	)
#endif
{}
