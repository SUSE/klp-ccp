static int strcmp(){return-1;}
#define strcmp __builtin_strcmp
main()
{
if(strcmp("X","X\376")>=0)abort();
exit(0);
}

/* cp-out: warning: [^:]*: line 5, columns 26-31: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 6, columns 0-4: identifier "exit" not declared */
