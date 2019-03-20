void foo ()
{
  if (1)
    goto foo;
  else
    for (;;)
      {
      foo:
	bar ();
	return;
      }
}

/* cp-out: warning: [^:]*: line 9, column 1: identifier "bar" not declared */
