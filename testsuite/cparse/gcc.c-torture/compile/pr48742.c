/* PR c/48742 */

void baz (int);

int
foo (void)
{
  return 1 / 0 > 0;
}

void
bar (void)
{
  baz (1 <= 2 % (3 >> 1 > 5 / 6 == 3));
}

/* cp-out: warning: [^:]*: line 8, columns 13-14: division by zero */
