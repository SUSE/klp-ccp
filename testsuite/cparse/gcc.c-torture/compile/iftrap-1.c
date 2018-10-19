/* Verify that ifcvt doesn't crash under a number of interesting conditions. */

void f1(int p)
{
  if (p)
    __builtin_trap();
}

void f2(int p)
{
  if (p)
    __builtin_trap();
  else
    bar();
}

void f3(int p)
{
  if (p)
    bar();
  else
    __builtin_trap();
}

void f4(int p, int q)
{
  if (p)
    {
      bar();
      if (q)
	bar();
    }
  else
    __builtin_trap();
}

void f5(int p)
{
  if (p)
    __builtin_trap();
  else
    abort();
}

void f6(int p)
{
  if (p)
    abort();
  else
    __builtin_trap();
}

void f7(int p)
{
  if (p)
    __builtin_trap();
  else
    __builtin_trap();
}

void f8(int p)
{
  if (p)
    __builtin_trap();
  else
    {
      bar();
      __builtin_trap();
    }
}

void f9(int p)
{
  if (p)
    {
      bar();
      __builtin_trap();
    }
  else
    __builtin_trap();
}

void f10(int p)
{
  if (p)
    __builtin_trap();
  while (1)
    bar();
}

void f11(int p)
{
  if (p)
    __builtin_trap();
  else
    bar();
  while (1)
    baz();
}

/* cp-out: warning: [^:]*: line 14, columns 4-7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 20, columns 4-7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 29, columns 6-9: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 31, columns 1-4: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 42, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 48, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 67, columns 6-9: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 76, columns 6-9: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 88, columns 4-7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 96, columns 4-7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 98, columns 4-7: identifier "baz" not declared */

