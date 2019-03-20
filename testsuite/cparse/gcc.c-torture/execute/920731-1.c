f(x){int i;for(i=0;i<8&&(x&1)==0;x>>=1,i++);return i;}
main(){if(f(4)!=2)abort();exit(0);}

/* cp-out: warning: [^:]*: line 2, column 18: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 2, column 26: identifier "exit" not declared */
