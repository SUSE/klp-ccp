int a[251];
__attribute__ ((noinline))
t(int i)
{
  if (i==0)
    exit(0);
  if (i>255)
    abort ();
}
main()
{
  unsigned int i;
  for (i=0;;i++)
    {
      a[i]=t((unsigned char)(i+5));
    }
}

/* cp-out: warning: [^:]*: line 6, columns 4-8: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 8, columns 4-9: identifier "abort" not declared */
