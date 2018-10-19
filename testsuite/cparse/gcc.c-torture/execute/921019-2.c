main()
{
  double x,y=0.5;
  x=y/0.2;
  if(x!=x)
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 6, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 7, columns 2-6: identifier "exit" not declared */
