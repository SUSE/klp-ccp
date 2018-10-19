typedef unsigned long long ULL;
ULL back;
ULL hpart, lpart;
ULL
build(long h, long l)
{
  hpart = h;
  hpart <<= 32;
  lpart = l;
  lpart &= 0xFFFFFFFFLL;
  back = hpart | lpart;
  return back;
}

main()
{
  if (build(0, 1) != 0x0000000000000001LL)
    abort();
  if (build(0, 0) != 0x0000000000000000LL)
    abort();
  if (build(0, 0xFFFFFFFF) != 0x00000000FFFFFFFFLL)
    abort();
  if (build(0, 0xFFFFFFFE) != 0x00000000FFFFFFFELL)
    abort();
  if (build(1, 1) != 0x0000000100000001LL)
    abort();
  if (build(1, 0) != 0x0000000100000000LL)
    abort();
  if (build(1, 0xFFFFFFFF) != 0x00000001FFFFFFFFLL)
    abort();
  if (build(1, 0xFFFFFFFE) != 0x00000001FFFFFFFELL)
    abort();
  if (build(0xFFFFFFFF, 1) != 0xFFFFFFFF00000001LL)
    abort();
  if (build(0xFFFFFFFF, 0) != 0xFFFFFFFF00000000LL)
    abort();
  if (build(0xFFFFFFFF, 0xFFFFFFFF) != 0xFFFFFFFFFFFFFFFFLL)
    abort();
  if (build(0xFFFFFFFF, 0xFFFFFFFE) != 0xFFFFFFFFFFFFFFFELL)
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 18, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 26, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 32, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 34, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 36, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 38, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 40, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 41, columns 2-6: identifier "exit" not declared */
