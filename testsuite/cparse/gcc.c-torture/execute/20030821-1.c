extern void abort (void);

int
foo (int x)
{
  if ((int) (x & 0x80ffffff) != (int) (0x8000fffe))
    abort ();

  return 0;
}

int
main ()
{
  return foo (0x8000fffe);
}

/* cp-out: warning: [^:]*: line 6, columns 32-33: integer overflow in cast */
