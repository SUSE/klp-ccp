void func(int, int);

int main()
{
        int x = 7;
        func(!x, !7);
	exit (0);
}

void func(int x, int y)
{
        if (x == y)
                return;
        else
                abort ();
}

/* cp-out: warning: [^:]*: line 7, column 1: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 15, column 16: identifier "abort" not declared */
