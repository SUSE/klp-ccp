enum bar
{
  one,
  two
};

enum bar foo;

void bar()
{
  switch (foo)
  {
    case one:
    case two:
      printf ("one to two\n");
    break;
  }
}

/* cp-out: warning: [^:]*: line 15, column 6: identifier "printf" not declared */
