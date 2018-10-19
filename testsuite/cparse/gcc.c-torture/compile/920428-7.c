x(float*x){int a[4],i=0,j;for(j=0;j<2;j++){f(a[i++]);f(a[i++]);}}

/* cp-out: warning: [^:]*: line 1, columns 43-44: identifier "f" not declared */
/* cp-out: warning: [^:]*: line 1, columns 53-54: identifier "f" not declared */
