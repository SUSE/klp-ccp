static __inline__ int f () { return g (); }
int g () { return f (); }

/* cp-out: warning: [^:]*: line 1, column 36: identifier "g" not declared */
