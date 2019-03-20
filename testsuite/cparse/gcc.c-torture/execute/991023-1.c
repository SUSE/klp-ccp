

int blah;
foo()
{
  int i;

  for (i=0 ; i< 7 ; i++)
    {
      if (i == 7 - 1)
	blah = 0xfcc;
      else
	blah = 0xfee;
    }
  return blah;
}


main()
{
  if (foo () != 0xfcc)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 22, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 23, column 2: identifier "exit" not declared */
