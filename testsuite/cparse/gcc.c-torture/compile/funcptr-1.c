extern int (*gp)(const char*);

int
g (const char* d)
{
  printf ("g");
  return 0;
}

f ()
{
  int errcnt=0;

  if (gp != g)
    {
      printf ("f");
      errcnt++;
    }
}

/* cp-out: warning: [^:]*: line 6, column 2: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 16, column 6: identifier "printf" not declared */
