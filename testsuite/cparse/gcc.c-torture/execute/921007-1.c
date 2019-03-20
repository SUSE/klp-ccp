static int strcmp(){return-1;}
#define strcmp __builtin_strcmp
main()
{
if(strcmp("X","X\376")>=0)abort();
exit(0);
}

/* cp-out: warning: [^:]*: line 5, column 26: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 6, column 0: identifier "exit" not declared */
