void foo(int p, int q)
{
  if (p)
    {
      if (q)
        __builtin_trap ();
    }
  else
    bar();
}

/* cp-out: warning: [^:]*: line 9, columns 4-7: identifier "bar" not declared */
