main()
{
  if ((signed int) 1 < (signed int) -2147483648)
    printf("true\n");
}

/* cp-out: warning: [^:]*: line 4, column 4: identifier "printf" not declared */
