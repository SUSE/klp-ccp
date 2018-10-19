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

/* cp-out: warning: [^:]*: line 3, columns 6-7: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 4, columns 4-5: identifier "h" not declared */
