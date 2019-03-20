int a[2] = { 18, 6 };

int main ()
{
  int b = (-3 * a[0] -3 * a[1]) / 12;
  if (b != -6)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 8, column 2: identifier "exit" not declared */
