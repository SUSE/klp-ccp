/* Verify unaligned address aliasing on Alpha EV[45].  */

static unsigned short x, y;

void foo()
{
  x = 0x345;
  y = 0x567;
}

int main()
{
  foo ();
  if (x != 0x345 || y != 0x567)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 15, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, columns 2-6: identifier "exit" not declared */
