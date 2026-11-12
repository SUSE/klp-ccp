/* Conflicting named address spaces through a typedef name. */
typedef __seg_gs int gs_int;
__seg_fs gs_int x;
/* cp-out: error: [^:]*: line 3, column 9: conflicting named address spaces */
