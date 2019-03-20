extern char bpp;

void foo()
{
  if (bpp == 32)
    {
      if (2 < 8)
	{
	  do
	    {
	      while (inb(0x9ae8) & (0x0100 >> (2 +1)));
	    }
	  while(0);
	}
      else
	{
	  do
	    {
	      while (inb(0x9ae8) & (0x0100 >> (2)));
	    }
	  while(0);
	}
    }
  else
    do
      { 
	while (inb(0x9ae8) & (0x0100 >> (1)));
      }
    while(0);
  if (8 < 8)
    {
      do
	{
	  while (inb(0x9ae8) & (0x0100 >> (8 +1)));
	}
      while(0);
    }
}

/* cp-out: warning: [^:]*: line 11, column 14: identifier "inb" not declared */
/* cp-out: warning: [^:]*: line 19, column 14: identifier "inb" not declared */
/* cp-out: warning: [^:]*: line 27, column 8: identifier "inb" not declared */
/* cp-out: warning: [^:]*: line 34, column 10: identifier "inb" not declared */
