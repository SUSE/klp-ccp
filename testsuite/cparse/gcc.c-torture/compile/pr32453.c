void ucs2reverse_internal_loop_single (long *irreversible, int foo)
{
    unsigned char bytebuf[2];
    const unsigned char *inptr = bytebuf;
    if (irreversible == (void *)0)
	;
    else 
	inptr += 2;
    if (inptr != bytebuf)
	((inptr - bytebuf > foo) ? (void) (0) : __assert_fail ());
}

/* cp-out: warning: [^:]*: line 10, columns 41-54: identifier "__assert_fail" not declared */
