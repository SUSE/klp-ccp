long f(short a,short b){return (long)a/b;}
main(){if(f(-32768,-1)!=32768L)abort();else exit(0);}

/* cp-out: warning: [^:]*: line 2, column 31: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 2, column 44: identifier "exit" not declared */
