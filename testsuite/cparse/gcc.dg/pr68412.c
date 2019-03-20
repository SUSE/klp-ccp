/* PR c/68412 */
/* { dg-do compile } */
/* { dg-options "-Wall -Wextra" } */

#define M (sizeof (int) * __CHAR_BIT__)

int
fn1 (int i)
{
  return i == (-1 << 8); /* { dg-warning "left shift of negative value" } */
}

int
fn2 (int i)
{
  return i == (1 << M); /* { dg-warning "left shift count" } */
}

int
fn3 (int i)
{
  return i == 10 << (M - 1); /* { dg-warning "requires" } */
}

int
fn4 (int i)
{
  return i == 1 << -1; /* { dg-warning "left shift count" } */
}

int
fn5 (int i)
{
  return i == 1 >> M; /* { dg-warning "right shift count" } */
}

int
fn6 (int i)
{
  return i == 1 >> -1; /* { dg-warning "right shift count" } */
}

/* cp-out: warning: [^:]*: line 16, columns 15-21: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 22, columns 14-27: integer overflow in shift operation */
/* cp-out: error: [^:]*: line 28, columns 19-21: negative shift distance */
