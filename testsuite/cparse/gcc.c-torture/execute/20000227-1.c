static const unsigned char f[] = "\0\377";
static const unsigned char g[] = "\0ÿ";

int main(void)
{
  if (sizeof f != 3 || sizeof g != 3)
    abort ();
  if (f[0] != g[0])
    abort ();
  if (f[1] != g[1])
    abort ();
  if (f[2] != g[2])
    abort ();
  return 0;
}

/* cp-out: warning: [^:]*: line 7, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 11, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, columns 4-9: identifier "abort" not declared */
/* cp-out: error: [^:]*: line 2, columns 33-38: invalid UTF-8 character encoding */
