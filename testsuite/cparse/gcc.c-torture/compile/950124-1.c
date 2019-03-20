f ()
{
  if (g ())
    h ();
  else
    {
      do
	{
	  return 0;
	  break;
	}
      while (1);
    }
  return 1;
}

/* cp-out: warning: [^:]*: line 3, column 6: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 4, column 4: identifier "h" not declared */
