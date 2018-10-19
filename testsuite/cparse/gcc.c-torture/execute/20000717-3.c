int c = -1;

foo (p)
     int *p;
{
  int x;
  int a;

  a = p[0];
  x = a + 5;
  a = c;
  p[0] = x - 15;
  return a;
}

int main()
{
   int b = 1;
   int a = foo(&b);

   if (a != -1 || b != (1 + 5 - 15))
     abort ();

   exit (0);
}

/* cp-out: warning: [^:]*: line 22, columns 5-10: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, columns 3-7: identifier "exit" not declared */
