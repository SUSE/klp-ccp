/* { dg-require-effective-target alloca } */
#define alloca __builtin_alloca

x (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, x, y)
{
  foo (alloca (8));
  return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+x+y;
}

/* cp-out: warning: [^:]*: line 6, columns 2-5: identifier "foo" not declared */
