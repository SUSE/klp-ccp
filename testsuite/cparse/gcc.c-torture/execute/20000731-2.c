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

/* cp-out: warning: [^:]*: line 13, column 6: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, column 4: identifier "exit" not declared */
