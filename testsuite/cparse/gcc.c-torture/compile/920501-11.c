typedef struct{int s;}S;foo(){int i=(int)&(S){(void*)((int)&(S){1})};}

/* cp-out: warning: [^:]*: line 1, columns 46-47: assignment to integer from pointer */
