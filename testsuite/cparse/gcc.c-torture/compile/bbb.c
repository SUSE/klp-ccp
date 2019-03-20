struct looksets
    {
        int lset[10];
    };

struct looksets lkst[];

flset( p )
struct looksets *p;
{
    p-- > lkst;
}

/* cp-out: warning: [^:]*: line 6, columns 15-22: init declarator has incomplete type */
