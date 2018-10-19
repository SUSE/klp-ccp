int i;
void
f (void)
{
  i = (1 ? 1 / 0 : 1 / 0);
}

/* cp-out: warning: [^:]*: line 5, columns 15-16: division by zero */
/* cp-out: warning: [^:]*: line 5, columns 23-24: division by zero */
