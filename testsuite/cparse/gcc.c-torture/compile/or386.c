typedef int xtype;

xtype
foo (a)
     xtype a;
{
  return a | 0x7f;
}

main ()
{
  printf ("%08x\n", foo (-1));
}

/* cp-out: warning: [^:]*: line 12, columns 2-8: identifier "printf" not declared */
