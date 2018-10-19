int
foo(void)
{
  int      a,b,g;
  int      i,len;
  int      stop;
                                                                     
  len = 10;
  stop = 0;
  for (i=0; i<len; i++)
  {
    a = bar1() ? 0 : 1;
    b = bar2() ? 0 : 1;
    g = bar3() ? 0 : 1;
                                                                     
    if (stop = ((a+b) % 2 != g)) break;
  }
 
  return stop;
}

/* cp-out: warning: [^:]*: line 12, columns 8-12: identifier "bar1" not declared */
/* cp-out: warning: [^:]*: line 13, columns 8-12: identifier "bar2" not declared */
/* cp-out: warning: [^:]*: line 14, columns 8-12: identifier "bar3" not declared */
