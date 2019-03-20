void *eintr_source (void *arg)
{
  int ts = 0;
  
  if (arg)
    foo ();
  
  while (1)
    {
      if (arg)
	foo ();
      
      foo1 (&ts);
    }
  
  return 0;
}

/* cp-out: warning: [^:]*: line 6, column 4: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 11, column 1: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 13, column 6: identifier "foo1" not declared */
