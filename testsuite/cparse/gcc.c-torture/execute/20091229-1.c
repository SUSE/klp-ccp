long long foo(long long v) { return v / -0x080000000LL; }
int main(int argc, char **argv) { if (foo(0x080000000LL) != -1) abort(); exit (0); }

/* cp-out: warning: [^:]*: line 2, column 64: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 2, column 73: identifier "exit" not declared */
