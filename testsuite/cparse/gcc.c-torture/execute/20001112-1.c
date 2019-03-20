int main ()
{
  long long i = 1;

  i = i * 2 + 1;
  
  if (i != 3)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, column 2: identifier "exit" not declared */
