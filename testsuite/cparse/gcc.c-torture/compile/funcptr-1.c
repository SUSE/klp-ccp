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

/* cp-out: warning: [^:]*: line 6, columns 2-8: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 16, columns 6-12: identifier "printf" not declared */
