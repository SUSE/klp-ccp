typedef struct
{
  short i __attribute__ ((aligned (2),packed));
  int f[2] __attribute__ ((aligned (2),packed));
} A;

f (ap)
  A *ap;
{
  short i, j = 1;

  i = ap->f[1];
  i += ap->f[j];
  for (j = 0; j < 2; j++)
    i += ap->f[j];

  return i;
}

main ()
{
  A a;
  a.f[0] = 100;
  a.f[1] = 13;
  if (f (&a) != 139)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 26, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 27, columns 2-6: identifier "exit" not declared */
