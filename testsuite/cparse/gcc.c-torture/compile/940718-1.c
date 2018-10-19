extern double log (double) __attribute__ ((const));

f (double x)
{
  for (;;)
    exp(log(x));
}

/* cp-out: warning: [^:]*: line 6, columns 4-7: identifier "exp" not declared */
