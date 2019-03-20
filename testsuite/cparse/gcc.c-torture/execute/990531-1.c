   unsigned long bad(int reg, unsigned long inWord)
   {
       union {
           unsigned long word;
           unsigned char byte[4];
       } data;

       data.word = inWord;
       data.byte[reg] = 0;

       return data.word;
   }

main()
{
  /* XXX This test could be generalized.  */
  if (sizeof (long) != 4)
    exit (0);

  if (bad (0, 0xdeadbeef) == 0xdeadbeef)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 18, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 21, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, column 2: identifier "exit" not declared */
