int v;

char *
g ()
{
  return "";
}

char *
f ()
{
  return (v == 0 ? g () : "abc");
}

main ()
{
  v = 1;
  if (!strcmp (f (), "abc"))
    exit (0);
  abort();
}

/* cp-out: warning: [^:]*: line 18, column 7: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 19, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 20, column 2: identifier "abort" not declared */
