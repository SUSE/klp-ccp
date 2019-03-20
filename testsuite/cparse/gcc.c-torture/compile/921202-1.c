f ()
{
  long dx[2055];
  long dy[2055];
  long s1[2055];
  int x, y;
  int i;
  long s;

  for (;;)
    {
      s = 2055;
      g (s1, s);
      for (i = 0; i < 1; i++);
      dy[s] = 0x12345;
      for (i = 0; i < 1; i++);
      if (x != y || h (dx, dy, s) || dx[s] != 0x12345)
	{
	  j (y);k (dy);
	}
    }
}

/* cp-out: warning: [^:]*: line 13, column 6: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 17, column 20: identifier "h" not declared */
/* cp-out: warning: [^:]*: line 19, column 3: identifier "j" not declared */
/* cp-out: warning: [^:]*: line 19, column 9: identifier "k" not declared */
