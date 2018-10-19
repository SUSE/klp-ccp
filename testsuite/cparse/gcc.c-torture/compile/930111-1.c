/* 2.3.3 crashes on 386 with -traditional */
f(a)
     char *a;
{
  int d = strcmp(a,"-");

  while (vfork() < 0)
    ;
  return d;
}

/* cp-out: warning: [^:]*: line 5, columns 10-16: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 7, columns 9-14: identifier "vfork" not declared */
