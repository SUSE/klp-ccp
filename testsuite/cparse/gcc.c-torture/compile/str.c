typedef struct
{
  char a;
  char b;
} foo;

bar ()
{
  foo foobar[100];
  foobar[1].a = 'a';
  foobar[2].a = 'b';
  barfoo (foobar);
}

/* cp-out: warning: [^:]*: line 12, columns 2-8: identifier "barfoo" not declared */
