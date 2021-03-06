/* Prove that HANDLE_PRAGMA_PUSH_POP_MACRO handling works somewhat. */

/* { dg-do link { target *-*-mingw* } } */

#define TEXT1  "ABC"
#define TEXT2  "DEF"

int main()
{
  if (__builtin_strcmp (TEXT1 TEXT2, "ABCDEF") != 0)
    link_error ();

#pragma push_macro("TEXT1")
#undef TEXT1
#define TEXT1  "XYZ"

  if (__builtin_strcmp (TEXT1 TEXT2, "XYZDEF") != 0)
    link_error ();

#pragma pop_macro("TEXT1")

  if (__builtin_strcmp (TEXT1 TEXT2, "ABCDEF") != 0)
    link_error ();

  return 0;
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "link_error" not declared */
/* cp-out: warning: [^:]*: line 18, column 4: identifier "link_error" not declared */
/* cp-out: warning: [^:]*: line 23, column 4: identifier "link_error" not declared */
