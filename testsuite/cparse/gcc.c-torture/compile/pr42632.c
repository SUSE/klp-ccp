static inline __attribute__((always_inline)) int
__pskb_trim(void)
{
  return ___pskb_trim();
}
static inline __attribute__((always_inline))
int pskb_trim(void)
{
  return __pskb_trim();
}
int ___pskb_trim(void)
{
  pskb_trim();
  return 0;
}

/* cp-out: warning: [^:]*: line 4, column 9: identifier "___pskb_trim" not declared */
