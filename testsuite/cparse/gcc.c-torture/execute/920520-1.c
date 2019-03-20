foo(int *bar)
{
  *bar = 8;
}

bugger()
{
  int oldDepth, newDepth;

  foo(&oldDepth);

  switch (oldDepth)
    {
    case 8:
    case 500:
      newDepth = 8;
      break;

    case 5000:
      newDepth = 500;
      break;

    default:
      newDepth = 17;
      break;
    }

  return newDepth - oldDepth;
}

main()
{
  if (bugger() != 0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 34, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 35, column 2: identifier "exit" not declared */
