foo (b, c)
     unsigned b, c;
{
  return (b << 12) | (c >> 20);
}

main ()
{
  printf ("0x%x\n", foo (0x11223344, 0xaabbccdd));
}

/* cp-out: warning: [^:]*: line 9, columns 2-8: identifier "printf" not declared */
