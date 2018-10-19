/* { dg-require-effective-target label_values } */

int
x (int i)
{
  static const void *j[] = {&& x, && y, && z};
  
  goto *j[i];
  
 x: return 2;
 y: return 3;
 z: return 5;
}

int
main (void)
{
  if (   x (0) != 2
      || x (1) != 3
      || x (2) != 5)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 21, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 23, columns 2-6: identifier "exit" not declared */
