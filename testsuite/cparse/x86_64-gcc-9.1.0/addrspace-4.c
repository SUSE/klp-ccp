/* Conflicting named address spaces in a pointer's qualifier list,
   repetition of the same one is fine. */
int * __seg_gs __seg_gs p;
int * __seg_fs __seg_gs q;
/* cp-out: error: [^:]*: line 4, column 15: conflicting named address spaces */
