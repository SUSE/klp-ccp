void b (int *);
void c (int, int);
void d (int);

int e;

void a (int x, int y)
{
  int f = x ? e : 0;
  int z = y;

  b (&y);
  c (z, y);
  d (f);
}

void b (int *y)
{
  (*y)++;
}

void c (int x, int y)
{
  if (x == y)
    abort ();
}

void d (int x)
{
}

int main (void)
{
  a (0, 0);
  exit (0);
}

/* cp-out: warning: [^:]*: line 25, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 35, column 2: identifier "exit" not declared */
