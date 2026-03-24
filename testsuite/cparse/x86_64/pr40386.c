/* { dg-options "-fno-ira-share-spill-slots -Wno-shift-overflow" } */

extern void abort (void);
extern void exit (int);

#define CHAR_BIT 8

#define ROR(a,b) (((a) >> (b)) | ((a) << ((sizeof (a) * CHAR_BIT) - (b))))
#define ROL(a,b) (((a) << (b)) | ((a) >> ((sizeof (a) * CHAR_BIT) - (b))))

#define CHAR_VALUE ((char)0xf234)
#define SHORT_VALUE ((short)0xf234)
#define INT_VALUE ((int)0xf234)
#define LONG_VALUE ((long)0xf2345678L)
#define LL_VALUE ((long long)0xf2345678abcdef0LL)

#define SHIFT1 4
#define SHIFT2 ((sizeof (long long) * CHAR_BIT) - SHIFT1)

char c = CHAR_VALUE;
short s = SHORT_VALUE;
int i = INT_VALUE;
long l = LONG_VALUE;
long long ll = LL_VALUE;
int shift1 = SHIFT1;
int shift2 = SHIFT2;

int
main ()
{
  if (ROR (c, shift1) != ROR (CHAR_VALUE, SHIFT1))
    abort ();

  if (ROR (c, SHIFT1) != ROR (CHAR_VALUE, SHIFT1))
    abort ();

  if (ROR (s, shift1) != ROR (SHORT_VALUE, SHIFT1))
    abort ();

  if (ROR (s, SHIFT1) != ROR (SHORT_VALUE, SHIFT1))
    abort ();

  if (ROR (i, shift1) != ROR (INT_VALUE, SHIFT1))
    abort ();

  if (ROR (i, SHIFT1) != ROR (INT_VALUE, SHIFT1))
    abort ();

  if (ROR (l, shift1) != ROR (LONG_VALUE, SHIFT1))
    abort ();

  if (ROR (l, SHIFT1) != ROR (LONG_VALUE, SHIFT1))
    abort ();

  if (ROR (ll, shift1) != ROR (LL_VALUE, SHIFT1))
    abort ();

  if (ROR (ll, SHIFT1) != ROR (LL_VALUE, SHIFT1))
    abort ();

  if (ROR (ll, shift2) != ROR (LL_VALUE, SHIFT2))
    abort ();

  if (ROR (ll, SHIFT2) != ROR (LL_VALUE, SHIFT2))
    abort ();

  if (ROL (c, shift1) != ROL (CHAR_VALUE, SHIFT1))
    abort ();

  if (ROL (c, SHIFT1) != ROL (CHAR_VALUE, SHIFT1))
    abort ();

  if (ROL (s, shift1) != ROL (SHORT_VALUE, SHIFT1))
    abort ();

  if (ROL (s, SHIFT1) != ROL (SHORT_VALUE, SHIFT1))
    abort ();

  if (ROL (i, shift1) != ROL (INT_VALUE, SHIFT1))
    abort ();

  if (ROL (i, SHIFT1) != ROL (INT_VALUE, SHIFT1))
    abort ();

  if (ROL (l, shift1) != ROL (LONG_VALUE, SHIFT1))
    abort ();

  if (ROL (l, SHIFT1) != ROL (LONG_VALUE, SHIFT1))
    abort ();

  if (ROL (ll, shift1) != ROL (LL_VALUE, SHIFT1))
    abort ();

  if (ROL (ll, SHIFT1) != ROL (LL_VALUE, SHIFT1))
    abort ();

  if (ROL (ll, shift2) != ROL (LL_VALUE, SHIFT2))
    abort ();

  if (ROL (ll, SHIFT2) != ROL (LL_VALUE, SHIFT2))
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 20, column 9: integer overflow in cast */
/* cp-out: warning: [^:]*: line 21, column 10: integer overflow in cast */
/* cp-out: warning: [^:]*: line 31, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 31, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 31, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 34, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 34, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 34, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 37, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 37, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 37, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 40, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 40, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 40, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 43, columns 25-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 46, columns 25-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 49, columns 25-49: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 52, columns 25-49: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 55, columns 26-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 58, columns 26-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 61, columns 26-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 64, columns 26-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 67, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 67, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 67, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 70, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 70, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 70, columns 25-49: integer overflow in cast */
/* cp-out: warning: [^:]*: line 73, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 73, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 73, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 76, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 76, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 76, columns 25-50: integer overflow in cast */
/* cp-out: warning: [^:]*: line 91, columns 26-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 94, columns 26-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 97, columns 26-48: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 100, columns 26-48: integer overflow in shift operation */
