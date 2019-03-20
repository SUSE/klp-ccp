/* PRs 17408 and 17409, with different options. */
extern int t;
extern int t = 0;
void f(){t =0;}
void g(){h(&t);}

/* cp-out: warning: [^:]*: line 5, column 9: identifier "h" not declared */
