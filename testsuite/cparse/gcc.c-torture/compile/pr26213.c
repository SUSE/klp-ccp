void
xnanosleep (_Bool overflow)
{
  struct { int tv_nsec; } ts_sleep;
  if (0 <= ts_sleep.tv_nsec)
    overflow |= 1;

  for (;;)
    {
      if (overflow)
        ts_sleep.tv_nsec = 0;
      if (foo (ts_sleep))
        break;
    }
}

/* cp-out: warning: [^:]*: line 12, columns 10-13: identifier "foo" not declared */
