struct foo
{
  char a;
};

foo ()
{
  struct foo bar[3];
  bar[0].a = '0';
  bar[1].a = '1';
  bar[2].a = '2';
  foof (bar);
}

/* cp-out: warning: [^:]*: line 12, column 2: identifier "foof" not declared */
