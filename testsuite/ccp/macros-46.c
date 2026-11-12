struct s { int X; };
struct s X_s;
int espr_i;

#define X (espr_i + X_s.X)
#define ID(y) y
#define S(y) #y
#define TOP(v)					\
	int a = X;				\
	{					\
		int espr_i = v;			\
		int b = ID(X) + a;		\
		const char *c = S(X);		\
	}

void pu_f(void)
{
	TOP(1);
}
/* cp-out: warning: [^:]*: line 2, column 9: referenced object "X_s" cannot get externalized */
