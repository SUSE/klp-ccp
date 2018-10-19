main()
{
  int i;
  for (i = 1; i < 100; i++)
    ;
  if (i == 100) 
    exit (0);
  abort ();
}

/* cp-out: warning: [^:]*: line 7, columns 4-8: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 8, columns 2-7: identifier "abort" not declared */
