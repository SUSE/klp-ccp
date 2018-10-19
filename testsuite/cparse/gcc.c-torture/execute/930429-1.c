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

/* cp-out: warning: [^:]*: line 12, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, columns 2-6: identifier "exit" not declared */
