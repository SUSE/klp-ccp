typedef unsigned short uint16;
f (unsigned char *w)
{
  w[2] = (uint16) ((((g (0) % 10000 + 42) & 0xFF) << 8) | (((g (0) % 10000 + 42) >> 8) & 0xFF)) & 0xFF,
  w[3] = (uint16) ((((g (0) % 10000 + 42) & 0xFF) << 8) | (((g (0) % 10000 + 42) >> 8) & 0xFF)) >> 8;
}

/* cp-out: warning: [^:]*: line 4, columns 22-23: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 4, columns 61-62: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 5, columns 22-23: identifier "g" not declared */
/* cp-out: warning: [^:]*: line 5, columns 61-62: identifier "g" not declared */
