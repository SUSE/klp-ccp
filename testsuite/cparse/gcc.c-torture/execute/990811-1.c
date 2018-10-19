struct s {long a; int b;};

int foo(int x, void *y)
{
  switch(x) {
    case 0: return ((struct s*)y)->a;
    case 1: return *(signed char*)y;
    case 2: return *(short*)y;
  }
  abort();
}

int main ()
{
  struct s s;
  short sh[10];
  signed char c[10];
  int i;

  s.a = 1;
  s.b = 2;
  for (i = 0; i < 10; i++) {
    sh[i] = i;
    c[i] = i;
  }

  if (foo(0, &s) != 1) abort();
  if (foo(1, c+3) != 3) abort();
  if (foo(2, sh+3) != 3) abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 10, columns 2-7: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 27, columns 23-28: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, columns 24-29: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 29, columns 25-30: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, columns 2-6: identifier "exit" not declared */
