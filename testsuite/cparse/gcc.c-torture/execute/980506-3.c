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

/* cp-out: warning: [^:]*: line 9, column 6: identifier "strlen" not declared */
/* cp-out: warning: [^:]*: line 11, column 2: identifier "memset" not declared */
/* cp-out: warning: [^:]*: line 20, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, column 4: identifier "exit" not declared */
