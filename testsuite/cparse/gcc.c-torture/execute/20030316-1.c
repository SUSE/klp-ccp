/* PR target/9164 */
/* The comparison operand was sign extended erraneously.  */

int
main (void)
{
    long j = 0x40000000;
    if ((unsigned int) (0x40000000 + j) < 0L)
 	abort ();

    return 0;
}

/* cp-out: warning: [^:]*: line 9, column 2: identifier "abort" not declared */
