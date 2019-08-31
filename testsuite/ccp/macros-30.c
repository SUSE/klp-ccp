
#define __READ_ONCE(x) (*(volatile typeof(x) *)(x))
#define READ_ONCE(x) __READ_ONCE(x)

int espr_i;

void pu_f(void)
{
	READ_ONCE(("foo", espr_i));
}
