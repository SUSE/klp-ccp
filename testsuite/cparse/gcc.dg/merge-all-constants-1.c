/* { dg-do compile } */
/* { dg-options "-w -O2 -fmerge-all-constants" } */

const char str1[36] = "0123456789abcdefghijklmnopqrstuvwxyz";
const char str2[38] = "0123456789abcdefghijklmnopqrstuvwxyz";
const char str3[10] = "0123456789abcdefghijklmnopqrstuvwxyz";

/* { dg-final { scan-assembler-not "\.rodata\.str" } } */

/* cp-out: warning: [^:]*: line 4, columns 22-60: excess characters in array initialization */
/* cp-out: warning: [^:]*: line 6, columns 22-60: excess characters in array initialization */
