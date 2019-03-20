int myvar;

int foo(int mynum)
{
  if ((((void *)0) == (myvar & ((1U<<0) << mynum))) && (mynum > 0))
    return 1;
  return 0;
}

/* cp-out: warning: [^:]*: line 5, columns 22-50: pointer comparison against non-NULL integer */
