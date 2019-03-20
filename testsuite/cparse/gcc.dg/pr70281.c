/* { dg-options "-Wall -fdiagnostics-show-caret" } */
int bch_stats_show ()
{
  return __builtin_types_compatible_p (unsigned, int) ? "" : "";  /* { dg-warning "cast" } */
/* { dg-begin-multiline-output "" }
   return __builtin_types_compatible_p (unsigned, int) ? "" : "";
          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
   { dg-end-multiline-output "" } */
}

/* cp-out: warning: [^:]*: line 4, columns 9-63: assignment to integer from pointer */
