int compare(x, y)
unsigned int x;
unsigned int y;
{
   if (x==y)
     return 0;
   else
     return 1;
}
 
main()
{
 unsigned int i, j, k, l;
 i = 5; j = 2; k=0; l=2;
 if (compare(5%(~(unsigned) 2), i%~j) 
     || compare(0, k%~l))
    abort();
 else
    exit(0);
}

/* cp-out: warning: [^:]*: line 17, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, column 4: identifier "exit" not declared */
