f (b, c)
     unsigned char *b;
     int c;
{
  unsigned long v = 0;
  switch (c)
    {
    case 'd':
      v = ((unsigned long)b[0] << 8) + b[1];
      v >>= 9;
      break;

    case 'k':
      v = b[3] >> 4;
      break;

    default:
      abort ();
    }

  return v;
}
main ()
{
  char buf[4];
  buf[0] = 170; buf[1] = 5;
  if (f (buf, 'd') != 85)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 18, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 29, columns 2-6: identifier "exit" not declared */
