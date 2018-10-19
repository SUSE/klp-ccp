void
f (void)
{
  int i = (0 ? 1 : 0U / 0);
}

/* cp-out: warning: [^:]*: line 4, columns 24-25: division by zero */
