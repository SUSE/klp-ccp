void ee_f(void)
{}

static void (*o)(void) = &ee_f;

void pu_g(void)
{
	o;
}

/* cp-out: warning: [^:]*: line 8, column 1: referenced object "o" cannot get externalized */
