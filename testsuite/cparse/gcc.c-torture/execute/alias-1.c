int val;

int *ptr = &val;
float *ptr2 = &val;

__attribute__((optimize ("-fno-strict-aliasing")))
typepun ()
{
  *ptr2=0;
}

main()
{
  *ptr=1;
  typepun ();
  if (*ptr)
    __builtin_abort ();
}

/* cp-out: warning: [^:]*: line 4, columns 14-15: incompatible pointer types in assignment */
