void f(long i)
{
  if ((signed char)i < 0 || (signed char)i == 0) 
    abort ();
  else
    exit (0);
}

main()
{
  f(0xffffff01);
}

/* cp-out: warning: [^:]*: line 4, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 6, column 4: identifier "exit" not declared */
