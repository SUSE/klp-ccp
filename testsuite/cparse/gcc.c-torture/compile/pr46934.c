int caller (unsigned int reg_type)
{
  switch (reg_type)
    {
    case 0x80000000:
      return (int)foo();

    case 0x80000003:
      return (int) bar();

    case 0x80000001:
      return (int) baz();

    case 0x80000004:
      return (int) fooz();
    }
}

/* cp-out: warning: [^:]*: line 6, column 18: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 9, column 19: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 12, column 19: identifier "baz" not declared */
/* cp-out: warning: [^:]*: line 15, column 19: identifier "fooz" not declared */
