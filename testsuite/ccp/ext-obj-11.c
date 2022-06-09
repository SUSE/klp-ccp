void ee_f(void)
{}

static void (*o)(void) = &ee_f;

void pu_g(void)
{
	o;
}

/* cp-out: warning: [^:]*: line 4, columns 12-22: referenced object "o" cannot get externalized */
