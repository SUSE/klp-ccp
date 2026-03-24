typedef __SIZE_TYPE__ size_t;
typedef __WCHAR_TYPE__ wchar_t;

size_t eu_o;

size_t pu_f(wchar_t w)
{
	eu_o = (size_t)w;
	return eu_o;
}
