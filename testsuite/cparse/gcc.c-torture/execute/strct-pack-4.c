typedef struct
{
  unsigned char a __attribute__((packed));
  unsigned short b __attribute__((packed));
} three_char_t;

unsigned char
my_set_a (void)
{
  return 0xab;
}

unsigned short
my_set_b (void)
{
  return 0x1234;
}

main ()
{
  three_char_t three_char;

  three_char.a = my_set_a ();
  three_char.b = my_set_b ();
  if (three_char.a != 0xab || three_char.b != 0x1234)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 26, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 27, column 2: identifier "exit" not declared */
