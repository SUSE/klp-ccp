int v;static inline f(){return 0;}g(){return f();}void h(){return v++;}

/* cp-out: warning: [^:]*: line 1, columns 59-65: return with value in function returning void */
