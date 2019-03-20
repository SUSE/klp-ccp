double test (const double le, const double ri)
{
	double val = ( ri - le ) / ( ri * ( le + 1.0 ) );
	return val;
}

int main ()
{
        double retval;

	retval = test(1.0,2.0);
        if (retval < 0.24 || retval > 0.26)
	  abort ();
	exit (0);
}

/* cp-out: warning: [^:]*: line 13, column 3: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, column 1: identifier "exit" not declared */
