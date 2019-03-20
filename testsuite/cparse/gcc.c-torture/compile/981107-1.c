unsigned long seed(void)
{
    unsigned long u;

    call();

    u = 26107 * (unsigned long)&u;
    return u;
}

/* cp-out: warning: [^:]*: line 5, column 4: identifier "call" not declared */
