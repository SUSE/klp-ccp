int main(void)
{
  char *c1 = "foo";
  char *c2 = "foo";
  int i;
  for (i = 0; i < 3; i++)
    if (c1[i] != c2[i])
      abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, column 6: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, column 2: identifier "exit" not declared */
