main()
{
int j=1081;
struct
{
signed int m:11;
}l;
if((l.m=j)==j)abort();
exit(0);
}

/* cp-out: warning: [^:]*: line 8, column 14: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, column 0: identifier "exit" not declared */
