int i;
void
f (void)
{
  i = (1 / 0) / 0;
}

/* cp-out: warning: [^:]*: line 5, columns 11-12: division by zero */
