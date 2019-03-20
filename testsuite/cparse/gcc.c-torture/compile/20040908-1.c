/* PR 17186 */

double foo()
{
    int i;
    double d;

    if (i)
        bar();
    else
        if (d) return 0;
}

/* cp-out: warning: [^:]*: line 9, column 8: identifier "bar" not declared */
