#define read_once(x) (*(volatile typeof(x) *)(x)) + espr_i
#define READ_once(x) (*(volatile typeof(x) *)(x)) + espr_i
#define __READ_once(x) (*(volatile typeof(x) *)(x)) + espr_i

int espr_i;

void pu_f(void)
{
	read_once(espr_i);
	READ_once(espr_i);
	__READ_once(espr_i);
}
