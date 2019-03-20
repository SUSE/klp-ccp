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

/* cp-out: warning: [^:]*: line 7, column 10: identifier "h" not declared */
/* cp-out: warning: [^:]*: line 10, column 5: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 11, column 3: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 11, column 6: identifier "h" not declared */
/* cp-out: warning: [^:]*: line 12, column 3: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 12, column 6: identifier "h" not declared */
/* cp-out: warning: [^:]*: line 16, column 3: identifier "g" not declared */
