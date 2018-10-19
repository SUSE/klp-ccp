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

/* cp-out: warning: [^:]*: line 6, columns 6-7: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 7, columns 8-9: identifier "h" not declared */
