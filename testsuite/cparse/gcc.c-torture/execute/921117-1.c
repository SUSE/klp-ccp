struct s {
  char text[11];
  int flag;
} cell;

int
check (struct s p)
{
  if (p.flag != 99)
    return 1;
  return strcmp (p.text, "0123456789");
}

main ()
{
  cell.flag = 99;
  strcpy (cell.text, "0123456789");

  if (check (cell))
    abort();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, column 9: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 17, column 2: identifier "strcpy" not declared */
/* cp-out: warning: [^:]*: line 20, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 2: identifier "exit" not declared */
