struct { long f8:8; long f24:24; } a;
struct { long f32:32; } b;

main ()
{
  if (sizeof (a) != sizeof (b))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 8, column 2: identifier "exit" not declared */
