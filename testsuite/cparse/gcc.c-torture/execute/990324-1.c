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

/* cp-out: warning: [^:]*: line 4, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 6, columns 4-8: identifier "exit" not declared */
