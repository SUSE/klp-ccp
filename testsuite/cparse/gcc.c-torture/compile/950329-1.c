/* { dg-require-effective-target untyped_assembly } */
f ()
{
  int i;
  for (i = 1;; i = 0)
    {
      if (h ())
	{
	  if (i)
	    g ();
	  g (h ());
	  g (h ());
	}
      else
	{
	  g ();
	  break;
	}
    }
}

/* cp-out: warning: [^:]*: line 7, columns 10-11: identifier "h" not declared */
/* cp-out: warning: [^:]*: line 10, columns 5-6: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 11, columns 3-4: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 11, columns 6-7: identifier "h" not declared */
/* cp-out: warning: [^:]*: line 12, columns 3-4: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 12, columns 6-7: identifier "h" not declared */
/* cp-out: warning: [^:]*: line 16, columns 3-4: identifier "g" not declared */
