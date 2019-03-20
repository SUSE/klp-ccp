typedef struct {
 int f[8];
} T;

f (w, l, r)
     T *w;
     unsigned short l, r;
{
  int i;

  for (i = l; i < r; i++)
    g (w->f[i]);
}

/* cp-out: warning: [^:]*: line 12, column 4: identifier "g" not declared */
