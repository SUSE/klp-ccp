/* { dg-require-effective-target indirect_jumps } */

f(x){goto*(char)x;}

/* cp-out: warning: [^:]*: line 3, columns 10-11: dereferencing integer at computed goto */
