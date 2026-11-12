/*
 * The operand to __has_attribute() must be an identifier.
 */
#if __has_attribute()
#endif

int pu_f(void)
{
	return 0;
}
/* cp-out: error: [^:]*: line 4, columns 4-21: macro "__has_attribute" requires an identifier */
