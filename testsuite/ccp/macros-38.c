#define DECLS(t) t a; t b

DECLS(int);

void pu_f(void)
{
	b = 1;
}
/* cp-out: warning: [^:]*: line 3, columns 0-10: referenced object "b" cannot get externalized */
