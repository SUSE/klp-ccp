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

/* cp-out: warning: [^:]*: line 14, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, columns 2-6: identifier "exit" not declared */
