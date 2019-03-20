
extern int foo (void);

int
avi_parse_comments (int fd, char *buf, int space_left)
{
  int len = 0, readlen = 0, k;
  char *data, *c, *d;

  if (fd <= 0 || !buf || space_left <= 0)
    return -1;

  memset (buf, 0, space_left);

  readlen = foo ();
  if (!(data = malloc (readlen * sizeof (char) + 1)))
    return -1;

  c = data;
  space_left--;

  while (len < space_left)
    {
      if (!c || *c == '\0')
	break;
      else if (*c == 'I')
	{
	  d = c + 4;

	  k = 0;
	  while (d[k] != '\r' && d[k] != '\n' && d[k] != '\0')
	    ++k;
	  if (k >= space_left)
	    return len;


	  memcpy (buf + len, c, 4);
	  len += 4;


	  long2str (buf + len, k + 1);
	  len += 4;


	  memcpy (buf + len, d, k);

	  *(buf + len + k + 1) = '\0';


	  if ((k + 1) & 1)
	    {
	      k++;
	      *(buf + len + k + 1) = '\0';
	    }
	  len += k + 1;


	  while (*c != '\n' && *c != '\0')
	    ++c;
	  if (*c != '\0')
	    ++c;
	  else
	    break;

	}
    }
  free (data);

  return len;
}

/* cp-out: warning: [^:]*: line 13, column 2: identifier "memset" not declared */
/* cp-out: warning: [^:]*: line 16, column 15: identifier "malloc" not declared */
/* cp-out: warning: [^:]*: line 37, column 3: identifier "memcpy" not declared */
/* cp-out: warning: [^:]*: line 41, column 3: identifier "long2str" not declared */
/* cp-out: warning: [^:]*: line 45, column 3: identifier "memcpy" not declared */
/* cp-out: warning: [^:]*: line 67, column 2: identifier "free" not declared */
/* cp-out: warning: [^:]*: line 16, columns 15-51: assignment to pointer from integer which is not NULL */
