typedef struct
{
  char x;
} s1;

s1 f (int arg0,...)
{
  int args;
  s1 back;
  va_start (args, arg0);
  va_end (args);
  return back;
}

/* cp-out: warning: [^:]*: line 10, columns 2-10: identifier "va_start" not declared */
/* cp-out: warning: [^:]*: line 11, columns 2-8: identifier "va_end" not declared */
