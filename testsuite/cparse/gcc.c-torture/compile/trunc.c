main ()
{
  printf ("%x, %x\n", (unsigned char) main, main);
}

foo (p)
     char *p;
{
  p[0] = (char)foo;
  p[1] = (char)foo;
}

/* cp-out: warning: [^:]*: line 3, column 2: identifier "printf" not declared */
