static char *
begfield (int tab, char *ptr, char *lim, int sword, int schar)
{
  if (tab)
    {
      while (ptr < lim && sword--)
	{
	  while (ptr < lim && *ptr != tab)
	    ++ptr;
	  if (ptr < lim)
	    ++ptr;
	}
    }
  else
    {
      while (1)
	;
    }

  if (ptr + schar <= lim)
    ptr += schar;

  return ptr;
}

main ()
{
  char *s = ":ab";
  char *lim = s + 3;
  if (begfield (':', s, lim, 1, 1) != s + 2)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 31, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 32, column 2: identifier "exit" not declared */
