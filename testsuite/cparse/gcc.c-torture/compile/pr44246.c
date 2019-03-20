int main(int argc, char *argv[])
{
  strcat(argv[0], "X");
  return strlen(argv[0]);
}

/* cp-out: warning: [^:]*: line 3, column 2: identifier "strcat" not declared */
/* cp-out: warning: [^:]*: line 4, column 9: identifier "strlen" not declared */
