/* Test that __builtin_va_list can be typedef'd and used. */
typedef __builtin_va_list va_list;

int f(int n, ...)
{
	va_list ap;
	__builtin_va_start(ap, n);
	int r = __builtin_va_arg(ap, int);
	__builtin_va_end(ap);
	return r;
}
