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

/* cp-out: warning: [^:]*: line 6, columns 4-7: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 11, columns 1-4: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 13, columns 6-10: identifier "foo1" not declared */
