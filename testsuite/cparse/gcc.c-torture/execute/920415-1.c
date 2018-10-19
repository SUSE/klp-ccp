/* { dg-require-effective-target label_values } */
main(){__label__ l;void*x(){return&&l;}goto*x();abort();return;l:exit(0);}

/* cp-out: warning: [^:]*: line 2, columns 48-53: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 2, columns 65-69: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 2, columns 56-62: return without value in function returning non-void */
