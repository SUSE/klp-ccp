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

/* cp-out: warning: [^:]*: line 9, column 4: identifier "bar" not declared */
