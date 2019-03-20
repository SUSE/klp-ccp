char *
f (char *p)
{
  short x = *p++ << 16;
  return p;
}

main ()
{
  char *p = "";
  if (f (p) != p + 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 12, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 2: identifier "exit" not declared */
