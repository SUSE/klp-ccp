main ()
{
  int i = 0;


  if (sizeof (unsigned long int) == 4)
    {
      unsigned long int z = 0;

      do {
	z -= 0x00004000;
	i++;
	if (i > 0x00040000)
	  abort ();
      } while (z > 0);
      exit (0);
    }
  else if (sizeof (unsigned int) == 4)
    {
      unsigned int z = 0;

      do {
	z -= 0x00004000;
	i++;
	if (i > 0x00040000)
	  abort ();
      } while (z > 0);
      exit (0);
    }
  else
    exit (0);
}

/* cp-out: warning: [^:]*: line 14, column 3: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, column 6: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 26, column 3: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, column 6: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 31, column 4: identifier "exit" not declared */
