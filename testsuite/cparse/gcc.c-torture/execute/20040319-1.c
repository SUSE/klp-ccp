int
blah (int zzz)
{
  int foo;
  if (zzz >= 0)
    return 1;
  foo = (zzz >= 0 ? (zzz) : -(zzz));
  return foo;
}

main()
{
  if (blah (-1) != 1)
    abort ();
  else
    exit (0);
}

/* cp-out: warning: [^:]*: line 14, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, column 4: identifier "exit" not declared */
