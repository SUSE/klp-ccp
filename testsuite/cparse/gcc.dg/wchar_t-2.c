/* { dg-options "-fshort-wchar" } */
/* Check that "-fshort-wchar" makes wchar_t the same size as "unsigned
   short".  */

extern int i[sizeof (L'a')];
int i[sizeof (unsigned short)];

/* cp-out: warning: [^:]*: line 6, column 4: incompatible redeclaration */
