x(){signed char c=-1;return c<0;}main(){if(x()!=1)abort();exit(0);}

/* cp-out: warning: [^:]*: line 1, columns 50-55: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 1, columns 58-62: identifier "exit" not declared */
