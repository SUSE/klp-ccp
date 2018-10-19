/* { dg-do compile } */
/* { dg-options "-Wc++-compat" } */

char a1[] = "a";
char a2[1] = "a";	/* { dg-warning "C\[+\]\[+\]" } */
char a3[2] = "a";

/* cp-out: warning: [^:]*: line 5, columns 13-16: excess characters in array initialization */
