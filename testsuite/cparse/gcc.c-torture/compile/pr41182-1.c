typedef long unsigned int size_t;
int _lae_process_opts(char *pr, char *pe)
{ 
  return (strlen ("on") < ((size_t) ((pe-&pr[2])>(strlen("on"))                
                                     ? (pe-&pr[2]) : (strlen("on")))));
}

/* cp-out: warning: [^:]*: line 4, column 10: identifier "strlen" not declared */
/* cp-out: warning: [^:]*: line 4, column 50: identifier "strlen" not declared */
/* cp-out: warning: [^:]*: line 5, column 54: identifier "strlen" not declared */
