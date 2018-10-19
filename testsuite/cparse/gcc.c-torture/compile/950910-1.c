f (char *p)
{
  char c;

  c = *++p;
  if (c != ' ')
    return 0;
  for (;;)
    {
      c = *p;
      if (g (c))
	p++;
      else
	{
	  if (c == ' ')
	    break;
	  else
	    return 0;
	}
    }
  return 1;
}

/* cp-out: warning: [^:]*: line 11, columns 10-11: identifier "g" not declared */
