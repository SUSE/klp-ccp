int main ()
{
    int nResult;
    int b=0;
    int i = -1;

    do
    {
     if (b!=0) {
       abort ();
       nResult=1;
     } else {
      nResult=0;
     }
     i++;
     b=(i+2)*4;
    } while (i < 0);
    exit (0);
}

/* cp-out: warning: [^:]*: line 10, columns 7-12: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, columns 4-8: identifier "exit" not declared */
