f(x, d1, d2, d3)
   double d1, d2, d3;
{
   return x;
}

g(b,s,x,y,i,j)
     char *b,*s;
     double x,y;
{
  if (x != 1.0 || y != 2.0 || i != 3 || j != 4)
    abort();
}

main()
{
  g("","", 1.0, 2.0, f(3, 0.0, 0.0, 0.0), f(4, 0.0, 0.0, 0.0));
  exit(0);
}

/* cp-out: warning: [^:]*: line 12, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, column 2: identifier "exit" not declared */
