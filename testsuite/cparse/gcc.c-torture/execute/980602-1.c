main()
{
  int i;
  for (i = 1; i < 100; i++)
    ;
  if (i == 100) 
    exit (0);
  abort ();
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 8, column 2: identifier "abort" not declared */
