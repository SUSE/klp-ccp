int FOO;
int espr_i;

#define FOO (FOO + espr_i)

void pu_f(void)
{
	int x = FOO;
}
/* cp-out: warning: [^:]*: line 1, column 4: referenced object "FOO" cannot get externalized */
