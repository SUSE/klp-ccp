void zgemm_ (const int*, const double*);
extern void matmul_c8 (_Complex double * dest)
{
  const int  ldc = 0;
  const double zero = 0;
  zgemm_ ( &zero, &ldc);
  dest[1] += 1 ;
}

/* cp-out: warning: [^:]*: line 6, columns 11-16: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 6, columns 18-22: incompatible pointer types in assignment */
