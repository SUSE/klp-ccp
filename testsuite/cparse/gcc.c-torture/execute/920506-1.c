int l[]={0,1};
main(){int*p=l;switch(*p++){case 0:exit(0);case 1:break;case 2:break;case 3:case 4:break;}abort();}

/* cp-out: warning: [^:]*: line 2, columns 35-39: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 2, columns 90-95: identifier "abort" not declared */
