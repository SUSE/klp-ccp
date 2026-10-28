/*
 * Forced macro renames: forwarding definitions for variadic macros
 * and for a macro with a parameter named like the macro itself.
 */
#define fr_a(fmt, ...) f(fmt, ##__VA_ARGS__)
#define fr_b(args...) g(args)
#define fr_c(fr_c) (fr_c)
#define fr_d() 4

extern void f(const char*, ...);
extern void g(int, int);

void pu_f(void)
{
	fr_a("x", 1, 2);
	fr_a("y");
	fr_b(1, 2);
	int i = fr_c(3);
	i += fr_d();
}
/* cp-out: warning: [^:]*: line 15, columns 1-16: function "f" without definition cannot get externalized */
/* cp-out: warning: [^:]*: line 16, columns 1-10: function "f" without definition cannot get externalized */
/* cp-out: warning: [^:]*: line 17, columns 1-11: function "g" without definition cannot get externalized */
