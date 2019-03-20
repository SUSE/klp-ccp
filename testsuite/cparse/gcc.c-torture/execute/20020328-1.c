int b = 0;

func () { }

void
testit(int x)
{
  if (x != 20)
    abort ();
}

int
main()

{
  int a = 0;

  if (b)
    func();

  /* simplify_and_const_int would incorrectly omit the mask in
     the line below.  */
  testit ((a + 23) & 0xfffffffc);
  exit (0);
}

/* cp-out: warning: [^:]*: line 9, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, column 2: identifier "exit" not declared */
