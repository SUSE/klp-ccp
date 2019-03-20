void p1 (p) int *p;
{ *p &= ~0xff;
}
void p2 (p) int *p;
{ *p &= ~0xff00;
}
void p3 (p) int *p;
{ *p &= ~0xffff0000;
}
void p4 (p) int *p;
{ *p &= ~0xffff;
}

main ()
{
  int a;

  a = 0x12345678;  p1 (&a);  printf ("%x\n", a);
  a = 0x12345678;  p2 (&a);  printf ("%x\n", a);
  a = 0x12345678;  p3 (&a);  printf ("%x\n", a);
  a = 0x12345678;  p4 (&a);  printf ("%x\n", a);
}

/* cp-out: warning: [^:]*: line 18, column 29: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 19, column 29: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 20, column 29: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 21, column 29: identifier "printf" not declared */
