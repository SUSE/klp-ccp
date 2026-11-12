struct s { int X; };
struct s X_s;
int espr_i;

#define X (espr_i + X_s.X)
#define TWICE(y)			\
	y;				\
	{				\
		int espr_i;		\
		y;			\
	}

void pu_f(void)
{
	int a;
	TWICE(a = X);
}
/* cp-out: warning: [^:]*: line 2, column 9: referenced object "X_s" cannot get externalized */
