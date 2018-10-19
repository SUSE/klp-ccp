/* REPRODUCED:RUN:SIGNAL MACHINE:i386 OPTIONS:-O */
main()
{
if(strcmp("X","")<0)abort();
exit(0);
}

/* cp-out: warning: [^:]*: line 4, columns 3-9: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 4, columns 20-25: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 5, columns 0-4: identifier "exit" not declared */
