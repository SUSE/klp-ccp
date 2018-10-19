int main(int argc, char *argv[])
{
  strcat(argv[0], "X");
  return strlen(argv[0]);
}

/* cp-out: warning: [^:]*: line 3, columns 2-8: identifier "strcat" not declared */
/* cp-out: warning: [^:]*: line 4, columns 9-15: identifier "strlen" not declared */
