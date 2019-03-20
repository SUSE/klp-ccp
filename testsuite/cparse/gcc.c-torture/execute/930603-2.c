int w[2][2];

f ()
{
  int i, j;

  for (i = 0; i < 2; i++)
    for (j = 0; j < 2; j++)
      if (i == j)
	w[i][j] = 1;
}

main ()
{
  f ();
  if (w[0][0] != 1 || w[1][1] != 1 || w[1][0] != 0 || w[0][1] != 0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 17, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, column 2: identifier "exit" not declared */
