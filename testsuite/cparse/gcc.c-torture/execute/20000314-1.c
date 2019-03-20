int main ()
{
  long winds = 0;

  while (winds != 0)
    {
      if (*(char *) winds)
	break;
    }

  if (winds == 0 || winds != 0 || *(char *) winds)
    exit (0);

  abort ();
}

/* cp-out: warning: [^:]*: line 12, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 14, column 2: identifier "abort" not declared */
