int a,b;
main()
{
  int c=-2;
  int d=0xfe;
  int e=a&1;
  int f=b&2;
  if ((char)(c|(e&f)) == (char)d)
    return 0;
  else
    abort();
}

/* cp-out: warning: [^:]*: line 11, columns 4-9: identifier "abort" not declared */

