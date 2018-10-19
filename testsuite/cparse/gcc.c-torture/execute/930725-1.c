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

/* cp-out: warning: [^:]*: line 18, columns 7-13: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 19, columns 4-8: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 20, columns 2-7: identifier "abort" not declared */
