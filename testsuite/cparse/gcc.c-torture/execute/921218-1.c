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

/* cp-out: warning: [^:]*: line 9, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, columns 2-6: identifier "exit" not declared */
