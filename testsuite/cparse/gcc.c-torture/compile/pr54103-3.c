void
f (void)
{
  1 && 0 / 0 ? : 0;
}

/* cp-out: warning: [^:]*: line 4, columns 7-12: division by zero */
