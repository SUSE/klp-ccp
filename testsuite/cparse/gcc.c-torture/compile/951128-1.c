char a[];
f (const int i)
{
  a[i] = 0;
}

/* cp-out: warning: [^:]*: line 1, columns 5-6: init declarator has incomplete type */
