/* Externalizable, but never accessed */
char espr_o[] = "123";

void pu_f(void)
{
	char s[sizeof(espr_o)];
}
