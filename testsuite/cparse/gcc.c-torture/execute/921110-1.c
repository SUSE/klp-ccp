extern void abort(void);
typedef void (*frob)();
frob f[] = {abort};

int main(void)
{
  exit(0);
}

/* cp-out: warning: [^:]*: line 7, column 2: identifier "exit" not declared */
