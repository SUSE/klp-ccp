void *foo[]={(void *)&("X"[0])};

main ()
{
  if (((char*)foo[0])[0] != 'X')
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 6, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 7, column 2: identifier "exit" not declared */
