/* Simple alignment checks;
   looking for compiler/assembler alignment disagreements,
   agreement between struct initialization and access.  */
struct a_short { char c; short s; } s_c_s = { 'a', 13 };
struct a_int { char c ; int i; } s_c_i = { 'b', 14 };
struct b_int { short s; int i; } s_s_i  = { 15, 16 };
struct a_float { char c; float f; } s_c_f = { 'c', 17.0 };
struct b_float { short s; float f; } s_s_f = { 18, 19.0 };
struct a_double { char c; double d; } s_c_d = { 'd', 20.0 };
struct b_double { short s; double d; } s_s_d = { 21, 22.0 };
struct c_double { int i; double d; } s_i_d = { 23, 24.0 };
struct d_double { float f; double d; } s_f_d = { 25.0, 26.0 };
struct a_ldouble { char c; long double ld; } s_c_ld = { 'e', 27.0 };
struct b_ldouble { short s; long double ld; } s_s_ld = { 28, 29.0 };
struct c_ldouble { int i; long double ld; } s_i_ld = { 30, 31.0 };
struct d_ldouble { float f; long double ld; } s_f_ld = { 32.0, 33.0 };
struct e_ldouble { double d; long double ld; } s_d_ld = { 34.0, 35.0 };

int main ()
{
  if (s_c_s.c != 'a') abort ();
  if (s_c_s.s != 13) abort ();
  if (s_c_i.c != 'b') abort ();
  if (s_c_i.i != 14) abort ();
  if (s_s_i.s != 15) abort ();
  if (s_s_i.i != 16) abort ();
  if (s_c_f.c != 'c') abort ();
  if (s_c_f.f != 17.0) abort ();
  if (s_s_f.s != 18) abort ();
  if (s_s_f.f != 19.0) abort ();
  if (s_c_d.c != 'd') abort ();
  if (s_c_d.d != 20.0) abort ();
  if (s_s_d.s != 21) abort ();
  if (s_s_d.d != 22.0) abort ();
  if (s_i_d.i != 23) abort ();
  if (s_i_d.d != 24.0) abort ();
  if (s_f_d.f != 25.0) abort ();
  if (s_f_d.d != 26.0) abort ();
  if (s_c_ld.c != 'e') abort ();
  if (s_c_ld.ld != 27.0) abort ();
  if (s_s_ld.s != 28) abort ();
  if (s_s_ld.ld != 29.0) abort ();
  if (s_i_ld.i != 30) abort ();
  if (s_i_ld.ld != 31.0) abort ();
  if (s_f_ld.f != 32.0) abort ();
  if (s_f_ld.ld != 33.0) abort ();
  if (s_d_ld.d != 34.0) abort ();
  if (s_d_ld.ld != 35.0) abort ();
  return 0;
}

/* cp-out: warning: [^:]*: line 21, column 22: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, column 21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 23, column 22: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, column 21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 25, column 21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 26, column 21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 27, column 22: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, column 23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 29, column 21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, column 23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 31, column 22: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 32, column 23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 33, column 21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 34, column 23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 35, column 21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 36, column 23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 37, column 23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 38, column 23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 39, column 23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 40, column 25: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 41, column 22: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 42, column 25: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 43, column 22: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 44, column 25: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 45, column 24: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 46, column 25: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 47, column 24: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 48, column 25: identifier "abort" not declared */
