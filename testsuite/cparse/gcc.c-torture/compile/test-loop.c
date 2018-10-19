main ()
{
  int i;
  for (i = 100;  i >= -1; i--)
    foo ();

}

/* cp-out: warning: [^:]*: line 5, columns 4-7: identifier "foo" not declared */
