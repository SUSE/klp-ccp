int count = 0;
int dummy;

static int *
bar(void)
{
  ++count;
  return &dummy;
}

static void
foo(void)
{
  asm("" : "+r"(*bar()));
}

main()
{
  foo();
  if (count != 1)
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 21, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, column 2: identifier "exit" not declared */
