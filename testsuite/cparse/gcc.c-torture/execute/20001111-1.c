
static int next_buffer = 0;
void bar (void);

static int t = 1, u = 0;

long
foo (unsigned int offset)
{
  unsigned i, buffer;
  int x;
  char *data;

  i = u;
  if (i)
    return i * 0xce2f;

  buffer = next_buffer;
  data = buffer * 0xce2f;
  for (i = 0; i < 2; i++)
    bar ();
  buffer = next_buffer;
  return buffer * 0xce2f + offset;

}

void
bar (void)
{
}

int
main ()
{
  if (foo (3) != 3)
    abort ();
  next_buffer = 1;
  if (foo (2) != 0xce2f + 2)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 36, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 39, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 40, column 2: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 19, columns 9-24: assignment to pointer from integer which is not NULL */
