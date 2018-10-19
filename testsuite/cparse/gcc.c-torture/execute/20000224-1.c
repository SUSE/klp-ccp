int loop_1 = 100;
int loop_2 = 7;
int flag = 0;

int test (void)
{
    int i;
    int counter  = 0;

    while (loop_1 > counter) {
        if (flag & 1) {
            for (i = 0; i < loop_2; i++) {
                counter++;
            }
        }
        flag++;
    }
    return 1;
}

int main()
{
    if (test () != 1)
      abort ();
    
    exit (0);
}

/* cp-out: warning: [^:]*: line 24, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 26, columns 4-8: identifier "exit" not declared */
