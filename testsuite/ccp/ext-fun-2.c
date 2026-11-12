/*
 * Functions externalized through a pointer: the declared entity
 * becomes a pointer variable, for which the 'inline' function
 * specifier is meaningless and provokes a warning. Remove it. It
 * stays on extern redeclarations.
 */
static inline int espr_f(int x)
{
	return x;
}

static __inline__ int espr_g(int x);

static __inline__ int espr_g(int x)
{
	return x;
}

inline static int __attribute__((const)) espr_h(int x)
{
	return x;
}

static inline int ee_i(int x)
{
	return x;
}

int pu_f(void)
{
	extern __inline__ int espr_g(int x);

	return espr_f(1) + espr_g(2) + espr_h(3) + ee_i(4);
}
