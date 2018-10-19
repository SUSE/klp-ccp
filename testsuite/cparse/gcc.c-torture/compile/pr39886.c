/* PR middle-end/39886 */

int foo (int);

volatile unsigned char g;

void bar (int p)
{
  char l = 0xE1;
  func ((foo ((p & g) <= l), 1));
}

/* cp-out: warning: [^:]*: line 10, columns 2-6: identifier "func" not declared */
