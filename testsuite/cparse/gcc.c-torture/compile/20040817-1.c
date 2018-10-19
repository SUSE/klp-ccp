/* PR 17051: SRA failed to rename the VOPS properly.  */

struct A
{
    char c, d;
};

void foo(struct A *p)
{
    struct A a = *p;

    if (p->c)
        bar1(a);
    else
    {
        if (p) bar2(a,a.c);
        bar3(a.c);
    }
}

/* cp-out: warning: [^:]*: line 13, columns 8-12: identifier "bar1" not declared */
/* cp-out: warning: [^:]*: line 16, columns 15-19: identifier "bar2" not declared */
/* cp-out: warning: [^:]*: line 17, columns 8-12: identifier "bar3" not declared */
