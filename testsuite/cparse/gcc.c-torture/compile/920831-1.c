/* { dg-require-effective-target indirect_jumps } */

f(x){goto*(char)x;}

/* cp-out: error: [^:]*: line 3, columns 10-11: dereferencing something which is not a pointer */
