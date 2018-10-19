int main ()
{
  int i = 2;

  i = i++;
  printf ("%d\n",i);
}

/* cp-out: warning: [^:]*: line 6, columns 2-8: identifier "printf" not declared */
