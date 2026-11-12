/* A type can live in at most one named address space. */
__seg_gs __seg_fs int x;
/* cp-out: error: [^:]*: line 2, column 9: conflicting named address spaces */
