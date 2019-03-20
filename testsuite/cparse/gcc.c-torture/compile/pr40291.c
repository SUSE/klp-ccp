/* PR middle-end/40291 */

int
foo (void *x, char *y, unsigned long long z)
{
  return memcmp (x, y, z);
}

/* cp-out: warning: [^:]*: line 6, column 9: identifier "memcmp" not declared */
