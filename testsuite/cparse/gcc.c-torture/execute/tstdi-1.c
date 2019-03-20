#define FALSE 140
#define TRUE 13

feq (x)
     long long int x;
{
  if (x == 0)
    return TRUE;
  else
    return FALSE;
}

fne (x)
     long long int x;
{
  if (x != 0)
    return TRUE;
  else
    return FALSE;
}

flt (x)
     long long int x;
{
  if (x < 0)
    return TRUE;
  else
    return FALSE;
}

fge (x)
     long long int x;
{
  if (x >= 0)
    return TRUE;
  else
    return FALSE;
}

fgt (x)
     long long int x;
{
  if (x > 0)
    return TRUE;
  else
    return FALSE;
}

fle (x)
     long long int x;
{
  if (x <= 0)
    return TRUE;
  else
    return FALSE;
}

main ()
{
  if (feq (0LL) != TRUE)
    abort ();
  if (feq (-1LL) != FALSE)
    abort ();
  if (feq (0x8000000000000000LL) != FALSE)
    abort ();
  if (feq (0x8000000000000001LL) != FALSE)
    abort ();
  if (feq (1LL) != FALSE)
    abort ();
  if (feq (0x7fffffffffffffffLL) != FALSE)
    abort ();

  if (fne (0LL) != FALSE)
    abort ();
  if (fne (-1LL) != TRUE)
    abort ();
  if (fne (0x8000000000000000LL) != TRUE)
    abort ();
  if (fne (0x8000000000000001LL) != TRUE)
    abort ();
  if (fne (1LL) != TRUE)
    abort ();
  if (fne (0x7fffffffffffffffLL) != TRUE)
    abort ();

  if (flt (0LL) != FALSE)
    abort ();
  if (flt (-1LL) != TRUE)
    abort ();
  if (flt (0x8000000000000000LL) != TRUE)
    abort ();
  if (flt (0x8000000000000001LL) != TRUE)
    abort ();
  if (flt (1LL) != FALSE)
    abort ();
  if (flt (0x7fffffffffffffffLL) != FALSE)
    abort ();

  if (fge (0LL) != TRUE)
    abort ();
  if (fge (-1LL) != FALSE)
    abort ();
  if (fge (0x8000000000000000LL) != FALSE)
    abort ();
  if (fge (0x8000000000000001LL) != FALSE)
    abort ();
  if (fge (1LL) != TRUE)
    abort ();
  if (fge (0x7fffffffffffffffLL) != TRUE)
    abort ();

  if (fgt (0LL) != FALSE)
    abort ();
  if (fgt (-1LL) != FALSE)
    abort ();
  if (fgt (0x8000000000000000LL) != FALSE)
    abort ();
  if (fgt (0x8000000000000001LL) != FALSE)
    abort ();
  if (fgt (1LL) != TRUE)
    abort ();
  if (fgt (0x7fffffffffffffffLL) != TRUE)
    abort ();

  if (fle (0LL) != TRUE)
    abort ();
  if (fle (-1LL) != TRUE)
    abort ();
  if (fle (0x8000000000000000LL) != TRUE)
    abort ();
  if (fle (0x8000000000000001LL) != TRUE)
    abort ();
  if (fle (1LL) != FALSE)
    abort ();
  if (fle (0x7fffffffffffffffLL) != FALSE)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 61, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 63, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 65, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 67, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 69, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 71, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 74, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 76, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 78, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 80, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 82, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 84, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 87, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 89, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 91, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 93, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 95, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 97, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 100, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 102, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 104, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 106, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 108, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 110, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 113, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 115, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 117, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 119, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 121, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 123, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 126, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 128, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 130, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 132, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 134, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 136, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 138, column 2: identifier "exit" not declared */
