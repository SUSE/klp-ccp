int main ()
{
  int i = 2;

  i = i++;
  printf ("%d\n",i);
}

/* cp-out: warning: [^:]*: line 6, column 2: identifier "printf" not declared */
