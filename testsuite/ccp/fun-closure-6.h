/*
 * The toy policy prefers not to externalize function definitions from
 * includeable headers, if possible.
 */

void ee_g(void);

void* ee_h(void)
{
	return &ee_g;
}

void ee_g(void)
{
	ee_h();
}
