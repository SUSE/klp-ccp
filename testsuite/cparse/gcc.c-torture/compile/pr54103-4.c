void
f (void)
{
  0 / 0 && 1 ? : 0;
}

/* cp-out: warning: [^:]*: line 4, columns 2-7: division by zero */
