/*
 * The toy policy prefers not to externalize function definitions from
 * includeable headers, if possible.
 */

void* ee_g(void);

void *ee_j(void)
{
	return &ee_g;
}

void *ee_i(void)
{
	return &ee_j;
}

void *ee_h(void)
{
	return &ee_i;
}

void *ee_g(void)
{
	return &ee_h;
}
