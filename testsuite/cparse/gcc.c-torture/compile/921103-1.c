struct {
  unsigned int f1, f2;
} s;

f()
{
 unsigned x, y;
 x = y = 0;
 while (y % 4)
   y++;
 g(&s.f2, s.f1 + x, 4);
}

/* cp-out: warning: [^:]*: line 11, columns 1-2: identifier "g" not declared */

