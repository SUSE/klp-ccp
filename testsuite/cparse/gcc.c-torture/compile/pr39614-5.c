/* { dg-do compile } */
/* { dg-options "-w -std=c99" } */
int i;
void
f (void)
{
  i = (1 / 0, 1 / 0);
}

/* cp-out: warning: [^:]*: line 7, columns 11-12: division by zero */
/* cp-out: warning: [^:]*: line 7, columns 18-19: division by zero */
