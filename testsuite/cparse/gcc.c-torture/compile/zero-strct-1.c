typedef struct { } empty_t;

f ()
{
  empty_t i;
  bar (i);
}

/* cp-out: warning: [^:]*: line 6, column 2: identifier "bar" not declared */
