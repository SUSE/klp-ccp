void
f (void)
{
  0 || 65536*65536 ? : 0;
}

/* cp-out: warning: [^:]*: line 4, columns 7-18: integer overflow in multiplication */
