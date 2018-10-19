static __inline__ int f () { return g (); }
int g () { return f (); }

/* cp-out: warning: [^:]*: line 1, columns 36-37: identifier "g" not declared */
