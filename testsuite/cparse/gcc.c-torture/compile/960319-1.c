static void
f()
{
  long long a[2];
  int i;
  if (g())
    if (h())
      ;
  *a |= (long long)i << 65 ;
}

/* cp-out: warning: [^:]*: line 6, column 6: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 7, column 8: identifier "h" not declared */
