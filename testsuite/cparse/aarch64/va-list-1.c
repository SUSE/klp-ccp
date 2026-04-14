/* Test that __builtin_va_list with AAPCS64 layout is accepted. */
void f(int n, ...)
{
	__builtin_va_list ap;
	__builtin_va_start(ap, n);
	int x = __builtin_va_arg(ap, int);
	__builtin_va_end(ap);
}
