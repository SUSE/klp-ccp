foo (a, b, c)
{
  bar (a, b);
  {
    int arr[10];
    arr[c] = b;
    bar (arr[0], arr[1]);
  }
}

/* cp-out: warning: [^:]*: line 3, columns 2-5: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 7, columns 4-7: identifier "bar" not declared */
