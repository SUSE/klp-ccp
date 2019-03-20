typedef unsigned char t;int i,j;
t*f(t*p){t c;c=*p++;i=((c&2)?1:0);j=(c&7)+1;return p;}
main(){t*p0="ab",*p1;p1=f(p0);if(p0+1!=p1)abort();exit(0);}

/* cp-out: warning: [^:]*: line 3, column 42: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 3, column 50: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 3, column 12: pointers to different integer types in assignment */
