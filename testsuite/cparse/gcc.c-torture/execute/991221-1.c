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

/* cp-out: warning: [^:]*: line 7, column 5: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 11, column 6: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 3: identifier "exit" not declared */
