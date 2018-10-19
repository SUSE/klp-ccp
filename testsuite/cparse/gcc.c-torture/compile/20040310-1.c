void I_wacom ()
{
  char buffer[50], *p;
  int RequestData (char *cmd)
  {
    p = buffer;
    foo (buffer);
  }
  RequestData (0);
}

/* cp-out: warning: [^:]*: line 7, columns 4-7: identifier "foo" not declared */
