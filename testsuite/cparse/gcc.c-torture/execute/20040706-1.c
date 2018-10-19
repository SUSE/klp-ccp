int main ()
{
  int i;
  for (i = 0; i < 10; i++)
    continue;
  if (i < 10)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 7, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 8, columns 2-6: identifier "exit" not declared */
