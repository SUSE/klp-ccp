struct F { int x; int y; };

int main()
{
  int timeout = 0;
  int x = 0;
  while (1)
    {
      const struct F i = { x++, };
      if (i.x > 0)
	break;
      if (++timeout > 5)
	goto die;
    }
  return 0;
 die:
  abort ();
}

/* cp-out: warning: [^:]*: line 17, columns 2-7: identifier "abort" not declared */
