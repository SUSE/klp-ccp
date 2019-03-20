/* { dg-options "-fshort-enums" } */
/* Check that "-fshort-enums" packs enumeration tyes into a minimal
   number of bytes..  */

enum e { e_1 };

extern int i[sizeof (enum e)];
int i[1];

/* cp-out: warning: [^:]*: line 8, column 4: incompatible redeclaration */
