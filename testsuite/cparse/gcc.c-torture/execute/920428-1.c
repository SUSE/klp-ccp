x(const char*s){char a[1];const char*ss=s;a[*s++]|=1;return(int)ss+1==(int)s;}
main(){if(x("")!=1)abort();exit(0);}

/* cp-out: warning: [^:]*: line 2, column 19: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 2, column 27: identifier "exit" not declared */
