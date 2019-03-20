struct s {
  unsigned long long a:8, b:32;
};

struct s
f(struct s x)
{
  x.b = 0xcdef1234;
  return x;
}

main()
{
  static struct s i;
  i.a = 12;
  i = f(i);
  if (i.a != 12 || i.b != 0xcdef1234)
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 18, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, column 2: identifier "exit" not declared */
