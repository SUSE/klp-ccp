int main( void )
{
   unsigned long totalsize = 80;
   unsigned long msize = 64;

   if (sizeof(long) != 4)
     exit(0);
   
   if ( totalsize > (2147483647L   * 2UL + 1)  
        || (msize != 0 && ((msize - 1) > (2147483647L   * 2UL + 1) )))
      abort();
   exit( 0 );
}

/* cp-out: warning: [^:]*: line 7, columns 5-9: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 11, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, columns 3-7: identifier "exit" not declared */
