typedef void foo (void);

f (x)
{
  if (x)
    {
      const foo* v;
      (*v)();
    }
  else
    g (0);
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "g" not declared */
