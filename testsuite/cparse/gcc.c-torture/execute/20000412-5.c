int main( void ) {
    struct {
	int node;
	int type;
    } lastglob[1] = { { 0   , 1  } };

    if (lastglob[0].node != 0 || lastglob[0].type != 1)
      abort ();
    exit (0);
}

/* cp-out: warning: [^:]*: line 8, column 6: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, column 4: identifier "exit" not declared */
