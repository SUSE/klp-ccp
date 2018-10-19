int
main()
{
    int i = 1;
    int j = 0;

    while (i != 1024 || j <= 0) {
        i *= 2;
        ++ j;
    }

    if (j != 10)
      abort ();

    exit (0);
}

/* cp-out: warning: [^:]*: line 13, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, columns 4-8: identifier "exit" not declared */
