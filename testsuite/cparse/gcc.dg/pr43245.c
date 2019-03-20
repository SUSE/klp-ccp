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

/* cp-out: warning: [^:]*: line 14, column 13: assignment's source qualifiers are not a subset of target ones */
/* cp-out: warning: [^:]*: line 15, column 7: assignment's source qualifiers are not a subset of target ones */
/* cp-out: warning: [^:]*: line 16, column 7: assignment's source qualifiers are not a subset of target ones */
/* cp-out: warning: [^:]*: line 17, column 9: assignment's source qualifiers are not a subset of target ones */
