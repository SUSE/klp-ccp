sub (int i, int array[i++])
{
  return i;
}

main()
{
  int array[10];
  exit (sub (10, array) != 11);
}

/* cp-out: warning: [^:]*: line 9, columns 2-6: identifier "exit" not declared */
