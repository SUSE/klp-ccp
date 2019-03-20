foo (a, b, c)
{
  bar (a, b);
  {
    int arr[10];
    arr[c] = b;
    bar (arr[0], arr[1]);
  }
}

/* cp-out: warning: [^:]*: line 3, column 2: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 7, column 4: identifier "bar" not declared */
