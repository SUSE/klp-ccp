double x = 0x1.fp1;
int main()
{
  if (x !=  3.875)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 5, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 6, columns 2-6: identifier "exit" not declared */
