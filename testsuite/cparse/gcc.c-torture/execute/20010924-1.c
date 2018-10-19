/* Verify that flexible arrays can be initialized from STRING_CST
   constructors. */

/* Baselines.  */
struct {
  char a1c;
  char *a1p;
} a1 = {
  '4',
  "62"
};

struct {
  char a2c;
  char a2p[2];
} a2 = {
  'v',
  "cq"
};

/* The tests.  */
struct {
  char a3c;
  char a3p[];
} a3 = {
  'o',
  "wx"
};

struct {
  char a4c;
  char a4p[];
} a4 = {
  '9',
  { 'e', 'b' }
};

main()
{
  if (a1.a1c != '4')
    abort();
  if (a1.a1p[0] != '6')
    abort();
  if (a1.a1p[1] != '2')
    abort();
  if (a1.a1p[2] != '\0')
    abort();

  if (a2.a2c != 'v')
    abort();
  if (a2.a2p[0] != 'c')
    abort();
  if (a2.a2p[1] != 'q')
    abort();

  if (a3.a3c != 'o')
    abort();
  if (a3.a3p[0] != 'w')
    abort();
  if (a3.a3p[1] != 'x')
    abort();

  if (a4.a4c != '9')
    abort();
  if (a4.a4p[0] != 'e')
    abort();
  if (a4.a4p[1] != 'b')
    abort();

  return 0;
}

/* cp-out: warning: [^:]*: line 41, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 43, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 45, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 47, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 50, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 52, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 54, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 57, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 59, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 61, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 64, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 66, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 68, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, columns 2-6: excess characters in array initialization */
