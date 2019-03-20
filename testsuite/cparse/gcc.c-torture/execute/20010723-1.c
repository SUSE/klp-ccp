int
test ()
{
  int biv,giv;
  for (biv = 0, giv = 0; giv != 8; biv++)
      giv = biv*8;
  return giv;
}


main()
{
  if (test () != 8)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 14, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, column 2: identifier "exit" not declared */
