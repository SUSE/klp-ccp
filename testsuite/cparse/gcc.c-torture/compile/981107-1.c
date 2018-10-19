unsigned long seed(void)
{
    unsigned long u;

    call();

    u = 26107 * (unsigned long)&u;
    return u;
}

/* cp-out: warning: [^:]*: line 5, columns 4-8: identifier "call" not declared */
