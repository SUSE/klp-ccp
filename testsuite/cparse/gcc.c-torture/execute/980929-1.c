void f(int i)
{
  if (i != 1000)
    abort ();
}


int main()
{
  int n=1000;
  int i;

  f(n);
  for(i=0; i<1; ++i) {
    f(n);
    n=666;
    &n;
  }

  exit (0);
}

/* cp-out: warning: [^:]*: line 4, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, columns 2-6: identifier "exit" not declared */
