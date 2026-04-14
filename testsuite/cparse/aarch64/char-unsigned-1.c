/* Test that char is unsigned on aarch64. */

/* On aarch64, plain char is unsigned, so (char)0xff should be 255,
   not -1.  This means no overflow warning for values 128-255. */
char c = 200;

/* Verify that signed char is still signed. */
signed char sc = -1;
