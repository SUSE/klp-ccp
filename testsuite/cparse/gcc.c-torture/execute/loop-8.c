double a[3] = { 0.0, 1.0, 2.0 };

void bar (int x, double *y)
{
  if (x || *y != 1.0)
    abort ();
}

int main ()
{
  double c;
  int d;
  for (d = 0; d < 3; d++)
  {
    c = a[d];
    if (c > 0.0) goto e;
  }
  bar(1, &c);
  exit (1);
e:
  bar(0, &c);
  exit (0);
}

/* cp-out: warning: [^:]*: line 6, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, columns 2-6: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 22, columns 2-6: identifier "exit" not declared */
