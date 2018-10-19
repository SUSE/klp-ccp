/* { dg-require-effective-target untyped_assembly } */
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
__extension__ typedef long long int int64_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
static inline unsigned int
lshift_u_s (unsigned int left, int right)
{
  if ((right) || (right >= sizeof (unsigned int) * 8)
      || (left > (4294967295U >> right)))
    return left;
}
static inline unsigned long int
div_rhs (long int rhs)
{
  if (rhs == 0)
    return 1;
  return rhs;
}

uint32_t g_230;
int8_t g_294;
uint16_t g_316;
uint32_t g_334;
int32_t g_375;
int64_t g_380;
int32_t func_99 (int16_t p_100, int32_t p_101, int32_t p_102, int32_t p_103,
                 int64_t p_105, int32_t p_106, int32_t p_107, int16_t p_108,
                 int16_t p_109);
int32_t
func_77 (int64_t p_79)
{
  int16_t l_397;
  if (mod_rhs (p_79))
    p_79 = 1;
  else
    for (p_79 = 0; 0; p_79 += 1)
      {
      }
  if (lshift_s_s (1, func_112 (2L, (lshift_u_s (g_334, p_79)))))
    {
      int8_t l_384;
      int64_t l_414;
      if (lshift_u_s (g_375, 1))
        {
          func_23 (func_99 (1, 1, 1, 1, g_230, p_79, 1, g_334, 1), 1);
          for (p_79 = 0; 0; ++p_79)
            {
            }
        }
      if (div_rhs (func_82 (1, 1, g_380, 1, l_397, 1, 1)))
        func_99 ((func_82
                  (1, g_334, g_294, func_112 (1, (p_79 & 1)), g_316, 1, 1)),
                 1, (0xFBA25CA382A8CA74LL), l_384, l_414, 0L, 1, 1, 1);
    }
}

/* cp-out: warning: [^:]*: line 36, columns 6-13: identifier "mod_rhs" not declared */
/* cp-out: warning: [^:]*: line 42, columns 6-16: identifier "lshift_s_s" not declared */
/* cp-out: warning: [^:]*: line 42, columns 21-29: identifier "func_112" not declared */
/* cp-out: warning: [^:]*: line 48, columns 10-17: identifier "func_23" not declared */
/* cp-out: warning: [^:]*: line 53, columns 19-26: identifier "func_82" not declared */
/* cp-out: warning: [^:]*: line 54, columns 18-25: identifier "func_82" not declared */
/* cp-out: warning: [^:]*: line 55, columns 36-44: identifier "func_112" not declared */
