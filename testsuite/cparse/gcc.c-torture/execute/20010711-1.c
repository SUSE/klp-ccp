void foo (int *a) {}

int main ()
{
  int a;
  if (&a == 0)
    abort ();
  else
    {
      foo (&a);
      exit (0);
    }
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 11, column 6: identifier "exit" not declared */
