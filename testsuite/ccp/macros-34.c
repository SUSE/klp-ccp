int espr_i;

static void g(void)
{
	espr_i = 1;
}

#define CALL_G() g()

void pu_f(void)
{
	CALL_G();
}
