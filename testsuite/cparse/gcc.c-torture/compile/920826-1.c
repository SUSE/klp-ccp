/* { dg-require-effective-target indirect_jumps } */

f(int*x){goto*(char)*x;}

/* cp-out: error: [^:]*: line 3, columns 14-15: dereferencing something which is not a pointer */
