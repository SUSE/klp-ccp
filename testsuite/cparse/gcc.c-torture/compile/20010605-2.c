/* Origin: Joseph Myers <jsm28@cam.ac.uk>.  */
/* As an extension, GCC allows a struct or union to be cast to its own
   type, but failed to allow this when a typedef was involved.
   Reported as PR c/2735 by <cowan@ccil.org>.  */
union u { int i; };
typedef union u uu;
union u a;
uu b;

void
foo (void)
{
  a = (union u) b;
  a = (uu) b;
  b = (union u) a;
  b = (uu) a;
}

/* cp-out: warning: [^:]*: line 13, columns 6-17: invalid types in cast */
/* cp-out: warning: [^:]*: line 14, columns 6-12: invalid types in cast */
/* cp-out: warning: [^:]*: line 15, columns 6-17: invalid types in cast */
/* cp-out: warning: [^:]*: line 16, columns 6-12: invalid types in cast */
