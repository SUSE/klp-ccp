/* { dg-require-effective-target untyped_assembly } */
int s[2];
x(){if(!s[0]){s[1+s[1]]=s[1];return 1;}}
main(){s[0]=s[1]=0;if(x(0)!=1)abort();exit(0);}

/* cp-out: warning: [^:]*: line 4, columns 30-35: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 4, columns 38-42: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 4, columns 22-23: wrong number of arguments to function invocation */
