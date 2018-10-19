unsigned char lookup_table [257];

static int 
build_lookup (pattern)
     unsigned char *pattern;
{
  int m;

  m = strlen (pattern) - 1;
  
  memset (lookup_table, ++m, 257);
  return m;
}

int main(argc, argv)
     int argc;
     char **argv;
{
  if (build_lookup ("bind") != 4)
    abort ();
  else
    exit (0);
}

/* cp-out: warning: [^:]*: line 9, columns 6-12: identifier "strlen" not declared */
/* cp-out: warning: [^:]*: line 11, columns 2-8: identifier "memset" not declared */
/* cp-out: warning: [^:]*: line 20, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 4-8: identifier "exit" not declared */
