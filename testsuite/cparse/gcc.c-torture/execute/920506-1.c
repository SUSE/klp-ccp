int l[]={0,1};
main(){int*p=l;switch(*p++){case 0:exit(0);case 1:break;case 2:break;case 3:case 4:break;}abort();}

/* cp-out: warning: [^:]*: line 2, column 35: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 2, column 90: identifier "abort" not declared */
