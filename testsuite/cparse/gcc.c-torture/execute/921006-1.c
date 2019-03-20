/* REPRODUCED:RUN:SIGNAL MACHINE:i386 OPTIONS:-O */
main()
{
if(strcmp("X","")<0)abort();
exit(0);
}

/* cp-out: warning: [^:]*: line 4, column 3: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 4, column 20: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 5, column 0: identifier "exit" not declared */
