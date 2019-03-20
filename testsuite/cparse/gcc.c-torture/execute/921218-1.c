f()
{
  return (unsigned char)("\377"[0]);
}

main()
{
  if (f() != (unsigned char)(0377))
    abort();
  exit (0);
}

/* cp-out: warning: [^:]*: line 9, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, column 2: identifier "exit" not declared */
