#define fr_a(x) ((x) + 1)
#define FR_B __attribute__((unused))

static inline int h1(void)
{
	return fr_a(1);
}

static inline int h2(void)
{
	return 2;
}

extern int o1 FR_B;
extern int o2;
extern int ee_o3 FR_B;
extern void ee_f1(void) FR_B;
extern void ee_f2(void);

/*
 * A single macro invocation providing both, a declaration not
 * involving any renamed macro and a function definition invoking
 * one. Only the latter is to get emitted in rewritten form.
 */
#define DECL_PAIR(name)				\
	extern int name;			\
	static inline int get_##name(void)	\
	{					\
		return fr_a(name);		\
	}
DECL_PAIR(o4)
