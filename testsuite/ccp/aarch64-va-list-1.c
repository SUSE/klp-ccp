typedef __builtin_va_list va_list;

int eu_o;

int pu_f(int n, ...)
{
	va_list ap;
	__builtin_va_start(ap, n);
	eu_o = __builtin_va_arg(ap, int);
	__builtin_va_end(ap);
	return eu_o;
}
