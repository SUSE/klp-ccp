int i;
void
f (void)
{
  i = (1 / 0 ? 1 : 0);
}

/* cp-out: warning: [^:]*: line 5, columns 7-12: division by zero */
