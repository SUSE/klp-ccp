/*
 * Nothing may follow the identifier operand to __has_attribute().
 */
#if __has_attribute(unused packed)
#endif

int pu_f(void)
{
	return 0;
}
/* cp-out: error: [^:]*: line 4, columns 4-34: missing '\)' after "__has_attribute" */
