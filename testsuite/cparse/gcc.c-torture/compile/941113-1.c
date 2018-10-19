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

/* cp-out: warning: [^:]*: line 11, columns 4-5: identifier "g" not declared */
