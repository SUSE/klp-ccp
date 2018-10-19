f(x){int i;for(i=0;i<8&&(x&1)==0;x>>=1,i++);return i;}
main(){if(f(4)!=2)abort();exit(0);}

/* cp-out: warning: [^:]*: line 2, columns 18-23: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 2, columns 26-30: identifier "exit" not declared */
