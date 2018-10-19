/* PR c/43245 */
/* { dg-do compile } */
/* { dg-options "-Wno-discarded-qualifiers" } */

void
foo (char *p)
{
}

char *
bar (void)
{
  const char *s = "foo";
  char *s1 = s;
  s1 = s;
  foo (s);
  return s;
}

/* cp-out: warning: [^:]*: line 14, columns 13-14: assignment's source qualifiers are not a subset of target ones */
/* cp-out: warning: [^:]*: line 15, columns 7-8: assignment's source qualifiers are not a subset of target ones */
/* cp-out: warning: [^:]*: line 16, columns 7-8: assignment's source qualifiers are not a subset of target ones */
/* cp-out: warning: [^:]*: line 17, columns 9-10: assignment's source qualifiers are not a subset of target ones */
