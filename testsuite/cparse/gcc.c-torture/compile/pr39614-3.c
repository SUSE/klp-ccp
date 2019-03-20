int i;
void
f (void)
{
  i = (1 ? 1 / 0 : 1 / 0);
}

/* cp-out: warning: [^:]*: line 5, columns 11-16: division by zero */
/* cp-out: warning: [^:]*: line 5, columns 19-24: division by zero */
