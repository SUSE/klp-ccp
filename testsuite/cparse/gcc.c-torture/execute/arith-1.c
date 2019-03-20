unsigned
sat_add (unsigned i)
{
  unsigned ret = i + 1;
  if (ret < i)
    ret = i;
  return ret;
}

unsigned
sat_add2 (unsigned i)
{
  unsigned ret = i + 1;
  if (ret > i)
    return ret;
  return i;
}

unsigned
sat_add3 (unsigned i)
{
  unsigned ret = i - 1;
  if (ret > i)
    ret = i;
  return ret;
}

unsigned
sat_add4 (unsigned i)
{
  unsigned ret = i - 1;
  if (ret < i)
    return ret;
  return i;
}
main ()
{
  if (sat_add (~0U) != ~0U)
    abort ();
  if (sat_add2 (~0U) != ~0U)
    abort ();
  if (sat_add3 (0U) != 0U)
    abort ();
  if (sat_add4 (0U) != 0U)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 39, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 41, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 43, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 45, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 46, column 2: identifier "exit" not declared */
