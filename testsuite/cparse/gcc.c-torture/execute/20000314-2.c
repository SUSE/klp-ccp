typedef unsigned long long uint64;
const uint64 bigconst = 1ULL << 34;

int a = 1;

static
uint64 getmask(void)
{
    if (a)
      return bigconst;
    else
      return 0;
}

main()
{
    uint64 f = getmask();
    if (sizeof (long long) == 8
	&& f != bigconst) abort ();
    exit (0);
}

/* cp-out: warning: [^:]*: line 19, columns 19-24: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, columns 4-8: identifier "exit" not declared */
