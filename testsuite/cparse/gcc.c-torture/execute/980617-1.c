void foo (unsigned int * p)
{
  if ((signed char)(*p & 0xFF) == 17 || (signed char)(*p & 0xFF) == 18)
    return;
  else
    abort ();
}

int main ()
{
  int i = 0x30011;
  foo(&i);
  exit (0);
}

/* cp-out: warning: [^:]*: line 6, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 2: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 12, columns 6-8: pointers to different integer types in assignment */
