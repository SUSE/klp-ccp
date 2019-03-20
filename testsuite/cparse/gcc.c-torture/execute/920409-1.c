x(){signed char c=-1;return c<0;}main(){if(x()!=1)abort();exit(0);}

/* cp-out: warning: [^:]*: line 1, column 50: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 1, column 58: identifier "exit" not declared */
