/* { dg-require-effective-target label_values } */
main(){__label__ l;void*x(){return&&l;}goto*x();abort();return;l:exit(0);}

/* cp-out: warning: [^:]*: line 2, column 48: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 2, column 65: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 2, columns 56-63: return without value in function returning non-void */
