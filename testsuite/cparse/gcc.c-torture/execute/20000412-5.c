int main( void ) {
    struct {
	int node;
	int type;
    } lastglob[1] = { { 0   , 1  } };

    if (lastglob[0].node != 0 || lastglob[0].type != 1)
      abort ();
    exit (0);
}

/* cp-out: warning: [^:]*: line 8, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, columns 4-8: identifier "exit" not declared */
