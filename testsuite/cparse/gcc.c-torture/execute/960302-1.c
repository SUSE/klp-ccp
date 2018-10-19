long a = 1;

foo ()
{
  switch (a % 2 % 2 % 2 % 2 % 2 % 2 % 2 % 2)
    {
    case 0:
      return 0;
    case 1:
      return 1;
    default:
      return -1;
    }
}

main ()
{
  if (foo () != 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 19, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, columns 2-6: identifier "exit" not declared */
