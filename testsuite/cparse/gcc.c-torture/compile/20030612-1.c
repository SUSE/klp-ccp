static inline void
foo (long long const v0, long long const v1)
{
  bar (v0 == v1);
}

void
test (void)
{
  foo (0, 1);
}

/* cp-out: warning: [^:]*: line 4, columns 2-5: identifier "bar" not declared */
