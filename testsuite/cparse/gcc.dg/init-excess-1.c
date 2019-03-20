/* Test for various cases of excess initializers for empty objects:
   bug 21873.  Various versions of GCC ICE, hang or loop repeating
   diagnostics on various of these tests.  */
/* Origin: Joseph Myers <joseph@codesourcery.com> */
/* { dg-do compile } */
/* { dg-options "" } */

struct s0 { };
struct s1 { int a; };
struct s2 { int a; int b; };

int a0[0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
int a1[0][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
int a2[0][1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
int a3[1][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
int a4[][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
int a5[][0][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
int a6[][0][1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
int a7[][1][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */

struct s0 b0[0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b1[0][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b2[0][1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b3[1][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b4[][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b5[][0][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b6[][0][1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b7[][1][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b8[1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s0 b9[] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */

struct s1 c0[0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s1 c1[0][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s1 c2[0][1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s1 c3[1][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s1 c4[][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s1 c5[][0][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s1 c6[][0][1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s1 c7[][1][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */

struct s2 d0[0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s2 d1[0][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s2 d2[0][1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s2 d3[1][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s2 d4[][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s2 d5[][0][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s2 d6[][0][1] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */
struct s2 d7[][1][0] = { 1, 2 }; /* { dg-warning "excess elements|near init" } */

/* cp-out: warning: [^:]*: line 12, columns 14-18: excess elements in intializer */
/* cp-out: warning: [^:]*: line 13, columns 17-21: excess elements in intializer */
/* cp-out: warning: [^:]*: line 14, columns 17-21: excess elements in intializer */
/* cp-out: warning: [^:]*: line 15, column 17: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 16, column 16: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 17, column 19: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 18, column 19: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 19, column 19: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 21, columns 20-24: excess elements in intializer */
/* cp-out: warning: [^:]*: line 22, columns 23-27: excess elements in intializer */
/* cp-out: warning: [^:]*: line 23, columns 23-27: excess elements in intializer */
/* cp-out: warning: [^:]*: line 24, column 23: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 25, column 22: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 26, column 25: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 27, column 25: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 28, column 25: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 29, column 20: excess elements initialization of empty struct/union */
/* cp-out: warning: [^:]*: line 30, column 19: excess elements initialization of empty struct/union */
/* cp-out: warning: [^:]*: line 32, columns 20-24: excess elements in intializer */
/* cp-out: warning: [^:]*: line 33, columns 23-27: excess elements in intializer */
/* cp-out: warning: [^:]*: line 34, columns 23-27: excess elements in intializer */
/* cp-out: warning: [^:]*: line 35, column 23: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 36, column 22: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 37, column 25: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 38, column 25: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 39, column 25: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 41, columns 20-24: excess elements in intializer */
/* cp-out: warning: [^:]*: line 42, columns 23-27: excess elements in intializer */
/* cp-out: warning: [^:]*: line 43, columns 23-27: excess elements in intializer */
/* cp-out: warning: [^:]*: line 44, column 23: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 45, column 22: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 46, column 25: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 47, column 25: excess elements in initialization of zero length array */
/* cp-out: warning: [^:]*: line 48, column 25: excess elements in initialization of zero length array */
