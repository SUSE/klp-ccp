f(got){if(got!=0xffff)abort();}
main(){signed char c=-1;unsigned u=(unsigned short)c;f(u);exit(0);}

/* cp-out: warning: [^:]*: line 1, column 22: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 2, column 58: identifier "exit" not declared */
