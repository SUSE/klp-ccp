static int f(int) __attribute__((const));
int main()
{
   int f1, f2, x;
   x = 1; f1 = f(x);
   x = 2; f2 = f(x);
   if (f1 != 1 || f2 != 2)
     abort ();
   exit (0);
}
static int f(int x) { return x; }

/* cp-out: warning: [^:]*: line 8, columns 5-10: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, columns 3-7: identifier "exit" not declared */
