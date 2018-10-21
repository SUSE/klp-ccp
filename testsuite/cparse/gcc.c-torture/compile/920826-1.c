/* { dg-require-effective-target indirect_jumps } */

f(int*x){goto*(char)*x;}

/* cp-out: warning: [^:]*: line 3, columns 14-15: dereferencing integer at computed goto */
