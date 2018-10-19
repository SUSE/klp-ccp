
struct tag {
  int m1;
  char *m2[5];
} s1, *p1;

int i;

main()
{
  s1.m1 = -1;
  p1 = &s1;

  if ( func1( &p1->m1 ) == -1 )
    foo ("ok");
  else
    abort ();

  i = 3;
  s1.m2[3]= "123";

  if ( strlen( (p1->m2[i])++ ) == 3 )
    foo ("ok");
  else
    abort ();

  exit (0);
}

func1(int *p) { return(*p); }

foo (char *s) {}

/* cp-out: warning: [^:]*: line 14, columns 7-12: identifier "func1" not declared */
/* cp-out: warning: [^:]*: line 15, columns 4-7: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 17, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 7-13: identifier "strlen" not declared */
/* cp-out: warning: [^:]*: line 23, columns 4-7: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 25, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 27, columns 2-6: identifier "exit" not declared */
