void
f (void)
{
  int i = (1 ? 0U / 0 : 1);
}

/* cp-out: warning: [^:]*: line 4, columns 15-21: division by zero */
