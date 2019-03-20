typedef int t;
f(t y){switch(y){case 1:return 1;}return 0;}
main(){if(f((t)1)!=1)abort();exit(0);}

/* cp-out: warning: [^:]*: line 3, column 21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 3, column 29: identifier "exit" not declared */
