/* { dg-skip-if "requires alloca" { ! alloca } { "-O0" } { "" } } */
/* REPRODUCED:RUN:SIGNAL MACHINE:sparc OPTIONS: */
f(int n)
{
int i;
double v[n];
for(i=0;i<n;i++)
v[i]=0;
}
main()
{
f(100);
exit(0);
}

/* cp-out: warning: [^:]*: line 13, columns 0-4: identifier "exit" not declared */
