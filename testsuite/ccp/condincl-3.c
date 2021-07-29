#define A(x) pu_f(void)
#define B 1

#if B /* 1 */
void A(
#endif /* 1 */
#if B /* 2 */
#if B /* 3 */
		unused
#else /* 3 */
		undefined
#endif /* 3 */
#else /* 2 */
		undefined
#endif /* 2 */
#if !B /* 4 */
		undefined
#else /* 4 */
		unused
#endif /* 4 */
#if B /* 5 */
	)
#endif /* 5 */
{}
