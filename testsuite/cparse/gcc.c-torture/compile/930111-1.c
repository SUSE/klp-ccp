/* 2.3.3 crashes on 386 with -traditional */
f(a)
     char *a;
{
  int d = strcmp(a,"-");

  while (vfork() < 0)
    ;
  return d;
}

/* cp-out: warning: [^:]*: line 5, column 10: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 7, column 9: identifier "vfork" not declared */
