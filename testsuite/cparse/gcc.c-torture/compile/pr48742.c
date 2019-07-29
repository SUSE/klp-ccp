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

/* cp-out: warning: [^:]*: line 8, columns 9-14: division by zero */
/* cp-out: warning: [^:]*: line 14, columns 12-37: division by zero */
