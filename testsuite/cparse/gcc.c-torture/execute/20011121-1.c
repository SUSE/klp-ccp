struct s
{
  int i[18];
  char f;
  char b[2];
};

struct s s1;

int
main()
{
  struct s s2;
  s2.b[0] = 100;
  __builtin_memcpy(&s2, &s1, ((unsigned int) &((struct s *)0)->b));
  if (s2.b[0] != 100)
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 17, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, columns 2-6: identifier "exit" not declared */
