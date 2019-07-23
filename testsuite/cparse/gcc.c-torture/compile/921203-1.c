char dispstr[];
f()
{
  strcpy(dispstr,"xxxxxxxxxxx");
}

/* cp-out: warning: [^:]*: line 4, column 2: identifier "strcpy" not declared */
/* cp-out: warning: [^:]*: line 1, columns 5-14: init declarator has incomplete type */
