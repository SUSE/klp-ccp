/* Test boolean conversion of an overflowing integer return value does
   not lead to an ICE.  */

_Bool f(void) { return __INT_MAX__ + 1; }

/* cp-out: warning: [^:]*: line 4, columns 23-34: integer overflow in addition */
