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

/* cp-out: warning: [^:]*: line 61, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 63, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 65, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 67, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 69, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 71, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 74, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 76, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 78, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 80, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 82, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 84, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 87, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 89, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 91, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 93, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 95, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 97, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 100, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 102, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 104, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 106, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 108, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 110, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 113, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 115, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 117, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 119, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 121, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 123, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 126, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 128, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 130, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 132, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 134, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 136, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 138, columns 2-6: identifier "exit" not declared */
