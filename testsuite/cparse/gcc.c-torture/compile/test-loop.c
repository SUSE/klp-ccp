main ()
{
  int i;
  for (i = 100;  i >= -1; i--)
    foo ();

}

/* cp-out: warning: [^:]*: line 5, column 4: identifier "foo" not declared */
