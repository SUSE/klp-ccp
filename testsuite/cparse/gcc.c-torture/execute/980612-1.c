struct fd
{
	unsigned char a;
	unsigned char b;
} f = { 5 };

struct fd *g() { return &f; }
int h() { return -1; }

int main()
{
	struct fd *f = g();
	f->b = h();
	if (((f->a & 0x7f) & ~0x10) <= 2)
		abort ();
	exit (0);
}

/* cp-out: warning: [^:]*: line 15, column 2: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, column 1: identifier "exit" not declared */
