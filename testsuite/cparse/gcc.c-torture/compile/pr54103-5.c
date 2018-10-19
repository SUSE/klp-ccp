void
f (void)
{
  !(0 / 0);
}

/* cp-out: warning: [^:]*: line 4, columns 8-9: division by zero */
