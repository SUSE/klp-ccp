f (cp, end)
     char *cp;
     char *end;
{
  return (cp < end);
}

main ()
{
  if (! f ((char *) 0, (char *) 1))
    abort();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
