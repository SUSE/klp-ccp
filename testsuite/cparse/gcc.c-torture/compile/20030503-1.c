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

/* cp-out: warning: [^:]*: line 9, columns 1-4: identifier "bar" not declared */
