/* { dg-require-effective-target untyped_assembly } */
static int
func1 (int a, int b)
{
  return b ? a : a / b;
}

static unsigned char
func2 (unsigned char a, int b)
{
  return b ? a : b;
}

int i;

void
func3 (const int arg)
{
  for (i = 0; i != 10; i = foo ())
    {
      if (!arg)
	{
	  int j;
	  for (j = 0; j < 5; j += 1)
	    {
	      int *ptr;
	      *ptr = func2 (func1 (arg, *ptr), foo (arg));
	    }
	}
    }
}

/* cp-out: warning: [^:]*: line 19, columns 27-30: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 27, columns 40-43: identifier "foo" not declared */
