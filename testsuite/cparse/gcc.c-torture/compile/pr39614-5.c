/* { dg-do compile } */
/* { dg-options "-w -std=c99" } */
int i;
void
f (void)
{
  i = (1 / 0, 1 / 0);
}

/* cp-out: warning: [^:]*: line 7, columns 7-12: division by zero */
/* cp-out: warning: [^:]*: line 7, columns 14-19: division by zero */
