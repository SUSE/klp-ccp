#define READ_ONCE(x) (*(volatile typeof(x) *)(x))

int espr_i;

void pu_f(void)
{
	READ_ONCE(espr_i);
}
