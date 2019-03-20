extern char foo[], bar[];
void f (void) { memcpy (foo, bar, 7); }

/* cp-out: warning: [^:]*: line 2, column 16: identifier "memcpy" not declared */
