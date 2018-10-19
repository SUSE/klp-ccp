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

/* cp-out: warning: [^:]*: line 8, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, columns 2-6: identifier "exit" not declared */
