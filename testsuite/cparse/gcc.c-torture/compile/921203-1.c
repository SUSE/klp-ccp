char dispstr[];
f()
{
  strcpy(dispstr,"xxxxxxxxxxx");
}

/* cp-out: warning: [^:]*: line 4, columns 2-8: identifier "strcpy" not declared */
/* cp-out: warning: [^:]*: line 1, columns 5-12: init declarator has incomplete type */
