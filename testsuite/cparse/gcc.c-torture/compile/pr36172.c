int f(float * );
__SIZE_TYPE__ FcCharSetFreeze (int *fcs, int b)
{
  int i;
  int a = 0;
  for (i = 0; i < *fcs; i++)
  {
    float *leaf = (float *)fcs;
    int hash = f (leaf);
    if (hash)
      a = b;
    if (!a)
      return;
  }
  return (__SIZE_TYPE__) fcs;
}

/* cp-out: warning: [^:]*: line 13, columns 6-12: return without value in function returning non-void */
