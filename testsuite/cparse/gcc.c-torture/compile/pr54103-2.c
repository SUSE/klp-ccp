void
f (void)
{
  0 / 0 || 0 ? : 0;
}

/* cp-out: warning: [^:]*: line 4, columns 6-7: division by zero */
